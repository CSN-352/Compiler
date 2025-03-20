%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h" 
#include "symbol_table.h" 
#include "ast.h"
#include "expression.h"

// External declarations 
extern "C" int yylex();
extern int yyparse();
extern int yylineno;
extern YYSTYPE yylval;
extern FILE *yyin;
int has_error=0;

void yyerror(const char *msg);
/*
#define MAX_PARSE_SYMBOLS 10000

typedef struct {
    char *token;
    char *type;       // Data type (int, float, etc.)
} ParseSymbol;

ParseSymbol parseSymbolTable[MAX_PARSE_SYMBOLS];

int parseSymbolCount = 0;

// Function to add an entry to the parser symbol table
void addParseSymbol(const char *token, Type type) {
    if (parseSymbolCount >= MAX_PARSE_SYMBOLS) return;

    symbolTable.insert(token, type, 1000);
    // symbolTable.print(); 

    parseSymbolTable[parseSymbolCount].token = strdup(token);
    parseSymbolTable[parseSymbolCount].type = type;
    parseSymbolCount++;
}

// Function to print the symbol table after parsing
void printParseSymbolTable() {
    printf("\nParser Symbol Table:\n");
    printf("--------------------------------------------------------------\n");
    printf("| %-20s | %-30s |\n", "Token", "Type");
    printf("--------------------------------------------------------------\n");

    for (int i = 0; i < parseSymbolCount; i++) {
        printf("| %-20s | %-30s |\n",
               parseSymbolTable[i].token,
               parseSymbolTable[i].type);
    }

    printf("--------------------------------------------------------------\n");
}
*/


%} 

%code requires {
    #include "ast.h"
    #include "expression.h"
    #include "symbol_table.h"
}

/* Token definitions */
%union {
    Node* node;
	Terminal* terminal;
    Identifier* identifier;
    Constant* constant;
	StringLiteral* string_literal;
    Expression* expression;
    ArgumentExpressionList* argument_expression_list;
    int intval;
    char* strval;
}

%token <identifier> IDENTIFIER
%token <constant> I_CONSTANT F_CONSTANT CHAR_CONSTANT
%token <string_literal> STRING_LITERAL
%token <terminal> INC_OP DEC_OP PTR_OP DOT
%type <expression> expression assignment_expression primary_expression postfix_expression
%type <argument_expression_list> argument_expression_list 
%token <strval> AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO
%token <strval> IF INT LONG REGISTER RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF UNION UNSIGNED TYPE_NAME
%token <strval> VOID VOLATILE WHILE UNTIL CLASS PRIVATE PUBLIC PROTECTED ASSEMBLY_DIRECTIVE
%token <strval> ELLIPSIS RIGHT_ASSIGN LEFT_ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token <strval> RIGHT_OP LEFT_OP INHERITANCE_OP LOGICAL_AND LOGICAL_OR LE_OP GE_OP EQ_OP NE_OP
%token <strval> SEMICOLON LEFT_CURLY RIGHT_CURLY LEFT_PAREN RIGHT_PAREN LEFT_SQUARE RIGHT_SQUARE COMMA COLON ASSIGN QUESTION
%token <strval> NOT BITWISE_NOT BITWISE_XOR BITWISE_OR BITWISE_AND MINUS PLUS MULTIPLY DIVIDE MOD LESS GREATER
%token <strval> NEWLINE ERROR SINGLE_QUOTE DOUBLE_QUOTE 
%type <strval> error_case type_specifier struct_or_union struct_or_union_specifier declaration_specifiers declaration storage_class_specifier type_qualifier
%type <strval> enum_specifier init_declarator_list init_declarator declarator direct_declarator pointer specifier_qualifier_list struct_declarator_list
%type <strval> struct_declarator type_qualifier_list parameter_declaration class_declaration class_declaration_list class_specifier access_specifier
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE 
%nonassoc LOW_PREC
%nonassoc HIGH_PREC
%type <node> translation_unit external_declaration 
%debug
%start translation_unit

%%

error_case:
    I_CONSTANT IDENTIFIER { yyerror("Invalid Identifier"); has_error=1; yyclearin;}
    | DIVIDE MULTIPLY { yyerror("Unterminated Comment"); has_error=1; yyclearin;}
    | ERROR { has_error=1; yyclearin;}
    | error {  has_error=1; yyclearin;}
    ;

primary_expression:
    IDENTIFIER {$$ = create_primary_expression($1);}
    | I_CONSTANT {$$ = create_primary_expression($1);}
    | F_CONSTANT {$$ = create_primary_expression($1);}
    | CHAR_CONSTANT {$$ = create_primary_expression($1);}
    | STRING_LITERAL {$$ = create_primary_expression($1);}
    ;

argument_expression_list:
    assignment_expression {$$ = create_argument_expression_list($1);}
    | argument_expression_list COMMA assignment_expression {$$ = create_argument_expression_list($1, $3);}
    ;

postfix_expression:
    primary_expression 
    | postfix_expression LEFT_SQUARE expression RIGHT_SQUARE
    | postfix_expression LEFT_PAREN argument_expression_list RIGHT_PAREN
    | postfix_expression LEFT_PAREN RIGHT_PAREN 
    | postfix_expression DOT IDENTIFIER
    | postfix_expression PTR_OP IDENTIFIER
    | postfix_expression INC_OP {$$ = create_postfix_expression($1,$2);}
    | postfix_expression DEC_OP {$$ = create_postfix_expression($1,$2);}
    ;

unary_expression:
    postfix_expression
    | INC_OP unary_expression
    | DEC_OP unary_expression
    | unary_operator cast_expression
    | SIZEOF unary_expression
    | SIZEOF LEFT_PAREN type_name RIGHT_PAREN
    ;

unary_operator:
    BITWISE_AND
    | MULTIPLY
    | PLUS
    | MINUS
    | NOT
    | BITWISE_NOT
    ;

cast_expression: 
    unary_expression
    | LEFT_PAREN type_name RIGHT_PAREN cast_expression
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
    | and_expression BITWISE_AND equality_expression
    ;

xor_expression:
    and_expression
    | xor_expression BITWISE_XOR and_expression
    ;

or_expression:
    xor_expression
    | or_expression BITWISE_OR xor_expression
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
    ASSIGN
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
    | error_case skip_until_semicolon SEMICOLON
    ;

declaration_specifiers:
    storage_class_specifier 
    | storage_class_specifier declaration_specifiers 
	| type_specifier %prec HIGH_PREC                                                                       
	| type_specifier declaration_specifiers %prec LOW_PREC  
	| type_qualifier    
	| type_qualifier declaration_specifiers 
    ;

init_declarator_list:
    init_declarator 
    | init_declarator_list COMMA init_declarator 
    ;

init_declarator:
    declarator 
    | declarator ASSIGN initializer
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
    | TYPE_NAME
	| struct_or_union_specifier  
	| enum_specifier  
    | class_specifier 
	;

struct_or_union_specifier:
    struct_or_union IDENTIFIER add_left_curly struct_declaration_list add_right_curly  
	| struct_or_union add_left_curly struct_declaration_list add_right_curly    
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

class_specifier:
    CLASS IDENTIFIER add_left_curly class_declaration_list add_right_curly 
    | CLASS IDENTIFIER INHERITANCE_OP init_declarator_list add_left_curly class_declaration_list add_right_curly 
    | CLASS add_left_curly class_declaration_list add_right_curly 
    | CLASS IDENTIFIER 
    ;

class_declaration_list:
    class_declaration
    | class_declaration_list class_declaration
    ;


class_declaration:
    access_specifier add_left_curly translation_unit add_right_curly 
    ;

access_specifier:
    PUBLIC  
    | PRIVATE 
    | PROTECTED 
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
    ENUM add_left_curly enumerator_list add_right_curly                 
	| ENUM IDENTIFIER add_left_curly enumerator_list add_right_curly    
	| ENUM IDENTIFIER                        
	;

enumerator_list:
    enumerator
    | enumerator_list COMMA enumerator
    ;

enumerator:
    IDENTIFIER
    | IDENTIFIER ASSIGN conditional_expression 
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
    | direct_declarator LEFT_SQUARE conditional_expression RIGHT_SQUARE 
	| direct_declarator LEFT_SQUARE RIGHT_SQUARE 
    | LEFT_PAREN declarator RIGHT_PAREN 
    | direct_declarator LEFT_PAREN parameter_type_list RIGHT_PAREN 
    | direct_declarator LEFT_PAREN identifier_list RIGHT_PAREN 
    | direct_declarator LEFT_PAREN RIGHT_PAREN 
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
    LEFT_PAREN abstract_declarator RIGHT_PAREN
    | LEFT_SQUARE conditional_expression RIGHT_SQUARE
	| LEFT_SQUARE RIGHT_SQUARE
	| LEFT_PAREN RIGHT_PAREN
	| LEFT_PAREN parameter_type_list RIGHT_PAREN
	| direct_abstract_declarator LEFT_PAREN RIGHT_PAREN
	| direct_abstract_declarator LEFT_PAREN parameter_type_list RIGHT_PAREN
	;

initializer:
    assignment_expression
    | add_left_curly initializer_list add_right_curly
    | add_left_curly initializer_list COMMA add_right_curly
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
    | error_case skip_until_semicolon
	;

labeled_statement:
    IDENTIFIER COLON statement
	| CASE conditional_expression COLON statement
	| DEFAULT COLON statement
	;

compound_statement:
    add_left_curly add_right_curly
    | add_left_curly declaration_statement_list add_right_curly
    ;

declaration_statement_list:
    declaration_list
    | statement_list
    | declaration_statement_list declaration_list 
    | declaration_statement_list statement_list
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
    | expression SEMICOLON
    ;

selection_statement:
    IF LEFT_PAREN expression RIGHT_PAREN statement ELSE statement
    | IF LEFT_PAREN expression RIGHT_PAREN statement
    | SWITCH LEFT_PAREN expression RIGHT_PAREN statement

iteration_statement:
    WHILE LEFT_PAREN expression RIGHT_PAREN statement
    | DO statement WHILE LEFT_PAREN expression RIGHT_PAREN SEMICOLON
    | FOR LEFT_PAREN expression_statement expression_statement RIGHT_PAREN statement
    | FOR LEFT_PAREN expression_statement expression_statement expression RIGHT_PAREN statement
    | FOR LEFT_PAREN declaration expression_statement RIGHT_PAREN statement
    | FOR LEFT_PAREN declaration expression_statement expression RIGHT_PAREN statement
    | UNTIL LEFT_PAREN expression RIGHT_PAREN statement
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

skip_until_semicolon:
    SEMICOLON   // Stop at semicolon and reset error handling
    | error 
    | skip_until_semicolon error  // Consume any unexpected token
    ;

add_left_curly:
    LEFT_CURLY {symbolTable.enterScope();}
    | error
    ;

add_right_curly:
    RIGHT_CURLY {symbolTable.exitScope();}
    | error
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

    yyin = file;
#ifdef DEBUG
    yydebug=1;
#endif
    yyparse();    // Call the parser
    fclose(file); // Close file after parsing
    has_error |= symbolTable.has_error();
    // if(!has_error)printParseSymbolTable();
    printf("Parsing completed successfully.\n");
    return 0;
}
