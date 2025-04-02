#include "symbol_table.h"
#include "ast.h"
#include "expression.h"
#include "tac.h"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>
// #include <3ac.h>
// #include <codegen.h>
// #include <statement.h>
// #include <y.tab.h>
#include <vector>
#include <utility>
#include <string>
#include <iterator>
#include <string.h>
extern void yyerror(const char *msg);

using namespace std;
Type ERROR_TYPE;

// ##############################################################################
// ################################## EXPRESSION ######################################
// ##############################################################################

Expression :: Expression() : NonTerminal("EXPRESSION"){
    operand_cnt = 0;
    result = TACOperand(TAC_OPERAND_EMPTY, "");
}

// ##############################################################################
// ################################## PRIMARY EXPRESSION ######################################
// ##############################################################################

PrimaryExpression :: PrimaryExpression() : Expression() {
    name = "PRIMARY EXPRESSION";
    identifier = nullptr;
    constant = nullptr;
    string_literal = nullptr;
}

Expression* create_primary_expression(Identifier* i){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION IDENTIFIER";
    P->line_no = i->line_no;
    P->column_no = i->column_no;
    P->identifier = i;
    Symbol* sym = symbolTable.getSymbol(i->value);
    if(sym) P->type = sym->type;
    else {
        P->type = ERROR_TYPE;
        string error_msg = "Undeclared Symbol " + i->value + " at line " + to_string(i->line_no) + ", column " + to_string(i->column_no);
		yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    P->result = new_identifier(i->value); //TAC
    return P;
}

Expression* create_primary_expression(Constant* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION CONSTANT";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->constant = x;
    P->type = x->get_constant_type();
    P->type.is_const_literal = true;
    P->result = new_constant(x->value); // TAC
    return P;
}

Expression* create_primary_expression(StringLiteral* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION STRING LITERAL";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->string_literal = x;
    P->type = Type(CHAR_T, 1, true);
    P->result = new_constant(x->value); // TAC
    return P;
}

Expression* create_primary_expression(Expression* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->type = x->type;
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->result = x->result; // TAC
    return x;
}

// ##############################################################################
// ################################## ARGUMENT EXPRESSION LIST ######################################
// ##############################################################################

ArgumentExpressionList :: ArgumentExpressionList() : Expression() {
    name = "ARGUMENT EXPRESSION LIST";
    type = ERROR_TYPE;
};

ArgumentExpressionList* create_argument_expression_list(Expression* x){
    ArgumentExpressionList* P = new ArgumentExpressionList();
    P->arguments.push_back(x);
    P->line_no = x->line_no;   
    P->column_no = x->column_no;
    P->type = x->type; // if argument expression list does not have an erronous expression, set type to the type of the first expression. Else set it as ERROR_TYPE.
    emit(TACOperator(TAC_OPERATOR_PARAM), TACOperand(TAC_OPERAND_EMPTY, ""), x->result, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
    return P;
}

ArgumentExpressionList* create_argument_expression_list(ArgumentExpressionList* args_expr_list, Expression* x){
    args_expr_list->arguments.push_back(x);
    if(x->type.is_error()){
        args_expr_list->type = ERROR_TYPE;
    }
    return args_expr_list;
}

// ##############################################################################
// ################################## POSTFIX EXPRESSION ######################################
// ##############################################################################

PostfixExpression :: PostfixExpression() : Expression() {
    name = "POSTFIX EXPRESSION";
    primary_expression = nullptr;
    base_expression = nullptr;
    index_expression = nullptr;
    argument_expression_list = nullptr;
    op = nullptr;
    member_name = nullptr;
}

Expression* create_postfix_expression(Expression* x){
    PostfixExpression* P = new PostfixExpression();
    P->primary_expression = dynamic_cast<PrimaryExpression*>(x);
    P->type = x->type;
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->result = x->result; // TAC
    return P;
}

Expression* create_postfix_expression(Expression* x, Terminal* op){
    PostfixExpression* P = new PostfixExpression();
    P->base_expression = dynamic_cast<PostfixExpression *>(x);
    P->primary_expression = P->base_expression->primary_expression;
    P->op = op;
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    if(op->name == "INC_OP") P->name = "POSTFIX EXPRESSION INC OP";
    else P->name = "POSTFIX EXPRESSION DEC OP";

    if(x->type.is_error()){
        P->type = ERROR_TYPE;
        return P;
    }

    if(x->type.is_const_variable || x->type.is_array || x->type.is_function || x->type.is_defined_type){
        P->type = ERROR_TYPE;
        string error_msg = "Invalid operator " + op->value + " at line  " + to_string(op->line_no) + ", column " + to_string(op->column_no);
		yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    else if(x->type.isInt()){
        P->type = x->type;
        P->result = new_temp_var(); // TAC
        emit(TACOperator(TAC_OPERATOR_NOP), P->result, x->result, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
        emit(TACOperator(op->name == "INC_OP" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), x->result, x->result, new_constant("1")); // TAC
    }
    else if(x->type.isFloat()){
        P->type = x->type;
        P->result = x->result; // TAC
        emit(TACOperator(TAC_OPERATOR_NOP), P->result, x->result, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
        emit(TACOperator(op->name == "INC_OP" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), x->result, x->result, new_constant("1")); // TAC
    }
    else if(x->type.is_pointer){
        P->type = x->type;
        P->result = x->result; // TAC
        emit(TACOperator(op->name == "INC_OP" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), P->result, x->result, new_constant(to_string(primitive_type_size[x->type.typeIndex]))); // TAC
        emit(TACOperator(TAC_OPERATOR_NOP), x->result, P->result, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
    }
    else{
        P->type = ERROR_TYPE;
        string error_msg = "Invalid operator " + op->value + " at line  " + to_string(op->line_no) + ", column " + to_string(op->column_no);
		yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    return P;
}

Expression* create_postfix_expression(Expression* x, Terminal* op, Identifier* id){
    PostfixExpression* P = new PostfixExpression();
    P->base_expression = dynamic_cast<PostfixExpression* >(x);
    P->primary_expression = P->base_expression->primary_expression;
    P->op = op;
    P->member_name = id;
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    
    if(x->type.is_error()){
        P->type = ERROR_TYPE;
        return P;
    }

    if(op->name == "PTR_OP") P->name = "POSTFIX EXPRESSION PTR_OP";
    else P->name = "POSTFIX EXPRESSION DOT";

    if(op->name == "PTR_OP"){
        if(!(x->type.ptr_level == 1) && !(x->type.is_defined_type)){ 
            P->type = ERROR_TYPE;
            string error_msg = "Operator '->' applied to non-pointer-to-struct/union/class object at line " + to_string(op->line_no) + ", column " + to_string(op->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return P;
        }
    }
    else if (op->name == "DOT") {
        if (!(x->type.ptr_level == 0) && !(x->type.is_defined_type)) {
            P->type = ERROR_TYPE;
            string error_msg = "Operator '.' applied to non-struct/union/class object at line " + to_string(op->line_no) + ", column " + to_string(op->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return P;
        }
    }
    else {
        if(!symbolTable.lookup_defined_type(x->type.defined_type_name)){
            P->type = ERROR_TYPE;
            string error_msg = "Defined_Type not found in Symbol Table " + to_string(op->line_no) + ", column " + to_string(op->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return P;
        }
        else if(!symbolTable.check_member_variable(x->type.defined_type_name, id->value)) {
            P->type = ERROR_TYPE;
            string error_msg = "Defined_Type does not have member_variable with name " + id->value + to_string(op->line_no) + ", column " + to_string(op->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return P;
        }
        else {
            P->type = symbolTable.get_type_of_member_variable(x->type.defined_type_name, id->value);
            TypeDefinition* td = symbolTable.get_defined_type(x->type.defined_type_name).type_definition;
            Symbol* member = td->type_symbol_table.getSymbol(id->value);
            if(op->name == "DOT"){
                TACOperand t1 = new_temp_var(); // TAC
                TACOperand t2 = new_temp_var(); // TAC
                TACOperand t3 = new_temp_var(); // TAC
                P->result = new_temp_var(); // TAC
                emit(TACOperator(TAC_OPERATOR_ADDR_OF), t1, x->result, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
                emit(TACOperator(TAC_OPERATOR_ADD), t2, t1, new_constant(to_string(member->offset))); // TAC
                emit(TACOperator(TAC_OPERATOR_DEREF), t3, t2, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
                emit(TACOperator(TAC_OPERATOR_NOP), P->result, t3, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
            }
            else if(op->name == "PTR_OP"){
                TACOperand t1 = new_temp_var(); // TAC
                TACOperand t2 = new_temp_var(); // TAC
                P->result = new_temp_var(); // TAC
                emit(TACOperator(TAC_OPERATOR_ADD), t1, x->result, new_constant(to_string(member->offset))); // TAC
                emit(TACOperator(TAC_OPERATOR_DEREF), t2, t1, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
                emit(TACOperator(TAC_OPERATOR_NOP), P->result, t2, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
            }
        }
    }
    return P;
}

Expression* create_postfix_expression(Expression* x, Expression* index_expression){
    PostfixExpression* P = new PostfixExpression();
    P->name = "POSTFIX EXPRESSION ARRAY ACCESS";
    P->base_expression = dynamic_cast<PostfixExpression*>(x);
    P->primary_expression = P->base_expression->primary_expression;
    P->index_expression = index_expression;
    P->line_no = x->line_no;
    P->column_no = x->column_no;

    if(x->type.is_error()){
        P->type = ERROR_TYPE;
        return P;
    }

    // Type checking
    if (!x->type.is_pointer && !x->type.is_array) {
        P->type = ERROR_TYPE;
        string error_msg = "Cannot index non-array/pointer at line " + to_string(index_expression->line_no) + ", column " + to_string(index_expression->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    } else if (!index_expression->type.isInt()) {
        P->type = ERROR_TYPE;
        string err = "Array index must be an integer at line " + to_string(index_expression->line_no) + ", column " + to_string(index_expression->column_no);
        yyerror(err.c_str());
        symbolTable.set_error();
        return P;
    } 
    else {
        if(!symbolTable.lookup(P->primary_expression->identifier->value)){
            P->type = ERROR_TYPE;
            string err = "Array not declared at line " + to_string(index_expression->line_no) + ", column " + to_string(index_expression->column_no);
            yyerror(err.c_str());
            symbolTable.set_error();
            return P;
        }
        else {
            P->type = x->type;
            if(x->type.is_array){
                P->type.array_dim--;
                P->type.array_dims.erase(P->type.array_dims.begin());
            }
            P->type.ptr_level--;
            if(P->type.array_dim == 0) P->type.is_array = false;
            if(P->type.ptr_level == 0) P->type.is_pointer = false;
        }
    }
    TACOperand t1 = new_temp_var(); // TAC
    TACOperand t2 = new_temp_var(); // TAC
    TACOperand t3 = new_temp_var(); // TAC
    P->result = new_temp_var(); // TAC
    emit(TACOperator(TAC_OPERATOR_MUL), t1, index_expression->result, new_constant(to_string(primitive_type_size[x->type.typeIndex]))); // TAC
    emit(TACOperator(TAC_OPERATOR_ADD), t2, x->result, t1); // TAC
    emit(TACOperator(TAC_OPERATOR_DEREF), t3, t2, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
    emit(TACOperator(TAC_OPERATOR_NOP), P->result, t3, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
    return P;
}

Expression* create_postfix_expression_func(Expression* x, ArgumentExpressionList* argument_expression_list) {
    // check if exist in symbol table and arguements no. and types
    PostfixExpression* P = new PostfixExpression();
    P->name = "POSTFIX EXPRESSION FUNCTION CALL";
    P->base_expression = dynamic_cast<PostfixExpression*>(x);
    P->primary_expression = P->base_expression->primary_expression;
    P->argument_expression_list = argument_expression_list;
    P->line_no = x->line_no;
    P->column_no = x->column_no;

    if(x->type.is_error()){
        P->type = ERROR_TYPE;
        return P;
    }
    
    vector<Type> arguments;
    if(argument_expression_list != nullptr){ // for function call with no args
        for(int i=0;i<argument_expression_list->arguments.size();i++){
            arguments.push_back(argument_expression_list->arguments[i]->type);
        }
    }
    // Type check: should be a function or function pointer
    if (!x->type.is_function && !(x->type.is_function && x->type.is_pointer)) {
        P->type = ERROR_TYPE;
        string error_msg = "Called object is not a function at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);;
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    } 
    else{
        if(!symbolTable.lookup_function(P->primary_expression->identifier->value, arguments)){
            P->type = ERROR_TYPE;
            string error_msg = "No matching function declaration found " + to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return P;
        }
        else{
            FunctionDefinition* fd = symbolTable.getFunction(P->primary_expression->identifier->value, arguments)->function_definition;
            if(fd == nullptr){
                P->type = ERROR_TYPE;
                string error_msg = "Function " + P->primary_expression->identifier->value+ " is declared but not defined at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
                yyerror(error_msg.c_str());
                symbolTable.set_error();
                return P;
            }
            else {
                P->type = x->type;
                P->type.is_function = false;
                P->type.num_args = 0;
                P->type.arg_types.clear();
                if(x->type.typeIndex == PrimitiveTypes :: VOID_T) P->result = TACOperand(TAC_OPERAND_EMPTY, ""); // TAC
                else P->result = new_temp_var(); // TAC
                emit(TACOperator(TAC_OPERATOR_CALL), P->result, x->result, new_constant(to_string(arguments.size()))); // TAC
            }
        }
    }
    return P;
}

// ##############################################################################
// ################################## UNARY EXPRESSION ######################################
// ##############################################################################

UnaryExpression :: UnaryExpression() : Expression() {
    name = "UNARY EXPRESSION";
    postfix_expression = nullptr;
    op = nullptr;
    base_expression = nullptr;
    cast_expression = nullptr;
    type_name = nullptr;
}

Expression* create_unary_expression(Expression* x){
    UnaryExpression* U = new UnaryExpression();
    U->postfix_expression = dynamic_cast<PostfixExpression *>(x);
    U->line_no = x->line_no;
    U->column_no = x->column_no;
    U->type = x->type;
    U->result = x->result; // TAC
    return U;
}

Expression* create_unary_expression(Expression* x, Terminal* op){
    UnaryExpression *U = new UnaryExpression();
    U->base_expression = dynamic_cast<UnaryExpression*> (x);
    U->op = op;
    U->postfix_expression = U->base_expression->postfix_expression;
    U->line_no = x->line_no;
    U->column_no = x->column_no;

    if(x->type.is_error()){
        U->type = ERROR_TYPE;
        return U;
    }

    if(op->name == "INC_OP") U->name = "UNARY EXPRESSION INC_OP";
    else if(op->name == "DEC_OP") U->name = "UNARY EXPRESSION DEC_OP";
    else U->name = "UNARY EXPRESSION SIZEOF";  
    
    if (op->name == "INC_OP" || op->name == "DEC_OP")
    {
        if (x->type.is_const_variable)
        {   
            U->type = ERROR_TYPE;
            string error_msg = "Invalid operator with constant type " + to_string(x->line_no) + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return U;
        }

        if (x->type.is_pointer || x->type.isIntorFloat())
        {
            U->type = x->type;
            U->result = new_temp_var(); // TAC
            if(x->type.is_pointer){
                emit(TACOperator(op->name == "INC_OP" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), U->result, x->result, new_constant(to_string(primitive_type_size[x->type.typeIndex]))); // TAC
                emit(TACOperator(TAC_OPERATOR_NOP), x->result, U->result, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
            }
            else{
                emit(TACOperator(op->name == "INC_OP" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), U->result, x->result, new_constant("1")); // TAC
                emit(TACOperator(TAC_OPERATOR_NOP), x->result, U->result, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
            }
        } 
        else
        {   
            U->type = ERROR_TYPE;
            string error_msg = "Operator with invalid type " + to_string(x->line_no) + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return U;
        }

    }
    else if (op->name == "SIZEOF")
    {
        U->type = Type(PrimitiveTypes::INT_T, 0, true);
    }
    return U;
}

Expression *create_unary_expression_cast(Expression* x, Terminal* op)
{
    UnaryExpression *U = new UnaryExpression();
    U->op = op;
    U->cast_expression = dynamic_cast<CastExpression*> (x);
    //U->postfix_expression = U->base_expression->postfix_expression;
    U->line_no = x->line_no;
    U->column_no = x->column_no;
    if(x->type.is_error()){
        U->type = ERROR_TYPE;
        return U;
    }

    if(op->name == "BITWISE_AND") U->name = "UNARY EXPRESSION BITWISE_AND";
    else if(op->name == "MULTIPLY") U->name = "UNARY EXPRESSION MULTIPLY";
    else if(op->name == "PLUS") U->name = "UNARY EXPRESSION PLUS";
    else if(op->name == "MINUS") U->name = "UNARY EXPRESSION MINUS";
    else if(op->name == "NOT") U->name = "UNARY EXPRESSION NOT";
    else U->name = "UNARY EXPRESSION BITWISE_NOT";

    if (op->name == "BITWISE_AND")
    {
        if (x->type.is_const_literal) {
            U->type = ERROR_TYPE;
            string err = "Cannot take address of a constant literal at line " +
                         to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(err.c_str());
            symbolTable.set_error();
            return U;
        }
        U->type = x->type;
        U->type.ptr_level++;
        U->type.is_pointer = true;
        U->result = new_temp_var(); // TAC
        emit(TAC_OPERATOR_ADDR_OF, U->result, x->result, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
    }
    else if (op->name == "MULTIPLY")
    {
        if (x->type.ptr_level <= 0)
        {
            U->type = ERROR_TYPE;
            string error_msg = "Cannot dereference a non-pointer at line " + to_string(x->line_no) + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return U;
        }
        U->type = x->type;
        U->type.ptr_level--;
        if (U->type.ptr_level == 0)
        {
            U->type.is_pointer = false;
        }
        U->result = new_temp_var(); // TAC
        emit(TACOperator(TAC_OPERATOR_DEREF), U->result, x->result, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
        
    }
    else if (op->name == "MINUS" || op->name == "PLUS")
    {

        if (!x->type.isIntorFloat())
        {
            // Throw Error
            U->type = ERROR_TYPE;
            string error_msg = "Invalid operand with unary operator '+/-' at line " + to_string(x->line_no) + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return U;
        }
        U->type = x->type;
        U->type.make_signed();
        U->result = new_temp_var(); // TAC
        emit(TACOperator(op->name == "MINUS" ? TAC_OPERATOR_UMINUS : TAC_OPERATOR_ADD), U->result, x->result, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
    }
    else if (op->name == "NOT")
    {
        if (!x->type.isIntorFloat() && !x->type.is_pointer)
        {   
            U->type = ERROR_TYPE;
            string error_msg = "Invalid operand type for logical NOT '!' at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return U;
        }
        else 
        {
            U->type = Type(PrimitiveTypes::INT_T, 0, true);
        }
        U->result = new_temp_var(); // TAC
        emit(TACOperator(TAC_OPERATOR_NOT), U->result, x->result, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
    }
    else if (op->name == "BITWISE_NOT"){
        if (!x->type.isInt())
        {
            U->type = ERROR_TYPE;
            string error_msg = "Invalid operand with unary operator '~' at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }
        else
        {
            U->type = x->type.promote_to_int(x->type);
        }
        U->result = new_temp_var(); // TAC
        emit(TACOperator(TAC_OPERATOR_BIT_NOT), U->result, x->result, TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
    }
    return U;
}

Expression* create_unary_expression(Terminal* op, TypeName* tn){
    UnaryExpression* U = new UnaryExpression();
    U->op = op;
    U->type_name = tn;
    U->line_no = op->line_no;
    U->column_no = op->column_no;
    U->name = "UNARY EXPRESSION SIZEOF TYPE";
    U->type = Type(PrimitiveTypes::INT_T, 0, true);
    U->result = new_temp_var(); // TAC
    emit(TACOperator(TAC_OPERATOR_NOP), U->result, new_constant(to_string(tn->type.get_size())), TACOperand(TAC_OPERAND_EMPTY, "")); // TAC
    return U;
}


// ##############################################################################
// ################################## CAST EXPRESSION ######################################
// ##############################################################################
CastExpression :: CastExpression(): Expression(){
    name = "CAST EXPRESSION";
    unary_expression = nullptr;
    base_expression = nullptr;
    type_name = nullptr;
}

Expression* create_cast_expression(Expression* x){
    CastExpression* C = new CastExpression();
    C->unary_expression = dynamic_cast<UnaryExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->type = x->type;
    C->result = x->result; // TAC
    return C;
}

Expression* create_cast_expression(TypeName* tn, Expression* x){
    CastExpression* C = new CastExpression();
    C->name = "CAST EXPRESSION WITH TYPENAME";
    C->type_name = tn;
    C->base_expression = dynamic_cast<CastExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;

    if (x->type.is_error()) {
        C->type = ERROR_TYPE;
        return C;
    }

    if (!(x->type.is_convertible_to(tn->type))) {
        C->type = ERROR_TYPE;
        string error_msg = "Invalid type conversion at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return C;
    }
    C->type = tn->type;
    C->result = new_temp_var(); // TAC
    emit(TACOperator(TAC_OPERATOR_CAST), C->result, TACOperand(TAC_OPERAND_TYPE, tn->type.to_string()), x->result); // TAC

    return C;
}

// ##############################################################################
// ################################## MULTIPLICATIVE EXPRESSION ######################################
// ##############################################################################

MultiplicativeExpression::MultiplicativeExpression(){
    name = "MULTIPLICATIVE EXPRESSION";
    cast_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
}

Expression* create_multiplicative_expression(Expression* x){
    MultiplicativeExpression* M = new MultiplicativeExpression();
    M->cast_expression = dynamic_cast<CastExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    M->result = x->result; // TAC
    return M;
}

Expression* create_multiplicative_expression(Expression* left, Terminal* op, Expression* right){
    MultiplicativeExpression* M = new MultiplicativeExpression();
    M->left = left;
    M->right = right;
    M->op = op;
    M->line_no = left->line_no;
    M->column_no = left->column_no;
    M->name = "MULTIPLICATIVE EXPRESSION";

    if (left->type.is_error() || right->type.is_error()) {
        M->type = ERROR_TYPE;
        return M;
    }

    if (!left->type.isIntorFloat() || !right->type.isIntorFloat() || left->type.is_array || right->type.is_array || left->type.is_function || right->type.is_function || left->type.is_pointer || right->type.is_pointer) {
        M->type = ERROR_TYPE;
        string error_msg = "Operands of '" + op->name + "' must be int or float at line " +
                           to_string(M->line_no) + ", column " + to_string(M->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return M;
    }

    Type lt = left->type;
    Type rt = right->type;

    if(op->name == "MULTIPLY" || op->name == "DIVIDE"){
        if (lt.isFloat() || rt.isFloat()) {
            // float * float => float
            if(lt.typeIndex > rt.typeIndex){
                TACOperand t1 = new_temp_var(); // TAC
                M->result = new_temp_var(); // TAC
                emit(TACOperator(TAC_OPERATOR_CAST), t1, TACOperand(TAC_OPERAND_TYPE, lt.to_string()), right->result); // TAC
                emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, left->result, t1); // TAC
            } 
            else if(lt.typeIndex == rt.typeIndex){
                M->type = lt;
                M->result = new_temp_var(); // TAC
                emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, left->result, right->result); // TAC
            }
            else{
                M->type = rt;
                TACOperand t1 = new_temp_var(); // TAC
                M->result = new_temp_var(); // TAC
                emit(TACOperator(TAC_OPERATOR_CAST), t1, TACOperand(TAC_OPERAND_TYPE, rt.to_string()), left->result); // TAC
                emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, t1, right->result); // TAC
            } 
        } 
        else if (lt.isInt() && rt.isInt()) {
            // int * int => int
            if(lt.typeIndex > rt.typeIndex) {
                M->type = lt;
                if(lt.isUnsigned() || rt.isUnsigned()){
                    M->type.make_unsigned();
                }
                TACOperand t1 = new_temp_var(); // TAC
                M->result = new_temp_var(); // TAC
                emit(TACOperator(TAC_OPERATOR_CAST), t1, TACOperand(TAC_OPERAND_TYPE, M->type.to_string()), right->result); // TAC
                emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, left->result, t1); // TAC
            }
            else if(lt.typeIndex == rt.typeIndex){
                M->type = lt;
                M->result = new_temp_var(); // TAC
                emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, left->result, right->result); // TAC
            }
            else{
                M->type = rt;
                if(lt.isUnsigned() || rt.isUnsigned()){
                    M->type.make_unsigned();
                }
                TACOperand t1 = new_temp_var(); // TAC
                M->result = new_temp_var(); // TAC
                emit(TACOperator(TAC_OPERATOR_CAST), t1, TACOperand(TAC_OPERAND_TYPE, M->type.to_string()), left->result); // TAC
                emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, t1, right->result); // TAC
            }
        }
    }
    else if(op->name == "MOD"){
        if (!lt.isInt() || !rt.isInt())
        {
            M->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must be int at line " +
                               to_string(M->line_no) + ", column " + to_string(M->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return M;
        }
        if(lt.typeIndex > rt.typeIndex) {
            M->type = lt;
            if(lt.isUnsigned() || rt.isUnsigned()){
                M->type.make_unsigned();
            }
            TACOperand t1 = new_temp_var(); // TAC
            M->result = new_temp_var(); // TAC
            emit(TACOperator(TAC_OPERATOR_CAST), t1, TACOperand(TAC_OPERAND_TYPE, M->type.to_string()), right->result); // TAC
            emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, left->result, t1); // TAC
        }
        else if(lt.typeIndex == rt.typeIndex){
            M->type = lt;
            M->result = new_temp_var(); // TAC
            emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, left->result, right->result); // TAC
        }
        else{
            M->type = rt;
            if(lt.isUnsigned() || rt.isUnsigned()){
                M->type.make_unsigned();
            }
            TACOperand t1 = new_temp_var(); // TAC
            M->result = new_temp_var(); // TAC
            emit(TACOperator(TAC_OPERATOR_CAST), t1, TACOperand(TAC_OPERAND_TYPE, M->type.to_string()), left->result); // TAC
            emit(TACOperator(op->name == "MULTIPLY" ? TAC_OPERATOR_MUL : TAC_OPERATOR_DIV), M->result, t1, right->result); // TAC
        }
        
    }
    return M;
}

// ##############################################################################
// ################################## ADDITIVE EXPRESSION ######################################
// ##############################################################################

AdditiveExpression::AdditiveExpression(){
    multiplicative_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "ADDITIVE EXPRESSION";
}

Expression* create_additive_expression(Expression* x){
    AdditiveExpression* M = new AdditiveExpression();
    M->multiplicative_expression = dynamic_cast<MultiplicativeExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    M->result = x->result; // TAC
    return M;
}

Expression* create_additive_expression(Expression* left, Terminal* op, Expression* right){
    AdditiveExpression* A = new AdditiveExpression();
    A->left = left;
    A->right = right;
    A->op = op;
    A->line_no = left->line_no;
    A->column_no = left->column_no;
    A->name = "ADDITIVE EXPRESSION";

    if (left->type.is_error() || right->type.is_error()) {
        A->type = ERROR_TYPE;
        return A;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (lt.isFloat() || rt.isFloat()) {
        // float * float => float
        if(lt.typeIndex > rt.typeIndex){
            TACOperand t1 = new_temp_var(); // TAC
            A->result = new_temp_var(); // TAC
            emit(TACOperator(TAC_OPERATOR_CAST), t1, TACOperand(TAC_OPERAND_TYPE, lt.to_string()), right->result); // TAC
            emit(TACOperator(op->name == "PLUS" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), A->result, left->result, t1); // TAC
        } 
        else if(lt.typeIndex == rt.typeIndex){
            A->type = lt;
            A->result = new_temp_var(); // TAC
            emit(TACOperator(op->name == "PLUS" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), A->result, left->result, right->result); // TAC
        }
        else{
            A->type = rt;
            TACOperand t1 = new_temp_var(); // TAC
            A->result = new_temp_var(); // TAC
            emit(TACOperator(TAC_OPERATOR_CAST), t1, TACOperand(TAC_OPERAND_TYPE, rt.to_string()), left->result); // TAC
            emit(TACOperator(op->name == "PLUS" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), A->result, t1, right->result); // TAC
        } 
    } 
    else if (lt.isInt() && rt.isInt()) {
        // int * int => int
        if(lt.typeIndex > rt.typeIndex) {
            A->type = lt;
            if(lt.isUnsigned() || rt.isUnsigned()){
                A->type.make_unsigned();
            }
            TACOperand t1 = new_temp_var(); // TAC
            A->result = new_temp_var(); // TAC
            emit(TACOperator(TAC_OPERATOR_CAST), t1, TACOperand(TAC_OPERAND_TYPE, A->type.to_string()), right->result); // TAC
            emit(TACOperator(op->name == "PLUS" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), A->result, left->result, t1); // TAC
        }
        else if(lt.typeIndex == rt.typeIndex){
            A->type = lt;
            A->result = new_temp_var(); // TAC
            emit(TACOperator(op->name == "PLUS" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), A->result, left->result, right->result); // TAC
        }
        else{
            A->type = rt;
            if(lt.isUnsigned() || rt.isUnsigned()){
                A->type.make_unsigned();
            }
            TACOperand t1 = new_temp_var(); // TAC
            A->result = new_temp_var(); // TAC
            emit(TACOperator(TAC_OPERATOR_CAST), t1, TACOperand(TAC_OPERAND_TYPE, A->type.to_string()), left->result); // TAC
            emit(TACOperator(op->name == "PLUS" ? TAC_OPERATOR_ADD : TAC_OPERATOR_SUB), A->result, t1, right->result); // TAC
        }
    }
    else if (op->name == "ADD" && lt.isPointer() && rt.isInt()) {
        A->type = lt;
        TACOperand t1 = new_temp_var(); // TAC
        A->result = new_temp_var(); // TAC
        emit(TACOperator(TAC_OPERATOR_MUL), t1, new_constant(to_string(rt.get_size())), right->result); // TAC
        emit(TACOperator(TAC_OPERATOR_ADD), A->result, left->result, t1); // TAC
    } 
    else if (op->name == "ADD" && lt.isInt() && rt.isPointer()) {
        A->type = rt;
        TACOperand t1 = new_temp_var(); // TAC
        A->result = new_temp_var(); // TAC
        emit(TACOperator(TAC_OPERATOR_MUL), t1, new_constant(to_string(lt.get_size())), left->result); // TAC
        emit(TACOperator(TAC_OPERATOR_ADD), A->result, right->result, t1); // TAC
    } 
    else if (op->name == "MINUS" && lt.isPointer() && rt.isInt()) {
        A->type = lt;
        TACOperand t1 = new_temp_var(); // TAC
        A->result = new_temp_var(); // TAC
        emit(TACOperator(TAC_OPERATOR_MUL), t1, new_constant(to_string(rt.get_size())), right->result); // TAC
        emit(TACOperator(TAC_OPERATOR_SUB), A->result, left->result, t1); // TAC
    } 
    else if (op->name == "MINUS" && lt.isPointer() && rt.isPointer()) {
        if (lt == rt) {
            A->type = Type(PrimitiveTypes::INT_T, 0, false); 
            TACOperand t1 = new_temp_var(); // TAC
            A->result = new_temp_var(); // TAC
            emit(TACOperator(TAC_OPERATOR_SUB), t1, left->result, right->result); // TAC
            emit(TACOperator(TAC_OPERATOR_DIV), A->result, t1, new_constant(to_string(lt.get_size()))); // TAC
        } 
        else {
            A->type = ERROR_TYPE;
            string error_msg = "Pointer subtraction requires both pointers to be of the same type at line " +
                               to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        }
    } 
    else {
        A->type = ERROR_TYPE;
        string error_msg = "Operands of '" + op->name + "' are invalid at line " +
                           to_string(A->line_no) + ", column " + to_string(A->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return A;
    }
    return A;
}

// ##############################################################################
// ################################## SHIFT EXPRESSION ######################################
// ##############################################################################
ShiftExpression::ShiftExpression(){
    additive_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "SHIFT EXPRESSION";
}

Expression* create_shift_expression(Expression* x){
    ShiftExpression* M = new ShiftExpression();
    M->additive_expression = dynamic_cast<AdditiveExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    M->result = x->result; // TAC
    return M;
}

// Assumptions Based on C Semantics:
// 1.Shift operators require integral types.
// 2.Result type is usually the left operand's type.
// 3.The right operand must be an integer type, but in most implementations only the lower bits are used (based on width of left operand).
// 4.Signed shift behavior is implementation-defined or undefined in certain cases (e.g., shifting into the sign bit), but compilers like GCC allow it.
Expression* create_shift_expression(Expression* left, Terminal* op, Expression* right){
    ShiftExpression* S = new ShiftExpression();
    S->left = left;
    S->right = right;
    S->op = op;
    S->line_no = left->line_no;
    S->column_no = left->column_no;
    S->name = "SHIFT EXPRESSION";

    if (left->type.is_error() || right->type.is_error()) {
        S->type = ERROR_TYPE;
        return S;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (!lt.isInt() || !rt.isInt()) {
        S->type = ERROR_TYPE;
        string error_msg = "Operands of '" + op->name + "' must be integers at line " +
                           to_string(S->line_no) + ", column " + to_string(S->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return S;
    }
    if (lt.typeIndex > rt.typeIndex) {
        S->type = lt;
        // Signedness: usually taken from left operand
        if (lt.isUnsigned()) {
            S->type.make_unsigned();
        } 
        else {
            S->type.make_signed();
        }
        TACOperand t1 = new_temp_var(); // TAC
        S->result = new_temp_var(); // TAC
        emit(TACOperator(TAC_OPERATOR_CAST), t1, TACOperand(TAC_OPERAND_TYPE, S->type.to_string()), right->result); // TAC
        emit(TACOperator(op->name == "LEFT_OP" ? TAC_OPERATOR_LEFT_SHIFT : TAC_OPERATOR_RIGHT_SHIFT), S->result, left->result, t1); // TAC
    } 
    else if(lt.typeIndex == rt.typeIndex){
        S->type = lt;
        S->result = new_temp_var(); // TAC
        emit(TACOperator(op->name == "LEFT_OP" ? TAC_OPERATOR_LEFT_SHIFT : TAC_OPERATOR_RIGHT_SHIFT), S->result, left->result, right->result); // TAC
    }
    else {
        S->type = rt;
        // Signedness: usually taken from left operand
        if (lt.isUnsigned()) {
            S->type.make_unsigned();
        } 
        else {
            S->type.make_signed();
        }
        TACOperand t1 = new_temp_var(); // TAC
        S->result = new_temp_var(); // TAC
        emit(TACOperator(TAC_OPERATOR_CAST), t1, TACOperand(TAC_OPERAND_TYPE, S->type.to_string()), left->result); // TAC   
        emit(TACOperator(op->name == "LEFT_OP" ? TAC_OPERATOR_LEFT_SHIFT : TAC_OPERATOR_RIGHT_SHIFT), S->result, t1, right->result); // TAC
    }
    return S;
}

// ##############################################################################
// ################################## RELATIONAL EXPRESSION ######################################
// ##############################################################################

RelationalExpression::RelationalExpression() {
    shift_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "RELATIONAL EXPRESSION";
}

Expression* create_relational_expression(Expression* x){
    RelationalExpression* M = new RelationalExpression();
    M->shift_expression = dynamic_cast<ShiftExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    return M;
}

Expression* create_relational_expression(Expression* left, Terminal* op, Expression* right) {
    RelationalExpression* R = new RelationalExpression();
    R->left = left;
    R->right = right;
    R->op = op;
    R->line_no = left->line_no;
    R->column_no = left->column_no;
    R->name = "RELATIONAL EXPRESSION";

    if (left->type.is_error() || right->type.is_error()) {
        R->type = ERROR_TYPE;
        return R;
    }

    Type lt = left->type;
    Type rt = right->type;

    if ((lt.isIntorFloat() && rt.isIntorFloat())) {
        // Usual promotions can be added here if needed
        R->type = Type(PrimitiveTypes::INT_T, 0, false); 
        if (lt.isUnsigned() != rt.isUnsigned()){
            // Print warning message
        }
    } 
    else if(lt.is_pointer && rt.is_pointer) {
        R->type = Type(PrimitiveTypes::INT_T, 0, false);
    } else {
        R->type = ERROR_TYPE;
        string error_msg = "Operands of '" + op->name + "' must be integers or float at line " +
                           to_string(R->line_no) + ", column " + to_string(R->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return R;
    }
    return R;
}

// ##############################################################################
// ################################## EQUALITY EXPRESSION ######################################
// ##############################################################################

EqualityExpression::EqualityExpression() {
    relational_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "EQUALITY EXPRESSION";
}

Expression* create_equality_expression(Expression* x){
    EqualityExpression* M = new EqualityExpression();
    M->relational_expression = dynamic_cast<RelationalExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    return M;
}

Expression* create_equality_expression(Expression* left, Terminal* op, Expression* right) {
    EqualityExpression* E = new EqualityExpression();
    E->left = left;
    E->right = right;
    E->op = op;
    E->line_no = left->line_no;
    E->column_no = left->column_no;
    E->name = "EQUALITY EXPRESSION";

    if (left->type.is_error() || right->type.is_error()) {
        E->type = ERROR_TYPE;
        return E;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (lt.isIntorFloat() && rt.isIntorFloat()) {
        E->type = Type(PrimitiveTypes::INT_T, 0, false);  // Result is always int (0 or 1)
        
    } else if (lt.isPointer() && rt.isPointer()) {
        E->type = Type(PrimitiveTypes::INT_T, 0, false);
        
    } else {
        E->type = ERROR_TYPE;
        std::string error_msg = "Invalid operand types for operator '" + op->name +
                                "' at line " + std::to_string(E->line_no) +
                                ", column " + std::to_string(E->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }

    return E;
} 

// ##############################################################################
// ################################## AND EXPRESSION ######################################
// ##############################################################################

AndExpression::AndExpression() {
    equality_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "AND EXPRESSION";
}

Expression* create_and_expression(Expression* x){
    AndExpression* M = new AndExpression();
    M->equality_expression = dynamic_cast<EqualityExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    return M;
}

Expression* create_and_expression(Expression* left, Terminal* op, Expression* right) {
    AndExpression* A = new AndExpression();
    A->left = left;
    A->right = right;
    A->op = op;
    A->line_no = left->line_no;
    A->column_no = left->column_no;
    A->name = "AND EXPRESSION";

    if (left->type.is_error() || right->type.is_error()) {
        A->type = ERROR_TYPE;
        return A;
    }

    Type lt = left->type;
    Type rt = right->type;

    if(op->name == "BITWISE_AND"){
        if(lt.isInt() && rt.isInt()){
            A->type = lt.typeIndex > rt.typeIndex ? lt : rt;
            if(lt.isUnsigned() && rt.isUnsigned()){
                A->type.make_unsigned();
            }
            else if(!lt.isUnsigned() && rt.isUnsigned()){
                A->type.make_unsigned();
            }
            else if(lt.isUnsigned() && !rt.isUnsigned()){
                A->type.make_unsigned();
            }
            else if(!lt.isUnsigned() && !rt.isUnsigned()){
                A->type.make_signed();
            }
        } else {
            A->type = ERROR_TYPE;
            string error_msg = "Operands of '&' must be integers at line " +
                               to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }
    }

    return A;
}

// ##############################################################################
// ################################## XOR EXPRESSION ######################################
// ##############################################################################

XorExpression::XorExpression() {
    and_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "XOR EXPRESSION";
}

Expression* create_xor_expression(Expression* x){
    XorExpression* M = new XorExpression();
    M->and_expression = dynamic_cast<AndExpression*> (x);
    M->line_no = x->line_no;
    M->column_no = x->column_no;
    M->type = x->type;
    return M;
}

Expression* create_xor_expression(Expression* left, Terminal* op, Expression* right) {
    XorExpression* X = new XorExpression();
    X->left = left;
    X->right = right;
    X->op = op;
    X->line_no = left->line_no;
    X->column_no = left->column_no;
    X->name = "XOR EXPRESSION";

    if (left->type.is_error() || right->type.is_error()) {
        X->type = ERROR_TYPE;
        return X;
    }

    Type lt = left->type;
    Type rt = right->type;

    if(op->name == "BITWISE_XOR"){
        if(lt.isInt() && rt.isInt()){
            X->type = lt.typeIndex > rt.typeIndex ? lt : rt;
            if(lt.isUnsigned() && rt.isUnsigned()){
                X->type.make_unsigned();
            }
            else if(!lt.isUnsigned() && rt.isUnsigned()){
                X->type.make_unsigned();
            }
            else if(lt.isUnsigned() && !rt.isUnsigned()){
                X->type.make_unsigned();
            }
            else if(!lt.isUnsigned() && !rt.isUnsigned()){
                X->type.make_signed();
            }
        } else {
            X->type = ERROR_TYPE;
            string error_msg = "Operands of '^' must be integers at line " +
                               to_string(X->line_no) + ", column " + to_string(X->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }
    }
    return X;
}

// ##############################################################################
// ################################## OR EXPRESSION ######################################
// ##############################################################################

OrExpression::OrExpression() {
    xor_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "OR EXPRESSION";
}

Expression* create_or_expression(Expression* x){
    OrExpression* C = new OrExpression();
    C->xor_expression = dynamic_cast<XorExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->type = x->type;
    return C;
}

Expression* create_or_expression(Expression* left, Terminal* op, Expression* right) {
    OrExpression* O = new OrExpression();
    O->left = left;
    O->right = right;
    O->op = op;
    O->line_no = left->line_no;
    O->column_no = left->column_no;
    O->name = "OR EXPRESSION";

    if (left->type.is_error() || right->type.is_error()) {
        O->type = ERROR_TYPE;
        return O;
    }

    Type lt = left->type;
    Type rt = right->type;

    if(op->name == "BITWISE_OR"){
        if(lt.isInt() && rt.isInt()){
            O->type = lt.typeIndex > rt.typeIndex ? lt : rt;
            if(lt.isUnsigned() && rt.isUnsigned()){
                O->type.make_unsigned();
            }
            else if(!lt.isUnsigned() && rt.isUnsigned()){
                O->type.make_unsigned();
            }
            else if(lt.isUnsigned() && !rt.isUnsigned()){
                O->type.make_unsigned();
            }
            else if(!lt.isUnsigned() && !rt.isUnsigned()){
                O->type.make_signed();
            }
        } else {
            O->type = ERROR_TYPE;
            string error_msg = "Operands of '|' must be integers at line " +
                               to_string(O->line_no) + ", column " + to_string(O->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }
    }
    return O;
}

// ##############################################################################
// ################################## LOGICAL AND EXPRESSION ######################################
// ##############################################################################

LogicalAndExpression::LogicalAndExpression() {
    or_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "LOGICAL AND EXPRESSION";
}

Expression* create_logical_and_expression(Expression* x){
    LogicalAndExpression* C = new LogicalAndExpression();
    C->or_expression = dynamic_cast<OrExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->type = x->type;
    return C;
}

Expression* create_logical_and_expression(Expression* left, Terminal* op, Expression* right) {
    LogicalAndExpression* L = new LogicalAndExpression();
    L->left = left;
    L->right = right;
    L->op = op;
    L->line_no = left->line_no;
    L->column_no = left->column_no;
    L->name = "LOGICAL AND EXPRESSION";

    if (left->type.is_error() || right->type.is_error()) {
        L->type = ERROR_TYPE;
        return L;
    }

    Type lt = left->type;
    Type rt = right->type;

    if(op->name == "LOGICAL_AND"){
        if(lt.isIntorFloat() && rt.isIntorFloat()){
            L->type = Type(PrimitiveTypes::INT_T, 0, false); 
        } else {
            L->type = ERROR_TYPE;
            string error_msg = "Operands of '&&' must be integers or float at line " +
                               to_string(L->line_no) + ", column " + to_string(L->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }
    }
    return L;
}

// ##############################################################################
// ################################## LOGICAL OR EXPRESSION ######################################
// ##############################################################################

LogicalOrExpression::LogicalOrExpression() {
    logical_and_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "LOGICAL OR EXPRESSION";
}

Expression* create_logical_or_expression(Expression* x){
    LogicalOrExpression* C = new LogicalOrExpression();
    C->logical_and_expression = dynamic_cast<LogicalAndExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->type = x->type;
    return C;
}

Expression* create_logical_or_expression(Expression* left, Terminal* op, Expression* right) {
    LogicalOrExpression* L = new LogicalOrExpression();
    L->left = left;
    L->right = right;
    L->op = op;
    L->line_no = left->line_no;
    L->column_no = left->column_no;
    L->name = "LOGICAL OR EXPRESSION";

    if (left->type.is_error() || right->type.is_error()) {
        L->type = ERROR_TYPE;
        return L;
    }

    Type lt = left->type;
    Type rt = right->type;

    if(op->name == "LOGICAL_OR"){
        if(lt.isIntorFloat() && rt.isIntorFloat()){
            L->type = Type(PrimitiveTypes::INT_T, 0, false); 
        } else {
            L->type = ERROR_TYPE;
            string error_msg = "Operands of '||' must be integers or float at line " +
                               to_string(L->line_no) + ", column " + to_string(L->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }
    }
    return L;
}

// ##############################################################################
// ################################## CONDITIONAL EXPRESSION ######################################
// ##############################################################################

ConditionalExpression::ConditionalExpression() {
    logical_or_expression = nullptr;
    condition = nullptr;
    true_expr = nullptr;
    false_expr = nullptr;
    name = "CONDITIONAL EXPRESSION";
}

Expression* create_conditional_expression(Expression* x){
    ConditionalExpression* C = new ConditionalExpression();
    C->logical_or_expression = dynamic_cast<LogicalOrExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->type = x->type;
    return C;
}

Expression* create_conditional_expression(Expression* condition, Expression* true_expr, Expression* false_expr) {
    ConditionalExpression* C = new ConditionalExpression();
    C->condition = condition;
    C->true_expr = true_expr;
    C->false_expr = false_expr;
    C->line_no = condition->line_no;
    C->column_no = condition->column_no;
    C->name = "CONDITIONAL EXPRESSION";

    if (condition->type.is_error() || true_expr->type.is_error() || false_expr->type.is_error()) {
        C->type = ERROR_TYPE;
        return C;
    }

    Type ct = condition->type;
    Type tt = true_expr->type;
    Type ft = false_expr->type;

    if (!ct.isIntorFloat()) {
        C->type = ERROR_TYPE;
        std::string error_msg = "Condition of conditional expression must be int at line " +
                                std::to_string(C->line_no) + ", column " + std::to_string(C->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return C;
    } else if (tt == ft) {
        C->type = tt;
    } else if (tt.isIntorFloat() && ft.isIntorFloat()) {
        // Promote both and take the higher ranked one
        C->type = tt.typeIndex > ft.typeIndex ? tt : ft;
        if (tt.isUnsigned() && ft.isUnsigned()) {
            C->type.make_unsigned();
        } else if (!tt.isUnsigned() && ft.isUnsigned()) {
            C->type.make_unsigned();
        } else if (tt.isUnsigned() && !ft.isUnsigned()) {
            C->type.make_unsigned();
        } else if (!tt.isUnsigned() && !ft.isUnsigned()) {
            C->type.make_signed();
        }
    } else if(tt == ft){
        C->type = tt;
    } else {
        C->type = ERROR_TYPE;
        std::string error_msg = "Expressions are incomatible for ternary operator at line " +
                                std::to_string(C->line_no) + ", column " + std::to_string(C->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }

    return C;
}

// ##############################################################################
// ################################## ASSIGNMENT EXPRESSION ######################################
// ##############################################################################

AssignmentExpression::AssignmentExpression() {
    conditional_expression = nullptr;
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "ASSIGNMENT EXPRESSION";
}

Expression* create_assignment_expression(Expression* x){
    AssignmentExpression* C = new AssignmentExpression();
    C->conditional_expression = dynamic_cast<ConditionalExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->type = x->type;  
    return C;
}

Expression* create_assignment_expression(Expression* left, Terminal* op, Expression* right) {
    AssignmentExpression* A = new AssignmentExpression();
    A->left = left;
    A->right = right;
    A->op = op;
    A->line_no = left->line_no;
    A->column_no = left->column_no;
    A->name = "ASSIGNMENT EXPRESSION";

    if (left->type.is_error() || right->type.is_error()) {
        A->type = ERROR_TYPE;
        return A;
    }

    Type lt = left->type;
    Type rt = right->type;

    if (lt.is_const_variable) {
        yyerror(("Assignment to const variable at line " + std::to_string(A->line_no)).c_str());
        symbolTable.set_error();
        A->type = ERROR_TYPE;
        return A;
    }

    if (op->name == "ASSIGN") {
        // Check if RHS can be implicitly converted to LHS
        if (!rt.is_convertible_to(lt)) {
            yyerror(("Incompatible types in assignment at line " + std::to_string(A->line_no)).c_str());
            symbolTable.set_error();
            A->type = ERROR_TYPE;
            return A;
        }
        A->type = lt;  // Resulting type is type of LHS
        return A;
    } else if (op->name == "MUL_ASSIGN" || op->name == "DIV_ASSIGN" ){
        if (!lt.isIntorFloat() || !rt.isIntorFloat()) {
            A->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must be int or float at line " +
                               to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        }
        A->type = lt;  // Resulting type is type of LHS
    } else if (op->name == "ADD_ASSIGN" || op->name == "SUB_ASSIGN") {
        if (!lt.isIntorFloat() || !rt.isIntorFloat()) {
            A->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must be int or float at line " +
                               to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        } else if (lt.isPointer() && rt.isInt())
        {
            A->type = lt; 
        }
        A->type = lt; 
    } else if (op->name == "MOD_ASSIGN"){
        if (!lt.isInt() || !rt.isInt()) {
            A->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must be int at line " +
                               to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        }
        A->type = lt; 
    } else if (op->name == "AND_ASSIGN" || op->name == "OR_ASSIGN" || op->name == "XOR_ASSIGN") {
        if (!lt.isInt() || !rt.isInt()) {
            A->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must be int at line " +
                               to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        } 
        A->type = lt; 
    } else if (op->name == "LEFT_ASSIGN" || op->name == "RIGHT_ASSIGN"){
        if (!lt.isInt() || !rt.isInt()) {
            A->type = ERROR_TYPE;
            string error_msg = "Operands of '" + op->name + "' must be int at line " +
                               to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return A;
        } 
        A->type = lt; 
    } 
    return A;
}

// ##############################################################################
// ################################## EXPRESSION LIST ######################################   
// ##############################################################################

ExpressionList::ExpressionList() {
    name = "EXPRESSION LIST";
    type = ERROR_TYPE;
}

ExpressionList* create_expression_list(Expression* x){
    ExpressionList* E = new ExpressionList();
    E->expression_list.push_back(x);
    E->line_no = x->line_no;
    E->column_no = x->column_no;
    E->type = x->type;  // if expression list does not have an erronous expression, set type to the type of the first expression. Else set it as ERROR_TYPE.
    return E;
}

ExpressionList* create_expression_list(ExpressionList* expression_list, Expression* new_expression) {
    expression_list->expression_list.push_back(new_expression);
    if(new_expression->type.is_error()) {
        expression_list->type = ERROR_TYPE;
        return expression_list;
    }
    return expression_list;
}
