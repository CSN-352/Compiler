%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// External declarations
extern int yylex();
extern int yyparse();
extern FILE *yyin;

void yyerror(const char *msg);
%}

/* Token definitions */
%token AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO
%token IF INT LONG REGISTER RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF UNION UNSIGNED
%token VOID VOLATILE WHILE UNTIL CLASS PRIVATE PUBLIC PROTECTED ASSEMBLY_DIRECTIVE
%token IDENTIFIER I_CONSTANT H_CONSTANT O_CONSTANT F_CONSTANT_INT F_CONSTANT_DECIMAL STRING_LITERAL C_CONSTANT
%token ELLIPSIS RIGHT_ASSIGN LEFT_ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token RIGHT_OP LEFT_OP INC_OP DEC_OP INHERITANCE_OP PTR_OP AND_OP OR_OP LE_OP GE_OP EQ_OP NE_OP
%token SEMICOLON LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET COMMA COLON EQ LEFT_BRACKET RIGHT_BRACKET LEFT_THIRD_BRACKET RIGHT_THIRD_BRACKET
%token DOT LOGICAL_AND NOT LOGICAL_NOT MINUS PLUS MULTIPLY DIVIDE MOD LESS GREATER EXPONENT LOGICAL_OR QUESTION
%token NEWLINE ERROR
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%left LOGICAL_OR
%left LOGICAL_AND
%left EQ_OP NE_OP
%left LESS GREATER LE_OP GE_OP
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%left COMMA
%right EQ
%%

program:
    translation_unit
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
    type_specifier IDENTIFIER LEFT_BRACKET parameter_list RIGHT_BRACKET compound_statement
    ;

parameter_list:
    parameter_declaration
    | parameter_list COMMA parameter_declaration
    ;

parameter_declaration:
    type_specifier IDENTIFIER
    ;

declaration:
    type_specifier init_declarator_list SEMICOLON
    ;

init_declarator_list:
    init_declarator
    | init_declarator_list COMMA init_declarator
    ;

init_declarator:
    IDENTIFIER
    | IDENTIFIER EQ constant_expression
    ;

constant_expression:
    I_CONSTANT | H_CONSTANT | O_CONSTANT | F_CONSTANT_INT | F_CONSTANT_DECIMAL | C_CONSTANT | STRING_LITERAL
    ;

type_specifier:
    AUTO | CHAR | CONST | DOUBLE | ENUM | EXTERN | FLOAT | INT | LONG | REGISTER | SHORT | SIGNED | STATIC | STRUCT
    | TYPEDEF | UNION | UNSIGNED | VOID | VOLATILE | CLASS
    ;

compound_statement:
    LEFT_CURLY_BRACKET statement_list RIGHT_CURLY_BRACKET
    ;

statement_list:
    statement
    | statement_list statement
    ;

statement:
    expression_statement
    | compound_statement
    | selection_statement
    | iteration_statement
    | jump_statement
    ;

expression_statement:
    expression SEMICOLON
    | SEMICOLON
    ;

expression:
    assignment_expression
    | expression COMMA assignment_expression %prec COMMA
    ;

assignment_expression:
    IDENTIFIER EQ assignment_expression
    | logical_or_expression
    ;

logical_or_expression:
    logical_and_expression
    | logical_or_expression LOGICAL_OR logical_and_expression
    ;

logical_and_expression:
    equality_expression
    | logical_and_expression LOGICAL_AND equality_expression
    ;

equality_expression:
    relational_expression
    | equality_expression EQ_OP relational_expression
    | equality_expression NE_OP relational_expression
;

relational_expression:
    additive_expression
    | relational_expression LESS additive_expression
    | relational_expression GREATER additive_expression
    | relational_expression LE_OP additive_expression
    | relational_expression GE_OP additive_expression
;

additive_expression:
    multiplicative_expression
    | additive_expression PLUS multiplicative_expression
    | additive_expression MINUS multiplicative_expression
    ;

multiplicative_expression:
    primary_expression
    | multiplicative_expression MULTIPLY primary_expression
    | multiplicative_expression DIVIDE primary_expression
    | multiplicative_expression MOD primary_expression
    ;

primary_expression:
    IDENTIFIER
    | constant_expression
    | LEFT_BRACKET expression RIGHT_BRACKET
    ;

selection_statement:
    IF LEFT_BRACKET expression RIGHT_BRACKET statement %prec LOWER_THAN_ELSE
    | IF LEFT_BRACKET expression RIGHT_BRACKET statement ELSE statement
    | SWITCH LEFT_BRACKET expression RIGHT_BRACKET statement
    ;

iteration_statement:
    WHILE LEFT_BRACKET expression RIGHT_BRACKET statement
    | DO statement WHILE LEFT_BRACKET expression RIGHT_BRACKET SEMICOLON
    | FOR LEFT_BRACKET expression_statement expression_statement expression RIGHT_BRACKET statement
    ;

jump_statement:
    RETURN expression SEMICOLON
    | RETURN SEMICOLON
    | BREAK SEMICOLON
    | CONTINUE SEMICOLON
    | GOTO IDENTIFIER SEMICOLON
    ;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
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
    yyparse();    // Call the parser

    fclose(file); // Close file after parsing
    printf("Parsing completed successfully.\n");
    return 0;
}


