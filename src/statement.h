#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include "ast.h"
#include "expression.h"
#include "symbol_table.h"

using namespace std;

class Statement;
class LabeledStatement;
class CompoundStatement;
class DeclarationStatementList;
class StatementList;
class ExpressionStatement;
class SelectionStatement;
class IterationStatement;
class JumpStatement;


class Statement : public NonTerminal{
    public:
    Type type;
    Statement();  
};
 
// ##############################################################################
// ################################## LABELED STATEMENT ######################################
// ##############################################################################

class LabeledStatement : public Statement{
    public: 
        Statement* statement;
        Identifier* identifier;
        Expression* expression;
        int label_type; // -1 for identifier, 0 for case, 1 for default  
        LabeledStatement(); 
};

Statement* create_labeled_statement_identifier(Identifier *identifier, Statement* statement);
Statement* create_labeled_statement_case(Expression *expression, Statement* statement);
Statement* create_labeled_statement_default(Statement* statement);

// ##############################################################################
// ################################## COMPOUND STATEMENT ######################################
// ##############################################################################

class CompoundStatement : public Statement{
    public:
    CompoundStatement();
};

Statement* create_compound_statement();
Statement* create_compound_statement(DeclarationStatementList* statement);

// ##############################################################################
// ################################## DECLARATION STATEMENT LIST ######################################
// ##############################################################################

class DeclarationStatementList : public Statement{
    public:
    DeclarationStatementList();
};
DeclarationStatementList* create_declaration_statement_list(DeclarationList* declaration_list);
DeclarationStatementList* create_declaration_statement_list(StatementList* statement_list);

// ##############################################################################
// ################################## STATEMENT LIST ######################################
// ##############################################################################

class StatementList : public Statement{
    public:
        vector<Statement*> statements;
        StatementList();
};

StatementList* create_statement_list(Statement* statement);
StatementList* create_statement_list(StatementList* statement_list, Statement* statement);

// ##############################################################################
// ################################## EXPRESSION STATEMENT ######################################
// ##############################################################################

class ExpressionStatement : public Statement{
    public:
        Expression* expression;
        ExpressionStatement();
};

// Expression* create_expression_statement();
Statement* create_expression_statement();
Statement* create_expression_statement(Expression* expression);

// ##############################################################################
// ################################## SELECTION STATEMENT ######################################
// ##############################################################################

class SelectionStatement : public Statement{
    public:
        Expression* expression;
        Statement* statement;
        Statement* else_statement;
        int selection_type; // 0 for if, 1 for if-else, 2 for switch 
        SelectionStatement();
};
Statement* create_selection_statement_if(Expression* expression, Statement* statement);
Statement* create_selection_statement_if_else(Expression* expression, Statement* statement, Statement* else_statement);
Statement* create_selection_statement_switch(Expression* expression, Statement* statement);

// ##############################################################################
// ################################## ITERATION STATEMENT ######################################
// ##############################################################################

class IterationStatement : public Statement{
    public:
        IterationStatement();
};
IterationStatement* create_iteration_statement_while(Expression* expression, Statement* statement);
IterationStatement* create_iteration_statement_do_while(Expression* expression, Statement* statement);
IterationStatement* create_iteration_statement_for(Statement* statement1, Statement* statement2, Expression* expression, Statement* statement3);
IterationStatement* create_iteration_statement_for_dec(Declaration* declaration, Statement* statement1, Expression* expression, Statement* statement2);
IterationStatement* create_iteration_statement_until(Expression* expression, Statement* statement);

// ##############################################################################
// ################################## JUMP STATEMENT ######################################
// ##############################################################################

class JumpStatement : public Statement{
    public:
        // return type ko function definition me return krvana hai
        Type return_type;
        JumpStatement();
};

JumpStatement* create_jump_statement(Terminal* op);
JumpStatement* create_jump_statement(Expression* expression);
#endif