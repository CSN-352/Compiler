#ifndef CODEGEN_UTILS_H
#define CODEGEN_UTILS_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <symbol_table.h>

// =================== Symbol Table ===================//

void initialize_global_symbol_table();
void insert_function_symbol_table(const string& function_name);
void erase_function_symbol_table(const string& function_name);

//=================== MIPS Register Definitions ===================//

enum MIPSRegister {
    ZERO, AT, V0, V1,
    A0, A1, A2, A3,
    T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
    S0, S1, S2, S3, S4, S5, S6, S7,
    GP, SP, FP, RA , HI, LO, 
    // Floating Point Registers
    F0, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31
};

std::string get_mips_register_name(MIPSRegister reg);

//=================== MIPS Opcode Definitions ===================//

enum MIPSOpcode {
    // Arithmetic
    ADD, ADDU, SUB, SUBU,
    MUL, MULU, MULT, MULTU, DIV, DIVU, ADD_S, ADD_D, SUB_S, SUB_D, MUL_S, MUL_D, DIV_S, DIV_D,

    // Logical
    AND, ANDI, OR, XOR, NOR,

    // Shifts
    SLL, SRL, SRA,

    // Set
    SLT, SLTU,

    // Loads
    LW, LH, LHU, LB, LBU,
    LUI, LI, LWC1, LDC1,

    // Stores
    SW, SH, SB, SWC1, SDC1,

    // Type Cnversions
    CVT_S_D, CVT_D_S, CVT_S_W, CVT_W_S, CVT_D_W, CVT_W_D,

    // Moves
    MOVE, MFHI, MFLO, MTHI, MTLO, MOVS, MOVD, MTC1, MFC1,

    // Branches
    BEQ, BNE, BGTZ, BLEZ, BLTZ, BGEZ,

    // Jumps
    J, JR, JAL, JALR,

    // Address
    LA,

    // Syscalls and pseudo
    SYSCALL, NOP, LABEL,

    // Fallback
    UNKNOWN
};

std::string get_opcode_name(MIPSOpcode opcode);

//=================== Descriptors ===================//

extern std::unordered_map<MIPSRegister, std::unordered_set<std::string> > register_descriptor;
extern std::unordered_map<std::string, std::unordered_set<std::string> > address_descriptor;

void init_descriptors();
bool check_if_variable_in_register(const std::string& var);

void update_for_load(MIPSRegister reg, const std::string& var);
void update_for_store(const std::string& var, MIPSRegister reg);
void update_for_add(const std::string& x, MIPSRegister rx);
void update_for_assign(const std::string& x, const std::string& y, MIPSRegister ry);

void clear_register(MIPSRegister reg);
void print_descriptors(); 

//=================== Leader Labels ===================//

extern std::unordered_map<int, std::string> leader_labels_map;
void set_leader_labels(vector<TACInstruction*> TAC_CODE);

//=================== Register Allocation ===================//

MIPSRegister get_register_for_operand(const string& var, bool for_result = false);
MIPSRegister get_float_register_for_operand(const string& var, bool for_result = false, bool is_double = false);

void spill_register(MIPSRegister reg);

//=================== MIPS Instruction Class ===================//

class MIPSInstruction {
public:
    std::string label;         // Optional label
    MIPSOpcode opcode;         // Enum opcode
    MIPSRegister rd, rs, rt;   // Registers
    std::string immediate;     // Immediate value or offset (12- bits size allowed only)

    // Constructor for normal 3-reg instruction (e.g., add rd, rs, rt)
    MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, MIPSRegister src1, MIPSRegister src2);

    // Constructor for load/store: opcode rt, offset(rs)
    MIPSInstruction(MIPSOpcode opc, MIPSRegister reg, const std::string& offset, MIPSRegister base);

    // Constructor for immediate instructions (e.g., li, lui, la)
    MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, const std::string& imm);

    // Constructor for immediate instructions with 2 registers (e.g., addi, andi, ori)
    MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, MIPSRegister src, const std::string& imm);

    // Constructor for move instructions (e.g., move rd, rs)
    MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, MIPSRegister src);

    // Constructor for mflo, mfhi instructions
    MIPSInstruction(MIPSOpcode opc, MIPSRegister dest);

    // Constructor for label-only instruction
    MIPSInstruction(const std::string& lbl);

    // Constructor for standalone ops like SYSCALL, NOP
    MIPSInstruction(MIPSOpcode opc);
};

//=================== MIPS Instruction Emission ===================//

extern std::vector<MIPSInstruction> mips_code_text;

void emit_instruction(string op, string dest, string src1, string src2);

//=================== MIPS Instruction Printing ===================//

vector<string> parameters_emit_instrcution(TACInstruction* instr);
void initalize_mips_code_vectors();
void print_mips_code();

//=================== MIPS Data Instruction Class ===================//

enum MIPSDirective {
    WORD, BYTE, HALF, FLOAT, DOUBLE, ASCIIZ, SPACE
};

string get_directive_name(MIPSDirective directive);

class MIPSDataInstruction {
    public:
        string label;         // Unique identifier for the data
        MIPSDirective directive;     // Directive (e.g., .word, .byte, .half, .float, .double, .asciiz)
        string value;         // Value to be stored in the data segment

        MIPSDataInstruction(const string& label, MIPSDirective dir, const string& val);
};

//=================== MIPS Data Storage ===================//

unordered_map<string, string> immediate_storage_map; // Immediate storage map
unordered_map<string, string> global_variable_storage_map; // Global variable storage map

extern std::vector<MIPSDataInstruction> mips_code_data; // Data segment

void store_immediate(const string& immediate, Type type);
bool check_immediate(const string& immediate);
void store_global_variable_data(const string& var, Type type, const string& value);
bool check_global_variable(const string& var);

#endif // CODEGEN_UTILS_H
