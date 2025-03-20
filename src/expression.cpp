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
    P->add_children(x);
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->identifier = x;
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
    P->add_children(x);
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->constant = x;
    P->type = x->get_constant_type();
    return P;
}

Expression* create_primary_expression(StringLiteral* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION STRING LITERAL";
    P->add_children(x);
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->string_literal = x;
    P->type = Type(CHAR_T, 1, true);
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
    return x;
}

Expression* create_postfix_expression(Expression* x, Terminal* op){
    PostfixExpression* P = new PostfixExpression();
    P->add_children(x);
    P->base_expression = dynamic_cast<PostfixExpression *>(x);
    P->op = op;
    if(op->name == "INC_OP") P->name = "POSTFIX EXPRESSION INC OP";
    if(op->name == "INC_OP") P->name = "POSTFIX EXPRESSION INC OP";
    else P->name = "POSTFIX EXPRESSION DEC OP";
    if(x->type.is_const){
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
}

// Expression* create_postfix_expression(Expression* x, Terminal* op, Identifier* id){
//     PostfixExpression* P = new PostfixExpression();
//     P->add_children(x);
//     P->member_name = id;
//     if(x->type.is_error()){
//         P->type = ERROR_TYPE;
//         return P;
//     }
    
//     return x;
// }

