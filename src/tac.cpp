#include "tac.h"
#include "symbol_table.h"
#include <stdio.h>
#include <iostream>
using namespace std;

// Define the global map for identifiers
unordered_map<string, TACOperand*> identifiers;

// Define the global vector for TAC instructions
vector<TACInstruction*> TAC_CODE(MAX_CODE_SIZE);

//##############################################################################
//################################## TACOperand ######################################
//##############################################################################

TACOperand::TACOperand(TACOperandType type, string value) : type(type), value(value) {}

TACOperand* new_temp_var() {
    string temp_var = "t" + to_string(temp_var_id++);
    TACOperand* t = new TACOperand(TAC_OPERAND_TEMP_VAR, temp_var);
    return t; // Return a pointer to the temporary variable
}

TACOperand* new_empty_var() {
    TACOperand* t = new TACOperand(TAC_OPERAND_EMPTY, "");
    return t; // Return a pointer to the empty variable
}

TACOperand* new_label() {
    string label = to_string(label_id++);
    TACOperand* t = new TACOperand(TAC_OPERAND_LABEL, label);
    return t; // Return a pointer to the label
}

TACOperand* new_constant(string value) {
    TACOperand* t = new TACOperand(TAC_OPERAND_CONSTANT, value);
    return t; // Return a pointer to the constant
}

TACOperand* new_identifier(string value) {
    if (identifiers.find(value) == identifiers.end()) {
        TACOperand* new_id = new TACOperand(TAC_OPERAND_IDENTIFIER, value);
        identifiers[value] = new_id;
        return new_id;
    }
    else {
        TACOperand* existing_id = identifiers[value];
        return existing_id;
    }
}

TACOperand* new_type(string value) {
    TACOperand* t = new TACOperand(TAC_OPERAND_TYPE, value);
    return t; // Return a pointer to the type
}

//##############################################################################
//################################## TACOperator ######################################
//##############################################################################
TACOperator::TACOperator() : type(TACOperatorType::TAC_OPERATOR_NOP) {}

TACOperator::TACOperator(TACOperatorType type) : type(type) {}

//##############################################################################
//################################## TACInstruction ######################################
//##############################################################################

TACInstruction::TACInstruction(TACOperator op, TACOperand* result, TACOperand* arg1, TACOperand* arg2, int flag) {
    this->label = *new_label(); // Default flag value
    this->flag = flag;
    this->op = op;
    this->result = result;
    this->arg1 = arg1;
    this->arg2 = arg2;
}

bool is_assignment(TACInstruction* instruction) {
    if (instruction->op.type == (TACOperatorType::TAC_OPERATOR_CALL) || instruction->op.type == (TACOperatorType::TAC_OPERATOR_PARAM) || instruction->op.type == (TACOperatorType::TAC_OPERATOR_RETURN)) {
        return false;
    }
    else return true;
}

TACInstruction* emit(TACOperator op, TACOperand* result, TACOperand* arg1, TACOperand* arg2, int flag) {
    if (instruction_id >= MAX_CODE_SIZE) {
        cerr << "Error: Code size exceeded maximum limit." << endl;
        exit(1);
    }
    TACInstruction* instruction = new TACInstruction(op, result, arg1, arg2, flag);
    return instruction;
}

// TACInstruction* get_instruction() {
//     return code[instruction_id - 1];
// }

// TACOperand get_instruction_label() {
//     return TACOperand(TAC_OPERAND_LABEL, to_string(instruction_id - 1)); // Return the ID of the last instruction
// }

void backpatch(unordered_set<TACInstruction*> list, TACOperand* label) {
    for (auto instruction : list) {
        if (instruction->result->type == TAC_OPERAND_EMPTY) instruction->result = label; // Update the result operand with the label
    }
}

unordered_set<TACInstruction*> merge_lists(unordered_set<TACInstruction*>& list1, unordered_set<TACInstruction*>& list2) {
    for (auto it = list2.begin(); it != list2.end(); ++it) {
        list1.insert(*it);
    }
    return list1;
}

//##############################################################################
//################################## PRINT TACInstruction ######################################
//##############################################################################

string get_operand_string(const TACOperand& operand) {
    if (operand.type == TAC_OPERAND_TEMP_VAR ||
        operand.type == TAC_OPERAND_IDENTIFIER ||
        operand.type == TAC_OPERAND_CONSTANT ||
        operand.type == TAC_OPERAND_LABEL) {
        return operand.value;  // Directly return the operand value
    }
    else if (operand.type == TAC_OPERAND_POINTER) {
        return "*" + operand.value;  // Pointer notation (e.g., *x)
    }
    else if (operand.type == TAC_OPERAND_EMPTY) {
        return "";  // Empty operand
    }
    return ""; // Default case
}

string get_operator_string(TACOperatorType op) {
    switch (op) {
    case TAC_OPERATOR_ADD: return "+";
    case TAC_OPERATOR_SUB: return "-";
    case TAC_OPERATOR_MUL: return "*";
    case TAC_OPERATOR_DIV: return "/";
    case TAC_OPERATOR_MOD: return "%";
    case TAC_OPERATOR_UMINUS: return "-"; // Unary minus
    case TAC_OPERATOR_EQ: return "==";
    case TAC_OPERATOR_NE: return "!=";
    case TAC_OPERATOR_GT: return ">";
    case TAC_OPERATOR_LT: return "<";
    case TAC_OPERATOR_GE: return ">=";
    case TAC_OPERATOR_LE: return "<=";
    case TAC_OPERATOR_AND: return "&&";
    case TAC_OPERATOR_OR: return "||";
    case TAC_OPERATOR_NOT: return "!";
    case TAC_OPERATOR_BIT_AND: return "&";
    case TAC_OPERATOR_BIT_OR: return "|";
    case TAC_OPERATOR_BIT_XOR: return "^";
    case TAC_OPERATOR_LEFT_SHIFT: return "<<";
    case TAC_OPERATOR_RIGHT_SHIFT: return ">>";
    case TAC_OPERATOR_BIT_NOT: return "~";
    case TAC_OPERATOR_ASSIGN: return "=";
    case TAC_OPERATOR_ADDR_OF: return "&"; // Address of operator
    default: return ""; // Default case
    }
}

void print_TAC_instruction(TACInstruction* instruction) {
    if (!instruction) return;

    // **Jump Instructions**
    cout<< instruction->label.value << ": "; // Print the label of the instruction
    if (instruction->flag == 1) {
        cout << "goto " << get_operand_string(*instruction->result); // may need to change depending on emit call
    }
    else if (instruction->flag == 2) {  // may need to change depending on emit call
        cout << "if " << get_operand_string(*instruction->arg1) << " "
            << get_operator_string(instruction->op.type) << " "
            << get_operand_string(*instruction->arg2) << " goto "
            << get_operand_string(*instruction->result);
    }
    // **Function Instructions**
    else if (instruction->op.type == TAC_OPERATOR_PARAM) {
        cout << "param " << get_operand_string(*instruction->arg1);
    }
    else if (instruction->op.type == TAC_OPERATOR_CALL) {
        if (instruction->result->type == TAC_OPERAND_EMPTY) {
            cout << "call " << get_operand_string(*instruction->arg1) << ", "
                << get_operand_string(*instruction->arg2);
        }
        else {
            cout << get_operand_string(*instruction->result) << " = call "
                << get_operand_string(*instruction->arg1) << ", "
                << get_operand_string(*instruction->arg2);
        }
    }
    else if (instruction->op.type == TAC_OPERATOR_RETURN) { // MAY NEED TO CHANGE
        cout << "return " << get_operand_string(*instruction->arg1);
    }
    else if(instruction->op.type == TAC_OPERATOR_FUNC_BEGIN) {
        cout << "function " << get_operand_string(*instruction->arg1);
    }
    else if (instruction->op.type == TAC_OPERATOR_FUNC_END) {
        cout << "end function ";
    }
    // **Pointer Instructions**
    // else if (instruction->op.type == TAC_OPERATOR_ASSIGN && instruction->arg1.type == TAC_OPERAND_POINTER) {
    //     cout << "*" << get_operand_string(instruction->result) << " = " 
    //          << get_operand_string(instruction->arg1);
    // } 
    // **Array Access (Indexed Assignment)**
    // else if (instruction->op.type == TAC_OPERATOR_INDEX) {
    //     // Read from array: `x = y[i]`
    //     cout << get_operand_string(instruction->result) << " = " 
    //          << get_operand_string(instruction->arg1) << "[" 
    //          << get_operand_string(instruction->arg2) << "]";
    // } else if (instruction->op.type == TAC_OPERATOR_INDEX_ASSIGN) {
    //     // Write to array: `x[i] = y`
    //     cout << get_operand_string(instruction->arg1) << "[" 
    //          << get_operand_string(instruction->arg2) << "] = " 
    //          << get_operand_string(instruction->result);
    // } 
    // **Assignment Cases**
    else if (is_assignment(instruction)) {
        if (instruction->arg2->type != TAC_OPERAND_EMPTY) {
            // Binary operation: `x = y op z`
            cout << get_operand_string(*instruction->result) << " = "
                << get_operand_string(*instruction->arg1) << " "
                << get_operator_string(instruction->op.type) << " "
                << get_operand_string(*instruction->arg2);
        }
        else if (instruction->op.type != TAC_OPERATOR_NOP) {
            // Unary operation: `x = op y`
            cout << get_operand_string(*instruction->result) << " = "
                << get_operator_string(instruction->op.type) << " "
                << get_operand_string(*instruction->arg1);
        }
        else {
            // Simple assignment: `x = y`
            cout << get_operand_string(*instruction->result) << " = "
                << get_operand_string(*instruction->arg1);
        }
    }
    else cout << "Nothing to print";
    cout << endl;
}

void print_TAC() {
    cout << "===== Three-Address Code (TAC) =====" << endl;
    for (int i = 0; i < TAC_CODE.size(); ++i) {
         // Stop when we reach uninitialized entries
        print_TAC_instruction(TAC_CODE[i]);
    }
    cout << "====================================" << endl;
}

