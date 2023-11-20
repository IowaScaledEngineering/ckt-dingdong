PROJECT_NAME=sbp-outhouse
PYTHON=python3

all: firmware ispl


firmware: 
	$(MAKE) -C src clean hex

ispl: firmware
	$(PYTHON) ../tools/isplcreate/isplcreate.py -i . -o sbp-outhouse.sbp
	
flash: 
	$(PYTHON) ../tools/spiflash-pyprog/flash.py sbp-outhouse.sbp
