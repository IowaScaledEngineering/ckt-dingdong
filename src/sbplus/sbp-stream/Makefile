PROJECT_NAME=sbp-stream
ISPL_TOOLS=../../../../ispltools/tools/

PYTHON=python3

all: firmware ispl


firmware: 
	$(MAKE) -C src clean hex

ispl: firmware
	$(PYTHON) $(ISPL_TOOLS)/isplcreate/isplcreate.py -i . -o $(PROJECT_NAME).sbp
	
flash:
	$(PYTHON) $(ISPL_TOOLS)/spiflash-pyprog/flash.py $(PROJECT_NAME).sbp

