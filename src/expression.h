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
        // vector<AssignmentExpression*> assignment_expression_list;
        Expression();
        virtual ~Expression() {}; // Virtual destructor for proper cleanup
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
        PrimaryExpression* primary_expression;
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
Expression* create_postfix_expression(Expression* x, Expression* index_expression);
Expression* create_postfix_expression_func(Expression* x, ArgumentExpressionList* argument_expression_list);

// ##############################################################################
// ################################## UNARY EXPRESSION ######################################
// ##############################################################################

class UnaryExpression : public Expression{
    public:
        PostfixExpression* postfix_expression;
        Terminal* op;
        UnaryExpression* base_expression;
        CastExpression* cast_expression;
        TypeName* type_name;
        UnaryExpression();
};

Expression* create_unary_expression(Expression* x);
Expression* create_unary_expression(Expression* x, Terminal* op);
Expression* create_unary_expression_cast(Expression* x, Terminal* op);
Expression* create_unary_expression(Terminal* op, TypeName* tn);

// ##############################################################################
// ################################## CAST EXPRESSION ######################################
// ##############################################################################

class CastExpression : public Expression{
    public:
        UnaryExpression* unary_expression;
        CastExpression* base_expression;
        TypeName* type_name;
        CastExpression();
};

Expression* create_cast_expression(Expression* x);
Expression* create_cast_expression(TypeName* tn, Expression* x);

// ##############################################################################
// ################################## MULTIPLICATIVE EXPRESSION ######################################
// ##############################################################################
 
class MultiplicativeExpression: public Expression {
    public:
        Expression* left;
        Expression* right;
        Terminal* op;
        MultiplicativeExpression();
};

Expression* create_multiplicative_expression(Expression* left, Terminal* op, Expression* right);

// ##############################################################################
// ################################## ADDITIVE EXPRESSION ######################################
// ##############################################################################

class AdditiveExpression: public Expression {
    public:
        Expression* left;
        Expression* right;
        Terminal* op;
        AdditiveExpression();
};
Expression* create_additive_expression(Expression* left, Terminal* op, Expression* right);

// ##############################################################################
// ################################## SHIFT EXPRESSION ######################################
// ##############################################################################

class ShiftExpression: public Expression {
    public:
        Expression* left;
        Expression* right;
        Terminal* op;
        ShiftExpression();
};
Expression* create_shift_expression(Expression* left, Terminal* op, Expression* right);

// ##############################################################################
// ################################## RELATIONAL EXPRESSION ######################################
// ##############################################################################

class RelationalExpression: public Expression {
    public:
        Expression* left;
        Expression* right;
        Terminal* op;
        RelationalExpression();
};
Expression* create_relational_expression(Expression* left, Terminal* op, Expression* right);

// ##############################################################################
// ################################## CONDITIONAL EXPRESSION ######################################
// ##############################################################################
class ConditionalExpression : public Expression{
    
};

// ##############################################################################
// ################################## ASSIGNMENT EXPRESSION ######################################
// ##############################################################################
class AssignmentExpression: public Expression{
    
};


#endif