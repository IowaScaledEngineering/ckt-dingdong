PROJECT_NAME=sbp-stream
PYTHON=python3

all: firmware ispl

firmware: 
	$(MAKE) -C src clean hex

ispl: firmware
	$(PYTHON) ../tools/isplcreate/isplcreate.py -i . -o $(PROJECT_NAME).sbp
	
program: 
	$(PYTHON) ../tools/spiflash-pyprog/flash.py $(PROJECT_NAME).sbp
