#include "codegen.h"
#include "tac.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
SymbolTable current_symbol_table; // Symbol Table for current scope (global scope + current function scope)

using namespace std;

//=================== Symbol Table ===================//
void initialize_global_symbol_table() {
    current_symbol_table = SymbolTable(); // Initialize the global symbol table
    for (const auto& entry : symbolTable.table){
        current_symbol_table.table.insert(entry); // Copy entries from the global symbol table
    }
    for(const auto& entry : symbolTable.defined_types){
        current_symbol_table.defined_types.insert(entry); // Copy entries from the global symbol table
    }
}

void insert_function_symbol_table(const string& function_name) {
    SymbolTable function_symbol_table = symbolTable.get_symbol_using_mangled_name(function_name)->function_definition->function_symbol_table;
    for(const auto& entry : function_symbol_table.table) {
        current_symbol_table.table[entry.first].insert(current_symbol_table.table[entry.first].begin(), entry.second.begin(), entry.second.end()); // Copy entries from the function symbol table
    }
}

void erase_function_symbol_table(const string& function_name) {
    SymbolTable function_symbol_table = symbolTable.get_symbol_using_mangled_name(function_name)->function_definition->function_symbol_table;
    for(const auto& entry : function_symbol_table.table) {
        for(auto sym:entry.second) {
            current_symbol_table.table[entry.first].erase(remove(current_symbol_table.table[entry.first].begin(), current_symbol_table.table[entry.first].end(), sym), current_symbol_table.table[entry.first].end()); 
        }
    }
}

//=================== MIPS Register Names ===================//

std::string get_mips_register_name(MIPSRegister reg) {
    switch (reg) {
        // Reserved
        case MIPSRegister::ZERO: return "$zero";
        case MIPSRegister::AT: return "$at";

        // Return values
        case MIPSRegister::V0: return "$v0";
        case MIPSRegister::V1: return "$v1";

        // Argument registers
        case MIPSRegister::A0: return "$a0";
        case MIPSRegister::A1: return "$a1";
        case MIPSRegister::A2: return "$a2";
        case MIPSRegister::A3: return "$a3";

        // Temporary registers
        case MIPSRegister::T0: return "$t0";
        case MIPSRegister::T1: return "$t1";
        case MIPSRegister::T2: return "$t2";
        case MIPSRegister::T3: return "$t3";
        case MIPSRegister::T4: return "$t4";
        case MIPSRegister::T5: return "$t5";
        case MIPSRegister::T6: return "$t6";
        case MIPSRegister::T7: return "$t7";
        case MIPSRegister::T8: return "$t8";
        case MIPSRegister::T9: return "$t9";

        // Saved registers
        case MIPSRegister::S0: return "$s0";
        case MIPSRegister::S1: return "$s1";
        case MIPSRegister::S2: return "$s2";
        case MIPSRegister::S3: return "$s3";
        case MIPSRegister::S4: return "$s4";
        case MIPSRegister::S5: return "$s5";
        case MIPSRegister::S6: return "$s6";
        case MIPSRegister::S7: return "$s7";

        // Special purpose
        case MIPSRegister::GP: return "$gp";
        case MIPSRegister::SP: return "$sp";
        case MIPSRegister::FP: return "$fp";
        case MIPSRegister::RA: return "$ra";
        case MIPSRegister::HI: return "$hi";
        case MIPSRegister::LO: return "$lo";

        // Floating Point Registers
        case MIPSRegister::F0: return "$f0";
        case MIPSRegister::F1: return "$f1";
        case MIPSRegister::F2: return "$f2";
        case MIPSRegister::F3: return "$f3";
        case MIPSRegister::F4: return "$f4";
        case MIPSRegister::F5: return "$f5";
        case MIPSRegister::F6: return "$f6";
        case MIPSRegister::F7: return "$f7";
        case MIPSRegister::F8: return "$f8";
        case MIPSRegister::F9: return "$f9";
        case MIPSRegister::F10: return "$f10";
        case MIPSRegister::F11: return "$f11";
        case MIPSRegister::F12: return "$f12";
        case MIPSRegister::F13: return "$f13";
        case MIPSRegister::F14: return "$f14";
        case MIPSRegister::F15: return "$f15";
        case MIPSRegister::F16: return "$f16";
        case MIPSRegister::F17: return "$f17";
        case MIPSRegister::F18: return "$f18";
        case MIPSRegister::F19: return "$f19";
        case MIPSRegister::F20: return "$f20";
        case MIPSRegister::F21: return "$f21";
        case MIPSRegister::F22: return "$f22";
        case MIPSRegister::F23: return "$f23";
        case MIPSRegister::F24: return "$f24";
        case MIPSRegister::F25: return "$f25";
        case MIPSRegister::F26: return "$f26";
        case MIPSRegister::F27: return "$f27";
        case MIPSRegister::F28: return "$f28";
        case MIPSRegister::F29: return "$f29";
        case MIPSRegister::F30: return "$f30";
        case MIPSRegister::F31: return "$f31";

        // Fallback
        default: return "$invalid";
    }
}


//=================== MIPS Opcode Names ===================//

std::string get_opcode_name(MIPSOpcode opcode) {
    switch (opcode) {
        case MIPSOpcode::ADD:      return "ADD";
        case MIPSOpcode::ADDU:     return "ADDU";
        case MIPSOpcode::ADDIU:    return "ADDIU";
        case MIPSOpcode::ADD_S:    return "ADD.S";
        case MIPSOpcode::ADD_D:    return "ADD.D";
        case MIPSOpcode::SUB:      return "SUB";
        case MIPSOpcode::SUBU:     return "SUBU";
        case MIPSOpcode::SUBIU:    return "SUBIU";
        case MIPSOpcode::SUB_S:    return "SUB.S";
        case MIPSOpcode::SUB_D:    return "SUB.D";
        case MIPSOpcode::MUL:      return "MUL";
        case MIPSOpcode::MULT:     return "MULT";
        case MIPSOpcode::MULTU:    return "MULTU";
        case MIPSOpcode::DIV:      return "DIV";
        case MIPSOpcode::DIVU:     return "DIVU";
        case MIPSOpcode::AND:      return "AND";
        case MIPSOpcode::OR:       return "OR";
        case MIPSOpcode::XOR:      return "XOR";
        case MIPSOpcode::NOR:      return "NOR";
        case MIPSOpcode::SLL:      return "SLL";
        case MIPSOpcode::SRL:      return "SRL";
        case MIPSOpcode::SRA:      return "SRA";
        case MIPSOpcode::SLLV:     return "SLLV";
        case MIPSOpcode::SRLV:     return "SRLV";
        case MIPSOpcode::SRAV:     return "SRAV";
        case MIPSOpcode::ANDI:     return "ANDI";
        case MIPSOpcode::NEG:      return "NEG";
        case MIPSOpcode::NEG_S:    return "NEG.S";
        case MIPSOpcode::NEG_D:    return "NEG.D";
        case MIPSOpcode::SLT:      return "SLT";
        case MIPSOpcode::SLTU:     return "SLTU";
        case MIPSOpcode::LW:       return "LW";
        case MIPSOpcode::LH:       return "LH";
        case MIPSOpcode::LHU:      return "LHU";
        case MIPSOpcode::LB:       return "LB";
        case MIPSOpcode::LBU:      return "LBU";
        case MIPSOpcode::LUI:      return "LUI";
        case MIPSOpcode::LI:       return "LI";
        case MIPSOpcode::LWC1:     return "LWC1";
        case MIPSOpcode::LDC1:     return "LDC1";
        case MIPSOpcode::SW:       return "SW";
        case MIPSOpcode::SH:       return "SH";
        case MIPSOpcode::SB:       return "SB";
        case MIPSOpcode::SWC1:     return "SWC1";
        case MIPSOpcode::SDC1:     return "SDC1";
        case MIPSOpcode::CVT_S_D:  return "CVT.S.D";
        case MIPSOpcode::CVT_D_S:  return "CVT.D.S";
        case MIPSOpcode::CVT_S_W:  return "CVT.S.W";
        case MIPSOpcode::CVT_W_S:  return "CVT.W.S";
        case MIPSOpcode::CVT_D_W:  return "CVT.D.W";
        case MIPSOpcode::CVT_W_D:  return "CVT.W.D";
        case MIPSOpcode::MOVE:     return "MOVE";
        case MIPSOpcode::MFHI:     return "MFHI";
        case MIPSOpcode::MFLO:     return "MFLO";
        case MIPSOpcode::MTHI:     return "MTHI";
        case MIPSOpcode::MTLO:     return "MTLO";
        case MIPSOpcode::MOVS:     return "MOV.S";
        case MIPSOpcode::MOVD:     return "MOV.D";
        case MIPSOpcode::MTC1:     return "MTC1";
        case MIPSOpcode::MFC1:     return "MFC1";
        case MIPSOpcode::BEQ:      return "BEQ";
        case MIPSOpcode::BNE:      return "BNE";
        case MIPSOpcode::BGTZ:     return "BGTZ";
        case MIPSOpcode::BLEZ:     return "BLEZ";
        case MIPSOpcode::BLTZ:     return "BLTZ";
        case MIPSOpcode::BGEZ:     return "BGEZ";
        case MIPSOpcode::J:        return "J";
        case MIPSOpcode::JR:       return "JR";
        case MIPSOpcode::JAL:      return "JAL";
        case MIPSOpcode::JALR:     return "JALR";
        case MIPSOpcode::LA:       return "LA";
        case MIPSOpcode::SYSCALL:  return "SYSCALL";
        case MIPSOpcode::NOP:      return "NOP";
        case MIPSOpcode::LABEL:    return "LABEL";
        default:                   return "UNKNOWN";
    }
}

//=================== Global Descriptors ===================//

std::unordered_map<MIPSRegister, std::unordered_set<std::string>> register_descriptor;
std::unordered_map<std::string, std::unordered_set<std::string>> address_descriptor;


//=================== Descriptor Functions ===================//

void init_descriptors() {
    register_descriptor.clear();
    address_descriptor.clear();
}

bool check_if_variable_in_register(const std::string& var){
    for (const auto& [reg, vars] : register_descriptor) {
        if (vars.count(var)){
            if(address_descriptor[var].count(get_mips_register_name(reg)))
                return true;
        }
    }
    return false;
}

// Aaditya complete implementation with is_double

void update_for_load(MIPSRegister reg, const std::string& var, bool is_double) {
    register_descriptor[reg].clear();
    register_descriptor[reg].insert(var);
    address_descriptor[var].insert(get_mips_register_name(reg));

    for (auto& [v, locs] : address_descriptor) {
        if (v != var) locs.erase(get_mips_register_name(reg));
    }
}
 
void update_for_store(const std::string& var, MIPSRegister reg, bool is_double) {
    address_descriptor[var].insert(get_mips_register_name(reg));
    address_descriptor[var].insert("mem");
}

void update_for_add(const std::string& x, MIPSRegister rx, bool is_double) {
    register_descriptor[rx].clear();
    register_descriptor[rx].insert(x);
    address_descriptor[x].clear();
    address_descriptor[x].insert(get_mips_register_name(rx));

    for (auto& [v, locs] : address_descriptor) {
        if (v != x) locs.erase(get_mips_register_name(rx));
    }
}

// x = y
void update_for_assign(const std::string& x, const std::string& y, MIPSRegister ry, bool is_double) {
    register_descriptor[ry].insert(x);
    address_descriptor[x].clear();
    address_descriptor[x].insert(get_mips_register_name(ry));
}

void clear_register(MIPSRegister reg) {
    register_descriptor[reg].clear();
    for (auto& [var, locs] : address_descriptor) {
        locs.erase(get_mips_register_name(reg));
    }
}

void print_descriptors() {
    std::cout << "=== Register Descriptor ===\n";
    for (auto& [reg, vars] : register_descriptor) {
        std::cout << get_mips_register_name(reg) << ": ";
        for (const auto& v : vars) std::cout << v << " ";
        std::cout << "\n";
    }

    std::cout << "\n=== Address Descriptor ===\n";
    for (auto& [var, locs] : address_descriptor) {
        std::cout << var << ": ";
        for (const auto& l : locs) std::cout << l << " ";
        std::cout << "\n";
    }
}
//=================== Leader Detection ===================//

std::unordered_map<int, std::string> leader_labels_map;
// to be changed.
void set_leader_labels() {
    std::unordered_set<int> leader_lines;
    // std::vector<std::pair<int, std::string>> instructions;

    if(TAC_CODE.empty()) return;

    leader_lines.insert(0);

    for(int instr_no = 0; instr_no<TAC_CODE.size();instr_no++){
        TACInstruction* instr = TAC_CODE[instr_no];
        if(instr->flag == 1){
            std::string label = get_operand_string(instr->result);
            int num = std::stoi(label.substr(1));
            leader_lines.insert(num);
            if(instr_no + 1 < TAC_CODE.size()) leader_lines.insert(num+1);
        } else if(instr->flag == 2){
            std::string label = get_operand_string(instr->result);
            int num = std::stoi(label.substr(1));
            leader_lines.insert(num);
        } else if(instr->op.type == TAC_OPERATOR_FUNC_BEGIN){
            if(instr_no + 1 < TAC_CODE.size()) leader_lines.insert(instr_no+1);
        }
    }

    int label_counter = 0;
    for (int line_num : leader_lines) {
        leader_labels_map[line_num] = "L" + std::to_string(label_counter++);
    }

    return;
}


//=================== Register Management ===================//

void spill_register(MIPSRegister reg) {
    // Emit store instruction (ST) for each variable in the register
    for (const auto& v : register_descriptor[reg]) {
        if(address_descriptor[v].count("mem")){
            continue;
        } else {
            emit_instruction("store",v,v,"");
            if(address_descriptor[v].count(get_mips_register_name(reg))){
                address_descriptor[v].erase(get_mips_register_name(reg));
            }
            address_descriptor[v].insert("mem");
        }
    }
    // Clear the register descriptor after spilling all its variables
    register_descriptor[reg].clear();
}

// Modify this so that register to be spilled is chosen cyclicly
MIPSRegister get_register_for_operand(const std::string& var,bool for_result) {

    // 1. Already in a register
    if(var.size()>0 && (var[0]=='#' || var[0]=='0') && !for_result){
        for (const auto& [reg, vars] : register_descriptor) {
            if (vars.count(var)){
                if(address_descriptor[var].count(get_mips_register_name(reg)))
                    return reg;
            } 
        }
    } else if((var[0]=='#' || var[0]=='0') && for_result){
        for (const auto& [reg, vars] : register_descriptor) {
            if (vars.count(var) && vars.size() == 1){
                if(address_descriptor[var].count(get_mips_register_name(reg)))
                    return reg;
            } 
        }
    }
        // 2. Empty register
        for (int r = T0; r <= T9; ++r) {
            MIPSRegister reg = static_cast<MIPSRegister>(r);
            if (register_descriptor[reg].empty()) return reg;
        }

        // 3. Reuse a register if all variables in it are also in memory
        for (int r = T0; r <= T9; ++r) {
            MIPSRegister reg = static_cast<MIPSRegister>(r);
            bool all_vars_safe = true;

            for (const auto& v : register_descriptor[reg]) {
                const auto& locs = address_descriptor[v];
                if (!locs.count("mem")) {
                    all_vars_safe = false;
                    break;
                }
            }

            if (all_vars_safe) {
                // Clean up: remove those vars from register_descriptor and address_descriptor
                for (const auto& v : register_descriptor[reg]) {
                    address_descriptor[v].erase(get_mips_register_name(reg));
                }
                register_descriptor[reg].clear();
                return reg;
            }
        }

        // 4. Reuse a register if it holds only the result variable itself (var == v)

        // 5. Spill case: Choose a register to spill if no safe register is available
        // Maintain a cyclic count for getting spill register
        static int reg_index = 0;
        static const std::vector<MIPSRegister> allocatableRegs = {
            T0, T1, T2, T3, T4, T5, T6, T7, T8, T9
        };

        MIPSRegister spill_reg = allocatableRegs[reg_index];
        reg_index = (reg_index + 1) % allocatableRegs.size();
        
        // Call the spill_register function to handle the spilling
        spill_register(spill_reg);

        // Return the spill register (T0 in this case) after spilling
        return spill_reg;
}

MIPSRegister get_float_register_for_operand(const std::string& var, bool for_result, bool is_double) {
    static int float_index = 0;
    static int double_index = 0;

    // Float: odd-numbered only
    static const std::vector<MIPSRegister> float_regs = {
        F1, F3, F5, F7, F9, F11, F13, F15,
        F17, F19, F21, F23, F25, F27, F29, F31
    };

    // Double: even-numbered only (each one implies paired reg: fN and fN+1)
    static const std::vector<MIPSRegister> double_regs = {
        F0, F2, F4, F6, F8, F10, F12, F14,
        F16, F18, F20, F22, F24, F26, F28, F30
    };

    const std::vector<MIPSRegister>& regs = is_double ? double_regs : float_regs;
    int& index = is_double ? double_index : float_index;

//     // 1. Already in a register
//     if(var.size()>0 && (var[0]=='#' || var[0]=='0') && !for_result){
//         for (const auto& [reg, vars] : register_descriptor) {
//             if (vars.count(var)) {
//                 if (address_descriptor[var].count(get_mips_register_name(reg))) {
//                     return reg;
//                 }
//             }
//         }
//     } else if((var[0]=='#' || var[0]=='0') && for_result){
//         for (const auto& [reg, vars] : register_descriptor) {
//             if (vars.count(var) && vars.size() == 1){
//                 if(address_descriptor[var].count(get_mips_register_name(reg)))
//                     return reg;
//             } 
//         }
//     }

//     // 2. Find empty register
//     for (MIPSRegister reg : regs) {
//         if (is_double) {
//             MIPSRegister next = static_cast<MIPSRegister>(static_cast<int>(reg) + 1);
//             if (register_descriptor[reg].empty() && register_descriptor[next].empty()) {
//                 return reg;
//             }
//         } else {
//             if (register_descriptor[reg].empty()) {
//                 return reg;
//             }
//         }
//     }

//     // 3. Reuse register if all vars are in memory
//     for (MIPSRegister reg : regs) {
//         bool safe = true;

//         if (is_double) {
//             MIPSRegister next = static_cast<MIPSRegister>(static_cast<int>(reg) + 1);
//             for (const auto& v : register_descriptor[reg]) {
//                 if (!address_descriptor[v].count("mem")) {
//                     safe = false;
//                     break;
//                 }
//             }
//             for (const auto& v : register_descriptor[next]) {
//                 if (!address_descriptor[v].count("mem")) {
//                     safe = false;
//                     break;
//                 }
//             }

//             if (safe) {
//                 for (const auto& v : register_descriptor[reg]) {
//                     address_descriptor[v].erase(get_mips_register_name(reg));
//                 }
//                 register_descriptor[reg].clear();

//                 for (const auto& v : register_descriptor[next]) {
//                     address_descriptor[v].erase(get_mips_register_name(next));
//                 }
//                 register_descriptor[next].clear();

//                 return reg;
//             }
//         } else {
//             for (const auto& v : register_descriptor[reg]) {
//                 if (!address_descriptor[v].count("mem")) {
//                     safe = false;
//                     break;
//                 }
//             }

//             if (safe) {
//                 for (const auto& v : register_descriptor[reg]) {
//                     address_descriptor[v].erase(get_mips_register_name(reg));
//                 }
//                 register_descriptor[reg].clear();
//                 return reg;
//             }
//         }
//     }

//     // 4. Spill cyclically
    MIPSRegister spill_reg = regs[index];
    index = (index + 1) % regs.size();

    if (is_double) {
        MIPSRegister next = static_cast<MIPSRegister>(static_cast<int>(spill_reg) + 1);
        spill_register(spill_reg);
        spill_register(next);
    } else {
        spill_register(spill_reg);
    }

    return spill_reg;
}


//=================== MIPS Instruction Class ===================//

// Constructor for normal 3-reg instruction (e.g., add dest_reg, src1_reg, src2_reg)
MIPSInstruction::MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, MIPSRegister src1, MIPSRegister src2)
    : opcode(opc), dest_reg(dest), src1_reg(src1), src2_reg(src2), immediate(""), label(""), instruction_type(_3_REG_TYPE) {}

// Constructor for load/store: opcode src2_reg, offset(src1_reg)
MIPSInstruction::MIPSInstruction(MIPSOpcode opc, MIPSRegister reg, const std::string& offset, MIPSRegister base)
    : opcode(opc), src2_reg(reg), immediate(offset), src1_reg(base), dest_reg(MIPSRegister::ZERO), label(""), instruction_type(_2_REG_OFFSET_TYPE) {}

// Constructor for immediate instructions (e.g., li, lui)
MIPSInstruction::MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, const std::string& imm)
    : opcode(opc), dest_reg(dest), immediate(imm), src1_reg(MIPSRegister::ZERO), src2_reg(MIPSRegister::ZERO), label(""), instruction_type(_1_REG_TYPE_IMMEDIATE) {}

// Constructor for immediate instructions with 2 registers (e.g., addi, andi, ori)
MIPSInstruction::MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, MIPSRegister src, const std::string& imm)
    : opcode(opc), dest_reg(dest), src1_reg(src), immediate(imm), src2_reg(MIPSRegister::ZERO), label(""), instruction_type(_2_REG_IMMEDIATE_TYPE) {}

// Constructor for move instructions (e.g., move dest_reg, src1_reg)
MIPSInstruction::MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, MIPSRegister src)
    : opcode(opc), dest_reg(dest), src1_reg(src), src2_reg(MIPSRegister::ZERO), immediate(""), label(""), instruction_type(_2_REG_TYPE) {}

// Constructor for mflo, mfhi instructions
MIPSInstruction::MIPSInstruction(MIPSOpcode opc, MIPSRegister dest)
    : opcode(opc), dest_reg(dest) , src1_reg(MIPSRegister::ZERO), src2_reg(MIPSRegister::ZERO), immediate(""), label(""), instruction_type(_1_REG_TYPE) {}

// Constructor for label-only instruction
MIPSInstruction::MIPSInstruction(const std::string& lbl)
    : label(lbl), opcode(MIPSOpcode::LABEL), dest_reg(MIPSRegister::ZERO), src1_reg(MIPSRegister::ZERO), src2_reg(MIPSRegister::ZERO), immediate(""), instruction_type(_LABEL_TYPE) {}

// Constructor for standalone ops like SYSCALL, NOP
MIPSInstruction::MIPSInstruction(MIPSOpcode opc)
    : opcode(opc), dest_reg(MIPSRegister::ZERO), src1_reg(MIPSRegister::ZERO), src2_reg(MIPSRegister::ZERO), immediate(""), label(""), instruction_type(_NOP_TYPE) {}

// ===================== Global Variable Storage ===================//

std::vector<MIPSDataInstruction> mips_code_data;

// helper function for character to ascii conversion
char decode_char_literal(const std::string& val) {
    if (val.length() == 3) {
        // Example: 'a'
        return val[1];
    } 
    else if (val.length() == 4 && val[1] == '\\') {
        switch (val[2]) {
            case 'a': return '\a';   // Bell (alert)
            case 'b': return '\b';   // Backspace
            case 'f': return '\f';   // Form feed
            case 'n': return '\n';   // Newline
            case 'r': return '\r';   // Carriage return
            case 't': return '\t';   // Horizontal tab
            case 'v': return '\v';   // Vertical tab
            case '\\': return '\\'; // Backslash
            case '\'': return '\''; // Single quote
            case '\"': return '\"'; // Double quote
            case '?': return '\?';  // Question mark
            case '0': return '\0';  // Null character
            default: return val[1]; // Default case
        }
    }
    return val[1]; // Default case for other characters
}

void store_global_variable_data(const string& var, Type type, const string& value) {
    if(value == "0"){
        global_variable_storage_map[var] = var;
        MIPSDataInstruction data_instr(var, MIPSDirective::SPACE, to_string(type.get_size())); // space for zero initialization
        mips_code_data.push_back(data_instr);
        return;
    }
    if(type.type_index == PrimitiveTypes::U_CHAR_T || type.type_index == PrimitiveTypes::CHAR_T){
        if(type.ptr_level == 0){
            global_variable_storage_map[var] = var;
            string value_ascii = to_string((int)(decode_char_literal(value))); // Convert char to ASCII value
            MIPSDataInstruction data_instr(var, MIPSDirective::BYTE, value_ascii); // char
            mips_code_data.push_back(data_instr);
        }
        else if(type.ptr_level == 1){
            global_variable_storage_map[var] = var;
            MIPSDataInstruction data_instr(var, MIPSDirective::ASCIIZ, value); // string
            mips_code_data.push_back(data_instr);
        }
    }
    else if(type.type_index == PrimitiveTypes::U_SHORT_T || type.type_index == PrimitiveTypes::SHORT_T){
        global_variable_storage_map[var] = var;
        MIPSDataInstruction data_instr(var, MIPSDirective::HALF, value); // short
        mips_code_data.push_back(data_instr);
    }
    else if(type.type_index >= PrimitiveTypes::U_INT_T && type.type_index <= PrimitiveTypes::LONG_T){
        global_variable_storage_map[var] = var;
        MIPSDataInstruction data_instr(var, MIPSDirective::WORD, value); // int
        mips_code_data.push_back(data_instr);
    }
    else if(type.type_index == PrimitiveTypes::U_LONG_LONG_T || type.type_index == PrimitiveTypes::LONG_LONG_T){
        string value_hi = to_string(stoll(value) >> 32);
        string value_lo = to_string(stoll(value) & 0xFFFFFFFF);
        global_variable_storage_map[var+"_hi"] = var + "_hi";
        global_variable_storage_map[var+"_lo"] = var + "_lo";
        // Store the upper and lower 32 bits of the long long value separately
        MIPSDataInstruction data_instr1(var + "_hi", MIPSDirective::WORD, value_hi); // upper 32 bits of long long
        MIPSDataInstruction data_instr2(var + "_lo", MIPSDirective::WORD, value_lo); // lower 32 bits of long long
        mips_code_data.push_back(data_instr1);
        mips_code_data.push_back(data_instr2);
    }
    else if(type.type_index == PrimitiveTypes::FLOAT_T){
        global_variable_storage_map[var] = var;
        MIPSDataInstruction data_instr(var, MIPSDirective::FLOAT, value); // float
        mips_code_data.push_back(data_instr);
    }
    else if(type.type_index == PrimitiveTypes::DOUBLE_T || type.type_index == PrimitiveTypes::LONG_DOUBLE_T){
        global_variable_storage_map[var] = var;
        MIPSDataInstruction data_instr(var, MIPSDirective::DOUBLE, value); // double
        mips_code_data.push_back(data_instr);
    }
}
    
bool check_global_variable(const string& var) {
    if(global_variable_storage_map.find(var) == global_variable_storage_map.end()){
        return false;
    }
    return true;
}

// ===================== MIPS Data Instruction Class ===================//

string get_directive_name(MIPSDirective directive) {
    switch (directive) {
        case MIPSDirective::WORD: return ".word";
        case MIPSDirective::BYTE: return ".byte";
        case MIPSDirective::HALF: return ".half";
        case MIPSDirective::FLOAT: return ".float";
        case MIPSDirective::DOUBLE: return ".double";
        case MIPSDirective::ASCIIZ: return ".asciiz";
        case MIPSDirective::SPACE: return ".space";
        default: return "UNKNOWN_DIRECTIVE";
    }
}

MIPSDataInstruction::MIPSDataInstruction(const string& label, MIPSDirective dir, const string& val)
    : label(label), directive(dir), value(val) {}

// ===================== MIPS Data Storage ===================//

void store_immediate(const string& immediate, Type type) {
    if(type.type_index == PrimitiveTypes::U_CHAR_T || type.type_index == PrimitiveTypes::CHAR_T){
        if(type.ptr_level == 0){
            immediate_storage_map[immediate] = "immediate_" + to_string(immediate_storage_map.size());
            string immediate_ascii = to_string((int)(immediate[0])); // Convert char to ASCII value
            MIPSDataInstruction data_instr(immediate_storage_map[immediate], MIPSDirective::BYTE, immediate_ascii); // char
            mips_code_data.push_back(data_instr);
        }
        else if(type.ptr_level == 1){
            immediate_storage_map[immediate] = "immediate_" + to_string(immediate_storage_map.size());
            MIPSDataInstruction data_instr(immediate_storage_map[immediate], MIPSDirective::ASCIIZ, immediate); // string
            mips_code_data.push_back(data_instr);
        }
    }
    else if(type.type_index == PrimitiveTypes::U_SHORT_T || type.type_index == PrimitiveTypes::SHORT_T){
        immediate_storage_map[immediate] = "immediate_" + to_string(immediate_storage_map.size());
        MIPSDataInstruction data_instr(immediate_storage_map[immediate], MIPSDirective::HALF, immediate); // short
        mips_code_data.push_back(data_instr);
    }
    else if(type.type_index >= PrimitiveTypes::U_INT_T && type.type_index <= PrimitiveTypes::LONG_T){
        immediate_storage_map[immediate] = "immediate_" + to_string(immediate_storage_map.size());
        MIPSDataInstruction data_instr(immediate_storage_map[immediate], MIPSDirective::WORD, immediate); // int
        mips_code_data.push_back(data_instr);
    }
    else if(type.type_index == PrimitiveTypes::U_LONG_LONG_T || type.type_index == PrimitiveTypes::LONG_LONG_T){
        string immediate_hi = to_string(stoll(immediate) >> 32);
        string immediate_lo = to_string(stoll(immediate) & 0xFFFFFFFF);
        immediate_storage_map[immediate_hi] = "immediate_" + to_string(immediate_storage_map.size()) + "_hi";
        immediate_storage_map[immediate_lo] = "immediate_" + to_string(immediate_storage_map.size()) + "_lo";
        // Store the upper and lower 32 bits of the long long value separately
        MIPSDataInstruction data_instr1(immediate_storage_map[immediate_hi], MIPSDirective::WORD, immediate_hi); // upper 32 bits of long long
        MIPSDataInstruction data_instr2(immediate_storage_map[immediate_lo], MIPSDirective::WORD, immediate_lo); // lower 32 bits of long long
        mips_code_data.push_back(data_instr1);
        mips_code_data.push_back(data_instr2);
    }
    else if(type.type_index == PrimitiveTypes::FLOAT_T){
        immediate_storage_map[immediate] = "immediate_" + to_string(immediate_storage_map.size());
        MIPSDataInstruction data_instr(immediate_storage_map[immediate], MIPSDirective::FLOAT, immediate); // float
        mips_code_data.push_back(data_instr);
    }
    else if(type.type_index == PrimitiveTypes::DOUBLE_T || type.type_index == PrimitiveTypes::LONG_DOUBLE_T){
        immediate_storage_map[immediate] = "immediate_" + to_string(immediate_storage_map.size());
        MIPSDataInstruction data_instr(immediate_storage_map[immediate], MIPSDirective::DOUBLE, immediate); // double
        mips_code_data.push_back(data_instr);
    }
}

bool check_immediate(const string& immediate) {
    if(immediate_storage_map.find(immediate) == immediate_storage_map.end()){
        return false;
    }
    return true;
}

//=================== MIPS Instruction Emission ===================//

std::vector<MIPSInstruction> mips_code_text;

void emit_instruction(string op, string dest, string src1, string src2){
    Symbol* dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
    Symbol* src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
    Symbol* src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);
    if(dest_sym == nullptr) return;
    if(op == "load"){ // assignment instruction
        if(dest_sym->scope == 0){ // global variable
            if(!check_global_variable(dest)){ // global variable initialization
                store_global_variable_data(dest, dest_sym->type, src1); // initialized global variable
                return;
            }
        }
        else {
            // local stack variable initialization
        }
        int size = dest_sym->type.get_size();
        if(check_if_variable_in_register(src1)){
            // No instruction needed, only change register descriptor and address descriptor
            update_for_assign(dest, src1, get_register_for_operand(src1));
        }
        else if(src1_sym != nullptr && src1_sym->scope == 0){// global variable
            // Load variable from memory
            emit_instruction("la", "addr", src1, ""); // Load address of src1
            MIPSRegister addr_reg = get_register_for_operand("addr"); // Get a register for the address

            if(dest_sym->type.type_index == PrimitiveTypes::U_CHAR_T){
                MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LBU, dest_reg, "0", addr_reg); // Load byte from memory
                mips_code_text.push_back(load_instr); // Emit load instruction
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::CHAR_T){
                MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LB, dest_reg, "0", addr_reg); // Load byte from memory
                mips_code_text.push_back(load_instr); // Emit load instruction
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::U_SHORT_T){
                MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LHU, dest_reg, "0", addr_reg); // Load halfword from memory
                mips_code_text.push_back(load_instr); // Emit load instruction
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::SHORT_T){
                MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LH, dest_reg, "0", addr_reg); // Load halfword from memory
                mips_code_text.push_back(load_instr); // Emit load instruction
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index >= PrimitiveTypes::U_INT_T && dest_sym->type.type_index <= PrimitiveTypes::LONG_T){
                MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LW, dest_reg, "0", addr_reg); // Load word from memory
                mips_code_text.push_back(load_instr); // Emit load instruction
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::U_LONG_LONG_T || dest_sym->type.type_index == PrimitiveTypes::LONG_LONG_T){
                MIPSRegister dest_reg_hi = get_register_for_operand(dest+"_hi", true); // Get a register for the destination
                MIPSRegister dest_reg_lo = get_register_for_operand(dest+"_lo", true); // Get a register for the destination
                MIPSInstruction load_instr_hi(MIPSOpcode::LW, dest_reg_hi, "0", addr_reg); // Load upper 32 bits of long long from memory
                MIPSInstruction load_instr_lo(MIPSOpcode::LW, dest_reg_lo, "4", addr_reg); // Load lower 32 bits of long long from memory
                mips_code_text.push_back(load_instr_hi); // Emit load instruction for upper 32 bits
                mips_code_text.push_back(load_instr_lo); // Emit load instruction for lower 32 bits
                update_for_load(dest_reg_hi, dest+"_hi"); // Update register descriptor and address descriptor
                update_for_load(dest_reg_lo, dest+"_lo"); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T){
                MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LWC1, dest_reg, "0", addr_reg); // Load float from memory
                mips_code_text.push_back(load_instr); // Emit load instruction for float
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T|| dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T){
                MIPSRegister dest_reg = get_float_register_for_operand(dest, true, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LDC1, dest_reg, "0", addr_reg); // Load long double from memory
                mips_code_text.push_back(load_instr); // Emit load instruction for long double
                update_for_load(dest_reg, dest, true); // Update register descriptor and address descriptor
            }
        }
        else if(src1_sym != nullptr){// local stack variable
            // to be implemented
        }
        else{
            // Load large immediate value
            if(!check_immediate(src1)){
                store_immediate(src1, dest_sym->type); // Store immediate value in immediate storage map
            }
            emit_instruction("la", "addr", dest, ""); // Load address of dest
            MIPSRegister addr_reg = get_register_for_operand("addr"); // Get a register for the address

            if(dest_sym->type.type_index == PrimitiveTypes::U_CHAR_T){
                MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LBU, dest_reg, "0", addr_reg); // Load byte from memory
                mips_code_text.push_back(load_instr); // Emit load instruction
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::CHAR_T){
                MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LB, dest_reg, "0", addr_reg); // Load byte from memory
                mips_code_text.push_back(load_instr); // Emit load instruction
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::U_SHORT_T){
                MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LHU, dest_reg, "0", addr_reg); // Load halfword from memory
                mips_code_text.push_back(load_instr); // Emit load instruction
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::SHORT_T){
                MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LH, dest_reg, "0", addr_reg); // Load halfword from memory
                mips_code_text.push_back(load_instr); // Emit load instruction
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index >= PrimitiveTypes::U_INT_T && dest_sym->type.type_index <= PrimitiveTypes::LONG_T){
                MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LW, dest_reg, "0", addr_reg); // Load word from memory
                mips_code_text.push_back(load_instr); // Emit load instruction
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::U_LONG_LONG_T || dest_sym->type.type_index == PrimitiveTypes::LONG_LONG_T){
                MIPSRegister dest_reg_hi = get_register_for_operand(dest+"_hi", true); // Get a register for the destination
                MIPSRegister dest_reg_lo = get_register_for_operand(dest+"_lo", true); // Get a register for the destination
                MIPSInstruction load_instr_hi(MIPSOpcode::LW, dest_reg_hi, "0", addr_reg); // Load upper 32 bits of long long from memory
                MIPSInstruction load_instr_lo(MIPSOpcode::LW, dest_reg_lo, "4", addr_reg); // Load lower 32 bits of long long from memory
                mips_code_text.push_back(load_instr_hi); // Emit load instruction for upper 32 bits
                mips_code_text.push_back(load_instr_lo); // Emit load instruction for lower 32 bits
                update_for_load(dest_reg_hi, dest+"_hi"); // Update register descriptor and address descriptor
                update_for_load(dest_reg_lo, dest+"_lo"); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T){
                MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LWC1, dest_reg, "0", addr_reg); // Load float from memory
                mips_code_text.push_back(load_instr); // Emit load instruction for float
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T || dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T){
                MIPSRegister dest_reg = get_float_register_for_operand(dest, true, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LDC1, dest_reg, "0", addr_reg); // Load long double from memory
                mips_code_text.push_back(load_instr); // Emit load instruction for long double
                update_for_load(dest_reg, dest, true); // Update register descriptor and address descriptor
            }
        }
    }
    else if(op == "li"){ // load immediate instruction
       
    }
    else if(op == "la"){ // load address instruction
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        if(src1_sym->scope == 0){ // global variable
            MIPSRegister addr_reg = get_register_for_operand(dest, true); // Get a register for the address
            MIPSInstruction load_addr_instr(MIPSOpcode::LA, addr_reg, src1); // Load address of dest
            mips_code_text.push_back(load_addr_instr); // Emit load address instruction
            update_for_load(addr_reg, dest); // Update register descriptor and address descriptor
        }
        else {
            // local stack variable initialization
        }
    }
    else if(op == "deref"){
        // same as load, but without la
    }
    else if(op == "store"){ // store instruction (generated by spill register only)
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        if(dest_sym->scope == 0){ // global variable storage
            emit_instruction("la", "addr", dest, ""); // Load address of dest
            MIPSRegister addr_reg = get_register_for_operand("addr"); // Get a register for the address
            if(dest_sym->type.type_index == PrimitiveTypes::U_CHAR_T || dest_sym->type.type_index == PrimitiveTypes::CHAR_T){
                MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source
                MIPSInstruction store_instr(MIPSOpcode::SB, src1_reg, "0", addr_reg); // Store byte to memory
                mips_code_text.push_back(store_instr); // Emit store instruction
                update_for_store(src1, src1_reg); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::U_SHORT_T || dest_sym->type.type_index == PrimitiveTypes::SHORT_T){
                MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source
                MIPSInstruction store_instr(MIPSOpcode::SH, src1_reg, "0", addr_reg); // Store halfword to memory
                mips_code_text.push_back(store_instr); // Emit store instruction
                update_for_store(src1, src1_reg); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index >= PrimitiveTypes::U_INT_T && dest_sym->type.type_index <= PrimitiveTypes::LONG_T){
                MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source
                MIPSInstruction store_instr(MIPSOpcode::SW, src1_reg, "0", addr_reg); // Store word to memory
                mips_code_text.push_back(store_instr); // Emit store instruction
                update_for_store(src1, src1_reg); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::U_LONG_LONG_T || dest_sym->type.type_index == PrimitiveTypes::LONG_LONG_T){
                MIPSRegister src1_reg_hi = get_register_for_operand(src1+"_hi"); // Get a register for the upper 32 bits of the source
                MIPSRegister src1_reg_lo = get_register_for_operand(src1+"_lo"); // Get a register for the lower 32 bits of the source
                MIPSInstruction store_instr_hi(MIPSOpcode::SW, src1_reg_hi, "0", addr_reg); // Store upper 32 bits of long long to memory
                MIPSInstruction store_instr_lo(MIPSOpcode::SW, src1_reg_lo, "4", addr_reg); // Store lower 32 bits of long long to memory
                mips_code_text.push_back(store_instr_hi); // Emit store instruction for upper 32 bits
                mips_code_text.push_back(store_instr_lo); // Emit store instruction for lower 32 bits
                update_for_store(src1+"_hi", src1_reg_hi); // Update register descriptor and address descriptor
                update_for_store(src1+"_lo", src1_reg_lo); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T){
                MIPSRegister src1_reg = get_float_register_for_operand(src1); // Get a register for the source
                MIPSInstruction store_instr(MIPSOpcode::SWC1, src1_reg, "0", addr_reg); // Store float to memory
                mips_code_text.push_back(store_instr); // Emit store instruction for float
                update_for_store(src1, src1_reg); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || dest_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T){
                MIPSRegister src1_reg = get_float_register_for_operand(src1, false, true); // Get a register for the source
                MIPSInstruction store_instr(MIPSOpcode::SDC1, src1_reg, "0", addr_reg); // Store double to memory
                mips_code_text.push_back(store_instr); // Emit store instruction for double
                update_for_store(src1, src1_reg, true); // Update register descriptor and address descriptor
            }
        }
        else{
            // local stack variable storage
        }
    }
    else if(op == "cast"){ // cast instruction
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if(dest_sym->type == src2_sym->type){ // same type
            emit_instruction("load", dest, src2, ""); // emit instruction dest = src2
        }
        else if(dest_sym->type.isUnsigned() && src2_sym->type.type_index == dest_sym->type.type_index + 1){ // signed to unsigned of same type
            emit_instruction("load", dest, src2, ""); // emit instruction dest = src2
        }
        else if(dest_sym->type.isSigned() && src2_sym->type.type_index == dest_sym->type.type_index - 1){ // unsigned to signed of same type
            emit_instruction("load", dest, src2, ""); // emit instruction dest = src2
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::U_LONG_LONG_T && src2_sym->type.isInt()){ // zero extend unsigned
            int smaller_bit_size = 8 * src2_sym->type.get_size();
            int mask = (1U << smaller_bit_size) - 1; // Create a mask for the smaller size
            emit_instruction("andi", dest+"_hi", dest+"_hi", "0"); // emit instruction dest_hi = 0
            emit_instruction("andi", dest+"_lo", src2, to_string(mask)); // emit instruction dest = src2 & mask
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::LONG_LONG_T && src2_sym->type.isInt() && src2_sym->type.isSigned()){ // sign extend signed
            int shift_size = 8 * (32 - min(dest_sym->type.get_size(),src2_sym->type.get_size()));
            emit_instruction("sll", dest+"_lo", src2, to_string(shift_size)); // emit instruction dest_lo = src2 << shift_size
            emit_instruction("sra", dest+"_lo", dest+"_lo", to_string(shift_size)); // emit instruction dest_lo = dest_lo >> shift_size
            emit_instruction("sra", dest+"_hi", dest+"_lo", to_string(31)); // emit instruction dest_hi = dest_lo >> 32
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::LONG_LONG_T && src2_sym->type.isInt() && src2_sym->type.isUnsigned()){ // zero extend unsigned
            int smaller_bit_size = 8 * src2_sym->type.get_size();
            int mask = (1U << smaller_bit_size) - 1; // Create a mask for the smaller size
            emit_instruction("andi", dest+"_hi", dest+"_hi", "0"); // emit instruction dest_hi = 0
            emit_instruction("andi", dest+"_lo", src2, to_string(mask)); // emit instruction dest = src2 & mask
        }
        else if((src2_sym->type.type_index == PrimitiveTypes::U_LONG_LONG_T || src2_sym->type.type_index == PrimitiveTypes::LONG_LONG_T) && dest_sym->type.isUnsigned()){ // zero extend unsigned
            int smaller_bit_size = 8 * dest_sym->type.get_size();
            int mask = (1U << smaller_bit_size) - 1; // Create a mask for the smaller size
            emit_instruction("andi", dest, src2+"_lo", to_string(mask)); // emit instruction dest = src2_lo & mask
        }
        else if((src2_sym->type.type_index == PrimitiveTypes::U_LONG_LONG_T || src2_sym->type.type_index == PrimitiveTypes::LONG_LONG_T) && dest_sym->type.isSigned()){ // sign extend signed
            int shift_size = 8 * (32 - min(dest_sym->type.get_size(),src2_sym->type.get_size()));
            emit_instruction("sll", dest, src2+"_lo", to_string(shift_size)); // emit instruction dest = src2_lo << shift_size
            emit_instruction("sra", dest, dest, to_string(shift_size)); // emit instruction dest = dest >> shift_size
        }
        else if(dest_sym->type.isUnsigned()){ // zero extend unsigned
            int smaller_bit_size = 8 * min(dest_sym->type.get_size(),src2_sym->type.get_size());
            int mask = (1U << smaller_bit_size) - 1; // Create a mask for the smaller size
            emit_instruction("andi", dest, src2, to_string(mask)); // emit instruction dest = src2 & mask
        }
        else if(dest_sym->type.isSigned() && src2_sym->type.isSigned()){ // sign extend signed
            int shift_size = 8 * (32 - min(dest_sym->type.get_size(),src2_sym->type.get_size()));
            emit_instruction("sll", dest, src2, to_string(shift_size)); // emit instruction dest = src2 << shift_size
            emit_instruction("sra", dest, dest, to_string(shift_size)); // emit instruction dest = dest >> shift_size
        }   
        else if(dest_sym->type.isSigned() && src2_sym->type.isUnsigned()){ // zero extend unsigned
            int smaller_bit_size = 8 * min(dest_sym->type.get_size(),src2_sym->type.get_size());
            int mask = (1U << smaller_bit_size) - 1; // Create a mask for the smaller size
            emit_instruction("andi", dest, src2, to_string(mask)); // emit instruction dest = src2 & mask
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T && (src2_sym->type.type_index == PrimitiveTypes::DOUBLE_T || src2_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T)){ // float to double
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src_reg = get_float_register_for_operand(src2, false); // Get a register for the source
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_S_D, dest_reg, src_reg); // Convert double to float
            mips_code_text.push_back(cvt_instr); // Emit conversion instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if((dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || src2_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T) && src2_sym->type.type_index == PrimitiveTypes::FLOAT_T ){ // double to float
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src_reg = get_float_register_for_operand(src2, false); // Get a register for the source
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_D_S, dest_reg, src_reg); // Convert float to double
            mips_code_text.push_back(cvt_instr); // Emit conversion instruction
            update_for_add(dest, dest_reg, true); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T && src2_sym->type.type_index < PrimitiveTypes::U_INT_T){
            // Convert to 32 bit int first
            if(src2_sym->type.isUnsigned()){
                int bit_size = 8 * src2_sym->type.get_size();
                int mask = (1U << bit_size) - 1;
                emit_instruction("andi", src2, src2, to_string(mask));
            }
            else{
                int shift_size = 8 * (32 - src2_sym->type.get_size());
                emit_instruction("sll", src2, src2, to_string(shift_size)); 
                emit_instruction("sra", src2, src2, to_string(shift_size)); 
            }
            MIPSRegister src_reg = get_register_for_operand(src2, false); // Get a register for the source
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction move_instr(MIPSOpcode::MTC1, dest_reg, src_reg); // Move int to float register
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_S_W, dest_reg, dest_reg); // Convert int to float
            mips_code_text.push_back(move_instr); // Emit move instruction
            mips_code_text.push_back(cvt_instr); 
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T && src2_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src_reg = get_register_for_operand(src2, false); // Get a register for the source
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction move_instr(MIPSOpcode::MTC1, dest_reg, src_reg); // Move int to float register
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_S_W, dest_reg, dest_reg); // Convert int to float
            mips_code_text.push_back(move_instr); // Emit move instruction
            mips_code_text.push_back(cvt_instr); // Emit conversion instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T && src2_sym->type.type_index <= PrimitiveTypes::LONG_LONG_T){
            emit_instruction("load", src2+"_lo", src2+"_lo", ""); // Load the source value into a register
            MIPSRegister src_reg = get_register_for_operand(src2+"_lo", false); // Get a register for the source
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction move_instr(MIPSOpcode::MTC1, dest_reg, src_reg); // Move int to float register
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_S_W, dest_reg, dest_reg); // Convert int to float
            mips_code_text.push_back(move_instr); // Emit move instruction
            mips_code_text.push_back(cvt_instr); // Emit conversion instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if((dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || src2_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T) && src2_sym->type.type_index < PrimitiveTypes::U_INT_T){
            // Convert to 32 bit int first
            if(src2_sym->type.isUnsigned()){
                int bit_size = 8 * src2_sym->type.get_size();
                int mask = (1U << bit_size) - 1;
                emit_instruction("andi", src2, src2, to_string(mask));
            }
            else{
                int shift_size = 8 * (32 - src2_sym->type.get_size());
                emit_instruction("sll", src2, src2, to_string(shift_size)); 
                emit_instruction("sra", src2, src2, to_string(shift_size)); 
            }
            MIPSRegister src_reg = get_register_for_operand(src2); // Get a register for the source
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true, true); // Get a register for the destination
            MIPSInstruction move_instr(MIPSOpcode::MTC1, dest_reg, src_reg); // Move int to float register
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_D_W, dest_reg, dest_reg); // Convert int to float
            mips_code_text.push_back(move_instr); // Emit move instruction
            mips_code_text.push_back(cvt_instr); 
            update_for_add(dest, dest_reg, true); // Update register descriptor and address descriptor
        }
        else if((dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || src2_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T) && src2_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src_reg = get_register_for_operand(src2); // Get a register for the source
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction move_instr(MIPSOpcode::MTC1, dest_reg, src_reg); // Move int to float register
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_D_W, dest_reg, dest_reg); // Convert int to float
            mips_code_text.push_back(move_instr); // Emit move instruction
            mips_code_text.push_back(cvt_instr); // Emit conversion instruction
            update_for_add(dest, dest_reg, true); // Update register descriptor and address descriptor
        }
        else if((dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || src2_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T) && src2_sym->type.type_index <= PrimitiveTypes::LONG_LONG_T){
            emit_instruction("load", src2+"_lo", src2+"_lo", ""); // Load the source value into a register
            MIPSRegister src_reg = get_register_for_operand(src2+"_lo"); // Get a register for the source
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction move_instr(MIPSOpcode::MTC1, dest_reg, src_reg); // Move int to float register
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_D_W, dest_reg, dest_reg); // Convert int to float
            mips_code_text.push_back(move_instr); // Emit move instruction
            mips_code_text.push_back(cvt_instr); // Emit conversion instruction
            update_for_add(dest, dest_reg, true); // Update register descriptor and address descriptor
        }
        else if(src2_sym->type.type_index == PrimitiveTypes::FLOAT_T && dest_sym->type.type_index < PrimitiveTypes::U_INT_T){
            // Convert to 32 bit int first
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src_reg = get_float_register_for_operand(src2); // Get a float register for the source
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination 
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_W_S, src_reg, src_reg); // Convert float to int
            MIPSInstruction move_instr(MIPSOpcode::MFC1, dest_reg, src_reg); // Move to int register
            mips_code_text.push_back(cvt_instr); 
            mips_code_text.push_back(move_instr); // Emit move instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
            if(dest_sym->type.isUnsigned()){
                int bit_size = 8 * dest_sym->type.get_size();
                int mask = (1U << bit_size) - 1;
                emit_instruction("andi", dest, dest, to_string(mask));
            }
            else{
                int shift_size = 8 * (32 - dest_sym->type.get_size());
                emit_instruction("sll", dest, dest, to_string(shift_size)); 
                emit_instruction("sra", dest, dest, to_string(shift_size)); 
            }
        }
        else if(src2_sym->type.type_index == PrimitiveTypes::FLOAT_T && dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src_reg = get_float_register_for_operand(src2); // Get a float register for the source
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_W_S, src_reg, src_reg); // Convert float to int
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination 
            MIPSInstruction move_instr(MIPSOpcode::MFC1, dest_reg, src_reg); // Move to int register
            mips_code_text.push_back(cvt_instr); 
            mips_code_text.push_back(move_instr); // Emit move instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(src2_sym->type.type_index == PrimitiveTypes::FLOAT_T && dest_sym->type.type_index <= PrimitiveTypes::LONG_LONG_T){
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src_reg = get_float_register_for_operand(src2); // Get a float register for the source
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_W_S, src_reg, src_reg); // Convert float to int
            MIPSRegister dest_lo_reg = get_register_for_operand(dest+"_lo", true); // Get a register for the destination 
            MIPSInstruction move_instr(MIPSOpcode::MFC1, dest_lo_reg, src_reg); // Move to int register
            mips_code_text.push_back(cvt_instr); 
            mips_code_text.push_back(move_instr); // Emit move instruction
            update_for_add(dest+"_lo", dest_lo_reg); // Update register descriptor and address descriptor
            if(dest_sym->type.isUnsigned()){
                emit_instruction("andi", dest+"_hi", dest+"_lo", "0"); // emit instruction dest_hi = 0
            }
            else{ 
                emit_instruction("sra", dest+"_hi", dest+"_lo", "31"); // emit instruction dest_hi = dest_lo >> 32
            }

        }
        else if((dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || src2_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T) && src2_sym->type.type_index < PrimitiveTypes::U_INT_T){
            // Convert to 32 bit int first
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src_reg = get_float_register_for_operand(src2, false, true); // Get a float register for the source
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_W_D, src_reg, src_reg); // Convert float to int
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination 
            MIPSInstruction move_instr(MIPSOpcode::MFC1, dest_reg, src_reg); // Move to int register
            mips_code_text.push_back(cvt_instr); 
            mips_code_text.push_back(move_instr); // Emit move instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
            if(dest_sym->type.isUnsigned()){
                int bit_size = 8 * dest_sym->type.get_size();
                int mask = (1U << bit_size) - 1;
                emit_instruction("andi", dest, dest, to_string(mask));
            }
            else{
                int shift_size = 8 * (32 - dest_sym->type.get_size());
                emit_instruction("sll", dest, dest, to_string(shift_size)); 
                emit_instruction("sra", dest, dest, to_string(shift_size)); 
            }
        }
        else if((dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || src2_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T) && src2_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src_reg = get_float_register_for_operand(src2, false, true); // Get a float register for the source
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_W_D, src_reg, src_reg); // Convert float to int
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination 
            MIPSInstruction move_instr(MIPSOpcode::MFC1, dest_reg, src_reg); // Move to int register
            mips_code_text.push_back(cvt_instr); 
            mips_code_text.push_back(move_instr); // Emit move instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if((dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || src2_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T) && src2_sym->type.type_index <= PrimitiveTypes::LONG_LONG_T){
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src_reg = get_float_register_for_operand(src2, false, true); // Get a float register for the source
            MIPSInstruction cvt_instr(MIPSOpcode::CVT_W_D, src_reg, src_reg); // Convert float to int
            MIPSRegister dest_lo_reg = get_register_for_operand(dest+"_lo", true); // Get a register for the destination 
            MIPSInstruction move_instr(MIPSOpcode::MFC1, dest_lo_reg, src_reg); // Move to int register
            mips_code_text.push_back(cvt_instr); 
            mips_code_text.push_back(move_instr); // Emit move instruction
            update_for_add(dest+"_lo", dest_lo_reg); // Update register descriptor and address descriptor
            if(dest_sym->type.isUnsigned()){
                emit_instruction("andi", dest+"_hi", dest+"_lo", "0"); // emit instruction dest_hi = 0
            }
            else{ 
                emit_instruction("sra", dest+"_hi", dest+"_lo", "31"); // emit instruction dest_hi = dest_lo >> 32
            }
        }
    }
    else if(op == "add"){ // add instruction
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction add_instr(MIPSOpcode::ADDU, dest_reg, src1_reg, src2_reg); // Add the two registers
            mips_code_text.push_back(add_instr); // Emit add instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::U_LONG_LONG_T || dest_sym->type.type_index == PrimitiveTypes::LONG_LONG_T){
            // Add lo parts first
            emit_instruction("load", src1+"_lo", src1+"_lo", ""); // Load the source value into a register
            emit_instruction("load", src2+"_lo", src2+"_lo", ""); // Load the source value into a register
            MIPSRegister src1_reg_lo = get_register_for_operand(src1+"_lo"); // Get a register for the source 1 lo
            MIPSRegister src2_reg_lo = get_register_for_operand(src2+"_lo"); // Get a register for the source 2 lo
            MIPSRegister dest_reg_lo = get_register_for_operand(dest+"_lo", true); // Get a register for the destination lo
            MIPSInstruction add_instr_lo(MIPSOpcode::ADDU, dest_reg_lo, src1_reg_lo, src2_reg_lo); // Add the two registers lo
            mips_code_text.push_back(add_instr_lo); // Emit add instruction for lo
            update_for_add(dest+"_lo", dest_reg_lo); // Update register descriptor and address descriptor for lo
            // Set carry if overflow occurs
            emit_instruction("sltu", "carry", dest+"_lo", src1+"_lo"); // Set carry register if overflow occurs
            MIPSRegister carry_reg = get_register_for_operand("carry"); // Get a register for the carry
            // Add hi parts
            emit_instruction("load", src1+"_hi", src1+"_hi", ""); // Load the source value into a register
            emit_instruction("load", src2+"_hi", src2+"_hi", ""); // Load the source value into a register
            MIPSRegister src1_reg_hi = get_register_for_operand(src1+"_hi"); // Get a register for the source 1 hi
            MIPSRegister src2_reg_hi = get_register_for_operand(src2+"_hi"); // Get a register for the source 2 hi
            MIPSRegister dest_reg_hi = get_register_for_operand(dest+"_hi", true); // Get a register for the destination hi
            MIPSInstruction add_instr_hi(MIPSOpcode::ADDU, dest_reg_hi, src1_reg_hi, src2_reg_hi); // Add the two registers hi
            MIPSInstruction add_carry_instr(MIPSOpcode::ADDU, dest_reg_hi, dest_reg_hi, carry_reg); // Add the carry to the hi register
            mips_code_text.push_back(add_instr_hi); // Emit add instruction for hi
            mips_code_text.push_back(add_carry_instr); // Emit add instruction for carry
            update_for_add(dest+"_hi", dest_reg_hi); // Update register descriptor and address descriptor for hi
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_float_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_float_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction add_instr(MIPSOpcode::ADD_S, dest_reg, src1_reg, src2_reg); // Add the two registers
            mips_code_text.push_back(add_instr); // Emit add instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || dest_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_float_register_for_operand(src1, false, true); // Get a register for the source 1
            MIPSRegister src2_reg = get_float_register_for_operand(src2, false, true); // Get a register for the source 2
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true, true); // Get a register for the destination
            MIPSInstruction add_instr(MIPSOpcode::ADD_D, dest_reg, src1_reg, src2_reg); // Add the two registers
            mips_code_text.push_back(add_instr); // Emit add instruction
            update_for_add(dest, dest_reg, true); // Update register descriptor and address descriptor
        }
    }
    else if(op == "addi"){ // addi instruction
        // only used for sp,fp,gp so no need to check for type
        MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
        MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
        MIPSInstruction addi_instr(MIPSOpcode::ADDIU, dest_reg, src1_reg, src2); // Add immediate instruction
        mips_code_text.push_back(addi_instr); // Emit add immediate instruction
        update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
    }
    else if(op == "sub"){ // sub instruction
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if (dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T) {
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction sub_instr(MIPSOpcode::SUBU, dest_reg, src1_reg, src2_reg); // Subtract the two registers
            mips_code_text.push_back(sub_instr); // Emit sub instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if (dest_sym->type.type_index == PrimitiveTypes::U_LONG_LONG_T || dest_sym->type.type_index == PrimitiveTypes::LONG_LONG_T) {
            emit_instruction("load", src1+"_lo", src1+"_lo", ""); // Load the source value into a register
            emit_instruction("load", src2+"_lo", src2+"_lo", ""); // Load the source value into a register
            MIPSRegister src1_reg_lo = get_register_for_operand(src1+"_lo"); // Get a register for the source 1 lo
            MIPSRegister src2_reg_lo = get_register_for_operand(src2+"_lo"); // Get a register for the source 2 lo
            MIPSRegister dest_reg_lo = get_register_for_operand(dest+"_lo", true); // Get a register for the destination lo
            // Subtract low parts
            MIPSInstruction sub_instr_lo(MIPSOpcode::SUBU, dest_reg_lo, src1_reg_lo, src2_reg_lo);
            mips_code_text.push_back(sub_instr_lo);
            update_for_add(dest+"_lo", dest_reg_lo); // Update register descriptor and address descriptor for lo
            // Set borrow if src1_lo < src2_lo
            emit_instruction("sltu", "borrow", src1+"_lo", src2+"_lo");
            MIPSRegister borrow_reg = get_register_for_operand("borrow");
            // Subtract high parts
            emit_instruction("load", src1+"_hi", src1+"_hi", ""); // Load the source value into a register
            emit_instruction("load", src2+"_hi", src2+"_hi", ""); // Load the source value into a register
            MIPSRegister src1_reg_hi = get_register_for_operand(src1+"_hi"); // Get a register for the source 1 hi
            MIPSRegister src2_reg_hi = get_register_for_operand(src2+"_hi"); // Get a register for the source 2 hi
            MIPSRegister dest_reg_hi = get_register_for_operand(dest+"_hi", true); // Get a register for the destination hi
            MIPSInstruction sub_instr_hi(MIPSOpcode::SUBU, dest_reg_hi, src1_reg_hi, src2_reg_hi);
            MIPSInstruction sub_borrow_instr(MIPSOpcode::SUBU, dest_reg_hi, dest_reg_hi, borrow_reg);
            mips_code_text.push_back(sub_instr_hi);
            mips_code_text.push_back(sub_borrow_instr);
            update_for_add(dest+"_hi", dest_reg_hi); // Update register descriptor and address descriptor for hi
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_float_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_float_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction sub_instr(MIPSOpcode::SUB_S, dest_reg, src1_reg, src2_reg); // Add the two registers
            mips_code_text.push_back(sub_instr); // Emit add instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || dest_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_float_register_for_operand(src1, false, true); // Get a register for the source 1
            MIPSRegister src2_reg = get_float_register_for_operand(src2, false, true); // Get a register for the source 2
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true, true); // Get a register for the destination
            MIPSInstruction sub_instr(MIPSOpcode::SUB_D, dest_reg, src1_reg, src2_reg); // Add the two registers
            mips_code_text.push_back(sub_instr); // Emit add instruction
            update_for_add(dest, dest_reg, true); // Update register descriptor and address descriptor
        }
    }
    else if(op == "subi"){ //subi instruction
        // only used for sp,fp,gp so no need to check for type
        MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
        MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
        MIPSInstruction neg_instr(MIPSOpcode::NEG, dest_reg, src1_reg); // Negate the source register
        mips_code_text.push_back(neg_instr); // Emit neg instruction
        MIPSInstruction add_instr(MIPSOpcode::ADDIU, dest_reg, src1_reg, src2); // Sub immediate instruction
        mips_code_text.push_back(add_instr); // Emit sub immediate instruction
        update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
    }
    else if(op == "mul"){ // mul instruction
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T && dest_sym->type.isUnsigned()){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction mul_instr(MIPSOpcode::MULU, src1_reg, src2_reg); // Multiply the two registers
            mips_code_text.push_back(mul_instr); // Emit mul instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T && dest_sym->type.isUnsigned()){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction mul_instr(MIPSOpcode::MUL, src1_reg, src2_reg); // Multiply the two registers
            mips_code_text.push_back(mul_instr); // Emit mul instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::U_LONG_LONG_T ){
            emit_instruction("load", src1+"_lo", src1+"_lo", ""); // Load the source value into a register
            emit_instruction("load", src2+"_lo", src2+"_lo", ""); // Load the source value into a register
            emit_instruction("load", src1+"_hi", src1+"_hi", ""); // Load the source value into a register
            emit_instruction("load", src2+"_hi", src2+"_hi", ""); // Load the source value into a register
            MIPSRegister src1_reg_lo = get_register_for_operand(src1+"_lo"); // Get a register for the source 1 lo
            MIPSRegister src2_reg_lo = get_register_for_operand(src2+"_lo"); // Get a register for the source 2 lo
            MIPSRegister src1_reg_hi = get_register_for_operand(src1+"_hi"); // Get a register for the source 1 hi
            MIPSRegister src2_reg_hi = get_register_for_operand(src2+"_hi"); // Get a register for the source 2 hi
            MIPSRegister dest_reg_lo = get_register_for_operand(dest+"_lo", true); // Get a register for the destination lo
            MIPSRegister dest_reg_hi = get_register_for_operand(dest+"_hi", true); // Get a register for the destination hi
            MIPSRegister carry_reg = get_register_for_operand("carry", true); // Get a register for the carry
            MIPSRegister temp_reg = get_register_for_operand("temp"); // Temporary register for upper bits of multiplication
            // set carry to 0
            emit_instruction("andi", "carry", "carry", "0"); // Set carry to 0
            // set dest_hi to 0
            emit_instruction("andi", dest+"_hi", dest+"_hi", "0"); // Set dest_hi to 0
            // t1 = src1_lo * src2_lo
            MIPSInstruction mul_instr_1(MIPSOpcode::MULTU, src1_reg_lo, src2_reg_lo); // Multiply the two registers
            mips_code_text.push_back(mul_instr_1); // Emit mul instruction
            MIPSInstruction mflo_instr_1(MIPSOpcode::MFLO, dest_reg_lo); // Move the result to the destination lo register
            mips_code_text.push_back(mflo_instr_1); // Emit move instruction
            update_for_add(dest+"_lo", dest_reg_lo); // Update register descriptor and address descriptor for lo
            MIPSInstruction mfhi_instr_1(MIPSOpcode::MFHI, temp_reg); // Move Carry from src1_lo * src2_lo to temp register
            mips_code_text.push_back(mfhi_instr_1); // Emit move instruction
            MIPSInstruction add_to_carry_instr_1(MIPSOpcode::ADDU, carry_reg, carry_reg, temp_reg); // Add the carry from src1_lo * src2_lo to the carry register
            mips_code_text.push_back(add_to_carry_instr_1); // Emit add instruction for carry
            // t2 =  src1_lo * src2_hi
            MIPSInstruction mul_instr_2(MIPSOpcode::MULTU, src1_reg_lo, src2_reg_hi); // Multiply the two registers
            mips_code_text.push_back(mul_instr_2); // Emit mul instruction
            MIPSInstruction mflo_instr_2(MIPSOpcode::MFLO, temp_reg); // Move the result to the temporary register
            mips_code_text.push_back(mflo_instr_2); // Emit move instruction
            MIPSInstruction add_to_carry_instr_2(MIPSOpcode::ADDU, carry_reg, carry_reg, temp_reg); // Add the carry from src1_lo * src2_hi to the carry register
            mips_code_text.push_back(add_to_carry_instr_2); // Emit add instruction for carry
            MIPSInstruction mfhi_instr_2(MIPSOpcode::MFHI, temp_reg); // Move Carry from src1_lo * src2_hi to temp register
            mips_code_text.push_back(mfhi_instr_2); // Emit move instruction
            MIPSInstruction add_to_dest_hi_instr_1(MIPSOpcode::ADDU, dest_reg_hi, dest_reg_hi, temp_reg); // Add the carry from src1_lo * src2_hi to the destination hi register
            mips_code_text.push_back(add_to_dest_hi_instr_1); // Emit add instruction for carry
            // t3 = src1_hi * src2_lo
            MIPSInstruction mul_instr_3(MIPSOpcode::MULTU, src1_reg_hi, src2_reg_lo); // Multiply the two registers
            mips_code_text.push_back(mul_instr_3); // Emit mul instruction
            MIPSInstruction mflo_instr_3(MIPSOpcode::MFLO, temp_reg); // Move the result to the temporary register
            mips_code_text.push_back(mflo_instr_3); // Emit move instruction
            MIPSInstruction add_to_carry_instr_3(MIPSOpcode::ADDU, carry_reg, carry_reg, temp_reg); // Add the carry from src1_hi * src2_lo to the carry register
            mips_code_text.push_back(add_to_carry_instr_3); // Emit add instruction for carry
            MIPSInstruction mfhi_instr_3(MIPSOpcode::MFHI, temp_reg); // Move Carry from src1_hi * src2_lo to temp register
            mips_code_text.push_back(mfhi_instr_3); // Emit move instruction
            MIPSInstruction add_to_dest_hi_instr_2(MIPSOpcode::ADDU, dest_reg_hi, dest_reg_hi, temp_reg); // Add the carry from src1_hi * src2_lo to the destination hi register
            mips_code_text.push_back(add_to_dest_hi_instr_2); // Emit add instruction for carry
            // add carry to dest_hi
            MIPSInstruction add_carry_instr(MIPSOpcode::ADDU, dest_reg_hi, dest_reg_hi, carry_reg); // Add the carry to the hi register
            mips_code_text.push_back(add_carry_instr); // Emit add instruction for carry
            update_for_add(dest+"_hi", dest_reg_hi); // Update register descriptor and address descriptor for hi
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::LONG_LONG_T){
            emit_instruction("load", src1+"_lo", src1+"_lo", ""); // Load the source value into a register
            emit_instruction("load", src2+"_lo", src2+"_lo", ""); // Load the source value into a register
            emit_instruction("load", src1+"_hi", src1+"_hi", ""); // Load the source value into a register
            emit_instruction("load", src2+"_hi", src2+"_hi", ""); // Load the source value into a register
            MIPSRegister src1_reg_lo = get_register_for_operand(src1+"_lo"); // Get a register for the source 1 lo
            MIPSRegister src2_reg_lo = get_register_for_operand(src2+"_lo"); // Get a register for the source 2 lo
            MIPSRegister src1_reg_hi = get_register_for_operand(src1+"_hi"); // Get a register for the source 1 hi
            MIPSRegister src2_reg_hi = get_register_for_operand(src2+"_hi"); // Get a register for the source 2 hi
            MIPSRegister dest_reg_lo = get_register_for_operand(dest+"_lo", true); // Get a register for the destination lo
            MIPSRegister dest_reg_hi = get_register_for_operand(dest+"_hi", true); // Get a register for the destination hi
            MIPSRegister carry_reg = get_register_for_operand("carry", true); // Get a register for the carry
            MIPSRegister temp_reg = get_register_for_operand("temp"); // Temporary register for upper bits of multiplication
            // set carry to 0
            emit_instruction("andi", "carry", "carry", "0"); // Set carry to 0
            // set dest_hi to 0
            emit_instruction("andi", dest+"_hi", dest+"_hi", "0"); // Set dest_hi to 0
            // t1 = src1_lo * src2_lo
            MIPSInstruction mul_instr_1(MIPSOpcode::MULT, src1_reg_lo, src2_reg_lo); // Multiply the two registers
            mips_code_text.push_back(mul_instr_1); // Emit mul instruction
            MIPSInstruction mflo_instr_1(MIPSOpcode::MFLO, dest_reg_lo); // Move the result to the destination lo register
            mips_code_text.push_back(mflo_instr_1); // Emit move instruction
            update_for_add(dest+"_lo", dest_reg_lo); // Update register descriptor and address descriptor for lo
            MIPSInstruction mfhi_instr_1(MIPSOpcode::MFHI, temp_reg); // Move Carry from src1_lo * src2_lo to temp register
            mips_code_text.push_back(mfhi_instr_1); // Emit move instruction
            MIPSInstruction add_to_carry_instr_1(MIPSOpcode::ADDU, carry_reg, carry_reg, temp_reg); // Add the carry from src1_lo * src2_lo to the carry register
            mips_code_text.push_back(add_to_carry_instr_1); // Emit add instruction for carry
            // t2 =  src1_lo * src2_hi
            MIPSInstruction mul_instr_2(MIPSOpcode::MULT, src1_reg_lo, src2_reg_hi); // Multiply the two registers
            mips_code_text.push_back(mul_instr_2); // Emit mul instruction
            MIPSInstruction mflo_instr_2(MIPSOpcode::MFLO, temp_reg); // Move the result to the temporary register
            mips_code_text.push_back(mflo_instr_2); // Emit move instruction
            MIPSInstruction add_to_carry_instr_2(MIPSOpcode::ADDU, carry_reg, carry_reg, temp_reg); // Add the carry from src1_lo * src2_hi to the carry register
            mips_code_text.push_back(add_to_carry_instr_2); // Emit add instruction for carry
            MIPSInstruction mfhi_instr_2(MIPSOpcode::MFHI, temp_reg); // Move Carry from src1_lo * src2_hi to temp register
            mips_code_text.push_back(mfhi_instr_2); // Emit move instruction
            MIPSInstruction add_to_dest_hi_instr_1(MIPSOpcode::ADDU, dest_reg_hi, dest_reg_hi, temp_reg); // Add the carry from src1_lo * src2_hi to the destination hi register
            mips_code_text.push_back(add_to_dest_hi_instr_1); // Emit add instruction for carry
            // t3 = src1_hi * src2_lo
            MIPSInstruction mul_instr_3(MIPSOpcode::MULT, src1_reg_hi, src2_reg_lo); // Multiply the two registers
            mips_code_text.push_back(mul_instr_3); // Emit mul instruction
            MIPSInstruction mflo_instr_3(MIPSOpcode::MFLO, temp_reg); // Move the result to the temporary register
            mips_code_text.push_back(mflo_instr_3); // Emit move instruction
            MIPSInstruction add_to_carry_instr_3(MIPSOpcode::ADDU, carry_reg, carry_reg, temp_reg); // Add the carry from src1_hi * src2_lo to the carry register
            mips_code_text.push_back(add_to_carry_instr_3); // Emit add instruction for carry
            MIPSInstruction mfhi_instr_3(MIPSOpcode::MFHI, temp_reg); // Move Carry from src1_hi * src2_lo to temp register
            mips_code_text.push_back(mfhi_instr_3); // Emit move instruction
            MIPSInstruction add_to_dest_hi_instr_2(MIPSOpcode::ADDU, dest_reg_hi, dest_reg_hi, temp_reg); // Add the carry from src1_hi * src2_lo to the destination hi register
            mips_code_text.push_back(add_to_dest_hi_instr_2); // Emit add instruction for carry
            // add carry to dest_hi
            MIPSInstruction add_carry_instr(MIPSOpcode::ADDU, dest_reg_hi, dest_reg_hi, carry_reg); // Add the carry to the hi register
            mips_code_text.push_back(add_carry_instr); // Emit add instruction for carry
            update_for_add(dest+"_hi", dest_reg_hi); // Update register descriptor and address descriptor for hi
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_float_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_float_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction mul_instr(MIPSOpcode::MUL_S, dest_reg, src1_reg, src2_reg); // Multiply the two registers
            mips_code_text.push_back(mul_instr); // Emit mul instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || dest_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_float_register_for_operand(src1, false, true); // Get a register for the source 1
            MIPSRegister src2_reg = get_float_register_for_operand(src2, false, true); // Get a register for the source 2
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true, true); // Get a register for the destination
            MIPSInstruction mul_instr(MIPSOpcode::MUL_D, dest_reg, src1_reg, src2_reg); // Multiply the two registers
            mips_code_text.push_back(mul_instr); // Emit mul instruction
            update_for_add(dest, dest_reg, true); // Update register descriptor and address descriptor
        }
    }
    else if(op == "div"){
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T && dest_sym->type.isUnsigned()){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction div_instr(MIPSOpcode::DIVU, src1_reg, src2_reg); // Multiply the two registers
            mips_code_text.push_back(div_instr); // Emit mul instruction
            MIPSInstruction mflo_instr(MIPSOpcode::MFLO, dest_reg); // Move the result to the destination register
            mips_code_text.push_back(mflo_instr); // Emit move instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T && dest_sym->type.isSigned()){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction div_instr(MIPSOpcode::DIV, src1_reg, src2_reg); // Multiply the two registers
            mips_code_text.push_back(div_instr); // Emit mul instruction
            MIPSInstruction mflo_instr(MIPSOpcode::MFLO, dest_reg); // Move the result to the destination register
            mips_code_text.push_back(mflo_instr); // Emit move instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::U_LONG_LONG_T ){
            // function call to helper function __udivdi3
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::LONG_LONG_T){
            // function call to helper function __divdi3
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_float_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_float_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction div_instr(MIPSOpcode::DIV_S, dest_reg, src1_reg, src2_reg); // Multiply the two registers
            mips_code_text.push_back(div_instr); // Emit mul instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || dest_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_float_register_for_operand(src1, false, true); // Get a register for the source 1
            MIPSRegister src2_reg = get_float_register_for_operand(src2, false, true); // Get a register for the source 2
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true, true); // Get a register for the destination
            MIPSInstruction div_instr(MIPSOpcode::DIV_D, dest_reg, src1_reg, src2_reg); // Multiply the two registers
            mips_code_text.push_back(div_instr); // Emit mul instruction
            update_for_add(dest, dest_reg, true); // Update register descriptor and address descriptor
        }
    }
    else if(op == "mod"){
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T && dest_sym->type.isUnsigned()){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction div_instr(MIPSOpcode::DIVU, src1_reg, src2_reg); // Multiply the two registers
            mips_code_text.push_back(div_instr); // Emit mul instruction
            MIPSInstruction mfhi_instr(MIPSOpcode::MFHI, dest_reg); // Move the result to the destination register
            mips_code_text.push_back(mfhi_instr); // Emit move instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T && dest_sym->type.isSigned()){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction div_instr(MIPSOpcode::DIV, src1_reg, src2_reg); // Multiply the two registers
            mips_code_text.push_back(div_instr); // Emit mul instruction
            MIPSInstruction mfhi_instr(MIPSOpcode::MFHI, dest_reg); // Move the result to the destination register
            mips_code_text.push_back(mfhi_instr); // Emit move instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::U_LONG_LONG_T ){
            // function call to helper function __umoddi3
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::LONG_LONG_T){
            // function call to helper function __moddi3
        }
    }
    else if(op == "and"){ // bitwise and instruction
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction and_instr(MIPSOpcode::AND, dest_reg, src1_reg, src2_reg); // Bitwise AND the two registers
            mips_code_text.push_back(and_instr); // Emit and instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index <= PrimitiveTypes::LONG_LONG_T){
            // AND low parts
            emit_instruction("load", src1+"_lo", src1+"_lo", ""); // Load the source value into a register
            emit_instruction("load", src2+"_lo", src2+"_lo", ""); // Load the source value into a register
            MIPSRegister src1_reg_lo = get_register_for_operand(src1+"_lo"); // Get a register for the source 1 lo
            MIPSRegister src2_reg_lo = get_register_for_operand(src2+"_lo"); // Get a register for the source 2 lo
            MIPSRegister dest_reg_lo = get_register_for_operand(dest+"_lo", true); // Get a register for the destination lo
            MIPSInstruction and_instr_lo(MIPSOpcode::AND, dest_reg_lo, src1_reg_lo, src2_reg_lo); // Bitwise AND the two registers
            mips_code_text.push_back(and_instr_lo); // Emit and instruction
            update_for_add(dest+"_lo", dest_reg_lo); // Update register descriptor and address descriptor for lo
            // AND high parts
            emit_instruction("load", src1+"_hi", src1+"_hi", ""); // Load the source value into a register
            emit_instruction("load", src2+"_hi", src2+"_hi", ""); // Load the source value into a register
            MIPSRegister src1_reg_hi = get_register_for_operand(src1+"_hi"); // Get a register for the source 1 hi
            MIPSRegister src2_reg_hi = get_register_for_operand(src2+"_hi"); // Get a register for the source 2 hi
            MIPSRegister dest_reg_hi = get_register_for_operand(dest+"_hi", true); // Get a register for the destination hi
            MIPSInstruction and_instr_hi(MIPSOpcode::AND, dest_reg_hi, src1_reg_hi, src2_reg_hi); // Bitwise AND the two registers
            mips_code_text.push_back(and_instr_hi); // Emit and instruction
            update_for_add(dest+"_hi", dest_reg_hi); // Update register descriptor and address descriptor for hi
        }
    }
    else if(op == "andi"){
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction andi_instr(MIPSOpcode::ANDI, dest_reg, src1_reg, src2); // Bitwise ANDI the register and immediate value
            mips_code_text.push_back(andi_instr); // Emit andi instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
    }
    else if(op == "or"){ // bitwise or instruction
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction or_instr(MIPSOpcode::OR, dest_reg, src1_reg, src2_reg); // Bitwise OR the two registers
            mips_code_text.push_back(or_instr); // Emit and instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index <= PrimitiveTypes::LONG_LONG_T){
            // OR low parts
            emit_instruction("load", src1+"_lo", src1+"_lo", ""); // Load the source value into a register
            emit_instruction("load", src2+"_lo", src2+"_lo", ""); // Load the source value into a register
            MIPSRegister src1_reg_lo = get_register_for_operand(src1+"_lo"); // Get a register for the source 1 lo
            MIPSRegister src2_reg_lo = get_register_for_operand(src2+"_lo"); // Get a register for the source 2 lo
            MIPSRegister dest_reg_lo = get_register_for_operand(dest+"_lo", true); // Get a register for the destination lo
            MIPSInstruction or_instr_lo(MIPSOpcode::OR, dest_reg_lo, src1_reg_lo, src2_reg_lo); // Bitwise OR the two registers
            mips_code_text.push_back(or_instr_lo); // Emit and instruction
            update_for_add(dest+"_lo", dest_reg_lo); // Update register descriptor and address descriptor for lo
            // OR high parts
            emit_instruction("load", src1+"_hi", src1+"_hi", ""); // Load the source value into a register
            emit_instruction("load", src2+"_hi", src2+"_hi", ""); // Load the source value into a register
            MIPSRegister src1_reg_hi = get_register_for_operand(src1+"_hi"); // Get a register for the source 1 hi
            MIPSRegister src2_reg_hi = get_register_for_operand(src2+"_hi"); // Get a register for the source 2 hi
            MIPSRegister dest_reg_hi = get_register_for_operand(dest+"_hi", true); // Get a register for the destination hi
            MIPSInstruction or_instr_hi(MIPSOpcode::OR, dest_reg_hi, src1_reg_hi, src2_reg_hi); // Bitwise OR the two registers
            mips_code_text.push_back(or_instr_hi); // Emit and instruction
            update_for_add(dest+"_hi", dest_reg_hi); // Update register descriptor and address descriptor for hi
        }
    }
    else if(op == "xor"){ // bitwise xor instruction
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction xor_instr(MIPSOpcode::XOR, dest_reg, src1_reg, src2_reg); // Bitwise XOR the two registers
            mips_code_text.push_back(xor_instr); // Emit and instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index <= PrimitiveTypes::LONG_LONG_T){
            // XOR low parts
            emit_instruction("load", src1+"_lo", src1+"_lo", ""); // Load the source value into a register
            emit_instruction("load", src2+"_lo", src2+"_lo", ""); // Load the source value into a register
            MIPSRegister src1_reg_lo = get_register_for_operand(src1+"_lo"); // Get a register for the source 1 lo
            MIPSRegister src2_reg_lo = get_register_for_operand(src2+"_lo"); // Get a register for the source 2 lo
            MIPSRegister dest_reg_lo = get_register_for_operand(dest+"_lo", true); // Get a register for the destination lo
            MIPSInstruction xor_instr_lo(MIPSOpcode::XOR, dest_reg_lo, src1_reg_lo, src2_reg_lo); // Bitwise XOR the two registers
            mips_code_text.push_back(xor_instr_lo); // Emit and instruction
            update_for_add(dest+"_lo", dest_reg_lo); // Update register descriptor and address descriptor for lo
            // xor high parts
            emit_instruction("load", src1+"_hi", src1+"_hi", ""); // Load the source value into a register
            emit_instruction("load", src2+"_hi", src2+"_hi", ""); // Load the source value into a register
            MIPSRegister src1_reg_hi = get_register_for_operand(src1+"_hi"); // Get a register for the source 1 hi
            MIPSRegister src2_reg_hi = get_register_for_operand(src2+"_hi"); // Get a register for the source 2 hi
            MIPSRegister dest_reg_hi = get_register_for_operand(dest+"_hi", true); // Get a register for the destination hi
            MIPSInstruction xor_instr_hi(MIPSOpcode::XOR, dest_reg_hi, src1_reg_hi, src2_reg_hi); // Bitwise XOR the two registers
            mips_code_text.push_back(xor_instr_hi); // Emit and instruction
            update_for_add(dest+"_hi", dest_reg_hi); // Update register descriptor and address descriptor for hi
        }
    }
    else if(op == "not"){
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction not_instr(MIPSOpcode::NOR, dest_reg, src1_reg, src1_reg); // Bitwise NOT the register
            mips_code_text.push_back(not_instr); // Emit and instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index <= PrimitiveTypes::LONG_LONG_T){
            // implement later if needed
        }
    }
    else if(op == "sllv" ){
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction sllv_instr(MIPSOpcode::SLLV, dest_reg, src1_reg, src2_reg); // Shift left logical variable instruction
            mips_code_text.push_back(sllv_instr); // Emit sllv instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index <= PrimitiveTypes::LONG_LONG_T){
            // implement later if needed
        }
    }
    else if(op == "sll"){
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction sll_instr(MIPSOpcode::SLL, dest_reg, src1_reg, src2); // Shift left logical instruction
            mips_code_text.push_back(sll_instr); // Emit sll instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
    }
    else if(op == "srlv" ){
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction srlv_instr(MIPSOpcode::SRLV, dest_reg, src1_reg, src2_reg); // Shift right logical variable instruction
            mips_code_text.push_back(srlv_instr); // Emit srlv instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index <= PrimitiveTypes::LONG_LONG_T){
            // implement later if needed
        }
    }
    else if(op == "srav"){
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);
        src2_sym = current_symbol_table.get_symbol_using_mangled_name(src2);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            emit_instruction("load", src2, src2, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister src2_reg = get_register_for_operand(src2); // Get a register for the source 2
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction srav_instr(MIPSOpcode::SRAV, dest_reg, src1_reg, src2_reg); // Shift right arithmetic instruction
            mips_code_text.push_back(srav_instr); // Emit sra instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index <= PrimitiveTypes::LONG_LONG_T){
            // implement later if needed
        }
    }
    else if(op == "sra"){
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction sra_instr(MIPSOpcode::SRA, dest_reg, src1_reg, src2); // Shift right arithmetic instruction
            mips_code_text.push_back(sra_instr); // Emit sra instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
    }
    else if(op == "neg"){
        dest_sym = current_symbol_table.get_symbol_using_mangled_name(dest);
        src1_sym = current_symbol_table.get_symbol_using_mangled_name(src1);

        if(dest_sym->type.type_index < PrimitiveTypes::U_LONG_LONG_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister dest_reg = get_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction neg_instr(MIPSOpcode::NEG, dest_reg, src1_reg); // Negate the source register
            mips_code_text.push_back(neg_instr); // Emit neg instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index <= PrimitiveTypes::LONG_LONG_T){
            // implement later if needed
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_float_register_for_operand(src1); // Get a register for the source 1
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
            MIPSInstruction neg_instr(MIPSOpcode::NEG_S, dest_reg, src1_reg); // Negate the source register
            mips_code_text.push_back(neg_instr); // Emit neg instruction
            update_for_add(dest, dest_reg); // Update register descriptor and address descriptor
        }
        else if(dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T || dest_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T){
            emit_instruction("load", src1, src1, ""); // Load the source value into a register
            MIPSRegister src1_reg = get_float_register_for_operand(src1, false, true); // Get a register for the source 1
            MIPSRegister dest_reg = get_float_register_for_operand(dest, true, true); // Get a register for the destination
            MIPSInstruction neg_instr(MIPSOpcode::NEG_D, dest_reg, src1_reg); // Negate the source register
            mips_code_text.push_back(neg_instr); // Emit neg instruction
            update_for_add(dest, dest_reg, true); // Update register descriptor and address descriptor
        }
    }
}

// ===================== MIPS Code Printing ===================//
vector<string> parameters_emit_instrcution(TACInstruction* instr){
    vector<string> emit_instruction_args(4,"");
    if(instr->op.type == TACOperatorType::TAC_OPERATOR_NOP && instr->flag == 0){
        emit_instruction_args[0] = "load";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_ADDR_OF && instr->flag == 1){
        emit_instruction_args[0] = "la";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_DEREF && instr->flag == 1){
        emit_instruction_args[0] = "deref";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_CAST && instr->flag == 0){
        emit_instruction_args[0] = "cast";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
        emit_instruction_args[3] = get_operand_string(instr->arg2);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_ADD && instr->flag == 0){
        emit_instruction_args[0] = "add";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
        emit_instruction_args[3] = get_operand_string(instr->arg2);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_SUB && instr->flag == 0){
        emit_instruction_args[0] = "sub";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
        emit_instruction_args[3] = get_operand_string(instr->arg2);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_MUL && instr->flag == 0){
        emit_instruction_args[0] = "mul";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
        emit_instruction_args[3] = get_operand_string(instr->arg2);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_DIV && instr->flag == 0){
        emit_instruction_args[0] = "div";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
        emit_instruction_args[3] = get_operand_string(instr->arg2);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_MOD && instr->flag == 0){
        emit_instruction_args[0] = "mod";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
        emit_instruction_args[3] = get_operand_string(instr->arg2);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_BIT_AND && instr->flag == 0){
        emit_instruction_args[0] = "and";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
        emit_instruction_args[3] = get_operand_string(instr->arg2);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_BIT_OR && instr->flag == 0){
        emit_instruction_args[0] = "or";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
        emit_instruction_args[3] = get_operand_string(instr->arg2);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_BIT_XOR && instr->flag == 0){
        emit_instruction_args[0] = "xor";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
        emit_instruction_args[3] = get_operand_string(instr->arg2);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_BIT_NOT && instr->flag == 0){
        emit_instruction_args[0] = "not";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_LEFT_SHIFT && instr->flag == 0){
        emit_instruction_args[0] = "sllv";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
        emit_instruction_args[3] = get_operand_string(instr->arg2);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_RIGHT_SHIFT && instr->flag == 0){
        Symbol* dest_sym = current_symbol_table.get_symbol_using_mangled_name(instr->result->value);
        if(dest_sym->type.isSigned()){
            emit_instruction_args[0] = "srav";
        }
        else{
            emit_instruction_args[0] = "srlv";
        }
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
        emit_instruction_args[3] = get_operand_string(instr->arg2);
    }
    else if(instr->op.type == TACOperatorType::TAC_OPERATOR_UMINUS && instr->flag == 0){
        emit_instruction_args[0] = "neg";
        emit_instruction_args[1] = get_operand_string(instr->result);
        emit_instruction_args[2] = get_operand_string(instr->arg1);
    }
    return emit_instruction_args;
}

void initalize_mips_code_vectors(){
    for(int instr_no=0; instr_no<TAC_CODE.size(); instr_no++){
        TACInstruction* instr = TAC_CODE[instr_no];
        vector<string> emit_instruction_args = parameters_emit_instrcution(instr);
        emit_instruction(emit_instruction_args[0],emit_instruction_args[1],emit_instruction_args[2],emit_instruction_args[3]);
    }
}

void print_mips_code() {
    initialize_global_symbol_table(); // Initialize global symbol table
    initalize_mips_code_vectors(); // Initialize MIPS code vectors
    // Printing data section
    cout<< ".data" << endl;
    for(int instr_no=0;instr_no<mips_code_data.size();instr_no++){
        MIPSDataInstruction instr = mips_code_data[instr_no];
        cout<<instr.label<<": "<<get_directive_name(instr.directive)<<" "<<instr.value<<endl;
    }
    // Printing text section
    cout<< ".text" << endl;
    for(int instr_no=0;instr_no<mips_code_text.size();instr_no++){
        MIPSInstruction instr = mips_code_text[instr_no];
        if(instr.label != "") cout<<instr.label<<": "<<endl;
        switch(instr.instruction_type){
            case(MIPSInstructionType::_3_REG_TYPE):
                cout<<get_opcode_name(instr.opcode)<<" "<<get_mips_register_name(instr.dest_reg)<<" "<<get_mips_register_name(instr.src1_reg)<<" "<<get_mips_register_name(instr.src2_reg);
                break;
            case(MIPSInstructionType::_2_REG_OFFSET_TYPE):
                cout<<get_opcode_name(instr.opcode)<<" "<<get_mips_register_name(instr.dest_reg)<<" "<<get_mips_register_name(instr.src1_reg)<<" "<<instr.immediate;
                break;
            case(MIPSInstructionType::_2_REG_IMMEDIATE_TYPE):
                cout<<get_opcode_name(instr.opcode)<<" "<<get_mips_register_name(instr.dest_reg)<<" "<<get_mips_register_name(instr.src1_reg)<<" "<<instr.immediate;
                break;
            case(MIPSInstructionType::_2_REG_TYPE):
                cout<<get_opcode_name(instr.opcode)<<" "<<get_mips_register_name(instr.dest_reg)<<" "<<get_mips_register_name(instr.src1_reg);
                break;
            case(MIPSInstructionType::_1_REG_TYPE):
                cout<<get_opcode_name(instr.opcode)<<" "<<get_mips_register_name(instr.dest_reg);
                break;
            case(MIPSInstructionType::_NOP_TYPE):
                cout<<get_opcode_name(instr.opcode);
                break;
        }
        cout<<endl;
    }
    cout<<endl;
}

