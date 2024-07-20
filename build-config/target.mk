## Information here is taken from [https://make.mad-scientist.net/papers/multi-architecture-builds/#target.mk]

include $(addprefix $(__full_build_config_dir), system_guard_clauses.mk)

# The sub-make routine. Runs the root Makefile and specifies the source directory while 
# assuming that we have jumped into the object directory.
_MAKE_TARGET = $(MAKE) --no-print-directory -C $@ -f $(__root_makefile) $(MAKECMDGOALS)

# Forces most built-in rules to be removed. 
.SUFFIXES:

# Relocates to the object directory and runs the sub-make routine.
.PHONY: $(__full_build_dir)
$(__full_build_dir):
	+@[ -d $@ ] || mkdir -p $@ # Jump to the object directory, or create it if it doesn't exist.
	+@$(_MAKE_TARGET)				 # Run the sub-make routine.

# Prevent Make from rebuilding Makefiles or .mk files, 
# invoking itself recursively in the object directory.
Makefile : ;
%.mk : ;

# Matches any input given, ignoring it and instead running the sub-make routine that navigates
# into the object directory.
% :: $(__full_build_dir) ; :

# Special rule for clean to prevent recursing into the object directory when cleaning, instead
# just deleting the build folder.
.PHONY: clean
clean:
	-rm -rf $(__full_build_dir)

