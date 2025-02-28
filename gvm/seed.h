/**
 * --- Seed ---
 *
 * A C-style build system for GVM.
 * Inspired by https://github.com/tsoding/nob.h
 *
 * The goals of Seed:
 *  - Configuration should not require more than a single command.
 *      Since the build system is written in C, it also needs to be compiled.
 *      This should not ever take more than a single command.
 *  - Require an opinionated file structure.
 *      Allowing any type of file structure is unnecessary.
 *      Enforced and opinionated file structures help keep code more readable as
 *      a codebase grows.
 *  - You should very rarely need to touch build code.
 *      An issue of C build systems and their unopinionated way of handling
 *      files is that build code often needs to be edited to add/remove a new
 *      source or header file.
 *      We shouldn't need to update build code unless we're adding a new
 *      independent module/executable or library.
 *  - Configuration should be centralized and easy.
 *      Configuration of the build system should all be kept to a single
 *      function. We shouldn't have to go through files to add a new module to
 *      the codebase.
 *
 * --- Usage ---
 *
 * Define [SEED_IMPLEMENTATION] in one C file before including the header file
 * to define the implementation code of the build tooling.
 *
 * Seed's primitives are "modules", a term for an independently compiled bucket
 * of code. In general, take a module to be either an executable or library, but
 * it can stand of anything that compiles into a single binary file.
 *
 * The root module is created using [sd_root_create] command. Only one root
 * module can exist, and it is the starting directory of the application.
 * All modules, even shared ones are dependencies of the root module, and exist
 * as subdirectories of it.
 *
 * A module is created using the [sd_module_create] command. Each module
 * corresponds to a directory within your codebase that contains header and
 * source files.
 *
 * Modules can have dependencies on other modules (e.x. An executable depends on
 * code from a library). Dependencies can be added using the [sd_module_add_dep]
 * command.
 * A module's dependencies must be found within a subdirectory inside the
 * module.
 *
 * If a module is depended on by more than one module, it can't reasonably be
 * put in any one module's subdirectory. Modules like these are called "shared
 * modules", and must be a directory found at root level.
 * A module can add a dependency on a shared module using the
 * [sd_module_add_shared] command.
 * Be careful with this! Shared modules should be used sparingly, generally for
 * utility libraries used by several other modules. They exist as an escape from
 * the standard module hierarchy, and having too many can get messy.
 *
 * As an example, an application with five modules:
 *    root/
 *    ├─ game/ (executable)
 *    │  ├─ lists/ (shared library -- used in all other modules)
 *    │  │  ├─ lists.c
 *    │  │  └─ lists.h
 *    │  ├─ scripts/ (dependency library -- used by main.c)
 *    │  │  ├─ io-handler/ (dependency library -- used by scripts)
 *    │  │  │  ├─ io.c
 *    │  │  │  └─ io.h
 *    │  │  ├─ handler.c
 *    │  │  └─ handler.h
 *    │  ├─ graphics/ (dependency library -- used by main.c)
 *    │  │  ├─ renderer.c
 *    │  │  └─ renderer.h
 *    │  └─ main.c
 *    ├─ seed.c (build tooling)
 *    └─ seed.h (build tooling)
 */

#ifndef __SEED_H
#define __SEED_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(__unix__)
#include <sys/stat.h>
#elif defined(__APPLE__)
#include <sys/stat.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

// __unused marks a parameter as unused. If it hasn't been defined by our
// compiler, ignore the macro.
#ifndef __unused
#define __unused
#endif

// The maximum number of modules within a seed program. If it hasn't been
// defined, define a default.
#ifndef SEED_MAX_MODULES
#define SEED_MAX_MODULES 32
#endif // SEED_MAX_MODULES

// The maximum number of flags that can be passed to the compiler when compiling
// a module. If it hasn't been defined, define a default.
#ifndef SEED_MAX_FLAGS
#define SEED_MAX_FLAGS 64
#endif // SEED_MAX_FLAGS

enum sd_mod_flags {
  SD_SHARED = 1 << 0, // Module is a shared dependency for other modules.
};
typedef enum sd_mod_flags sd_mod_flags;

// One independently compilable object in our build system.
struct sd_module {
  const char *name;                   // Module name
  const char *path;                   // Absolute module path
  const char *command;                // Command run when compiling this module.
  sd_mod_flags mflags;                // Module flags.
  const char *cflags[SEED_MAX_FLAGS]; // Flags passed into compiler.
};
typedef struct sd_module sd_module;

// Static globals we use.
struct sd_core {
  sd_module modules[SEED_MAX_MODULES]; // Static pool of seed modules. [0] is
                                       // reserved for root.
  uint32_t offset; // Offset to space in static memory for unallocated modules.
  sd_module *root; // Root module location.
};
typedef struct sd_core sd_core;

static sd_core core = {
    .offset = 1,
    .modules = {0},
    .root = &core.modules[0]}; // Global core object for intermediate building.

sd_module *sd_root_create(const char *restrict name, const char *restrict path);
sd_module *sd_module_create(const char *name);
void sd_module_add_dep(sd_module *restrict module,
                       sd_module *restrict dependency);
void sd_module_share(sd_module *shared);

void sd_compile(void);

#endif // __SEED_H

#define SEED_IMPLEMENTATION // TODO: Remove
#ifdef SEED_IMPLEMENTATION

// Create root module. The module path is relative to [seed.h].
sd_module *sd_root_create(const char *restrict name,
                          const char *restrict path) {
  core.root->name = name;
  core.root->path = path;
  core.root->mflags = 0;
  return core.root;
}

// Create a new module under the root module.
sd_module *sd_module_create(const char *name) {
  sd_module *new_module = &core.modules[core.offset];
  core.offset++;

  new_module->name = name;
  new_module->path = name; // One subdirectory under owner.
  return new_module;
}

// Add a module dependency.
void sd_module_add_dep(sd_module *restrict module __unused,
                       sd_module *restrict dependency __unused) {
  /* no-op */
}

// Make a module shared.
void sd_module_share(sd_module *shared) { shared->mflags |= SD_SHARED; }

void sd_create_dir(const char *name) {}

// Compile the root module and its dependencies.
void sd_compile(void) {
  // 0. Check if $root/build exists, otherwise create it.
  FILE *build = fopen("build/out", "w");

  // 1. Compile all shared into $root/build

  // 2. Collect all .o names and pass it in as an argument to the final compile
  // command.

  // 3. Compile using cc main into $root/build
}

#endif // SEED_IMPLEMENTATION
