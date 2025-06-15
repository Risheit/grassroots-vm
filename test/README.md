This directory contains testing code and test .ga and .gbc files. 
A description of the test .ga and .gbc files are described here.

#### Glossary

| Terminology | Description |
|--|--|
| Clean | The process of putting a GA file through the first pass of the grassroots assembler: inlining macros and removing comments, empty lines, unneccessary whitespace, and other preprocessing work. GA files that are *cleaned* have already had this step performed on them |

---

## `add_two_clean.ga`

Contains cleaned GA code to store the two 16 bit numbers "65535" (0xFFFF), being written as a hexadecimal immediate, and "165" (0xA5), being written as a decimal immediate, into two 32-bit registers, add them into a third 32-bit register, then print the result of the addition "65700" (0x100A4) on screen using the system call `PRINT`.