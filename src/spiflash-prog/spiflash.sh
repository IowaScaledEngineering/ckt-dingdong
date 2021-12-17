#!/bin/bash

/usr/bin/python3 spi_flash_programmer_client.py -d /dev/ttyUSB0 --rate 500000 -l -1 --pad 0xff -f ../scrunchy/flash.bin write