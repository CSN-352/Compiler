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
extern unsigned int line_no;
extern unsigned int column_no;

Expression* create_primary_expression(Identifier* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION IDENTIFIER";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->identifier = x;
    P->add_children(x);
    Symbol* sym = symbolTable.getSymbol(x->value);
    if(sym) P->type = sym->type;
    else {
        P->type = ERROR_TYPE;
        string error_msg = "Undeclared Symbol " + x->value + " at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
		yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return P;
}

Expression* create_primary_expression(Constant* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION CONSTANT";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->constant = x;
    P->type = x->get_constant_type();
    P->add_children(x);
    return P;
}

Expression* create_primary_expression(StringLiteral* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION STRING LITERAL";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->string_literal = x;
    P->type = Type(CHAR_T, 1, true);
    P->add_children(x);
    return P;
}

Expression* create_primary_expression(Expression* x){
    return x;
}

ArgumentExpressionList* create_argument_expression_list(Expression* x){
    ArgumentExpressionList* P = new ArgumentExpressionList();
    P->arguments.push_back(x);
    P->add_children(x);
    return P;
}

ArgumentExpressionList* create_argument_expression_list(ArgumentExpressionList* args_expr_list, Expression* x){
    args_expr_list->arguments.push_back(x);
    args_expr_list->add_children(x);
    return args_expr_list;
}

Expression* create_postfix_expression(Expression* x){
    return expression;
}

Expression* create_postfix_expression(Expression* x, Terminal* op){
    PostfixExpression* P = new PostfixExpression(x);
    P->op = op;
    if(op->name = "INC_OP") P->name = "POSTFIX EXPRESSION INC OP";
    else P->name = "POSTFIX EXPRESSION DEC OP";
}

