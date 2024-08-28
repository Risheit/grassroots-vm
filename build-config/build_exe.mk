## This file specifies the Make configuration for executable builds.
## See: [https://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#unusual]

include $(addprefix $(__full_build_config_dir), system_guard_clauses.mk)

_DEPSDIR := .deps
_INCFLAGS := $(addprefix -I$(__top_level_dir), $(__specified_inc_dirs)) -I$(__top_level_dir)
_DEPS_FLAGS = -MT $@ -MD -MP -MF $(_DEPSDIR)/$*.Td
_POSTCOMPILE = @mv -f $(_DEPSDIR)/$*.Td $(_DEPSDIR)/$*.d && touch $@

_ALL_C_FILES := $(notdir $(wildcard $(addprefix $(__full_src_dir), *.c)))
_SRCS := $(filter-out $(ENTRY), $(_ALL_C_FILES))
_OBJECTS := $(_SRCS:%.c=%.o)

_TESTDIR := tests
_ALL_TEST_FILES := $(addprefix $(__specified_test_dir), $(notdir $(wildcard $(addprefix $(__full_test_dir), *.c))))
_TEST_EXES := $(_ALL_TEST_FILES:%.c=%.test)

.PHONY: build
build: $(EXECUTABLE)

.PHONY: run
run: $(EXECUTABLE)
	@$(addprefix $(__full_build_dir), $(EXECUTABLE))

.PHONY: test
test: $(_TEST_EXES)
	@$(foreach _EXE, $(addprefix $(__full_build_dir), $(_TEST_EXES)), $(_EXE);)

$(EXECUTABLE): $(ENTRY) $(_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(_INCFLAGS) $(LNKFLAGS)

$(addprefix $(__specified_test_dir), %.test): $(addprefix $(__full_test_dir), %.c) $(_OBJECTS) | $(_TESTDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(_INCFLAGS) $(LNKFLAGS)

# Compile the .o file alongside a .d file, which we use to store dependency information.
%.o: %.c
%.o: %.c $(_DEPSDIR)/%.d | $(_DEPSDIR)
	$(CC) $(_INCFLAGS) $(CFLAGS) $(_DEPS_FLAGS) -c $< -o $@ $(LNKFLAGS)
	$(_POSTCOMPILE)

$(_DEPSDIR): ; @mkdir -p $@

$(_TESTDIR): ; @mkdir -p $@

_DEPSFILES := $(_SRCS:%.c=$(_DEPSDIR)/%.d)
$(_DEPSFILES):

include $(wildcard $(_DEPSFILES))
