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

TACInstruction::TACInstruction(TACOperator op, TACOperand arg1, TACOperand arg2, TACOperand result)
    : op(op), arg1(arg1), arg2(arg2), result(result) {}

