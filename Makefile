## This file contains customizable configurations for the project.
##
## To see all make commands, run:
##		make help
##
## Template file structure:
##    root/
##     ├── build/                 --- Generated build location, customizable under [BUILDDIR].
##     │   ├── .deps/             --- Makefile dependency files for .o files.
##     │   ├── foo.o              --- Object files built for linking.
##     │   └── executable.exe     --- The executable, customizable under [EXECUTABLE].
##     ├── ext/                   --- External headers, customizable and extendable under [INCDIRS].
##     │   └── third_party.h
##     └── src/                   --- Source code directory. All .c files must be found directly in this folder.
##         │                          Customizable under [SRCDIR].
##         ├── main.c             --- Entry point for code, customizable under [ENTRY].
##         ├── foo.c              --- Internal source file. All .c files in [SRCDIR] have their .o representation 
##         │                          generated automatically during building.
##         └── foo.h
##
## Compiler flags:
##	CC = determines compiler used
##	CFLAGS = general compiler flags.
##	LNKFLAGS = general linker flags.

export EXECUTABLE := vm.exe

export ENTRY := main.c
export SRCDIR := src
export BUILDDIR := build
export INCDIRS := ext

export CC := gcc-12 
export CFLAGS := -Wall -Wextra -g
export LNKFLAGS := 

#########################
#########################

## Entry point to build configuration

BUILDCONFIG := $(addsuffix build-config/build.mk, $(dir $(abspath $(firstword $(MAKEFILE_LIST)))))
include $(BUILDCONFIG) # Hand-off to internal Makefile ./build-config/build.mk
