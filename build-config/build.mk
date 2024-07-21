## This makefile handles depositing output objects into a segregated build directory while allowing ## compilation of the top-level makefile from outside of that directory.
## See: [https://make.mad-scientist.net/papers/multi-architecture-builds] for details.

#### Guard clauses ####

ifndef BUILDDIR

$(error No build directory specified. Make sure `BUILDDIR` is set in the top-level makefile)

endif

ifndef SRCDIR

$(error No source code directory specified. Make sure `SRCDIR` is set in the top-level makefile)

endif

#######################


__specified_build_dir := $(addsuffix /, $(patsubst %/, %, $(BUILDDIR)))
__specified_src_dir := $(addsuffix /, $(patsubst %/, %, $(SRCDIR)))
__specified_inc_dirs := $(addsuffix /, $(patsubst %/, %, $(INCDIRS)))
__root_makefile := $(abspath $(firstword $(MAKEFILE_LIST)))
__top_level_dir := $(dir $(__root_makefile))
__full_src_dir := $(addprefix $(__top_level_dir), $(__specified_src_dir))
__full_entry_path := $(addprefix $(__full_src_dir), $(ENTRY))
__full_build_dir := $(addprefix $(__top_level_dir), $(__specified_build_dir))
__full_build_config_dir := $(addprefix $(__top_level_dir), build-config/)

export # Export all defined system variables here


ifeq ($(addsuffix /, $(CURDIR)),$(__full_build_dir)) # Check if we're in the build directory

VPATH = $(__full_src_dir)
include $(addprefix $(__full_build_config_dir), build_debug.mk)

else # Currently not in any build directory

include $(addprefix $(__full_build_config_dir), target.mk)

endif
