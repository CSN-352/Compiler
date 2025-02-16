%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"  

// External declarations
extern int yylex();
extern int yyparse();
extern int yylineno;
extern FILE *yyin;

void yyerror(const char *msg);

%} 

/* Token definitions */
%debug
%token AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO
%token IF INT LONG REGISTER RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF UNION UNSIGNED
%token VOID VOLATILE WHILE UNTIL CLASS PRIVATE PUBLIC PROTECTED ASSEMBLY_DIRECTIVE
%token IDENTIFIER I_CONSTANT H_CONSTANT O_CONSTANT F_CONSTANT STRING_LITERAL C_CONSTANT
%token ELLIPSIS RIGHT_ASSIGN LEFT_ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token RIGHT_OP LEFT_OP INC_OP DEC_OP INHERITANCE_OP PTR_OP AND_OP OR_OP LE_OP GE_OP EQ_OP NE_OP XOR_OP
%token SEMICOLON LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET COMMA COLON EQ LEFT_BRACKET RIGHT_BRACKET LEFT_THIRD_BRACKET RIGHT_THIRD_BRACKET
%token DOT LOGICAL_AND NOT LOGICAL_NOT MINUS PLUS MULTIPLY DIVIDE MOD LESS GREATER EXPONENT LOGICAL_OR QUESTION
%token NEWLINE ERROR
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%start translation_unit
%%

primary_expression:
    IDENTIFIER
    | I_CONSTANT
    | O_CONSTANT
    | H_CONSTANT
    | F_CONSTANT
    | C_CONSTANT
    | STRING_LITERAL
    | LEFT_BRACKET expression RIGHT_BRACKET
    ;

postfix_expression:
    primary_expression
    | postfix_expression LEFT_BRACKET expression RIGHT_BRACKET
    | postfix_expression LEFT_BRACKET RIGHT_BRACKET
    | postfix_expression LEFT_THIRD_BRACKET expression RIGHT_THIRD_BRACKET
    | postfix_expression DOT IDENTIFIER
    | postfix_expression PTR_OP IDENTIFIER
    | postfix_expression INC_OP
    | postfix_expression DEC_OP
    ;

unary_expression:
    postfix_expression
    | INC_OP unary_expression
    | DEC_OP unary_expression
    | unary_operator cast_expression
    | SIZEOF unary_expression
    | SIZEOF LEFT_BRACKET type_name RIGHT_BRACKET
    ;

unary_operator:
    AND_OP
    | MULTIPLY
    | PLUS
    | MINUS
    | NOT
    | LOGICAL_NOT
    ;

cast_expression:
    unary_expression
    | LEFT_BRACKET type_name RIGHT_BRACKET cast_expression
    ;

multiplicative_expression:
    cast_expression
    | multiplicative_expression MULTIPLY cast_expression
    | multiplicative_expression DIVIDE cast_expression
    | multiplicative_expression MOD cast_expression
    ;

additive_expression:
    multiplicative_expression
    | additive_expression PLUS multiplicative_expression
    | additive_expression MINUS multiplicative_expression
    ;

shift_expression:
    additive_expression
    | shift_expression LEFT_OP additive_expression
    | shift_expression RIGHT_OP additive_expression
    ;

relational_expression:
    shift_expression
    | relational_expression LESS shift_expression
    | relational_expression GREATER shift_expression
    | relational_expression LE_OP shift_expression
    | relational_expression GE_OP shift_expression
    ;

equality_expression:
    relational_expression
    | equality_expression EQ_OP relational_expression
    | equality_expression NE_OP relational_expression
    ;

and_expression:
    equality_expression
    | and_expression AND_OP equality_expression
    ;

xor_expression:
    and_expression
    | xor_expression XOR_OP and_expression
    ;

or_expression:
    xor_expression
    | or_expression OR_OP xor_expression
    ;

logical_and_expression:
    or_expression
    | logical_and_expression LOGICAL_AND or_expression
    ;

logical_or_expression:
    logical_and_expression
    | logical_or_expression LOGICAL_OR logical_and_expression
    ;

conditional_expression:
    logical_or_expression
    | logical_or_expression QUESTION expression COLON conditional_expression
    ;

assignment_expression:
    conditional_expression
    | unary_expression assignment_operator assignment_expression
    ;

assignment_operator:
    EQ
    | MUL_ASSIGN
    | DIV_ASSIGN
    | MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
    ;

expression:
    assignment_expression
    | expression COMMA assignment_expression
    ;

declaration:
    declaration_specifiers SEMICOLON
    | declaration_specifiers init_declarator_list SEMICOLON
    ;

declaration_specifiers:
    storage_class_specifier
    | storage_class_specifier declaration_specifiers
	| type_specifier
	| type_specifier declaration_specifiers
	| type_qualifier
	| type_qualifier declaration_specifiers
    ;

init_declarator_list:
    init_declarator
    | init_declarator_list COMMA init_declarator
    ;

init_declarator:
    declarator
    | declarator EQ initializer
    ;

storage_class_specifier:
    TYPEDEF
    | EXTERN
    | STATIC
    | AUTO
    | REGISTER
    ;

type_specifier:
    VOID
    | CHAR
	| SHORT
	| INT
	| LONG
	| FLOAT
	| DOUBLE
	| SIGNED
	| UNSIGNED
	| struct_or_union_specifier
	| enum_specifier
	;

struct_or_union_specifier:
    struct_or_union IDENTIFIER LEFT_CURLY_BRACKET struct_declaration_list RIGHT_CURLY_BRACKET
	| struct_or_union LEFT_CURLY_BRACKET struct_declaration_list RIGHT_CURLY_BRACKET
	| struct_or_union IDENTIFIER
	;

struct_or_union:
    STRUCT
    | UNION
    ;

struct_declaration_list:
    struct_declaration
    | struct_declaration_list struct_declaration
    ;

struct_declaration:
    specifier_qualifier_list struct_declarator_list SEMICOLON
    ;

specifier_qualifier_list:
    type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

struct_declarator_list:
    struct_declarator
	| struct_declarator_list COMMA struct_declarator
	;

struct_declarator:
    declarator
	| COLON conditional_expression
	| declarator COLON conditional_expression
	;

enum_specifier:
    ENUM LEFT_CURLY_BRACKET enumerator_list RIGHT_CURLY_BRACKET
	| ENUM IDENTIFIER LEFT_CURLY_BRACKET enumerator_list RIGHT_CURLY_BRACKET
	| ENUM IDENTIFIER
	;

enumerator_list:
    enumerator
    | enumerator_list COMMA enumerator
    ;

enumerator:
    IDENTIFIER
    | IDENTIFIER EQ conditional_expression
    ;

type_qualifier:
    CONST
    | VOLATILE
    ;

declarator:
    pointer direct_declarator
    | direct_declarator
    ;

direct_declarator:
    IDENTIFIER 
    | direct_declarator LEFT_THIRD_BRACKET conditional_expression RIGHT_THIRD_BRACKET
	| direct_declarator LEFT_THIRD_BRACKET RIGHT_THIRD_BRACKET
    | LEFT_BRACKET declarator RIGHT_BRACKET
    | direct_declarator LEFT_BRACKET parameter_type_list RIGHT_BRACKET 
    | direct_declarator LEFT_BRACKET identifier_list RIGHT_BRACKET 
    | direct_declarator LEFT_BRACKET RIGHT_BRACKET 
    ;

pointer:
    MULTIPLY
    | MULTIPLY type_qualifier_list
    | MULTIPLY pointer
    | MULTIPLY type_qualifier_list pointer
    ;

type_qualifier_list:
    type_qualifier
    | type_qualifier_list type_qualifier
    ;

parameter_type_list:
    parameter_list
    | parameter_list COMMA ELLIPSIS
    ;

parameter_list:
    parameter_declaration
    | parameter_list COMMA parameter_declaration
    ;

parameter_declaration:
    declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list:
    IDENTIFIER
	| identifier_list COMMA IDENTIFIER
	;

type_name:
    specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator:
    pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator:
    LEFT_BRACKET abstract_declarator RIGHT_BRACKET
    | LEFT_THIRD_BRACKET conditional_expression RIGHT_THIRD_BRACKET
	| LEFT_THIRD_BRACKET RIGHT_THIRD_BRACKET
	| LEFT_BRACKET RIGHT_BRACKET
	| LEFT_BRACKET parameter_type_list RIGHT_BRACKET
	| direct_abstract_declarator LEFT_BRACKET RIGHT_BRACKET
	| direct_abstract_declarator LEFT_BRACKET parameter_type_list RIGHT_BRACKET
	;

initializer:
    assignment_expression
    | LEFT_CURLY_BRACKET initializer_list RIGHT_CURLY_BRACKET
    | LEFT_CURLY_BRACKET initializer_list COMMA RIGHT_CURLY_BRACKET
    ;

initializer_list:
    initializer
    | initializer_list COMMA initializer
    ;

statement:
    labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement:
    IDENTIFIER COLON statement
	| CASE conditional_expression COLON statement
	| DEFAULT COLON statement
	;

compound_statement:
    LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET
    | LEFT_CURLY_BRACKET statement_list RIGHT_CURLY_BRACKET
    | LEFT_CURLY_BRACKET declaration_list RIGHT_CURLY_BRACKET
    | LEFT_CURLY_BRACKET declaration_list statement_list RIGHT_CURLY_BRACKET
    ;

declaration_list:
    declaration
    | declaration_list declaration
    ;

statement_list:
    statement
    | statement_list statement
    ;

expression_statement:
    SEMICOLON
    expression SEMICOLON
    ;

selection_statement:
    IF LEFT_BRACKET expression RIGHT_BRACKET statement %prec LOWER_THAN_ELSE
    | IF LEFT_BRACKET expression RIGHT_BRACKET ELSE statement
    | SWITCH LEFT_BRACKET expression statement
    ;

iteration_statement:
    WHILE LEFT_BRACKET expression RIGHT_BRACKET statement
    | DO statement WHILE LEFT_BRACKET expression RIGHT_BRACKET SEMICOLON
    | FOR LEFT_BRACKET expression_statement expression_statement RIGHT_BRACKET statement
    | FOR LEFT_BRACKET expression_statement expression_statement expression RIGHT_BRACKET statement
    | UNTIL LEFT_BRACKET expression RIGHT_BRACKET statement
    ;

jump_statement:
    GOTO IDENTIFIER SEMICOLON
	| CONTINUE SEMICOLON
	| BREAK SEMICOLON
	| RETURN SEMICOLON
	| RETURN expression SEMICOLON
	;

translation_unit:
    external_declaration
    | translation_unit external_declaration
    ;

external_declaration:
	function_definition
	| declaration
	;

function_definition:
    declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
	| declarator declaration_list compound_statement
	| declarator compound_statement
	;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, msg);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    yyin = file;  // Set yyin to read from the input file
    yydebug = 1;
    yyparse();    // Call the parser
    fclose(file); // Close file after parsing
    printf("Parsing completed successfully.\n");
    return 0;
}


