#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include "symbol_table.h"
#include "ast.h"
using namespace std;

class Expression;
class PrimaryExpression;
class ArgumentExpressionList;
class PostfixExpression;
class UnaryExpression;
class CastExpression;

class Type;
class Identifier;
class Constant;
class StringLiteral;
class TypeName;

// ##############################################################################
// ################################## EXPRESSION ######################################
// ##############################################################################

class Expression : public NonTerminal{
    public:
        Type type;
        int operand_cnt;
        vector<AssignmentExpression*> assignment_expression_list;
        Expression();
};

// ##############################################################################
// ################################## PRIMARY EXPRESSION ######################################
// ##############################################################################

class PrimaryExpression : public Expression{
    public:
        Identifier* identifier;
        Constant* constant;
        StringLiteral* string_literal;
        PrimaryExpression();
};

Expression* create_primary_expression(Identifier* x);
Expression* create_primary_expression(Constant* x);
Expression* create_primary_expression(StringLiteral* x);
Expression* create_primary_expression(Expression* x);

// ##############################################################################
// ################################## ARGUMENT EXPRESSION LIST ######################################
// ##############################################################################


class ArgumentExpressionList : public Expression{
    public:
      vector <Expression*> arguments;
      ArgumentExpressionList();
};
  
ArgumentExpressionList* create_argument_expression_list(Expression* x);
ArgumentExpressionList* create_argument_expression_list(ArgumentExpressionList* args_expr_list, Expression* x);

// ##############################################################################
// ################################## POSTFIX EXPRESSION ######################################
// ##############################################################################

class PostfixExpression : public Expression{
    public:
        PostfixExpression* base_expression;
        Expression* index_expression;
        ArgumentExpressionList* argument_expression_list;
        Terminal* op;
        Identifier* member_name;
        PostfixExpression();
};

Expression* create_postfix_expression(Expression* x);
Expression* create_postfix_expression(Expression* x, Terminal* op);
Expression* create_postfix_expression(Expression* x, Terminal* op, Identifier* id);

class UnaryExpression : public Expression{
    public:
        Terminal* op;
        UnaryExpression* unary_expression;
        CastExpression* cast_expression;
        UnaryExpression();
};

class CastExpression : public Expression{
    public:
        CastExpression* base_expression;
        TypeName* type_name;
        CastExpression();
};

class ConditionalExpression : public Expression{

};

#endif