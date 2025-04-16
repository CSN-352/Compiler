#ifndef CODEGEN_UTILS_H
#define CODEGEN_UTILS_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>

//=================== MIPS Register Definitions ===================//

enum MIPSRegister {
    ZERO, AT, V0, V1,
    A0, A1, A2, A3,
    T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
    S0, S1, S2, S3, S4, S5, S6, S7,
    GP, SP, FP, RA 
    // long long 
};

std::string get_mips_register_name(MIPSRegister reg);

//=================== MIPS Opcode Definitions ===================//

enum class MIPSOpcode {
    // Arithmetic
    ADD, ADDU, SUB, SUBU,
    MUL, MULT, MULTU, DIV, DIVU,

    // Logical
    AND, OR, XOR, NOR,

    // Shifts
    SLL, SRL, SRA,

    // Set
    SLT, SLTU,

    // Loads
    LW, LH, LHU, LB, LBU,
    LUI, LI,

    // Stores
    SW, SH, SB,

    // Moves
    MOVE, MFHI, MFLO, MTHI, MTLO,

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

extern std::unordered_map<MIPSRegister, std::unordered_set<std::string>> register_descriptor;
extern std::unordered_map<std::string, std::unordered_set<std::string>> address_descriptor;

void init_descriptors();

void update_for_load(MIPSRegister reg, const std::string& var);
void update_for_store(const std::string& var, MIPSRegister reg);
void update_for_add(const std::string& x, MIPSRegister rx);
void update_for_assign(const std::string& x, const std::string& y, MIPSRegister ry);

void clear_register(MIPSRegister reg);
void print_descriptors();

//=================== Leader Labels ===================//

extern std::unordered_map<int, std::string> leader_labels_map;
void set_leader_labels(const std::unordered_map<int, std::string>& leaders);

//=================== Register Allocation ===================//

MIPSRegister get_register_for_operand(const std::string& var, bool for_result = false);

void spill_register(MIPSRegister reg);

//=================== MIPS Instruction Struct ===================//

class MIPSInstruction {
    std::string label;         // Optional label
    MIPSOpcode opcode;         // Enum opcode
    MIPSRegister rd, rs, rt;   // Registers
    std::string immediate;     // Immediate value or offset (12- bits size allowed only)

    // Constructor for normal 3-reg instruction (e.g., add rd, rs, rt)
    MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, MIPSRegister src1, MIPSRegister src2);

    // Constructor for load/store: opcode rt, offset(rs)
    MIPSInstruction(MIPSOpcode opc, MIPSRegister reg, const std::string& offset, MIPSRegister base);

    // Constructor for immediate instructions (e.g., li, lui)
    MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, const std::string& imm);

    // Constructor for label-only instruction
    MIPSInstruction(const std::string& lbl);

    // Constructor for standalone ops like SYSCALL, NOP
    MIPSInstruction(MIPSOpcode opc);
};

//=================== MIPS Instruction Emission ===================//

extern std::vector<MIPSInstruction> mips_code;

void emit_instruction(const MIPSInstruction& instr);
void print_mips_code();

#endif // CODEGEN_UTILS_H
