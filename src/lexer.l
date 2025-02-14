%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int line_no = 1;  // Line number tracker
int has_error=0;

// Symbol table structure
#define MAX_SYMBOLS 10000
typedef struct {
    char *lexeme;  // Pointer to char for storing strings
    char *token;   // Pointer to char for storing token types
} Symbol;

Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

extern int yylex();  // Lexical analyzer function

// Function to handle errors
void report_error(const char *msg, const char *symbol) {
    has_error=1;
    printf("Error at line %d: %s %s\n", line_no, msg, symbol);
}

// Function to add symbol to the symbol table
void addToSymbolTable(const char *lexeme, const char *type) {
    if (symbolCount >= MAX_SYMBOLS) {
        report_error("Symbol table is full.", lexeme);
        return;  // Avoid adding more symbols if table is full
    }

    symbolTable[symbolCount].token = strdup(type);
    if (symbolTable[symbolCount].token == NULL) {
        report_error("Memory allocation failed for token.", lexeme);
        return;  // Handle memory allocation failure
    }

    symbolTable[symbolCount].lexeme = strdup(lexeme);
    if (symbolTable[symbolCount].lexeme == NULL) {
        report_error("Memory allocation failed for lexeme.", lexeme);
        return;  // Handle memory allocation failure
    }

    symbolCount++;
}

// Function to print the symbol table in a structured format
void printSymbolTable() {
    printf("\n+----------------------+----------------------+\n");
    printf("|      Lexeme          |        Token         |\n");
    printf("+----------------------+----------------------+\n");

    for (int i = 0; i < symbolCount; i++) {
        printf("| %-20s | %-20s |\n", symbolTable[i].lexeme, symbolTable[i].token);
    }

    printf("+----------------------+----------------------+\n");
}

// Function to check identifier length
void checkIdentifierLength(const char *identifier) {
    if (strlen(identifier) > 32) {
        report_error("Identifier exceeds maximum length (32 characters):", identifier);
    }
}

%}

%option noyywrap

/* Keywords for the C language */
AUTO          "auto"
BREAK         "break"
CASE          "case"
CHAR          "char"
CONST         "const"
CONTINUE      "continue"
DEFAULT       "default"
DO            "do"
DOUBLE        "double"
ELSE          "else"
ENUM          "enum"
EXTERN        "extern"
FLOAT         "float"
FOR           "for"
GOTO          "goto"
IF            "if"
INT           "int"
LONG          "long"
REGISTER      "register"
RETURN        "return"
SHORT         "short"
SIGNED        "signed"
SIZEOF        "sizeof"
STATIC        "static"
STRUCT        "struct"
SWITCH        "switch"
TYPEDEF       "typedef"
UNION         "union"
UNSIGNED      "unsigned"
VOID          "void"
VOLATILE      "volatile"
WHILE         "while"
UNTIL         "until"
CLASS         "class"
PRIVATE       "private"
PUBLIC        "public"
PROTECTED     "protected"
ASSEMBLY_DIRECTIVE "asm_directive"

/* Regular expression for identifiers, constants, literals */
IDENTIFIER    [A-Za-z_][A-Za-z0-9_]*
I_CONSTANT    [0-9]+[uU]?[lL]{0,2}
H_CONSTANT    0[xX][0-9a-fA-F]+[uU]?[lL]{0,2}
O_CONSTANT    0[0-7]+[uU]?[lL]{0,2}
F_CONSTANT_INT [0-9]+([eE][+-]?[0-9]+)?[fFlL]?
F_CONSTANT_DECIMAL   [0-9]*\.[0-9]+([eE][+-]?[0-9]+)?[fFlL]?
STRING_LITERAL \"((\\.)|[^\"\\])*\"
C_CONSTANT    \'([^'\\n]|(\\.))\'

/* Operators and special symbols */
ELLIPSIS      \.\.\.
RIGHT_ASSIGN  >>=       
LEFT_ASSIGN   <<=       
ADD_ASSIGN    \+=       
SUB_ASSIGN    -=       
MUL_ASSIGN    \*=      
DIV_ASSIGN    "/="      
MOD_ASSIGN    %=       
AND_ASSIGN    &=       
XOR_ASSIGN    ^=       
OR_ASSIGN     \|=      

RIGHT_OP      >>       
LEFT_OP       <<       
INC_OP        \+\+     
DEC_OP        --       
INHERITANCE_OP <-     
PTR_OP        ->       
AND_OP        &      
OR_OP         \|     
LE_OP         <=       
GE_OP         >=       
EQ_OP         ==       
NE_OP         !=       

SEMICOLON     ;        
LEFT_CURLY_BRACKET  \{  
RIGHT_CURLY_BRACKET \}  
COMMA         ,        
COLON         :      
EQ            =        
LEFT_BRACKET   \(      
RIGHT_BRACKET  \)      
LEFT_THIRD_BRACKET \[  
RIGHT_THIRD_BRACKET \]  

DOT           \.       
LOGICAL_AND   &&       
NOT           !        
LOGICAL_NOT   ~        
MINUS         -        
PLUS          \+       
MULTIPLY      \*       
DIVIDE        "/"       
MOD           %        
LESS          <        
GREATER       >        
EXPONENT      \^       
LOGICAL_OR    \|\|     
QUESTION      \? 

SINGLE_LINE_COMMENT "//"[^\n]*\n          
MULTI_LINE_COMMENT  "/*"[^*]*"*"+[^/]*"/" 

%%

{AUTO}                { addToSymbolTable(yytext, "keyword"); }
{BREAK}               { addToSymbolTable(yytext, "keyword"); }
{CASE}                { addToSymbolTable(yytext, "keyword"); }
{CHAR}                { addToSymbolTable(yytext, "keyword"); }
{CONST}               { addToSymbolTable(yytext, "keyword"); }
{CONTINUE}            { addToSymbolTable(yytext, "keyword"); }
{DEFAULT}             { addToSymbolTable(yytext, "keyword"); }
{DO}                  { addToSymbolTable(yytext, "keyword"); }
{DOUBLE}              { addToSymbolTable(yytext, "keyword"); }
{ELSE}                { addToSymbolTable(yytext, "keyword"); }
{ENUM}                { addToSymbolTable(yytext, "keyword"); }
{EXTERN}              { addToSymbolTable(yytext, "keyword"); }
{FLOAT}               { addToSymbolTable(yytext, "keyword"); }
{FOR}                 { addToSymbolTable(yytext, "keyword"); }
{GOTO}                { addToSymbolTable(yytext, "keyword"); }
{IF}                  { addToSymbolTable(yytext, "keyword"); }
{INT}                 { addToSymbolTable(yytext, "keyword"); }
{LONG}                { addToSymbolTable(yytext, "keyword"); }
{REGISTER}            { addToSymbolTable(yytext, "keyword"); }
{RETURN}              { addToSymbolTable(yytext, "keyword"); }
{SHORT}               { addToSymbolTable(yytext, "keyword"); }
{SIGNED}              { addToSymbolTable(yytext, "keyword"); }
{STATIC}              { addToSymbolTable(yytext, "keyword"); }
{STRUCT}              { addToSymbolTable(yytext, "keyword"); }
{SWITCH}              { addToSymbolTable(yytext, "keyword"); }
{TYPEDEF}             { addToSymbolTable(yytext, "keyword"); }
{UNION}               { addToSymbolTable(yytext, "keyword"); }
{UNSIGNED}            { addToSymbolTable(yytext, "keyword"); }
{VOID}                { addToSymbolTable(yytext, "keyword"); }
{VOLATILE}            { addToSymbolTable(yytext, "keyword"); }
{WHILE}               { addToSymbolTable(yytext, "keyword"); }
{UNTIL}               { addToSymbolTable(yytext, "keyword"); }
{CLASS}               { addToSymbolTable(yytext, "keyword"); }
{PRIVATE}             { addToSymbolTable(yytext, "keyword"); }
{PUBLIC}              { addToSymbolTable(yytext, "keyword"); }
{PROTECTED}           { addToSymbolTable(yytext, "keyword"); }
{ASSEMBLY_DIRECTIVE}  { addToSymbolTable(yytext, "keyword"); }

{IDENTIFIER}   { checkIdentifierLength(yytext); addToSymbolTable(yytext, "identifier"); }
{H_CONSTANT}   { addToSymbolTable(yytext, "hexadecimal_constant"); }
{O_CONSTANT}   { addToSymbolTable(yytext, "octal_constant"); }
{I_CONSTANT}   { addToSymbolTable(yytext, "integer_constant"); }
{C_CONSTANT}   { addToSymbolTable(yytext, "char_constant"); }
{F_CONSTANT_DECIMAL}   { addToSymbolTable(yytext, "float_constant"); }
{F_CONSTANT_INT}   { addToSymbolTable(yytext, "float_constant"); }
{STRING_LITERAL}   { addToSymbolTable(yytext, "string_literal"); }

{RIGHT_ASSIGN}     { addToSymbolTable(yytext, "operator"); }
{LEFT_ASSIGN}      { addToSymbolTable(yytext, "operator"); }
{ADD_ASSIGN}       { addToSymbolTable(yytext, "operator"); }
{SUB_ASSIGN}       { addToSymbolTable(yytext, "operator"); }
{MUL_ASSIGN}       { addToSymbolTable(yytext, "operator"); }
{DIV_ASSIGN}       { addToSymbolTable(yytext, "operator"); } 
{MOD_ASSIGN}       { addToSymbolTable(yytext, "operator"); }
{AND_ASSIGN}       { addToSymbolTable(yytext, "operator"); }
{XOR_ASSIGN}       { addToSymbolTable(yytext, "operator"); }
{OR_ASSIGN}        { addToSymbolTable(yytext, "operator"); }
{DIVIDE}           { addToSymbolTable(yytext, "operator"); }
{LOGICAL_AND}     { addToSymbolTable(yytext, "operator"); }
{NOT}             { addToSymbolTable(yytext, "operator"); }
{LOGICAL_NOT}     { addToSymbolTable(yytext, "operator"); }
{MINUS}           { addToSymbolTable(yytext, "operator"); }
{PLUS}            { addToSymbolTable(yytext, "operator"); }
{MULTIPLY}        { addToSymbolTable(yytext, "operator"); }
{LOGICAL_OR}      { addToSymbolTable(yytext, "operator"); }

{RIGHT_OP}        { addToSymbolTable(yytext, "operator"); }
{LEFT_OP}         { addToSymbolTable(yytext, "operator"); }
{INC_OP}          { addToSymbolTable(yytext, "operator"); }
{DEC_OP}          { addToSymbolTable(yytext, "operator"); }
{INHERITANCE_OP}  { addToSymbolTable(yytext, "operator"); }
{PTR_OP}          { addToSymbolTable(yytext, "operator"); }
{AND_OP}          { addToSymbolTable(yytext, "operator"); }
{OR_OP}           { addToSymbolTable(yytext, "operator"); }
{LE_OP}           { addToSymbolTable(yytext, "operator"); }
{GE_OP}           { addToSymbolTable(yytext, "operator"); }
{EQ_OP}           { addToSymbolTable(yytext, "operator"); }
{NE_OP}           { addToSymbolTable(yytext, "operator"); }

{SEMICOLON}           { addToSymbolTable(yytext, "symbol"); }
{LEFT_CURLY_BRACKET}  { addToSymbolTable(yytext, "symbol"); }
{RIGHT_CURLY_BRACKET} { addToSymbolTable(yytext, "symbol"); }
{COMMA}               { addToSymbolTable(yytext, "symbol"); }
{COLON}               { addToSymbolTable(yytext, "symbol"); }
{EQ}                  { addToSymbolTable(yytext, "symbol"); }
{LEFT_BRACKET}        { addToSymbolTable(yytext, "symbol"); }
{RIGHT_BRACKET}       { addToSymbolTable(yytext, "symbol"); }
{LEFT_THIRD_BRACKET}  { addToSymbolTable(yytext, "symbol"); }
{RIGHT_THIRD_BRACKET} { addToSymbolTable(yytext, "symbol"); }

{DOT}              { addToSymbolTable(yytext, "symbol"); }

{MOD}              { addToSymbolTable(yytext, "operator"); }
{LESS}             { addToSymbolTable(yytext, "operator"); }
{GREATER}          { addToSymbolTable(yytext, "operator"); }
{EXPONENT}         { addToSymbolTable(yytext, "operator"); }

{QUESTION}         { addToSymbolTable(yytext, "operator"); }

{SINGLE_LINE_COMMENT}       { }
{MULTI_LINE_COMMENT}        { }

[ \t]+             { /* Ignore whitespace */ }

\n                 { line_no++; }

<<EOF>>            { return 0; }

.                  { report_error("Unrecognized symbol", yytext); }

%%


