%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"  

// External declarations 
extern int yylex();
extern int yyparse();
extern int yylineno;
extern YYSTYPE yylval;
extern FILE *yyin;
int has_error=0;

void yyerror(const char *msg);

#define MAX_PARSE_SYMBOLS 10000

typedef struct {
    char *token;
    char *type;       // Data type (int, float, etc.)
} ParseSymbol;

ParseSymbol parseSymbolTable[MAX_PARSE_SYMBOLS];
int parseSymbolCount = 0;

// Function to add an entry to the parser symbol table
void addParseSymbol(const char *token, const char *type) {
    if (parseSymbolCount >= MAX_PARSE_SYMBOLS) return;

    parseSymbolTable[parseSymbolCount].token = strdup(token);
    parseSymbolTable[parseSymbolCount].type = strdup(type);
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



%} 

/* Token definitions */
%union {
    int intval;
    char* strval;
}
%token <strval> AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO
%token <strval> IF INT LONG REGISTER RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF UNION UNSIGNED TYPE_NAME
%token <strval> VOID VOLATILE WHILE UNTIL CLASS PRIVATE PUBLIC PROTECTED ASSEMBLY_DIRECTIVE
%token <strval> IDENTIFIER I_CONSTANT H_CONSTANT O_CONSTANT F_CONSTANT HEX_F_CONSTANT STRING_LITERAL CHAR_CONSTANT
%token <strval> ELLIPSIS RIGHT_ASSIGN LEFT_ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token <strval> RIGHT_OP LEFT_OP INC_OP DEC_OP INHERITANCE_OP PTR_OP LOGICAL_AND LOGICAL_OR LE_OP GE_OP EQ_OP NE_OP
%token <strval> SEMICOLON LEFT_CURLY RIGHT_CURLY LEFT_PAREN RIGHT_PAREN LEFT_SQUARE RIGHT_SQUARE COMMA COLON ASSIGN DOT QUESTION POUND
%token <strval> NOT BITWISE_NOT BITWISE_XOR BITWISE_OR BITWISE_AND MINUS PLUS MULTIPLY DIVIDE MOD LESS GREATER
%token <strval> NEWLINE ERROR SINGLE_QUOTE DOUBLE_QUOTE 
%type <strval> error_case type_specifier struct_or_union struct_or_union_specifier declaration_specifiers declaration storage_class_specifier type_qualifier
%type <strval> enum_specifier init_declarator_list init_declarator declarator direct_declarator pointer specifier_qualifier_list struct_declarator_list
%type <strval> struct_declarator type_qualifier_list parameter_declaration class_declaration class_declaration_list class_specifier access_specifier
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE 
%nonassoc LOW_PREC
%nonassoc HIGH_PREC
%start translation_unit

%%

error_case:
    I_CONSTANT IDENTIFIER { yyerror("Invalid Identifier"); has_error=1; yyclearin;}
    | DIVIDE MULTIPLY { yyerror("Unterminated Comment"); has_error=1; yyclearin;}
    | ERROR { has_error=1; yyclearin;}
    | error {  has_error=1; yyclearin;}
    ;

primary_expression:
    IDENTIFIER
    | I_CONSTANT
    | O_CONSTANT
    | H_CONSTANT
    | F_CONSTANT
    | CHAR_CONSTANT
    | STRING_LITERAL
    ;

postfix_expression:
    primary_expression
    | LEFT_PAREN expression RIGHT_PAREN
    | postfix_expression LEFT_PAREN expression RIGHT_PAREN
    | postfix_expression LEFT_PAREN RIGHT_PAREN 
    | postfix_expression LEFT_SQUARE expression RIGHT_SQUARE
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
    | declaration_specifiers init_declarator_list SEMICOLON  {
        char *type = strdup($1);
        char *variables = strdup($2);

        char formattedType[256];

        if (strncmp(type, "enum ", 5) == 0) {
            sprintf(formattedType, "enum(%s)", type + 5);  
        } else if (strncmp(type, "struct ", 7) == 0) {
            sprintf(formattedType, "struct(%s)", type + 7); 
        } else if (strncmp(type, "class ", 6) == 0) {   // Handle class type
            sprintf(formattedType, "class(%s)", type + 6);  
        } else {
            strcpy(formattedType, type); 
        }

        char *token = strtok(variables, ", ");
        while (token != NULL) {
            char fullType[256];
            char *varName = token;
            int starCount = 0;

            while (*varName == '*') {
                starCount++;
                varName++; 
            }
            sprintf(fullType, "%s%.*s", formattedType, starCount, "****************");

            addParseSymbol(varName, fullType);
            token = strtok(NULL, ", ");
        }
    }
    ;

declaration_specifiers:
    storage_class_specifier {$$ = strdup($1);}   
    | storage_class_specifier declaration_specifiers {
        char *newType = (char *)malloc(strlen($1) + strlen($2) + 2);
        sprintf(newType, "%s %s", $1, $2);
        $$ = newType;
    }
	| type_specifier %prec HIGH_PREC                                    {$$ = strdup($1);}                                             
	| type_specifier declaration_specifiers %prec LOW_PREC  {
        char *newType = (char *)malloc(strlen($1) + strlen($2) + 2);
        sprintf(newType, "%s %s", $1, $2);
        $$ = newType;
    }
	| type_qualifier    {$$ = strdup($1);}   
	| type_qualifier declaration_specifiers {
        char *newType = (char *)malloc(strlen($1) + strlen($2) + 2);
        sprintf(newType, "%s %s", $1, $2);
        $$ = newType;
    }
    ;

init_declarator_list:
    init_declarator {$$ = strdup($1);}
    | init_declarator_list COMMA init_declarator {
        $$ = (char *)malloc(strlen($1) + strlen($3) + 3);  
        sprintf($$, "%s, %s", $1, $3);  
    }
    ;

init_declarator:
    declarator {$$ = strdup($1);}
    | declarator ASSIGN initializer {$$ = strdup($1);}
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
	| struct_or_union_specifier  {
        $$ = (char *)malloc(strlen($1) + strlen("struct ") + 1);
        sprintf($$, "struct %s", $1);
    }
	| enum_specifier  {
        $$ = (char *)malloc(strlen($1) + strlen("enum ") + 1);
        sprintf($$, "enum %s", $1);
    }
    | class_specifier {
        $$ = (char *)malloc(strlen($1) + strlen("class ") + 1);
        sprintf($$, "class %s", $1);
    }
	;

struct_or_union_specifier:
    struct_or_union IDENTIFIER LEFT_CURLY struct_declaration_list RIGHT_CURLY  {
        addParseSymbol($2, $1); 
        $$ = strdup($2);
    }
	| struct_or_union LEFT_CURLY struct_declaration_list RIGHT_CURLY    {
        $$ = strdup("unidentified");
    }
	| struct_or_union IDENTIFIER {
        addParseSymbol($2, $1); 
        $$ = strdup($2);
    }                                                 
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
    CLASS IDENTIFIER LEFT_CURLY class_declaration_list RIGHT_CURLY {
        addParseSymbol($2, "class");
        $$ = strdup($2);
    }
    | CLASS IDENTIFIER INHERITANCE_OP init_declarator_list LEFT_CURLY class_declaration_list RIGHT_CURLY {
        addParseSymbol($2, "class");
        $$ = strdup($2);
    }
    | CLASS LEFT_CURLY class_declaration_list RIGHT_CURLY {
        $$ = strdup("anonymous_class");
    }
    | CLASS IDENTIFIER {
        $$ = strdup($2);
    }
    ;

class_declaration_list:
    class_declaration
    | class_declaration_list class_declaration
    ;

class_declaration:
    access_specifier LEFT_CURLY struct_declaration_list RIGHT_CURLY 
    ;

access_specifier:
    PUBLIC  { $$ = strdup("public"); }
    | PRIVATE { $$ = strdup("private"); }
    | PROTECTED { $$ = strdup("protected"); }
    ;

struct_declaration:
    specifier_qualifier_list struct_declarator_list SEMICOLON {
        char *type = strdup($1);
        char *variables = strdup($2);

        char *token = strtok(variables, ", ");
        while (token != NULL) {
            char fullType[256];
            char *varName = token;
            int starCount = 0;

            while (*varName == '*') {
                starCount++;
                varName++; // Move past '*'
            }

            sprintf(fullType, "%s%.*s", type, starCount, "****************");

            addParseSymbol(varName, fullType); 
            token = strtok(NULL, ", ");
        }
    }
    ;

specifier_qualifier_list:
    type_specifier specifier_qualifier_list {
        $$ = (char *)malloc(strlen($1) + strlen($2) + 2);
        sprintf($$, "%s %s", $1, $2); // Append qualifier to type
    }
	| type_specifier                                { $$ = strdup($1);}
	| type_qualifier specifier_qualifier_list {
        $$ = (char *)malloc(strlen($1) + strlen($2) + 2);
        sprintf($$, "%s %s", $1, $2); // Append qualifier to type
    }     
	| type_qualifier                                { $$ = strdup($1);}
	;

struct_declarator_list:
    struct_declarator { $$ = strdup($1); }
	| struct_declarator_list COMMA struct_declarator {
        $$ = (char *)malloc(strlen($1) + strlen($3) + 3);
        sprintf($$, "%s, %s", $1, $3);
    }
	;

struct_declarator:
    declarator
	| COLON conditional_expression
	| declarator COLON conditional_expression
	;

enum_specifier:
    ENUM LEFT_CURLY enumerator_list RIGHT_CURLY                 { $$ = strdup("unidentified");}
	| ENUM IDENTIFIER LEFT_CURLY enumerator_list RIGHT_CURLY    {
        addParseSymbol($2, "enum");
        $$ = strdup($2);
    }
	| ENUM IDENTIFIER  {
        addParseSymbol($2, "enum");
        $$ = strdup($2);
    }                                        
	;

enumerator_list:
    enumerator
    | enumerator_list COMMA enumerator
    ;

enumerator:
    IDENTIFIER {
        addParseSymbol($1, "int");  
    }
    | IDENTIFIER ASSIGN conditional_expression {
        addParseSymbol($1, "int");  
    }
    ;

type_qualifier:
    CONST  { $$ = strdup($1); }
    | VOLATILE  { $$ = strdup($1); }
    ;

declarator:
    pointer direct_declarator {
        char *fullType = (char *)malloc(strlen($1) + strlen($2) + 1);
        sprintf(fullType, "%s%s", $1, $2); 
        $$ = fullType;
    }
    | direct_declarator  { $$ = strdup($1);}
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
    MULTIPLY { $$ = strdup("*"); }
    | MULTIPLY type_qualifier_list { 
        $$ = (char *)malloc(strlen("* ") + strlen($2) + 1);
        sprintf($$, "* %s", $2); 
    }
    | MULTIPLY pointer { 
        $$ = (char *)malloc(strlen($2) + 2);
        sprintf($$, "*%s", $2);  
    }
    | MULTIPLY type_qualifier_list pointer { 
        $$ = (char *)malloc(strlen($3) + strlen($2) + 3);
        sprintf($$, "* %s %s", $2, $3);  
    }
    ;

type_qualifier_list:
    type_qualifier  { $$ = strdup($1); }
    | type_qualifier_list type_qualifier  { 
        $$ = (char *)malloc(strlen($1) + strlen($2) + 2);
        sprintf($$, "%s %s", $1, $2);  
    }
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
    declaration_specifiers declarator { 
        char *fullType = strdup($1);  
        char *varName = strdup($2);   
       
        if ($2[0] == '*') {  
            fullType = (char *)malloc(strlen($1) + 2);
            sprintf(fullType, "%s*", $1);
            varName = strdup($2 + 1);  
        }

        $$ = fullType;
        addParseSymbol(varName, fullType);  
    }
	| declaration_specifiers abstract_declarator
	| declaration_specifiers                        { $$ = strdup($1); }
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
    | LEFT_CURLY initializer_list RIGHT_CURLY
    | LEFT_CURLY initializer_list COMMA RIGHT_CURLY
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
    LEFT_CURLY RIGHT_CURLY
    | LEFT_CURLY statement_list RIGHT_CURLY
    | LEFT_CURLY declaration_list RIGHT_CURLY
    | LEFT_CURLY declaration_list statement_list RIGHT_CURLY
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
    | error_case skip_until_semicolon 
    | error_case skip_until_semicolon 
	;

function_definition:
    declaration_specifiers declarator declaration_list compound_statement {
        char functionType[256];  
        sprintf(functionType, "Function(%s)", $1);  
        addParseSymbol($2, functionType);
    }
    | declaration_specifiers declarator compound_statement {
        char functionType[256];  
        sprintf(functionType, "Function(%s)", $1);  
        addParseSymbol($2, functionType);
    }
    | declarator declaration_list compound_statement {
        char functionType[256];  
        sprintf(functionType, "Function(%s)", "int"); // Default return type
        addParseSymbol($1, functionType);
    }
    | declarator compound_statement {
        char functionType[256];  
        sprintf(functionType, "Function(%s)", "int"); // Default return type
        addParseSymbol($1, functionType);
    }
    ;

skip_until_semicolon:
    SEMICOLON { yyclearin; }  // Stop at semicolon and reset error handling
    | error 
    | skip_until_semicolon error  // Consume any unexpected token
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
    
    yyparse();    // Call the parser
    fclose(file); // Close file after parsing
    if(!has_error)printParseSymbolTable();
    printf("Parsing completed successfully.\n");
    return 0;
}
