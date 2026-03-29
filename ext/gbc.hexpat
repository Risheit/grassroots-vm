import std.mem;
import std.io;
import std.ptr;

#pragma endian little
#pragma magic [ F7 47 42 43 0A 0D 1A 0A ] @ 0x00

fn instr_type(u32 raw) {
    u8 opcode = (raw >> 26) & 0x3F;
    match (opcode) {
        (0x02 | 0x03 | 0x04 | 0x12 |
         0x13 | 0x14 | 0x16 | 0x18): return 1;
        (0x01 | 0x0a | 0x0e | 0x15): return 2;
        (_): return 3;
    }
};

namespace GBC {

struct Header {
    u8 ident[8];
    u16 major       [[name("Major Version")]];
    u16 minor       [[name("Minor Version")]];
    u16 patch       [[name("Patch Version")]];
    u16 end;

    u64 lt_off      [[name("Link Table Offset")]];
    u64 code_off    [[name("Code Section Offset")]];
    u64 type_off    [[name("Type Table Offset")]];
    u64 data_off    [[name("Data Section Offset")]];
};

struct LinkTableRow {
    u64 addr        [[name("Load Address")]];
    char func_id[];
};

struct LinkTable {
    u64 count;
    LinkTableRow entry[count];
};

enum Opcode : u8 {
    LOAD = 0x01,
    LOADI,
    JUMP,
    JMPR,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    NOT,
    AND,
    OR,
    XOR,
    SHIFT,
    LT,
    LE,
    EQ,
    PUSH,
    POP,
    CALL,
    MOVE,
    MOVEI,
    TYPE,
    EXIT
};

bitfield InstrType1 {
    immediate : 16;
    regA      : 8;
    func      : 2;
    Opcode opcode    : 6;
};

bitfield InstrType2 {
    immediate : 10;
    regB      : 8;
    regA      : 8;
    Opcode opcode    : 6;
};

bitfield InstrType3 {
    regC   : 8;
    regB   : 8;
    regA   : 8;
    func   : 2;
    Opcode opcode : 6;
};

struct Instruction {
    u32 raw [[hidden]];
    u16 t = instr_type(raw);
    if (t == 1)      InstrType1 instr @ addressof(raw);
    else if (t == 2) InstrType2 instr @ addressof(raw);
    else             InstrType3 instr @ addressof(raw);
};

struct Code {
    Instruction instr[while(std::mem::read_unsigned($, 4) != 0x00000000)];
    u32 code_end [[name("End Marker")]];
};

enum DataType : u8 {
    UINT = 0,
    INT,
    FLOAT
};

struct DataTypeRow {
    u8 size         [[name("Element Size (bytes)")]];
    DataType type;
    padding[4];
    u64 addr        [[name("Data Section Address")]];
};

struct DataTypeTable {
    u16 count;
    DataTypeRow entry[count];
};

struct Data {
    u8 data[while(!std::mem::eof())];
};

struct File {
    Header header;

    if (header.lt_off != 0)
        LinkTable link_table @ header.lt_off [[name("Link Table")]];

    Code code @ header.code_off [[name("Code Section")]];

    if (header.type_off != 0)
        DataTypeTable type_table @ header.type_off [[name("Data Type Table")]];

    if (header.data_off != 0)
        Data data @ header.data_off [[name("Data Section")]];
};

}

GBC::File file @ 0x00;


// -- Highlighting tools

// Highlights the relevant positions in the data array for each 

fn find_end(u64 address) {
    u64 end = std::ptr::relative_to_end(0);
    for (u16 i = 0, i < file.type_table.count, i += 1) {
        u64 other = file.type_table.entry[i].addr;
        if (other > address && other < end)
            end = other;
    }
    
    return end - address;
};

u16 row_count = 0;

struct DataEntryLoc {
    u8 value[find_end(file.type_table.entry[row_count].addr)] @ file.type_table.entry[row_count].addr;
    row_count += 1;
};

struct DataEntryMap {
    if (file.header.type_off != 0)
        DataEntryLoc entryLocs[while(row_count < file.type_table.count)];
};

DataEntryMap map @ file.header.data_off;