#include "symbol_table.h"
#include "ast.h"
#include "expression.h"
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
    }
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
    return P;
}

Expression* create_primary_expression(StringLiteral* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION STRING LITERAL";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->string_literal = x;
    P->type = Type(CHAR_T, 1, true);
    return P;
}

Expression* create_primary_expression(Expression* x){
    return x;
}

// ##############################################################################
// ################################## ARGUMENT EXPRESSION LIST ######################################
// ##############################################################################

ArgumentExpressionList :: ArgumentExpressionList() : Expression() {
    name = "ARGUMENT EXPRESSION LIST";
};

ArgumentExpressionList* create_argument_expression_list(Expression* x){
    ArgumentExpressionList* P = new ArgumentExpressionList();
    P->arguments.push_back(x);
    return P;
}

ArgumentExpressionList* create_argument_expression_list(ArgumentExpressionList* args_expr_list, Expression* x){
    args_expr_list->arguments.push_back(x);
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
    op = nullptr;
    member_name = nullptr;
}

Expression* create_postfix_expression(Expression* x){
    PostfixExpression* P = new PostfixExpression();
    P->primary_expression = dynamic_cast<PrimaryExpression*>(x);
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
    }
    else if(x->type.isFloat()){
        P->type = x->type;
    }
    else if(x->type.is_pointer){
        P->type = x->type;
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
        else{
            
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
        }
    }
    return P;
}

Expression* create_postfix_expression(Expression* x, Expression* index_expression) {
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
            else P->type = x->type;
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
    U->base_expression = dynamic_cast<UnaryExpression*> (x);
    U->postfix_expression = U->base_expression->postfix_expression;
    U->line_no = x->line_no;
    U->column_no = x->column_no;

    if(x->type.is_error()){
        U->type = ERROR_TYPE;
        return U;
    }
    
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
        }

        if (x->type.is_pointer || x->type.isIntorFloat())
        {
            U->type = x->type;
        } else
        {   
            U->type = ERROR_TYPE;
            string error_msg = "Operator with invalid type " + to_string(x->line_no) + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
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
    U->postfix_expression = U->base_expression->postfix_expression;
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
        }
        U->type = x->type;
        U->type.ptr_level++;
        U->type.is_pointer = true;
    }
    else if (op->name == "MULTIPLY")
    {
        if (x->type.ptr_level <= 0)
        {
            U->type = ERROR_TYPE;
            string error_msg = "Cannot dereference a non-pointer at line " + to_string(x->line_no) + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }

        U->type = x->type;
        U->type.ptr_level--;
        if (U->type.ptr_level == 0)
        {
            U->type.is_pointer = false;
        }
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
        }

        U->type = x->type;
        U->type.make_signed();
    }
    else if (op->name == "NOT")
    {
        if (!x->type.isIntorFloat() && !x->type.is_pointer)
        {   
            U->type = ERROR_TYPE;
            string error_msg = "Invalid operand type for logical NOT '!' at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }
        else 
        {
            U->type = Type(PrimitiveTypes::INT_T, 0, true);
        }
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
    return U;
}


// ##############################################################################
// ################################## CAST EXPRESSION ######################################
// ##############################################################################
CastExpression :: CastExpression(): Expression(){
    unary_expression = nullptr;
    base_expression = nullptr;
    type_name = nullptr;
}

Expression* create_cast_expression(Expression* x){
    CastExpression* C = new CastExpression();
    C->unary_expression = dynamic_cast<UnaryExpression*> (x);
    C->line_no = x->line_no;
    C->column_no = x->column_no;
    C->name = "CAST EXPRESSION";
    if (x->type.is_error()) {
        C->type = ERROR_TYPE;
        return C;
    }
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
    return C;
}

// ##############################################################################
// ################################## MULTIPLICATIVE EXPRESSION ######################################
// ##############################################################################

MultiplicativeExpression::MultiplicativeExpression(){
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "MULTIPLICATIVE EXPRESSION";
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

    if (!left->type.isIntorFloat() || !right->type.isIntorFloat()) {
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
        if (lt.isFloat() && rt.isFloat()) {
            // float * float => float
            if(lt->typeIndex > rt->typeIndex) M->type = lt;
            else M->type = rt;
        } else if (lt.isFloat() && rt.isInt()) {
            // float * int => float
            if(lt->typeIndex > rt->typeIndex) M->type = lt;
            else M->type = rt;
        } else if (lt.isInt() && rt.isFloat()) {
            // int * float => float
            if(lt->typeIndex > rt->typeIndex) M->type = lt;
            else M->type = rt;
        } else if (lt.isInt() && rt.isInt()) {
            // int * int => int
            if(lt->typeIndex > rt->typeIndex) M->type = lt;
            else M->type = rt;
            if(lt.isUnsigned() && rt.isUnsigned()){
                M->type.make_unsigned();
            }
            else if(!lt.isUnsigned() && rt.isUnsigned()){
                M->type.make_unsigned();
            }
            else if(lt.isUnsigned() && !rt.isUnsigned()){
                M->type.make_unsigned();
            }
            else if(!lt.isUnsigned() && !rt.isUnsigned()){
                M->type.make_signed();
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

        if(lt->typeIndex > rt->typeIndex) M->type = lt;
        else M->type = rt;
        
        if(lt.isUnsigned() && rt.isUnsigned()){
            M->type.make_unsigned();
        }
        else if(!lt.isUnsigned() && rt.isUnsigned()){
            M->type.make_unsigned();
        }
        else if(lt.isUnsigned() && !rt.isUnsigned()){
            M->type.make_unsigned();
        }
        else if(!lt.isUnsigned() && !rt.isUnsigned()){
            M->type.make_signed();
        }
    }
    return M;
}

// ##############################################################################
// ################################## ADDITIVE EXPRESSION ######################################
// ##############################################################################

AdditiveExpression::AdditiveExpression(){
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "ADDITIVE EXPRESSION";
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

    if (lt.isFloat() && rt.isFloat()) {
        // float * float => float
        if(lt->typeIndex > rt->typeIndex) M->type = lt;
        else M->type = rt;
    } else if (lt.isFloat() && rt.isInt()) {
        // float * int => float
        if(lt->typeIndex > rt->typeIndex) M->type = lt;
        else M->type = rt;
    } else if (lt.isInt() && rt.isFloat()) {
        // int * float => float
        if(lt->typeIndex > rt->typeIndex) M->type = lt;
        else M->type = rt;
    } else if (lt.isInt() && rt.isInt()) {
        // int * int => int
        if(lt->typeIndex > rt->typeIndex) M->type = lt;
        else M->type = rt;
        if(lt.isUnsigned() && rt.isUnsigned()){
            M->type.make_unsigned();
        }
        else if(!lt.isUnsigned() && rt.isUnsigned()){
            M->type.make_unsigned();
        }
        else if(lt.isUnsigned() && !rt.isUnsigned()){
            M->type.make_unsigned();
        }
        else if(!lt.isUnsigned() && !rt.isUnsigned()){
            M->type.make_signed();
        }
    } else if (op->name == "ADD" && lt.isPointer() && rt.isInt()) {
        A->type = lt;
    } else if (op->name == "ADD" && lt.isInt() && rt.isPointer()) {
        A->type = rt;
    } else if (op->name == "MINUS" && lt.isPointer() && rt.isInt()) {
        A->type = lt;
    } else if (op->name == "MINUS" && lt.isPointer() && rt.isPointer()) {
        if (lt == rt) {
            A->type = Type(PrimitiveTypes::INT_T, 0, false); 
        } else {
            A->type = ERROR_TYPE;
            string error_msg = "Pointer subtraction requires both pointers to be of the same type at line " +
                               to_string(A->line_no) + ", column " + to_string(A->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }
    } else {
        A->type = ERROR_TYPE;
        string error_msg = "Operands of '" + op->name + "' are invalid at line " +
                           to_string(A->line_no) + ", column " + to_string(A->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
}

// ##############################################################################
// ################################## SHIFT EXPRESSION ######################################
// ##############################################################################
ShiftExpression::ShiftExpression(){
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "SHIFT EXPRESSION";
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
    if (lt.typeIndex >= rt.typeIndex) {
        S->type = lt;
    } else {
        S->type = rt;
    }

    // Signedness: usually taken from left operand
    if (lt.isUnsigned()) {
        S->type.make_unsigned();
    } else {
        S->type.make_signed();
    }
    return S;
}

// ##############################################################################
// ################################## RELATIONAL EXPRESSION ######################################
// ##############################################################################

RelationalExpression::RelationalExpression() {
    left = nullptr;
    right = nullptr;
    op = nullptr;
    name = "RELATIONAL EXPRESSION";
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
        R->type = Type(PrimitiveTypes::INT_T, 0, true); // Result of relational is always int (true/false)
        return R;
    }
    

}

// ##############################################################################
// ################################## CONDITIONAL EXPRESSION ######################################
// ##############################################################################

