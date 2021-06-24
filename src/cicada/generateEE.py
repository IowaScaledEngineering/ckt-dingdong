from intelhex import IntelHex
import random

ih = IntelHex()

for i in range(0,256):
#  ih[2*i] = i
#  ih[2*i+1] = 10
  ih[2*i] = random.randint(0,255)
  ih[2*i+1] = random.randint(0,255)

  # Make initial delay 1 sec on all units
  ih[1] = 10

ih.write_hex_file("cicada.eep")
