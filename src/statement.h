#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include <unordered_set>
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
struct ForIterationStruct;

extern unordered_map<string, TACOperand*> labels; // Map to store labels and their corresponding TAC operands
 extern unordered_map<string, unordered_set<TACInstruction*>> labels_list; // Map to store labels and their corresponding goto

class Statement : public NonTerminal {
public:
    Type type;
    Type return_type;
    unordered_set<TACInstruction*> next_list; // List of next instructions (for jumps)
    TACOperand* begin_label; // Label for the beginning of the statement
    vector<TACInstruction*> code;
    vector<TACInstruction*> continue_list; // List of continue instructions (for loops)
    vector<TACInstruction*> break_list;
    Statement();
};

// ##############################################################################
// ################################## LABELED STATEMENT ######################################
// ##############################################################################

class LabeledStatement : public Statement {
public:
    Statement* statement;
    Identifier* identifier;
    Expression* expression;
    int label_type; // -1 for identifier, 0 for case, 1 for default  

    LabeledStatement();
};

Statement* create_labeled_statement_identifier(Identifier* identifier, Statement* statement);
Statement* create_labeled_statement_case(Expression* expression, Statement* statement);
Statement* create_labeled_statement_default(Statement* statement);

// ##############################################################################
// ################################## COMPOUND STATEMENT ######################################
// ##############################################################################

class CompoundStatement : public Statement {
public:
    CompoundStatement();
};

Statement* create_compound_statement();
Statement* create_compound_statement(DeclarationStatementList* statement);

// ##############################################################################
// ################################## DECLARATION STATEMENT LIST ######################################
// ##############################################################################

class DeclarationStatementList : public Statement {
public:
    DeclarationStatementList();
};
DeclarationStatementList* create_declaration_statement_list(DeclarationList* declaration_list);
DeclarationStatementList* create_declaration_statement_list(StatementList* statement_list);

// ##############################################################################
// ################################## STATEMENT LIST ######################################
// ##############################################################################

class StatementList : public Statement {
public:
    vector<Statement*> statements;
    StatementList();
};

StatementList* create_statement_list(Statement* statement);
StatementList* create_statement_list(StatementList* statement_list, Statement* statement);

// ##############################################################################
// ################################## EXPRESSION STATEMENT ######################################
// ##############################################################################

class ExpressionStatement : public Statement {
public:
    Expression* expression;
    vector<TACInstruction*> jump_code; //TAC
    ExpressionStatement();
};

// Expression* create_expression_statement();
Statement* create_expression_statement();
Statement* create_expression_statement(Expression* expression);

// ##############################################################################
// ################################## SELECTION STATEMENT ######################################
// ##############################################################################

class SelectionStatement : public Statement {
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

class IterationStatement : public Statement {
public:
    IterationStatement();
};
Statement* create_iteration_statement_while(Expression* expression, Statement* statement);
Statement* create_iteration_statement_do_while(Expression* expression, Statement* statement);
Statement* create_iteration_statement_for(Statement* statement1, Statement* statement2, Expression* expression, Statement* statement3);
Statement* create_iteration_statement_for_dec(ForIterationStruct* fis, Expression* expression, Statement* statement2);
Statement* create_iteration_statement_until(Expression* expression, Statement* statement);

// ##############################################################################
// ################################## FOR ITERATION STRUCT ######################################
// ##############################################################################

struct ForIterationStruct {
    Declaration* declaration;
    Statement* statement1;
    ForIterationStruct(Declaration* declaration, Statement* statement1) {
        this->declaration = declaration;
        this->statement1 = statement1;
    }
};

// ##############################################################################
// ################################## JUMP STATEMENT ######################################
// ##############################################################################

class JumpStatement : public Statement {
public:
    // return type ko function definition me return krvana hai
    Type return_type;
    JumpStatement();
};

Statement* create_jump_statement(Terminal* op);
Statement* create_jump_statement(Expression* expression);
#endif