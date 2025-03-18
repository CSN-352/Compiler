#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "ast.h"
#include "symbol_table.h"
using namespace std;

class Expression : public NonTerminal{
    public:
        string type;
        int operand_cnt;
        Expression();
};

class PrimaryExpression : public Expression{
    public: 
        Expression* create_primary_expression_identifier(string x);
        Expression* create_primary_expression(string x);
};

#endif