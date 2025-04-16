#include "symbol_table.h"
#include "ast.h"
#include "expression.h"
#include "tac.h" 
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>
#include <utility>
#include <string>
#include <iterator>
#include <string.h>
extern void yyerror(const char* msg);
// extern void print_code_vector(vector<TACInstruction*>& v);
extern void print_code_vector(vector<TACInstruction*>& v);
using namespace std;
Type ERROR_TYPE;

// ##############################################################################
// ################################## EXPRESSION ######################################
// ##############################################################################

Expression::Expression() : NonTerminal("EXPRESSION") {
    operand_cnt = 0;
    result = new_empty_var();
}

// ##############################################################################
// ################################## PRIMARY EXPRESSION ######################################
// ##############################################################################

PrimaryExpression::PrimaryExpression() : Expression() {
    name = "PRIMARY EXPRESSION";
    identifier = nullptr;
    constant = nullptr;
    string_literal = nullptr;
}

Expression* create_primary_expression(Identifier* i) {
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION IDENTIFIER";
    P->line_no = i->line_no;
    P->column_no = i->column_no;
    P->identifier = i;
    Symbol* sym = symbolTable.getSymbol(i->value);
    if (sym) P->type = sym->type;
    else {
        P->type = ERROR_TYPE;
        string error_msg = "Undeclared Symbol " + i->value + " at line " + to_string(i->line_no) + ", column " + to_string(i->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    P->result = new_temp_var(); // TAC
    TACInstruction* i0 = emit(TACOperator(TAC_OPERATOR_NOP),P->result,new_identifier(sym->mangled_name), new_empty_var(), 0); // TAC
    P->code.push_back(i0); // TAC
    TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
    TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
    TACInstruction* i1_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
    TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
    P->true_list.insert(i1_);
    P->false_list.insert(i2_); // TAC
    P->jump_true_list.insert(i1);
    P->jump_false_list.insert(i2);
    P->jump_code.push_back(i0); // TAC
    P->jump_code.push_back(i1); // TAC
    P->jump_code.push_back(i2); // TAC
    P->jump_code.push_back(i1_); // TAC
    P->jump_code.push_back(i2_); // TAC

    return P;
}

Expression* create_primary_expression(Constant* x) {
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION CONSTANT";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->constant = x;
    P->type = x->get_constant_type();
    P->type.is_const_literal = true;
    P->result = new_constant(x->value); // TAC
    TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
    TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
    TACInstruction* i1_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
    TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
    P->jump_true_list.insert(i1);
    P->jump_false_list.insert(i2);
    P->true_list.insert(i1_);
    P->false_list.insert(i2_); // TAC
    P->jump_code.push_back(i1); // TAC
    P->jump_code.push_back(i2); // TAC
    P->jump_code.push_back(i1_); // TAC
    P->jump_code.push_back(i2_); // TAC
    return P;
}

Expression* create_primary_expression(StringLiteral* x) {
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION STRING LITERAL";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->string_literal = x;
    P->type = Type(CHAR_T, 1, true);
    P->result = new_temp_var(); // TAC
    TACInstruction* i0 = emit(TACOperator(TAC_OPERATOR_ADDR_OF), P->result, new_string(x->value), new_empty_var(), 0); // TAC
    TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
    TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
    TACInstruction* i1_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
    TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
    P->jump_true_list.insert(i1);
    P->jump_false_list.insert(i2);
    P->true_list.insert(i1_);
    P->false_list.insert(i2_); // TAC
    P->code.push_back(i0); // TAC
    P->jump_code.push_back(i0); // TAC
    P->jump_code.push_back(i1); // TAC
    P->jump_code.push_back(i2); // TAC
    P->jump_code.push_back(i1_); // TAC
    P->jump_code.push_back(i2_); // TAC
    return P;
}

Expression* create_primary_expression(Expression* x) {
    PrimaryExpression* P = new PrimaryExpression();
    P->type = x->type;
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->result = x->result; // TAC
    P->true_list = x->true_list; // TAC
    P->false_list = x->false_list; // TAC
    P->jump_false_list = x->jump_false_list; // TAC
    P->jump_true_list = x->jump_true_list; // TAC
    P->next_list = x->next_list; // TAC
    P->jump_next_list = x->jump_next_list; // TAC
    P->code = x->code; // TAC
    P->jump_code = x->jump_code; // TAC
    return x;
}

// ##############################################################################
// ################################## ARGUMENT EXPRESSION LIST ######################################
// ##############################################################################

ArgumentExpressionList::ArgumentExpressionList() : Expression() {
    name = "ARGUMENT EXPRESSION LIST";
    type = ERROR_TYPE;
};

ArgumentExpressionList* create_argument_expression_list(Expression* x) {
    ArgumentExpressionList* P = new ArgumentExpressionList();
    P->arguments.push_back(x);
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->type = x->type; // if argument expression list does not have an erronous expression, set type to the type of the first expression. Else set it as ERROR_TYPE.
    TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_PARAM), x->result, new_empty_var(), new_empty_var(), 0); // TAC
    backpatch(x->next_list, i1->label); // TAC
    backpatch(x->jump_next_list, i1->label); // TAC
    backpatch(x->true_list, i1->label); // TAC
    backpatch(x->false_list, i1->label); // TAC
    backpatch(x->jump_true_list, i1->label); // TAC
    backpatch(x->jump_false_list, i1->label); // TAC
    P->type.is_const_literal = false;
    P->code.insert(P->code.end(), x->code.begin(), x->code.end());
    P->code.push_back(i1); // TAC
    return P;
}

ArgumentExpressionList* create_argument_expression_list(ArgumentExpressionList* args_expr_list, Expression* x) {
    args_expr_list->arguments.push_back(x);
    if (x->type.is_error()) {
        args_expr_list->type = ERROR_TYPE;
    }
    TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_PARAM), x->result, new_empty_var(), new_empty_var(), 0); // TAC
    backpatch(x->next_list, i1->label); // TAC
    backpatch(x->jump_next_list, i1->label); // TAC
    backpatch(x->true_list, i1->label); // TAC
    backpatch(x->false_list, i1->label); // TAC
    backpatch(x->jump_true_list, i1->label); // TAC
    backpatch(x->jump_false_list, i1->label); // TAC
    args_expr_list->code.insert(args_expr_list->code.begin(), x->code.begin(), x->code.end());
    args_expr_list->code.push_back(i1); // TAC
    return args_expr_list;
}

// ##############################################################################
// ################################## POSTFIX EXPRESSION ######################################
// ##############################################################################

PostfixExpression::PostfixExpression() : Expression() {
    name = "POSTFIX EXPRESSION";
    primary_expression = nullptr;
    base_expression = nullptr;
    index_expression = nullptr;
    argument_expression_list = nullptr;
    op = nullptr;
    member_name = nullptr;
}

Expression* create_postfix_expression(Expression* x) {
    PostfixExpression* P = new PostfixExpression();
    P->primary_expression = dynamic_cast<PrimaryExpression*>(x);
    P->type = x->type;
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->result = x->result; // TAC
    P->true_list = x->true_list; // TAC
    P->false_list = x->false_list; // TAC
    P->jump_false_list = x->jump_false_list; // TAC
    P->jump_true_list = x->jump_true_list; // TAC
    P->code = x->code; // TAC
    P->jump_code = x->jump_code; // TAC
    P->next_list = x->next_list; // TAC
    P->jump_next_list = x->jump_next_list; // TAC
    return P;
}

Expression* create_postfix_expression(Expression* x, Terminal* op) {
    PostfixExpression* P = new PostfixExpression();
    P->base_expression = dynamic_cast<PostfixExpression*>(x);
    P->primary_expression = P->base_expression->primary_expression;
    P->op = op;
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->code.insert(P->code.begin(), x->code.begin(), x->code.end()); // TAC
    P->jump_code.insert(P->jump_code.begin(), x->code.begin(), x->code.end()); // TAC
    for(auto i:x->jump_next_list){
        P->code.erase(remove(P->code.begin(), P->code.end(), i), P->code.end()); // TAC
    }
    for(auto i:x->next_list){
        P->jump_code.erase(remove(P->jump_code.begin(), P->jump_code.end(), i), P->jump_code.end()); // TAC
    }
    if (op->name == "INC_OP") P->name = "POSTFIX EXPRESSION INC OP";
    else P->name = "POSTFIX EXPRESSION DEC OP";

    if (x->type.is_error()) {
        P->type = ERROR_TYPE;
        return P;
    }

    if (x->type.is_const_variable || x->type.is_array || x->type.is_function || x->type.is_defined_type) {
        P->type = ERROR_TYPE;
        string error_msg = "Invalid operator " + op->value + " at line  " + to_string(op->line_no) + ", column " + to_string(op->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    else if (x->type.isInt()) {
        P->type = x->type;
        P->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), P->result, x->result, new_empty_var(), 0); // TAC
        backpatch(x->next_list, i1->label); // TAC
        backpatch(x->jump_next_list, i1->label); // TAC
        backpatch(x->true_list, i1->label); // TAC
        backpatch(x->false_list, i1->label); // TAC
        backpatch(x->jump_true_list, i1->label); // TAC
        backpatch(x->jump_false_list, i1->label); // TAC
        TACInstruction* i2 = emit(TACOperator(op->name == "INC_OP" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), x->result, x->result, new_constant("1"), 0); // TAC
        P->code.push_back(i1); // TAC
        P->code.push_back(i2); // TAC
        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
        TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
        TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        P->true_list.insert(i3_); // TAC
        P->false_list.insert(i4_); // TAC
        P->jump_true_list.insert(i3); // TAC
        P->jump_false_list.insert(i4); // TAC
        P->jump_code.push_back(i1); // TAC
        P->jump_code.push_back(i2); // TAC
        P->jump_code.push_back(i3); // TAC
        P->jump_code.push_back(i4); // TAC
        P->jump_code.push_back(i3_); // TAC
        P->jump_code.push_back(i4_); // TAC
    }
    else if (x->type.isFloat()) {
        P->type = x->type;
        P->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), P->result, x->result, new_empty_var(), 0); // TAC
        backpatch(x->next_list, i1->label); // TAC
        backpatch(x->jump_next_list, i1->label); // TAC
        backpatch(x->true_list, i1->label); // TAC
        backpatch(x->false_list, i1->label); // TAC
        backpatch(x->jump_true_list, i1->label); // TAC
        backpatch(x->jump_false_list, i1->label); // TAC
        TACInstruction* i2 = emit(TACOperator(op->name == "INC_OP" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), x->result, x->result, new_constant("1"), 0); // TAC
        P->code.push_back(i1); // TAC
        P->code.push_back(i2); // TAC

        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
        TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
        TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        P->true_list.insert(i3_); // TAC
        P->false_list.insert(i4_); // TAC
        P->jump_true_list.insert(i3); // TAC
        P->jump_false_list.insert(i4); // TAC
        P->jump_code.push_back(i1); // TAC
        P->jump_code.push_back(i2); // TAC
        P->jump_code.push_back(i3); // TAC
        P->jump_code.push_back(i4); // TAC
        P->jump_code.push_back(i3_); // TAC
        P->jump_code.push_back(i4_); // TAC
    }
    else if (x->type.is_pointer) {
        P->type = x->type;
        P->result = x->result; // TAC
        P->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), P->result, x->result, new_empty_var(), 0); // TAC
        backpatch(x->next_list, i1->label); // TAC
        backpatch(x->jump_next_list, i1->label); // TAC
        backpatch(x->true_list, i1->label); // TAC
        backpatch(x->false_list, i1->label); // TAC
        backpatch(x->jump_true_list, i1->label); // TAC
        backpatch(x->jump_false_list, i1->label); // TAC
        TACInstruction* i2 = emit(TACOperator(op->name == "INC_OP" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), x->result, x->result, new_constant(to_string(x->type.get_size())), 0); // TAC
        P->code.push_back(i1); // TAC
        P->code.push_back(i2); // TAC

        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
        TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
        TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        P->true_list.insert(i3_); // TAC
        P->false_list.insert(i4_); // TAC
        P->jump_true_list.insert(i3); // TAC
        P->jump_false_list.insert(i4); // TAC
        P->jump_code.push_back(i1); // TAC
        P->jump_code.push_back(i2); // TAC
        P->jump_code.push_back(i3); // TAC
        P->jump_code.push_back(i4); // TAC
        P->jump_code.push_back(i3_); // TAC
        P->jump_code.push_back(i4_); // TAC
    }
    else {
        P->type = ERROR_TYPE;
        string error_msg = "Invalid operator " + op->value + " at line  " + to_string(op->line_no) + ", column " + to_string(op->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    P->type.is_const_literal = false;
    return P;
}

Expression* create_postfix_expression(Expression* x, Terminal* op, Identifier* id) {
    PostfixExpression* P = new PostfixExpression();
    P->base_expression = dynamic_cast<PostfixExpression*>(x);
    P->primary_expression = P->base_expression->primary_expression;
    P->op = op;
    P->member_name = id;
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->code.insert(P->code.begin(), x->code.begin(), x->code.end()); // TAC
    P->jump_code.insert(P->jump_code.begin(), x->code.begin(), x->code.end()); // TAC
    for(auto i:x->jump_next_list){
        P->code.erase(remove(P->code.begin(), P->code.end(), i), P->code.end()); // TAC
    }
    for(auto i:x->next_list){
        P->jump_code.erase(remove(P->jump_code.begin(), P->jump_code.end(), i), P->jump_code.end()); // TAC
    }

    if (x->type.is_error()) {
        P->type = ERROR_TYPE;
        return P;
    }

    if (op->name == "PTR_OP") P->name = "POSTFIX EXPRESSION PTR_OP";
    else P->name = "POSTFIX EXPRESSION DOT";
    if (op->name == "PTR_OP") {
        if (!(x->type.ptr_level == 1) && !(x->type.is_defined_type)) {
            P->type = ERROR_TYPE;
            string error_msg = "Operator '->' applied to non-pointer-to-struct/union/class object at line " + to_string(op->line_no) + ", column " + to_string(op->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return P;
        }
    }

    else if (op->name == "DOT") {
        if (!(x->type.ptr_level == 0) && !(x->type.is_defined_type)) {
            P->type = ERROR_TYPE;
            string error_msg = "Operator '.' applied to non-struct/union/class object at line " + to_string(op->line_no) + ", column " + to_string(op->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return P;
        }
    }

    if (!symbolTable.lookup_defined_type(x->type.defined_type_name)) {
        P->type = ERROR_TYPE;
        string error_msg = "Defined_Type not found in Symbol Table " + to_string(op->line_no) + ", column " + to_string(op->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    else if (!symbolTable.check_member_variable(x->type.defined_type_name, id->value)) {
        P->type = ERROR_TYPE;
        // string error_msg = "Defined_Type does not have member_variable with name " + id->value + to_string(op->line_no) + ", column " + to_string(op->column_no);
        // yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    else {
        P->type = symbolTable.get_type_of_member_variable(x->type.defined_type_name, id->value); // Data types only
        TypeDefinition* td = symbolTable.get_defined_type(x->type.defined_type_name)->type_definition;
        Symbol* member = td->type_symbol_table.getSymbol(id->value);
        P->member_name = id;
        if (op->name == "DOT") {
            TACOperand* t1 = new_temp_var(); // TAC
            TACOperand* t2 = new_temp_var(); // TAC
            TACOperand* t3 = new_temp_var(); // TAC
            P->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_ADDR_OF), t1, x->result, new_empty_var(), 0); // TAC
            backpatch(x->next_list, i1->label); // TAC
            backpatch(x->jump_next_list, i1->label); // TAC
            backpatch(x->true_list, i1->label); // TAC
            backpatch(x->false_list, i1->label); // TAC
            backpatch(x->jump_true_list, i1->label); // TAC
            backpatch(x->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_ADD), t2, t1, new_constant(to_string(member->offset)), 0); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_DEREF), t3, t2, new_empty_var(), 0); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), P->result, t3, new_empty_var(), 0); // TAC
            P->code.push_back(i1); // TAC
            P->code.push_back(i2); // TAC
            P->code.push_back(i3); // TAC
            P->code.push_back(i4); // TAC

            TACInstruction* i5 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
            TACInstruction* i6 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i5_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
            TACInstruction* i6_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            P->true_list.insert(i5_); // TAC
            P->false_list.insert(i6_); // TAC
            P->jump_true_list.insert(i5); // TAC
            P->jump_false_list.insert(i6); // TAC
            P->jump_code.push_back(i1); // TAC
            P->jump_code.push_back(i2); // TAC
            P->jump_code.push_back(i3); // TAC
            P->jump_code.push_back(i4); // TAC
            P->jump_code.push_back(i5); // TAC
            P->jump_code.push_back(i6); // TAC
            P->jump_code.push_back(i5_); // TAC
            P->jump_code.push_back(i6_); // TAC
        }
        else if (op->name == "PTR_OP") {
            TACOperand* t1 = new_temp_var(); // TAC
            TACOperand* t2 = new_temp_var(); // TAC
            P->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_ADD), t1, x->result, new_constant(to_string(member->offset)), 0); // TAC
            backpatch(x->next_list, i1->label); // TAC
            backpatch(x->jump_next_list, i1->label); // TAC
            backpatch(x->true_list, i1->label); // TAC
            backpatch(x->false_list, i1->label); // TAC
            backpatch(x->jump_true_list, i1->label); // TAC
            backpatch(x->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_DEREF), t2, t1, new_empty_var(), 0); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), P->result, t2, new_empty_var(), 0); // TAC
            P->code.push_back(i1); // TAC   
            P->code.push_back(i2); // TAC
            P->code.push_back(i3); // TAC

            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
            TACInstruction* i5 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
            TACInstruction* i5_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            P->true_list.insert(i4_); // TAC
            P->false_list.insert(i5_); // TAC
            P->jump_true_list.insert(i4); // TAC
            P->jump_false_list.insert(i5); // TAC
            P->jump_code.push_back(i1); // TAC
            P->jump_code.push_back(i2); // TAC
            P->jump_code.push_back(i3); // TAC
            P->jump_code.push_back(i4); // TAC
            P->jump_code.push_back(i5); // TAC
            P->jump_code.push_back(i4_); // TAC
            P->jump_code.push_back(i5_); // TAC
        }
    }
    P->type.is_const_literal = false;
    return P;
}

Expression* create_postfix_expression(Expression* x, Expression* index_expression) {
    PostfixExpression* P = new PostfixExpression();
    P->name = "POSTFIX EXPRESSION ARRAY ACCESS";
    P->base_expression = dynamic_cast<PostfixExpression*>(x);
    P->primary_expression = P->base_expression->primary_expression;
    P->index_expression = index_expression;
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->code.insert(P->code.begin(), x->code.begin(), x->code.end()); // TAC
    P->code.insert(P->code.end(), index_expression->code.begin(), index_expression->code.end()); // TAC
    P->jump_code.insert(P->jump_code.begin(), x->code.begin(), x->code.end()); // TAC
    P->jump_code.insert(P->jump_code.end(), index_expression->code.begin(), index_expression->code.end()); // TAC
    for(auto i : x->jump_next_list) {
        P->code.erase(remove(P->code.begin(), P->code.end(), i), P->code.end()); // TAC
    }
    for(auto i : x->next_list) {
        P->jump_code.erase(remove(P->jump_code.begin(), P->jump_code.end(), i), P->jump_code.end()); // TAC
    }
    for(auto i : index_expression->jump_next_list) {
        P->code.erase(remove(P->code.begin(), P->code.end(), i), P->code.end()); // TAC
    }
    for(auto i : index_expression->next_list) {
        P->jump_code.erase(remove(P->jump_code.begin(), P->jump_code.end(), i), P->jump_code.end()); // TAC
    }

    if (x->type.is_error()) {
        P->type = ERROR_TYPE;
        return P;
    }
    // Type checking
    if (!x->type.is_pointer && !x->type.is_array) {
        P->type = ERROR_TYPE;
        string error_msg = "Cannot index non-array/pointer at line " + to_string(index_expression->line_no) + ", column " + to_string(index_expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    else if (!index_expression->type.isInt()) {
        P->type = ERROR_TYPE;
        string err = "Array index must be an integer at line " + to_string(index_expression->line_no) + ", column " + to_string(index_expression->column_no);
        yyerror(err.c_str());
        symbolTable.set_error();
        return P;
    }
    else {
        if (!symbolTable.lookup(P->primary_expression->identifier->value)) {
            P->type = ERROR_TYPE;
            string err = "Array not declared at line " + to_string(index_expression->line_no) + ", column " + to_string(index_expression->column_no);
            yyerror(err.c_str());
            symbolTable.set_error();
            return P;
        }
        else {
            P->type = x->type;
            if (x->type.is_array) {
                P->type.array_dim--;
                P->type.array_dims.erase(P->type.array_dims.begin());
                if (P->type.array_dim == 0){
                    P->type.is_array = false;
                    P->type.ptr_level--;
                }
            }
            else P->type.ptr_level--;
            if (P->type.ptr_level == 0) P->type.is_pointer = false;
        }
    }
    TACOperand* t1 = new_temp_var(); // TAC
    P->result = new_temp_var(); // TAC
    TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_MUL), t1, index_expression->result, new_constant(to_string(P->type.get_size())), 0); // TAC
    backpatch(x->next_list, i1->label); // TAC
    backpatch(x->jump_next_list, i1->label); // TAC
    backpatch(index_expression->next_list, i1->label); // TAC
    backpatch(index_expression->jump_next_list, i1->label); // TAC
    backpatch(x->true_list, i1->label); // TAC
    backpatch(x->false_list, i1->label); // TAC
    backpatch(x->jump_true_list, i1->label); // TAC
    backpatch(x->jump_false_list, i1->label); // TAC
    backpatch(index_expression->true_list, i1->label); // TAC
    backpatch(index_expression->false_list, i1->label); // TAC
    backpatch(index_expression->jump_true_list, i1->label); // TAC
    backpatch(index_expression->jump_false_list, i1->label); // TAC
    P->code.push_back(i1); // TAC
    P->jump_code.push_back(i1); // TAC
    if(P->type.ptr_level == 0){
        TACOperand* t2 = new_temp_var(); // TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_ADD), t2, x->result, t1, 0); // TAC
        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_DEREF), P->result, t2, new_empty_var(), 0); // TAC
        P->code.push_back(i2); // TAC
        P->jump_code.push_back(i2); // TAC
        P->code.push_back(i3); // TAC
        P->jump_code.push_back(i3); // TAC
    }
    else {
        TACInstruction* i2 = emit(TAC_OPERATOR_ADD, P->result, x->result, t1, 0); // TAC
        P->code.push_back(i2); // TAC
        P->jump_code.push_back(i2); // TAC
    }
    TACInstruction* i5 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
    TACInstruction* i6 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
    TACInstruction* i5_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
    TACInstruction* i6_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
    P->true_list.insert(i5_); // TAC
    P->false_list.insert(i6_); // TAC
    P->jump_true_list.insert(i5); // TAC
    P->jump_false_list.insert(i6); // TAC
    P->jump_code.push_back(i5); // TAC
    P->jump_code.push_back(i6); // TAC
    P->jump_code.push_back(i5_); // TAC
    P->jump_code.push_back(i6_); // TAC
    P->type.is_const_literal = false;
    return P;
}

Expression* create_postfix_expression_func(Expression* x, ArgumentExpressionList* argument_expression_list) {
    // check if exist in symbol table and arguements no. and types
    PostfixExpression* P = new PostfixExpression();
    P->name = "POSTFIX EXPRESSION FUNCTION CALL";
    P->base_expression = dynamic_cast<PostfixExpression*>(x);
    P->primary_expression = P->base_expression->primary_expression;
    P->argument_expression_list = argument_expression_list;
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->code.insert(P->code.begin(), x->code.begin(), x->code.end()); // TAC
    P->jump_code.insert(P->jump_code.begin(), x->code.begin(), x->code.end()); // TAC
    for(auto i:x->jump_next_list){
        P->code.erase(remove(P->code.begin(), P->code.end(), i), P->code.end()); // TAC
    }
    for(auto i:x->next_list){
        P->jump_code.erase(remove(P->jump_code.begin(), P->jump_code.end(), i), P->jump_code.end()); // TAC
    }

    vector<Type> arguments;
    if (argument_expression_list != nullptr) { // for function call with no args
        for (int i = 0;i < argument_expression_list->arguments.size();i++) {
            arguments.push_back(argument_expression_list->arguments[i]->type);
        }
    }
    // Type check: should be a function or function pointer
    if (!x->type.is_function && !(x->type.is_function && x->type.is_pointer)) {
        P->type = ERROR_TYPE;
        string error_msg = "Called object is not a function at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);;
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    else {
        if (!symbolTable.lookup_function(P->primary_expression->identifier->value, arguments) && (P->base_expression->member_name==nullptr || !symbolTable.check_member_variable(P->base_expression->base_expression->type.defined_type_name, P->base_expression->member_name->value))) {
            P->type = ERROR_TYPE;
            string error_msg = "No matching function declaration found " + to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return P;
        }
        else {
            Symbol* sym;
            if(P->base_expression->member_name != nullptr){
                if(P->base_expression->base_expression->type.defined_type_name != ""){
                    auto dt = symbolTable.get_defined_type(P->base_expression->base_expression->type.defined_type_name);
                    if(dt == nullptr){
                        P->type = ERROR_TYPE;
                        string error_msg = "Defined_Type not found in Symbol Table " + to_string(x->line_no) + ", column " + to_string(x->column_no);
                        yyerror(error_msg.c_str());
                        symbolTable.set_error();
                        return P;
                    }
                    sym = dt->type_definition->type_symbol_table.getSymbol(P->base_expression->member_name->value);
                }
            }
            else sym = symbolTable.getSymbol(P->primary_expression->identifier->value);
            if (sym == nullptr) {
                P->type = ERROR_TYPE;
                string error_msg = "Function " + P->primary_expression->identifier->value + " not found";
                yyerror(error_msg.c_str());
                symbolTable.set_error();
                return P;
            }
            FunctionDefinition* fd = sym->function_definition;
            if (fd == nullptr) {
                P->type = ERROR_TYPE;
                string error_msg = "Function " + sym->name + " is declared but not defined at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
                yyerror(error_msg.c_str());
                symbolTable.set_error();
                return P;
            }
            else {
                P->type = x->type;
                P->type.is_function = false;
                P->type.num_args = 0;
                P->type.arg_types.clear();
                P->result = new_temp_var(); // TAC
                TACInstruction* i1;
                if(argument_expression_list != nullptr)P->code.insert(P->code.begin(),argument_expression_list->code.begin(),argument_expression_list->code.end()); // TAC
                if (x->type.type_index == PrimitiveTypes::VOID_T){
                    i1 = emit(TACOperator(TAC_OPERATOR_CALL), new_constant(""), x->result, new_constant(to_string(arguments.size())), 0); // TAC
                }
                else{
                    i1 = emit(TACOperator(TAC_OPERATOR_CALL), P->result, x->result, new_constant(to_string(arguments.size())), 0); // TAC
                }
                
                backpatch(x->next_list, i1->label); // TAC
                backpatch(x->jump_next_list, i1->label); // TAC
                backpatch(x->true_list, i1->label); // TAC
                backpatch(x->false_list, i1->label); // TAC
                backpatch(x->jump_true_list, i1->label); // TAC
                backpatch(x->jump_false_list, i1->label); // TAC
                P->code.push_back(i1); // TAC

                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), P->result, new_empty_var(), 2); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                P->true_list.insert(i2_); // TAC
                P->false_list.insert(i3_); // TAC
                P->jump_true_list.insert(i2); // TAC
                P->jump_false_list.insert(i3); // TAC
                P->jump_code.push_back(i1); // TAC
                P->jump_code.push_back(i2); // TAC
                P->jump_code.push_back(i3); // TAC
                P->jump_code.push_back(i2_); // TAC
                P->jump_code.push_back(i3_); // TAC
            }
        }
    }
    P->type.is_const_literal = false;
    return P;
}

// ##############################################################################
// ################################## UNARY EXPRESSION ######################################
// ##############################################################################

UnaryExpression::UnaryExpression() : Expression() {
    name = "UNARY EXPRESSION";
    postfix_expression = nullptr;
    op = nullptr;
    base_expression = nullptr;
    cast_expression = nullptr;
    type_name = nullptr;
}

Expression* create_unary_expression(Expression* x) {
    UnaryExpression* U = new UnaryExpression();
    U->postfix_expression = dynamic_cast<PostfixExpression*>(x);
    U->line_no = x->line_no;
    U->column_no = x->column_no;
    U->type = x->type;
    U->result = x->result; // TAC
    U->true_list = x->true_list; // TAC 
    U->false_list = x->false_list; // TAC
    U->jump_false_list = x->jump_false_list; // TAC
    U->jump_true_list = x->jump_true_list; // TAC
    U->code = x->code; // TAC
    U->jump_code = x->jump_code; // TAC
    U->next_list = x->next_list; // TAC
    U->jump_next_list = x->jump_next_list; // TAC
    return U;
}

Expression* create_unary_expression(Expression* x, Terminal* op) {
    UnaryExpression* U = new UnaryExpression();
    U->base_expression = dynamic_cast<UnaryExpression*> (x);
    U->op = op;
    U->postfix_expression = U->base_expression->postfix_expression;
    U->line_no = x->line_no;
    U->column_no = x->column_no;
    U->code.insert(U->code.begin(), x->code.begin(), x->code.end()); // TAC
    U->jump_code.insert(U->jump_code.begin(), x->code.begin(), x->code.end()); // TAC
    for(auto i:x->jump_next_list){
        U->code.erase(remove(U->code.begin(), U->code.end(), i), U->code.end()); // TAC
    }
    for(auto i:x->next_list){
        U->jump_code.erase(remove(U->jump_code.begin(), U->jump_code.end(), i), U->jump_code.end()); // TAC
    }

    if (x->type.is_error()) {
        U->type = ERROR_TYPE;
        return U;
    }

    if (op->name == "INC_OP") U->name = "UNARY EXPRESSION INC_OP";
    else if (op->name == "DEC_OP") U->name = "UNARY EXPRESSION DEC_OP";
    else U->name = "UNARY EXPRESSION SIZEOF";

    if (op->name == "INC_OP" || op->name == "DEC_OP")
    {
        if (x->type.is_const_variable)
        {
            U->type = ERROR_TYPE;
            string error_msg = "Invalid operator with constant type " + to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return U;
        }

        if (x->type.is_pointer || x->type.isIntorFloat())
        {
            U->type = x->type;
            U->result = new_temp_var(); // TAC
            TACInstruction* i1;
            TACInstruction* i2;
            if (x->type.is_pointer) {
                i1 = emit(TACOperator(op->name == "INC_OP" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), U->result, x->result, new_constant(to_string(x->type.get_size())), 0); // TAC
                i2 = emit(TACOperator(TAC_OPERATOR_NOP), x->result, U->result, new_empty_var(), 0); // TAC
            }
            else {
                i1 = emit(TACOperator(op->name == "INC_OP" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), U->result, x->result, new_constant("1"), 0); // TAC        
                i2 = emit(TACOperator(TAC_OPERATOR_NOP), x->result, U->result, new_empty_var(), 0); // TAC
            }
            backpatch(x->next_list, i1->label); // TAC
            backpatch(x->jump_next_list, i1->label); // TAC
            backpatch(x->true_list, i1->label); // TAC
            backpatch(x->false_list, i1->label); // TAC
            backpatch(x->jump_true_list, i1->label); // TAC
            backpatch(x->jump_false_list, i1->label); // TAC
            U->code.push_back(i1); // TAC
            U->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            U->true_list.insert(i3_); // TAC
            U->false_list.insert(i4_); // TAC
            U->jump_true_list.insert(i3); // TAC
            U->jump_false_list.insert(i4); // TAC
            U->jump_code.push_back(i1); // TAC
            U->jump_code.push_back(i2); // TAC
            U->jump_code.push_back(i3); // TAC
            U->jump_code.push_back(i4); // TAC
            U->jump_code.push_back(i3_); // TAC
            U->jump_code.push_back(i4_); // TAC
        }
        else
        {
            U->type = ERROR_TYPE;
            string error_msg = "Operator with invalid type " + to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return U;
        }

    }
    else if (op->name == "SIZEOF")
    {
        U->type = Type(PrimitiveTypes::INT_T, 0, true);
        U->result = new_temp_var(); //TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), U->result, new_constant(to_string(x->type.get_size())), new_empty_var(), 0); // TAC
        backpatch(x->next_list, i1->label); // TAC
        backpatch(x->jump_next_list, i1->label); // TAC
        backpatch(x->true_list, i1->label); // TAC
        backpatch(x->false_list, i1->label); // TAC
        backpatch(x->jump_true_list, i1->label); // TAC
        backpatch(x->jump_false_list, i1->label); // TAC
        U->code.push_back(i1); // TAC

        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        U->true_list.insert(i2_); // TAC
        U->false_list.insert(i3_); // TAC
        U->jump_true_list.insert(i2); // TAC
        U->jump_false_list.insert(i3); // TAC
        U->jump_code.push_back(i1); // TAC
        U->jump_code.push_back(i2); // TAC
        U->jump_code.push_back(i3); // TAC
        U->jump_code.push_back(i2_); // TAC
        U->jump_code.push_back(i3_); // TAC
    }
    U->type.is_const_literal = false;
    return U;
}

Expression* create_unary_expression_cast(Expression* x, Terminal* op)
{
    UnaryExpression* U = new UnaryExpression();
    U->op = op;
    U->cast_expression = dynamic_cast<CastExpression*> (x);
    //U->postfix_expression = U->base_expression->postfix_expression;
    U->line_no = x->line_no;
    U->column_no = x->column_no;

    U->code.insert(U->code.begin(), x->code.begin(), x->code.end()); // TAC
    U->jump_code.insert(U->jump_code.begin(), x->code.begin(), x->code.end()); // TAC
    for(auto i:x->jump_next_list){
        U->code.erase(remove(U->code.begin(), U->code.end(), i), U->code.end()); // TAC
    }
    for(auto i:x->next_list){
        U->jump_code.erase(remove(U->jump_code.begin(), U->jump_code.end(), i), U->jump_code.end()); // TAC
    }

    if (x->type.is_error()) {
        U->type = ERROR_TYPE;
        return U;
    }

    if (op->name == "BITWISE_AND") U->name = "UNARY EXPRESSION BITWISE_AND";
    else if (op->name == "MULTIPLY") U->name = "UNARY EXPRESSION MULTIPLY";
    else if (op->name == "PLUS") U->name = "UNARY EXPRESSION PLUS";
    else if (op->name == "MINUS") U->name = "UNARY EXPRESSION MINUS";
    else if (op->name == "NOT") U->name = "UNARY EXPRESSION NOT";
    else U->name = "UNARY EXPRESSION BITWISE_NOT";

    if (op->name == "BITWISE_AND")
    {
        if (x->type.is_const_literal) {
            U->type = ERROR_TYPE;
            string err = "Cannot take address of a constant literal at line " +
                to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(err.c_str());
            symbolTable.set_error();
            return U;
        }
        U->type = x->type;
        U->type.ptr_level++;
        U->type.is_pointer = true;
        U->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TAC_OPERATOR_ADDR_OF, U->result, x->result, new_empty_var(), 0); // TAC
        backpatch(x->next_list, i1->label); // TAC
        backpatch(x->jump_next_list, i1->label); // TAC
        backpatch(x->true_list, i1->label); // TAC
        backpatch(x->false_list, i1->label); // TAC
        backpatch(x->jump_true_list, i1->label); // TAC
        backpatch(x->jump_false_list, i1->label); // TAC
        U->code.push_back(i1); // TAC

        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        U->true_list.insert(i2_); // TAC
        U->false_list.insert(i3_); // TAC
        U->jump_true_list.insert(i2); // TAC
        U->jump_false_list.insert(i3); // TAC
        U->jump_code.push_back(i1); // TAC
        U->jump_code.push_back(i2); // TAC
        U->jump_code.push_back(i3); // TAC
        U->jump_code.push_back(i2_); // TAC
        U->jump_code.push_back(i3_); // TAC
    }
    else if (op->name == "MULTIPLY")
    {
        if (x->type.ptr_level <= 0)
        {
            U->type = ERROR_TYPE;
            string error_msg = "Cannot dereference a non-pointer at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return U;
        }
        U->type = x->type;
        U->type.ptr_level--;
        if (U->type.ptr_level == 0)
        {
            U->type.is_pointer = false;
        }
        U->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_DEREF), U->result, x->result, new_empty_var(), 0); // TAC
        backpatch(x->next_list, i1->label); // TAC
        backpatch(x->jump_next_list, i1->label); // TAC
        backpatch(x->true_list, i1->label); // TAC
        backpatch(x->false_list, i1->label); // TAC
        backpatch(x->jump_true_list, i1->label); // TAC
        backpatch(x->jump_false_list, i1->label); // TAC
        U->code.push_back(i1); // TAC

        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        U->true_list.insert(i2_); // TAC
        U->false_list.insert(i3_); // TAC
        U->jump_true_list.insert(i2); // TAC
        U->jump_false_list.insert(i3); // TAC
        U->jump_code.push_back(i1); // TAC
        U->jump_code.push_back(i2); // TAC
        U->jump_code.push_back(i3); // TAC
        U->jump_code.push_back(i2_); // TAC 
        U->jump_code.push_back(i3_); // TAC
    }
    else if (op->name == "MINUS" || op->name == "PLUS")
    {
        if (!x->type.isIntorFloat())
        {
            // Throw Error
            U->type = ERROR_TYPE;
            string error_msg = "Invalid operand with unary operator '+/-' at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return U;
        }
        U->type = x->type;
        U->type.make_signed();
        U->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(op->name == "MINUS" ? TAC_OPERATOR_UMINUS : TAC_OPERATOR_ADD), U->result, x->result, new_empty_var(), 0); // TAC
        backpatch(x->next_list, i1->label); // TAC
        backpatch(x->jump_next_list, i1->label); // TAC
        backpatch(x->true_list, i1->label); // TAC
        backpatch(x->false_list, i1->label); // TAC
        backpatch(x->jump_true_list, i1->label); // TAC
        backpatch(x->jump_false_list, i1->label); // TAC
        U->code.push_back(i1); // TAC

        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        U->true_list.insert(i2_); // TAC
        U->false_list.insert(i3_); // TAC
        U->jump_true_list.insert(i2); // TAC
        U->jump_false_list.insert(i3); // TAC
        U->jump_code.push_back(i1); // TAC
        U->jump_code.push_back(i2); // TAC
        U->jump_code.push_back(i3); // TAC
        U->jump_code.push_back(i2_); // TAC
        U->jump_code.push_back(i3_); // TAC
    }
    else if (op->name == "NOT")
    {
        if (!x->type.isIntorFloat() && !x->type.is_pointer)
        {
            U->type = ERROR_TYPE;
            string error_msg = "Invalid operand type for logical NOT '!' at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return U;
        }
        else
        {
            U->type = Type(PrimitiveTypes::INT_T, 0, true);
        }
        U->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), x->result, new_empty_var(), 2); // TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), U->result, new_constant("0"), new_empty_var(), 0); // TAC
        TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); //TAC
        TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i5 = emit(TACOperator(TAC_OPERATOR_NOP), U->result, new_constant("1"), new_empty_var(), 0); // TAC
        U->next_list.insert(i4); // TAC
        i1->result = i3->label; // TAC
        i2->result = i5->label; // TAC
        backpatch(x->next_list, i1->label); // TAC
        backpatch(x->true_list, i1->label); // TAC
        backpatch(x->false_list, i1->label); // TAC
        U->code = x->jump_code; // TAC
        for(auto i:x->jump_true_list){
            if(x->true_list.find(i)==x->true_list.end() && x->false_list.find(i)==x->false_list.end())U->code.erase(remove(U->code.begin(), U->code.end(), i), U->code.end()); // TAC
        }
        for(auto i:x->jump_false_list){
            if(x->true_list.find(i)==x->true_list.end() && x->false_list.find(i)==x->false_list.end())U->code.erase(remove(U->code.begin(), U->code.end(), i), U->code.end()); // TAC
        }

        U->code.push_back(i1); // TAC
        U->code.push_back(i2); // TAC
        U->code.push_back(i3); // TAC
        U->code.push_back(i4); // TAC
        U->code.push_back(i4_); // TAC
        U->code.push_back(i5); // TAC

        U->jump_code = x->jump_code; // TAC
        for(auto i:x->true_list){
            if(x->jump_true_list.find(i)==x->jump_true_list.end() && x->jump_false_list.find(i)==x->jump_false_list.end())U->jump_code.erase(remove(U->jump_code.begin(), U->jump_code.end(), i), U->jump_code.end()); // TAC
        }
        for(auto i:x->false_list){
            if(x->jump_true_list.find(i)==x->jump_true_list.end() && x->jump_false_list.find(i)==x->jump_false_list.end())U->jump_code.erase(remove(U->jump_code.begin(), U->jump_code.end(), i), U->jump_code.end()); // TAC
        }
        U->jump_next_list = x->jump_next_list; // TAC
        U->jump_next_list.insert(i4_); // TAC
        U->jump_true_list = x->jump_false_list; // TAC
        U->jump_false_list = x->jump_true_list; // TAC
        U->true_list = U->jump_true_list; // TAC
        U->false_list = U->jump_false_list; // TAC
    }
    else if (op->name == "BITWISE_NOT") {
        if (!x->type.isInt())
        {
            U->type = ERROR_TYPE;
            string error_msg = "Invalid operand with unary operator '~' at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }
        else
        {
            U->type = x->type.promote_to_int(x->type);
        }
        U->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_BIT_NOT), U->result, x->result, new_empty_var(), 0); // TAC
        backpatch(x->next_list, i1->label); // TAC
        backpatch(x->jump_next_list, i1->label); // TAC
        backpatch(x->true_list, i1->label); // TAC
        backpatch(x->false_list, i1->label); // TAC
        backpatch(x->jump_true_list, i1->label); // TAC
        backpatch(x->jump_false_list, i1->label); // TAC
        U->code.push_back(i1); // TAC

        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        U->true_list.insert(i2_); // TAC
        U->false_list.insert(i3_); // TAC
        U->jump_true_list.insert(i2); // TAC
        U->jump_false_list.insert(i3); // TAC
        U->jump_code.push_back(i1); // TAC
        U->jump_code.push_back(i2); // TAC
        U->jump_code.push_back(i3); // TAC
        U->jump_code.push_back(i2_); // TAC
        U->jump_code.push_back(i3_); // TAC
    }
    U->type.is_const_literal = false;
    return U;
}

Expression* create_unary_expression(Terminal* op, TypeName* tn) {
    UnaryExpression* U = new UnaryExpression();
    U->op = op;
    U->type_name = tn;
    U->line_no = op->line_no;
    U->column_no = op->column_no;
    U->name = "UNARY EXPRESSION SIZEOF TYPE";
    U->type = Type(PrimitiveTypes::INT_T, 0, true);
    U->type.is_const_literal = false;
    U->result = new_temp_var(); // TAC
    TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), U->result, new_constant(to_string(tn->type.get_size())), new_empty_var(), 0); // TAC
    U->code.push_back(i1); // TAC

    TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
    TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
    TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), U->result, new_empty_var(), 2); // TAC
    TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
    U->true_list.insert(i2_); // TAC
    U->false_list.insert(i3_); // TAC
    U->jump_true_list.insert(i2); // TAC
    U->jump_false_list.insert(i3); // TAC
    U->jump_code.push_back(i1); // TAC
    U->jump_code.push_back(i2); // TAC
    U->jump_code.push_back(i3); // TAC
    U->jump_code.push_back(i2_); // TAC
    U->jump_code.push_back(i3_); // TAC
    U->type.is_const_literal = false;
    return U;
}

// ##############################################################################
// ################################## CAST EXPRESSION ######################################
// ##############################################################################
CastExpression::CastExpression() : Expression() {
    name = "CAST EXPRESSION";
    unary_expression = nullptr;
    base_expression = nullptr;
    type_name = nullptr;
}

Expression* create_cast_expression(Expression* x) {
    CastExpression* C = new CastExpression();
    C->unary_expression = dynamic_cast<UnaryExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->type = x->type;
    C->result = x->result; // TAC
    C->true_list = x->true_list; // TAC
    C->false_list = x->false_list; // TAC
    C->jump_true_list = x->jump_true_list; // TAC
    C->jump_false_list = x->jump_false_list; // TAC
    C->code = x->code; // TAC
    C->jump_code = x->jump_code; // TAC
    C->next_list = x->next_list; // TAC
    C->jump_next_list = x->jump_next_list; // TAC
    return C;
}

Expression* create_cast_expression(TypeName* tn, Expression* x) {
    CastExpression* C = new CastExpression();
    C->name = "CAST EXPRESSION WITH TYPENAME";
    C->type_name = tn;
    C->base_expression = dynamic_cast<CastExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->code.insert(C->code.begin(), x->code.begin(), x->code.end()); // TAC
    C->jump_code.insert(C->jump_code.begin(), x->code.begin(), x->code.end()); // TAC
    for(auto i:x->jump_next_list){
        C->code.erase(remove(C->code.begin(), C->code.end(), i), C->code.end()); // TAC
    }
    for(auto i:x->next_list){
        C->jump_code.erase(remove(C->jump_code.begin(), C->jump_code.end(), i), C->jump_code.end()); // TAC
    }

    if (x->type.is_error()) {
        C->type = ERROR_TYPE;
        return C;
    }

    if (!(x->type.is_convertible_to(tn->type))) {
        C->type = ERROR_TYPE;
        string error_msg = "Invalid type conversion at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return C;
    }
    C->type = tn->type;
    C->result = new_temp_var(); // TAC
    TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), C->result, new_type(tn->type.to_string()), x->result, 0); // TAC
    backpatch(x->next_list, i1->label); // TAC
    backpatch(x->jump_next_list, i1->label); // TAC
    backpatch(x->true_list, i1->label); // TAC
    backpatch(x->false_list, i1->label); // TAC
    backpatch(x->jump_true_list, i1->label); // TAC
    backpatch(x->jump_false_list, i1->label); // TAC
    C->code.push_back(i1); // TAC

    TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), C->result, new_empty_var(), 2); // TAC
    TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
    TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), C->result, new_empty_var(), 2); // TAC
    TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
    C->true_list.insert(i2_); // TAC
    C->false_list.insert(i3_); // TAC
    C->jump_true_list.insert(i2); // TAC
    C->jump_false_list.insert(i3); // TAC
    C->jump_code.push_back(i1); // TAC
    C->jump_code.push_back(i2); // TAC
    C->jump_code.push_back(i3); // TAC
    C->jump_code.push_back(i2_); // TAC
    C->jump_code.push_back(i3_); // TAC
    C->type.is_const_literal = false;
    return C;
}

// ##############################################################################
// ################################## MULTIPLICATIVE EXPRESSION ######################################
// ##############################################################################

MultiplicativeExpression::MultiplicativeExpression() {
    name = "MULTIPLICATIVE EXPRESSION";
    cast_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
}

Expression* create_multiplicative_expression(Expression* x) {
    MultiplicativeExpression* M = new MultiplicativeExpression();
    M->cast_expression = dynamic_cast<CastExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    M->result = x->result; // TAC
    M->true_list = x->true_list; // TAC
    M->false_list = x->false_list; // TAC
    M->jump_true_list = x->jump_true_list; // TAC
    M->jump_false_list = x->jump_false_list; // TAC
    M->code = x->code; // TAC
    M->jump_code = x->jump_code; // TAC
    M->next_list = x->next_list; // TAC
    M->jump_next_list = x->jump_next_list; // TAC
    return M;
}

Expression* create_multiplicative_expression(Expression* left, Terminal* op, Expression* right) {
    MultiplicativeExpression* M = new MultiplicativeExpression();
    M->left = left;
    M->right = right;
    M->op = op;
    M->line_no = left->line_no;
    M->column_no = left->column_no;
    M->name = "MULTIPLICATIVE EXPRESSION";
    M->code.insert(M->code.begin(), left->code.begin(), left->code.end()); // TAC
    M->code.insert(M->code.end(), right->code.begin(), right->code.end()); // TAC
    M->jump_code.insert(M->jump_code.begin(), left->code.begin(), left->code.end()); // TAC
    M->jump_code.insert(M->jump_code.end(), right->code.begin(), right->code.end()); // TAC
    for(auto i:left->jump_next_list){
        M->code.erase(remove(M->code.begin(), M->code.end(), i), M->code.end()); // TAC
    }
    for(auto i:right->jump_next_list){
        M->jump_code.erase(remove(M->code.begin(), M->code.end(), i), M->code.end()); // TAC
    }
    for(auto i:left->next_list){
        M->jump_code.erase(remove(M->jump_code.begin(), M->jump_code.end(), i), M->jump_code.end()); // TAC
    }
    for(auto i:right->next_list){
        M->jump_code.erase(remove(M->jump_code.begin(), M->jump_code.end(), i), M->jump_code.end()); // TAC
    }

    if (left->type.is_error() || right->type.is_error()) {
        M->type = ERROR_TYPE;
        return M;
    }

    if (!left->type.isIntorFloat() || !right->type.isIntorFloat() || left->type.is_array || right->type.is_array || left->type.is_function || right->type.is_function || left->type.is_pointer || right->type.is_pointer) {
        M->type = ERROR_TYPE;
        string error_msg = "Operands of '" + op->name + "' must be int or float at line " +
            to_string(M->line_no) + ", column " + to_string(M->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return M;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (op->name == "MULTIPLY" || op->name == "DIVIDE") {
        if (lt.isFloat() || rt.isFloat()) {
            // float * float => float 
            if (lt.type_index > rt.type_index) {
                TACOperand* t1 = new_temp_var(); // TAC
                M->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(lt.to_string()), right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                TACInstruction* i2 = emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, left->result, t1, 0); // TAC
                M->code.push_back(i1); // TAC
                M->code.push_back(i2); // TAC

                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
                TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
                TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                M->true_list.insert(i3_); // TAC
                M->false_list.insert(i4_); // TAC
                M->jump_true_list.insert(i3); // TAC
                M->jump_false_list.insert(i4); // TAC
                M->jump_code.push_back(i1); // TAC  
                M->jump_code.push_back(i2); // TAC
                M->jump_code.push_back(i3); // TAC
                M->jump_code.push_back(i4); // TAC
                M->jump_code.push_back(i3_); // TAC
                M->jump_code.push_back(i4_); // TAC
            }
            else if (lt.type_index == rt.type_index) {
                M->type = lt;
                M->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, left->result, right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                M->code.push_back(i1); // TAC

                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                M->true_list.insert(i2_); // TAC
                M->false_list.insert(i3_); // TAC
                M->jump_true_list.insert(i2); // TAC
                M->jump_false_list.insert(i3); // TAC
                M->jump_code.push_back(i1); // TAC
                M->jump_code.push_back(i2); // TAC
                M->jump_code.push_back(i3); // TAC
                M->jump_code.push_back(i2_); // TAC
                M->jump_code.push_back(i3_); // TAC
            }
            else {
                M->type = rt;
                TACOperand* t1 = new_temp_var(); // TAC
                M->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(rt.to_string()), left->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                TACInstruction* i2 = emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, t1, right->result, 0); // TAC
                M->code.push_back(i1); // TAC

                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
                TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
                TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                M->true_list.insert(i3_); // TAC
                M->false_list.insert(i4_); // TAC
                M->jump_true_list.insert(i3); // TAC
                M->jump_false_list.insert(i4); // TAC
                M->jump_code.push_back(i1); // TAC
                M->jump_code.push_back(i2); // TAC
                M->jump_code.push_back(i3); // TAC
                M->jump_code.push_back(i4); // TAC
                M->jump_code.push_back(i3_); // TAC
                M->jump_code.push_back(i4_); // TAC
            }
        }
        else if (lt.isInt() && rt.isInt()) {
            // int * int => int
            if (lt.type_index > rt.type_index) {
                M->type = lt;
                if (lt.isUnsigned() || rt.isUnsigned()) {
                    M->type.make_unsigned();
                }
                TACOperand* t1 = new_temp_var(); // TAC
                M->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(M->type.to_string()), right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                TACInstruction* i2 = emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, left->result, t1, 0); // TAC
                M->code.push_back(i1); // TAC
                M->code.push_back(i2); // TAC

                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
                TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
                TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                M->true_list.insert(i3_); // TAC
                M->false_list.insert(i4_); // TAC
                M->jump_true_list.insert(i3); // TAC
                M->jump_false_list.insert(i4); // TAC
                M->jump_code.push_back(i1); // TAC
                M->jump_code.push_back(i2); // TAC
                M->jump_code.push_back(i3); // TAC
                M->jump_code.push_back(i4); // TAC
                M->jump_code.push_back(i3_); // TAC
                M->jump_code.push_back(i4_); // TAC
            }
            else if (lt.type_index == rt.type_index) {
                M->type = lt;
                M->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, left->result, right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                M->code.push_back(i1); // TAC

                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                M->true_list.insert(i2_); // TAC
                M->false_list.insert(i3_); // TAC
                M->jump_true_list.insert(i2); // TAC
                M->jump_false_list.insert(i3); // TAC
                M->jump_code.push_back(i1); // TAC
                M->jump_code.push_back(i2); // TAC
                M->jump_code.push_back(i3); // TAC
                M->jump_code.push_back(i2_); // TAC
                M->jump_code.push_back(i3_); // TAC
            }
            else {
                M->type = rt;
                if (lt.isUnsigned() || rt.isUnsigned()) {
                    M->type.make_unsigned();
                }
                TACOperand* t1 = new_temp_var(); // TAC
                M->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(M->type.to_string()), left->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                TACInstruction* i2 = emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, t1, right->result, 0); // TAC
                M->code.push_back(i1); // TAC
                M->code.push_back(i2); // TAC

                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
                TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
                TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                M->true_list.insert(i3_); // TAC
                M->false_list.insert(i4_); // TAC
                M->jump_true_list.insert(i3); // TAC
                M->jump_false_list.insert(i4); // TAC
                M->jump_code.push_back(i1); // TAC
                M->jump_code.push_back(i2); // TAC
                M->jump_code.push_back(i3); // TAC
                M->jump_code.push_back(i4); // TAC
                M->jump_code.push_back(i3_); // TAC
                M->jump_code.push_back(i4_); // TAC
            }
        }
    }
    else if (op->name == "MOD") {
        if (!lt.isInt() || !rt.isInt())
        {
            M->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must be int at line " +
                to_string(M->line_no) + ", column " + to_string(M->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return M;
        }
        if (lt.type_index > rt.type_index) {
            M->type = lt;
            if (lt.isUnsigned() || rt.isUnsigned()) {
                M->type.make_unsigned();
            }
            TACOperand* t1 = new_temp_var(); // TAC
            M->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(M->type.to_string()), right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_MOD), M->result, left->result, t1, 0); // TAC
            M->code.push_back(i1); // TAC
            M->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            M->true_list.insert(i3_); // TAC
            M->false_list.insert(i4_); // TAC
            M->jump_true_list.insert(i3); // TAC
            M->jump_false_list.insert(i4); // TAC
            M->jump_code.push_back(i1); // TAC
            M->jump_code.push_back(i2); // TAC
            M->jump_code.push_back(i3); // TAC
            M->jump_code.push_back(i4); // TAC
            M->jump_code.push_back(i3_); // TAC
            M->jump_code.push_back(i4_); // TAC
        }
        else if (lt.type_index == rt.type_index) {
            M->type = lt;
            M->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_MOD), M->result, left->result, right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            M->code.push_back(i1); // TAC

            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            M->true_list.insert(i2_); // TAC
            M->false_list.insert(i3_); // TAC
            M->jump_true_list.insert(i2); // TAC
            M->jump_false_list.insert(i3); // TAC
            M->jump_code.push_back(i1); // TAC
            M->jump_code.push_back(i2); // TAC
            M->jump_code.push_back(i3); // TAC
            M->jump_code.push_back(i2_); // TAC
            M->jump_code.push_back(i3_); // TAC
        }
        else {
            M->type = rt;
            if (lt.isUnsigned() || rt.isUnsigned()) {
                M->type.make_unsigned();
            }
            TACOperand* t1 = new_temp_var(); // TAC
            M->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(M->type.to_string()), left->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_MOD), M->result, t1, right->result, 0); // TAC
            M->code.push_back(i1); // TAC
            M->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), M->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            M->true_list.insert(i3_); // TAC
            M->false_list.insert(i4_); // TAC
            M->jump_true_list.insert(i3); // TAC
            M->jump_false_list.insert(i4); // TAC
            M->jump_code.push_back(i1); // TAC
            M->jump_code.push_back(i2); // TAC
            M->jump_code.push_back(i3); // TAC
            M->jump_code.push_back(i4); // TAC
            M->jump_code.push_back(i3_); // TAC
            M->jump_code.push_back(i4_); // TAC
        }

    }
    M->type.is_const_literal = false;
    return M;
}

// ##############################################################################
// ################################## ADDITIVE EXPRESSION ######################################
// ##############################################################################

AdditiveExpression::AdditiveExpression() {
    multiplicative_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "ADDITIVE EXPRESSION";
}

Expression* create_additive_expression(Expression* x) {
    AdditiveExpression* M = new AdditiveExpression();
    M->multiplicative_expression = dynamic_cast<MultiplicativeExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    M->result = x->result; // TAC
    M->true_list = x->true_list; // TAC
    M->false_list = x->false_list; // TAC
    M->jump_true_list = x->jump_true_list; // TAC
    M->jump_false_list = x->jump_false_list; // TAC
    M->code = x->code; // TAC
    M->jump_code = x->jump_code; // TAC
    M->next_list = x->next_list; // TAC
    M->jump_next_list = x->jump_next_list; // TAC
    return M;
}

Expression* create_additive_expression(Expression* left, Terminal* op, Expression* right) {
    AdditiveExpression* A = new AdditiveExpression();
    A->left = left;
    A->right = right;
    A->op = op;
    A->line_no = left->line_no;
    A->column_no = left->column_no;
    A->name = "ADDITIVE EXPRESSION";
    A->code.insert(A->code.begin(), left->code.begin(), left->code.end()); // TAC
    A->code.insert(A->code.end(), right->code.begin(), right->code.end()); // TAC
    A->jump_code.insert(A->jump_code.begin(), left->code.begin(), left->code.end()); // TAC
    A->jump_code.insert(A->jump_code.end(), right->code.begin(), right->code.end()); // TAC
    for(auto i:left->jump_next_list){
        A->code.erase(remove(A->code.begin(), A->code.end(), i), A->code.end()); // TAC
    }
    for(auto i:right->jump_next_list){
        A->code.erase(remove(A->code.begin(), A->code.end(), i), A->code.end()); // TAC
    }
    for(auto i:left->next_list){
        A->code.erase(remove(A->jump_code.begin(), A->jump_code.end(), i), A->jump_code.end()); // TAC
    }
    for(auto i:right->next_list){
        A->code.erase(remove(A->jump_code.begin(), A->jump_code.end(), i), A->jump_code.end()); // TAC
    }

    if (left->type.is_error() || right->type.is_error()) {
        A->type = ERROR_TYPE;
        return A;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (lt.isFloat() || rt.isFloat()) {
        // float * float => float
        if (lt.type_index > rt.type_index) {
            TACOperand* t1 = new_temp_var(); // TAC
            A->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(lt.to_string()), right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(op->name == "PLUS" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), A->result, left->result, t1, 0); // TAC
            A->code.push_back(i1); // TAC
            A->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
        else if (lt.type_index == rt.type_index) {
            A->type = lt;
            A->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(op->name == "PLUS" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), A->result, left->result, right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            A->code.push_back(i1); // TAC

            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i2_); // TAC
            A->false_list.insert(i3_); // TAC
            A->jump_true_list.insert(i2); // TAC
            A->jump_false_list.insert(i3); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i2_); // TAC
            A->jump_code.push_back(i3_); // TAC
        }
        else {
            A->type = rt;
            TACOperand* t1 = new_temp_var(); // TAC
            A->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(rt.to_string()), left->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(op->name == "PLUS" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), A->result, t1, right->result, 0); // TAC
            A->code.push_back(i1); // TAC
            A->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
    }
    else if (lt.isInt() && rt.isInt()) {
        // int * int => int
        if (lt.type_index > rt.type_index) {
            A->type = lt;
            if (lt.isUnsigned() || rt.isUnsigned()) {
                A->type.make_unsigned();
            }
            TACOperand* t1 = new_temp_var(); // TAC
            A->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(A->type.to_string()), right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(op->name == "PLUS" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), A->result, left->result, t1, 0); // TAC
            A->code.push_back(i1); // TAC
            A->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
        else if (lt.type_index == rt.type_index) {
            A->type = lt;
            A->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(op->name == "PLUS" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), A->result, left->result, right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            A->code.push_back(i1); // TAC

            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i2_); // TAC
            A->false_list.insert(i3_); // TAC
            A->jump_true_list.insert(i2); // TAC
            A->jump_false_list.insert(i3); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i2_); // TAC
            A->jump_code.push_back(i3_); // TAC
        }
        else {
            A->type = rt;
            if (lt.isUnsigned() || rt.isUnsigned()) {
                A->type.make_unsigned();
            }
            TACOperand* t1 = new_temp_var(); // TAC
            A->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(A->type.to_string()), left->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(op->name == "PLUS" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), A->result, t1, right->result, 0); // TAC
            A->code.push_back(i1); // TAC
            A->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
    }
    else if (op->name == "PLUS" && lt.isPointer() && rt.isInt()) {
        A->type = lt;
        Type t = lt;
        if(t.is_array){
            if(t.array_dim == 1){
                t.is_array = false;
                t.is_pointer = false;
                t.ptr_level = 0;
            }
            t.array_dim--;
            t.array_dims.erase(t.array_dims.begin());
        }
        else {
            t.ptr_level--;
            if(t.ptr_level == 0){
                t.is_pointer = false;
            }
        }
        TACOperand* t1 = new_temp_var(); // TAC
        A->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_MUL), t1, new_constant(to_string(t.get_size())), right->result, 0); // TAC
        backpatch(left->next_list, i1->label); // TAC
        backpatch(right->next_list, i1->label); // TAC
        backpatch(left->jump_next_list, i1->label); // TAC
        backpatch(right->jump_next_list, i1->label); // TAC
        backpatch(left->true_list, i1->label); // TAC
        backpatch(right->true_list, i1->label); // TAC
        backpatch(left->false_list, i1->label); // TAC
        backpatch(right->false_list, i1->label); // TAC
        backpatch(left->jump_true_list, i1->label); // TAC
        backpatch(right->jump_true_list, i1->label); // TAC
        backpatch(left->jump_false_list, i1->label); // TAC
        backpatch(right->jump_false_list, i1->label); // TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_ADD), A->result, left->result, t1, 0); // TAC
        A->code.push_back(i1); // TAC
        A->code.push_back(i2); // TAC

        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
        TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
        TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        A->true_list.insert(i3_); // TAC
        A->false_list.insert(i4_); // TAC
        A->jump_true_list.insert(i3); // TAC
        A->jump_false_list.insert(i4); // TAC
        A->jump_code.push_back(i1); // TAC
        A->jump_code.push_back(i2); // TAC
        A->jump_code.push_back(i3); // TAC
        A->jump_code.push_back(i4); // TAC
        A->jump_code.push_back(i3_); // TAC
        A->jump_code.push_back(i4_); // TAC
    }
    else if (op->name == "PLUS" && lt.isInt() && rt.isPointer()) {
        A->type = rt;
        Type t = rt;
        if(t.is_array){
            if(t.array_dim == 1){
                t.is_array = false;
                t.is_pointer = false;
                t.ptr_level = 0;
            }
            t.array_dim--;
            t.array_dims.erase(t.array_dims.begin());
        }
        else {
            t.ptr_level--;
            if(t.ptr_level == 0){
                t.is_pointer = false;
            }
        }
        TACOperand* t1 = new_temp_var(); // TAC
        A->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_MUL), t1, new_constant(to_string(t.get_size())), left->result, 0); // TAC
        backpatch(left->next_list, i1->label); // TAC
        backpatch(right->next_list, i1->label); // TAC
        backpatch(left->jump_next_list, i1->label); // TAC
        backpatch(right->jump_next_list, i1->label); // TAC
        backpatch(left->true_list, i1->label); // TAC
        backpatch(right->true_list, i1->label); // TAC
        backpatch(left->false_list, i1->label); // TAC
        backpatch(right->false_list, i1->label); // TAC
        backpatch(left->jump_true_list, i1->label); // TAC
        backpatch(right->jump_true_list, i1->label); // TAC
        backpatch(left->jump_false_list, i1->label); // TAC
        backpatch(right->jump_false_list, i1->label); // TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_ADD), A->result, right->result, t1, 0); // TAC
        A->code.push_back(i1); // TAC
        A->code.push_back(i2); // TAC

        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
        TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
        TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        A->true_list.insert(i3_); // TAC
        A->false_list.insert(i4_); // TAC
        A->jump_true_list.insert(i3); // TAC
        A->jump_false_list.insert(i4); // TAC
        A->jump_code.push_back(i1); // TAC
        A->jump_code.push_back(i2); // TAC
        A->jump_code.push_back(i3); // TAC
        A->jump_code.push_back(i4); // TAC
        A->jump_code.push_back(i3_); // TAC
        A->jump_code.push_back(i4_); // TAC
    }
    else if (op->name == "MINUS" && lt.isPointer() && rt.isInt()) {
        A->type = lt;
        Type t = lt;
        if(t.is_array){
            if(t.array_dim == 1){
                t.is_array = false;
                t.is_pointer = false;
                t.ptr_level = 0;
            }
            t.array_dim--;
            t.array_dims.erase(t.array_dims.begin());
        }
        else {
            t.ptr_level--;
            if(t.ptr_level == 0){
                t.is_pointer = false;
            }
        }
        TACOperand* t1 = new_temp_var(); // TAC
        A->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_MUL), t1, new_constant(to_string(t.get_size())), right->result, 0); // TAC
        backpatch(left->next_list, i1->label); // TAC
        backpatch(right->next_list, i1->label); // TAC
        backpatch(left->jump_next_list, i1->label); // TAC
        backpatch(right->jump_next_list, i1->label); // TAC
        backpatch(left->true_list, i1->label); // TAC
        backpatch(right->true_list, i1->label); // TAC
        backpatch(left->false_list, i1->label); // TAC
        backpatch(right->false_list, i1->label); // TAC
        backpatch(left->jump_true_list, i1->label); // TAC
        backpatch(right->jump_true_list, i1->label); // TAC
        backpatch(left->jump_false_list, i1->label); // TAC
        backpatch(right->jump_false_list, i1->label); // TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_SUB), A->result, left->result, t1, 0); // TAC
        A->code.push_back(i1); // TAC
        A->code.push_back(i2); // TAC

        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
        TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
        TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        A->true_list.insert(i3_); // TAC
        A->false_list.insert(i4_); // TAC
        A->jump_true_list.insert(i3); // TAC
        A->jump_false_list.insert(i4); // TAC
        A->jump_code.push_back(i1); // TAC
        A->jump_code.push_back(i2); // TAC
        A->jump_code.push_back(i3); // TAC
        A->jump_code.push_back(i4); // TAC
        A->jump_code.push_back(i3_); // TAC
        A->jump_code.push_back(i4_); // TAC
    }
    else if (op->name == "MINUS" && lt.isPointer() && rt.isPointer()) {
        if (lt == rt) {
            A->type = Type(PrimitiveTypes::INT_T, 0, false);
            TACOperand* t1 = new_temp_var(); // TAC
            A->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_SUB), t1, left->result, right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_DIV), A->result, t1, new_constant(to_string(lt.get_size())), 0); // TAC
            A->code.push_back(i1); // TAC
            A->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
        else {
            A->type = ERROR_TYPE;
            string error_msg = "Pointer subtraction requires both pointers to be of the same type at line " +
                to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        }
    }
    else {
        A->type = ERROR_TYPE;
        string error_msg = "Operands of '" + op->name + "' are invalid at line " +
            to_string(A->line_no) + ", column " + to_string(A->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return A;
    }
    A->type.is_const_literal = false;

    return A;
}

// ##############################################################################
// ################################## SHIFT EXPRESSION ######################################
// ##############################################################################
ShiftExpression::ShiftExpression() {
    additive_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "SHIFT EXPRESSION";
}

Expression* create_shift_expression(Expression* x) {
    ShiftExpression* M = new ShiftExpression();
    M->additive_expression = dynamic_cast<AdditiveExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    M->result = x->result; // TAC
    M->true_list = x->true_list; // TAC
    M->false_list = x->false_list; // TAC
    M->jump_true_list = x->jump_true_list; // TAC
    M->jump_false_list = x->jump_false_list; // TAC
    M->code = x->code; // TAC
    M->jump_code = x->jump_code; // TAC
    M->next_list = x->next_list; // TAC
    M->jump_next_list = x->jump_next_list; // TAC
    return M;
}

// Assumptions Based on C Semantics:
// 1.Shift operators require integral types.
// 2.Result type is usually the left operand's type.
// 3.The right operand must be an integer type, but in most implementations only the lower bits are used (based on width of left operand).
// 4.Signed shift behavior is implementation-defined or undefined in certain cases (e.g., shifting into the sign bit), but compilers like GCC allow it.
Expression* create_shift_expression(Expression* left, Terminal* op, Expression* right) {
    ShiftExpression* S = new ShiftExpression();
    S->left = left;
    S->right = right;
    S->op = op;
    S->line_no = left->line_no;
    S->column_no = left->column_no;
    S->name = "SHIFT EXPRESSION";
    S->code.insert(S->code.begin(), left->code.begin(), left->code.end()); // TAC
    S->code.insert(S->code.end(), right->code.begin(), right->code.end()); // TAC
    S->jump_code.insert(S->jump_code.begin(), left->code.begin(), left->code.end()); // TAC
    S->jump_code.insert(S->jump_code.end(), right->code.begin(), right->code.end()); // TAC
    for(auto i:left->jump_next_list){
        S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); // TAC
    }
    for(auto i:right->jump_next_list){
        S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); // TAC
    }
    for(auto i:left->next_list){
        S->jump_code.erase(remove(S->jump_code.begin(), S->jump_code.end(), i), S->jump_code.end()); // TAC
    }
    for(auto i:right->next_list){
        S->jump_code.erase(remove(S->jump_code.begin(), S->jump_code.end(), i), S->jump_code.end()); // TAC
    }

    if (left->type.is_error() || right->type.is_error()) {
        S->type = ERROR_TYPE;
        return S;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (!lt.isInt() || !rt.isInt()) {
        S->type = ERROR_TYPE;
        string error_msg = "Operands of '" + op->name + "' must be integers at line " +
            to_string(S->line_no) + ", column " + to_string(S->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return S;
    }
    if (lt.type_index > rt.type_index) {
        S->type = lt;
        // Signedness: usually taken from left operand
        if (lt.isUnsigned()) {
            S->type.make_unsigned();
        }
        else {
            S->type.make_signed();
        }
        TACOperand* t1 = new_temp_var(); // TAC
        S->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(S->type.to_string()), right->result, 0); // TAC
        backpatch(left->next_list, i1->label); // TAC
        backpatch(right->next_list, i1->label); // TAC
        backpatch(left->jump_next_list, i1->label); // TAC
        backpatch(right->jump_next_list, i1->label); // TAC
        backpatch(left->true_list, i1->label); // TAC
        backpatch(right->true_list, i1->label); // TAC
        backpatch(left->false_list, i1->label); // TAC
        backpatch(right->false_list, i1->label); // TAC
        backpatch(left->jump_true_list, i1->label); // TAC
        backpatch(right->jump_true_list, i1->label); // TAC
        backpatch(left->jump_false_list, i1->label); // TAC
        backpatch(right->jump_false_list, i1->label); // TAC

        TACInstruction* i2 = emit(TACOperator(op->name == "LEFT_OP" ? TAC_OPERATOR_LEFT_SHIFT : TAC_OPERATOR_RIGHT_SHIFT), S->result, left->result, t1, 0); // TAC
        S->code.push_back(i1); // TAC
        S->code.push_back(i2); // TAC

        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), S->result, new_empty_var(), 2); // TAC
        TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), S->result, new_empty_var(), 2); // TAC
        TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        S->true_list.insert(i3_); // TAC
        S->false_list.insert(i4_); // TAC
        S->jump_true_list.insert(i3); // TAC
        S->jump_false_list.insert(i4); // TAC
        S->jump_code.push_back(i1); // TAC
        S->jump_code.push_back(i2); // TAC
        S->jump_code.push_back(i3); // TAC
        S->jump_code.push_back(i4); // TAC
        S->jump_code.push_back(i3_); // TAC
        S->jump_code.push_back(i4_); // TAC
    }
    else if (lt.type_index == rt.type_index) {
        S->type = lt;
        S->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(op->name == "LEFT_OP" ? TAC_OPERATOR_LEFT_SHIFT : TAC_OPERATOR_RIGHT_SHIFT), S->result, left->result, right->result, 0); // TAC
        backpatch(left->next_list, i1->label); // TAC
        backpatch(right->next_list, i1->label); // TAC
        backpatch(left->jump_next_list, i1->label); // TAC
        backpatch(right->jump_next_list, i1->label); // TAC
        backpatch(left->true_list, i1->label); // TAC
        backpatch(right->true_list, i1->label); // TAC
        backpatch(left->false_list, i1->label); // TAC
        backpatch(right->false_list, i1->label); // TAC
        backpatch(left->jump_true_list, i1->label); // TAC
        backpatch(right->jump_true_list, i1->label); // TAC
        backpatch(left->jump_false_list, i1->label); // TAC
        backpatch(right->jump_false_list, i1->label); // TAC
        S->code.push_back(i1); // TAC

        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), S->result, new_empty_var(), 2); // TAC
        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), S->result, new_empty_var(), 2); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        S->true_list.insert(i2_); // TAC
        S->false_list.insert(i3_); // TAC
        S->jump_true_list.insert(i2); // TAC
        S->jump_false_list.insert(i3); // TAC
        S->jump_code.push_back(i1); // TAC
        S->jump_code.push_back(i2); // TAC
        S->jump_code.push_back(i3); // TAC
        S->jump_code.push_back(i2_); // TAC
        S->jump_code.push_back(i3_); // TAC
    }
    else {
        S->type = rt;
        // Signedness: usually taken from left operand
        if (lt.isUnsigned()) {
            S->type.make_unsigned();
        }
        else {
            S->type.make_signed();
        }
        TACOperand* t1 = new_temp_var(); // TAC
        S->result = new_temp_var(); // TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(S->type.to_string()), left->result, 0); // TAC   
        backpatch(left->next_list, i1->label); // TAC
        backpatch(right->next_list, i1->label); // TAC
        backpatch(left->jump_next_list, i1->label); // TAC
        backpatch(right->jump_next_list, i1->label); // TAC
        backpatch(left->true_list, i1->label); // TAC
        backpatch(right->true_list, i1->label); // TAC
        backpatch(left->false_list, i1->label); // TAC
        backpatch(right->false_list, i1->label); // TAC
        backpatch(left->jump_true_list, i1->label); // TAC
        backpatch(right->jump_true_list, i1->label); // TAC
        backpatch(left->jump_false_list, i1->label); // TAC
        backpatch(right->jump_false_list, i1->label); // TAC
        TACInstruction* i2 = emit(TACOperator(op->name == "LEFT_OP" ? TAC_OPERATOR_LEFT_SHIFT : TAC_OPERATOR_RIGHT_SHIFT), S->result, t1, right->result, 0); // TAC
        S->code.push_back(i1); // TAC
        S->code.push_back(i2); // TAC

        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), S->result, new_empty_var(), 2); // TAC
        TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), S->result, new_empty_var(), 2); // TAC
        TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        S->true_list.insert(i3_); // TAC
        S->false_list.insert(i4_); // TAC
        S->jump_true_list.insert(i3); // TAC
        S->jump_false_list.insert(i4); // TAC
        S->jump_code.push_back(i1); // TAC
        S->jump_code.push_back(i2); // TAC
        S->jump_code.push_back(i3); // TAC
        S->jump_code.push_back(i4); // TAC
        S->jump_code.push_back(i3_); // TAC
        S->jump_code.push_back(i4_); // TAC
    }
    S->type.is_const_literal = false;
    return S;
}

// ##############################################################################
// ################################## RELATIONAL EXPRESSION ######################################
// ##############################################################################

RelationalExpression::RelationalExpression() {
    shift_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "RELATIONAL EXPRESSION";
}

Expression* create_relational_expression(Expression* x) {
    RelationalExpression* M = new RelationalExpression();
    M->shift_expression = dynamic_cast<ShiftExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    M->result = x->result; // TAC
    M->true_list = x->true_list; // TAC
    M->false_list = x->false_list; // TAC
    M->jump_true_list = x->jump_true_list; // TAC
    M->jump_false_list = x->jump_false_list; // TAC
    M->code = x->code; // TAC
    M->jump_code = x->jump_code; // TAC
    M->next_list = x->next_list; // TAC
    return M;
}

Expression* create_relational_expression(Expression* left, Terminal* op, Expression* right) {
    RelationalExpression* R = new RelationalExpression();
    R->left = left;
    R->right = right;
    R->op = op;
    R->line_no = left->line_no;
    R->column_no = left->column_no;
    R->name = "RELATIONAL EXPRESSION";
    R->code.insert(R->code.begin(), left->code.begin(), left->code.end()); // TAC
    R->code.insert(R->code.end(), right->code.begin(), right->code.end()); // TAC
    R->jump_code.insert(R->jump_code.begin(), left->code.begin(), left->code.end()); // TAC
    R->jump_code.insert(R->jump_code.end(), right->code.begin(), right->code.end()); // TAC
    for(auto i:left->jump_next_list){
        R->code.erase(remove(R->code.begin(), R->code.end(), i), R->code.end()); // TAC
    }
    for(auto i:right->jump_next_list){
        R->code.erase(remove(R->code.begin(), R->code.end(), i), R->code.end()); // TAC
    }
    for(auto i:left->next_list){
        R->jump_code.erase(remove(R->jump_code.begin(), R->jump_code.end(), i), R->jump_code.end()); // TAC
    }
    for(auto i:right->next_list){
        R->jump_code.erase(remove(R->jump_code.begin(), R->jump_code.end(), i), R->jump_code.end()); // TAC
    }

    if (left->type.is_error() || right->type.is_error()) {
        R->type = ERROR_TYPE;
        return R;
    }

    Type lt = left->type;
    Type rt = right->type;

    if ((lt.isIntorFloat() && rt.isIntorFloat())) {
        // Usual promotions can be added here if needed
        R->type = Type(PrimitiveTypes::INT_T, 0, false);
        if (lt.isUnsigned() != rt.isUnsigned()) {
            // causes undefined behaviour
            R->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must have same signedness " +
                to_string(R->line_no) + ", column " + to_string(R->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return R;
        }
        if (lt.type_index > rt.type_index) {
            TACOperand* t1 = new_temp_var(); // TAC
            R->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(lt.to_string()), right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(op->name == "LESS" ? TAC_OPERATOR_LT :
                op->name == "LE_OP" ? TAC_OPERATOR_LE :
                op->name == "GREATER" ? TAC_OPERATOR_GT :
                TAC_OPERATOR_GE), new_empty_var(), left->result, t1, 2); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), R->result, new_constant("1"), new_empty_var(), 0); // TAC
            TACInstruction* i5 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i5_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i6 = emit(TACOperator(TAC_OPERATOR_NOP), R->result, new_constant("0"), new_empty_var(), 0); // TAC
            i2->result = i4->label; // TAC
            i3->result = i6->label; // TAC
            R->next_list.insert(i5); // TAC
            R->code.push_back(i1); // TAC
            R->code.push_back(i2); // TAC
            R->code.push_back(i3); // TAC
            R->code.push_back(i4); // TAC
            R->code.push_back(i5); // TAC
            R->code.push_back(i5_); // TAC
            R->code.push_back(i6); // TAC

            TACInstruction* i7 = emit(TACOperator(op->name == "LESS" ? TAC_OPERATOR_LT :
                op->name == "LE_OP" ? TAC_OPERATOR_LE :
                op->name == "GREATER" ? TAC_OPERATOR_GT :
                TAC_OPERATOR_GE), new_empty_var(), left->result, t1, 2); // TAC if goto
            TACInstruction* i8 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            TACInstruction* i7_ = emit(TACOperator(op->name == "LESS" ? TAC_OPERATOR_LT :
                op->name == "LE_OP" ? TAC_OPERATOR_LE :
                op->name == "GREATER" ? TAC_OPERATOR_GT :
                TAC_OPERATOR_GE), new_empty_var(), left->result, t1, 2); // TAC if goto
            TACInstruction* i8_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            R->true_list.insert(i7_); // TAC
            R->false_list.insert(i8_); // TAC
            R->jump_true_list.insert(i7); // TAC
            R->jump_false_list.insert(i8); // TAC
            R->jump_next_list.insert(i5_); // TAC
            R->jump_code.push_back(i1); // TAC
            R->jump_code.push_back(i7); // TAC
            R->jump_code.push_back(i8); // TAC
            R->jump_code.push_back(i7_); // TAC
            R->jump_code.push_back(i8_); // TAC
        }
        else if (lt.type_index == rt.type_index) {
            R->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(op->name == "LESS" ? TAC_OPERATOR_LT :
                op->name == "LE_OP" ? TAC_OPERATOR_LE :
                op->name == "GREATER" ? TAC_OPERATOR_GT :
                TAC_OPERATOR_GE), new_empty_var(), left->result, right->result, 2); // TAC
            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), R->result, new_constant("1"), new_empty_var(), 0); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i5 = emit(TACOperator(TAC_OPERATOR_NOP), R->result, new_constant("0"), new_empty_var(), 0); // TAC
            i1->result = i3->label; // TAC
            i2->result = i5->label; // TAC
            R->next_list.insert(i4); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            R->code.push_back(i1); // TAC
            R->code.push_back(i2); // TAC
            R->code.push_back(i3); // TAC
            R->code.push_back(i4); // TAC   
            R->code.push_back(i4_); // TAC
            R->code.push_back(i5); // TAC

            TACInstruction* i6 = emit(TACOperator(op->name == "LESS" ? TAC_OPERATOR_LT :
                op->name == "LE_OP" ? TAC_OPERATOR_LE :
                op->name == "GREATER" ? TAC_OPERATOR_GT :
                TAC_OPERATOR_GE), new_empty_var(), left->result, right->result, 2); // TAC if goto
            TACInstruction* i7 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            TACInstruction* i6_ = emit(TACOperator(op->name == "LESS" ? TAC_OPERATOR_LT :
                op->name == "LE_OP" ? TAC_OPERATOR_LE :
                op->name == "GREATER" ? TAC_OPERATOR_GT :
                TAC_OPERATOR_GE), new_empty_var(), left->result, right->result, 2); // TAC if goto
            TACInstruction* i7_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            backpatch(left->jump_next_list, i6->label); // TAC
            backpatch(right->jump_next_list, i6->label); // TAC
            backpatch(left->jump_true_list, i6->label); // TAC
            backpatch(right->jump_true_list, i6->label); // TAC
            backpatch(left->jump_false_list, i6->label); // TAC
            backpatch(right->jump_false_list, i6->label); // TAC 
            R->true_list.insert(i6_); // TAC
            R->false_list.insert(i7_); // TAC
            R->jump_true_list.insert(i6); // TAC
            R->jump_false_list.insert(i7); // TAC
            R->jump_next_list.insert(i4_); // TAC
            R->jump_code.push_back(i6); // TAC
            R->jump_code.push_back(i7); // TAC
            R->jump_code.push_back(i6_); // TAC
            R->jump_code.push_back(i7_); // TAC
        }
        else {
            TACOperand* t1 = new_temp_var(); // TAC
            R->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(rt.to_string()), left->result, 0); // TAC   
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(op->name == "LESS" ? TAC_OPERATOR_LT :
                op->name == "LE_OP" ? TAC_OPERATOR_LE :
                op->name == "GREATER" ? TAC_OPERATOR_GT :
                TAC_OPERATOR_GE), new_empty_var(), t1, right->result, 2); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), R->result, new_constant("1"), new_empty_var(), 0); // TAC
            TACInstruction* i5 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i5_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i6 = emit(TACOperator(TAC_OPERATOR_NOP), R->result, new_constant("0"), new_empty_var(), 0); // TAC
            i2->result = i4->label; // TAC
            i3->result = i6->label; // TAC
            R->next_list.insert(i5); // TAC
            R->code.push_back(i1); // TAC
            R->code.push_back(i2); // TAC
            R->code.push_back(i3); // TAC
            R->code.push_back(i4); // TAC
            R->code.push_back(i5); // TAC
            R->code.push_back(i5_); // TAC
            R->code.push_back(i6); // TAC

            TACInstruction* i7 = emit(TACOperator(op->name == "LESS" ? TAC_OPERATOR_LT :
                op->name == "LE_OP" ? TAC_OPERATOR_LE :
                op->name == "GREATER" ? TAC_OPERATOR_GT :
                TAC_OPERATOR_GE), new_empty_var(), t1, right->result, 2); // TAC if goto
            TACInstruction* i8 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            TACInstruction* i7_ = emit(TACOperator(op->name == "LESS" ? TAC_OPERATOR_LT :
                op->name == "LE_OP" ? TAC_OPERATOR_LE :
                op->name == "GREATER" ? TAC_OPERATOR_GT :
                TAC_OPERATOR_GE), new_empty_var(), t1, right->result, 2); // TAC if goto
            TACInstruction* i8_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            R->true_list.insert(i7_); // TAC
            R->false_list.insert(i8_); // TAC
            R->jump_true_list.insert(i7); // TAC
            R->jump_false_list.insert(i8); // TAC
            R->jump_next_list.insert(i5_); // TAC
            R->jump_code.push_back(i1); // TAC
            R->jump_code.push_back(i7); // TAC
            R->jump_code.push_back(i8); // TAC
            R->jump_code.push_back(i7_); // TAC
            R->jump_code.push_back(i8_); // TAC
        }
    }
    else if (lt.is_pointer && rt.is_pointer) {
        R->type = Type(PrimitiveTypes::INT_T, 0, false);
        if (lt == rt) {
            // Pointer comparison
            R->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(op->name == "LESS" ? TAC_OPERATOR_LT :
                op->name == "LE_OP" ? TAC_OPERATOR_LE :
                op->name == "GREATER" ? TAC_OPERATOR_GT :
                TAC_OPERATOR_GE), new_empty_var(), left->result, right->result, 2); // TAC
            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), R->result, new_constant("1"), new_empty_var(), 0); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i5 = emit(TACOperator(TAC_OPERATOR_NOP), R->result, new_constant("0"), new_empty_var(), 0); // TAC
            i1->result = i3->label; // TAC
            i2->result = i5->label; // TAC
            R->next_list.insert(i4); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            R->code.push_back(i1); // TAC
            R->code.push_back(i2); // TAC
            R->code.push_back(i3); // TAC
            R->code.push_back(i4); // TAC 
            R->code.push_back(i4_); // TAC  
            R->code.push_back(i5); // TAC

            TACInstruction* i6 = emit(TACOperator(op->name == "LESS" ? TAC_OPERATOR_LT :
                op->name == "LE_OP" ? TAC_OPERATOR_LE :
                op->name == "GREATER" ? TAC_OPERATOR_GT :
                TAC_OPERATOR_GE), new_empty_var(), left->result, right->result, 2); // TAC if goto
            TACInstruction* i7 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            TACInstruction* i6_ = emit(TACOperator(op->name == "LESS" ? TAC_OPERATOR_LT :
                op->name == "LE_OP" ? TAC_OPERATOR_LE :
                op->name == "GREATER" ? TAC_OPERATOR_GT :
                TAC_OPERATOR_GE), new_empty_var(), left->result, right->result, 2); // TAC if goto
            TACInstruction* i7_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            backpatch(left->jump_next_list, i6->label); // TAC
            backpatch(right->jump_next_list, i6->label); // TAC
            backpatch(left->jump_true_list, i6->label); // TAC
            backpatch(right->jump_true_list, i6->label); // TAC
            backpatch(left->jump_false_list, i6->label); // TAC
            backpatch(right->jump_false_list, i6->label); // TAC
            R->true_list.insert(i6_); // TAC
            R->false_list.insert(i7_); // TAC
            R->jump_true_list.insert(i6); // TAC
            R->jump_false_list.insert(i7); // TAC
            R->jump_next_list.insert(i4_); // TAC
            R->jump_code.push_back(i1); // TAC
            R->jump_code.push_back(i2); // TAC
            R->jump_code.push_back(i6); // TAC
            R->jump_code.push_back(i7); // TAC
            R->jump_code.push_back(i6_); // TAC
            R->jump_code.push_back(i7_); // TAC
        }
        else {
            R->type = ERROR_TYPE;
            string error_msg = "Pointer comparison requires both pointers to be of the same type at line " +
                to_string(R->line_no) + ", column " + to_string(R->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return R;
        }
    }
    else {
        R->type = ERROR_TYPE;
        string error_msg = "Operands of '" + op->name + "' must be integers or float or pointer at line " +
            to_string(R->line_no) + ", column " + to_string(R->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return R;
    }
    R->type.is_const_literal = false;
    return R;
}

// ##############################################################################
// ################################## EQUALITY EXPRESSION ######################################
// ##############################################################################

EqualityExpression::EqualityExpression() {
    relational_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "EQUALITY EXPRESSION";
}

Expression* create_equality_expression(Expression* x) {
    EqualityExpression* M = new EqualityExpression();
    M->relational_expression = dynamic_cast<RelationalExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    M->result = x->result; // TAC
    M->true_list = x->true_list; // TAC
    M->false_list = x->false_list; // TAC
    M->jump_true_list = x->jump_true_list; // TAC
    M->jump_false_list = x->jump_false_list; // TAC
    M->code = x->code; // TAC
    M->jump_code = x->jump_code; // TAC
    M->next_list = x->next_list; // TAC
    M->jump_next_list = x->jump_next_list; // TAC
    return M;
}

Expression* create_equality_expression(Expression* left, Terminal* op, Expression* right) {
    EqualityExpression* E = new EqualityExpression();
    E->left = left;
    E->right = right;
    E->op = op;
    E->line_no = left->line_no;
    E->column_no = left->column_no;
    E->name = "EQUALITY EXPRESSION";
    E->code.insert(E->code.begin(), left->code.begin(), left->code.end()); // TAC
    E->code.insert(E->code.end(), right->code.begin(), right->code.end()); // TAC
    E->jump_code.insert(E->jump_code.begin(), left->code.begin(), left->code.end()); // TAC
    E->jump_code.insert(E->jump_code.end(), right->code.begin(), right->code.end()); // TAC
    for(auto i:left->jump_next_list){
        E->code.erase(remove(E->code.begin(), E->code.end(), i), E->code.end()); // TAC
    }
    for(auto i:right->jump_next_list){
        E->code.erase(remove(E->code.begin(), E->code.end(), i), E->code.end()); // TAC
    }
    for(auto i:left->next_list){
        E->jump_code.erase(remove(E->jump_code.begin(), E->jump_code.end(), i), E->jump_code.end()); // TAC
    }
    for(auto i:right->next_list){
        E->jump_code.erase(remove(E->jump_code.begin(), E->jump_code.end(), i), E->jump_code.end()); // TAC
    }

    if (left->type.is_error() || right->type.is_error()) {
        E->type = ERROR_TYPE;
        return E;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (lt.isIntorFloat() && rt.isIntorFloat()) {
        E->type = Type(PrimitiveTypes::INT_T, 0, false);  // Result is always int (0 or 1)
        if (lt.isUnsigned() != rt.isUnsigned()) {
            // causes undefined behaviour
            E->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must have same signedness " +
                to_string(E->line_no) + ", column " + to_string(E->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return E;
        }
        if (lt.type_index > rt.type_index) {
            TACOperand* t1 = new_temp_var(); // TAC
            E->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(lt.to_string()), right->result, 0); // TAC
            TACInstruction* i2 = emit(TACOperator(op->name == "EQ_OP" ? TAC_OPERATOR_EQ : TAC_OPERATOR_NE), new_empty_var(), left->result, t1, 0); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), E->result, new_constant("1"), new_empty_var(), 0); // TAC
            TACInstruction* i5 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i5_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i6 = emit(TACOperator(TAC_OPERATOR_NOP), E->result, new_constant("0"), new_empty_var(), 0); // TAC
            i2->result = i4->label; // TAC
            i3->result = i6->label; // TAC
            E->next_list.insert(i5); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            E->code.push_back(i1); // TAC
            E->code.push_back(i2); // TAC
            E->code.push_back(i3); // TAC
            E->code.push_back(i4); // TAC
            E->code.push_back(i5); // TAC
            E->code.push_back(i5_); // TAC
            E->code.push_back(i6); // TAC

            TACInstruction* i7 = emit(TACOperator(op->name == "EQ_OP" ? TAC_OPERATOR_EQ : TAC_OPERATOR_NE), new_empty_var(), left->result, t1, 2); // TAC if goto
            TACInstruction* i8 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            TACInstruction* i7_ = emit(TACOperator(op->name == "EQ_OP" ? TAC_OPERATOR_EQ : TAC_OPERATOR_NE), new_empty_var(), left->result, t1, 2); // TAC if goto
            TACInstruction* i8_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            E->true_list.insert(i7_);
            E->false_list.insert(i8_); // TAC
            E->jump_true_list.insert(i7); // TAC
            E->jump_false_list.insert(i8); // TAC
            E->jump_next_list.insert(i5_); // TAC
            E->jump_code.push_back(i1); // TAC
            E->jump_code.push_back(i7); // TAC
            E->jump_code.push_back(i8); // TAC
            E->jump_code.push_back(i7_); // TAC
            E->jump_code.push_back(i8_); // TAC
        }
        else if (lt.type_index == rt.type_index) {
            E->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(op->name == "EQ_OP" ? TAC_OPERATOR_EQ : TAC_OPERATOR_NE), new_empty_var(), left->result, right->result, 2); // TAC
            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), E->result, new_constant("1"), new_empty_var(), 0); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i5 = emit(TACOperator(TAC_OPERATOR_NOP), E->result, new_constant("0"), new_empty_var(), 0); // TAC
            i1->result = i3->label; // TAC
            i2->result = i5->label; // TAC
            E->next_list.insert(i4); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            E->code.push_back(i1); // TAC
            E->code.push_back(i2); // TAC
            E->code.push_back(i3); // TAC
            E->code.push_back(i4); // TAC
            E->code.push_back(i4_); // TAC
            E->code.push_back(i5); // TAC

            TACInstruction* i6 = emit(TACOperator(op->name == "EQ_OP" ? TAC_OPERATOR_EQ : TAC_OPERATOR_NE), new_empty_var(), left->result, right->result, 2); // TAC if goto
            TACInstruction* i7 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            TACInstruction* i6_ = emit(TACOperator(op->name == "EQ_OP" ? TAC_OPERATOR_EQ : TAC_OPERATOR_NE), new_empty_var(), left->result, right->result, 2); // TAC if goto
            TACInstruction* i7_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            backpatch(left->jump_next_list, i6->label); // TAC
            backpatch(right->jump_next_list, i6->label); // TAC
            backpatch(left->jump_true_list, i6->label); // TAC
            backpatch(right->jump_true_list, i6->label); // TAC
            backpatch(left->jump_false_list, i6->label); // TAC
            backpatch(right->jump_false_list, i6->label); // TAC
            E->true_list.insert(i6_); // TAC
            E->false_list.insert(i7_); // TAC
            E->jump_true_list.insert(i6); // TAC
            E->jump_false_list.insert(i7); // TAC
            E->jump_next_list.insert(i4_); // TAC
            E->jump_code.push_back(i6); // TAC
            E->jump_code.push_back(i7); // TAC
            E->jump_code.push_back(i6_); // TAC
            E->jump_code.push_back(i7_); // TAC
        }
        else {
            TACOperand* t1 = new_temp_var(); // TAC
            E->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(rt.to_string()), left->result, 0); // TAC
            TACInstruction* i2 = emit(TACOperator(op->name == "EQ_OP" ? TAC_OPERATOR_EQ : TAC_OPERATOR_NE), new_empty_var(), t1, right->result, 0); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), E->result, new_constant("1"), new_empty_var(), 0); // TAC
            TACInstruction* i5 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i5_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i6 = emit(TACOperator(TAC_OPERATOR_NOP), E->result, new_constant("0"), new_empty_var(), 0); // TAC
            i2->result = i4->label; // TAC
            i3->result = i6->label; // TAC
            E->next_list.insert(i5); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC    
            E->code.push_back(i1); // TAC
            E->code.push_back(i2); // TAC
            E->code.push_back(i3); // TAC
            E->code.push_back(i4); // TAC
            E->code.push_back(i5); // TAC
            E->code.push_back(i5_); // TAC
            E->code.push_back(i6); // TAC

            TACInstruction* i7 = emit(TACOperator(op->name == "EQ_OP" ? TAC_OPERATOR_EQ : TAC_OPERATOR_NE), new_empty_var(), left->result, t1, 2); // TAC if goto
            TACInstruction* i8 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            TACInstruction* i7_ = emit(TACOperator(op->name == "EQ_OP" ? TAC_OPERATOR_EQ : TAC_OPERATOR_NE), new_empty_var(), left->result, t1, 2); // TAC if goto
            TACInstruction* i8_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            E->true_list.insert(i7_);
            E->false_list.insert(i8_); // TAC
            E->jump_true_list.insert(i7); // TAC
            E->jump_false_list.insert(i8); // TAC
            E->jump_next_list.insert(i5_); // TAC
            E->jump_code.push_back(i1); // TAC
            E->jump_code.push_back(i7); // TAC
            E->jump_code.push_back(i8); // TAC
            E->jump_code.push_back(i7_); // TAC
            E->jump_code.push_back(i8_); // TAC
        }

    }
    else if (lt.isPointer() && rt.isPointer()) {
        E->type = Type(PrimitiveTypes::INT_T, 0, false);
        if (lt == rt) {
            // Pointer comparison
            E->result = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(op->name == "EQ_OP" ? TAC_OPERATOR_EQ : TAC_OPERATOR_NE), new_empty_var(), left->result, right->result, 2); // TAC
            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), E->result, new_constant("1"), new_empty_var(), 0); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i5 = emit(TACOperator(TAC_OPERATOR_NOP), E->result, new_constant("0"), new_empty_var(), 0); // TAC
            i1->result = i3->label; // TAC
            i2->result = i5->label; // TAC
            E->next_list.insert(i4); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            E->code.push_back(i1); // TAC
            E->code.push_back(i2); // TAC
            E->code.push_back(i3); // TAC
            E->code.push_back(i4); // TAC
            E->code.push_back(i4_); // TAC
            E->code.push_back(i5); // TAC

            TACInstruction* i6 = emit(TACOperator(op->name == "EQ_OP" ? TAC_OPERATOR_EQ : TAC_OPERATOR_NE), new_empty_var(), left->result, right->result, 2); // TAC if goto
            TACInstruction* i7 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            TACInstruction* i6_ = emit(TACOperator(op->name == "EQ_OP" ? TAC_OPERATOR_EQ : TAC_OPERATOR_NE), new_empty_var(), left->result, right->result, 2); // TAC if goto
            TACInstruction* i7_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC goto
            backpatch(left->jump_next_list, i6->label); // TAC
            backpatch(right->jump_next_list, i6->label); // TAC
            backpatch(left->jump_true_list, i6->label); // TAC
            backpatch(right->jump_true_list, i6->label); // TAC
            backpatch(left->jump_false_list, i6->label); // TAC
            backpatch(right->jump_false_list, i6->label); // TAC
            E->true_list.insert(i6_); // TAC
            E->false_list.insert(i7_); // TAC
            E->jump_true_list.insert(i6); // TAC
            E->jump_false_list.insert(i7); // TAC
            E->jump_next_list.insert(i4_); // TAC
            E->jump_code.push_back(i6); // TAC
            E->jump_code.push_back(i7); // TAC
            E->jump_code.push_back(i6_); // TAC
            E->jump_code.push_back(i7_); // TAC
        }
        else {
            E->type = ERROR_TYPE;
            string error_msg = "Pointer comparison requires both pointers to be of the same type at line " +
                to_string(E->line_no) + ", column " + to_string(E->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return E;
        }

    }
    else {
        E->type = ERROR_TYPE;
        std::string error_msg = "Operands of '" + op->name + "' must be integers or float or pointer at line " +
            to_string(E->line_no) + ", column " + to_string(E->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return E;
    }
    E->type.is_const_literal = false;
    return E;
}

// ##############################################################################
// ################################## AND EXPRESSION ######################################
// ##############################################################################

AndExpression::AndExpression() {
    equality_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "AND EXPRESSION";
}

Expression* create_and_expression(Expression* x) {
    AndExpression* M = new AndExpression();
    M->equality_expression = dynamic_cast<EqualityExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    M->result = x->result; // TAC
    M->true_list = x->true_list; // TAC
    M->false_list = x->false_list; // TAC
    M->jump_true_list = x->jump_true_list; // TAC
    M->jump_false_list = x->jump_false_list; // TAC
    M->code = x->code; // TAC
    M->jump_code = x->jump_code; // TAC
    M->next_list = x->next_list; // TAC
    M->jump_next_list = x->jump_next_list; // TAC
    return M;
}

Expression* create_and_expression(Expression* left, Terminal* op, Expression* right) {
    AndExpression* A = new AndExpression();
    A->left = left;
    A->right = right;
    A->op = op;
    A->line_no = left->line_no;
    A->column_no = left->column_no;
    A->name = "AND EXPRESSION";
    A->code.insert(A->code.begin(), left->code.begin(), left->code.end()); // TAC
    A->code.insert(A->code.end(), right->code.begin(), right->code.end()); // TAC
    A->jump_code.insert(A->jump_code.begin(), left->code.begin(), left->code.end()); // TAC
    A->jump_code.insert(A->jump_code.end(), right->code.begin(), right->code.end()); // TAC
    for(auto i:left->jump_next_list){
        A->code.erase(remove(A->code.begin(), A->code.end(), i), A->code.end()); // TAC
    }
    for(auto i:right->jump_next_list){
        A->code.erase(remove(A->code.begin(), A->code.end(), i), A->code.end()); // TAC
    }
    for(auto i:left->next_list){
        A->jump_code.erase(remove(A->jump_code.begin(), A->jump_code.end(), i), A->jump_code.end()); // TAC
    }
    for(auto i:right->next_list){
        A->jump_code.erase(remove(A->jump_code.begin(), A->jump_code.end(), i), A->jump_code.end()); // TAC
    }

    if (left->type.is_error() || right->type.is_error()) {
        A->type = ERROR_TYPE;
        return A;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (op->name == "BITWISE_AND") {
        if (lt.isInt() && rt.isInt()) {
            if (lt.type_index > rt.type_index) {
                A->type = lt;
                if (lt.isUnsigned() || rt.isUnsigned()) {
                    A->type.make_unsigned();
                }
                else if (!lt.isUnsigned() && !rt.isUnsigned()) {
                    A->type.make_signed();
                }
                TACOperand* t1 = new_temp_var(); // TAC
                A->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(A->type.to_string()), right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_BIT_AND), A->result, left->result, t1, 0); // TAC
                A->code.push_back(i1); // TAC
                A->code.push_back(i2); // TAC

                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
                TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
                TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                A->true_list.insert(i3_); // TAC
                A->false_list.insert(i4_); // TAC
                A->jump_true_list.insert(i3); // TAC
                A->jump_false_list.insert(i4); // TAC
                A->jump_code.push_back(i1); // TAC
                A->jump_code.push_back(i2); // TAC
                A->jump_code.push_back(i3); // TAC
                A->jump_code.push_back(i4); // TAC
                A->jump_code.push_back(i3_); // TAC
                A->jump_code.push_back(i4_); // TAC
            }
            else if (lt.type_index == rt.type_index) {
                A->type = lt;
                A->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_BIT_AND), A->result, left->result, right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                A->code.push_back(i1); // TAC

                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                A->true_list.insert(i2_); // TAC
                A->false_list.insert(i3_); // TAC
                A->jump_true_list.insert(i2); // TAC
                A->jump_false_list.insert(i3); // TAC
                A->jump_code.push_back(i1); // TAC
                A->jump_code.push_back(i2); // TAC
                A->jump_code.push_back(i3); // TAC
                A->jump_code.push_back(i2_); // TAC
                A->jump_code.push_back(i3_); // TAC
            }
            else {
                A->type = rt;
                if (lt.isUnsigned() || rt.isUnsigned()) {
                    A->type.make_unsigned();
                }
                else if (!lt.isUnsigned() && !rt.isUnsigned()) {
                    A->type.make_signed();
                }
                TACOperand* t1 = new_temp_var(); // TAC
                A->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(A->type.to_string()), left->result, 0); // TAC
                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_BIT_AND), A->result, t1, right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                A->code.push_back(i1); // TAC
                A->code.push_back(i2); // TAC

                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
                TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), A->result, new_empty_var(), 2); // TAC
                TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                A->true_list.insert(i3_); // TAC
                A->false_list.insert(i4_); // TAC
                A->jump_true_list.insert(i3); // TAC
                A->jump_false_list.insert(i4); // TAC
                A->jump_code.push_back(i1); // TAC
                A->jump_code.push_back(i2); // TAC
                A->jump_code.push_back(i3); // TAC
                A->jump_code.push_back(i4); // TAC
                A->jump_code.push_back(i3_); // TAC
                A->jump_code.push_back(i4_); // TAC
            }
        }
        else {
            A->type = ERROR_TYPE;
            string error_msg = "Operands of '&' must be integers at line " +
                to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        }
    }
    A->type.is_const_literal = false;
    return A;
}

// ##############################################################################
// ################################## XOR EXPRESSION ######################################
// ##############################################################################

XorExpression::XorExpression() {
    and_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "XOR EXPRESSION";
}

Expression* create_xor_expression(Expression* x) {
    XorExpression* M = new XorExpression();
    M->and_expression = dynamic_cast<AndExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    M->result = x->result; // TAC
    M->true_list = x->true_list; // TAC
    M->false_list = x->false_list; // TAC
    M->jump_true_list = x->jump_true_list; // TAC
    M->jump_false_list = x->jump_false_list; // TAC
    M->code = x->code; // TAC
    M->jump_code = x->jump_code; // TAC
    M->next_list = x->next_list; // TAC
    M->jump_next_list = x->jump_next_list; // TAC
    return M;
}

Expression* create_xor_expression(Expression* left, Terminal* op, Expression* right) {
    XorExpression* X = new XorExpression();
    X->left = left;
    X->right = right;
    X->op = op;
    X->line_no = left->line_no;
    X->column_no = left->column_no;
    X->name = "XOR EXPRESSION";
    X->code.insert(X->code.begin(), left->code.begin(), left->code.end()); // TAC
    X->code.insert(X->code.end(), right->code.begin(), right->code.end()); // TAC
    X->jump_code.insert(X->jump_code.begin(), left->code.begin(), left->code.end()); // TAC
    X->jump_code.insert(X->jump_code.end(), right->code.begin(), right->code.end()); // TAC
    for(auto i:left->jump_next_list){
        X->code.erase(remove(X->code.begin(), X->code.end(), i), X->code.end()); // TAC
    }
    for(auto i:right->jump_next_list){
        X->code.erase(remove(X->code.begin(), X->code.end(), i), X->code.end()); // TAC
    }
    for(auto i:left->next_list){
        X->jump_code.erase(remove(X->jump_code.begin(), X->jump_code.end(), i), X->jump_code.end()); // TAC
    }
    for(auto i:right->next_list){
        X->jump_code.erase(remove(X->jump_code.begin(), X->jump_code.end(), i), X->jump_code.end()); // TAC
    }

    if (left->type.is_error() || right->type.is_error()) {
        X->type = ERROR_TYPE;
        return X;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (op->name == "BITWISE_XOR") {
        if (lt.isInt() && rt.isInt()) {
            if (lt.type_index > rt.type_index) {
                X->type = lt;
                if (lt.isUnsigned() || rt.isUnsigned()) {
                    X->type.make_unsigned();
                }
                else if (!lt.isUnsigned() && !rt.isUnsigned()) {
                    X->type.make_signed();
                }
                TACOperand* t1 = new_temp_var(); // TAC
                X->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(X->type.to_string()), right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_BIT_XOR), X->result, left->result, t1, 0); // TAC
                X->code.push_back(i1); // TAC
                X->code.push_back(i2); // TAC

                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), X->result, new_empty_var(), 2); // TAC
                TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), X->result, new_empty_var(), 2); // TAC
                TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                X->true_list.insert(i3_); // TAC
                X->false_list.insert(i4_); // TAC
                X->jump_true_list.insert(i3); // TAC
                X->jump_false_list.insert(i4); // TAC
                X->jump_code.push_back(i1); // TAC
                X->jump_code.push_back(i2); // TAC
                X->jump_code.push_back(i3); // TAC
                X->jump_code.push_back(i4); // TAC
                X->jump_code.push_back(i3_); // TAC
                X->jump_code.push_back(i4_); // TAC
            }
            else if (lt.type_index == rt.type_index) {
                X->type = lt;
                X->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_BIT_XOR), X->result, left->result, right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                X->code.push_back(i1); // TAC

                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), X->result, new_empty_var(), 2); // TAC
                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), X->result, new_empty_var(), 2); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                X->true_list.insert(i2_); // TAC
                X->false_list.insert(i3_); // TAC
                X->jump_true_list.insert(i2); // TAC
                X->jump_false_list.insert(i3); // TAC
                X->jump_code.push_back(i1); // TAC
                X->jump_code.push_back(i2); // TAC
                X->jump_code.push_back(i3); // TAC
                X->jump_code.push_back(i2_); // TAC
                X->jump_code.push_back(i3_); // TAC
            }
            else {
                X->type = rt;
                if (lt.isUnsigned() || rt.isUnsigned()) {
                    X->type.make_unsigned();
                }
                else if (!lt.isUnsigned() && !rt.isUnsigned()) {
                    X->type.make_signed();
                }
                TACOperand* t1 = new_temp_var(); // TAC
                X->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(X->type.to_string()), left->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_BIT_XOR), X->result, t1, right->result, 0); // TAC
                X->code.push_back(i1); // TAC
                X->code.push_back(i2); // TAC

                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), X->result, new_empty_var(), 2); // TAC
                TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), X->result, new_empty_var(), 2); // TAC
                TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                X->true_list.insert(i3_); // TAC
                X->false_list.insert(i4_); // TAC
                X->jump_true_list.insert(i3); // TAC
                X->jump_false_list.insert(i4); // TAC
                X->jump_code.push_back(i1); // TAC
                X->jump_code.push_back(i2); // TAC
                X->jump_code.push_back(i3); // TAC
                X->jump_code.push_back(i4); // TAC
                X->jump_code.push_back(i3_); // TAC
                X->jump_code.push_back(i4_); // TAC
            }
        }
        else {
            X->type = ERROR_TYPE;
            string error_msg = "Operands of '^' must be integers at line " +
                to_string(X->line_no) + ", column " + to_string(X->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return X;
        }
    }
    X->type.is_const_literal = false;
    return X;
}

// ##############################################################################
// ################################## OR EXPRESSION ######################################
// ##############################################################################

OrExpression::OrExpression() {
    xor_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "OR EXPRESSION";
}

Expression* create_or_expression(Expression* x) {
    OrExpression* C = new OrExpression();
    C->xor_expression = dynamic_cast<XorExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->type = x->type;
    C->result = x->result; // TAC
    C->true_list = x->true_list; // TAC
    C->false_list = x->false_list; // TAC
    C->jump_true_list = x->jump_true_list; // TAC
    C->jump_false_list = x->jump_false_list; // TAC
    C->code = x->code; // TAC
    C->jump_code = x->jump_code; // TAC
    C->next_list = x->next_list; // TAC
    C->jump_next_list = x->jump_next_list; // TAC
    return C;
}

Expression* create_or_expression(Expression* left, Terminal* op, Expression* right) {
    OrExpression* O = new OrExpression();
    O->left = left;
    O->right = right;
    O->op = op;
    O->line_no = left->line_no;
    O->column_no = left->column_no;
    O->name = "OR EXPRESSION";
    O->code.insert(O->code.begin(), left->code.begin(), left->code.end()); // TAC
    O->code.insert(O->code.end(), right->code.begin(), right->code.end()); // TAC
    O->jump_code.insert(O->jump_code.begin(), left->code.begin(), left->code.end()); // TAC
    O->jump_code.insert(O->jump_code.end(), right->code.begin(), right->code.end()); // TAC
    for(auto i:left->jump_next_list){
        O->code.erase(remove(O->code.begin(), O->code.end(), i), O->code.end()); // TAC
    }
    for(auto i:right->jump_next_list){
        O->code.erase(remove(O->code.begin(), O->code.end(), i), O->code.end()); // TAC
    }
    for(auto i:left->next_list){
        O->jump_code.erase(remove(O->jump_code.begin(), O->jump_code.end(), i), O->jump_code.end()); // TAC
    }
    for(auto i:right->next_list){
        O->jump_code.erase(remove(O->jump_code.begin(), O->jump_code.end(), i), O->jump_code.end()); // TAC
    }

    if (left->type.is_error() || right->type.is_error()) {
        O->type = ERROR_TYPE;
        return O;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (op->name == "BITWISE_OR") {
        if (lt.isInt() && rt.isInt()) {
            if (lt.type_index > rt.type_index) {
                O->type = lt;
                if (lt.isUnsigned() || rt.isUnsigned()) {
                    O->type.make_unsigned();
                }
                else if (!lt.isUnsigned() && !rt.isUnsigned()) {
                    O->type.make_signed();
                }
                TACOperand* t1 = new_temp_var(); // TAC
                O->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(O->type.to_string()), right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_BIT_OR), O->result, left->result, t1, 0); // TAC
                O->code.push_back(i1); // TAC
                O->code.push_back(i2); // TAC

                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), O->result, new_empty_var(), 2); // TAC
                TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), O->result, new_empty_var(), 2); // TAC
                TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                O->true_list.insert(i3_); // TAC
                O->false_list.insert(i4_); // TAC
                O->jump_true_list.insert(i3); // TAC
                O->jump_false_list.insert(i4); // TAC
                O->jump_code.push_back(i1); // TAC
                O->jump_code.push_back(i2); // TAC
                O->jump_code.push_back(i3); // TAC
                O->jump_code.push_back(i4); // TAC
                O->jump_code.push_back(i3_); // TAC
                O->jump_code.push_back(i4_); // TAC
            }
            else if (lt.type_index == rt.type_index) {
                O->type = lt;
                O->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_BIT_OR), O->result, left->result, right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                O->code.push_back(i1); // TAC

                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), O->result, new_empty_var(), 2); // TAC
                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), O->result, new_empty_var(), 2); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                O->true_list.insert(i2_); // TAC
                O->false_list.insert(i3_); // TAC
                O->jump_true_list.insert(i2); // TAC
                O->jump_false_list.insert(i3); // TAC
                O->jump_code.push_back(i1); // TAC
                O->jump_code.push_back(i2); // TAC
                O->jump_code.push_back(i3); // TAC
                O->jump_code.push_back(i2_); // TAC
                O->jump_code.push_back(i3_); // TAC
            }
            else {
                O->type = rt;
                if (lt.isUnsigned() || rt.isUnsigned()) {
                    O->type.make_unsigned();
                }
                else if (!lt.isUnsigned() && !rt.isUnsigned()) {
                    O->type.make_signed();
                }
                TACOperand* t1 = new_temp_var(); // TAC
                O->result = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(O->type.to_string()), left->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_BIT_OR), O->result, t1, right->result, 0); // TAC
                O->code.push_back(i1); // TAC
                O->code.push_back(i2); // TAC

                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), O->result, new_empty_var(), 2); // TAC
                TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), O->result, new_empty_var(), 2); // TAC
                TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                O->true_list.insert(i3_); // TAC
                O->false_list.insert(i4_); // TAC
                O->jump_true_list.insert(i3); // TAC
                O->jump_false_list.insert(i4); // TAC
                O->jump_code.push_back(i1); // TAC
                O->jump_code.push_back(i2); // TAC
                O->jump_code.push_back(i3); // TAC
                O->jump_code.push_back(i4); // TAC
                O->jump_code.push_back(i3_); // TAC
                O->jump_code.push_back(i4_); // TAC
            }
        }
        else {
            O->type = ERROR_TYPE;
            string error_msg = "Operands of '|' must be integers at line " +
                to_string(O->line_no) + ", column " + to_string(O->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return O;
        }
    }
    O->type.is_const_literal = false;
    return O;
}

// ##############################################################################
// ################################## LOGICAL AND EXPRESSION ######################################
// ##############################################################################

LogicalAndExpression::LogicalAndExpression() {
    or_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "LOGICAL AND EXPRESSION";
}

Expression* create_logical_and_expression(Expression* x) {
    LogicalAndExpression* C = new LogicalAndExpression();
    C->or_expression = dynamic_cast<OrExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->type = x->type;
    C->result = x->result; // TAC
    C->true_list = x->true_list; // TAC
    C->false_list = x->false_list; // TAC
    C->jump_true_list = x->jump_true_list; // TAC
    C->jump_false_list = x->jump_false_list; // TAC
    C->code = x->code; // TAC
    C->jump_code = x->jump_code; // TAC
    C->next_list = x->next_list; // TAC
    C->jump_next_list = x->jump_next_list; // TAC
    return C;
}

Expression* create_logical_and_expression(Expression* left, Terminal* op, Expression* right) {
    LogicalAndExpression* L = new LogicalAndExpression();
    L->left = left;
    L->right = right;
    L->op = op;
    L->line_no = left->line_no;
    L->column_no = left->column_no;
    L->name = "LOGICAL AND EXPRESSION";

    if (left->type.is_error() || right->type.is_error()) {
        L->type = ERROR_TYPE;
        return L;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (op->name == "LOGICAL_AND") {
        if (lt.isIntorFloat() && rt.isIntorFloat()) {
            L->type = Type(PrimitiveTypes::INT_T, 0, false);
            if (lt.isUnsigned() != rt.isUnsigned()) {
                // causes undefined behaviour
                L->type = ERROR_TYPE;
                string error_msg = "Operands of '&&' must have same signedness " +
                    to_string(L->line_no) + ", column " + to_string(L->column_no);
                yyerror(error_msg.c_str());
                symbolTable.set_error();
                return L;
            }
            else{
                TACOperand* t1 = new_temp_var(); // TAC
                L->result = new_temp_var(); // TAC
                L->code = left->jump_code; // TAC
                L->code.insert(L->code.end(), right->jump_code.begin(), right->jump_code.end()); // TAC
                for(auto i:left->jump_true_list){
                    if(left->true_list.find(i)==left->true_list.end() && left->false_list.find(i)==left->false_list.end())L->code.erase(remove(L->code.begin(),L->code.end(),i), L->code.end()); // TAC
                }
                for(auto i:right->jump_true_list){
                    if(right->true_list.find(i)==right->true_list.end() && right->false_list.find(i)==right->false_list.end())L->code.erase(remove(L->code.begin(),L->code.end(),i), L->code.end()); // TAC
                }
                for(auto i:left->jump_false_list){
                    if(left->true_list.find(i)==left->true_list.end() && left->false_list.find(i)==left->false_list.end())L->code.erase(remove(L->code.begin(),L->code.end(),i), L->code.end()); // TAC
                }
                for(auto i:right->jump_false_list){
                    if(right->true_list.find(i)==right->true_list.end() && right->false_list.find(i)==right->false_list.end())L->code.erase(remove(L->code.begin(),L->code.end(),i), L->code.end()); // TAC
                }
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), L->result, new_constant("1"), new_empty_var(), 0); // TAC
                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), L->result, new_constant("0"), new_empty_var(), 0); // TAC
                backpatch(left->true_list, right->jump_code[0]->label); // TAC
                backpatch(left->false_list, i3->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(right->false_list, i3->label); // TAC

                L->next_list.insert(i2_); // TAC
                L->jump_next_list.insert(i2); // TAC
                L->code.push_back(i1); // TAC
                L->code.push_back(i2); // TAC
                L->code.push_back(i2_); // TAC
                L->code.push_back(i3); // TAC

                L->jump_code = left->jump_code; // TAC
                L->jump_code.insert(L->jump_code.end(), right->jump_code.begin(), right->jump_code.end()); // TAC
                for(auto i:left->true_list){
                    if(left->jump_true_list.find(i)==left->jump_true_list.end() && left->jump_false_list.find(i)==left->jump_false_list.end())L->jump_code.erase(remove(L->jump_code.begin(),L->jump_code.end(),i), L->jump_code.end()); // TAC
                }
                for(auto i:right->true_list){
                    if(right->jump_true_list.find(i)==right->jump_true_list.end() && right->jump_false_list.find(i)==right->jump_false_list.end())L->jump_code.erase(remove(L->jump_code.begin(),L->jump_code.end(),i), L->jump_code.end()); // TAC
                }
                for(auto i:left->false_list){
                    if(left->jump_true_list.find(i)==left->jump_true_list.end() && left->jump_false_list.find(i)==left->jump_false_list.end())L->jump_code.erase(remove(L->jump_code.begin(),L->jump_code.end(),i), L->jump_code.end()); // TAC
                }
                for(auto i:right->false_list){
                    if(right->jump_true_list.find(i)==right->jump_true_list.end() && right->jump_false_list.find(i)==right->jump_false_list.end())L->jump_code.erase(remove(L->jump_code.begin(),L->jump_code.end(),i), L->jump_code.end()); // TAC
                }
                backpatch(left->jump_true_list, right->jump_code[0]->label); // TAC
                L->jump_true_list = right->jump_true_list; // TAC
                L->jump_false_list = merge_lists(left->jump_false_list, right->jump_false_list); // TAC
                L->true_list = L->jump_true_list; // TAC
                L->false_list = L->jump_false_list; // TAC
            }
        }
        else {
            L->type = ERROR_TYPE;
            string error_msg = "Operands of '&&' must be integers or float at line " +
                to_string(L->line_no) + ", column " + to_string(L->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }
    }
    L->type.is_const_literal = false;
    return L;
}

// ##############################################################################
// ################################## LOGICAL OR EXPRESSION ######################################
// ##############################################################################

LogicalOrExpression::LogicalOrExpression() {
    logical_and_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "LOGICAL OR EXPRESSION";
}

Expression* create_logical_or_expression(Expression* x) {
    LogicalOrExpression* C = new LogicalOrExpression();
    C->logical_and_expression = dynamic_cast<LogicalAndExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->type = x->type;
    C->result = x->result; // TAC
    C->true_list = x->true_list; // TAC
    C->false_list = x->false_list; // TAC
    C->jump_true_list = x->jump_true_list; // TAC
    C->jump_false_list = x->jump_false_list; // TAC
    C->code = x->code; // TAC
    C->jump_code = x->jump_code; // TAC
    C->next_list = x->next_list; // TAC
    C->jump_next_list = x->jump_next_list; // TAC
    return C;
}

Expression* create_logical_or_expression(Expression* left, Terminal* op, Expression* right) {
    LogicalOrExpression* L = new LogicalOrExpression();
    L->left = left;
    L->right = right;
    L->op = op;
    L->line_no = left->line_no;
    L->column_no = left->column_no;

    if (left->type.is_error() || right->type.is_error()) {
        L->type = ERROR_TYPE;
        return L;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (op->name == "LOGICAL_OR") {
        if (lt.isIntorFloat() && rt.isIntorFloat()) {
            L->type = Type(PrimitiveTypes::INT_T, 0, false);
            if (lt.isUnsigned() != rt.isUnsigned()) {
                // causes undefined behaviour
                L->type = ERROR_TYPE;
                string error_msg = "Operands of '||' must have same signedness " +
                    to_string(L->line_no) + ", column " + to_string(L->column_no);
                yyerror(error_msg.c_str());
                symbolTable.set_error();
                return L;
            }
            else{
                TACOperand* t1 = new_temp_var(); // TAC
                L->result = new_temp_var(); // TAC
                L->code = left->jump_code; // TAC
                L->code.insert(L->code.end(), right->jump_code.begin(), right->jump_code.end()); // TAC
                for(auto i:left->jump_true_list){
                    if(left->true_list.find(i)==left->true_list.end() && left->false_list.find(i)==left->false_list.end())L->code.erase(remove(L->code.begin(),L->code.end(),i), L->code.end()); // TAC
                }
                for(auto i:right->jump_true_list){
                    if(right->true_list.find(i)==right->true_list.end() && right->false_list.find(i)==right->false_list.end())L->code.erase(remove(L->code.begin(),L->code.end(),i), L->code.end()); // TAC
                }
                for(auto i:left->jump_false_list){
                    if(left->true_list.find(i)==left->true_list.end() && left->false_list.find(i)==left->false_list.end())L->code.erase(remove(L->code.begin(),L->code.end(),i), L->code.end()); // TAC
                }
                for(auto i:right->jump_false_list){
                    if(right->true_list.find(i)==right->true_list.end() && right->false_list.find(i)==right->false_list.end())L->code.erase(remove(L->code.begin(),L->code.end(),i), L->code.end()); // TAC
                }
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), L->result, new_constant("1"), new_empty_var(), 0); // TAC
                TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), L->result, new_constant("0"), new_empty_var(), 0); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(left->false_list, right->jump_code[0]->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(right->false_list, i3->label); // TAC
                L->next_list.insert(i2_); // TAC
                L->jump_next_list.insert(i2); // TAC
                L->code.push_back(i1); // TAC
                L->code.push_back(i2); // TAC
                L->code.push_back(i2_); // TAC
                L->code.push_back(i3); // TAC

                L->jump_code = left->jump_code; // TAC
                L->jump_code.insert(L->jump_code.end(), right->jump_code.begin(), right->jump_code.end()); // TAC
                for(auto i:left->true_list){
                    if(left->jump_true_list.find(i)==left->jump_true_list.end() && left->jump_false_list.find(i)==left->jump_false_list.end())L->jump_code.erase(remove(L->jump_code.begin(),L->jump_code.end(),i), L->jump_code.end()); // TAC
                }
                for(auto i:right->true_list){
                    if(right->jump_true_list.find(i)==right->jump_true_list.end() && right->jump_false_list.find(i)==right->jump_false_list.end())L->jump_code.erase(remove(L->jump_code.begin(),L->jump_code.end(),i), L->jump_code.end()); // TAC
                }
                for(auto i:left->false_list){
                    if(left->jump_true_list.find(i)==left->jump_true_list.end() && left->jump_false_list.find(i)==left->jump_false_list.end())L->jump_code.erase(remove(L->jump_code.begin(),L->jump_code.end(),i), L->jump_code.end()); // TAC
                }
                for(auto i:right->false_list){
                    if(right->jump_true_list.find(i)==right->jump_true_list.end() && right->jump_false_list.find(i)==right->jump_false_list.end())L->jump_code.erase(remove(L->jump_code.begin(),L->jump_code.end(),i), L->jump_code.end()); // TAC
                }
                backpatch(left->jump_false_list, right->jump_code[0]->label); // TAC
                L->false_list = right->jump_false_list; // TAC
                L->true_list = merge_lists(left->jump_true_list, right->jump_true_list); // TAC
                L->jump_true_list = L->true_list; // TAC
                L->jump_false_list = L->false_list; // TAC
            }
        }
        else {
            L->type = ERROR_TYPE;
            string error_msg = "Operands of '||' must be integers or float at line " +
                to_string(L->line_no) + ", column " + to_string(L->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }
    }
    L->type.is_const_literal = false;
    return L;
}

// ##############################################################################
// ################################## CONDITIONAL EXPRESSION ######################################
// ##############################################################################

ConditionalExpression::ConditionalExpression() {
    logical_or_expression = nullptr;
    condition = nullptr;
    true_expr = nullptr;
    false_expr = nullptr;
    name = "CONDITIONAL EXPRESSION";
}

Expression* create_conditional_expression(Expression* x) {
    ConditionalExpression* C = new ConditionalExpression();
    C->logical_or_expression = dynamic_cast<LogicalOrExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->type = x->type;
    C->result = x->result; // TAC
    C->true_list = x->true_list; // TAC
    C->false_list = x->false_list; // TAC
    C->jump_true_list = x->jump_true_list; // TAC
    C->jump_false_list = x->jump_false_list; // TAC
    C->code = x->code; // TAC
    C->jump_code = x->jump_code; // TAC
    C->next_list = x->next_list; // TAC
    C->jump_next_list = x->jump_next_list; // TAC
    return C;
}

Expression* create_conditional_expression(Expression* condition, Expression* true_expr, Expression* false_expr) {
    ConditionalExpression* C = new ConditionalExpression();
    C->condition = condition;
    C->true_expr = true_expr;
    C->false_expr = false_expr;
    C->line_no = condition->line_no;
    C->column_no = condition->column_no;
    C->name = "CONDITIONAL EXPRESSION";
    C->code.insert(C->code.begin(), condition->jump_code.begin(), condition->jump_code.end()); // TAC
    C->jump_code.insert(C->jump_code.begin(), condition->jump_code.begin(), condition->jump_code.end()); // TAC

    if (condition->type.is_error() || true_expr->type.is_error() || false_expr->type.is_error()) {
        C->type = ERROR_TYPE;
        return C;
    }

    Type ct = condition->type;
    Type tt = true_expr->type;
    Type ft = false_expr->type;

    if (!ct.isIntorFloat()) {
        C->type = ERROR_TYPE;
        std::string error_msg = "Condition of conditional expression must be of type int or float at line " +
            std::to_string(C->line_no) + ", column " + std::to_string(C->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return C;
    }
    else if (tt == ft) {
        C->type = tt;
        C->result = new_temp_var(); // TAC
        C->code.insert(C->code.end(), true_expr->code.begin(), true_expr->code.end()); // TAC
        for(auto i:true_expr->jump_next_list){
            C->code.erase(remove(C->code.begin(),C->code.end(),i), C->code.end()); // TAC
        }
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), C->result, true_expr->result, new_empty_var(), 0); // TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
        C->code.push_back(i1); // TAC
        C->code.push_back(i2); // TAC
        C->code.push_back(i2_); // TAC
        C->code.insert(C->code.end(), false_expr->code.begin(), false_expr->code.end()); // TAC
        for(auto i:false_expr->jump_next_list){
            C->code.erase(remove(C->code.begin(),C->code.end(),i), C->code.end()); // TAC
        }
        TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), C->result, false_expr->result, new_empty_var(), 0); // TAC
        C->code.push_back(i3); // TAC
        backpatch(condition->true_list, true_expr->code[0]->label); // TAC
        backpatch(condition->true_list, i1->label); // TAC
        backpatch(condition->false_list, false_expr->code[0]->label); // TAC
        backpatch(condition->false_list, i3->label); // TAC
        backpatch(true_expr->next_list, i1->label); // TAC
        backpatch(false_expr->next_list, i3->label); // TAC
        for(auto i:condition->jump_true_list){
            if(condition->true_list.find(i)==condition->true_list.end() && condition->false_list.find(i)==condition->false_list.end())C->code.erase(remove(C->code.begin(),C->code.end(),i), C->code.end()); // TAC
        }
        for(auto i:condition->jump_false_list){
            if(condition->true_list.find(i)==condition->true_list.end() && condition->false_list.find(i)==condition->false_list.end())C->code.erase(remove(C->code.begin(),C->code.end(),i), C->code.end()); // TAC
        }
        C->next_list.insert(i2_); // TAC
        C->jump_next_list.insert(i2); // TA

        C->jump_code = condition->jump_code; // TAC
        C->jump_code.insert(C->jump_code.end(), true_expr->jump_code.begin(), true_expr->jump_code.end()); // TAC
        C->jump_code.insert(C->jump_code.end(), false_expr->jump_code.begin(), false_expr->jump_code.end()); // TAC
        for(auto i:condition->true_list){
            if(condition->jump_true_list.find(i)==condition->jump_true_list.end() && condition->jump_false_list.find(i)==condition->jump_false_list.end())C->jump_code.erase(remove(C->jump_code.begin(),C->jump_code.end(),i), C->jump_code.end()); // TAC
        }
        for(auto i:condition->false_list){
            if(condition->jump_true_list.find(i)==condition->jump_true_list.end() && condition->jump_false_list.find(i)==condition->jump_false_list.end())C->jump_code.erase(remove(C->jump_code.begin(),C->jump_code.end(),i), C->jump_code.end()); // TAC
        }
        backpatch(condition->jump_true_list, true_expr->jump_code[0]->label); // TAC
        backpatch(condition->jump_false_list, false_expr->jump_code[0]->label); // TAC
        C->true_list = merge_lists(true_expr->jump_true_list, false_expr->jump_true_list); // TAC
        C->false_list = merge_lists(true_expr->jump_false_list, false_expr->jump_false_list); // TAC
        C->jump_true_list = C->true_list; // TAC
        C->jump_false_list = C->false_list; // TAC
    }
    else if (tt.isIntorFloat() && ft.isIntorFloat() && tt.is_convertible_to(ft)) {
        // Promote both and take the higher ranked one
        if (tt.type_index > ft.type_index) {
            C->type = tt;
            if (tt.isUnsigned() || ft.isUnsigned()) {
                C->type.make_unsigned();
            }
            TACOperand* t1 = new_temp_var(); // TAC
            TACInstruction* i0 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(C->type.to_string()), false_expr->result, 0); // TAC
            C->result = new_temp_var(); // TAC
            C->code.insert(C->code.end(), true_expr->code.begin(), true_expr->code.end()); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), C->result, t1, new_empty_var(), 0); // TAC
            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i2_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            C->code.push_back(i0); // TAC
            C->code.push_back(i1); // TAC
            C->code.push_back(i2); // TAC
            C->code.push_back(i2_); // TAC
            C->code.insert(C->code.end(), false_expr->code.begin(), false_expr->code.end()); // TAC
            C->code.push_back(i0); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), C->result, t1, new_empty_var(), 0); // TAC
            C->code.push_back(i3); // TAC
            backpatch(condition->true_list, true_expr->code[0]->label); // TAC
            backpatch(condition->true_list, i1->label); // TAC
            backpatch(condition->false_list, false_expr->code[0]->label); // TAC
            backpatch(condition->false_list, i3->label); // TAC
            backpatch(true_expr->next_list, i1->label); // TAC
            backpatch(false_expr->next_list, i3->label); // TAC
            C->next_list.insert(i2_); // TAC
            C->jump_next_list.insert(i2); // TA

            C->jump_code = condition->jump_code; // TAC
            C->jump_code.insert(C->jump_code.end(), true_expr->jump_code.begin(), true_expr->jump_code.end()); // TAC
            C->jump_code.insert(C->jump_code.end(), false_expr->jump_code.begin(), false_expr->jump_code.end()); // TAC
            backpatch(condition->jump_true_list, true_expr->jump_code[0]->label); // TAC
            backpatch(condition->jump_false_list, false_expr->jump_code[0]->label); // TAC
            C->true_list = merge_lists(true_expr->jump_true_list, false_expr->jump_true_list); // TAC
            C->false_list = merge_lists(true_expr->jump_false_list, false_expr->jump_false_list); // TAC
            C->jump_true_list = C->true_list; // TAC
            C->jump_false_list = C->false_list; // TAC
        }
        else if (tt.type_index < ft.type_index) {
            C->type = ft;
            if (tt.isUnsigned() || ft.isUnsigned()) {
                C->type.make_unsigned();
            }
            TACOperand* t1 = new_temp_var(); // TAC
            TACInstruction* i0 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(C->type.to_string()), true_expr->result, 0); // TAC
            C->code.push_back(i0); // TAC
        }
    }
    else {
        C->type = ERROR_TYPE;
        std::string error_msg = "Expressions are incomatible for ternary operator at line " +
            std::to_string(C->line_no) + ", column " + std::to_string(C->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    C->type.is_const_literal = false;
    return C;
}

// ##############################################################################
// ################################## ASSIGNMENT EXPRESSION ######################################
// ##############################################################################

AssignmentExpression::AssignmentExpression() {
    conditional_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "ASSIGNMENT EXPRESSION";
}

Expression* create_assignment_expression(Expression* x) {
    AssignmentExpression* C = new AssignmentExpression();
    C->conditional_expression = dynamic_cast<ConditionalExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->type = x->type;
    C->result = x->result; // TAC
    C->true_list = x->true_list; // TAC
    C->false_list = x->false_list; // TAC
    C->jump_true_list = x->jump_true_list; // TAC
    C->jump_false_list = x->jump_false_list; // TAC
    C->code = x->code; // TAC
    C->jump_code = x->jump_code; // TAC
    C->next_list = x->next_list; // TAC
    C->jump_next_list = x->jump_next_list; // TAC
    // C->begin_label = x->begin_label; // TAC 

    return C;
}

Expression* create_assignment_expression(Expression* left, Terminal* op, Expression* right) {
    AssignmentExpression* A = new AssignmentExpression();
    A->left = left;
    A->right = right;
    A->op = op;
    A->line_no = left->line_no;
    A->column_no = left->column_no;
    A->name = "ASSIGNMENT EXPRESSION";
    A->code.insert(A->code.begin(), left->code.begin(), left->code.end()); // TAC
    A->code.insert(A->code.end(), right->code.begin(), right->code.end()); // TAC
    A->jump_code.insert(A->jump_code.begin(), left->code.begin(), left->code.end()); // TAC
    A->jump_code.insert(A->jump_code.end(), right->code.begin(), right->code.end()); // TAC
    for(auto i:left->jump_next_list){
        A->code.erase(remove(A->code.begin(),A->code.end(),i), A->code.end()); // TAC
    }
    for(auto i:right->jump_next_list){
        A->code.erase(remove(A->code.begin(),A->code.end(),i), A->code.end()); // TAC
    }
    for(auto i:left->jump_next_list){
        A->jump_code.erase(remove(A->jump_code.begin(),A->jump_code.end(),i), A->jump_code.end()); // TAC
    }
    for(auto i:right->jump_next_list){
        A->jump_code.erase(remove(A->jump_code.begin(),A->jump_code.end(),i), A->jump_code.end()); // TAC
    }

    if (left->type.is_error() || right->type.is_error()) {
        A->type = ERROR_TYPE;
        return A;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (lt.is_const_variable) {
        yyerror(("Assignment to const variable at line " + std::to_string(A->line_no)).c_str());
        symbolTable.set_error();
        A->type = ERROR_TYPE;
        return A;
    }

    if (op->name == "ASSIGN") {
        // Check if RHS can be implicitly converted to LHS
        if (!rt.is_convertible_to(lt)) {
            yyerror(("Incompatible types in assignment at line " + std::to_string(A->line_no)).c_str());
            symbolTable.set_error();
            A->type = ERROR_TYPE;
            return A;
        }
        A->type = lt;  // Resulting type is type of LHS
        if (lt != rt) {
            TACOperand* t1 = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(lt.to_string()), right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC

            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), left->result, t1, new_empty_var(), 0); // TAC
            A->code.push_back(i1); // TAC
            A->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC

        }
        else {
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), left->result, right->result, new_empty_var(), 0); // TAC
            A->code.push_back(i1); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
    }
    else if (op->name == "MUL_ASSIGN" || op->name == "DIV_ASSIGN") {
        if (!lt.isIntorFloat() || !rt.isIntorFloat()) {
            A->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must be int or float at line " +
                to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        }
        A->type = lt;  // Resulting type is type of LHS
        if (lt != rt) {
            TACOperand* t1 = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(lt.to_string()), right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(op->name == "MUL_ASSIGN" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), left->result, left->result, t1, 0); // TAC
            A->code.push_back(i1); // TAC
            A->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
        else {
            TACInstruction* i1 = emit(TACOperator(op->name == "MUL_ASSIGN" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), left->result, left->result, right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            A->code.push_back(i1); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
    }
    else if (op->name == "ADD_ASSIGN" || op->name == "SUB_ASSIGN") {
        if (!lt.isIntorFloat() || !rt.isIntorFloat()) {
            A->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must be int or float at line " +
                to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        }
        else if (lt.isPointer() && rt.isInt()) {
            A->type = lt;
            TACOperand* t1 = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_MUL), t1, right->result, new_constant(to_string(lt.get_size())), 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(op->name == "ADD_ASSIGN" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), left->result, left->result, t1, 0); // TAC
            A->code.push_back(i1); // TAC
            A->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
        else {
            A->type = lt;
            if (lt != rt) {
                TACOperand* t1 = new_temp_var(); // TAC
                TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(lt.to_string()), right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                TACInstruction* i2 = emit(TACOperator(op->name == "ADD_ASSIGN" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), left->result, left->result, t1, 0); // TAC
                A->code.push_back(i1); // TAC
                A->code.push_back(i2); // TAC

                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
                TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
                TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                A->true_list.insert(i3_); // TAC
                A->false_list.insert(i4_); // TAC
                A->jump_true_list.insert(i3); // TAC
                A->jump_false_list.insert(i4); // TAC
                A->jump_code.push_back(i1); // TAC
                A->jump_code.push_back(i2); // TAC
                A->jump_code.push_back(i3); // TAC
                A->jump_code.push_back(i4); // TAC
                A->jump_code.push_back(i3_); // TAC
                A->jump_code.push_back(i4_); // TAC
            }
            else {
                TACInstruction* i1 = emit(TACOperator(op->name == "ADD_ASSIGN" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), left->result, left->result, right->result, 0); // TAC
                backpatch(left->next_list, i1->label); // TAC
                backpatch(left->jump_next_list, i1->label); // TAC
                backpatch(right->next_list, i1->label); // TAC
                backpatch(right->jump_next_list, i1->label); // TAC
                backpatch(left->true_list, i1->label); // TAC
                backpatch(left->false_list, i1->label); // TAC
                backpatch(right->true_list, i1->label); // TAC
                backpatch(right->false_list, i1->label); // TAC
                backpatch(left->jump_true_list, i1->label); // TAC
                backpatch(left->jump_false_list, i1->label); // TAC
                backpatch(right->jump_true_list, i1->label); // TAC
                backpatch(right->jump_false_list, i1->label); // TAC
                A->code.push_back(i1); // TAC

                TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
                TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
                TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
                A->true_list.insert(i3_); // TAC
                A->false_list.insert(i4_); // TAC
                A->jump_true_list.insert(i3); // TAC
                A->jump_false_list.insert(i4); // TAC
                A->jump_code.push_back(i1); // TAC
                A->jump_code.push_back(i3); // TAC
                A->jump_code.push_back(i4); // TAC
                A->jump_code.push_back(i3_); // TAC
                A->jump_code.push_back(i4_); // TAC
            }
        }
    }
    else if (op->name == "MOD_ASSIGN") {
        if (!lt.isInt() || !rt.isInt()) {
            A->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must be int at line " +
                to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        }
        A->type = lt;
        if (lt != rt) {
            TACOperand* t1 = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(lt.to_string()), right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_MOD), left->result, left->result, t1, 0); // TAC
            A->code.push_back(i1); // TAC
            A->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
        else {
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_MOD), left->result, left->result, right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            A->code.push_back(i1); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC

        }
    }
    else if (op->name == "AND_ASSIGN" || op->name == "OR_ASSIGN" || op->name == "XOR_ASSIGN") {
        if (!lt.isInt() || !rt.isInt()) {
            A->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must be int at line " +
                to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        }
        A->type = lt;
        if (lt != rt) {
            TACOperand* t1 = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(lt.to_string()), right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(op->name == "AND_ASSIGN" ? TAC_OPERATOR_BIT_AND : op->name == "OR_ASSIGN" ? TAC_OPERATOR_BIT_OR : TAC_OPERATOR_BIT_XOR), left->result, left->result, t1, 0); // TAC
            A->code.push_back(i1); // TAC
            A->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
        else {
            TACInstruction* i1 = emit(TACOperator(op->name == "AND_ASSIGN" ? TAC_OPERATOR_BIT_AND : op->name == "OR_ASSIGN" ? TAC_OPERATOR_BIT_OR : TAC_OPERATOR_BIT_XOR), left->result, left->result, right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            A->code.push_back(i1); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
    }
    else if (op->name == "LEFT_ASSIGN" || op->name == "RIGHT_ASSIGN") {
        if (!lt.isInt() || !rt.isInt()) {
            A->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must be int at line " +
                to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        }
        A->type = lt;
        if (lt != rt) {
            TACOperand* t1 = new_temp_var(); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type(lt.to_string()), right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            TACInstruction* i2 = emit(TACOperator(op->name == "LEFT_ASSIGN" ? TAC_OPERATOR_LEFT_SHIFT : TAC_OPERATOR_RIGHT_SHIFT), left->result, left->result, t1, 0); // TAC
            A->code.push_back(i1); // TAC
            A->code.push_back(i2); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i2); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
        else {
            TACInstruction* i1 = emit(TACOperator(op->name == "LEFT_ASSIGN" ? TAC_OPERATOR_LEFT_SHIFT : TAC_OPERATOR_RIGHT_SHIFT), left->result, left->result, right->result, 0); // TAC
            backpatch(left->next_list, i1->label); // TAC
            backpatch(left->jump_next_list, i1->label); // TAC
            backpatch(right->next_list, i1->label); // TAC
            backpatch(right->jump_next_list, i1->label); // TAC
            backpatch(left->true_list, i1->label); // TAC
            backpatch(left->false_list, i1->label); // TAC
            backpatch(right->true_list, i1->label); // TAC
            backpatch(right->false_list, i1->label); // TAC
            backpatch(left->jump_true_list, i1->label); // TAC
            backpatch(left->jump_false_list, i1->label); // TAC
            backpatch(right->jump_true_list, i1->label); // TAC
            backpatch(right->jump_false_list, i1->label); // TAC
            A->code.push_back(i1); // TAC

            TACInstruction* i3 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            TACInstruction* i3_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), left->result, new_empty_var(), 2); // TAC
            TACInstruction* i4_ = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); // TAC
            A->true_list.insert(i3_); // TAC
            A->false_list.insert(i4_); // TAC
            A->jump_true_list.insert(i3); // TAC
            A->jump_false_list.insert(i4); // TAC
            A->jump_code.push_back(i1); // TAC
            A->jump_code.push_back(i3); // TAC
            A->jump_code.push_back(i4); // TAC
            A->jump_code.push_back(i3_); // TAC
            A->jump_code.push_back(i4_); // TAC
        }
    }
    A->type.is_const_literal = false;
    return A;
}

// ##############################################################################
// ################################## EXPRESSION LIST ######################################   
// ##############################################################################

ExpressionList::ExpressionList() {
    name = "EXPRESSION LIST";
    type = ERROR_TYPE;
}

ExpressionList* create_expression_list(Expression* x) { // top level expression - corresponds to non terminal expression in grammar
    ExpressionList* E = new ExpressionList();
    E->expression_list.push_back(x);
    E->line_no = x->line_no;
    E->column_no = x->column_no;
    E->type = x->type;  // if expression list does not have an erronous expression, set type to the type of the first expression. Else set it as ERROR_TYPE.
    E->result = x->result; // TAC
    E->true_list = x->true_list; // TAC
    E->false_list = x->false_list; // TAC
    E->jump_true_list = x->jump_true_list; // TAC
    E->jump_false_list = x->jump_false_list; // TAC
    E->code = x->code; // TAC
    E->jump_code = x->jump_code; // TAC
    E->next_list = x->next_list; // TAC
    E->jump_next_list = x->jump_next_list; // TAC
    return E;
}

ExpressionList* create_expression_list(ExpressionList* expression_list, Expression* new_expression) {
    expression_list->expression_list.push_back(new_expression);
    expression_list->code.insert(expression_list->code.end(), new_expression->code.begin(), new_expression->code.end()); // TAC
    expression_list->jump_code.insert(expression_list->jump_code.begin(), new_expression->jump_code.begin(), new_expression->jump_code.end()); // TAC
    backpatch(expression_list->next_list, new_expression->code[0]->label); // TAC
    backpatch(expression_list->jump_next_list, new_expression->jump_code[0]->label); // TAC
    expression_list->next_list = new_expression->next_list; // TAC
    expression_list->jump_next_list = new_expression->jump_next_list; // TAC
    expression_list->true_list = new_expression->true_list; // TAC
    expression_list->false_list = new_expression->false_list; // TAC
    expression_list->jump_false_list = new_expression->jump_false_list; // TAC
    expression_list->jump_true_list = new_expression->jump_true_list; // TAC

    if (new_expression->type.is_error()) {
        expression_list->type = ERROR_TYPE;
        return expression_list;
    }
    // debug("expression: ");
    // print_code_vector(expression_list->code); // TAC
    return expression_list;
}