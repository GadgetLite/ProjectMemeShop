.PHONY: all clean frontend backend lib debug

all: lib frontend backend

lib:
	@echo "\n\tBuilding Backend Library:"
	$(MAKE) -C backend/utils
	@echo "\n\tBuilding Frontend Library:"
	$(MAKE) -C frontend/utils

frontend:
	@echo "\n\tBuilding Frontend (svrWeb):"
	$(MAKE) -C frontend

backend:
	@echo "\n\tBuilding Backend (svrBackend):"
	$(MAKE) -C backend/database

debug:
	find . -name "*.cpp" -exec g++ -I ./backend/include -fsyntax-only -Wall -Wextra {} +

clean:
	$(MAKE) -C backend/utils clean
	$(MAKE) -C backend/database clean
	$(MAKE) -C frontend/utils clean
	$(MAKE) -C frontend clean
