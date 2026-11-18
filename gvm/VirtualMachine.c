#include "VirtualMachine.h"
#include "Constants.h"
#include "std/error.h"
#include "std/io.h"
#include "std/memory.h"
#include <assert.h>
#include <stdint.h>

// Holds required details to run the VM
struct Machine {
  // Underlying data
  std_arena *buffer; // Machine's storage buffer
  std_file *file;    // Open GBC file

  // Code handling
  PageIndex
      codePageIndex[GBC_MAX_CONCURRENT_PAGES]; // Absolute page index
                                               // inside GBC file (-1 for N/A)
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

[[nodiscard]] static inline uint32_t instrOpcode(Instr instr) {
  return instr & GBC_OPCODE_MASK;
}

[[nodiscard]] static inline uint32_t instrFunc(Instr instr) {
  return (instr & GBC_FUNC_MASK) >> GBC_FUNC_SHIFT;
}
[[nodiscard]] static inline Register *instrT1RegA(Machine *machine,
                                                  Instr instr) {
  const uint32_t index = (instr & GBC_T1_REG_A_MASK) >> GBC_T1_REG_A_SHIFT;

  if (index > GBC_MAX_REGS) {
    std_eprintf("Accessing invalid register %u at address %llu", index,
                machine->rPc->valu);
    std_exit(EXIT_BAD_INSTR);
  }

  return &machine->registers[index];
}

[[nodiscard]] static inline uint32_t instrT1Imm(Instr instr) {
  return (instr & GBC_T1_IMM_MASK) >> GBC_T1_IMM_SHIFT;
}

[[nodiscard]] static inline Register *instrT2RegA(Machine *machine,
                                                  Instr instr) {
  const uint32_t index = (instr & GBC_T2_REG_A_MASK) >> GBC_T2_REG_A_SHIFT;

  if (index > GBC_MAX_REGS) {
    std_eprintf("Accessing invalid register %u at address %llu", index,
                machine->rPc->valu);
    std_exit(EXIT_BAD_INSTR);
  }

  return &machine->registers[index];
}

[[nodiscard]] static inline Register *instrT2RegB(Machine *machine,
                                                  Instr instr) {
  const uint32_t index = (instr & GBC_T2_REG_B_MASK) >> GBC_T2_REG_B_SHIFT;

  if (index > GBC_MAX_REGS) {
    std_eprintf("Accessing invalid register %u at address %llu", index,
                machine->rPc->valu);
    std_exit(EXIT_BAD_INSTR);
  }

  return &machine->registers[index];
}

[[nodiscard]] static inline uint32_t instrT2Imm(Instr instr) {
  return (instr & GBC_T2_IMM_MASK) >> GBC_T2_IMM_SHIFT;
}

[[nodiscard]] static inline Register *instrT3RegA(Machine *machine,
                                                  Instr instr) {
  const uint32_t index = (instr & GBC_T3_REG_A_MASK) >> GBC_T3_REG_A_SHIFT;

  if (index > GBC_MAX_REGS) {
    std_eprintf("Accessing invalid register %u at address %llu", index,
                machine->rPc->valu);
    std_exit(EXIT_BAD_INSTR);
  }

  return &machine->registers[index];
}

[[nodiscard]] static inline Register *instrT3RegB(Machine *machine,
                                                  Instr instr) {
  const uint32_t index = (instr & GBC_T3_REG_B_MASK) >> GBC_T3_REG_B_SHIFT;

  if (index > GBC_MAX_REGS) {
    std_eprintf("Accessing invalid register %u at address %llu", index,
                machine->rPc->valu);
    std_exit(EXIT_BAD_INSTR);
  }

  return &machine->registers[index];
}

[[nodiscard]] static inline Register *instrT3RegC(Machine *machine,
                                                  Instr instr) {
  const uint32_t index = (instr & GBC_T3_REG_C_MASK) >> GBC_T3_REG_C_SHIFT;

  if (index > GBC_MAX_REGS) {
    std_eprintf("Accessing invalid register %u at address %llu", index,
                machine->rPc->valu);
    std_exit(EXIT_BAD_INSTR);
  }

  return &machine->registers[index];
}

// Reads the file, exiting with an error if there is a read error or if the end
// of file is read.
[[nodiscard]]
static inline void *readFileChecked(std_file *file, std_arena *arena, size_t n,
                                    size_t size) {
  std_szptr read = std_file_read(file, arena, n, size);

  // Assert validity of read bytes
  if (read.size < n * size) {
    if (std_file_err(file) != FERR_EOF) {
      std_errno_msg("File read error");
    } else {
      std_errno_msg("Unexpected file end");
    }
    std_exit(EXIT_FILE_ERR);
  }

  return read.ptr;
}

static inline void seekFileChecked(std_file *file, long offset,
                                   std_seek_pos whence) {
  std_file_seek(file, offset, whence);
  if (std_file_err(file) != 0) {
  }
}

TEST_STATIC void consumeIdentifier(std_arena *arena, std_file *gbcFile) {
  uint64_t *ident = readFileChecked(gbcFile, arena, 1, sizeof(uint64_t));
  if (*ident != GBC_IDENT) {
    std_eprintf("File is not a GBC file!\n");
    std_exit(EXIT_FILE_ERR);
  }
}

TEST_STATIC void consumeVersionNumber(std_arena *arena, std_file *gbcFile) {
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
                                             std_file *gbcFile) {
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

  PageIndex absPageIndex = (PageIndex)(offset / GBC_MAX_PAGE_SZ);
  MemOffset pageOffsetInFile = (MemOffset)(GBC_MAX_PAGE_SZ * absPageIndex) +
                               machine->sections.codeSection;

  // Read page from file
  std_file_seek(machine->file, pageOffsetInFile, FSEEK_SET);
  size_t read =
      std_file_readp(machine->file, machine->codePages[machine->freePage],
                     GBC_MAX_PAGE_INSTRS, sizeof(Instr));

  if (read < GBC_MAX_PAGE_INSTRS && std_file_err(machine->file) != FERR_EOF) {
    std_eprintf("Failed to read GBC file!\n");
    std_exit(EXIT_FILE_ERR);
  } else if (std_file_err(machine->file) == FERR_EOF) { // Ignore EOF
    std_file_reset_err(machine->file);
  }

  trimExcessRead(machine->codePages[machine->freePage]);

  machine->codePageIndex[machine->freePage] = absPageIndex;

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
  std_memset((std_szptr){.ptr = machine->codePageIndex,
                         .size = GBC_MAX_CONCURRENT_PAGES * sizeof(PageIndex)},
             -1);

  machine->sections = parseHeader(gbcFile);
  machine->rPc = &machine->registers[GBC_REG_PC];
  machine->rIf = &machine->registers[GBC_REG_IF];
  machine->rEc = &machine->registers[GBC_REG_EC];

  // Set all registers to 0 as unsigned int (type 0)
  std_memset((std_szptr){.size = GBC_MAX_REGS * sizeof(Register),
                         .ptr = machine->registers},
             0);

  // Initialize program counter to start of file
  machine->rPc->valu = machine->sections.codeSection;
  if (machine->rPc->valu == 0) {
    std_eprintf("Invalid initial code offset %llull", machine->rPc->valu);
    std_exit(EXIT_BAD_INSTR);
  }

  readCodePage(machine,
               machine->rPc->valu); // Load first code page into memory

  return machine;
}

TEST_STATIC void destroyVirtualMachine(Machine *machine) {
  std_arena_destroy(machine->buffer);
}

static Instr *getLoadedCodePage(Machine *machine, PageIndex absPageIndex) {
  for (PageIndex i = 0; i < GBC_MAX_CONCURRENT_PAGES; i++) {
    if (machine->codePageIndex[i] == absPageIndex) {
      return machine->codePages[i];
    }
  }

  return nullptr;
}

TEST_STATIC Instr consumeNextInstruction(Machine *machine) {
  // Convert from memory address to index in codePage array.
  //  RelOffset = rPc - sections.codeSection (rpc >= codeSection)
  //  PageIndex = floor(RelOffset / PG_SIZE)
  //    Which absolute code page index? We then linear search through
  //    current code pages loaded in to find the correct one.
  //  MemIndex = (RelOffset % PG_SIZE) / sizeof(Instr)

  const MemOffset relOffset =
      machine->rPc->valu - machine->sections.codeSection;
  const PageIndex absPageIndex = (PageIndex)(relOffset / GBC_MAX_PAGE_SZ);

  Instr *page = getLoadedCodePage(machine, absPageIndex);
  if (!page) { // Page not loaded in memory
    readCodePage(machine, machine->rPc->valu);
    page = getLoadedCodePage(machine, absPageIndex);

    std_assert(page != nullptr, "pc out of section bounds at %llu",
               machine->rPc->valu);
  }

  machine->rPc->valu += sizeof(Instr);

  const MemOffset inPageOffset = (relOffset % GBC_MAX_PAGE_SZ) / sizeof(Instr);
  return page[inPageOffset];
}

TEST_STATIC MemOffset *getProgramCounter(Machine *machine) {
  return &machine->rPc->valu;
}

TEST_STATIC Register *getRegister(Machine *machine, uint32_t index) {
  return &machine->registers[index];
}

TEST_STATIC void runInstruction(Machine *machine, Instr instr) {
  // Small shared memory scratch arena
  static std_arena *scratch = nullptr;
  if (scratch == nullptr) {
    scratch = std_arena_create(3 * sizeof(uint64_t), ARENA_STOP_RESIZE);
  } else {
    std_arena_clean(scratch);
  }

  switch (instrOpcode(instr)) {
  case GBC_OP_LOADR: {
    Register *regDest = instrT2RegA(machine, instr);
    const Register *regSource = instrT2RegB(machine, instr);
    const uint16_t immediate =
        instrT2Imm(instr) & 0x0007; // Only consider bottom 3 bits

    const uint16_t readAmt = immediate == 0 ? 8 : immediate;
    std_file_seek(machine->file, regSource->valu, FSEEK_SET);

    regDest->valu = 0;
    regDest->type = RT_uint;
    std_file_readp(machine->file, &regDest->valu, readAmt, sizeof(byte));

    break;
  }

  case GBC_OP_LOAD:
    break;

  case GBC_OP_JUMP: {
    const uint32_t func = instrFunc(instr);
    const uint32_t imm = instrT1Imm(instr);
    switch (func) {
    case 0:
      machine->rPc->valu += imm;
      break;
    case 1:
      machine->rPc->valu -= imm;
      break;
    case 2:
      if (machine->rIf->valu > 0)
        machine->rPc->valu += imm;
      break;
    case 3:
      if (machine->rIf->valu > 0)
        machine->rPc->valu -= imm;
      break;
    default: // Should never occur
      std_panic("Impossible JUMP func value %u", func);
    }
    break;
  }

  case GBC_OP_JMPR: {
    const uint32_t func = instrFunc(instr);
    const Register *regA = instrT1RegA(machine, instr);

    switch (func) {
    case 0:
      machine->rPc->valu = regA->valu;
      break;
    case 1:
      if (machine->rIf->valu > 0)
        machine->rPc->valu = regA->valu;
      break;
    default:
      std_eprintf("Invalid JMPR func value %u\n", func);
      std_exit(EXIT_BAD_INSTR);
    }

    break;
  }

  case GBC_OP_ADD:
    break;
  case GBC_OP_SUB:
    break;
  case GBC_OP_MUL:
    break;
  case GBC_OP_DIV:
    break;
  case GBC_OP_MOD:
    break;
  case GBC_OP_NOT:
    break;
  case GBC_OP_AND:
    break;
  case GBC_OP_OR:
    break;
  case GBC_OP_XOR:
    break;
  case GBC_OP_SHIFT:
    break;
  case GBC_OP_LT:
    break;
  case GBC_OP_LE:
    break;
  case GBC_OP_EQ:
    break;
  case GBC_OP_PUSH:
    break;
  case GBC_OP_POP:
    break;
  case GBC_OP_CALL:
    break;
  case GBC_OP_MOVE:
    break;
  case GBC_OP_MOVEI:
    break;
  case GBC_OP_TYPE:
    break;
  case GBC_OP_EXIT:
    break;
  default:
    std_eprintf("Invalid GBC Opcode %d at address %llu", instrOpcode(instr),
                machine->rPc->valu);
    std_exit(EXIT_BAD_INSTR);
    break;
  }
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
    Instr instr = consumeNextInstruction(machine);
    runInstruction(machine, instr);
  }

  destroyVirtualMachine(machine);
}
