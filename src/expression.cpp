#include "symbol_table.h"
#include "ast.h"
#include "expression.h"
extern void yyerror(const char *msg);

using namespace std;

Expression :: Expression() : NonTerminal(0, 0, "") {}

Expression* PrimaryExpression :: create_primary_expression(Identifier* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION IDENTIFIER";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
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

Expression* PrimaryExpression :: create_primary_expression(Constant* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION CONSTANT";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->type = x->get_constant_type();
    P->add_children(x);
    return P;
}

Expression* PrimaryExpression :: create_primary_expression(StringLiteral* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION STRING LITERAL";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->type = Type(CHAR_T, 1, true) 
    P->add_children(x);
    return P;
}

Expression* PrimaryExpression :: create_primary_expression(Expression* x){
    return x;
}



