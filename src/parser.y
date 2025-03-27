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
    Declaration * declaration;
    Declarator* declarator;
    DirectDeclarator* direct_declarator;
    DeclaratorList * init_declarator_list;
    Constant* constant;
	StringLiteral* string_literal;
    Expression* expression;
    ArgumentExpressionList* argument_expression_list;
    DirectAbstractDeclarator* direct_abstract_declarator;
    AbstractDeclarator* abstract_declarator;
    ParameterList* parameter_list;
    ParameterTypeList* parameter_type_list;
    TypeName* type_name;
    SpecifierQualifierList* specifier_qualifier_list;
    TypeSpecifier* type_specifier;
    Pointer* pointer;
    TypeQualifierList* type_qualifier_list;
    DeclarationSpecifiers* declaration_specifiers;
    ParameterDeclaration* parameter_declaration;
    Enumerator* enumerator;
    EnumeratorList* enumerator_list;
    EnumSpecifier* enum_specifier;
    StructUnionSpecifier* struct_or_union_specifier;
    ClassSpecifier* class_specifier;
    int intval;
    char* strval;
}

%token <terminal> BITWISE_NOT NOT BITWISE_AND PLUS MINUS MULTIPLY SIZEOF
%token <identifier> IDENTIFIER
%token <constant> I_CONSTANT F_CONSTANT CHAR_CONSTANT
%token <string_literal> STRING_LITERAL
%token <terminal> INC_OP DEC_OP PTR_OP DOT
%type <terminal> unary_operator
%type <expression> expression assignment_expression primary_expression postfix_expression unary_expression cast_expression conditional_expression
%type <argument_expression_list> argument_expression_list
%type <type_name> type_name
%type <direct_abstract_declarator> direct_abstract_declarator
%type <abstract_declarator> abstract_declarator
%type <declarator> init_declarator declarator
%type <init_declarator_list> init_declarator_list
%type <declaration> declaration
%type <parameter_type_list> parameter_type_list
%type <direct_declarator> direct_declarator
%type <parameter_list> parameter_list
%type <type_specifier> type_specifier 
%type <struct_or_union_specifier> struct_or_union_specifier
%type <class_specifier> class_specifier
%type <enum_specifier> enum_specifier
%type <intval> type_qualifier storage_class_specifier
%type <specifier_qualifier_list> specifier_qualifier_list
%type <pointer> pointer
%type <type_qualifier_list> type_qualifier_list;
%type <declaration_specifiers> declaration_specifiers
%type <parameter_declaration> parameter_declaration;
%type <enumerator> enumerator
%type <enumerator_list> enumerator_list
%token <intval> VOID CHAR SHORT INT LONG FLOAT DOUBLE SIGNED UNSIGNED TYPE_NAME
%token <intval> TYPEDEF EXTERN STATIC AUTO REGISTER CONST VOLATILE
%token <strval> BREAK CASE CONTINUE DEFAULT DO ELSE ENUM FOR GOTO
%token <strval> IF RETURN STRUCT SWITCH UNION
%token <strval> WHILE UNTIL CLASS PRIVATE PUBLIC PROTECTED ASSEMBLY_DIRECTIVE
%token <strval> ELLIPSIS RIGHT_ASSIGN LEFT_ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token <strval> RIGHT_OP LEFT_OP INHERITANCE_OP LOGICAL_AND LOGICAL_OR LE_OP GE_OP EQ_OP NE_OP
%token <strval> SEMICOLON LEFT_CURLY RIGHT_CURLY LEFT_PAREN RIGHT_PAREN LEFT_SQUARE RIGHT_SQUARE COMMA COLON ASSIGN QUESTION
%token <strval> BITWISE_XOR BITWISE_OR DIVIDE MOD LESS GREATER
%token <strval> NEWLINE ERROR SINGLE_QUOTE DOUBLE_QUOTE 
%type <strval> error_case struct_or_union
%type <strval> struct_declarator_list
%type <strval> struct_declarator class_declaration class_declaration_list access_specifier
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
    | LEFT_PAREN primary_expression RIGHT_PAREN {$$ = create_primary_expression($2);} 
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
    BITWISE_AND {$$ = $1;}
    | MULTIPLY {$$ = $1;}
    | PLUS  {$$ = $1;}
    | MINUS {$$ = $1;}
    | NOT   {$$ = $1;}
    | BITWISE_NOT   {$$ = $1;}
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
    | declaration_specifiers init_declarator_list SEMICOLON  {$$ = new_declaration( $1, $2 );}
    | error_case skip_until_semicolon SEMICOLON
    ;

declaration_specifiers:
    storage_class_specifier declaration_specifiers {$$ = create_declaration_specifiers($2,$1);}
	| specifier_qualifier_list {$$ = create_declaration_specifiers($1);}                                                         
	| declaration_specifiers specifier_qualifier_list {$$ = create_declaration_specifiers($1,$2);}
    ;

init_declarator_list:
    init_declarator { $$ = create_init_declarator_list( $1 ); }
    | init_declarator_list COMMA init_declarator 
    ;

init_declarator:
    declarator {$$ = $1;}
    | declarator ASSIGN initializer
    ;

storage_class_specifier:
    TYPEDEF {$$ = 0;}
    | EXTERN {$$ = 1;}
    | STATIC {$$ = 2;}
    | AUTO {$$ = 3;}
    | REGISTER {$$ = 4;}
    ;

type_specifier:
    VOID        {$$ = create_type_specifier($1);}
    | CHAR      {$$ = create_type_specifier($1);}
	| SHORT     {$$ = create_type_specifier($1);}
	| INT       {$$ = create_type_specifier($1);}  
	| LONG      {$$ = create_type_specifier($1);}
	| FLOAT     {$$ = create_type_specifier($1);}
	| DOUBLE    {$$ = create_type_specifier($1);}
	| SIGNED    {$$ = create_type_specifier($1);}
	| UNSIGNED  {$$ = create_type_specifier($1);}
    | TYPE_NAME {$$ = create_type_specifier($1);}
	| struct_or_union_specifier {$$ = create_struct_or_union_type_specifier($1);}
	| enum_specifier            {$$ = create_enum_type_specifier($1);}
    | class_specifier           /*{$$ = create_class_type_specifier($1);} */
	;

struct_or_union_specifier:
    struct_or_union IDENTIFIER LEFT_CURLY {symbolTable.enterScope();} struct_declaration_list RIGHT_CURLY {symbolTable.exitScope();}  
	| struct_or_union LEFT_CURLY {symbolTable.enterScope();} struct_declaration_list RIGHT_CURLY {symbolTable.exitScope();}    
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
    CLASS IDENTIFIER LEFT_CURLY {symbolTable.enterScope();} class_declaration_list RIGHT_CURLY {symbolTable.exitScope();} 
    | CLASS IDENTIFIER INHERITANCE_OP init_declarator_list LEFT_CURLY {symbolTable.enterScope();} class_declaration_list RIGHT_CURLY {symbolTable.exitScope();} 
    | CLASS LEFT_CURLY {symbolTable.enterScope();} class_declaration_list RIGHT_CURLY {symbolTable.exitScope();} 
    | CLASS IDENTIFIER 
    ;

class_declaration_list:
    class_declaration
    | class_declaration_list class_declaration
    ;


class_declaration:
    access_specifier LEFT_CURLY {symbolTable.enterScope();} translation_unit RIGHT_CURLY {symbolTable.exitScope();} 
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
    type_specifier {$$ = create_specifier_qualifier_list($1);}
    | specifier_qualifier_list type_specifier {$$ = create_specifier_qualifier_list($1,$2);}                      
	| type_qualifier specifier_qualifier_list {$$ = create_specifier_qualifier_list($2,$1);}
    | specifier_qualifier_list type_qualifier {$$ = create_specifier_qualifier_list($1,$2);}                  
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
    ENUM LEFT_CURLY enumerator_list RIGHT_CURLY { $$ = create_enumerator_specifier($3); }                
	| ENUM IDENTIFIER LEFT_CURLY enumerator_list RIGHT_CURLY { $$ = create_enumerator_specifier($2, $4); }    
	| ENUM IDENTIFIER { $$ = create_enumerator_specifier($2, nullptr); }                        
	;

enumerator_list:
    enumerator { $$ = create_enumerator_list(nullptr,$1);}
    | enumerator_list COMMA enumerator { $$ = create_enumerator_list($1, $3);}
    ;

enumerator:
    IDENTIFIER {$$ = create_enumerator($1,nullptr);}
    | IDENTIFIER ASSIGN conditional_expression {$$ = create_enumerator($1,$3);} 
    ;

type_qualifier:
    CONST  {$$ = 0;}
    | VOLATILE  {$$ = 1;}
    ;

declarator:
    pointer direct_declarator { $$ = create_declarator( $1, $2 ); } 
    | direct_declarator  { $$ = create_declarator( nullptr, $1 ); }
    ;

direct_declarator:
    IDENTIFIER  { $$ = create_dir_declarator_id( $1 ); }
    | direct_declarator LEFT_SQUARE conditional_expression RIGHT_SQUARE {$$ = create_direct_declarator_array($1, $3);}
	| direct_declarator LEFT_SQUARE RIGHT_SQUARE {$$ = create_direct_declarator_array($1, nullptr);} 
    | LEFT_PAREN declarator RIGHT_PAREN { $$ = create_direct_declarator($2); } 
    | direct_declarator LEFT_PAREN parameter_type_list RIGHT_PAREN {$$ = create_direct_declarator_function($1, $3);}
    | direct_declarator LEFT_PAREN RIGHT_PAREN {$$ = create_direct_declarator_function($1, nullptr);} 
    ;

pointer:
    MULTIPLY {$$ = create_pointer(nullptr);}
    | MULTIPLY type_qualifier_list {$$ = create_pointer($2);}
    | MULTIPLY pointer {$$ = create_pointer($2,nullptr);}
    | MULTIPLY type_qualifier_list pointer {$$ = create_pointer($3,$2);}
    ;

type_qualifier_list:
    type_qualifier  {$$ = create_type_qualifier_list($1);}
    | type_qualifier_list type_qualifier  {$$ = create_type_qualifier_list($1,$2);}
    ;

parameter_type_list:
    parameter_list {$$ = create_parameter_type_list($1,false);}
    | parameter_list COMMA ELLIPSIS {$$ = create_parameter_type_list($1,true);}
    ;

parameter_list:
    parameter_declaration {$$ = create_parameter_list($1);}
    | parameter_list COMMA parameter_declaration {$$ = create_parameter_list($1,$3);}
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
    specifier_qualifier_list {$$ = create_type_name($1,nullptr);}
	| specifier_qualifier_list abstract_declarator {$$ = create_type_name($1,$2);}
	;

abstract_declarator:
    pointer {$$ = create_abstract_declarator($1,nullptr);}
	| direct_abstract_declarator {$$ = create_abstract_declarator(nullptr,$1);}
	| pointer direct_abstract_declarator {$$ = create_abstract_declarator($1,$2);}
	;

direct_abstract_declarator:
    LEFT_PAREN abstract_declarator RIGHT_PAREN {$$ = create_direct_abstract_declarator($2);}
    | LEFT_SQUARE conditional_expression RIGHT_SQUARE {$$ = create_direct_abstract_declarator_array($2);}
	| LEFT_SQUARE RIGHT_SQUARE {$$ = create_direct_abstract_declarator_array(nullptr);}
	| LEFT_PAREN RIGHT_PAREN {$$ = create_direct_abstract_declarator_function(nullptr);}
	| LEFT_PAREN parameter_type_list RIGHT_PAREN {$$ = create_direct_abstract_declarator_function($2);}
    | LEFT_SQUARE conditional_expression RIGHT_SQUARE direct_abstract_declarator {$$ = create_direct_abstract_declarator_array($4,$2);}
    | LEFT_SQUARE RIGHT_SQUARE direct_abstract_declarator {$$ = create_direct_abstract_declarator_array($3,nullptr);}
	| LEFT_PAREN RIGHT_PAREN direct_abstract_declarator{$$ = create_direct_abstract_declarator_function($3, nullptr);}
	| LEFT_PAREN parameter_type_list RIGHT_PAREN direct_abstract_declarator {$$ = create_direct_abstract_declarator_function($4,$2);}
	;

initializer:
    assignment_expression
    | LEFT_CURLY {symbolTable.enterScope();} initializer_list RIGHT_CURLY {symbolTable.exitScope();}
    | LEFT_CURLY {symbolTable.enterScope();} initializer_list COMMA RIGHT_CURLY {symbolTable.exitScope();}
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
    LEFT_CURLY {symbolTable.enterScope();} RIGHT_CURLY {symbolTable.exitScope();}
    | LEFT_CURLY {symbolTable.enterScope();} declaration_statement_list RIGHT_CURLY {symbolTable.exitScope();}
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
    symbolTable.printSymbolTable();
    // if(!has_error)printParseSymbolTable();
    printf("Parsing completed successfully.\n");
    return 0;
}
