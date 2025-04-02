#include "tac.h"
#include "symbol_table.h"
#include <stdio.h>
#include <iostream>
using namespace std;

// Define the global map for identifiers
unordered_map<string, TACOperand*> identifiers;

// Define the global array for TAC instructions
TACInstruction* code[MAX_CODE_SIZE] = {nullptr};

//##############################################################################
//################################## TACOperand ######################################
//##############################################################################

TACOperand::TACOperand(TACOperandType type, string value) : type(type), value(value) {}

TACOperand new_temp_var(){
    string temp_var = "t" + to_string(temp_var_id++);
    return TACOperand(TAC_OPERAND_TEMP_VAR, temp_var);
}

TACOperand new_label(){
    string label = "L" + to_string(label_id++);
    return TACOperand(TAC_OPERAND_LABEL, label);
}

TACOperand new_constant(string value){
    return TACOperand(TAC_OPERAND_CONSTANT, value);
}

TACOperand new_identifier(string value){
    if(identifiers.find(value) == identifiers.end()) {
        TACOperand new_id = TACOperand(TAC_OPERAND_IDENTIFIER, value);
        identifiers[value] = &new_id;
        return new_id;
    }
    else {
        TACOperand existing_id = *identifiers[value];
        return existing_id;
    }
}

TACOperand new_pointer(string value){
    string pointer_value = value + "_ptr";
    return TACOperand(TAC_OPERAND_POINTER, pointer_value);
}

// TACOperand new_type(Type t){
    
//     return TACOperand(TAC_OPERAND_TYPE,);
// }

//##############################################################################
//################################## TACOperator ######################################
//##############################################################################
TACOperator::TACOperator() : type(TACOperatorType::TAC_OPERATOR_NOP) {}

TACOperator::TACOperator(TACOperatorType type) : type(type) {}

//##############################################################################
//################################## TACInstruction ######################################
//##############################################################################

TACInstruction::TACInstruction() : op(TACOperatorType::TAC_OPERATOR_NOP), result(TACOperandType::TAC_OPERAND_EMPTY, ""), arg1(TACOperandType::TAC_OPERAND_EMPTY, ""), arg2(TACOperandType::TAC_OPERAND_EMPTY, "") {}

TACInstruction::TACInstruction(TACOperator op, TACOperand result, TACOperand arg1, TACOperand arg2){
    this->id = instruction_id;
    this->op = op;
    this->result = result;
    this->arg1 = arg1;
    this->arg2 = arg2;
}

bool is_assignment(TACInstruction* instruction) {
    if(instruction->op.type == (TACOperatorType::TAC_OPERATOR_CALL || TACOperatorType::TAC_OPERATOR_PARAM || TACOperatorType::TAC_OPERATOR_RETURN)) {
        return false;
    } else return true;
}

void emit(TACOperator op, TACOperand result, TACOperand arg1, TACOperand arg2) {
    if(instruction_id >= MAX_CODE_SIZE) {
        cerr << "Error: Code size exceeded maximum limit." << endl;
        exit(1);
    }
    TACInstruction instruction(op, result, arg1, arg2);
    code[instruction_id++] = &instruction;
}

void backpatch(TACInstruction* instruction, TACOperand label) {
    if(instruction->op.type == TACOperatorType::TAC_OPERATOR_GOTO) {
        instruction->arg1 = label;
    } 
    else if(instruction->op.type == TACOperatorType::TAC_OPERATOR_IF_GOTO){
        instruction->arg2 = label;
    }
    code[instruction->id] = instruction;
}

unordered_set<TACInstruction*> merge_lists(unordered_set<TACInstruction*> &list1, unordered_set<TACInstruction*> &list2) {
    for (auto it = list2.begin(); it != list2.end(); ++it) {
        list1.insert(*it);
    }
    return list1;
}


