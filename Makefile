## This file contains customizable configurations for the project.
##
## To see all make commands, run:
##		make help
##
## Template file structure:
##    root/
##     ├── build/                 --- Generated build location, customizable under [BUILDDIR].
##     │   ├── .deps/             --- Makefile dependency files for .o files.
##     │   ├── tests/             --- Folder containing built test executables.
##     │   │   └── foo_test.test  --- Executable test file.
##     │   ├── foo.o              --- Object files built for linking.
##     │   └── executable.exe     --- The executable, customizable under [EXECUTABLE].
##     ├── ext/                   --- External headers, customizable and extendable under [INCDIRS].
##     │   │                          These should only contain header files. If any source files are needed,
##     │   │                          put them in the [SRCDIR] directory to allow for correct linking.
##     │   └── third_party.h
##     ├── src/                   --- Source code directory. All .c files must be found directly in this folder.
##     │   │                          Customizable under [SRCDIR].
##     │   ├── main.c             --- Entry point for code, customizable under [ENTRY].
##     │   ├── foo.c              --- Internal source file. All .c files in [SRCDIR] have their .o representation 
##     │   │                          generated automatically during building.
##     │   └── foo.h
##     └── tests/                 --- Test code directory. All tests to be auto-run must be found directly in run
##         │                          this folder. Customizable under [TESTDIR].
##         └── foo_test.c         --- Test file that is auto run using `make test`
##
## Note on clangd:
##  The compile_commands.json file is best generated using `bear`. Run:
##     make clean && bear -- make build
##  to (re-)generate a compile_commands.json file.
##  Overriding the $(CC) environment variable will cause bear to fail. This needs to be fixed eventually.
##  For now, stick with the standard C compiler provided by your OS.
##
## Compiler flags:
##	CC = determines compiler used (this is automatically generated.)
##	CFLAGS = general compiler flags.
##	LNKFLAGS = general linker flags.

export EXECUTABLE := vm.exe

export ENTRY := main.c
export SRCDIR := src
export TESTDIR := tests
export BUILDDIR := build
export INCDIRS := ext

export CFLAGS := -Wall -Wextra -g
export LNKFLAGS := 

#########################
#########################

## Entry point to build configuration

BUILDCONFIG := $(addsuffix build-config/build.mk, $(dir $(abspath $(firstword $(MAKEFILE_LIST)))))
include $(BUILDCONFIG) # Hand-off to internal Makefile ./build-config/build.mk
