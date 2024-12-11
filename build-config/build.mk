## This makefile handles depositing output objects into a segregated build directory while allowing 
## compilation of the top-level makefile from outside of that directory.
## See: [https://make.mad-scientist.net/papers/multi-architecture-builds] for details.

#### Guard clauses ####

ifndef BUILDDIR

$(error No build directory specified. Make sure `BUILDDIR` is set in the top-level makefile)

endif

ifndef SRCDIR

$(error No source code directory specified. Make sure `SRCDIR` is set in the top-level makefile)

endif

#######################

__full_build_config_dir := $(addprefix $(__top_level_dir), build-config/)
__specified_inc_dirs := $(addsuffix , $(patsubst %/, %, $(INCDIRS)))
__specified_lib_dirs := $(addsuffix , $(patsubst %/, %, $(SUBDIRS)))
__root_makefile := $(abspath $(firstword $(MAKEFILE_LIST)))
__top_level_dir := $(dir $(__root_makefile))

__specified_src_dir := $(addsuffix /, $(patsubst %/, %, $(SRCDIR)))
__full_src_dir := $(addprefix $(__top_level_dir), $(__specified_src_dir))
__full_entry_path := $(addprefix $(__full_src_dir), $(ENTRY))

__specified_test_dir := $(addsuffix /, $(patsubst %/, %, $(TESTDIR)))
__full_test_dir := $(addprefix $(__top_level_dir), $(__specified_test_dir))

__specified_build_dir := $(addsuffix /, $(patsubst %/, %, $(BUILDDIR)))
__full_build_dir := $(addprefix $(__top_level_dir), $(__specified_build_dir))

export # Export all defined system variables here

#######################
# Includes the help command. This should be kept as the first include to ensure that help is called by default.
include $(addprefix $(__full_build_config_dir), help.mk)
#######################

ifeq ($(addsuffix /, $(CURDIR)), $(__full_build_dir)) # Check if we're in the build directory

VPATH = $(__full_src_dir)
include $(addprefix $(__full_build_config_dir), build_exe.mk)

else # Currently not in any build directory

include $(addprefix $(__full_build_config_dir), target.mk)

endif
