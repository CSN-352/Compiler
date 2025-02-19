%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

#ifdef ENABLE_MAIN
    #define RETURN_IF_NOT_MAIN(x)  // Expands to nothing
#else
    #define RETURN_IF_NOT_MAIN(x) return x
#endif 

int line_no = 1;  // Line number tracker
int has_error=0;
int column_no = 1; // Column number tracker

// Symbol table structure
#define MAX_SYMBOLS 10000
typedef struct {
    char *lexeme;  // Pointer to char for storing strings
    char *token;   // Pointer to char for storing token types
    int line;      // Line where the symbol appears
    int column;    // Column where the symbol appears
} Symbol;

Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

extern int yylex();  // Lexical analyzer function
extern FILE *yyin;   // Input file

// Function to handle errors
void report_error(const char *msg, const char *symbol) {
    has_error=1;
    printf("Error at line %d, column %d: %s '%s'\n", line_no, column_no, msg, symbol);
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
        free(symbolTable[symbolCount].token); // Clean up previous allocation
        return;  // Handle memory allocation failure
    }

    // Store position information
    symbolTable[symbolCount].line = line_no;
    symbolTable[symbolCount].column = column_no;

    symbolCount++;
}

// Function to print the symbol table in a structured format
void printSymbolTable() {
    printf("\n+----------------------+----------------------+--------+--------+\n");
    printf("|      Lexeme          |        Token         |  Line  | Column |\n");
    printf("+----------------------+----------------------+--------+--------+\n");

    for (int i = 0; i < symbolCount; i++) {
        printf("| %-20s | %-20s | %6d | %6d |\n", 
               symbolTable[i].lexeme, 
               symbolTable[i].token,
               symbolTable[i].line,
               symbolTable[i].column);
    }

    printf("+----------------------+----------------------+--------+--------+\n");
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

// Function to update column number
void updateColumn(int length) {
    column_no += length;
}

// Function to handle multiline comment column tracking
void handleMultilineComment(const char* text, int length) {
    for(int i = 0; i < length; i++) {
        if(text[i] == '\n') {
            line_no++;
            column_no = 1;
        } else {
            column_no++;
        }
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
I_CONSTANT    [0-9]+[uU]?[lL]?[lL]?
H_CONSTANT    0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?
O_CONSTANT    0[0-7]+[uU]?[lL]?[lL]?
F_CONSTANT    ([0-9]*\.[0-9]+|[0-9]+\.)([eE][+-]?[0-9]+)?[fFlL]?|[0-9]+[eE][+-]?[0-9]+[fFlL]?
HEX_F_CONSTANT 0[xX]([0-9a-fA-F]*\.[0-9a-fA-F]+|[0-9a-fA-F]+\.)[pP][+-]?[0-9]+[fFlL]?
STRING_LITERAL \"(\\(['"?\\abfnrtv]|[0-7]{1,3}|x[0-9a-fA-F]+)|[^"\\])*\"
CHAR_CONSTANT \'(\\(['"?\\abfnrtv]|[0-7]{1,3}|x[0-9a-fA-F]+)|[^'\\])\'

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
XOR_ASSIGN    \^=       
OR_ASSIGN     \|=      

RIGHT_OP      >>       
LEFT_OP       <<       
INC_OP        \+\+     
DEC_OP        --       
INHERITANCE_OP <-     
PTR_OP        ->       
LOGICAL_AND   &&       
LOGICAL_OR    \|\|     
LE_OP         <=       
GE_OP         >=       
EQ_OP         ==       
NE_OP         !=       

/* Punctuators */
SEMICOLON     ;        
LEFT_CURLY    \{       
RIGHT_CURLY   \}       
COMMA         ,        
COLON         :        
ASSIGN        =        
LEFT_PAREN    \(       
RIGHT_PAREN   \)       
LEFT_BRACKET  \[       
RIGHT_BRACKET \]       
DOT           \.       
QUESTION      \?       
POUND         "#"

/* Individual operators */
NOT           !        
BITWISE_NOT   ~        
MINUS         -        
PLUS          \+       
MULTIPLY      \*       
DIVIDE        "/"       
MOD           %        
LESS          <        
GREATER       >        
BITWISE_XOR   \^       
BITWISE_OR    \|       
BITWISE_AND   &

/* Comments and whitespace */
SINGLE_LINE_COMMENT "//".*\n
MULTI_LINE_COMMENT  "/*"([^*]|\*[^/])*"*/"
WHITESPACE     [ \t\v\f\r]+
NEWLINE        \n

%%

{AUTO}                { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(AUTO); }
{BREAK}               { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(BREAK); }
{CASE}                { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CASE); }
{CHAR}                { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CHAR); }
{CONST}               { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CONST); }
{CONTINUE}            { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CONTINUE); }
{DEFAULT}             { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DEFAULT); }
{DO}                  { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DO); }
{DOUBLE}              { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DOUBLE); }
{ELSE}                { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(ELSE); }
{ENUM}                { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(ENUM); }
{EXTERN}              { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(EXTERN); }
{FLOAT}               { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(FLOAT); }
{FOR}                 { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(FOR); }
{GOTO}                { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(GOTO); }
{IF}                  { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(IF); }
{INT}                 { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(INT); }
{LONG}                { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LONG); }
{REGISTER}            { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(REGISTER); }
{RETURN}              { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(RETURN); }
{SHORT}               { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(SHORT); }
{SIGNED}              { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(SIGNED); }
{SIZEOF}              { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(SIZEOF); }
{STATIC}              { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(STATIC); }
{STRUCT}              { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(STRUCT); }
{SWITCH}              { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(SWITCH); }
{TYPEDEF}             { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(TYPEDEF); }
{UNION}               { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(UNION); }
{UNSIGNED}            { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(UNSIGNED); }
{VOID}                { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(VOID); }
{VOLATILE}            { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(VOLATILE); }
{WHILE}               { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(WHILE); }
{UNTIL}               { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(UNTIL); }
{CLASS}               { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CLASS); }
{PRIVATE}             { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(PRIVATE); }
{PUBLIC}              { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(PUBLIC); }
{PROTECTED}           { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(PROTECTED); }
{ASSEMBLY_DIRECTIVE}  { addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(ASSEMBLY_DIRECTIVE); }

{IDENTIFIER}          { checkIdentifierLength(yytext); addToSymbolTable(yytext, "identifier"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(IDENTIFIER); }
{H_CONSTANT}          { addToSymbolTable(yytext, "hexadecimal_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CONSTANT); }
{O_CONSTANT}          { addToSymbolTable(yytext, "octal_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CONSTANT); }
{I_CONSTANT}          { addToSymbolTable(yytext, "integer_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CONSTANT); }
{F_CONSTANT}          { addToSymbolTable(yytext, "float_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CONSTANT); }
{HEX_F_CONSTANT}      { addToSymbolTable(yytext, "hexadecimal_float_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CONSTANT); }
{CHAR_CONSTANT}       { addToSymbolTable(yytext, "char_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CONSTANT); }
{STRING_LITERAL}      { addToSymbolTable(yytext, "string_literal"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(STRING_LITERAL); }

{RIGHT_ASSIGN}        { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(RIGHT_ASSIGN); }
{LEFT_ASSIGN}         { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LEFT_ASSIGN); }
{ADD_ASSIGN}          { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(ADD_ASSIGN); }
{SUB_ASSIGN}          { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(SUB_ASSIGN); }
{MUL_ASSIGN}          { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(MUL_ASSIGN); }
{DIV_ASSIGN}          { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DIV_ASSIGN); }
{MOD_ASSIGN}          { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(MOD_ASSIGN); }
{AND_ASSIGN}          { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(AND_ASSIGN); }
{XOR_ASSIGN}          { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(XOR_ASSIGN); }
{OR_ASSIGN}           { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(OR_ASSIGN); }

{RIGHT_OP}            { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(RIGHT_OP); }
{LEFT_OP}             { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LEFT_OP); }
{INC_OP}              { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(INC_OP); }
{DEC_OP}              { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DEC_OP); }
{INHERITANCE_OP}      { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(INHERITANCE_OP); }
{PTR_OP}              { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(PTR_OP); }
{LOGICAL_AND}         { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LOGICAL_AND); }
{LOGICAL_OR}          { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LOGICAL_OR); }
{LE_OP}               { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LE_OP); }
{GE_OP}               { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(GE_OP); }
{EQ_OP}               { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(EQ_OP); }
{NE_OP}               { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(NE_OP); }

{SEMICOLON}           { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(SEMICOLON); }
{LEFT_CURLY}          { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LEFT_CURLY); }
{RIGHT_CURLY}         { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(RIGHT_CURLY); }
{COMMA}               { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(COMMA); }
{COLON}               { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(COLON); }
{ASSIGN}              { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(ASSIGN); }
{LEFT_PAREN}          { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LEFT_PAREN); }
{RIGHT_PAREN}         { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(RIGHT_PAREN); }
{LEFT_BRACKET}        { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LEFT_BRACKET); }
{RIGHT_BRACKET}       { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(RIGHT_BRACKET); }
{DOT}                 { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DOT); }
{ELLIPSIS}            { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(ELLIPSIS); }
{POUND}               { addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(POUND); }

{NOT}                 { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(NOT); }
{BITWISE_NOT}         { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(BITWISE_NOT); }
{MINUS}               { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(MINUS); }
{PLUS}                { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(PLUS); }
{MULTIPLY}            { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(MULTIPLY); }
{DIVIDE}              { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DIVIDE); }
{MOD}                 { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(MOD); }
{LESS}                { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LESS); }
{GREATER}             { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(GREATER); }
{BITWISE_XOR}         { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(BITWISE_XOR); }
{BITWISE_OR}          { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(BITWISE_OR); }
{BITWISE_AND}         { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(BITWISE_AND); }
{QUESTION}            { addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(QUESTION); }

{SINGLE_LINE_COMMENT} { line_no++; column_no = 1; /* Reset column for new line */ }
{MULTI_LINE_COMMENT}  { handleMultilineComment(yytext, yyleng); }

{WHITESPACE}          { updateColumn(yyleng); /* Just count whitespace */ }
{NEWLINE}             { line_no++; column_no = 1; }

<<EOF>>               { return 0; }

.                     { report_error("Unrecognized symbol", yytext); updateColumn(1); }

%%

#ifdef ENABLE_MAIN
int main(int argc, char *argv[]) {
    // Check if file name is provided
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot open input file %s\n", argv[1]);
            return 1;
        }
    } else {
        yyin = stdin; // Read from standard input if no file provided
    }
    
    // Process lexical analysis
    yylex();
    
    // Print symbol table if no errors occurred
    if(has_error == 0) {
        printSymbolTable();
    } else {
        printf("\nLexical analysis completed with errors.\n");
    }
    
    // Clean up
    if (yyin != stdin) {
        fclose(yyin);
    }
    
    // Free memory in symbol table
    for (int i = 0; i < symbolCount; i++) {
        free(symbolTable[i].lexeme);
        free(symbolTable[i].token);
    }
    
    return has_error;
}
#endif
