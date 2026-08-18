#include "VirtualMachine.h"
#include "Constants.h"
#include "std/error.h"
#include "std/io.h"
#include "std/memory.h"
#include <stdint.h>

[[nodiscard]]
static inline void *readFileChecked(std_file *restrict file,
                                    std_arena *restrict arena, size_t n,
                                    size_t size) {
  std_szptr read = std_file_read(file, arena, n, size);

  // Assert validity of read bytes
  if (read.size < n * size) {
    if (std_file_err(file) != FERR_EOF) {
      std_errno_msg("File read error");
    }
    std_eprintf("Unable to parse GBC file.\n");
    std_exit(EXIT_FILE_ERR);
  }

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
struct Machine {
  // Underlying data
  std_arena *buffer; // Machine's storage buffer
  std_file *file;    // Open GBC file

  // Code handling
  Instr codePages[GBC_MAX_CONCURRENT_PAGES][GBC_MAX_PAGE_INSTRS];
  uint16_t freePage; // Next page in 0-index that is available to be written to.
                     // If this is >= GBC_MAX_PAGE_SZ, then it wraps to 0.
  bool shouldExit;   // Has program ended?

  // Memory
  Register registers[GBC_MAX_REGS];
  // TODO: Static data

  // GBC data
  GBCOffsets sections;

  // Specialized Registers
  Register *rPc; // Program counter
  Register *rIf; // Comp results
  Register *rEc; // Exit code
};

// Clears any data after an end marker read (0x0000 0000) to prevent private
// data from being read.
static void trimExcessRead(Instr codePage[GBC_MAX_PAGE_INSTRS]) {
  uint32_t codeEnd = 0; // 0x0000 0000

  for (size_t i = 0; i < GBC_MAX_PAGE_INSTRS; i++) {
    if (codePage[i] == codeEnd) {
      size_t remainingBuffer = GBC_MAX_PAGE_SZ - (i * sizeof(Instr));
      std_memset((std_szptr){.ptr = codePage + i, .size = remainingBuffer}, 0);
      return;
    }
  }
}

TEST_STATIC void readCodePage(Machine *machine, MemOffset offset) {
  // Steps:
  //  - Calculate which code page offset is on: page = floor(offset /
  //  GBC_MAX_PAGE_SZ)
  //  - Calculate starting offset of the code page in that file.
  //  - Read GBC_MAX_PAGE_SZ bytes from that offset or until 0x0000 0000 is
  //  read.
  //  - Increase freePage by 1 or reset it to 0 (if > GBC_MAX_CONCURRENT_PAGES).

  uint16_t page = (uint16_t)(offset / GBC_MAX_PAGE_SZ);
  MemOffset pageOffsetInFile =
      (MemOffset)(GBC_MAX_PAGE_SZ * page) + machine->sections.codeSection;

  // Read page from file
  std_file_seek(machine->file, pageOffsetInFile, FSEEK_SET);
  size_t read =
      std_file_readp(machine->file, machine->codePages[machine->freePage],
                     GBC_MAX_PAGE_INSTRS, sizeof(Instr));

  if (read < GBC_MAX_PAGE_INSTRS && std_file_err(machine->file) != FERR_EOF) {
    std_eprintf("Failed to read GBC file!\n");
    std_exit(EXIT_FILE_ERR);
  }

  trimExcessRead(machine->codePages[machine->freePage]);

  // Increment free page or reset to 0
  machine->freePage++;
  if (machine->freePage >= GBC_MAX_CONCURRENT_PAGES) {
    machine->freePage = 0;
  }
}

TEST_STATIC Machine *initVirtualMachine(std_file *gbcFile) {
  std_arena *buffer = std_dyn_arena();
  Machine *machine = std_arena_alloc(buffer, sizeof(Machine));

  machine->buffer = buffer;
  machine->file = gbcFile;
  machine->shouldExit = false;

  std_memset((std_szptr){.ptr = machine->codePages,
                         .size = GBC_MAX_CONCURRENT_PAGES * GBC_MAX_PAGE_SZ},
             0);
  machine->freePage = 0;

  machine->sections = parseHeader(gbcFile);
  machine->rPc = &machine->registers[GBC_REG_PC];
  machine->rIf = &machine->registers[GBC_REG_IF];
  machine->rEc = &machine->registers[GBC_REG_EC];

  // Initialize program counter to start of file
  *machine->rPc = machine->sections.codeSection;
  std_assert(*machine->rPc != 0, "Invalid code offset");
  readCodePage(machine, *machine->rPc); // Load first code page into memory

  return machine;
}

TEST_STATIC void destroyVirtualMachine(Machine *machine) {
  std_arena_destroy(machine->buffer);
}

void runVirtualMachine(std_file *gbcFile) {
  Machine *machine = initVirtualMachine(gbcFile);

  // TODO
  // 1. Read next page into memory based on PC.
  //  1a. Overwrite oldest memory if necessary.
  // 2. Run instruction at current PC.
  //  2a. Switch based on op.
  // 3. Increment counter.

  while (!machine->shouldExit) {
  }

  destroyVirtualMachine(machine);
}
