#include "assembler.h"
#include "gbc_constants.h"
#include "std/error.h"
#include "std/io.h"
#include "std/memory.h"
#include "std/strings.h"

#define ASSEMBLE_PANIC() std_panic("Failed to assemble file")

void gsm_first_pass(std_file *restrict ga_file, std_file *restrict store) {
  // Inline macros.

  // 1. Read line
  // 2. If line starts with ![WORD] [expand] then store [WORD] -> [expand].
  // 3. If line contains ![WORD] that has been found, replace it with [expand].

  // Remove comments.

  // Trim leading and trailing whitespace, and remove empty lines.

  return;
}

typedef struct {
  std_string name;
  uint32_t code;
} code_mapping;

static uint32_t get_code(std_string opcode);
static uint32_t get_opcode(std_string opcode);
static uint32_t get_register(std_string reg);
static uint32_t get_code1_func(std_string opcode);
static uint32_t get_code3_func(std_string opcode);

void gsm_second_pass(std_file *restrict ga_file, std_file *restrict gbc_file) {
  // 1. Add GBC metadata to binary file.
  // 2. Traverse through data section (until main:)
  //    2a. Write variable to GBC file.
  //    2b. Store memory offset and reference to variable.
  // 3. Traverse through code section (until EOF)
  //    3a. If link: Store memory offset and reference to any link.
  //    3b. If code: Convert into GBC, replace $vars and #links with memory
  //    offset. Replace numbers with necessary binary (default to base 10 if no
  //    0-alpha prefix)

  // Add GBC file header (See GBC file spec for more details on these)
  std_file_write(&g_gsm_guard, sizeof g_gsm_guard, 1, gbc_file);
  std_file_write(&g_gsm_magic, sizeof g_gsm_magic, 1, gbc_file);
  std_file_write(&g_gsm_major, sizeof g_gsm_major, 1, gbc_file);
  std_file_write(&g_gsm_minor, sizeof g_gsm_minor, 1, gbc_file);
  std_file_write(&g_gsm_patch, sizeof g_gsm_patch, 1, gbc_file);
  std_file_write(&g_gsm_unused, sizeof g_gsm_unused, 1, gbc_file);

  std_file_seek(ga_file, 0, FSEEK_SET);

  std_arena *line_memory = std_arena_create(sizeof(gbc_command), 0);

  // TODO: Read the data section of the GBC file

  // Begin reading code lines from the GA file
  while (std_file_err(ga_file) == 0) {
    std_string line = std_file_read_line(line_memory, ga_file);

    std_str_token *token;
    char space = ' ';
    std_str_tokenize(token, line, space);

    // TODO: Read tokens from a GA line and write its bytecode to the gbc file.
    gbc_command bytecode = {0};

    std_string opcode_str = std_str_token_get(token);
    bytecode |= get_opcode(opcode_str) << OPCODE_SHFT;

    switch (get_code(opcode_str)) {
    case 1: // imm? | rx? | func | opcode
      // Set opcode
      bytecode |= OPCODE_CODE1 << OPCODE_SHFT;

      // Set func
      uint32_t func = get_code1_func(opcode_str);
      bytecode |= func << CODE1_FNC_SHFT;

      // Set register rx if possible
      if (std_str_token_next(token)) {
        uint32_t rx = get_register(std_str_token_get(token));
        bytecode |= rx << CODE1_RX1_SHFT;
      }

      // Set immediate imm
      if (std_str_token_next(token)) {
      }

      break;
    case 2: // imm? | rx1 | rx2 | opcode

      break;
    case 3: // rx2 | rx1 | rx3 | 000 | func | opcode

      break;
    case 4: // Code 4 - imm | opcode

      break;
    default:
      std_panic("Reached unreachable code. Something's wrong!");
      break;
    }

    std_arena_clean(line_memory);
  }

  if (std_file_err(ga_file) != FERR_EOF) {
    std_errno_msg("Failed to properly read GA file");
    ASSEMBLE_PANIC();
  }

  std_arena_destroy(line_memory);
}

// Gets the code type number of the GA command based off its opcode.
static uint32_t get_code(std_string opcode) {
  static const code_mapping codes[] = {
      {str("ldu"), 1},  {str("jr"), 1},    {str("jrc"), 1},  {str("jrcx"), 1},
      {str("push"), 1}, {str("pop"), 1},   {str("zero"), 1},

      {str("addi"), 2}, {str("subi"), 2},  {str("bli"), 2},  {str("bri"), 2},
      {str("briu"), 2}, {str("andi"), 2},  {str("ori"), 2},  {str("xori"), 2},
      {str("lw"), 2},   {str("lh"), 2},    {str("lb"), 2},   {str("sw"), 2},
      {str("sh"), 2},   {str("sb"), 2},    {str("eq"), 2},   {str("gt"), 2},
      {str("lt"), 2},

      {str("add"), 3},  {str("sub"), 3},   {str("mul"), 3},  {str("mulu"), 3},
      {str("div"), 3},  {str("blx"), 3},   {str("brx"), 3},  {str("brxu"), 3},
      {str("and"), 3},  {str("or"), 3},    {str("xor"), 3},  {str("eqx"), 3},
      {str("gtx"), 3},  {str("ltx"), 3},   {str("addf"), 3}, {str("subf"), 3},
      {str("mulf"), 3}, {str("mulfu"), 3}, {str("divf"), 3},

      {str("sys"), 4},
  };
  static const uint32_t code_len = sizeof(codes) / sizeof(codes[0]);

  for (uint32_t i = 0; i < code_len; i++) {
    if (std_str_compare(opcode, codes[i].name) == 0) {
      return codes[i].code;
    }
  }

  std_panic("Invalid opcode read: %s", std_str_get(opcode));
}

// Gets the opcode integer from its string representation. This opcode needs
// to be shifted before being placed into an instruction.
static uint32_t get_opcode(std_string opcode) {
  static const code_mapping opcode_mappings[] = {
      {str("ldu"), OPCODE_CODE1},  {str("jr"), OPCODE_CODE1},
      {str("jrc"), OPCODE_CODE1},  {str("jrcx"), OPCODE_CODE1},
      {str("push"), OPCODE_CODE1}, {str("pop"), OPCODE_CODE1},
      {str("zero"), OPCODE_CODE1},

      {str("addi"), OPCODE_ADDI},  {str("subi"), OPCODE_SUBI},
      {str("bli"), OPCODE_BLI},    {str("bri"), OPCODE_BRI},
      {str("briu"), OPCODE_BRIU},  {str("andi"), OPCODE_ANDI},
      {str("ori"), OPCODE_ORI},    {str("xori"), OPCODE_XORI},
      {str("lw"), OPCODE_LW},      {str("lh"), OPCODE_LH},
      {str("lb"), OPCODE_LB},      {str("sw"), OPCODE_SW},
      {str("sh"), OPCODE_SH},      {str("sb"), OPCODE_SB},
      {str("eq"), OPCODE_EQ},      {str("gt"), OPCODE_GT},
      {str("lt"), OPCODE_LT},

      {str("add"), OPCODE_CODE3},  {str("sub"), OPCODE_CODE3},
      {str("mul"), OPCODE_CODE3},  {str("mulu"), OPCODE_CODE3},
      {str("div"), OPCODE_CODE3},  {str("blx"), OPCODE_CODE3},
      {str("brx"), OPCODE_CODE3},  {str("brxu"), OPCODE_CODE3},
      {str("and"), OPCODE_CODE3},  {str("or"), OPCODE_CODE3},
      {str("xor"), OPCODE_CODE3},  {str("eqx"), OPCODE_CODE3},
      {str("gtx"), OPCODE_CODE3},  {str("ltx"), OPCODE_CODE3},
      {str("addf"), OPCODE_CODE3}, {str("subf"), OPCODE_CODE3},
      {str("mulf"), OPCODE_CODE3}, {str("mulfu"), OPCODE_CODE3},
      {str("divf"), OPCODE_CODE3},

      {str("sys"), OPCODE_SYS},
  };

  static const uint32_t opcode_table_size =
      sizeof(opcode_mappings) / sizeof(opcode_mappings[0]);
  for (uint32_t i = 0; i < opcode_table_size; i++) {
    if (std_str_compare(opcode, opcode_mappings[i].name) == 0) {
      return opcode_mappings[i].code;
    }
  }

  std_panic("Invalid opcode read: %s", std_str_get(opcode));
}

// Gets the register number from its string representation. This register
// needs to be shifted before being placed into an instruction.
static uint32_t get_register(std_string reg) {
  static const code_mapping reg_mappings[] = {
      {str("ax"), R_AX},   {str("bx"), R_BX},   {str("cx"), R_CX},
      {str("dx"), R_DX},   {str("ex"), R_EX},   {str("fx"), R_FX},
      {str("gx"), R_GX},   {str("hx"), R_HX},

      {str("sax"), R_SAX}, {str("sbx"), R_SBX}, {str("scx"), R_SCX},
      {str("sdx"), R_SDX},

      {str("ret"), R_RET}, {str("up"), R_UP},   {str("0"), R_ZERO},
      {str("if"), R_IF},   {str("tg"), R_TG},   {str("sp"), R_SP},
      {str("srg"), R_SRG}, {str("asm"), R_ASM}, {str("pc"), R_PC},
  };

  static const uint32_t reg_table_size =
      sizeof(reg_mappings) / sizeof(reg_mappings[0]);
  for (uint32_t i = 0; i < reg_table_size; i++) {
    if (std_str_compare(reg, reg_mappings[i].name) == 0) {
      return reg_mappings[i].code;
    }
  }

  std_panic("Invalid register read: %s", std_str_get(reg));
}

// Gets the func from a code 1 opcode string. This func needs to be shifted
// before being placed into an instruction.
static uint32_t get_code1_func(std_string opcode) {
  static const code_mapping func_mappings[] = {
      {str("ldu"), FUNC_LDU},   {str("jr"), FUNC_JR},
      {str("jrc"), FUNC_JRC},   {str("jrcx"), FUNC_JRCX},
      {str("push"), FUNC_PUSH}, {str("pop"), FUNC_POP},
      {str("zero"), FUNC_ZERO},
  };

  static const uint32_t func_table_size =
      sizeof(func_mappings) / sizeof(func_mappings[0]);
  for (uint32_t i = 0; i < func_table_size; i++) {
    if (std_str_compare(opcode, func_mappings[i].name) == 0) {
      return func_mappings[i].code;
    }
  }

  std_panic("Invalid opcode read: %s", std_str_get(opcode));
}

// Gets the func from a code 3 opcode string. This func needs to be shifted
// before being placed into an instruction.
static uint32_t get_code3_func(std_string opcode) {
  static const code_mapping func_mappings[] = {
      {str("add"), FUNC_ADD},   {str("sub"), FUNC_SUB},
      {str("mul"), FUNC_MUL},   {str("mulu"), FUNC_MULU},
      {str("div"), FUNC_DIV},   {str("blx"), FUNC_BLX},
      {str("brx"), FUNC_BRX},   {str("brxu"), FUNC_BRXU},
      {str("and"), FUNC_AND},   {str("or"), FUNC_OR},
      {str("xor"), FUNC_XOR},   {str("eqx"), FUNC_EQX},
      {str("gtx"), FUNC_GTX},   {str("ltx"), FUNC_LTX},
      {str("addf"), FUNC_ADDF}, {str("subf"), FUNC_SUBF},
      {str("mulf"), FUNC_MULF}, {str("mulfu"), FUNC_MULFU},
      {str("divf"), FUNC_DIVF},
  };

  uint32_t func_table_size = sizeof(func_mappings) / sizeof(func_mappings[0]);
  for (uint32_t i = 0; i < func_table_size; i++) {
    if (std_str_compare(opcode, func_mappings[i].name) == 0) {
      return func_mappings[i].code;
    }
  }

  std_panic("Invalid opcode read: %s", std_str_get(opcode));
}
