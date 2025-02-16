## This file specifies the Make configuration for executable builds.
## See: [https://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#unusual]

include $(addprefix $(__full_build_config_dir), system_guard_clauses.mk)

_LIBSDIR := std
_DEPSDIR := .deps
_INCFLAGS := $(addprefix -I$(__top_level_dir), $(__specified_inc_dirs)) -I$(__top_level_dir) -I$(__full_src_dir)
_DEPS_FLAGS = -MT $@ -MD -MP -MF $(_DEPSDIR)/$(basename $@).d
_POSTCOMPILE = #mv -f $(_DEPSDIR)/$(basename $@).Td $(_DEPSDIR)/$(basename $@).d && touch $@

_TESTDIR := tests
_TEST_RUNNER := test_runner
_TEST_RUNNER_EXE := ./$(_TESTDIR)/$(_TEST_RUNNER).exe
_TEST_RUNNER_SRC := $(addprefix $(__full_src_dir), $(_LIBSDIR)/$(_TEST_RUNNER).c)
_ALL_TEST_FILES := $(addprefix $(__specified_test_dir), $(notdir $(wildcard $(addprefix $(__full_test_dir), *.c))))

_TEST_EXES := $(_ALL_TEST_FILES:%.c=%.test)
_ALL_C_FILES := $(notdir $(wildcard $(addprefix $(__full_src_dir), *.c)))
_LIB_C_FILES := $(subst $(__full_src_dir),, $(wildcard $(addprefix $(__full_src_dir), $(_LIBSDIR)/*.c)))
_SRC_FILTER_LIST := $(ENTRY) $(_LIBSDIR)/$(_TEST_RUNNER).c
_SRCS := $(filter-out $(_SRC_FILTER_LIST), $(_ALL_C_FILES)) $(filter-out $(_SRC_FILTER_LIST), $(_LIB_C_FILES))
_OBJECTS := $(_SRCS:%.c=%.o)
ARGS ?= 


.PHONY: build
build: $(EXECUTABLE)

.PHONY: run
run: $(EXECUTABLE)
	@$(addprefix $(__full_build_dir), $(EXECUTABLE)) $(ARGS)

.PHONY: test
test: $(_TEST_RUNNER_EXE) $(_TEST_EXES)
	@$< $(foreach _EXE, $(addprefix $(__full_build_dir), $(_TEST_EXES)), $(_EXE))

$(EXECUTABLE): $(ENTRY) $(_OBJECTS)
	$(CC) $(CFLAGS) $< $(filter-out $(<),$(^)) -o $@ $(_INCFLAGS) $(LNKFLAGS)

# _TEST_EXES compilation
$(addprefix $(__specified_test_dir), %.test): $(addprefix $(__full_test_dir), %.c) $(_OBJECTS) | $(_TESTDIR)
	$(CC) $(CFLAGS) $^ -o $(_TESTDIR)/$(@F) $(_INCFLAGS) $(LNKFLAGS)

$(_TEST_RUNNER_EXE): $(_TEST_RUNNER_SRC) | $(_TESTDIR)
	$(CC) $(CFLAGS) $(_TEST_RUNNER_SRC) -o $(_TESTDIR)/$(@F) $(_INCFLAGS) $(LNKFLAGS)

# Compile the .o file alongside a .d file, which we use to store dependency information.
$(_LIBSDIR)/%.o: $(_LIBSDIR)/%.c $(_DEPSDIR)/$(_LIBSDIR)/%.d | $(_LIBSDIR) $(_DEPSDIR)
	$(CC) $(_INCFLAGS) $(CFLAGS) $(_DEPS_FLAGS) -c $< -o $@ $(LNKFLAGS)
	$(_POSTCOMPILE)

%.o: %.c $(_DEPSDIR)/%.d | $(_DEPSDIR)
	$(CC) $(_INCFLAGS) $(CFLAGS) $(_DEPS_FLAGS) -c $< -o $@ $(LNKFLAGS)
	$(_POSTCOMPILE)

.PHONY: $(_DEPSDIR) $(_LIBSDIR) $(_TESTDIR)

$(_DEPSDIR): ; @mkdir -p $@

$(_LIBSDIR): ; @mkdir -p $@

$(_TESTDIR): ; @mkdir -p $@

_DEPSFILES := $(_SRCS:%.c=$(_DEPSDIR)/%.d) $(patsubst %.c, $(_DEPSDIR)/%.d, $(notdir $(_LIB_C_FILES)))
$(_DEPSFILES):

include $(wildcard $(_DEPSFILES))
