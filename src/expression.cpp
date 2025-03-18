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
Expression :: Expression() : NonTerminal(0, 0, "") {}
extern unsigned int line_no;
extern unsigned int column_no;

Expression* PrimaryExpression :: create_primary_expression(Identifier* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION IDENTIFIER";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->add_children(x);
    Symbol* sym = symbolTable.getSymbol(x->value);
    if(sym) P->type = sym->type;
    else {
        P->type = ERROR_TYPE;
        string error_msg = "Undeclared Symbol " + x->value + " at line " + to_string(x->line_no) + ", column " + to_string(x->column_no);
		yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return P;
}

Expression* PrimaryExpression :: create_primary_expression(Constant* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION CONSTANT";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->type = x->get_constant_type();
    P->add_children(x);
    return P;
}

Expression* PrimaryExpression :: create_primary_expression(StringLiteral* x){
    PrimaryExpression* P = new PrimaryExpression();
    P->name = "PRIMARY EXPRESSION STRING LITERAL";
    P->line_no = x->line_no;
    P->column_no = x->column_no;
    P->type = Type(CHAR_T, 1, true) 
    P->add_children(x);
    return P;
}

Expression* PrimaryExpression :: create_primary_expression(Expression* x){
    return x;
}




Expression *create_unary_expression( Terminal *op, Expression *ue ) {
    UnaryExpression *U = new UnaryExpression();
    U->op1 = ue;
    U->op = op->name;
    Type ueT = ue->type;
    if ( ueT.is_invalid() ) {
        U->type = ERROR_TYPE;
        return U;
    }
    std::string u_op = op->name;
    U->name = u_op;
    // Address *inc_value = nullptr;

    if ( u_op == "++" || u_op == "--" ) {
        if ( ueT.is_const == true ) {
            string error_msg = "Line no:" + to_string(line_no) + " Column no:" + to_string(column_no) + " Invalid operand "+ u_op + " with constant type";
            // error_msg( "Invalid operand " + u_op + " with constant type",
            //            op->line_no, op->column_no );
            yyerror(error_msg.c_str());
            U->type = ERROR_TYPE;
            return U;
        }
        
		u_op = u_op.substr( 0, 1 );
		if ( ue->type.isPointer() ) {
			U->type = ue->type;
			    // U->res = new_mem(U->type);
			Type t = ue->type;
			t.ptr_level--;
			// inc_value = new_3const( t.get_size() , INT3 );
		} else if ( ue->type.isInt() ) {
            // U->res = new_temp();
			U->type = ue->type;
			// inc_value = new_3const( 1 , INT3 );

		} else if ( ue->type.isFloat() ) {
            // U->res = new_temp();
			U->type = ue->type;
			// inc_value = new_3const( 1.0, FLOAT3 );
		} else {
			// Incorrect type throw error
            string error_msg = "Line no:" + to_string(line_no) + " Column no:" + to_string(column_no) + " Invalid operand "+ u_op + " with type";
			// error_msg( "Invalid operand " + u_op + " with type " +
			// 			   ue->type.get_name(),
			// 		   op->line_no, op->column_no );
            yyerror(error_msg.c_str());
			// delete U->res;
			// U->res = nullptr;
			U->type = ERROR_TYPE;
			return U;
		}



        // if ( ue->res->type == MEM ) {
		// Address * t1 = new_temp();
        //     emit( t1, "()", ue->res, nullptr );
        //     emit( U->res, u_op, t1, inc_value );
        //     emit( ue->res, "()s", U->res, nullptr );
        // } else if ( ue->res->type == ID3 ) {
        //     U->res = ue->res;
		// 	ue->res->type = TEMP;
        //     emit( U->res, u_op, U->res, inc_value );
        // } else {
		// 	delete inc_value;
		// 	inc_value = nullptr;
        //     error_msg( "lvalue required as unary " + op->name + " operand",
        //                op->line_num, op->column );
        //     U->type = ERROR_TYPE;
        //     return U;
        // }
    } else if ( u_op == "sizeof" ) {
        U->name = "sizeof";
        U->type = Type();
        U->type.typeIndex = PrimitiveTypes::INT_T;
        U->type.ptr_level = 0;
        U->type.is_const = true;
        // U->res = new_3const( ue->type.get_size() , INT3);
    } else {
        // Raise Error
        std::cerr << "Error parsing Unary Expression.\n";
        std::cerr << "ERROR at line " << line_no << "\n";
        exit( 0 );
    }
    U->add_children( ue );
    return U;
}

