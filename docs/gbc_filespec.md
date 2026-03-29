Version 1.0.0

# Intro

The purpose of this sheet is to document the formal definition and requirements of the Grassroots
bytecode (GBC) language specification.

# Motivations

GBC is a language built to run on the 64-bit Grassroots virtual machine (GVM).
It should take advantage of GVM abilities to encode higher-level concepts
and utilities into each binary. At the same time, GBC aims to be relatively
simple in its implementation.

# File Specifications

## Bytecode size

GBC has _32-bit_ bytecode instruction size.
Registers can hold _64 bits_ of data.

## File Structure

A GBC file has the following file format:

```
+--------------+
| GBC header   |
+--------------+
| Link Table   |
+--------------+
| Code Section |
+--------------+
| Data Type    |
| Table        |
+--------------+
| Data Section |
+--------------+
```

All GBC values must be aligned to 32-bits, since that is the smallest unit the
GVM is expected to read at a time.

Following from the previous specification, a GBC file size should be a multiple of
32 bits.

### GBC Header (`gbc_header`)

The GBC header is defined as the following:

```c
typedef struct {
    uint8_t ident[8];   // Identifier
    uint16_t ver[4];    // Version identifier

    uint64_t lt_off;    // Link table offset in bytes
    uint64_t code_off;  // GBC code section offset in bytes
    uint64_t type_off;  // Data type table offset in bytes
    uint64_t data_off;  // Static data section offset in bytes
} gbc_header;
```

All offsets should be aligned to 32-bit boundaries.

#### GBC Identifier (`ident`)

`ident` is defined as the following:

```
hex    F7   47 42 43 0a 0d 1a   0a
ascii  \367 G  B  C  \n \r \032 \n
```

See [PNG File Signature Rationale](http://www.libpng.org/pub/png/spec/1.2/PNG-Rationale.html#R.PNG-file-signature)
for more details about this type of file signature.

#### Version (`ver`)

`version` defines the current GBC version in SemVer format.

- `ver[0]` defines major version.
- `ver[1]` defines minor version.
- `ver[2]` defines patch version.
- `ver[3]` marks the end of versioning metadata as is defined as NULL character \0.

`version` is defined following the current GBC version.

GBC files should be backwards compatible with any previous minor and patch versions of the same
major version, and forwards compatible with any future patch versions of the same major version.

New major versions can break compatibility.

#### Link Table Offset (`lt_off`)

`lt_off` is defined as the offset to the link table in bytes.
This is set to 0 if no link table exists.

#### Code Section Offset (`code_off`)

`code_off` is defined as the offset to the code section in bytes.

#### Type Section Offset (`type_off`)

`type_off` is defined as the offset to the data type map section in bytes.
This is set to 0 if no data section exists.

#### Data Section Offset (`data_off`)

`data_off` is defined as the offset to the data section in bytes.
This is set to 0 if no data section exists.

### Link Table

The link table is defined as the following:

```c
typedef struct {
    uint64_t count;         // Number of table entries
    lt_row entry[];         // Link table entries
} link_table;

typedef struct {
    uint64_t addr;          // Loading address for function
    const char func_id[];   // Search path to function
} lt_row;
```

An example link table entry to a function loaded from a relative file path "./libs/lib.gbs"
may look like this:

```c
lt_row row = {
    .addr: 0xA0001000,
    .func_id: "./libs::lib::func",
};
```

A `CALL` instruction to the 0xA0001000 virtual address then causes this function to be accessed and run.

#### Count of Link Table Entries (`count`)

`count` is defined as the number of existing link table entries.

#### Dynamic Function Address (`addr`)

`addr` is defined as the virtual address that the dynamic function should be loaded into.

#### Function Id (`func_id`)

`func_id` defines the search path and function name identifier of a dynamic function.
`func_id` is a null-terminated char array, defined as `[path]::[filename]::[func]` where `path`
defines the search path, `filename` defines the name of the file, and `func` defines the
dynamic function name.

- `func` cannot be an empty string (`path::name::` is disallowed).
- `filename` cannot be an empty string (`path::::func` is disallowed).
- `path` may be an empty string (`::name::func` is allowed). When `path` is undefined, it is the
  responsibility of the virtual machine to define and search default file paths to find the correct
  filename and function.

### Code Section

The code section is defined as the following:

```c
typedef struct {
    uint32_t instr[];       // Instruction segments
    uint32_t code_end;      // Marks the end of segment
} code;
```

#### Instruction Segments (`instr`)

The `instr` array defines each 32-bit instruction.

#### End marker (`code_end`)

`code_end` is defined as `0x00000000`, and marks the end of the code section.

### Data Type Table

The data type table defines a map between index values and static data inside the
data section. The map allows registers and memory accesses to be typed, letting
the GVM automatically determine how many bytes to read on a memory access during
a `LOAD` instruction.

There is an table entry for each primitive data type stored in the data section.
For arrays of a type, the table contains an entry to the first element, marking
that element's size (1, 2, 4, 8 bytes). The size of the array is not maintained.

```c
// Type definitions for dt_row::type field
#define UINT 0              // SIZE-byte unsigned integer
#define INT 1               // SIZE-byte signed integer
#define FLOAT 2             // SIZE-byte floating point

typedef struct {
    uint16_t count;     // Number of table entries
    dt_row entry[];     // Data type table entries
} type_table;

typedef struct {
    uint8_t size;       // Size in bytes of data type
    uint8_t type;       // Defined using type definitions
    uint16_t padding;   // Should be set to 0; reserved space for future versions.
    uint64_t addr;      // Data section address
} dt_row;
```

### Data Section

The data section is defined as a raw dump of static bytes, addressable through 64-bit
pointer addresses.

```c
typedef struct {
    uint32_t data[];
} data;
```

Though data is byte-addressable, it must be stored internally as 32-bit values, to
avoid the GVM (which reads in 32-bit words) reading past the end of the file. Any excess
data space should be initialized to 0.

## Memory Map

The following table describes at which virtual addresses different GBC
sections are loaded into memory.

The table defines the upper 32-bits of each virtual address.
All sections start with bottom 4 bytes 0x00, and end with bottom 4 bytes 0xFF.

| Section Name      | Virtual Address Start | Virtual Address End |
| ----------------- | --------------------- | ------------------- |
| Runtime Memory    | 0x00010000            | 0x9FFFFFFF          |
| Dynamic Functions | 0xA0000000            | 0xAFFFFFFF          |
| Code Section      | 0xB0000000            | 0xBFFFFFFF          |
| Data Section      | 0xC0000000            | 0xFFEFFFFF          |

Link and data type tables should be maintained by the GVM, but should not be
accessible by the to the GBC program.

Other memory addresses are reserved for GVM uses, such as marking system interrupt boundaries.

## GBC Instructions

### Terminology

#### Opcodes

Opcodes are 6-bit wide identifiers put into the lowest order bits of an instruction.

#### Registers

There are 256 allowed registers, marked from 0-255.
Registers are represented in this document as _'N_, where _N_ represents the _N_-th
integer.
230 general use registers ('1 - '230) are made available for general purpose use.
The remaining registers ('0, '231 - '255) currently, or may in the future be used to
represent specific information.

Registers maintain type information about the value they contain, derived from `TYPE`
operations or from `LOAD` operations from memory. Registers contain fixed-width
64-bit values, and smaller-size values are extended as necessary when loaded. As an
example, loading a signed, 32-bit negative number into a register will move the sign
bit to the 64th bit instead, extending it to a 64-bit negative number.

Reserved registers have specified initialization values they should be set to.

The following registers are reserved:

- `'zero = '0`: The void register is always zero, when copying from or to.
- `'pc = '255`: The program counter register represents the current address in code.
- `'if = '254`: The cond register stores the result of comparison operations. This starts at 0.
- `'ec = '253`: The exit code register, storing the program return value. Only the lowest 16 bits are read. This starts at 0.

### Instruction Format

#### Instruction Notation

- 'A, 'B, and 'C represent an arbitrary register from '0 to '255.
- STACK represents the program stack.
- IMM:N represents an n-bit immediate.
- val('A) represents taking the value of register A (as its underlying type).
- uint('A) represents taking the value of register A as an unsigned 64-bit integer.
- int('A) represents taking the value of register A as an signed 64-bit integer.
- float('A) represents taking the value of register A as a 64-bit float.
- type('A) represents the type of a value stored at register A.
- at(X) represents value stored at an address X.

#### Type 1 Instructions

Type 1 instructions take in 1 register as an argument.
They are defined as follows:

```
    31             15     7        5        0
   | immediate:16 | 'A:8 | func:2 | opcode:6 |
```

#### Type 2 Instructions

Type 2 instructions take in 2 registers as arguments.
They are defined as follows:

```
    31             21     13     5        0
   | immediate:10 | 'B:8 | 'A:8 | opcode:6 |
```

#### Type 3 Instructions

Type 3 instructions take in 3 registers as arguments.
They are defined as follows:

```
    31     23     15      7        5        0
   | 'C:8 | 'B:8 | 'A:8  | func:2 | opcode:6 |
```

### Instruction Set

The following table summarizes the current set of instructions.

| Opcode | Type | Name  | Description                                                        |
| ------ | ---- | ----- | ------------------------------------------------------------------ |
| 0x01   | 2    | LOAD  | Load a static value into a register.                               |
| 0x02   | 1    | LOADI | Load a value from an immediate address into a register.            |
| 0x03   | 1    | JUMP  | Jump to an offset from the current program counter.                |
| 0x04   | 1    | JMPR  | Jump to an address stored in a register.                           |
| 0x05   | 3    | ADD   | Add the values of two registers.                                   |
| 0x06   | 3    | SUB   | Subtract the values of two registers.                              |
| 0x07   | 3    | MUL   | Multiply the values of two registers.                              |
| 0x08   | 3    | DIV   | Divide the values of two registers.                                |
| 0x09   | 3    | MOD   | Modulus the values of two registers.                               |
| 0x0a   | 2    | NOT   | Negate the value in a register.                                    |
| 0x0b   | 3    | AND   | Logically AND the values of two registers.                         |
| 0x0c   | 3    | OR    | Logically OR the values of two registers.                          |
| 0x0d   | 3    | XOR   | Logically XOR the values of two registers.                         |
| 0x0e   | 2    | SHIFT | Bitshift the value in register.                                    |
| 0x0f   | 3    | LT    | Check if the value of a register is smaller than another.          |
| 0x10   | 3    | LE    | Check if the value of a register is equal or smaller than another. |
| 0x11   | 3    | EQ    | Check if the value of a register is equal to another.              |
| 0x12   | 1    | PUSH  | Puhsh the value in a register onto the stack.                      |
| 0x13   | 1    | POP   | Pop the value in a register off the stack.                         |
| 0x14   | 1    | CALL  | Call a system call or dynamic function.                            |
| 0x15   | 2    | MOVE  | Move a value into a register.                                      |
| 0x16   | 1    | MOVEI | Move a value into a register.                                      |
| 0x17   | 3    | TYPE  | Cast the value of a register to another type.                      |
| 0x18   | 1    | EXIT  | Exit the program.                                                  |

#### `LOAD`, `LOADI` Instructions

```
LOAD    'A 'B             == 'A := at(uint('B))
LOADI   'A IMM:16         == 'A := at(IMM:16)
```

Loads the value from a memory address into a register 'A. When loading 16 or 32-bit
values, the lower 16/32 bits of a register is filled.

#### `JUMP`, `JMPR` Instructions

```
JUMP 0 IMM:16               == 'pc := 'pc + IMM:16
JUMP 1 IMM:16               == 'pc := 'pc - IMM:16
JUMP 2 IMM:16               == 'pc := 'pc + IMM:16 [if 'if > 0] ; Conditional jump
JUMP 3 IMM:16               == 'pc := 'pc - IMM:16 [if 'if > 0] ; Conditional jump

JMPR 0 'A                   == 'pc := uint('A)
JMPR 1 'A                   == 'pc := uint('A) [if 'if > 0] ; Conditional jump
```

`JUMP` jumps forward or back by the value of an immediate, treated as an
unsigned integer. `JMPR` jumps directly to the memory address stored in register 'A.

#### `ADD`, `SUB`, `MUL`, `DIV`, `MOD` Instructions

```
ADD 'A 'B 'C            == 'A := val('B) + val('C)
SUB 'A 'B 'C            == 'A := val('B) - val('C)
MUL 'A 'B 'C            == 'A := val('B) * val('C)
DIV A 'B 'C             == 'A := val('B) / val('C)
MOD 'A 'B 'C            == 'A := val('B) % val('C)
```

Performs arithmetic on target registers.
Performing arithmetic operations on values with different types
is undefined behaviour.

#### `NOT`, `AND`, `OR`, `XOR` Instructions

```
NOT 'A 'B                   == 'A := ~val('B)
AND 'A 'B 'C                == 'A := val('B) & val('C)
OR 'A 'B 'C                 == 'A := val('B) | val('C)
XOR 'A 'B 'C                == 'A := val('B) ^ val('C)
```

Performs logical operations on target registers.
All registers are read as raw bit values during these operations,
regardless of register type.

#### `LT`, `LE`, `EQ` Instructions

```
LT 'A 'B        == 'if := 1 if val('A) < val('B)
LE 'A 'B        == 'if := 1 if val('A) <= val('B)
LE 'A 'B        == 'if := 1 if val('A) == val('B)
```

Performs comparison operations on target registers.
Performing comparison operations on values with different types
is undefined behaviour.

#### `PUSH`, `POP` Instructions

```
PUSH 'A         == STACK := val('A), STACK increment
PUSH 'A         == 'A := at(STACK), STACK decrement
```

Performs stack manipulations. Type information is maintained during
transfer between the register and stack.

#### `CALL` Instruction

```
CALL 'A             == run func at('A)
```

Calls a system call or dynamic function loaded into a defined address at
register A. Arguments to these functions are provided by storing values on
the stack, and returned values on function exit must be stored on the stack.

Arguments must be pushed onto the stack in order:
A dynamic function `('D, 'E) = func('A, 'B, 'C)` is must be set up in this order:

```
PUSH 'A
PUSH 'B
PUSH 'C
CALL 'F     ; Assume function address loaded into 'F
POP 'D
POP 'E
```

#### `MOVE`, `MOVEI` Instructions

```
MOVE 'A 'B          == 'A := val('B)
MOVEI 0 'A IMM:16   == 'A := uint(IMM:16)         ; Inserted into the bottom 16-bits
MOVEI 1 'A IMM:16   == 'A := uint(IMM:16) << 4    ; Inserted into the lower-middle 16-bits
MOVEI 2 'A IMM:16   == 'A := uint(IMM:16) << 8    ; Inserted into the upper-middle 16-bits
MOVEI 3 'A IMM:16   == 'A := uint(IMM:16) << 12   ; Inserted into the upper 16-bits
```

```
    | imm:16 | imm:16 | imm:16 | imm:16 |
```

Copies an immediate or another register into a register.
`MOVE` instructions maintain register typing.
`MOVEI` instructions load bits into a register as unsigned ints. The type of the
register can be updated through a `TYPE` instruction.

#### `TYPE` Instructions

```
TYPE 0 'A 'B        == 'A := uint(val('B))      ; Read as unsigned 64-bit int
TYPE 1 'A 'B        == 'A := int(val('B))       ; Read as signed 64-bit int
TYPE 2 'A 'B        == 'A := float(val('B))     ; Read as 64-bit float
```

Copies the value of one register to another, reinterpreting the
underlying value as another type. It's important to note that the underlying
value is not changed, just the register type.

#### `EXIT` Instruction

```
EXIT
```

Immediately exits the program and returns the value stored in 'ec as the program 
exit code.
