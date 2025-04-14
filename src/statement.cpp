#include<statement.h>
#include<symbol_table.h>
#include<expression.h>
#include<ast.h>
#include<algorithm>
#include "parser.tab.h"
#include<string>
#include<vector>
#include<utility> 

using namespace std;

extern void yyerror(const char* msg);
extern void print_code_vector(vector<TACInstruction*>& v);

unordered_map<string, TACOperand*> labels; // Map to store labels and their corresponding TAC operands
unordered_map<string, unordered_set<TACInstruction*>> labels_list; // Map to store labels and their corresponding goto instructions
vector<TACInstruction*> switch_case; // Map to store switch case labels
vector<TACInstruction*> cases; 

// ##############################################################################
// ################################## STATEMENT ######################################
// ##############################################################################

Statement::Statement() : NonTerminal("STATEMENT") {
    type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    begin_label = new_empty_var(); // Initialize with an empty variable
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

Statement* create_labeled_statement_identifier(Identifier* identifier, Statement* statement) {
    LabeledStatement* L = new LabeledStatement();
    L->statement = statement;
    L->identifier = identifier;
    L->label_type = -1; // No label
    L->name = "Labeled Statement IDENTIFIER";
    L->code = statement->code; // TAC
    L->begin_label = statement->begin_label; //TAC
    L->next_list = statement->next_list; //TAC
    L->continue_list = statement->continue_list; //TAC
    L->break_list = statement->break_list; //TAC
    labels.insert({identifier->value, L->begin_label }); // Add label to the map

    if (statement->type == ERROR_TYPE) {
        L->type = ERROR_TYPE;
        string error_msg = "Invalid statement at line " + to_string(identifier->line_no) + ", column " + to_string(identifier->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return L;
    }
    else {
        L->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        if(labels_list.find(identifier->value) != labels_list.end()) {
            if(L->begin_label->type == TAC_OPERAND_EMPTY) {
                L->begin_label = new_label(); // Create a new label for the statement
            }
            backpatch(labels_list[identifier->value], L->begin_label); // Backpatch the label with the statement's begin label
            labels_list.erase(identifier->value); // Remove the label from the list
        }
    }
    return L;
}

Statement* create_labeled_statement_case(Expression* expression, Statement* statement) {
    LabeledStatement* L = new LabeledStatement();
    L->statement = statement;
    L->expression = expression;
    L->label_type = 0; // Case label
    L->name = "Labeled Statement CASE";
    ConditionalExpression* cond_expr = dynamic_cast<ConditionalExpression*>(expression);
    if (!expression->type.isInt() || !cond_expr->type.is_const_literal) {
        string error_msg = "Case label must be a constant integer or character at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
    }
    else if (expression->type == ERROR_TYPE || statement->type == ERROR_TYPE) {
        L->type = ERROR_TYPE;
        string error_msg = "Invalid statement at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    else {
        L->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_EQ), statement->begin_label, new_empty_var(), expression->result, 2); //TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); //TAC
        backpatch(expression->next_list, i1->label); //TAC
        backpatch(expression->true_list, i1->label); //TAC
        backpatch(expression->false_list, i1->label); //TAC
        L->code = expression->code; //TAC
        for(auto i:expression->jump_false_list){
            L->code.erase(remove(L->code.begin(), L->code.end(), i), L->code.end()); //TAC
        }
        for(auto i:expression->jump_true_list){
            L->code.erase(remove(L->code.begin(), L->code.end(), i), L->code.end()); //TAC
        }   
        for(auto i:expression->jump_next_list){
            L->code.erase(remove(L->code.begin(), L->code.end(), i), L->code.end()); //TAC
        }
        L->code.push_back(i1); //TAC
        L->code.push_back(i2); //TAC
        L->code.insert(L->code.end(), statement->code.begin(), statement->code.end()); //TAC
        L->begin_label = L->code[0]->label; //TAC
        L->next_list = statement->next_list; //TAC
        L->next_list.insert(i2); //TAC
        L->continue_list = statement->continue_list; //TAC
        L->break_list = statement->break_list; //TAC
        if(!cases.empty()){
            (*cases.rbegin())->result = i1->label;
        }
        switch_case.push_back(i1);
        cases.push_back(i2);
    }
    return L;
}

Statement* create_labeled_statement_default(Statement* statement) {
    LabeledStatement* L = new LabeledStatement();
    L->statement = statement;
    L->label_type = 1; // Default label
    L->name = "Labeled Statement DEFAULT";

    if (statement->type == ERROR_TYPE) {
        L->type = ERROR_TYPE;
        string error_msg = "Invalid statement at line " + to_string(statement->line_no) + ", column " + to_string(statement->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    else {
        L->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        L->code.insert(L->code.begin(), statement->code.begin(), statement->code.end()); //TAC
        L->begin_label = statement->begin_label; //TAC
        L->next_list = statement->next_list; //TAC
        L->continue_list = statement->continue_list; //TAC
        L->break_list = statement->break_list; //TAC
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
    C->return_type.push_back(Type(PrimitiveTypes::VOID_T, 0, false));
    return C;
}

Statement* create_compound_statement(DeclarationStatementList* statement)
{
    CompoundStatement* C = new CompoundStatement();
    C->name = "COMPOUND STATEMENT WITH DECALARATION STATEMENT LIST";
    C->return_type.insert(C->return_type.end(), statement->return_type.begin(), statement->return_type.end());
    for(Type t : C->return_type){
        Type rt = C->return_type[0];
        if(!t.is_convertible_to(rt)){
            string error_msg = "Invalid return type at line " + to_string(statement->line_no) + ", column " + to_string(statement->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return C;
        }
    }
    if (statement->type == ERROR_TYPE)
    {
        C->type = ERROR_TYPE;
        return C;
    }
    else
    {
        C->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    }
    C->code = statement->code; //TAC
    C->next_list = statement->next_list; //TAC
    C->begin_label = statement->begin_label; //TAC
    C->continue_list = statement->continue_list; //TAC
    C->break_list = statement->break_list; //TAC

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
    D->declarations.push_back(declaration_list);
    D->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    for (Declaration* d : declaration_list->declaration_list) {
        for (InitDeclarator* id : d->init_declarator_list->init_declarator_list) {
            D->code.insert(D->code.end(), id->code.begin(), id->code.end()); //TAC
        }
    }
    if (!D->code.empty()) {
        D->begin_label = D->code[0]->label; //TAC
    }
    return D;
}

DeclarationStatementList* create_declaration_statement_list(StatementList* statement_list) {
    DeclarationStatementList* D = new DeclarationStatementList();
    D->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    D->return_type = statement_list->return_type;
    D->code = statement_list->code; //TAC
    D->begin_label = statement_list->begin_label; //TAC
    D->next_list = statement_list->next_list; //TAC
    D->continue_list = statement_list->continue_list; //TAC
    D->break_list = statement_list->break_list; //TAC
    return D;
}

DeclarationStatementList* create_declaration_statement_list(DeclarationStatementList* declaration_statement_list, StatementList* statement_list) {
    declaration_statement_list->statements.push_back(statement_list);
    declaration_statement_list->return_type.insert(declaration_statement_list->return_type.end(), statement_list->return_type.begin(), statement_list->return_type.end());
    if (statement_list->type == ERROR_TYPE) {
        declaration_statement_list->type = ERROR_TYPE;
        return declaration_statement_list;
    }
    else {
        declaration_statement_list->code.insert(declaration_statement_list->code.end(), statement_list->code.begin(), statement_list->code.end()); //TAC
        backpatch(declaration_statement_list->next_list, statement_list->begin_label); //TAC
        declaration_statement_list->next_list = merge_lists(statement_list->next_list,declaration_statement_list->next_list); //TAC
        declaration_statement_list->continue_list = merge_lists(statement_list->continue_list, declaration_statement_list->continue_list); //TAC
        declaration_statement_list->break_list = merge_lists(statement_list->break_list, declaration_statement_list->break_list); //TAC

    }
    if(declaration_statement_list->begin_label->type == TAC_OPERAND_EMPTY) {
        declaration_statement_list->begin_label = statement_list->begin_label; //TAC
    }
    return declaration_statement_list;
}

DeclarationStatementList* create_declaration_statement_list(DeclarationStatementList* declaration_statement_list, DeclarationList* declaration_list) {
    declaration_statement_list->declarations.push_back(declaration_list);
    vector<TACInstruction*> dec_code;
    for (Declaration* d : declaration_list->declaration_list) {
        for (InitDeclarator* id : d->init_declarator_list->init_declarator_list) {
            dec_code.insert(dec_code.end(), id->code.begin(), id->code.end()); //TAC
        }
    }
    declaration_statement_list->code.insert(declaration_statement_list->code.end(), dec_code.begin(), dec_code.end()); //TAC
    if(!dec_code.empty()) {
        if(declaration_statement_list->begin_label->type == TAC_OPERAND_EMPTY) {
            declaration_statement_list->begin_label = dec_code[0]->label; //TAC
        }
        backpatch(declaration_statement_list->next_list, dec_code[0]->label); //TAC
    }
    return declaration_statement_list;
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
    if (statement->type == ERROR_TYPE) {
        S->type = ERROR_TYPE;
        return S;
    }
    else {
        S->begin_label = statement->begin_label;
        S->code = statement->code; //TAC
        S->next_list = statement->next_list; //TAC
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        S->continue_list = statement->continue_list; //TAC
        S->break_list = statement->break_list; //TAC
    }
    return S;
}

StatementList* create_statement_list(StatementList* statement_list, Statement* statement) {
    statement_list->statements.push_back(statement);
    statement_list->return_type.insert(statement_list->return_type.end(), statement->return_type.begin(), statement->return_type.end());
    if (statement->type == ERROR_TYPE) {
        statement_list->type = ERROR_TYPE;
        return statement_list;
    }
    statement_list->code.insert(statement_list->code.end(), statement->code.begin(), statement->code.end()); //TAC
    backpatch(statement_list->next_list, statement->begin_label); //TAC
    statement_list->next_list = merge_lists(statement->next_list, statement_list->next_list); //TAC
    statement_list->continue_list = merge_lists(statement_list->continue_list, statement->continue_list); //TAC
    statement_list->break_list = merge_lists(statement_list->break_list, statement->break_list); //TAC
    if(statement_list->begin_label->type == TAC_OPERAND_EMPTY) {
        statement_list->begin_label = statement->begin_label; //TAC
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

Statement* create_expression_statement() {
    ExpressionStatement* S = new ExpressionStatement();
    S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    return S;
}

Statement* create_expression_statement(Expression* x) {
    ExpressionStatement* S = new ExpressionStatement();
    S->expression = x;
    S->line_no = x->line_no;
    S->column_no = x->column_no;

    if (x->type == ERROR_TYPE) {
        S->type = ERROR_TYPE;
    }
    else {
        S->code = x->code; //TAC
        S->jump_code = x->jump_code; //TAC
        for(auto i:x->jump_true_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        for(auto i:x->jump_false_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        for(auto i:x->jump_next_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        for(auto i:x->true_list){
            S->jump_code.erase(remove(S->jump_code.begin(), S->jump_code.end(), i), S->jump_code.end()); //TAC
        }
        for(auto i:x->false_list){
            S->jump_code.erase(remove(S->jump_code.begin(), S->jump_code.end(), i), S->jump_code.end()); //TAC
        }

        S->next_list = merge_lists(x->next_list, x->jump_next_list); //TAC
        if (!x->code.empty()) {
            S->begin_label = x->code[0]->label; //TAC
        }
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        if(!x->code.empty()) S->begin_label = x->code[0]->label; //TAC
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

// expression ki next->list bhi backpatch krni hai
Statement* create_selection_statement_if(Expression* expression, Statement* statement) {
    SelectionStatement* S = new SelectionStatement();
    S->expression = expression;
    S->statement = statement;
    S->selection_type = 0; // If statement
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "SELECTION STATEMENT IF";

    if (expression->type == ERROR_TYPE || statement->type == ERROR_TYPE) {
        S->type = ERROR_TYPE;
        return S;
    }
    else if (!expression->type.isInt()) {
        S->type = ERROR_TYPE;
        string error_msg = "Condition of if statement must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return S;
    }
    else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        S->code = expression->jump_code; //TAC
        for(auto i:expression->true_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        for(auto i:expression->false_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        S->code.insert(S->code.end(), statement->code.begin(), statement->code.end()); //TAC
        backpatch(expression->jump_true_list, statement->begin_label); //TAC
        backpatch(expression->jump_next_list, statement->begin_label); //TAC
        S->next_list = merge_lists(statement->next_list, expression->jump_false_list); //TAC
        S->next_list = merge_lists(S->next_list, expression->jump_next_list); //TAC
        S->begin_label = S->code[0]->label; //TAC
        S->continue_list = statement->continue_list; //TAC
        S->break_list = statement->break_list; //TAC
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

    if (expression->type == ERROR_TYPE || statement->type == ERROR_TYPE || else_statement->type == ERROR_TYPE) {
        S->type = ERROR_TYPE;
        return S;
    }
    else if (!expression->type.isInt()) {
        S->type = ERROR_TYPE;
        string error_msg = "Condition of if statement must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return S;
    }
    else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        S->code = expression->jump_code; //TAC
        for(auto i:expression->true_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        for(auto i:expression->false_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        S->code.insert(S->code.end(), statement->code.begin(), statement->code.end()); //TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); //TAC
        S->code.push_back(i1); //TAC
        S->code.insert(S->code.end(), else_statement->code.begin(), else_statement->code.end()); //TAC
        backpatch(expression->true_list, statement->begin_label); //TAC
        backpatch(expression->jump_true_list, statement->begin_label); //TAC
        backpatch(expression->false_list, else_statement->code[0]->label); //TAC
        backpatch(expression->jump_false_list, else_statement->code[0]->label); //TAC
        statement->next_list = merge_lists(statement->next_list, expression->jump_true_list); //TAC
        statement->next_list = merge_lists(statement->next_list, expression->jump_next_list); //TAC
        backpatch(statement->next_list, i1->label); //TAC
        else_statement->next_list = merge_lists(else_statement->next_list, expression->jump_false_list); //TAC
        S->next_list = else_statement->next_list ; //TAC
        S->next_list.insert(i1); //TAC
        S->begin_label = expression->jump_code[0]->label; //TAC
        S->continue_list = merge_lists(statement->continue_list,else_statement->continue_list); //TAC
        S->break_list = merge_lists(statement->break_list,else_statement->break_list); //TAC
    }
    return S;
}

//TODO: switch statement ki case label ki list ko backpatch krna hai
Statement* create_selection_statement_switch(Expression* expression, Statement* statement) {
    SelectionStatement* S = new SelectionStatement();
    S->expression = expression;
    S->statement = statement;
    S->selection_type = 2; // Switch statement
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "SELECTION STATEMENT SWITCH";

    if (expression->type == ERROR_TYPE || statement->type == ERROR_TYPE) {
        S->type = ERROR_TYPE;
        return S;
    }
    else if (!expression->type.isInt()) {
        S->type = ERROR_TYPE;
        string error_msg = "Condition of switch statement must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return S;
    }
    else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        if(!switch_case.empty()){
            S->code.insert(S->code.end(), expression->code.begin(), expression->code.end());
            for(auto i:expression->jump_next_list){
                S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
            }
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP),(*switch_case.begin())->label , new_empty_var(), new_empty_var(),1);
            backpatch(expression->next_list, i1->label); //TAC
            for(auto instr: switch_case){
                instr->arg1 = expression->result;
            }
            S->code.push_back(i1);
            S->code.insert(S->code.end(), statement->code.begin(), statement->code.end());
            S->begin_label = S->code[0]->label;
            S->next_list = statement->next_list;
            S->break_list = statement->break_list;
            S->continue_list = statement->continue_list;
        }
        else {
            S->code.insert(S->code.end(), expression->code.begin(), expression->code.end()); //TAC
            for(auto i:expression->jump_next_list){
                S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
            }
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP),new_empty_var() , new_empty_var(), new_empty_var(),1);
            S->code.push_back(i1); //TAC
            backpatch(expression->next_list, i1->label); //TAC
            S->next_list.insert(i1); //TAC
            S->begin_label = S->code[0]->label; //TAC
        }
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


    if (expression->type == ERROR_TYPE || statement->type == ERROR_TYPE) {
        S->type = ERROR_TYPE;
        return S;
    }
    else if (!expression->type.isIntorFloat()) {
        S->type = ERROR_TYPE;
        string error_msg = "Condition of while statement must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return S;
    }
    else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        S->code = expression->jump_code; //TAC
        for(auto i:expression->true_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        for(auto i:expression->false_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        backpatch(expression->jump_true_list, statement->begin_label); //TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); //TAC
        i1->result = expression->jump_code[0]->label; //TAC
        S->code.insert(S->code.end(), statement->code.begin(), statement->code.end()); //TAC
        S->code.push_back(i1); //TAC
        backpatch(statement->next_list, i1->label); //TAC
        S->next_list = merge_lists(expression->jump_false_list, expression->jump_true_list); //TAC
        S->begin_label = S->code[0]->label; //TAC
        backpatch(statement->continue_list, S->code[0]->label); //TAC
        S->next_list = merge_lists(statement->break_list, S->next_list); //TAC
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

    if (expression->type == ERROR_TYPE || statement->type == ERROR_TYPE) {
        S->type = ERROR_TYPE;
    }
    else if (!expression->type.isIntorFloat()) {
        S->type = ERROR_TYPE;
        string error_msg = "Condition of while statement must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        S->code = statement->code; //TAC
        S->code.insert(S->code.end(), expression->jump_code.begin(), expression->jump_code.end()); //TAC
        for(auto i:expression->true_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        for(auto i:expression->false_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        backpatch(statement->next_list, expression->jump_code[0]->label); //TAC
        backpatch(expression->jump_true_list, statement->begin_label); //TAC
        S->next_list = merge_lists(expression->jump_false_list,expression->jump_true_list); //TAC
        S->begin_label = S->code[0]->label; //TAC
        backpatch(statement->continue_list, expression->jump_code[0]->label); //TAC
        S->next_list = merge_lists(statement->break_list, S->next_list); //TAC
    }
    return S;
}

Statement* create_iteration_statement_for(Statement* statement1, Statement* statement2, Expression* expression, Statement* statement3) {
    IterationStatement* S = new IterationStatement();
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "ITERATION STATEMENT FOR";
    if (statement1->type == ERROR_TYPE || statement2->type == ERROR_TYPE || statement3->type == ERROR_TYPE) {
        S->type = ERROR_TYPE;
        return S;
    }
    else if (expression != nullptr && expression->type == ERROR_TYPE) {
            S->type = ERROR_TYPE;
            return S;
    }
    else if(statement1 == nullptr || statement2 == nullptr || statement3 == nullptr || expression == nullptr) {
        S->type = ERROR_TYPE;
        string error_msg = "Invalid statement at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return S;
    }
    else if (!expression->type.isIntorFloat()) {
        S->type = ERROR_TYPE;
        string error_msg = "Condition of for statement must be an integerorfloat at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return S;
    }
    else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        S->code = statement1->code; //TAC
        ExpressionStatement* exp = dynamic_cast<ExpressionStatement*>(statement2);
        S->code.insert(S->code.end(), exp->jump_code.begin(), exp->jump_code.end()); //TAC
        backpatch(statement1->next_list, exp->jump_code[0]->label); //TAC
        backpatch(exp->expression->jump_true_list, statement3->begin_label); //TAC
        S->code.insert(S->code.end(), statement3->code.begin(), statement3->code.end()); //TAC
        if (expression != nullptr) {
            S->code.insert(S->code.end(), expression->code.begin(), expression->code.end()); //TAC
            for(auto i:expression->jump_next_list){
                S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
            }
            if (!expression->code.empty()) {
                backpatch(statement3->next_list, expression->code[0]->label); //TAC
                backpatch(statement3->continue_list, expression->code[0]->label); //TAC
            }
        }
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); //TAC
        i1->result = statement2->begin_label; //TAC
        backpatch(statement3->next_list, i1->label); //TAC
        backpatch(statement3->continue_list, i1->label); //TAC
        S->code.push_back(i1); //TAC
        S->next_list = merge_lists(exp->expression->jump_false_list, exp->expression->jump_true_list); //TAC
        S->begin_label = S->code[0]->label; //TAC
        S->next_list = merge_lists(statement3->break_list, S->next_list); //TAC
    }
    return S;
}

Statement* create_iteration_statement_for_dec(ForIterationStruct* fis, Expression* expression, Statement* statement2) {
    Declaration* declaration = fis->declaration;
    Statement* statement1 = fis->statement1;
    IterationStatement* S = new IterationStatement();
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "ITERATION STATEMENT FOR DEC";

    if (statement1->type == ERROR_TYPE || statement2->type == ERROR_TYPE) {
        S->type = ERROR_TYPE;
        symbolTable.set_error();
        return S;
    }
    else if (expression != nullptr && expression->type == ERROR_TYPE) {
        S->type = ERROR_TYPE;
        symbolTable.set_error();
        return S;
        
    }
    else if(statement1 == nullptr || statement2 == nullptr || expression == nullptr) {
        S->type = ERROR_TYPE;
        string error_msg = "Invalid statement at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return S;
    }
    else if (!expression->type.isIntorFloat()) {
        S->type = ERROR_TYPE;
        string error_msg = "Condition of for statement must be an integerorfloat at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return S;
    }
    else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        for (auto id : declaration->init_declarator_list->init_declarator_list) {
            if (id->initializer != nullptr) S->code.insert(S->code.end(), id->code.begin(), id->code.end()); //TAC
        }
        ExpressionStatement* exp = dynamic_cast<ExpressionStatement*>(statement1);
        S->code.insert(S->code.end(), exp->jump_code.begin(), exp->jump_code.end()); //TAC
        backpatch(exp->expression->jump_true_list, statement2->begin_label); //TAC
        S->code.insert(S->code.end(), statement2->code.begin(), statement2->code.end()); //TAC
        if (expression != nullptr) {
            S->code.insert(S->code.end(), expression->code.begin(), expression->code.end()); //TAC
            for(auto i:expression->jump_next_list){
                S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
            }
            if (!expression->code.empty()) {
                backpatch(statement2->next_list, expression->code[0]->label); //TAC
                backpatch(statement2->continue_list, expression->code[0]->label); //TAC
            }
        }
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); //TAC
        i1->result = statement1->begin_label; //TAC
        backpatch(statement2->next_list, i1->label); //TAC
        backpatch(statement2->continue_list, i1->label); //TAC
        S->code.push_back(i1); //TAC
        S->next_list = merge_lists(exp->expression->jump_false_list, exp->expression->jump_true_list); //TAC
        S->begin_label = S->code[0]->label; //TAC
        S->next_list = merge_lists(statement2->break_list, S->next_list); //TAC
    }
    return S;
}

Statement* create_iteration_statement_until(Expression* expression, Statement* statement) {
    IterationStatement* S = new IterationStatement();
    S->line_no = expression->line_no;
    S->column_no = expression->column_no;
    S->name = "ITERATION STATEMENT UNTIL";

    if (expression->type == ERROR_TYPE || statement->type == ERROR_TYPE) {
        S->type = ERROR_TYPE;
        return S;
    }
    else if (!expression->type.isIntorFloat()) {
        S->type = ERROR_TYPE;
        string error_msg = "Condition of until statement must be an integer at line " + to_string(expression->line_no) + ", column " + to_string(expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return S;
    }
    else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        S->code = expression->jump_code; //TAC
        for(auto i:expression->true_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        for(auto i:expression->false_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        S->code.insert(S->code.end(), statement->code.begin(), statement->code.end()); //TAC
        backpatch(expression->jump_false_list, statement->begin_label); //TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); //TAC
        i1->result = expression->jump_code[0]->label; //TAC
        backpatch(statement->next_list, i1->label); //TAC
        S->code.push_back(i1); //TAC
        S->next_list = merge_lists(expression->jump_true_list, expression->jump_false_list); //TAC
        S->begin_label = S->code[0]->label; //TAC
        backpatch(statement->continue_list, S->begin_label); //TAC
        S->next_list = merge_lists(statement->break_list, S->next_list); //TAC
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
    if (op->name == "CONTINUE") {
        S->name = "JUMP STATEMENT CONTINUE";
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); //TAC
        S->continue_list.insert(i1);
        S->code.push_back(i1); //TAC
        S->begin_label = i1->label;
    }
    else if (op->name == "BREAK") {
        S->name = "JUMP STATEMENT BREAK";
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); //TAC
        S->break_list.insert(i1);
        S->code.push_back(i1); //TAC
        S->begin_label = i1->label; //TAC
    }
    else if (op->name == "RETURN") {
        S->return_type.push_back(Type(PrimitiveTypes::VOID_T, 0, false));
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_RETURN), new_empty_var(), new_empty_var(), new_empty_var(), 0); //TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP),new_empty_var(), new_empty_var(), new_empty_var(),1);
        S->code.push_back(i1); //TAC
        S->code.push_back(i2);
        S->break_list.insert(i2);
        S->begin_label = i1->label; //TAC
        S->name = "JUMP STATEMENT RETURN";
    }
    S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    return S;
}

Statement* create_jump_statement_goto(Identifier* identifier) {
    JumpStatement* S = new JumpStatement();
    S->line_no = identifier->line_no;
    S->column_no = identifier->column_no;
    S->name = "JUMP STATEMENT GOTO";
    S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
    TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), new_empty_var(), new_empty_var(), new_empty_var(), 1); //TAC
    if(labels.find(identifier->value) != labels.end()) {
        i1->result = labels[identifier->value]; //TAC
    }
    else {
        labels_list[identifier->value].insert(i1); //TAC
    }
    S->code.push_back(i1); //TAC
    S->begin_label = i1->label; //TAC
    return S;
}

Statement* create_jump_statement(Expression* expression) {
    JumpStatement* S = new JumpStatement();
    S->line_no = expression->line_no;
    S->return_type.push_back(expression->type);
    S->column_no = expression->column_no;
    S->name = "JUMP STATEMENT RETURN WITH EXPRESSION";
    if (expression->type == ERROR_TYPE) {
        S->type = ERROR_TYPE;
    }
    else {
        S->type = Type(PrimitiveTypes::VOID_STATEMENT_T, 0, false);
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_RETURN), expression->result,new_empty_var(), new_empty_var(), 0); //TAC
        S->code.insert(S->code.end(), expression->code.begin(), expression->code.end()); //TAC
        for(auto i:expression->jump_next_list){
            S->code.erase(remove(S->code.begin(), S->code.end(), i), S->code.end()); //TAC
        }
        S->code.push_back(i1); //TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP),new_empty_var(), new_empty_var(), new_empty_var(),1);
        S->begin_label = S->code[0]->label; //TAC
        S->break_list.insert(i2);
        S->code.push_back(i2);
    }
    return S;
}
