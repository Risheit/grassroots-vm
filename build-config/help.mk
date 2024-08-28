## Makefile defining the [help] make command.

include $(addprefix $(__full_build_config_dir), system_guard_clauses.mk)

.PHONY: help
help:
	@echo "Available commands to build and run C and C++ programs. See the root MAKEFILE to configure different details."
	@echo "build: Compiles the source and header files into a single executable."
	@echo "run: Runs the built executable. If any changes have been made to the source code, this will rebuild the executable as well."
	@echo "test: Builds and runs all project tests."
	@echo "help: Shows this message."
