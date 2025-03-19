#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "ast.h"
#include "symbol_table.h"
#include <vector>
using namespace std;

class Expression : public NonTerminal{
    public:
        Type type;
        int operand_cnt;

        Expression() : NonTerminal("EXPRESSION") operand_cnt(0) {}
};

class PrimaryExpression : public Expression{
    public:
        Identifier* identifier;
        Constant* constant;
        StringLiteral* string_literal;
        PrimaryExpression : Expression() name("PRIMARY EXPRESSION"), identifier(nullptr), constant(nullptr), string_literal(nullptr) {}
};

Expression* create_primary_expression(Identifier* x);
Expression* create_primary_expression(Constant* x);
Expression* create_primary_expression(StringLiteral* x);
Expression* create_primary_expression(Expression* x);

class ArgumentExpressionList : public Expression{
    public:
      vector <Expression*> arguments;
      ArgumentExpressionList() : Expression(), name("ARGUMENT EXPRESSION LIST") {};
  };
  
ArgumentExpressionList* create_argument_expression_list(Expression* x);
ArgumentExpressionList* create_argument_expression_list(ArgumentExpressionList* args_expr_list, Expression* x);

class PostfixExpression : public Expression{
    public:
        Expression* base_expression; //The expression being modified
        vector<Expression*> arguments;
        Terminal* op;
        Identifier* member_name;

        PostfixExpression(Expression* base) : Expression(), name("POSTFIX EXPRESSION"), base_expression(base) {}
};

Expression* create_postfix_expression(Expression* x);
Expression* create_postfix_expression(Expression* x, Terminal* op);

#endif