// GSM - The Grassroots ASM parser.

// Steps to convert to GBC:
//
// A. Get GA file from command line.
// B. Open GA file for reading.
//
// C. First Pass (We create a temporary GA file):
// 1. Inline macros.
// 2. Remove comments.
// 3. Trim leading and trailing whitespace, and remove empty lines.
//
// D. Second Pass (We create a .gbc file):
// 1. Add GBC metadata to binary file.
// 2. Traverse through data section (until main:)
//    2a. Write variable to GBC file.
//    2b. Store memory offset and reference to variable.
// 3. Traverse through code section (until EOF)
//    3a. If link: Store memory offset and reference to any link.
//    3b. If code: Convert into GBC, replace $vars and #links with memory
//    offset. Replace numbers with necessary binary (default to base 10 if no
//    0-alpha prefix)
//
// On error:
// 1. Remove any created artefacts:
//    - .gbc file.

#include "gbc_constants.h"
#include "std/cli.h"
#include "std/error.h"
#include "std/io.h"
#include "std/memory.h"
#include "std/strings.h"
#include <stdint.h>

int32_t gsm_first_pass(std_file *restrict ga_file, std_file *restrict store);
int32_t gsm_second_pass(std_file *restrict ga_file,
                        std_file *restrict gbc_file);

int main(int argc, const char **argv) {
  std_string file_name = str_empty();

  // Parse CLI arguments

  std_argument arg;
  do {
    arg = cli_argv_next(argc, argv);

    if (arg.type == ARG_ARGUMENT) { // First arg is the name of the file.
      file_name = arg.argument.val;
      break;
    }
  } while (arg.type != ARG_END);

  // No file name passed in
  if (str_isempty(file_name)) {
    eprintf("Invalid file name provided\n");
    return 1;
  }

  std_arena *working_memory = arena_create(0, 0);

  // Open GA file for reading
  std_file *ga_file = file_open(working_memory, file_name, FOPEN_READ, 0);
  if (file_err(ga_file)) {
    errno_msg("Couldn't open GA file");
    return 2;
  }

  std_file *first_pass = file_temp(working_memory);
  if (file_err(first_pass)) {
    errno_msg("Couldn't create temporary file for assembler first pass");
    return 3;
  }

  std_file *out_file = file_open(working_memory, str("a.out"), FOPEN_WRITE, 0);
  if (file_err(out_file)) {
    errno_msg("Couldn't create output file");
    return 4;
  }

  // Copy GA file into temporary, inlining any macros.
  gsm_first_pass(ga_file, first_pass);

  // Create GBC file
  gsm_second_pass(first_pass, out_file);
}

// Performs a GSM first pass on [ga_file] and stores the result in [store].
// Returns 0 on a success, and a non-zero error code on failure.
int32_t gsm_first_pass(std_file *restrict ga_file, std_file *restrict store) {
  // Inline macros.

  // 1. Read line
  // 2. If line starts with ![WORD] [expand] then store [WORD] -> [expand].
  // 3. If line contains ![WORD] that has been found, replace it with [expand].

  // Remove comments.

  // Trim leading and trailing whitespace, and remove empty lines.

  return 0;
}

// Gets the opcode integer from its string representation. This opcode needs
// to be shifted before being placed into an instruction.
uint32_t get_opcode(std_string opcode);

// Gets the register number from its string representation. This register
// needs to be shifted before being placed into an instruction.
uint32_t get_register(std_string reg);

// Gets the func from a code 1 opcode string. This func needs to be shifted
// before being placed into an instruction.
uint32_t get_code1_func(std_string opcode);

// Gets the func from a code 3 opcode string. This func needs to be shifted
// before being placed into an instruction.
uint32_t get_code3_func(std_string opcode);

// Performs a GSM second pass on [ga_file] and stores the resulting binary in
// [gbc_file]. Returns 0 on a success, and a non-zero error code on failure.
int32_t gsm_second_pass(std_file *restrict ga_file,
                        std_file *restrict gbc_file) {

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

  file_write(&g_gsm_guard, sizeof g_gsm_guard, 1, gbc_file);
  file_write(&g_gsm_magic, sizeof g_gsm_magic, 1, gbc_file);
  file_write(&g_gsm_major, sizeof g_gsm_major, 1, gbc_file);
  file_write(&g_gsm_minor, sizeof g_gsm_minor, 1, gbc_file);
  file_write(&g_gsm_patch, sizeof g_gsm_patch, 1, gbc_file);
  file_write(&g_gsm_unused, sizeof g_gsm_unused, 1, gbc_file);

  long instr_start_mark = file_tell(gbc_file);

  file_seek(ga_file, 0, FSEEK_SET);

  if (file_err(gbc_file))
    return file_err(gbc_file);

  std_arena *line_memory = arena_create(sizeof(int32_t), 0);

  // TODO: Read the data section of the GBC file

  // Begin reading code lines from the GA file
  while (file_err(ga_file) == 0) {
    std_string line = file_read_line(line_memory, ga_file);

    std_str_token *token;
    char space = ' ';
    str_tokenize(token, line, space);

    // TODO: Read tokens from a GA line and write its bytecode to the gbc file.
    gbc_command command;

    uint32_t bytecode;

    uint32_t opcode = get_opcode(str_token_get(token));
    bytecode |= opcode << OPCODE_SHFT;

    switch (opcode) {
    case OPCODE_CODE1: // imm | rx1 | func | opcode
      uint32_t func = get_code1_func(str_token_get(token));
      bytecode |= func << CODE1_FNC_SHFT;

      uint32_t rx1 = get_register(str_token_get(token));
      bytecode |= rx1 << CODE1_RX1_SHFT;

      uint32_t imm = eprintf("bytecode: %x", bytecode);

    case OPCODE_CODE3: // rx3 | rx2 | rx1 | 000 | func | opcode

    default: // imm | rx2 | rx1 | opcode
    }

    arena_clean(line_memory);
  }

  arena_destroy(line_memory);
  return 0;
}

typedef struct {
  const char *name;
  uint32_t code;
} code_mapping;

uint32_t get_opcode(std_string opcode) {
  static const code_mapping opcode_mappings[] = {
      {"ldu", OPCODE_CODE1},  {"jr", OPCODE_CODE1},   {"jrc", OPCODE_CODE1},
      {"jrcx", OPCODE_CODE1}, {"push", OPCODE_CODE1}, {"pop", OPCODE_CODE1},
      {"zero", OPCODE_CODE1}, {"sys", OPCODE_CODE1},

      {"add", OPCODE_CODE3},  {"sub", OPCODE_CODE3},  {"mul", OPCODE_CODE3},
      {"mulu", OPCODE_CODE3}, {"div", OPCODE_CODE3},  {"blx", OPCODE_CODE3},
      {"brx", OPCODE_CODE3},  {"brxu", OPCODE_CODE3}, {"and", OPCODE_CODE3},
      {"or", OPCODE_CODE3},   {"xor", OPCODE_CODE3},  {"eq", OPCODE_CODE3},
      {"gt", OPCODE_CODE3},   {"lt", OPCODE_CODE3},   {"eqx", OPCODE_CODE3},
      {"gtx", OPCODE_CODE3},  {"ltx", OPCODE_CODE3},  {"addf", OPCODE_CODE3},
      {"subf", OPCODE_CODE3}, {"mulf", OPCODE_CODE3}, {"mulfu", OPCODE_CODE3},
      {"divf", OPCODE_CODE3},

      {"addi", OPCODE_ADDI},  {"subi", OPCODE_SUBI},  {"bli", OPCODE_BLI},
      {"bri", OPCODE_BRI},    {"briu", OPCODE_BRIU},  {"andi", OPCODE_ANDI},
      {"ori", OPCODE_ORI},    {"xori", OPCODE_XORI},  {"lw", OPCODE_LW},
      {"lh", OPCODE_LH},      {"lb", OPCODE_LB},      {"sw", OPCODE_SW},
      {"sh", OPCODE_SH},      {"sb", OPCODE_SB},
  };

  uint32_t opcode_table_size =
      sizeof(opcode_mappings) / sizeof(opcode_mappings[0]);
  for (uint32_t i = 0; i < opcode_table_size; i++) {
    if (str_compare(opcode, str(opcode_mappings[i].name)) == 0) {
      return opcode_mappings[i].code;
    }
  }

  std_panic("Invalid opcode read: %s", str_get(opcode));
}

uint32_t get_register(std_string reg) {
  static const code_mapping reg_mappings[] = {
      {"ax", R_AX},   {"bx", R_BX},   {"cx", R_CX},   {"dx", R_DX},
      {"ex", R_EX},   {"fx", R_FX},   {"gx", R_GX},   {"hx", R_HX},

      {"sax", R_SAX}, {"sbx", R_SBX}, {"scx", R_SCX}, {"sdx", R_SDX},

      {"ret", R_RET}, {"up", R_UP},   {"0", R_ZERO},  {"if", R_IF},
      {"tg", R_TG},   {"sp", R_SP},   {"srg", R_SRG}, {"asm", R_ASM},
      {"pc", R_PC},
  };

  uint32_t reg_table_size = sizeof(reg_mappings) / sizeof(reg_mappings[0]);
  for (uint32_t i = 0; i < reg_table_size; i++) {
    if (str_compare(reg, str(reg_mappings[i].name)) == 0) {
      return reg_mappings[i].code;
    }
  }

  std_panic("Invalid register read: %s", str_get(reg));
}

uint32_t get_code1_func(std_string opcode) {
  static const code_mapping func_mappings[] = {
      {"ldu", FUNC_LDU},   {"jr", FUNC_JR},     {"jrc", FUNC_JRC},
      {"jrcx", FUNC_JRCX}, {"push", FUNC_PUSH}, {"pop", FUNC_POP},
      {"zero", FUNC_ZERO}, {"sys", FUNC_SYS},
  };

  uint32_t func_table_size = sizeof(func_mappings) / sizeof(func_mappings[0]);
  for (uint32_t i = 0; i < func_table_size; i++) {
    if (str_compare(opcode, str(func_mappings[i].name)) == 0) {
      return func_mappings[i].code;
    }
  }

  std_panic("Invalid opcode read: %s", str_get(opcode));
}

uint32_t get_code3_func(std_string opcode) {
  static const code_mapping func_mappings[] = {
      {"add", FUNC_ADD},   {"sub", FUNC_SUB},   {"mul", FUNC_MUL},
      {"mulu", FUNC_MULU}, {"div", FUNC_DIV},   {"blx", FUNC_BLX},
      {"brx", FUNC_BRX},   {"brxu", FUNC_BRXU}, {"and", FUNC_AND},
      {"or", FUNC_OR},     {"xor", FUNC_XOR},   {"eq", FUNC_EQ},
      {"gt", FUNC_GT},     {"lt", FUNC_LT},     {"eqx", FUNC_EQX},
      {"gtx", FUNC_GTX},   {"ltx", FUNC_LTX},   {"addf", FUNC_ADDF},
      {"subf", FUNC_SUBF}, {"mulf", FUNC_MULF}, {"mulfu", FUNC_MULFU},
      {"divf", FUNC_DIVF},
  };

  uint32_t func_table_size = sizeof(func_mappings) / sizeof(func_mappings[0]);
  for (uint32_t i = 0; i < func_table_size; i++) {
    if (str_compare(opcode, str(func_mappings[i].name)) == 0) {
      return func_mappings[i].code;
    }
  }

  std_panic("Invalid opcode read: %s", str_get(opcode));
}
