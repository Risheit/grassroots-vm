Version 0.1.0

> [!WARNING]
> This sheet is currently under development.

# Intro

The purpose of this sheet is to document the formal specifications of the
64-bit Grassroots virtual machine (GVM).

# Motivations

The GVM is built to parse and run Grassroots bytecode (GBC).
It aims to be as lightweight as possible. It should not perform more work than
absolutely necessary, and avoid a large number of features. The GVM should be
able to handle a minimum word size of 32-bits (it should read at least 32 bits
in at a time from a GBC file).

# Running GBC

> [!IMPORTANT]
> Features to be implemented in future versions of the GVM are marked (TBI).

## Setup work

During setup, the following steps are necessary:

- Initialize registers.
- (TBI) Find and load dynamic functions from the link table.
- Load the code section and map it to virtual memory.
- Load the data section and map it to virtual memory.

## Code parsing

During code parsing, the following steps are necessary:

- Parse binary instruction into instruction structure.
- Choose the instruction based on its opcode.
- Run the instruction.

# Feature Set

## Paged Memory loads

The GBC code section should be segmented into pages, with code being loaded
in at page boundaries based on the location of the current instruction pointer.

As a performance feature, the GVM should maintain at minimum two code pages loaded,
the current and previous, to prevent a degradation of performance due to loops
between page boundaries.

Pages should be a minimum of 4096 bytes in size (128 instructions).

# System Calls

The GVM defines and maps several system calls to memory addresses that can
be run using the `CALL` GBC instruction. The following table summarizes the
current set of system calls.

The table defines the bottom 32-bits of each virtual address. All system
calls have the upper-most 16-bits set to 0x000F, and the upper-middle
16-bits set to 0.

| Definition                                    | Virtual Address | Description                       |
| --------------------------------------------- | --------------- | --------------------------------- |
| `void print(uint64_t size, const char \*str)` | 0x00000001      | Prints a string to standard out.  |
| `int64_t getint()`                            | 0x00000002      | Gets an integer from standard in. |
