#include "tac.h"
#include "symbol_table.h"
#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

void yyerror(const char *msg);
// Define the global map for identifiers
unordered_map<string, TACOperand*> identifiers;

// Define the global vector for TAC instructions
vector<TACInstruction*> TAC_CODE;

//##############################################################################
//################################## TACOperand ######################################
//##############################################################################

TACOperand::TACOperand(TACOperandType type, string value) : type(type), value(value) {}

TACOperand* new_temp_var() {
    string temp_var = "#t" + to_string(temp_var_id++);
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


TACOperand* new_string(string value) {
    TACOperand* t = new TACOperand(TAC_OPERAND_STRING, value);
    return t; // Return a pointer to the string literal
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
    this->label = new_label(); // Default flag value
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
        yyerror("Error: Code size exceeded maximum limit.");
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

string get_operand_string(TACOperand* operand) {
    if(operand == nullptr) return ""; // Handle null operand case
    if (operand->type == TAC_OPERAND_TEMP_VAR ||
        operand->type == TAC_OPERAND_IDENTIFIER ||
        operand->type == TAC_OPERAND_CONSTANT) {
        return operand->value;  // Directly return the operand value
    }
    else if(operand->type == TAC_OPERAND_LABEL){
        return "I" + operand->value;
    }
    else if (operand->type == TAC_OPERAND_POINTER) {
        return "*" + operand->value;  // Pointer notation (e->g->, *x)
    }
    else if (operand->type == TAC_OPERAND_EMPTY) {
        return "";  // Empty operand
    }
    else if (operand->type == TAC_OPERAND_TYPE) {
        return operand->value;
    }
    else if(operand->type == TAC_OPERAND_STRING) {
        return operand->value;  // String literal
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
    case TAC_OPERATOR_DEREF: return "*"; // Dereference operator
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
    case TAC_OPERATOR_INDEX: return "[]"; // Array index operator
    default: return ""; // Default case
    }
}

void print_TAC_instruction(TACInstruction* instruction) {
    // if(instruction->op.type == TACOperatorType::TAC_OPERATOR_CALL) cout<<"calling function: "<<get_operand_string(instruction->arg1)<<endl;
    // if (!instruction||instruction->result->type == TAC_OPERAND_EMPTY) return;
    if(instruction->label->type == TAC_OPERAND_LABEL) cout<<instruction->label->value << ": "; // Print the label of the instruction
    // **Jump Instructions**
    if (instruction->flag == 1) {
        cout << "goto " << get_operand_string(instruction->result); // may need to change depending on emit call
    }
    else if (instruction->flag == 2) {  // may need to change depending on emit call
        cout << "if " << get_operand_string(instruction->arg1) << " "
            << get_operator_string(instruction->op.type) << " "
            << get_operand_string(instruction->arg2) << " goto "
            << get_operand_string(instruction->result);
    }
    // **Function Instructions**
    else if(instruction->op.type == TAC_OPERATOR_CAST) {
        cout << get_operand_string(instruction->result) << " = "
                << "(" << get_operand_string(instruction->arg1) << ")"
                << get_operand_string(instruction->arg2) ;
    }
    else if (instruction->op.type == TAC_OPERATOR_PARAM) {
        cout << "param " << get_operand_string(instruction->result);
    }
    else if (instruction->op.type == TAC_OPERATOR_CALL) {
        if (instruction->result->value == "") {
            // If the result is empty, it means it's a void function call
            cout << "call " << get_operand_string(instruction->arg1) << ", "
                << get_operand_string(instruction->arg2);
        }
        else {
            cout << get_operand_string(instruction->result) << " = call "
                << get_operand_string(instruction->arg1) << ", "
                << get_operand_string(instruction->arg2);
        }
    }
    else if (instruction->op.type == TAC_OPERATOR_RETURN) { // MAY NEED TO CHANGE
        cout << "return " << get_operand_string(instruction->result);
    }
    else if(instruction->op.type == TAC_OPERATOR_FUNC_BEGIN) {
        cout << "function " << get_operand_string(instruction->result);
    }
    else if (instruction->op.type == TAC_OPERATOR_FUNC_END) {
        cout << "end function "<< get_operand_string(instruction->result);
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
            cout << get_operand_string(instruction->result) << " = "
                << get_operand_string(instruction->arg1) << " "
                << get_operator_string(instruction->op.type) << " "
                << get_operand_string(instruction->arg2);
        }
        else if (instruction->op.type != TAC_OPERATOR_NOP) {
            // Unary operation: `x = op y`
            cout << get_operand_string(instruction->result) << " = "
                << get_operator_string(instruction->op.type) << " "
                << get_operand_string(instruction->arg1);
        }
        else {
            // Simple assignment: `x = y`
            cout << get_operand_string(instruction->result) << " = "
                << get_operand_string(instruction->arg1);
        }
    }
    else cout << "Nothing to print";
    cout << endl;
}

void print_TAC() {
    cout << "===== Three-Address Code (TAC) =====" << endl;
    for (int i = 0; i < TAC_CODE.size(); ++i) {
         // Stop when we reach uninitialized entries
         TACInstruction* instruction = TAC_CODE[i];
         if(instruction == nullptr) continue; // Stop printing if we reach an uninitialized entry  
        print_TAC_instruction(instruction); // Print each instruction
    }
    cout << "====================================" << endl;
    // remove_dead_code();
}


void print_code_vector(vector<TACInstruction*>& code) {
    cout << "===== Three-Address Code (TAC) intermediate =====" << endl;
    for (int i = 0; i < code.size(); ++i) {
         // Stop when we reach uninitialized entries
        TACInstruction* instruction = code[i];
        print_TAC_instruction(instruction); // Print each instruction
    }
    cout << "====================================" << endl;
}

void fix_labels_temps(){
    map<string,int> labels;
    int label_ct = 1;

    for(int i=0;i<TAC_CODE.size();i++){
        TACInstruction* instr = TAC_CODE[i];
        if (TAC_CODE[i]->result->type == TAC_OPERAND_EMPTY && instr->flag!=0) TAC_CODE.erase(TAC_CODE.begin() + i); // Remove uninitialized entries
        if(TAC_CODE[i]->label->type == TAC_OPERAND_LABEL && TAC_CODE[i]->label->value != ""){
            if(labels.find(TAC_CODE[i]->label->value) == labels.end()){
                labels[TAC_CODE[i]->label->value] = label_ct;
                label_ct++;
            }
        }
    }

    map<string,int> temps;
    int temp_ct = 1;
    for(int i=0;i<TAC_CODE.size();i++){
        // if(TAC_CODE[i]->result->type == TAC_OPERAND_TEMP_VAR && TAC_CODE[i]->result->value != ""){
        //     if(temps.find(TAC_CODE[i]->result->value) == temps.end()){
        //         temps[TAC_CODE[i]->result->value] = temp_ct;
        //         temp_ct++;
        //     }
        // }
        // if(TAC_CODE[i]->arg1->type == TAC_OPERAND_TEMP_VAR && TAC_CODE[i]->arg1->value != ""){
        //     if(temps.find(TAC_CODE[i]->arg1->value) == temps.end()){
        //         temps[TAC_CODE[i]->arg1->value] = temp_ct;
        //         temp_ct++;
        //     }
        // }
        // if(TAC_CODE[i]->arg2->type == TAC_OPERAND_TEMP_VAR && TAC_CODE[i]->arg2->value != ""){
        //     if(temps.find(TAC_CODE[i]->arg2->value) == temps.end()){
        //         temps[TAC_CODE[i]->arg2->value] = temp_ct;
        //         temp_ct++;
        //     }
        // }
    }

    for(int i=0;i<TAC_CODE.size();i++){
        if(TAC_CODE[i]->label->type == TAC_OPERAND_LABEL && TAC_CODE[i]->label->value != ""){
            TAC_CODE[i]->label->value = to_string(labels[TAC_CODE[i]->label->value]);
        }
        // if(TAC_CODE[i]->result->type == TAC_OPERAND_LABEL && TAC_CODE[i]->result->value != ""){
        //     TAC_CODE[i]->result->value = to_string(labels[TAC_CODE[i]->result->value]);
        // }
        // if(TAC_CODE[i]->result->type == TAC_OPERAND_TEMP_VAR && TAC_CODE[i]->result->value != ""){
        //     TAC_CODE[i]->result->value = "t" + to_string(temps[TAC_CODE[i]->result->value]);
        // }
        // if(TAC_CODE[i]->arg1->type == TAC_OPERAND_TEMP_VAR && TAC_CODE[i]->arg1->value != ""){
        //     TAC_CODE[i]->arg1->value = "t" + to_string(temps[TAC_CODE[i]->arg1->value]);
        // }
        // if(TAC_CODE[i]->arg2->type == TAC_OPERAND_TEMP_VAR && TAC_CODE[i]->arg2->value != ""){
        //     TAC_CODE[i]->arg2->value = "t" + to_string(temps[TAC_CODE[i]->arg2->value]);
        // }
    }
    // cout<<labels["22"]<<endl;
}

const char* getOperatorName(TACOperatorType op) {
    switch (op) {
        #define CASE(x) case x: return #x;
        CASE(TAC_OPERATOR_ADD)
        CASE(TAC_OPERATOR_SUB)
        CASE(TAC_OPERATOR_MUL)
        CASE(TAC_OPERATOR_DIV)
        CASE(TAC_OPERATOR_MOD)
        CASE(TAC_OPERATOR_UMINUS)
        CASE(TAC_OPERATOR_EQ)
        CASE(TAC_OPERATOR_NE)
        CASE(TAC_OPERATOR_GT)
        CASE(TAC_OPERATOR_LT)
        CASE(TAC_OPERATOR_GE)
        CASE(TAC_OPERATOR_LE)
        CASE(TAC_OPERATOR_AND)
        CASE(TAC_OPERATOR_OR)
        CASE(TAC_OPERATOR_NOT)
        CASE(TAC_OPERATOR_BIT_AND)
        CASE(TAC_OPERATOR_BIT_OR)
        CASE(TAC_OPERATOR_BIT_XOR)
        CASE(TAC_OPERATOR_LEFT_SHIFT)
        CASE(TAC_OPERATOR_RIGHT_SHIFT)
        CASE(TAC_OPERATOR_BIT_NOT)
        CASE(TAC_OPERATOR_ASSIGN)
        CASE(TAC_OPERATOR_ADDR_OF)
        CASE(TAC_OPERATOR_DEREF)
        CASE(TAC_OPERATOR_CAST)
        CASE(TAC_OPERATOR_GOTO)
        CASE(TAC_OPERATOR_IF_GOTO)
        CASE(TAC_OPERATOR_LABEL)
        CASE(TAC_OPERATOR_CALL)
        CASE(TAC_OPERATOR_RETURN)
        CASE(TAC_OPERATOR_PARAM)
        CASE(TAC_OPERATOR_FUNC_BEGIN)
        CASE(TAC_OPERATOR_FUNC_END)
        CASE(TAC_OPERATOR_INDEX)
        CASE(TAC_OPERATOR_INDEX_ASSIGN)
        CASE(TAC_OPERATOR_NOP)
        #undef CASE
        default: return "UNKNOWN_OPERATOR";
    }
}

void eliminateDeadCode(std::vector<TACInstruction*>& TAC_CODE) {
    using std::string;
    using std::unordered_set;
    using std::vector;

    unordered_set<string> usedVars;
    unordered_set<string> referencedLabels;

    // === Phase 1: Collect all referenced labels (i.e., jump targets) ===
    for (const auto* instr : TAC_CODE) {
        if (!instr) continue;
        const TACOperatorType& opType = instr->op.type;
        bool is_goto = instr->flag == 1;
        bool is_if_goto = instr->flag == 2;

        if ((is_goto || is_if_goto) && instr->result) {
            const string& label = instr->result->value;
            if (!label.empty()) {
                referencedLabels.insert(label);
            }
        }
    }

    // === Phase 2: Backward pass to track variable liveness and eliminate dead code ===
    vector<TACInstruction*> new_TAC_CODE;
    for (int i = static_cast<int>(TAC_CODE.size()) - 1; i >= 0; --i) {
        TACInstruction* instr = TAC_CODE[i];
        if (!instr) continue;
        bool is_goto = instr->flag == 1;
        bool is_if_goto = instr->flag == 2;
        const TACOperatorType& type = instr->op.type;

        string resultVal = (instr->result ? instr->result->value : "");
        string arg1Val = (instr->arg1 ? instr->arg1->value : "");
        string arg2Val = (instr->arg2 ? instr->arg2->value : "");

        // === Always preserve instructions with side-effects or control flow impact ===
        bool hasSideEffect = (
            is_goto || is_if_goto ||
            type == TAC_OPERATOR_GOTO ||
            type == TAC_OPERATOR_IF_GOTO ||
            type == TAC_OPERATOR_CALL ||
            type == TAC_OPERATOR_RETURN ||
            type == TAC_OPERATOR_PARAM ||
            type == TAC_OPERATOR_FUNC_BEGIN ||
            type == TAC_OPERATOR_FUNC_END ||
            type == TAC_OPERATOR_DEREF ||
            type == TAC_OPERATOR_INDEX_ASSIGN
        );

        if (hasSideEffect) {
            if (!arg1Val.empty()) usedVars.insert(arg1Val);
            if (!arg2Val.empty()) usedVars.insert(arg2Val);
            if (!resultVal.empty()) usedVars.insert(resultVal);
            new_TAC_CODE.push_back(instr);
            continue;
        }

        // === Preserve labels if they are jump targets ===
        if (type == TAC_OPERATOR_LABEL && referencedLabels.count(resultVal)) {
            new_TAC_CODE.push_back(instr);
            continue;
        }

        // === Preserve if the result is live (used later) ===
        if (!resultVal.empty() && usedVars.count(resultVal)) {
            if (!arg1Val.empty()) usedVars.insert(arg1Val);
            if (!arg2Val.empty()) usedVars.insert(arg2Val);
            new_TAC_CODE.push_back(instr);
            continue;
        }

        // === Preserve if arguments are used and there’s no result (e.g., pointer loads) ===
        if (resultVal.empty() && (!arg1Val.empty() || !arg2Val.empty())) {
            if (!arg1Val.empty()) usedVars.insert(arg1Val);
            if (!arg2Val.empty()) usedVars.insert(arg2Val);
            new_TAC_CODE.push_back(instr);
            continue;
        }

        // === Otherwise, the instruction is dead and can be safely removed ===
    }

    // Restore original order
    std::reverse(new_TAC_CODE.begin(), new_TAC_CODE.end());
    TAC_CODE = std::move(new_TAC_CODE);
}


void remove_dead_code(){
    debug("Removing dead code", YELLOW);
    debug("TAC_CODE size before: " + to_string(TAC_CODE.size()), YELLOW);
    size_t prevSize;
    do {
        prevSize = TAC_CODE.size();
        eliminateDeadCode(TAC_CODE);
    } while (TAC_CODE.size() < prevSize);
    debug("TAC_CODE size after: " + to_string(TAC_CODE.size()), YELLOW);
}
