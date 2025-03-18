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
        
};

Expression* create_primary_expression(Identifier* x);
Expression* create_primary_expression(Constant* x);
Expression* create_primary_expression(StringLiteral* x);
Expression* create_primary_expression(Expression* x);

class ArgumentExprList : public Expression {
    public:
      std::vector <Expression * > args;
  
      ArgumentExprList() {};
  };
  
class PostfixExpression : public Expression {
    public:
      PostfixExpression *pe;
      Expression *exp;
      Identifier *id;
      ArgumentExprList *ae_list;
      std::string op;
  
      PostfixExpression() {
          pe = nullptr;
          exp = nullptr;
          id = nullptr;
          ae_list = nullptr;
          op = "";
      };
  };

class UnaryExpression : public Expression {
    public:
      Expression *op1;
      std::string op;
  
      UnaryExpression() {
          op1 = nullptr;
          op = "";
      }
};

Expression *create_unary_expression( Terminal * op,Expression *ue ); // INC_OP, DEC_OP, SIZEOF
Expression *create_unary_expression_cast( Node *n_op, Expression *ce );


#endif