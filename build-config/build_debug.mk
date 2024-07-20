## This file specifies the Make configuration for debug builds.
## See: [https://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#unusual]

include $(addprefix $(__full_build_config_dir), system_guard_clauses.mk)

_DEPSDIR := .deps
_INCFLAGS := $(addprefix -I/, $(__specified_inc_dirs))
_DEPS_FLAGS = -MT $@ -MD -MP -MF $(_DEPSDIR)/$*.Td
_POSTCOMPILE = @mv -f $(_DEPSDIR)/$*.Td $(_DEPSDIR)/$*.d && touch $@

_ALL_C_FILES := $(notdir $(wildcard $(addprefix $(__full_src_dir), *.c)))
_SRCS := $(filter-out $(ENTRY), $(_ALL_C_FILES))
_OBJECTS := $(_SRCS:%.c=%.o)

# Run full compilation.
# This should remain the first rule, since we want this to be the default rule.
.PHONY: run
run: $(EXECUTABLE)

$(EXECUTABLE): $(ENTRY) $(_OBJECTS)
	$(CC) $(_INCFLAGS) $(CFLAGS) $^ -o $@ $(LNKFLAGS)

# Compile the .o file alongside a .d file, which we use to store dependency information.
%.o: %.c
%.o: %.c $(_DEPSDIR)/%.d | $(_DEPSDIR)
	$(CC) $(_INCFLAGS) $(CFLAGS) $(_DEPS_FLAGS) -c $< -o $@ $(LNKFLAGS)
	$(_POSTCOMPILE)

$(_DEPSDIR): ; @mkdir -p $@

_DEPSFILES := $(_SRCS:%.c=$(_DEPSDIR)/%.d)
$(_DEPSFILES):

include $(wildcard $(_DEPSFILES))

