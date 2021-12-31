#!/usr/bin/python3
import re
import argparse
import struct
from isplasm import isplAssemble
from pydub import AudioSegment

class ISPLManifest:
  def __init__(self):
    self.audioAssets = { }
  

class AudioAssetManifest:
  def __init__(self):
    self.audioAssets = { }
    self.audioAssetIdxs = [ ]

  def add(self, asset):
    if asset.name not in self.audioAssets.keys():
      self.audioAssets[asset.name] = asset
      self.audioAssetIdxs.append(asset.name)
      return self.audioAssetIdxs.index(asset.name)

  def getAssetIdx(self, assetName):
    return self.audioAssetIdxs.index(assetName)

  def getEntrySize(self):
    return 15

  def getEntries(self):
    return len(self.audioAssetIdxs)

  def write(self, startingOffset):
    # We know we have X audio assets, each of which needs a table entry
    # Audio Asset Record Table Entry:
    # [Type:8] [Addr:32] [Size:32] [SampleRate:16] [Flags:32]
    
    # audio asset table len = 15 bytes * num_assets
    assetTableLen = 15 * len(self.audioAssetIdxs)
    
    tableBlob = bytearray()
    audioBlob = bytearray()
    
    for i in range(0,len(self.audioAssetIdxs)):
      asset = self.audioAssets[self.audioAssetIdxs[i]]
      
      typeVal = 0
      if asset.audioType == 'PCM':
        typeVal = 1

      print("Writing audio asset %d [%s] at addr=%d, len=%d, rate=%d" % (i, self.audioAssetIdxs[i], startingOffset + assetTableLen + len(audioBlob), len(asset.data), asset.rate))
      tableRec = struct.pack('>BIIHI', typeVal, startingOffset + assetTableLen + len(audioBlob), len(asset.data), asset.rate, asset.flags)
      tableBlob += tableRec
      audioBlob += asset.data
      
    return tableBlob + audioBlob
    
  
class AudioAsset:
  def __init__(self, name='', audioType='', data=None, rate=0, flags=0):
    self.name = name
    self.audioType = audioType
    self.data = data
    self.rate = rate
    self.flags = flags
    
    
  

def main():
  parser = argparse.ArgumentParser(description='Interface with an Arduino-based SPI flash programmer')
  parser.add_argument('-i', dest='input_directory', default='test',
                      help='input ISPL file')
  parser.add_argument('-o', dest='output_filename', default='flash.bin',
                      help='file to write to')

  args = parser.parse_args()
  
  pcm_asset_match = re.compile(r'AudioFile (?P<assetName>\w+) PCM (?P<sampleRate>\d+) (?P<assetFilename>.+)')
  wav_asset_match = re.compile(r'AudioFile (?P<assetName>\w+) WAV (?P<sampleRate>\d+) (?P<assetFilename>.+)')

  asset_match = re.compile(r'AudioFile (?P<assetName>\w+) PCM (?P<sampleRate>\d+) (?P<assetFilename>.+)')
  
  code_begin_match = re.compile(r'ISPLCode')
  code_end_match = re.compile(r'ISPLCodeEnd')
  
  # Compile assets
  
  isplFilename = args.input_directory + '/program.ispl'
  
  audioAssetManifest = AudioAssetManifest()
  assetDict = { }
  with open(isplFilename, 'r+') as isplFile:
    print("Assembling audio assets")

    pcmAssetRecs = filter(lambda e: e is not None, [pcm_asset_match.match(rec) for rec in isplFile.readlines()])
    isplFile.seek(0)
    wavAssetRecs = filter(lambda e: e is not None, [wav_asset_match.match(rec) for rec in isplFile.readlines()])
    
    for assetRec in pcmAssetRecs:
      try:
        with open(args.input_directory + '/' + assetRec.group('assetFilename'), 'rb') as f:
          newAsset = AudioAsset(name=assetRec.group('assetName'), 
            audioType = 'PCM',
            data = f.read(),
            rate = int(assetRec.group('sampleRate')))
          assetDict[newAsset.name] = audioAssetManifest.add(newAsset)
          
          print("- Added PCM asset [%s] - rate=%dHz, bytes=%d len=%.1fs" % (newAsset.name, newAsset.rate, len(newAsset.data), len(newAsset.data) / newAsset.rate))    
      except IOError:
        print("ERROR: Audio asset [%s] - file [%s] cannot be opened" % (assetRec.group('assetName'), assetRec.group('assetFilename')))
        raise IOError

    for assetRec in wavAssetRecs:
      try:
        wavFilename = args.input_directory + '/' + assetRec.group('assetFilename')
        wav = AudioSegment.from_wav(wavFilename)
        # Convert to 8-bit unsigned PCM at prescribed sample rate here
        sampleRate = int(assetRec.group('sampleRate'))
        wav = AudioSegment.from_wav(wavFilename).set_channels(1).set_frame_rate(sampleRate).set_sample_width(1)

        newAsset = AudioAsset(name=assetRec.group('assetName'), 
          audioType = 'PCM',
          data = wav.raw_data,
          rate = sampleRate)
        assetDict[newAsset.name] = audioAssetManifest.add(newAsset)
          
        print("- Added WAV asset [%s] - rate=%dHz, bytes=%d len=%.1fs" % (newAsset.name, newAsset.rate, len(newAsset.data), len(newAsset.data) / newAsset.rate))    
      except IOError:
        print("ERROR: Audio asset [%s] - file [%s] cannot be opened" % (assetRec.group('assetName'), assetRec.group('assetFilename')))
        raise IOError



    isplFile.seek(0)
      
    inCode = False
    code = ""
    
    for line in isplFile.readlines():
      if code_begin_match.match(line):
        inCode = True
        continue

      if code_end_match.match(line):
        inCode = False
        continue

      if inCode:
        code += line

    print("Building ISPL assembly")
    #print(code)
    
    programData, listData = isplAssemble(code, assetDict)
    print("- Program is %d bytes long" % (len(programData)))
    #print(listData)
  
  # Build asset table
  
  
  # Build manifest
  
  # Write Header
  
  with open(args.output_filename, 'wb') as outfile:
    bytesWritten = 0
    
    headerData = struct.pack('>4sI', 'ISPL'.encode('utf-8'), 1)
    outfile.write(headerData)
    bytesWritten += len(headerData)

    # Hard-wire manifest for now
    # Three records
    # 0 - Manifest itself
    # 1 - Program data
    # 2 - Audio asset table
    # *  Manifest Table Record:
    # *  [Addr:32] [Recs-N:32] [Recs-S:16]
    
    programDataWorks = bytearray([
      0x00,                # ldst                 0x00
      0x22, 0x00, 0x01,    # push 0x0001          0x22 0x00 0x01
      0x35,                # and                  0x35
      0x3A,                # lnot                 0x3A
      0x01,                # ldin                 0x01
      0x22, 0x00, 0x01,    # push 0x0001          0x22 0x00 0x01
      0x35,                # and                  0x35
      0x3B,                # land                 0x3B
      0x22, 0x00, 0x00,    # push 0               0x22 0x00 0x00
      0x91, 0x00, 0x04,    # rje 4                0x91 0x00 0x04
      0x22, 0x00, 0x00,    # push 0               0x22 0x00 0x00
      0x0A,                # play                 0x0A
      0x80, 0x00, 0x00])   # jmp 0                0x80 0x00 0x00
    
    manifestFinalSize = 10 * 3
    
    print("Starting manifest write at address %d" % (bytesWritten))
    
    manifestData = struct.pack('>IIH', bytesWritten, 3, 10)  # manifest self record
    print("Manifest Table 0 - %d %d %d" % (bytesWritten, 3, 10))
    
    manifestData += struct.pack('>IIH', bytesWritten + manifestFinalSize, len(programData), 1)
    print("Manifest Table 1 - %d %d %d" % (bytesWritten + manifestFinalSize, len(programData), 1))
    
    manifestData += struct.pack('>IIH', bytesWritten + manifestFinalSize + len(programData), audioAssetManifest.getEntries(), audioAssetManifest.getEntrySize())
    print("Manifest Table 2 - %d %d %d" % (bytesWritten + manifestFinalSize + len(programData), audioAssetManifest.getEntries(), audioAssetManifest.getEntrySize()))
    
    outfile.write(manifestData)
    bytesWritten += len(manifestData)
    
    
    print("Starting program write at address %d" % (bytesWritten))
    outfile.write(programData)
    bytesWritten += len(programData)
    
    print("Starting audio asset write at address %d" % (bytesWritten))
    outfile.write(audioAssetManifest.write(bytesWritten))
 
def hexDump(byteArray):
  for i in range(0, len(byteArray)):
    if 0 == i%16:
      print("");
    print("%02X " % byteArray[i], end='')

if __name__ == '__main__':
  main()

