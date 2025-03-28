#include "symbol_table.h"
#include "ast.h"
#include "expression.h"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>
// #include <3ac.h>
// #include <codegen.h>
// #include <statement.h>
// #include <y.tab.h>
#include <vector>
#include <utility>
#include <string>
#include <iterator>
#include <string.h>
extern void yyerror(const char *msg);

using namespace std;
Type ERROR_TYPE;

// ##############################################################################
// ################################## EXPRESSION ######################################
// ##############################################################################

Expression :: Expression() : NonTerminal("EXPRESSION"){
    operand_cnt = 0;
}

// ##############################################################################
// ################################## PRIMARY EXPRESSION ######################################
// ##############################################################################

PrimaryExpression :: PrimaryExpression() : Expression() {
    name = "PRIMARY EXPRESSION";
    identifier = nullptr;
    constant = nullptr;
    string_literal = nullptr;
}

Expression* create_primary_expression(Identifier* i){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION IDENTIFIER";
    P->line_no = i->line_no;
    P->column_no = i->column_no;
    P->identifier = i;
    Symbol* sym = symbolTable.getSymbol(i->value);
    if(sym) P->type = sym->type;
    else {
        P->type = ERROR_TYPE;
        string error_msg = "Undeclared Symbol " + i->value + " at line " + to_string(i->line_no) + ", column " + to_string(i->column_no);
		yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return P;
}

Expression* create_primary_expression(Constant* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION CONSTANT";
    //P->add_children(x);
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->constant = x;
    P->type = x->get_constant_type();
    return P;
}

Expression* create_primary_expression(StringLiteral* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION STRING LITERAL";
    //P->add_children(x);
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->string_literal = x;
    P->type = Type(CHAR_T, 1, true);
    return P;
}

Expression* create_primary_expression(Expression* x){
    return x;
}

// ##############################################################################
// ################################## ARGUMENT EXPRESSION LIST ######################################
// ##############################################################################

ArgumentExpressionList :: ArgumentExpressionList() : Expression() {
    name = "ARGUMENT EXPRESSION LIST";
};

ArgumentExpressionList* create_argument_expression_list(Expression* x){
    ArgumentExpressionList* P = new ArgumentExpressionList();
    P->arguments.push_back(x);
    //P->add_children(x);
    return P;
}

ArgumentExpressionList* create_argument_expression_list(ArgumentExpressionList* args_expr_list, Expression* x){
    args_expr_list->arguments.push_back(x);
    //args_expr_list->add_children(x);
    return args_expr_list;
}

// ##############################################################################
// ################################## POSTFIX EXPRESSION ######################################
// ##############################################################################

PostfixExpression :: PostfixExpression() : Expression() {
    name = "POSTFIX EXPRESSION";
    base_expression = nullptr;
    index_expression = nullptr;
    op = nullptr;
    member_name = nullptr;
}

Expression* create_postfix_expression(Expression* x){
    return x;
}

Expression* create_postfix_expression(Expression* x, Terminal* op){
    PostfixExpression* P = new PostfixExpression();
    //P->add_children(x);
    P->base_expression = dynamic_cast<PostfixExpression *>(x);
    P->op = op;
    if(op->name == "INC_OP") P->name = "POSTFIX EXPRESSION INC OP";
    else P->name = "POSTFIX EXPRESSION DEC OP";
    if(x->type.is_const_variable || x->type.is_array || x->type.is_function){
        P->type = ERROR_TYPE;
        string error_msg = "Invalid operator " + op->value + " at line  " + to_string(op->line_no) + ", column " + to_string(op->column_no);
		yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    else if(x->type.isInt()){
        P->type = x->type;
    }
    else if(x->type.isFloat()){
        P->type = x->type;
    }
    else if(x->type.is_pointer){
        P->type = x->type;
    }
    else{
        P->type = ERROR_TYPE;
        string error_msg = "Invalid operator " + op->value + " at line  " + to_string(op->line_no) + ", column " + to_string(op->column_no);
		yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    return P;
}

Expression* create_postfix_expression(Expression* x, Terminal* op, Identifier* id){
    PostfixExpression* P = new PostfixExpression();
    P->base_expression = dynamic_cast<PostfixExpression *>(x);
    P->op = op;
    P->member_name = id;
    if(x->type.is_error()){
        P->type = ERROR_TYPE;
        return P;
    }
    
    if(op->name = "PTR_OP"){
        P->name = "POSTFIX EXPRESSION PTR_OP";
        if(!(x->type.is_pointer) && !(x->type.is_defined_type)){
            P->type = ERROR_TYPE;
            string error_msg = "Operator '->' applied to non-pointer-to-struct/union/class at line " +
                         to_string(op->line_no) + ", column " + to_string(op->column_no);
            yyerror(err.c_str());
            symbolTable.set_error();
            return P;
        }
    }
    else if (op->name == "DOT") {
        P->name = "POSTFIX EXPRESSION DOT";
        if (!base_type.is_defined_type) {
            P->type = ERROR_TYPE;
            string error_msg = "Operator '.' applied to non-struct/union/class at line " +
                         to_string(op->line_no) + ", column " + to_string(op->column_no);
            yyerror(err.c_str());
            symbolTable.set_error();
            return P;
        }
    }
    else {
        P->type = x->type;
    }
    return P;
}

Expression* create_postfix_expression(Expression* x, Expression* index_expression) {
    PostfixExpression* P = new PostfixExpression();
    P->name = "POSTFIX EXPRESSION ARRAY ACCESS";
    P->base_expression = dynamic_cast<PostfixExpression*>(x);
    P->index_expression = index_expression;

    // Type checking
    if (!x->type.is_pointer && !x->type.is_array) {
        P->type = ERROR_TYPE;
        string error_msg = "Cannot index non-array/pointer at line " + to_string(index_expr->line_no);
        yyerror(err.c_str());
        symbolTable.set_error();
    } else if (!index_expression->type.isInt()) {
        P->type = ERROR_TYPE;
        string err = "Array index must be integer at line " + to_string(index_expr->line_no);
        yyerror(err.c_str());
        symbolTable.set_error();
    } else {
        P->type = x->type;
    }

    return P;
}

Expression* create_postfix_expression(Expression* x, ArgumentExpressionList* argument_expression_list) {
    // check if exist in symbol table and arguements no. and types
    PostfixExpression* P = new PostfixExpression();
    P->name = "POSTFIX EXPRESSION FUNCTION CALL WITH ARGS";
    P->base_expression = dynamic_cast<PostfixExpression*>(x);
    P->argument_expression_list = argument_expression_list;

    // Type check: should be a function or function pointer
    if (!x->type.is_function && !(x->type.is_function && x->type.is_pointer)) {
        P->type = ERROR_TYPE;
        string error_msg = "Called object is not a function at line " + to_string(x->line_no);
        yyerror(err.c_str());
        symbolTable.set_error();
    } else {
        P->type = x->type;
    }

    return P;
}

Expression* create_postfix_expression(Expression* func_expr, Terminal* left_paren, Terminal* right_paren) {
    // correct the function
    PostfixExpression* P = new PostfixExpression();
    P->name = "POSTFIX EXPRESSION FUNCTION CALL EMPTY";
    P->add_children(func_expr);
    P->add_children(left_paren);
    P->add_children(right_paren);
    P->base_expression = dynamic_cast<PostfixExpression*>(func_expr);

    // Type check: should be a function or function pointer
    if (!func_expr->type.is_function && !func_expr->type.is_function_pointer()) {
        P->type = ERROR_TYPE;
        string err = "Called object is not a function at line " + to_string(left_paren->line_no);
        yyerror(err.c_str());
        symbolTable.set_error();
    } else {
        P->type = *(func_expr->type.return_type);
    }

    return P;
}



// ##############################################################################
// ################################## UNARY EXPRESSION ######################################
// ##############################################################################

UnaryExpression :: UnaryExpression() : Expression() {
    unary_expression = nullptr;
    op = nullptr;
    cast_expression = nullptr;
    name = "UNARY EXPRESSION";
}

// ##############################################################################
// ################################## CAST EXPRESSION ######################################
// ##############################################################################
CastExpression :: CastExpression(): Expression(){
    base_expression = nullptr;
    type_name = NULL;
}

// ##############################################################################
// ################################## CONDITIONAL EXPRESSION ######################################
// ##############################################################################

