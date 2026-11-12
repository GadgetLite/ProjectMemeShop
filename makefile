.PHONY: all clean processing lib 

all: lib processing 

lib:
	@echo "\n\tBuilding Frontend Library:"
	$(MAKE) -C processing/src/utils

processing:
	@echo "\n\tBuilding Processing Executable (svrWeb):"
	$(MAKE) -C processing

clean:
	$(MAKE) -C processing/src/utils clean
	$(MAKE) -C processing clean
