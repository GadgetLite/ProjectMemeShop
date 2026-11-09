.PHONY: all lib exe wipe peek clean

all: lib exe

lib:
	@echo Making Library:
	make -C database/src/utils

exe:
	@echo Making Executable:
	make -C database/src

wipe:
	@echo Wiping Database Data Files:
	rm database/data/*.bin

peek:
	@echo Peeking into Database Data Files:
	@echo Map File:
	xxd database/data/map.bin
	@echo Header File:
	xxd database/data/header.bin
	@echo Data Pool:
	xxd database/data/dataPool.bin

clean:
	make -C database/src clean
	make -C database/src/utils clean
