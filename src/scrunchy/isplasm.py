#!/usr/bin/python3
import ply.lex as lex
import ply.yacc as yacc
import struct
import sys

opcodes = {
    'ldst'      : { 'op':0x00, 'args':[] },
    'ldin'      : { 'op':0x01, 'args':[] },
    'rand'      : { 'op':0x02, 'args':['number','number'] },
    'ldcip'     : { 'op':0x03, 'args':[] },
    'ldsp'      : { 'op':0x04, 'args':[] },
    'ldtime'    : { 'op':0x05, 'args':[] },
    'play'      : { 'op':0x0A, 'args':[] },
    'stop'      : { 'op':0x0B, 'args':[] },
    'drop'      : { 'op':0x20, 'args':[] },
    'dup'       : { 'op':0x21, 'args':[] },
    'push'      : { 'op':0x22, 'args':['number'] },
    'swap'      : { 'op':0x23, 'args':[] },
    'rot'       : { 'op':0x24, 'args':[] },
    'over'      : { 'op':0x25, 'args':[] },
    'load'      : { 'op':0x26, 'args':[] },
    'loadn'     : { 'op':0x27, 'args':['number'] },
    'stor'      : { 'op':0x28, 'args':[] },
    'storn'     : { 'op':0x29, 'args':['number'] },
    'rotn'      : { 'op':0x2A, 'args':['number'] },
    'add'       : { 'op':0x30, 'args':[] },
    'sub'       : { 'op':0x31, 'args':[] },
    'mul'       : { 'op':0x32, 'args':[] },
    'div'       : { 'op':0x33, 'args':[] },
    'mod'       : { 'op':0x34, 'args':[] },
    'and'       : { 'op':0x35, 'args':[] },
    'or'        : { 'op':0x36, 'args':[] },
    'xor'       : { 'op':0x37, 'args':[] },
    'not'       : { 'op':0x38, 'args':[] },
    'notl'      : { 'op':0x3A, 'args':[] },
    'andl'      : { 'op':0x3B, 'args':[] },
    'call'      : { 'op':0x70, 'args':['addr'] },
    'ret'       : { 'op':0x71, 'args':[] },
    'jmp'       : { 'op':0x80, 'args':['addr'] },
    'jeq'       : { 'op':0x81, 'args':['addr'] },
    'jne'       : { 'op':0x82, 'args':['addr'] },
    'jlt'       : { 'op':0x83, 'args':['addr'] },
    'jlte'      : { 'op':0x84, 'args':['addr'] },
    'jgt'       : { 'op':0x85, 'args':['addr'] },
    'jgte'      : { 'op':0x86, 'args':['addr'] },
    'jz'        : { 'op':0x90, 'args':['raddr'] },
    'jnz'       : { 'op':0x91, 'args':['raddr'] },
    'nop'       : { 'op':0xFF, 'args':[] },
    }


class ISPLASMLexer(object):

  opcodelist = tuple(opcodes.keys())

  tokens = opcodelist + (
    'NEWLINE',
    'HEXNUMBER',
    'NUMBER',
    'OPCODE',
    'VAR',
    'CNST',
    'LABEL',
    'ID'
  )

  t_ignore_COMMENT = r'\#.*|;.*'

  def t_newline(self, t):
    r'\n+'
    t.lexer.lineno += len(t.value)

  t_ignore = ' \t'

  def t_CNST(self, t):
    r'const'
    return t

  def t_VAR(self, t):
    r'var'
    return t


  # Error handling rule
  def t_error(self, t):
    print("Illegal character '%s'" % t.value[0])
    t.lexer.skip(1)

  def t_LABEL(self, t):
    r'[A-Za-z][A-Za-z0-9_]*:'
    if t.value in opcodes.keys():
      print("Label cannot be keyword [%s]" % (t.value))
      t.lexer.skip(1)
    return t

  def t_ID(self, t):
    r'[A-Za-z][A-Za-z0-9_]*'
    if t.value in opcodes.keys():
        t.type = t.value
    return t

  def t_HEXNUMBER(self, t):
    r'0[xX][A-Fa-f0-9]+'
    try:
      originaVal = t.value
      t.value = int(t.value, 16)    
      t.type = 'NUMBER'
    except ValueError:
      print("Invalid number [%s] on line %d" % (originaVal, t.lexer.lineno))
      t.value = 0
    return t

  def t_NUMBER(self, t):
    r'(-\d+ | \d+)'
    try:
      t.value = int(t.value)    
    except ValueError:
      print("number too large [%d]" % (t.value))
      t.value = 0
    return t

  def build(self,**kwargs):
    self.lexer = lex.lex(module=self, **kwargs)

  # Test it output
  def test(self,data):
    self.lexer.input(data)
    while True:
      tok = self.lexer.token()
      if not tok: 
        break
      print(tok)

  def getTokens(self):
    return self.tokens

class ISPLOpCode:
  def __init__(self, op='nop', line=0, args=[]):
    self.op = op
    self.arguments = args
    self.numBytes = 1 + 2* len(opcodes[self.op]['args'])
    self.line = line
    #print("[%s] line %d - args %s" % (self.op, self.line, self.arguments))

  def getLen(self):
    return self.numBytes
  
  def resolveReferences(self, refs):
    for i in range(0,len(self.arguments)):
      if self.arguments[i]['type'] == 'ref':
        if self.arguments[i]['value'] not in refs.keys():
          print("Line %d: ERROR opcode [%s] unable to resolve label [%s]" % (self.line, self.op, self.arguments[i]['value']))
          raise ValueError
        else:
          self.arguments[i]['type'] = 'u16'
          self.arguments[i]['value'] = refs[self.arguments[i]['value']]
        

  def listDumpGet(self):
    argstr = ""
    for arg in self.arguments:
      argstr += "0x%02x " % (arg['value'])
    return "%s %s" % (self.op, argstr)
    
  
  def byteCodeGet(self, debug=False):
    if debug:
      argstr = ""
      for arg in self.arguments:
        argstr += "0x%02x " % (arg['value'])
      print("%s %s" % (self.op, argstr))
    retval = struct.pack(">B", opcodes[self.op]['op'])
    
    if len(self.arguments) != len(opcodes[self.op]['args']):
      print("Line %d: ERROR opcode [%s] needs %d args, has %d" % (self.line, self.op, len(opcodes[self.op]['args']), len(self.arguments)))

    for arg in self.arguments:
      #print("opcode %s line %d arg is type %s val %s" % (self.op, self.line, arg['type'], arg['value']))
      
      if arg['type'] not in ('u16', 's16'):
        raise ValueError("Line %d: Opcode [%s] is not fully resolved" % (self.line, self.op))
      
      if arg['type'] == 'u16':
        retval += struct.pack(">H", arg['value'])
      elif arg['type'] == 's16':
        retval += struct.pack(">h", arg['value'])
    return retval

def getCurrentByteOffset(ops):
  addr = 0
  for op in ops:
    #print("op [%s] bytes [%d]" % (op.op, op.numBytes))
    addr += op.getLen()
  return addr

def main(inFileName=None, outfile=None):
  with open(inFileName, "r+") as infile:
    data = infile.read()
    bytecode, listdump = isplAssemble(data, { })
    print("Bytecode len %d" % (len(bytecode)))
    
def isplAssemble(data, aarDict={}, debug=False):
  l = ISPLASMLexer()
  l.build()
  tokens = l.getTokens()
  
  refs = aarDict  # AARs become constant references
  addrrefs = { } # Holds address references, since we need to offset these for variables later
  ops = [ ]  # Holds opcode objects
  variables = { }  # Holds information about variables
  
  l.lexer.input(data)

  while True:
    tok = l.lexer.token()
    if not tok: 
      break
    if debug:
      print(tok)

    if tok.type in opcodes.keys():
      if 0 == opcodes[tok.type]['args']:
        # Has no arguments, just encode it
        ops.append(ISPLOpCode(tok.type, line=tok.lineno))
      else:
        # Opcode has arguments
        arglist = [ ]

        for argtype in opcodes[tok.type]['args']:
          argtok = l.lexer.token()

          if debug:
            print(" [%d] " % len(arglist) + "%s" % argtok)

          if argtype in ('number', 'addr'):
            if argtok.type in ('NUMBER'):
              arglist.append({'type':'u16', 'value':argtok.value})
            elif argtok.type in ('ID'):
              arglist.append({'type':'ref', 'value':argtok.value})
            else:
              print("Line %d: ERROR opcode [%s] argument [%d] takes a number or reference, not '%s'" % (tok.lineno, tok.type, len(arglist), argtok.type))
              raise ValueError

          elif argtype in ('raddr'):
            if argtok.type in ('NUMBER'):
              arglist.append({'type':'u16', 'value':argtok.value})
            elif argtok.type in ('ID'):
              arglist.append({'type':'ref', 'value':argtok.value})
            else:
              print("Line %d: ERROR opcode [%s] argument [%d] takes a number or reference, not '%s'" % (tok.lineno, tok.type, len(arglist), argtok.type))
              raise ValueError

          elif argtype in ('audioasset'):
            if argtok.type in ('NUMBER'):
              arglist.append({'type':'u16', 'value':argtok.value})
            elif argtok.type in ('ID'):
              if argtok.value not in aarDict.keys():
                print("Line %d: ERROR play opcode audio asset [%s] does not exist" % (tok.lineno, argtok.value))
                raise ValueError
              arglist.append({'type':'u16', 'value':aarDict[argtok.value]})

          else:
            print("ERROR: Don't know how to handle argtype [%s]" % argtype)

        op = ISPLOpCode(tok.type, line=tok.lineno, args=arglist)
        ops.append(op)

       
    elif tok.type == 'LABEL':
      refname = tok.value[:-1]
      if refname in refs.keys() or refname in addrrefs.keys():
        print("Line %d: ERROR Label [%s] is already defined" % (refname))
        raise ValueError
      addrrefs[refname] = getCurrentByteOffset(ops)

      #print("Label [%s] at offset [%d]" % (refname, refs[refname]))

    elif tok.type == 'VAR':
      # Variables - these live top of stack
      argtok = l.lexer.token()
      if argtok.type not in ('ID'):
        print("Line %d: Invalid variable name [%s]" % (tok.lineno, argtok.value))
        raise ValueError
      if argtok.value in refs.keys():
        print("Line %d: ERROR symbol name [%s] is already defined" % (argtok.value))
        raise ValueError
      
      variables[argtok.value] = {'stack_pos':len(variables), 'line':tok.lineno}
      refs[argtok.value] = variables[argtok.value]['stack_pos']

    elif tok.type == 'CNST':
      # Constant - just add to reference list
      argtok_name = l.lexer.token()
      if argtok_name.type not in ('ID'):
        print("Line %d: Invalid constant name [%s]" % (tok.lineno, argtok_name.value))
        raise ValueError
      if argtok_name.value in refs.keys():
        print("Line %d: ERROR Label [%s] is already defined" % (argtok_name.value))
        raise ValueError

      argtok_value = l.lexer.token()
      if argtok_value.type not in ('NUMBER'):
        print("Line %d: Constant [%s] must be a number" % (tok.lineno, argtok_name.value))
        raise ValueError

      refs[argtok_name.value] = argtok_value.value

    else:
      print("Line %d: Unexpected token [%s] of type [%s]" % (tok.lineno, tok.type, tok.value))

  # Variables create offsets because they're pushed on at the beginning before user code
  var_ops = [ ]
  for name in variables.keys():
    var = variables[name]
    op = ISPLOpCode('push', line=var['line'], args=[{'type':'u16', 'value':0}])
    var_ops.append(op)

  offset = getCurrentByteOffset(var_ops)

  for ref in addrrefs.keys():
    refs[ref] = addrrefs[ref] + offset
  
  ops = var_ops + ops
  
  for op in ops:
    op.resolveReferences(refs)

  byteCode = bytes()
  listDump = "\n"
  
  for ref in refs.keys():
    listDump += "const %s %d\n" % (ref, refs[ref])
  listDump +="\n"
  
  for op in ops:
    if listDump != None:
      listDump += "0x%04X  %s\n" % (len(byteCode), op.listDumpGet()) 

    byteCode += op.byteCodeGet(debug=debug)
  
  return byteCode, listDump

if __name__ == '__main__':
  main(inFileName = sys.argv[1])


