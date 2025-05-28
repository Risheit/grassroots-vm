/// --- Seed ---
///
/// A C-style build system for GVM.
/// Inspired by https://github.com/tsoding/nob.h
///
/// The goals of Seed:
///  - Require an opinionated file structure.
///      Allowing any type of file structure is unnecessary.
///      Enforced and opinionated file structures help keep code more readable
///      as a codebase grows.
///  - You should very rarely need to touch build code.
///      An issue of C build systems and their unopinionated way of handling
///      files is that build code often needs to be edited to add/remove a new
///      source or header file.
///      We shouldn't need to update build code unless we're adding a new
///      independent module/executable or library.
///  - Configuration should be centralized and easy.
///      Configuration of the build system should all be kept to a single
///      function. We shouldn't have to go through files to add a new module to
///      the codebase.
///
/// --- Usage ---
///
/// Define [SEED_IMPLEMENTATION] in one C file before including the header file
/// to define the implementation code of the build tooling.
///
/// Seed's primitives are "modules", a term for an independently compiled bucket
/// of code. In general, take a module to be either an executable or library,
/// but it can stand of anything that compiles into a single binary file.
///
/// A module is created using the [sd_mod_create] command. Each module
/// corresponds to a directory within your codebase that contains header and
/// source files.
///
/// Modules can have dependencies on other modules (e.x. An executable depends
/// on code from a library). Dependencies can be added using the
/// [sd_mod_add_dep] command. A module's dependencies must be found within a
/// subdirectory inside the module.
///
/// Top level modules are marked using [sd_mod_set_root]. Shared modules are
/// marked using [sd_mod_set_shared], all shared modules must be top level
/// modules.
///
/// If a module is depended on by more than one module, it can't reasonably be
/// put in any one module's subdirectory. Modules like these are called "shared
/// modules", and must be a directory found at root level.
/// A module can add a dependency on a shared module same as any other
/// dependency. Be careful with this! Shared modules should be used sparingly,
/// generally for utility libraries used by several other modules. They exist as
/// an escape from the standard module hierarchy, and having too many can get
/// messy. Shared modules should be present in the top level root directory,
/// containing the seed.h file.
///
/// As an example, an application with five modules:
///   src/
///   ├─ lists/ (shared library -- used in all other modules)
///   │  ├─ lists.c
///   │  └─ lists.h
///   ├─ text-renderer/ (top-level library module)
///   │  ├─ unicode-handler/ (dependency library -- used by text-renderer.c)
///   │  │  ├─ unicode.c
///   │  │  └─ unicode.h
///   │  └─ text-renderer.c
///   ├─ renderer-gui/ (top-level executable module)
///   │  ├─ graphics/ (dependency library -- used by main.c)
///   │  │  ├─ renderer.c
///   │  │  └─ renderer.h
///   │  └─ main.c
///   ├─ seed.c (build tooling)
///   └─ seed.h (build tooling)

#ifndef __SEED_H
#define __SEED_H

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__unix__)
#include <sys/stat.h>
#elif defined(__APPLE__)
#include <sys/stat.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

/// The maximum number of modules within a seed program. If it hasn't been
/// defined, define a default.
#ifndef SEED_MAX_MODULES
#define SEED_MAX_MODULES 32
#endif // SEED_MAX_MODULES

/// The maximum number of flags that can be passed to the compiler when
/// compiling a module. If it hasn't been defined, define a default.
#ifndef SEED_MAX_FLAGS
#define SEED_MAX_FLAGS 64
#endif // SEED_MAX_FLAGS

/// Seed module flags.
typedef enum sd_mod_flags {
  SD_SHARED = 1 << 0, // Module is a shared dependency for other modules.
  SD_ROOT = 1 << 1,   // Module is top-level.
} sd_mod_flags;

typedef struct sd_list sd_list;

/// One independently compilable object in our build system.
typedef struct sd_module {
  const char *name;             /// Module name.
  const char *command;          /// Command run when compiling this module.
  sd_mod_flags mflags;          /// Module flags.
  char *cflags[SEED_MAX_FLAGS]; /// Flags passed into compiler.
  sd_list *deps;                /// Dependency list.
} sd_module;

/// Linked list node of modules
struct sd_list {
  sd_module *module;
  struct sd_list *next;
};

/// Global structure that holds all created modules.
typedef struct sd_core {
  sd_module mods[SEED_MAX_MODULES]; /// An array of all modules. We allocate a
                                    /// static pool of memory for modules to be
                                    /// created from, so that we can avoid some
                                    /// more complex memory management.
  int32_t amt;                      /// The amount of modules.
  sd_list *roots;                   /// A list of top-level modules.
  const char *root_path;            /// Absolute path to root directory
                                    /// containing seed.h.
} sd_core;

/// Creates a new module. The name should be the identical to the directory name
/// of the module in the file path.
sd_module *sd_mod_create(const char *name);

/// Adds [dependency] as a dependency of [module].
void sd_mod_add_dep(sd_module *restrict module, sd_module *restrict dependency);

/// Sets the module [shared] as a shared module. It should be located in the
/// root directory that contains seed.h.
void sd_mod_set_shared(sd_module *shared);

/// Sets the module [root] as a top-level module. It should be located in the
/// root directory that contains seed.h.
void sd_mod_set_root(sd_module *root);

/// Compiles the module [module].
void sd_compile_mod(sd_module *module);

/// Compiles all modules.
void sd_compile(void);

/// Initializes the build system.
void sd_init(void);

/// Cleans up allocated memory.
void sd_finish(void);

#endif // __SEED_H

#define SEED_IMPLEMENTATION // TODO: Remove
#ifdef SEED_IMPLEMENTATION

static sd_core core = {0};

static inline sd_list *push_front(sd_list *restrict node,
                                  sd_list *restrict list) {
  node->next = list;
  return node;
}

/// Prints an error to standard error and exits. Formatted identically to
/// printf.
_Noreturn static inline void sd_error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);

  sd_finish();
  exit(1);
}

/// Attempts to zero-allocate [amt] * [size] bytes and fails via error if it
/// can't.
static inline void *sd_try_alloc(size_t amt, size_t size) {
  void *alloc = calloc(amt, size);
  if (alloc == NULL)
    sd_error("Ran out of heap memory!\n");

  return alloc;
}

sd_module *sd_mod_create(const char *name) {
  if (core.amt > SEED_MAX_MODULES)
    sd_error("You're trying to create more than the allowed number of "
             "modules! Set SEED_MAX_MODULES to be some higher value.\n");

  sd_module *mod = &core.mods[core.amt];
  core.amt++;

  mod->name = name;
  mod->command = "cc";
  // Other fields are already default zeroed, so we don't do it manually here.

  return mod;
}

void sd_mod_add_dep(sd_module *restrict module,
                    sd_module *restrict dependency) {
  sd_list *mod_node = sd_try_alloc(1, sizeof *mod_node);
  mod_node->module = dependency;
  push_front(mod_node, module->deps);
}

inline void sd_mod_set_shared(sd_module *shared) {
  shared->mflags |= SD_SHARED;
}

void sd_mod_set_root(sd_module *root) {
  root->mflags |= SD_ROOT;
  sd_list *mod_node = sd_try_alloc(1, sizeof *mod_node);
  mod_node->module = root;
  push_front(mod_node, core.roots);
}

/// Returns the absolute path to the root directory containing seed.h.
/// If dst is not NULL, it is set to contain the path as well.
static const char *sd_get_root_path(char *dst) {
  const char *path;
#if defined(__unix__)
  path = realpath(".", dst);
#elif defined(__APPLE__)
  path = realpath(".", dst);
#elif defined(_WIN32)
  sd_error("Finding the current top level path not yet supported for Windows.");
#endif

  if (path == NULL)
    sd_error("Unable to find the directory containing seed.h: %s.\n",
             strerror(errno));

  return path;
}

/// Creates the directory [name]. Does nothing if it exists.
static void sd_create_dir_if_not_exists(const char *name) {
  char path[PATH_MAX];
  int32_t err = 0;

  // Append the directory name to the path.
  sd_get_root_path(path);
  strlcat(path, "/", PATH_MAX);
  strlcat(path, name, PATH_MAX);

#if defined(__unix__)
  err = mkdir(path, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
#elif defined(__APPLE__)
  err = mkdir(path, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
#elif defined(_WIN32)
  sd_error("Creating directories not yet supported for Windows.");
#endif

  if (err < 0 && errno != EEXIST)
    sd_error("Failed to create the directory %s.: %s.\n", name,
             strerror(errno));
}

void sd_compile_mod(sd_module *module) {
  sd_create_dir_if_not_exists("build");

  // Compile all dependencies
  sd_list *cur = module->deps;
  while (cur != NULL) {
    sd_compile_mod(cur->module);
    cur = cur->next;
  }

  // Compile module itself
  // We depend on all files inside 
}

void sd_compile(void) { sd_create_dir_if_not_exists("build"); }

void sd_init(void) { core.root_path = sd_get_root_path(NULL); }

void sd_finish(void) {
  sd_list *cur, *next;

  // Clean up free list
  cur = core.roots;
  while (cur != NULL) {
    next = cur->next;
    free(cur);
    cur = next;
  }

  // Clean up dependency lists
  for (int32_t i = 0; i < core.amt; i++) {
    cur = core.mods[i].deps;
    while (cur != NULL) {
      next = cur->next;
      free(cur);
      cur = next;
    }
  }
}

#endif // SEED_IMPLEMENTATION
