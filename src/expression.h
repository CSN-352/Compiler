#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "ast.h"
#include "symbol_table.h"
#include <vector>
using namespace std;

class Expression;
class PrimaryExpression;
class ArgumentExpressionList;
class PostfixExpression;
class UnaryExpression;
class CastExpression;

class Expression : public NonTerminal{
    public:
        Type type;
        int operand_cnt;

        Expression() : NonTerminal("EXPRESSION"), operand_cnt(0) {}
};

class PrimaryExpression : public Expression{
    public:
        Identifier* identifier;
        Constant* constant;
        StringLiteral* string_literal;
        PrimaryExpression() : Expression(), identifier(nullptr), constant(nullptr), string_literal(nullptr) {
            name = "PRIMARY EXPRESSION";
        }
};

Expression* create_primary_expression(Identifier* x);
Expression* create_primary_expression(Constant* x);
Expression* create_primary_expression(StringLiteral* x);
Expression* create_primary_expression(Expression* x);

class ArgumentExpressionList : public Expression{
    public:
      vector <Expression*> arguments;
      ArgumentExpressionList() : Expression() {
        name = "ARGUMENT EXPRESSION LIST";
      };
};
  
ArgumentExpressionList* create_argument_expression_list(Expression* x);
ArgumentExpressionList* create_argument_expression_list(ArgumentExpressionList* args_expr_list, Expression* x);

class PostfixExpression : public Expression{
    public:
        PostfixExpression* base_expression;
        vector<Expression*> arguments;
        Terminal* op;
        Identifier* member_name;
        Expression* expression;

        PostfixExpression() : Expression() {
            name = "POSTFIX EXPRESSION";
        }
};

Expression* create_postfix_expression(Expression* x);
Expression* create_postfix_expression(Expression* x, Terminal* op);
//Expression* create_postfix_expression(Expression* x, Terminal* op, Identifier* id);

class UnaryExpression : public Expression{
    public:
        Terminal* op;
        UnaryExpression* unary_expression;
        CastExpression* cast_expression;

};

class CastExpression : public Expression{
    
};

#endif