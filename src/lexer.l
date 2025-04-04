%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"
#include "symbol_table.h"
#include "ast.h"
#include "expression.h"

#ifdef ENABLE_MAIN
    #define RETURN_IF_NOT_MAIN(x)  // Expands to nothing
#else
    #define RETURN_IF_NOT_MAIN(x) return x
#endif 

unsigned int line_no = 1;  // Line number tracker
unsigned int column_no = 1; // Column number tracker
int is_typedef=0;
extern void yyerror(const char* msg);
extern "C" int yylex();  // Lexical analyzer function
extern FILE *yyin;   // Input file

// Symbol table structure
// #define MAX_SYMBOLS 10000
// typedef struct {
//     char *lexeme;  // Pointer to char for storing strings
//     char *token;   // Pointer to char for storing token types
//     int line;      // Line where the symbol appears
//     int column;    // Column where the symbol appears
// } Symbol;

// Symbol symbolTable[MAX_SYMBOLS];
// int symbolCount = 0;


// Function to handle errors
//void report_error(const char *msg, const char *symbol) {
//    has_error=1;
//    printf("Error at line %d, column %d: %s '%s'\n", line_no, column_no, msg, symbol);
//}

// Function to add symbol to the symbol table
void addToSymbolTable(const char *lexeme, const char *type) {
    // if (symbolCount >= MAX_SYMBOLS) {
    //     return;  // Avoid adding more symbols if table is full
    // }

    // symbolTable[symbolCount].token = strdup(type);
    // if (symbolTable[symbolCount].token == NULL) {
    //     return;  // Handle memory allocation failure
    // }

    // symbolTable[symbolCount].lexeme = strdup(lexeme);
    // if (symbolTable[symbolCount].lexeme == NULL) {
    //     free(symbolTable[symbolCount].token); // Clean up previous allocation
    //     return;  // Handle memory allocation failure
    // }

    // // Store position information
    // symbolTable[symbolCount].line = line_no;
    // symbolTable[symbolCount].column = column_no;

    // symbolCount++;
}

// Function to print the symbol table in a structured format
void printSymbolTable() {
    // printf("\n+----------------------+----------------------+--------+--------+\n");
    // printf("|      Lexeme          |        Token         |  Line  | Column |\n");
    // printf("+----------------------+----------------------+--------+--------+\n");

    // for (int i = 0; i < symbolCount; i++) {
    //     printf("| %-20s | %-20s | %6d | %6d |\n", 
    //            symbolTable[i].lexeme, 
    //            symbolTable[i].token,
    //            symbolTable[i].line,
    //            symbolTable[i].column);
    // }

    // printf("+----------------------+----------------------+--------+--------+\n");
}

int check_type_name(const char* lexeme) {  
    // for (int i = 0; i < symbolCount; i++) {  
    //     if (strcmp(symbolTable[i].token, "type_name") == 0 && strcmp(symbolTable[i].lexeme, lexeme) == 0) {  
    //         return 1;
    //     }  
    // }  
    return 0;
}  


// Function to check identifier length
int checkIdentifierLength(const char *identifier) {
    if (strlen(identifier) > 32) {
        return 0;
    }
    return 1;
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
O_F_CONSTANT 0([0-9a-fA-F]*\.[0-9a-fA-F]+|[0-9a-fA-F]+\.)[pP][+-]?[0-9]+[fFlL]?
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
LEFT_PAREN    \(       
RIGHT_PAREN   \)       
LEFT_SQUARE  \[       
RIGHT_SQUARE \]  
COMMA         ,        
COLON         :        
ASSIGN        =       
DOT           \.       
QUESTION      \?       


/* Individual operators */
NOT           !        
BITWISE_NOT   ~    
BITWISE_XOR   \^       
BITWISE_OR    \|       
BITWISE_AND   &    
MINUS         -        
PLUS          \+       
MULTIPLY      \*       
DIVIDE        "/"       
MOD           %        
LESS          <        
GREATER       >        


/* Comments and whitespace */
SINGLE_LINE_COMMENT "//".*\n
MULTI_LINE_COMMENT  "/*"([^*]|\*[^/])*"*/"
WHITESPACE     [ \t\v\f\r]+
NEWLINE        \n

%%
{AUTO}                { yylval.intval = AUTO; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(AUTO); }
{BREAK}               { Terminal* t = new Terminal("BREAK",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(BREAK); }
{CASE}                { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CASE); }
{CHAR}                { Terminal* t = new Terminal("CHAR",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CHAR); }
{CONST}               { yylval.intval = CONST; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CONST); }
{CONTINUE}            { Terminal* t = new Terminal("CONTINUE",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CONTINUE); }
{DEFAULT}             { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DEFAULT); }
{DO}                  { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DO); }
{DOUBLE}              { Terminal* t = new Terminal("DOUBLE",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DOUBLE); }
{ELSE}                { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(ELSE); }
{ENUM}                { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(ENUM); }
{EXTERN}              { yylval.intval = EXTERN; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(EXTERN); }
{FLOAT}               { Terminal* t = new Terminal("FLOAT",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(FLOAT); }
{FOR}                 { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(FOR); }
{GOTO}                { Terminal* t = new Terminal("GOTO",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(GOTO); }
{IF}                  { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(IF); }
{INT}                 { Terminal* t = new Terminal("INT",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(INT); }
{LONG}                { Terminal* t = new Terminal("LONG",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LONG); }
{REGISTER}            { yylval.intval = REGISTER; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(REGISTER); }
{RETURN}              { Terminal* t = new Terminal("RETURN",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(RETURN); }
{SHORT}               { Terminal* t = new Terminal("SHORT",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(SHORT); }
{SIGNED}              { Terminal* t = new Terminal("SIGNED",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(SIGNED); }
{SIZEOF}              { Terminal* t = new Terminal("SIZEOF",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(SIZEOF); }
{STATIC}              { yylval.intval = STATIC; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(STATIC); }
{STRUCT}              { Terminal* t = new Terminal("STRUCT",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(STRUCT); }
{SWITCH}              { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(SWITCH); }
{TYPEDEF}             { yylval.intval = TYPEDEF; is_typedef=1; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(TYPEDEF); }
{UNION}               { Terminal* t = new Terminal("UNION",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(UNION); }
{UNSIGNED}            { Terminal* t = new Terminal("UNSIGNED",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(UNSIGNED); }
{VOID}                { Terminal* t = new Terminal("VOID",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(VOID); }
{VOLATILE}            { yylval.intval = VOLATILE; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(VOLATILE); }
{WHILE}               { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(WHILE); }
{UNTIL}               { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(UNTIL); }
{CLASS}               { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CLASS); }
{PRIVATE}             { Terminal* t = new Terminal("PRIVATE",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(PRIVATE); }
{PUBLIC}              { Terminal* t = new Terminal("PUBLIC",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(PUBLIC); }
{PROTECTED}           { Terminal* t = new Terminal("PROTECTED",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(PROTECTED); }
{ASSEMBLY_DIRECTIVE}  { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(ASSEMBLY_DIRECTIVE); }
{IDENTIFIER}          { Identifier* id = new Identifier(yytext, line_no, column_no); yylval.identifier = id;
                        if(!checkIdentifierLength(yytext)) {
                            yyerror("Identifier exceeds maximum length (32 characters):");
                            return(ERROR);
                        } 
                        else 
                            addToSymbolTable(yytext, "identifier");
                        updateColumn(yyleng); 
                        if(symbolTable.lookup_typedef(yytext))
                            return(TYPE_NAME);
                        RETURN_IF_NOT_MAIN(IDENTIFIER);
            }
{H_CONSTANT}          { Constant* c = new Constant("I_CONSTANT", yytext, line_no, column_no); yylval.constant = c; addToSymbolTable(yytext, "hexadecimal_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(I_CONSTANT); }
{O_CONSTANT}          { Constant* c = new Constant("I_CONSTANT", yytext, line_no, column_no); yylval.constant = c;  addToSymbolTable(yytext, "octal_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(I_CONSTANT); }
{I_CONSTANT}          { Constant* c = new Constant("I_CONSTANT", yytext, line_no, column_no); yylval.constant = c;  addToSymbolTable(yytext, "integer_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(I_CONSTANT); }
{F_CONSTANT}          { Constant* c = new Constant("F_CONSTANT", yytext, line_no, column_no); yylval.constant = c;  addToSymbolTable(yytext, "float_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(F_CONSTANT); }
{HEX_F_CONSTANT}      { Constant* c = new Constant("F_CONSTANT", yytext, line_no, column_no); yylval.constant = c;  addToSymbolTable(yytext, "hexadecimal_float_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(F_CONSTANT); }
{O_F_CONSTANT}        { Constant* c = new Constant("F_CONSTANT", yytext, line_no, column_no); yylval.constant = c; addToSymbolTable(yytext, "hexadecimal_float_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(F_CONSTANT); }
{CHAR_CONSTANT}       { Constant* c = new Constant("CHAR_CONSTANT", yytext, line_no, column_no); yylval.constant = c; addToSymbolTable(yytext, "char_constant"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(CHAR_CONSTANT); }
{STRING_LITERAL}      { StringLiteral* s = new StringLiteral(yytext,line_no,column_no); yylval.string_literal = s; addToSymbolTable(yytext, "string_literal"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(STRING_LITERAL); }

{RIGHT_ASSIGN}        { Terminal* t = new Terminal("RIGHT_ASSIGN",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(RIGHT_ASSIGN); }
{LEFT_ASSIGN}         { Terminal* t = new Terminal("LEFT_ASSIGN",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LEFT_ASSIGN); }
{ADD_ASSIGN}          { Terminal* t = new Terminal("ADD_ASSIGN",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(ADD_ASSIGN); }
{SUB_ASSIGN}          { Terminal* t = new Terminal("SUB_ASSIGN",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(SUB_ASSIGN); }
{MUL_ASSIGN}          { Terminal* t = new Terminal("MUL_ASSIGN",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(MUL_ASSIGN); }
{DIV_ASSIGN}          { Terminal* t = new Terminal("DIV_ASSIGN",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DIV_ASSIGN); }
{MOD_ASSIGN}          { Terminal* t = new Terminal("MOD_ASSIGN",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(MOD_ASSIGN); }
{AND_ASSIGN}          { Terminal* t = new Terminal("AND_ASSIGN",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(AND_ASSIGN); }
{XOR_ASSIGN}          { Terminal* t = new Terminal("XOR_ASSIGN",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(XOR_ASSIGN); }
{OR_ASSIGN}           { Terminal* t = new Terminal("OR_ASSIGN",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(OR_ASSIGN); }

{RIGHT_OP}            { Terminal* t = new Terminal("RIGHT_OP",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(RIGHT_OP); }
{LEFT_OP}             { Terminal* t = new Terminal("LEFT_OP",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LEFT_OP); }
{INC_OP}              { Terminal* t = new Terminal("INC_OP",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(INC_OP); }
{DEC_OP}              { Terminal* t = new Terminal("INC_OP",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DEC_OP); }
{INHERITANCE_OP}      { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(INHERITANCE_OP); }
{PTR_OP}              { Terminal* t = new Terminal("PTR_OP",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(PTR_OP); }
{LOGICAL_AND}         { Terminal* t = new Terminal("LOGICAL_AND",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LOGICAL_AND); }
{LOGICAL_OR}          { Terminal* t = new Terminal("LOGICAL_OR",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LOGICAL_OR); }
{LE_OP}               { Terminal* t = new Terminal("LE_OP",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LE_OP); }
{GE_OP}               { Terminal* t = new Terminal("GE_OP",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(GE_OP); }
{EQ_OP}               { Terminal* t = new Terminal("EQ_OP",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(EQ_OP); }
{NE_OP}               { Terminal* t = new Terminal("NE_OP",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "keyword"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(NE_OP); }

{SEMICOLON}           { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(SEMICOLON); }
{LEFT_CURLY}          { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LEFT_CURLY); }
{RIGHT_CURLY}         { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(RIGHT_CURLY); }
{COMMA}               { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(COMMA); }
{COLON}               { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(COLON); }
{ASSIGN}              { Terminal* t = new Terminal("ASSIGN",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(ASSIGN); }
{LEFT_PAREN}          { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LEFT_PAREN); }
{RIGHT_PAREN}         { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(RIGHT_PAREN); }
{LEFT_SQUARE}        { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LEFT_SQUARE); }
{RIGHT_SQUARE}       { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(RIGHT_SQUARE); }
{DOT}                 { Terminal* t = new Terminal("DOT",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DOT); }
{ELLIPSIS}            { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "punctuator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(ELLIPSIS); }


{NOT}                 { Terminal* t = new Terminal("NOT",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(NOT); }
{BITWISE_NOT}         { Terminal* t = new Terminal("BITWISE_NOT",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(BITWISE_NOT); }
{MINUS}               { Terminal* t = new Terminal("MINUS",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(MINUS); }
{PLUS}                { Terminal* t = new Terminal("PLUS",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(PLUS); }
{MULTIPLY}            { Terminal* t = new Terminal("MULTIPLY",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(MULTIPLY); }
{DIVIDE}              { Terminal* t = new Terminal("DIVIDE",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(DIVIDE); }
{MOD}                 { Terminal* t = new Terminal("MOD",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(MOD); }
{LESS}                { Terminal* t = new Terminal("LESS",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(LESS); }
{GREATER}             { Terminal* t = new Terminal("GREATER",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(GREATER); }
{BITWISE_XOR}         { Terminal* t = new Terminal("BITWISE_XOR",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(BITWISE_XOR); }
{BITWISE_OR}          { Terminal* t = new Terminal("BITWISE_OR",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(BITWISE_OR); }
{BITWISE_AND}         { Terminal* t = new Terminal("BITWISE_AND",yytext,line_no,column_no); yylval.terminal = t; addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(BITWISE_AND); }
{QUESTION}            { yylval.strval = strdup(yytext); addToSymbolTable(yytext, "operator"); updateColumn(yyleng); RETURN_IF_NOT_MAIN(QUESTION); }

{SINGLE_LINE_COMMENT} { line_no++; column_no = 1; /* Reset column for new line */ }
{MULTI_LINE_COMMENT}  { handleMultilineComment(yytext, yyleng); }

{WHITESPACE}          { updateColumn(yyleng); /* Just count whitespace */ }
{NEWLINE}             { line_no++; column_no = 1; }

<<EOF>>               { return 0; }

\'                   { return(SINGLE_QUOTE);}
\"                   { return(DOUBLE_QUOTE);}

.                     { yyerror("Unrecognized symbol"); updateColumn(1); return(ERROR); }

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