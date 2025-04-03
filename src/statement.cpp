#include<statement.h>
#include<symbol_table.h>
#include<expression.h>
#include<ast.h>
#include "parser.tab.h"
#include<string>
#include<vector>
#include<utility>

using namespace std;

extern void yyerror(const char *msg);

// ##############################################################################
// ################################## STATEMENT ######################################
// ##############################################################################

Statement::Statement() : NonTerminal("STATEMENT") {
    type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    return_type = Type(PrimitiveTypes::VOID_T, 0, false);
}

// ##############################################################################
// ################################## LABELED STATEMENT ######################################
// ##############################################################################

LabeledStatement::LabeledStatement() : Statement() {
    statement = nullptr;
    identifier = nullptr;
    expression = nullptr;
    label_type = -1; // -1 for identifier, 0 for case, 1 for default
}

Statement* create_labeled_statement_identifier(Identifier *identifier, Statement* statement) {
    LabeledStatement* L = new LabeledStatement();
    L->statement = statement;
    L->identifier = identifier;
    L->label_type = -1; // No label
    L->name = "Labeled Statement IDENTIFIER";

    if(statement->type == ERROR_TYPE){
        L->type = ERROR_TYPE;
        string error_msg = "Invalid statement at line " + to_string(identifier->line_no) + ", column " + to_string(identifier->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    } else {
        L->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return L;
}

Statement* create_labeled_statement_case(Expression *expression, Statement* statement) {
    LabeledStatement* L = new LabeledStatement();
    L->statement = statement;
    L->expression = expression;
    L->label_type = 0; // Case label
    L->name = "Labeled Statement CASE";

    if(!expression->type.isInt()){
        string error_msg = "Case label must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
    } else if(expression->type == ERROR_TYPE || statement->type == ERROR_TYPE){
       L->type = ERROR_TYPE;
       string error_msg = "Invalid statement at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
       yyerror(error_msg.c_str());
       symbolTable.set_error();
    } else {
        L->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return L;
} 

Statement* create_labeled_statement_default(Statement* statement) {
    LabeledStatement* L = new LabeledStatement();
    L->statement = statement;
    L->label_type = 1; // Default label
    L->name = "Labeled Statement DEFAULT";

    if(statement->type == ERROR_TYPE){
        L->type = ERROR_TYPE;
        string error_msg = "Invalid statement at line " + to_string(statement->line_no) + ", column " + to_string(statement->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    } else {
        L->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return L;
}

// ##############################################################################
// ################################## COMPOUND STATEMENT ######################################
// ##############################################################################

CompoundStatement::CompoundStatement() : Statement() {
    name = "COMPOUND STATEMENT";
}

Statement* create_compound_statement() {
    CompoundStatement* C = new CompoundStatement();
    C->name = "COMPOUND STATEMENT";
    C->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    C->return_type = Type(PrimitiveTypes::VOID_T, 0, false);
    return C;
}

Statement *create_compound_statement(DeclarationStatementList *statement)
{
    CompoundStatement* C = new CompoundStatement();
    C->name = "COMPOUND STATEMENT WITH DECALARATION STATEMENT LIST";
    C->return_type = statement->return_type;
    if (statement->type == ERROR_TYPE)
    {
        C->type = ERROR_TYPE;
    }
    else
    {
        C->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return C;
}

// ##############################################################################
// ################################## DECLARATION STATEMENT LIST ######################################
// ##############################################################################

DeclarationStatementList::DeclarationStatementList() : Statement() {
    name = "DECLARATION STATEMENT LIST";
}

DeclarationStatementList* create_declaration_statement_list(DeclarationList* declaration_list) {
    DeclarationStatementList* D = new DeclarationStatementList();
    D->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    
    return D;
}
   
DeclarationStatementList* create_declaration_statement_list(StatementList* statement_list){
    DeclarationStatementList* D = new DeclarationStatementList();
    D->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    D->return_type = statement_list->return_type;
    return D;
}

// ##############################################################################
// ################################## STATEMENT LIST ######################################
// ##############################################################################

StatementList::StatementList() : Statement() {
    name = "STATEMENT LIST";
}

StatementList* create_statement_list(Statement* statement) {
    StatementList* S = new StatementList();
    S->statements.push_back(statement);
    S->return_type = statement->return_type;
    if(statement->type == ERROR_TYPE){
        S->type = ERROR_TYPE;
    } else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return S;
}

StatementList* create_statement_list(StatementList* statement_list, Statement* statement) {
    statement_list->statements.push_back(statement);
    statement_list->return_type = statement->return_type;
    if(statement->type == ERROR_TYPE){
        statement_list->type = ERROR_TYPE;
    } 
    return statement_list;
}

// ##############################################################################
// ################################## EXPRESSION STATEMENT ######################################
// ##############################################################################

ExpressionStatement::ExpressionStatement() : Statement() {
    expression = nullptr;
    name = "EXPRESSION STATEMENT";
}

Statement* create_expression_statement(){
    ExpressionStatement* S = new ExpressionStatement();
    S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    return S;
} 

Statement* create_expression_statement(Expression* x){
    ExpressionStatement* S = new ExpressionStatement();
    S->expression = x;
    S->line_no = x->line_no;
    S->column_no = x->column_no;

    if(x->type == ERROR_TYPE){
        S->type = ERROR_TYPE;
    } else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return S;
}

// ##############################################################################
// ################################## SELECTION STATEMENT ######################################
// ##############################################################################

SelectionStatement::SelectionStatement() : Statement() {
    expression = nullptr;
    statement = nullptr;
    else_statement = nullptr;
    selection_type = -1; // -1 for if, 0 for switch
    name = "SELECTION STATEMENT";
}

Statement* create_selection_statement_if(Expression* expression, Statement* statement) {
    SelectionStatement* S = new SelectionStatement();
    S->expression = expression;
    S->statement = statement;
    S->selection_type = 0; // If statement
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "SELECTION STATEMENT IF";

    if(expression->type == ERROR_TYPE || statement->type == ERROR_TYPE){
        S->type = ERROR_TYPE;
    }
    else if(!expression->type.isInt()){
        S->type = ERROR_TYPE;
        string error_msg = "Condition of if statement must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    } else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return S;
}

Statement* create_selection_statement_if_else(Expression* expression, Statement* statement, Statement* else_statement) {
    SelectionStatement* S = new SelectionStatement();
    S->expression = expression;
    S->statement = statement;
    S->else_statement = else_statement;
    S->selection_type = 1; // If-else statement
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "SELECTION STATEMENT IF ELSE";

    if(expression->type == ERROR_TYPE || statement->type == ERROR_TYPE || else_statement->type == ERROR_TYPE){
        S->type = ERROR_TYPE;
    }
    else if(!expression->type.isInt()){
        S->type = ERROR_TYPE;
        string error_msg = "Condition of if statement must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    } else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return S;
}

Statement* create_selection_statement_switch(Expression* expression, Statement* statement) {
    SelectionStatement* S = new SelectionStatement();
    S->expression = expression;
    S->statement = statement;
    S->selection_type = 2; // Switch statement
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "SELECTION STATEMENT SWITCH";

    if(expression->type == ERROR_TYPE || statement->type == ERROR_TYPE){
        S->type = ERROR_TYPE;
    }
    else if(!expression->type.isInt()){
        S->type = ERROR_TYPE;
        string error_msg = "Condition of switch statement must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    } else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return S;
}

// ##############################################################################
// ################################## ITERATION STATEMENT ######################################
// ##############################################################################

IterationStatement::IterationStatement() : Statement() {
    name = "ITERATION STATEMENT";
}

Statement* create_iteration_statement_while(Expression* expression, Statement* statement) {
    IterationStatement* S = new IterationStatement();
    // S->expression = expression;
    // S->statement = statement;
    // S->iteration_type = 0; // While statement
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "ITERATION STATEMENT WHILE";

    if(expression->type == ERROR_TYPE || statement->type == ERROR_TYPE){
        S->type = ERROR_TYPE;
    } else if(!expression->type.isIntorFloat()){
        S->type = ERROR_TYPE;
        string error_msg = "Condition of while statement must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    } else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return S;
}

Statement* create_iteration_statement_do_while(Expression* expression, Statement* statement) {
    IterationStatement* S = new IterationStatement();
    // S->expression = expression;
    // S->statement = statement;
    // S->iteration_type = 0; // While statement
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "ITERATION STATEMENT WHILE";

    if(expression->type == ERROR_TYPE || statement->type == ERROR_TYPE){
        S->type = ERROR_TYPE;
    } else if(!expression->type.isIntorFloat()){
        S->type = ERROR_TYPE;
        string error_msg = "Condition of while statement must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    } else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return S;
}


Statement* create_iteration_statement_for(Statement* statement1, Statement* statement2, Expression* expression, Statement* statement3){
    IterationStatement* S = new IterationStatement();
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "ITERATION STATEMENT FOR";

    
    if(statement1->type == ERROR_TYPE || statement2->type == ERROR_TYPE || statement3->type == ERROR_TYPE){
        S->type = ERROR_TYPE;
    } else if(expression!=nullptr){
        if(expression->type == ERROR_TYPE){
            S->type = ERROR_TYPE;
        }
    } else if(!expression->type.isIntorFloat()){
        S->type = ERROR_TYPE;
        string error_msg = "Condition of for statement must be an integerorfloat at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    } else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return S;
}

Statement* create_iteration_statement_for_dec(Declaration* declaration, Statement* statement1, Expression* expression, Statement* statement2){
    IterationStatement* S = new IterationStatement();
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "ITERATION STATEMENT FOR DEC";

    if(statement1->type == ERROR_TYPE || statement2->type == ERROR_TYPE){
        S->type = ERROR_TYPE;
    } else if(expression!=nullptr){
        if(expression->type == ERROR_TYPE){
            S->type = ERROR_TYPE;
        } 
    } else if(!expression->type.isIntorFloat()){
        S->type = ERROR_TYPE;
        string error_msg = "Condition of for statement must be an integerorfloat at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    } else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return S;    
}

Statement* create_iteration_statement_until(Expression* expression, Statement* statement){
    IterationStatement* S = new IterationStatement();
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "ITERATION STATEMENT UNTIL";

    if(expression->type == ERROR_TYPE || statement->type == ERROR_TYPE){
        S->type = ERROR_TYPE;
    } else if(!expression->type.isIntorFloat()){
        S->type = ERROR_TYPE;
        string error_msg = "Condition of until statement must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    } else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return S;
}

// ##############################################################################
// ################################## JUMP STATEMENT ######################################
// ##############################################################################

JumpStatement::JumpStatement() : Statement() {
    name = "JUMP STATEMENT";
}

Statement* create_jump_statement(Terminal* op) {
    JumpStatement* S = new JumpStatement();
    if(op->name == "GOTO"){
        S->name = "JUMP STATEMENT GOTO";
    } else if(op->name == "CONTINUE"){
        S->name = "JUMP STATEMENT CONTINUE";
    } else if(op->name == "BREAK"){
        S->name = "JUMP STATEMENT BREAK";
    } else if(op->name == "RETURN"){
        S->name = "JUMP STATEMENT RETURN";
    }
    S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    return S;
}

Statement* create_jump_statement(Expression* expression) {
    JumpStatement* S = new JumpStatement();
    S->line_no = expression->line_no;
    S->return_type = expression->type;
    S->column_no = expression->column_no;
    S->name = "JUMP STATEMENT RETURN WITH EXPRESSION";
    S->return_type = expression->type;
    if(expression->type == ERROR_TYPE){
        S->type = ERROR_TYPE;
    } else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    return S;
}
