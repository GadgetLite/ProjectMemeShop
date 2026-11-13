.PHONY: all clean processing lib host

all: lib processing 

lib:
	@echo "\n\tBuilding Frontend Library:"
	$(MAKE) -C processing/src/utils

processing:
	@echo "\n\tBuilding Processing Executable (svrWeb):"
	$(MAKE) -C processing

host: all
	@echo "\n\tHosting Web Server:"
	cd processing/exe && (nohup ./svrWeb </dev/null >svrWeb.log 2>&1 & echo $$! > svrWeb.pid)
	multitail processing/exe/svrWeb.log

stop:
	@if [ -f processing/exe/svrWeb.pid ]; then \
		kill -9 `cat processing/exe/svrWeb.pid` 2>/dev/null || true; \
		rm -f processing/exe/svrWeb.pid; \
		rm -f processing/exe/svrWeb.log; \
		echo "Server stopped successfully."; \
	else \
		echo "PID file not found. Forcing cleanup of svrWeb..."; \
		killall -9 svrWeb 2>/dev/null || echo "No running svrWeb process found."; \
	fi

clean:
	$(MAKE) -C processing/src/utils clean
	$(MAKE) -C processing clean
