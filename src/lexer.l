%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

int line_no = 1;  // Line number tracker
int has_error=0;
int is_typedef=0;

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

int check_type_name(const char* lexeme) {  
    for (int i = 0; i < symbolCount; i++) {  
        if (strcmp(symbolTable[i].token, "type_name") == 0 && strcmp(symbolTable[i].lexeme, lexeme) == 0) {  
            return 1;
        }  
    }  
    return 0;
}  


// Function to check identifier length
void checkIdentifierLength(const char *identifier) {
    if (strlen(identifier) > 32) {
        report_error("Identifier exceeds maximum length (32 characters):", identifier);
    }
}

%}

%option noyywrap
%option yylineno

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
XOR_OP        "^"    
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

{AUTO}                { addToSymbolTable(yytext, "keyword"); return(AUTO); }
{BREAK}               { addToSymbolTable(yytext, "keyword"); return(BREAK); }
{CASE}                { addToSymbolTable(yytext, "keyword"); return(CASE); }
{CHAR}                { addToSymbolTable(yytext, "keyword"); return(CHAR); }
{CONST}               { addToSymbolTable(yytext, "keyword"); return(CONST); }
{CONTINUE}            { addToSymbolTable(yytext, "keyword"); return(CONTINUE); }
{DEFAULT}             { addToSymbolTable(yytext, "keyword"); return(DEFAULT); }
{DO}                  { addToSymbolTable(yytext, "keyword"); return(DO); }
{DOUBLE}              { addToSymbolTable(yytext, "keyword"); return(DOUBLE); }
{ELSE}                { addToSymbolTable(yytext, "keyword"); return(ELSE); }
{ENUM}                { addToSymbolTable(yytext, "keyword"); return(ENUM); }
{EXTERN}              { addToSymbolTable(yytext, "keyword"); return(EXTERN); }
{FLOAT}               { addToSymbolTable(yytext, "keyword"); return(FLOAT); }
{FOR}                 { addToSymbolTable(yytext, "keyword"); return(FOR); }
{GOTO}                { addToSymbolTable(yytext, "keyword"); return(GOTO); }
{IF}                  { addToSymbolTable(yytext, "keyword"); return(IF); }
{INT}                 { addToSymbolTable(yytext, "keyword"); return(INT); }
{LONG}                { addToSymbolTable(yytext, "keyword"); return(LONG); }
{REGISTER}            { addToSymbolTable(yytext, "keyword"); return(REGISTER); }
{RETURN}              { addToSymbolTable(yytext, "keyword"); return(RETURN); }
{SHORT}               { addToSymbolTable(yytext, "keyword"); return(SHORT); }
{SIGNED}              { addToSymbolTable(yytext, "keyword"); return(SIGNED); }
{STATIC}              { addToSymbolTable(yytext, "keyword"); return(STATIC); }
{STRUCT}              { addToSymbolTable(yytext, "keyword"); return(STRUCT); }
{SWITCH}              { addToSymbolTable(yytext, "keyword"); return(SWITCH); }
{TYPEDEF}             { addToSymbolTable(yytext, "keyword"); is_typedef=1; return(TYPEDEF); }
{UNION}               { addToSymbolTable(yytext, "keyword"); return(UNION); }
{UNSIGNED}            { addToSymbolTable(yytext, "keyword"); return(UNSIGNED); }
{VOID}                { addToSymbolTable(yytext, "keyword"); return(VOID); }
{VOLATILE}            { addToSymbolTable(yytext, "keyword"); return(VOLATILE); }
{WHILE}               { addToSymbolTable(yytext, "keyword"); return(WHILE); }
{UNTIL}               { addToSymbolTable(yytext, "keyword"); return(UNTIL); }
{CLASS}               { addToSymbolTable(yytext, "keyword"); return(CLASS); }
{PRIVATE}             { addToSymbolTable(yytext, "keyword"); return(PRIVATE); }
{PUBLIC}              { addToSymbolTable(yytext, "keyword"); return(PUBLIC); }
{PROTECTED}           { addToSymbolTable(yytext, "keyword"); return(PROTECTED); }
{ASSEMBLY_DIRECTIVE}  { addToSymbolTable(yytext, "keyword"); return(ASSEMBLY_DIRECTIVE); }

{IDENTIFIER}   { checkIdentifierLength(yytext); if(check_type_name(yytext)){addToSymbolTable(yytext, "type_name");return(TYPE_NAME);} if(is_typedef==1){is_typedef=0;addToSymbolTable(yytext, "type_name");return(TYPE_NAME);} addToSymbolTable(yytext, "identifier"); return(IDENTIFIER); }
{H_CONSTANT}   { addToSymbolTable(yytext, "hexadecimal_constant"); return(H_CONSTANT); }
{O_CONSTANT}   { addToSymbolTable(yytext, "octal_constant"); return(O_CONSTANT); }
{I_CONSTANT}   { addToSymbolTable(yytext, "integer_constant"); return(I_CONSTANT); }
{C_CONSTANT}   { addToSymbolTable(yytext, "char_constant"); return(C_CONSTANT); }
{F_CONSTANT_DECIMAL}   { addToSymbolTable(yytext, "float_constant"); return(F_CONSTANT); }
{F_CONSTANT_INT}   { addToSymbolTable(yytext, "float_constant"); return(F_CONSTANT); }
{STRING_LITERAL}   { addToSymbolTable(yytext, "string_literal"); return(STRING_LITERAL); }

{RIGHT_ASSIGN}     { addToSymbolTable(yytext, "operator"); return(RIGHT_ASSIGN); }
{LEFT_ASSIGN}      { addToSymbolTable(yytext, "operator"); return(LEFT_ASSIGN); }
{ADD_ASSIGN}       { addToSymbolTable(yytext, "operator"); return(ADD_ASSIGN); }
{SUB_ASSIGN}       { addToSymbolTable(yytext, "operator"); return(SUB_ASSIGN); }
{MUL_ASSIGN}       { addToSymbolTable(yytext, "operator"); return(MUL_ASSIGN); }
{DIV_ASSIGN}       { addToSymbolTable(yytext, "operator"); return(DIV_ASSIGN); } 
{MOD_ASSIGN}       { addToSymbolTable(yytext, "operator"); return(MOD_ASSIGN); }
{AND_ASSIGN}       { addToSymbolTable(yytext, "operator"); return(AND_ASSIGN); }
{XOR_ASSIGN}       { addToSymbolTable(yytext, "operator"); return(XOR_ASSIGN); }
{OR_ASSIGN}        { addToSymbolTable(yytext, "operator"); return(OR_ASSIGN); }
{DIVIDE}           { addToSymbolTable(yytext, "operator"); return(DIVIDE); }
{LOGICAL_AND}     { addToSymbolTable(yytext, "operator"); return(LOGICAL_AND); }
{NOT}             { addToSymbolTable(yytext, "operator"); return(NOT); }
{LOGICAL_NOT}     { addToSymbolTable(yytext, "operator"); return(LOGICAL_NOT); }
{MINUS}           { addToSymbolTable(yytext, "operator"); return(MINUS); }
{PLUS}            { addToSymbolTable(yytext, "operator"); return(PLUS); }
{MULTIPLY}        { addToSymbolTable(yytext, "operator"); return(MULTIPLY); }
{LOGICAL_OR}      { addToSymbolTable(yytext, "operator"); return(LOGICAL_OR); }

{RIGHT_OP}        { addToSymbolTable(yytext, "operator"); return(RIGHT_OP); }
{LEFT_OP}         { addToSymbolTable(yytext, "operator"); return(LEFT_OP); }
{INC_OP}          { addToSymbolTable(yytext, "operator"); return(INC_OP); }
{DEC_OP}          { addToSymbolTable(yytext, "operator"); return(DEC_OP); }
{INHERITANCE_OP}  { addToSymbolTable(yytext, "operator"); return(INHERITANCE_OP); }
{PTR_OP}          { addToSymbolTable(yytext, "operator"); return(PTR_OP); }
{AND_OP}          { addToSymbolTable(yytext, "operator"); return(AND_OP); }
{OR_OP}           { addToSymbolTable(yytext, "operator"); return(OR_OP); }
{LE_OP}           { addToSymbolTable(yytext, "operator"); return(LE_OP); }
{GE_OP}           { addToSymbolTable(yytext, "operator"); return(GE_OP); }
{EQ_OP}           { addToSymbolTable(yytext, "operator"); return(EQ_OP); }
{NE_OP}           { addToSymbolTable(yytext, "operator"); return(NE_OP); }
{XOR_OP}          { addToSymbolTable(yytext, "operator"); return(XOR_OP); }

{SEMICOLON}           { addToSymbolTable(yytext, "symbol"); return(SEMICOLON); }
{LEFT_CURLY_BRACKET}  { addToSymbolTable(yytext, "symbol"); return(LEFT_CURLY_BRACKET); }
{RIGHT_CURLY_BRACKET} { addToSymbolTable(yytext, "symbol"); return(RIGHT_CURLY_BRACKET); }
{COMMA}               { addToSymbolTable(yytext, "symbol"); return(COMMA); }
{COLON}               { addToSymbolTable(yytext, "symbol"); return(COLON); }
{EQ}                  { addToSymbolTable(yytext, "symbol"); return(EQ); }
{LEFT_BRACKET}        { addToSymbolTable(yytext, "symbol"); return(LEFT_BRACKET); }
{RIGHT_BRACKET}       { addToSymbolTable(yytext, "symbol"); return(RIGHT_BRACKET); }
{LEFT_THIRD_BRACKET}  { addToSymbolTable(yytext, "symbol"); return(LEFT_THIRD_BRACKET); }
{RIGHT_THIRD_BRACKET} { addToSymbolTable(yytext, "symbol"); return(RIGHT_THIRD_BRACKET); }

{DOT}              { addToSymbolTable(yytext, "symbol"); return(DOT); }

{MOD}              { addToSymbolTable(yytext, "operator"); return(MOD); }
{LESS}             { addToSymbolTable(yytext, "operator"); return(LESS); }
{GREATER}          { addToSymbolTable(yytext, "operator"); return(GREATER); }
{EXPONENT}         { addToSymbolTable(yytext, "operator"); return(EXPONENT); }

{QUESTION}         { addToSymbolTable(yytext, "operator"); return(QUESTION); }

{SINGLE_LINE_COMMENT}       { }
{MULTI_LINE_COMMENT}        { }

[ \t]+             { /* Ignore whitespace */ }

\n                 { line_no++;  }

<<EOF>>            { return 0; }

.                  { report_error("Unrecognized symbol", yytext); return(ERROR); }

%%


