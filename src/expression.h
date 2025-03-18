#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "ast.h"
#include "symbol_table.h"
using namespace std;

class Expression : public NonTerminal{
    public:
        Type type;
        int operand_cnt;
        Expression();
};

class PrimaryExpression : public Expression{
    public: 
        Expression* create_primary_expression(Identifier* x);
        Expression* create_primary_expression(Constant* x);
        Expression* create_primary_expression(StringLiteral* x);
        Expression* create_primary_expression(Expression* x);
};

#endif