#include "VirtualMachine.h"
#include "Constants.h"
#include "std/error.h"
#include "std/io.h"
#include "std/memory.h"
#include <stdint.h>

#define ASSERT_VALID(file, read, expectedSize)                                 \
  do {                                                                         \
    if ((read).size < expectedSize) {                                          \
      if (std_file_err((file)) != FERR_EOF) {                                  \
        std_errno_msg("File read error");                                      \
      }                                                                        \
      std_eprintf("Unable to parse GBC file.\n");                              \
      std_exit(EXIT_FILE_ERR);                                                 \
    }                                                                          \
  } while (0)

[[nodiscard]]
static inline void *readFileChecked(std_file *restrict file,
                                    std_arena *restrict arena, size_t n,
                                    size_t size) {
  std_szptr read = std_file_read(file, arena, n, size);
  ASSERT_VALID(file, read, n * size);
  return read.ptr;
}

TEST_STATIC void consumeIdentifier(std_arena *restrict arena,
                                   std_file *restrict gbcFile) {
  uint64_t *ident = readFileChecked(gbcFile, arena, 1, sizeof(uint64_t));
  if (*ident != GBC_IDENT) {
    std_eprintf("File is not a GBC file!\n");
    std_exit(EXIT_FILE_ERR);
  }
}

TEST_STATIC void consumeVersionNumber(std_arena *restrict arena,
                                      std_file *restrict gbcFile) {
  uint16_t *version =
      readFileChecked(gbcFile, arena, GBC_VER_BYTES, sizeof(uint16_t));

  bool isIncorrectVersion =
      version[0] != GBC_VER_MAJOR || version[1] != GBC_VER_MINOR;
  if (isIncorrectVersion) {
    std_eprintf(
        "GBC file is incorrect version: is v%d.%d.%d, needs v%d.0 - v%d.%d!\n",
        version[0], version[1], version[2], GBC_VER_MAJOR, GBC_VER_MAJOR,
        GBC_VER_MINOR);
    std_exit(EXIT_FILE_ERR);
  }
}

TEST_STATIC GBCOffsets consumeSectionOffsets(std_arena *arena,
                                             std_file *restrict gbcFile) {
  MemOffset *readOffsets =
      readFileChecked(gbcFile, arena, 4, sizeof(MemOffset));
  return (GBCOffsets){
      .linkTable = readOffsets[0],
      .codeSection = readOffsets[1],
      .typeTable = readOffsets[2],
      .dataSection = readOffsets[3],
  };
}

static GBCOffsets parseHeader(std_file *gbcFile) {
  std_file_seek(gbcFile, 0, FSEEK_SET);

  GBCOffsets offsets = {};
  std_with_arena(arena, std_dyn_arena()) {
    consumeIdentifier(arena, gbcFile);
    consumeVersionNumber(arena, gbcFile);
    offsets = consumeSectionOffsets(arena, gbcFile);
  }

  return offsets;
}

// Holds required details to run the VM
typedef struct {
  // Code handling
  Instr codePage[GBC_MAX_PAGE_SZ][GBC_MAX_PAGES];

  // Memory
  Register registers[GBC_MAX_REGS];
  // TODO: Static data

  // GBC data
  GBCOffsets sections;

  // Specialized Registers
  Register *rPc; // Program counter
  Register *rIf; // Comp results
  Register *rEc; // Exit code
} Machine;

// Reads up to GBC_MAX_PAGE_SZ instructions or until end of
// code section into memory.
static void readCodePage(Machine *machine, uint32_t page) {}

static Machine initVirtualMachine(std_file *gbcFile) {
  Machine machine = {};

  machine.sections = parseHeader(gbcFile);
  machine.rPc = &machine.registers[GBC_REG_PC];
  machine.rIf = &machine.registers[GBC_REG_IF];
  machine.rEc = &machine.registers[GBC_REG_EC];

  // Initialize program counter to start of file
  *machine.rPc = machine.sections.codeSection;
  std_assert(*machine.rPc != 0, "Invalid code offset");

  return machine;
}

static Machine destroyVirtualMachine([[maybe_unused]] Machine *machine) {
  // no-op
}

int runVirtualMachine(std_file *gbcFile) {
  Machine machine = initVirtualMachine(gbcFile);

  destroyVirtualMachine(&machine);
  return 0;
}
