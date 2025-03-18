#include "symbol_table.h"
#include "ast.h"
#include "expression.h"
extern void yyerror(const char *msg);

using namespace std;

Expression :: Expression() : NonTerminal(0, 0, "") {}

Expression* PrimaryExpression :: create_primary_expression_identifier(string x, unsigned int line_no, unsigned int column_no){
    PrimaryExpression* P = new PrimaryExpression();
    Symbol* sym = symbolTable.getSymbol(x);
    if(sym) P->type = sym->type;
    else {
        P->type = ERROR_TYPE;
        string error_msg = "Undeclared Symbol " + x + line_no + column_no;
		yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
}

Expression* PrimaryExpression :: create_primary_expression_constant(string *x){
    PrimaryExpression* P = new PrimaryExpression();

}

