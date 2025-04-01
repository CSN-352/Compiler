#include<tac.h>

using namespace std;

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
    return TACOperand(TAC_OPERAND_IDENTIFIER, value);
}

//##############################################################################
//################################## TACOperator ######################################
//##############################################################################

TACOperator::TACOperator(TACOperatorType type) : type(type) {}

//##############################################################################
//################################## TACInstruction ######################################
//##############################################################################

TACInstruction::TACInstruction(TACOperator op, TACOperand result, TACOperand arg1, TACOperand arg2) : op(op), result(result), arg1(arg1), arg2(arg2) {}

TACInstruction::TACInstruction(TACOperator op, TACOperand result, TACOperand arg1) : op(op), result(result), arg1(arg1) {
    arg2 = TACOperand(TACOperandType::TAC_OPERAND_EMPTY, ""); // Initialize arg2 to empty
}

TACInstruction::TACInstruction(TACOperand result, TACOperand arg1) : result(result), arg1(arg1) {
    op = TACOperator(TACOperatorType::TAC_OPERATOR_NOP); // Default operator for assignment
    arg2 = TACOperand(TACOperandType::TAC_OPERAND_EMPTY, ""); // Initialize arg2 to empty
}

TACInstruction::TACInstruction(TACOperator op, TACOperand arg1) : op(op), arg1(arg1) {
    arg2 = TACOperand(TACOperandType::TAC_OPERAND_EMPTY, ""); // Initialize arg2 to empty
    result = TACOperand(TACOperandType::TAC_OPERAND_EMPTY, ""); // Initialize result to empty
}

TACInstruction::TACInstruction(TACOperator op, TACOperand arg1, TACOperand arg2) : op(op), arg1(arg1), arg2(arg2) {
    result = TACOperand(TACOperandType::TAC_OPERAND_EMPTY, ""); // Initialize result to empty
}

bool is_assignment(TACInstruction* instruction) {
    if(instruction->op.type == (TACOperatorType::TAC_OPERATOR_CALL || TACOperatorType::TAC_OPERATOR_PARAM || TACOperatorType::TAC_OPERATOR_RETURN)) {
        return false;
    } else return true;
}





