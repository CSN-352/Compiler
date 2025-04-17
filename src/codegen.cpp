#include "codegen.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
using namespace std;

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
        case MIPSOpcode::SUB:      return "SUB";
        case MIPSOpcode::SUBU:     return "SUBU";
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
        case MIPSOpcode::MOVE:     return "MOVE";
        case MIPSOpcode::MFHI:     return "MFHI";
        case MIPSOpcode::MFLO:     return "MFLO";
        case MIPSOpcode::MTHI:     return "MTHI";
        case MIPSOpcode::MTLO:     return "MTLO";
        case MIPSOpcode::MOVS:     return "MOV.S";
        case MIPSOpcode::MOVD:     return "MOV.D";
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
        if (vars.count(var)) return true;
    }
    return false;
}

void update_for_load(MIPSRegister reg, const std::string& var) {
    register_descriptor[reg].clear();
    register_descriptor[reg].insert(var);
    address_descriptor[var].insert(get_mips_register_name(reg));

    for (auto& [v, locs] : address_descriptor) {
        if (v != var) locs.erase(get_mips_register_name(reg));
    }
}
 
void update_for_store(const std::string& var, MIPSRegister reg) {
    address_descriptor[var].insert(get_mips_register_name(reg));
    address_descriptor[var].insert("mem");
}

void update_for_add(const std::string& x, MIPSRegister rx) {
    register_descriptor[rx].clear();
    register_descriptor[rx].insert(x);
    address_descriptor[x].clear();
    address_descriptor[x].insert(get_mips_register_name(rx));

    for (auto& [v, locs] : address_descriptor) {
        if (v != x) locs.erase(get_mips_register_name(rx));
    }
}

// x = y
void update_for_assign(const std::string& x, const std::string& y, MIPSRegister ry) {
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

std::unordered_map<int, std::string> find_leaders_from_tac_file(const std::string& filename) {
    std::ifstream infile(filename);
    std::unordered_map<int, std::string> leader_labels;
    std::unordered_set<int> leader_lines;
    std::vector<std::pair<int, std::string>> instructions;

    if (!infile) {
        std::cerr << "Error: Cannot open file " << filename << "\n";
        return leader_labels;
    }

    std::string line;
    std::regex line_num_regex(R"(^(\d+):\s+(.*)$)");
    std::smatch match;

    while (std::getline(infile, line)) {
        if (std::regex_match(line, match, line_num_regex)) {
            int line_num = std::stoi(match[1]);
            std::string instr = match[2];
            instructions.emplace_back(line_num, instr);
        }
    }

    if (!instructions.empty()) {
        leader_lines.insert(instructions.front().first); // first instruction
    }

    for (size_t i = 0; i < instructions.size(); ++i) {
        const std::string& instr = instructions[i].second;

        if (instr.find("goto") != std::string::npos) {
            std::smatch target_match;
            if (std::regex_search(instr, target_match, std::regex(R"(goto\s+I(\d+))"))) {
                int target = std::stoi(target_match[1]);
                leader_lines.insert(target);
            }
            if (i + 1 < instructions.size()) {
                leader_lines.insert(instructions[i + 1].first);
            }
        }
    }

    int label_counter = 0;
    for (int line_num : leader_lines) {
        leader_labels[line_num] = "L" + std::to_string(label_counter++);
    }

    return leader_labels;
}


//=================== Register Management ===================//

// Modify this so that register to be spilled is chosen cyclicly
MIPSRegister get_register_for_operand(const std::string& var,bool for_result) {
        // 1. Already in a register
        for (const auto& [reg, vars] : register_descriptor) {
            if (vars.count(var)) return reg;
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
        for (int r = T0; r <= T9; ++r) {
            MIPSRegister reg = static_cast<MIPSRegister>(r);

            bool all_same_as_var = true;
            for (const auto& v : register_descriptor[reg]) {
                if (v != var) {
                    all_same_as_var = false;
                    break;
                }
            }

            if (all_same_as_var) {
                register_descriptor[reg].clear();  // We can safely reuse it
                address_descriptor[var].erase(get_mips_register_name(reg));
                return reg;
            }
        }

        // 5. Spill case: Choose a register to spill if no safe register is available
        // Let's assume T0 is the spill register for now
        MIPSRegister spill_reg = T0;
        
        // Call the spill_register function to handle the spilling
        spill_register(spill_reg);

        // Return the spill register (T0 in this case) after spilling
        return spill_reg;
    
}


MIPSRegister get_float_register_for_operand(const std::string& var, bool for_result, bool is_double){
    // add logic for double and float register allocation
}

void spill_register(MIPSRegister reg) {
    // Emit store instruction (ST) for each variable in the register
    for (const auto& v : register_descriptor[reg]) {
        // Generate store instruction to move variable `v` to memory
        // Add instruction here
        // std::cout << "ST " << v << ", " << get_mips_register_name(reg) << "\n"; // Placeholder for actual instruction generation

        // Update the address descriptor to mark the variable `v` as stored in memory
        address_descriptor[v].erase(get_mips_register_name(reg));  // Remove from register
        address_descriptor[v].insert("mem");  // Mark as in memory
    }

    // Clear the register descriptor after spilling all its variables
    register_descriptor[reg].clear();
}


//=================== MIPS Instruction Class ===================//

// Constructor for normal 3-reg instruction (e.g., add rd, rs, rt)
MIPSInstruction::MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, MIPSRegister src1, MIPSRegister src2)
    : opcode(opc), rd(dest), rs(src1), rt(src2), immediate(""), label("") {}

// Constructor for load/store: opcode rt, offset(rs)
MIPSInstruction::MIPSInstruction(MIPSOpcode opc, MIPSRegister reg, const std::string& offset, MIPSRegister base)
    : opcode(opc), rt(reg), immediate(offset), rs(base), rd(MIPSRegister::ZERO), label("") {}

// Constructor for immediate instructions (e.g., li, lui)
MIPSInstruction::MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, const std::string& imm)
    : opcode(opc), rd(dest), immediate(imm), rs(MIPSRegister::ZERO), rt(MIPSRegister::ZERO), label("") {}

// Constructor for move instructions (e.g., move rd, rs)
MIPSInstruction::MIPSInstruction(MIPSOpcode opc, MIPSRegister dest, MIPSRegister src)
    : opcode(opc), rd(dest), rs(src), rt(MIPSRegister::ZERO), immediate(""), label("") {}

// Constructor for label-only instruction
MIPSInstruction::MIPSInstruction(const std::string& lbl)
    : label(lbl), opcode(MIPSOpcode::LABEL), rd(MIPSRegister::ZERO), rs(MIPSRegister::ZERO), rt(MIPSRegister::ZERO), immediate("") {}

// Constructor for standalone ops like SYSCALL, NOP
MIPSInstruction::MIPSInstruction(MIPSOpcode opc)
    : opcode(opc), rd(MIPSRegister::ZERO), rs(MIPSRegister::ZERO), rt(MIPSRegister::ZERO), immediate(""), label("") {}

//=================== MIPS Instruction Emission ===================//

void emit_instruction(string op, string dest, string src1, string src2, bool is_assignment){
    Symbol* dest_sym = symbolTable.get_symbol_using_mangled_name(dest);
    Symbol* src1_sym = symbolTable.get_symbol_using_mangled_name(src1);
    Symbol* src2_sym = symbolTable.get_symbol_using_mangled_name(src2);
    if(op == "" && is_assignment){
        int size = dest_sym->type.get_size();
        if(check_if_variable_in_register(src1)){
            // No instruction needed, only change register descriptor and address descriptor
            update_for_assign(dest, src1, get_register_for_operand(src1));
        }
        else if(symbolTable.lookup_symbol_using_mangled_name(src1)){
            // Load variable from memory
            if(src1_sym->scope == 0){ // global variable
                MIPSRegister addr_reg = get_register_for_operand("addr", true); // Get a register for the address
                MIPSInstruction load_addr_instr(MIPSOpcode::LA, addr_reg, src1); // Load address of src1
                mips_code_text.push_back(load_addr_instr); // Emit load address instruction
                update_for_load(addr_reg, "addr"); // Update register descriptor and address descriptor

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
                else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T || dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T){
                    MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
                    MIPSInstruction load_instr(MIPSOpcode::LWC1, dest_reg, "0", addr_reg); // Load float from memory
                    mips_code_text.push_back(load_instr); // Emit load instruction for float
                    update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
                }
                else if(dest_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T){
                    MIPSRegister dest_reg = get_float_register_for_operand(dest, true, true); // Get a register for the destination
                    MIPSInstruction load_instr(MIPSOpcode::LDC1, dest_reg, "0", addr_reg); // Load long double from memory
                    mips_code_text.push_back(load_instr); // Emit load instruction for long double
                    update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
                }
            }
            else{
                // Load variable from stack (to be implemented)
            }
        }
        else{
            // Load immediate value
            if(!check_immediate(src1)){
                store_immediate(src1, dest_sym->type); // Store immediate value in immediate storage map
            }
            MIPSRegister addr_reg = get_register_for_operand("addr", true); // Get a register for the address
            MIPSInstruction load_addr_instr(MIPSOpcode::LA, addr_reg, immediate_storage_map[src1]); // Load address of src1
            mips_code_text.push_back(load_addr_instr); // Emit load address instruction
            update_for_load(addr_reg, "addr"); // Update register descriptor and address descriptor

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
            else if(dest_sym->type.type_index == PrimitiveTypes::FLOAT_T || dest_sym->type.type_index == PrimitiveTypes::DOUBLE_T){
                MIPSRegister dest_reg = get_float_register_for_operand(dest, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LWC1, dest_reg, "0", addr_reg); // Load float from memory
                mips_code_text.push_back(load_instr); // Emit load instruction for float
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
            else if(dest_sym->type.type_index == PrimitiveTypes::LONG_DOUBLE_T){
                MIPSRegister dest_reg = get_float_register_for_operand(dest, true, true); // Get a register for the destination
                MIPSInstruction load_instr(MIPSOpcode::LDC1, dest_reg, "0", addr_reg); // Load long double from memory
                mips_code_text.push_back(load_instr); // Emit load instruction for long double
                update_for_load(dest_reg, dest); // Update register descriptor and address descriptor
            }
        }
    }
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
            mips_code_rodata.push_back(data_instr);
        }
        else if(type.ptr_level == 1){
            immediate_storage_map[immediate] = "immediate_" + to_string(immediate_storage_map.size());
            MIPSDataInstruction data_instr(immediate_storage_map[immediate], MIPSDirective::ASCIIZ, immediate); // string
            mips_code_rodata.push_back(data_instr);
        }
    }
    else if(type.type_index == PrimitiveTypes::U_SHORT_T || type.type_index == PrimitiveTypes::SHORT_T){
        immediate_storage_map[immediate] = "immediate_" + to_string(immediate_storage_map.size());
        MIPSDataInstruction data_instr(immediate_storage_map[immediate], MIPSDirective::HALF, immediate); // short
        mips_code_rodata.push_back(data_instr);
    }
    else if(type.type_index == PrimitiveTypes::U_INT_T || type.type_index == PrimitiveTypes::INT_T){
        immediate_storage_map[immediate] = "immediate_" + to_string(immediate_storage_map.size());
        MIPSDataInstruction data_instr(immediate_storage_map[immediate], MIPSDirective::WORD, immediate); // int
        mips_code_rodata.push_back(data_instr);
    }
    else if(type.type_index == PrimitiveTypes::U_LONG_T || type.type_index == PrimitiveTypes::LONG_T){
        immediate_storage_map[immediate] = "immediate_" + to_string(immediate_storage_map.size());
        MIPSDataInstruction data_instr(immediate_storage_map[immediate], MIPSDirective::WORD, immediate); // long
        mips_code_rodata.push_back(data_instr);
    }
    else if(type.type_index == PrimitiveTypes::U_LONG_LONG_T || type.type_index == PrimitiveTypes::LONG_LONG_T){
        string immediate_hi = to_string(stoll(immediate) >> 32);
        string immediate_lo = to_string(stoll(immediate) & 0xFFFFFFFF);
        immediate_storage_map[immediate_hi] = "immediate_" + to_string(immediate_storage_map.size()) + "_hi";
        immediate_storage_map[immediate_lo] = "immediate_" + to_string(immediate_storage_map.size()) + "_lo";
        // Store the upper and lower 32 bits of the long long value separately
        MIPSDataInstruction data_instr1(immediate_storage_map[immediate_hi], MIPSDirective::WORD, immediate_hi); // upper 32 bits of long long
        MIPSDataInstruction data_instr2(immediate_storage_map[immediate_lo], MIPSDirective::WORD, immediate_lo); // lower 32 bits of long long
        mips_code_rodata.push_back(data_instr1);
        mips_code_rodata.push_back(data_instr2);
    }
    else if(type.type_index == PrimitiveTypes::FLOAT_T){
        immediate_storage_map[immediate] = "immediate_" + to_string(immediate_storage_map.size());
        MIPSDataInstruction data_instr(immediate_storage_map[immediate], MIPSDirective::FLOAT, immediate); // float
        mips_code_rodata.push_back(data_instr);
    }
    else if(type.type_index == PrimitiveTypes::DOUBLE_T || type.type_index == PrimitiveTypes::LONG_DOUBLE_T){
        immediate_storage_map[immediate] = "immediate_" + to_string(immediate_storage_map.size());
        MIPSDataInstruction data_instr(immediate_storage_map[immediate], MIPSDirective::DOUBLE, immediate); // double
        mips_code_rodata.push_back(data_instr);
    }
}

bool check_immediate(const string& immediate) {
    if(immediate_storage_map.find(immediate) == immediate_storage_map.end()){
        return false;
    }
    return true;
}

// ===================== MIPS Code Printing ===================//

void print_mips_code() {
    // correct this code....
    // for (const auto& instr : mips_code) {
    //     // Print label if present
    //     if (!instr.label.empty()) {
    //         std::cout << instr.label << ":\n";
    //         continue;
    //     }

    //     std::string opcode_str = get_opcode_name(instr.opcode);
    //     std::string rd_str = get_mips_register_name(instr.rd);
    //     std::string rs_str = get_mips_register_name(instr.rs);
    //     std::string rt_str = get_mips_register_name(instr.rt);

    //     // Load/store style: rt, offset(rs)
    //     if (!instr.immediate.empty() && instr.rs != MIPSRegister::ZERO) {
    //         std::cout << "    " << opcode_str << " " << rt_str << ", " << instr.immediate << "(" << rs_str << ")\n";
    //     }
    //     // 3-register instruction: rd, rs, rt
    //     else if (instr.rt != MIPSRegister::ZERO) {
    //         std::cout << "    " << opcode_str << " " << rd_str << ", " << rs_str << ", " << rt_str << "\n";
    //     }
    //     // 2-register or immediate instruction
    //     else {
    //         std::cout << "    " << opcode_str;
    //         if (instr.rd != MIPSRegister::ZERO) std::cout << " " << rd_str;
    //         if (instr.rs != MIPSRegister::ZERO) std::cout << ", " << rs_str;
    //         if (!instr.immediate.empty()) std::cout << ", " << instr.immediate;
    //         std::cout << "\n";
    //     }
    // }
}

