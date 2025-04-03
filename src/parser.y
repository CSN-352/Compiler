%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h" 
#include "symbol_table.h" 
#include "ast.h"
#include "expression.h"
#include "utils.h"

// External declarations 
extern "C" int yylex();
extern int yyparse();
extern int yylineno;
extern YYSTYPE yylval;
extern FILE *yyin;
int has_error=0;
int function_flag = 0;
FunctionDefinition* fd;
StructUnionSpecifier* sus;
ClassSpecifier* cs;

void yyerror(const char *msg);
%} 

%code requires {
    #include "ast.h"
    #include "expression.h"
    #include "symbol_table.h"
    #include "statement.h"
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
    ExpressionList* expression_list;

    Declaration* declaration;
    DeclarationSpecifiers* declaration_specifiers;
    InitDeclaratorList* init_declarator_list;
    InitDeclarator* init_declarator;
    TypeSpecifier* type_specifier;
    StructUnionSpecifier* struct_or_union_specifier;
    StructDeclarationSet* struct_declaration_set;
    StructDeclarationListAccess* struct_declaration_list_access;
    StructDeclarationList* struct_declaration_list;
    ClassSpecifier* class_specifier;
    ClassDeclaratorList* class_declarator_list;
    ClassDeclarator* class_declarator;
    ClassDeclarationList* class_declaration_list;
    ClassDeclaration* class_declaration;
    StructDeclaration* struct_declaration;
    SpecifierQualifierList* specifier_qualifier_list;
    StructDeclaratorList* struct_declarator_list;
    StructDeclarator* struct_declarator;
    EnumSpecifier* enum_specifier;
    EnumeratorList* enumerator_list;
    Enumerator* enumerator;
    Declarator* declarator;
    DirectDeclarator* direct_declarator;
    Pointer* pointer;
    TypeQualifierList* type_qualifier_list;
    ParameterTypeList* parameter_type_list;
    ParameterList* parameter_list;
    ParameterDeclaration* parameter_declaration;
    IdentifierList* identifier_list;
    TypeName* type_name;
    AbstractDeclarator* abstract_declarator;
    DirectAbstractDeclarator* direct_abstract_declarator;
    Initializer* initializer;
    DeclarationList* declaration_list;
    TranslationUnit* translation_unit;
    ExternalDeclaration* external_declaration;
    FunctionDefinition* function_definition;

    Statement* statement;
    LabeledStatement* labeled_statement;
    CompoundStatement* compound_statement;
    DeclarationStatementList* declaration_statement_list;
    StatementList* statement_list;
    int intval;
    char* strval;
}

%token <terminal> BITWISE_NOT NOT BITWISE_AND PLUS MINUS MULTIPLY DIVIDE MOD SIZEOF
%token <identifier> IDENTIFIER
%token <constant> I_CONSTANT F_CONSTANT CHAR_CONSTANT
%token <string_literal> STRING_LITERAL
%token <terminal> INC_OP DEC_OP PTR_OP DOT RIGHT_OP LEFT_OP LOGICAL_AND LOGICAL_OR LE_OP GE_OP EQ_OP NE_OP BITWISE_XOR BITWISE_OR LESS GREATER
%token <terminal> RIGHT_ASSIGN LEFT_ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token <terminal> ASSIGN
%token <terminal> VOID CHAR SHORT INT LONG FLOAT DOUBLE SIGNED UNSIGNED TYPE_NAME 
%token <terminal> STRUCT UNION PUBLIC PRIVATE PROTECTED
%token <terminal> GOTO CONTINUE BREAK RETURN
%type <terminal> unary_operator assignment_operator

%type <expression> assignment_expression primary_expression postfix_expression unary_expression cast_expression conditional_expression multiplicative_expression additive_expression shift_expression relational_expression equality_expression and_expression xor_expression or_expression logical_and_expression logical_or_expression
%type <argument_expression_list> argument_expression_list
%type <expression_list> expression

%type <declaration> declaration
%type <declaration_specifiers> declaration_specifiers
%type <init_declarator_list> init_declarator_list
%type <init_declarator> init_declarator
%type <intval> storage_class_specifier
%type <type_specifier> type_specifier
%type <struct_or_union_specifier> struct_or_union_specifier
%type <terminal> struct_or_union
%type <struct_declaration_set> struct_declaration_set
%type <struct_declaration_list_access> struct_declaration_list_access
%type <struct_declaration_list> struct_declaration_list
%type <class_specifier> class_specifier
%type <class_declarator_list> class_declarator_list
%type <class_declarator> class_declarator
%type <class_declaration_list> class_declaration_list
%type <class_declaration> class_declaration
%type <terminal> access_specifier
%type <struct_declaration> struct_declaration
%type <specifier_qualifier_list> specifier_qualifier_list
%type <struct_declarator_list> struct_declarator_list
%type <struct_declarator> struct_declarator
%type <enum_specifier> enum_specifier
%type <enumerator_list> enumerator_list
%type <enumerator> enumerator
%type <intval> type_qualifier
%type <declarator> declarator
%type <direct_declarator> direct_declarator
%type <pointer> pointer
%type <type_qualifier_list> type_qualifier_list
%type <parameter_type_list> parameter_type_list
%type <parameter_list> parameter_list
%type <parameter_declaration> parameter_declaration
%type <identifier_list> identifier_list
%type <type_name> type_name
%type <abstract_declarator> abstract_declarator
%type <direct_abstract_declarator> direct_abstract_declarator
%type <initializer> initializer
%type <declaration_list> declaration_list
%type <translation_unit> translation_unit
%type <external_declaration> external_declaration
%type <function_definition> function_definition
%type <statement> statement compound_statement labeled_statement expression_statement selection_statement iteration_statement jump_statement
%type <statement_list> statement_list
%type <declaration_statement_list> declaration_statement_list

%token <intval> TYPEDEF EXTERN STATIC AUTO REGISTER CONST VOLATILE
%token <strval> CASE DEFAULT DO ELSE ENUM FOR
%token <strval> IF SWITCH
%token <strval> WHILE UNTIL CLASS ASSEMBLY_DIRECTIVE
%token <strval> ELLIPSIS 
%token <strval> INHERITANCE_OP 
%token <strval> SEMICOLON LEFT_CURLY RIGHT_CURLY LEFT_PAREN RIGHT_PAREN LEFT_SQUARE RIGHT_SQUARE COMMA COLON QUESTION
%token <strval> NEWLINE ERROR SINGLE_QUOTE DOUBLE_QUOTE 
%type <strval> error_case

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE 
%nonassoc LOW_PREC
%nonassoc HIGH_PREC
%debug
%start translation_unit

%%

error_case:
    I_CONSTANT IDENTIFIER { yyerror("Invalid Identifier"); has_error=1; yyclearin;}
    | DIVIDE MULTIPLY { yyerror("Unterminated Comment"); has_error=1; yyclearin;}
    | ERROR { has_error=1; yyclearin;}
    | error {  has_error=1; yyclearin;}
    ;

//DONE
primary_expression:
    IDENTIFIER {$$ = create_primary_expression($1);}
    | I_CONSTANT {$$ = create_primary_expression($1);}
    | F_CONSTANT {$$ = create_primary_expression($1);}
    | CHAR_CONSTANT {$$ = create_primary_expression($1);}
    | STRING_LITERAL {$$ = create_primary_expression($1);}
    | LEFT_PAREN primary_expression RIGHT_PAREN {$$ = create_primary_expression($2);} 
    ;

//DONE
argument_expression_list:
    assignment_expression {$$ = create_argument_expression_list($1);}
    | argument_expression_list COMMA assignment_expression {$$ = create_argument_expression_list($1, $3);}
    ;

//DONE
postfix_expression:
    primary_expression {$$ = create_postfix_expression($1);}
    | postfix_expression LEFT_SQUARE expression RIGHT_SQUARE { $$ = create_postfix_expression($1, $3);}
    | postfix_expression LEFT_PAREN argument_expression_list RIGHT_PAREN {$$ = create_postfix_expression($1, $3);}
    | postfix_expression LEFT_PAREN RIGHT_PAREN  { $$ = create_postfix_expression_func($1, nullptr);}
    | postfix_expression DOT IDENTIFIER {$$ = create_postfix_expression($1, $2, $3);}
    | postfix_expression PTR_OP IDENTIFIER {$$ = create_postfix_expression($1, $2, $3);}
    | postfix_expression INC_OP {$$ = create_postfix_expression($1,$2);}
    | postfix_expression DEC_OP {$$ = create_postfix_expression($1,$2);}
    ;

//DONE
unary_expression:
    postfix_expression {$$ = create_unary_expression($1);}
    | INC_OP unary_expression {$$ = create_unary_expression($2, $1);}
    | DEC_OP unary_expression {$$ = create_unary_expression($2, $1);}
    | SIZEOF unary_expression {$$ = create_unary_expression($2, $1);}
    | unary_operator cast_expression {$$ = create_unary_expression_cast($2, $1);}
    | SIZEOF LEFT_PAREN type_name RIGHT_PAREN {$$ = create_unary_expression($1, $3);}
    ;

//DONE
unary_operator:
    BITWISE_AND {$$ = $1;}
    | MULTIPLY {$$ = $1;}
    | PLUS  {$$ = $1;}
    | MINUS {$$ = $1;}
    | NOT   {$$ = $1;}
    | BITWISE_NOT   {$$ = $1;}
    ;

//DONE
cast_expression: 
    unary_expression {$$ = create_cast_expression($1);} 
    | LEFT_PAREN type_name RIGHT_PAREN cast_expression  {$$ = create_cast_expression($2,$4);}
    ;

//DONE
multiplicative_expression:
    cast_expression {$$ = create_multiplicative_expression($1);}
    | multiplicative_expression MULTIPLY cast_expression {$$ = create_multiplicative_expression($1, $2, $3);}
    | multiplicative_expression DIVIDE cast_expression {$$ = create_multiplicative_expression($1, $2, $3);}
    | multiplicative_expression MOD cast_expression {$$ = create_multiplicative_expression($1, $2, $3);}
    ;

//DONE
additive_expression:
    multiplicative_expression {$$ = create_additive_expression($1);}
    | additive_expression PLUS multiplicative_expression {$$ = create_additive_expression($1, $2, $3);}
    | additive_expression MINUS multiplicative_expression {$$ = create_additive_expression($1, $2, $3);}
    ;

//DONE
shift_expression:
    additive_expression {$$ = create_shift_expression($1);}
    | shift_expression LEFT_OP additive_expression {$$ = create_shift_expression($1, $2, $3);}
    | shift_expression RIGHT_OP additive_expression {$$ = create_shift_expression($1, $2, $3);}
    ;

// DONE
relational_expression:
    shift_expression {$$ = create_relational_expression($1);}
    | relational_expression LESS shift_expression {$$ = create_relational_expression($1, $2, $3);}
    | relational_expression GREATER shift_expression {$$ = create_relational_expression($1, $2, $3);}
    | relational_expression LE_OP shift_expression {$$ = create_relational_expression($1, $2, $3);}
    | relational_expression GE_OP shift_expression {$$ = create_relational_expression($1, $2, $3);}
    ;

// DONE
equality_expression: 
    relational_expression {$$ = create_equality_expression($1);}
    | equality_expression EQ_OP relational_expression {$$ = create_equality_expression($1, $2, $3);}
    | equality_expression NE_OP relational_expression  {$$ = create_equality_expression($1, $2, $3);}
    ;

// DONE
and_expression:
    equality_expression {$$ = create_and_expression($1);}
    | and_expression BITWISE_AND equality_expression {$$ = create_and_expression($1, $2, $3);}
    ;

// DONE
xor_expression:
    and_expression {$$ = create_xor_expression($1);}
    | xor_expression BITWISE_XOR and_expression {$$ = create_xor_expression($1, $2, $3);}
    ;

// DONE
or_expression:
    xor_expression {$$ = create_or_expression($1);}
    | or_expression BITWISE_OR xor_expression  {$$ = create_or_expression($1, $2, $3);}
    ;

// DONE
logical_and_expression:
    or_expression {$$ = create_logical_and_expression($1);}
    | logical_and_expression LOGICAL_AND or_expression {$$ = create_logical_and_expression($1, $2, $3);}
    ;

// DONE
logical_or_expression:
    logical_and_expression {$$ = create_logical_or_expression($1);}
    | logical_or_expression LOGICAL_OR logical_and_expression {$$ = create_logical_or_expression($1, $2, $3);}
    ;

// DONE
conditional_expression:
    logical_or_expression {$$ = create_conditional_expression($1);}
    | logical_or_expression QUESTION expression COLON conditional_expression {$$ = create_conditional_expression($1, $3, $5);}
    ;

// DONE
assignment_expression:
    conditional_expression {$$ = create_assignment_expression($1);}
    | unary_expression assignment_operator assignment_expression  {$$ = create_assignment_expression($1, $2, $3);}
    ;

// DONE
assignment_operator:
    ASSIGN {$$ = $1;}
    | MUL_ASSIGN {$$ = $1;}
    | DIV_ASSIGN {$$ = $1;}
    | MOD_ASSIGN {$$ = $1;}
	| ADD_ASSIGN {$$ = $1;}
	| SUB_ASSIGN {$$ = $1;}
	| LEFT_ASSIGN {$$ = $1;}
	| RIGHT_ASSIGN {$$ = $1;}
	| AND_ASSIGN {$$ = $1;}
	| XOR_ASSIGN {$$ = $1;}
	| OR_ASSIGN {$$ = $1;}
    ;

// DONE
expression:
    assignment_expression {$$ = create_expression_list($1);}
    | expression COMMA assignment_expression {$$ = create_expression_list($1, $3);}
    ;

// DONE
declaration:
    declaration_specifiers SEMICOLON     { $$ = create_declaration($1, nullptr);}
    | declaration_specifiers init_declarator_list SEMICOLON  {$$ = create_declaration( $1, $2 );}
    | error_case skip_until_semicolon SEMICOLON
    ;

// DONE
declaration_specifiers:
    storage_class_specifier declaration_specifiers {$$ = create_declaration_specifiers($2,$1);}
	| specifier_qualifier_list {$$ = create_declaration_specifiers($1);}                                                         
	| declaration_specifiers specifier_qualifier_list {$$ = create_declaration_specifiers($1,$2);}
    ;

// DONE
init_declarator_list:
    init_declarator { $$ = create_init_declarator_list( $1 ); }
    | init_declarator_list COMMA init_declarator {$$ = create_init_declarator_list($1, $3);}
    ;

// DONE
init_declarator:
    declarator {$$ = create_init_declarator($1, nullptr);}
    | declarator ASSIGN initializer {$$ = create_init_declarator($1,$3);}
    ;

// DONE
storage_class_specifier:
    TYPEDEF {$$ = $1;}
    | EXTERN {$$ = $1;}
    | STATIC {$$ = $1;}
    | AUTO {$$ = $1;}
    | REGISTER {$$ = $1;}
    ;

// DONE
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
    | TYPE_NAME {$$ = create_type_specifier($1, true);}
	| struct_or_union_specifier {$$ = create_type_specifier($1);}
	| enum_specifier            {$$ = create_type_specifier($1);}
    | class_specifier           {$$ = create_type_specifier($1);} 
	;

// DONE
struct_or_union_specifier:
    struct_or_union IDENTIFIER LEFT_CURLY {sus = create_struct_union_specifier($1->name,$2); Type t(-1,0,false); t.is_defined_type = true; symbolTable.enterScope(t,$2->value);} struct_declaration_set RIGHT_CURLY { $$ = create_struct_union_specifier(sus,$5); symbolTable.exitScope();}  
	// | struct_or_union LEFT_CURLY {symbolTable.enterScope();} struct_declaration_set RIGHT_CURLY {create_struct_union_specifier($1->name,nullptr,$4); symbolTable.exitScope();}  
	| struct_or_union IDENTIFIER {$$ = create_struct_union_specifier($1->name,$2,nullptr);}                  
	;

// DONE
struct_or_union:
    STRUCT {$$ = $1;}
    | UNION {$$ = $1;}
    ;

// DONE
struct_declaration_set:
    struct_declaration_list_access {$$ = create_struct_declaration_set($1);}
    | struct_declaration_set struct_declaration_list_access {$$ = create_struct_declaration_set($1,$2);}
    ;

// DONE
struct_declaration_list_access:
    struct_declaration_list {$$ = create_struct_declaration_list_access(nullptr,$1);}
    | access_specifier COLON struct_declaration_list {$$ = create_struct_declaration_list_access($1,$3);}
    ;

// DONE
struct_declaration_list:
    struct_declaration {$$ = create_struct_declaration_list($1);}
    | struct_declaration_list struct_declaration {$$ = create_struct_declaration_list($1, $2);}
    ;

// DONE
class_specifier:
    CLASS IDENTIFIER LEFT_CURLY {cs = create_class_specifier($2); Type t(-1,0,false); t.is_defined_type = true; symbolTable.enterScope(t,$2->value);} class_declaration_list RIGHT_CURLY {$$ = create_class_specifier(cs,nullptr,$5); symbolTable.exitScope();} 
    | CLASS IDENTIFIER INHERITANCE_OP class_declarator_list LEFT_CURLY {cs = create_class_specifier($2); Type t(-1,0,false); t.is_defined_type = true; symbolTable.enterScope(t,$2->value);} class_declaration_list RIGHT_CURLY {$$ = create_class_specifier(cs,$4,$7); symbolTable.exitScope();} 
    | CLASS IDENTIFIER {$$ = create_class_specifier($2,nullptr,nullptr);}
    ;

// DONE
class_declarator_list:
    class_declarator {$$ = create_class_declarator_list($1);}
    | class_declarator_list COMMA class_declarator {$$ = create_class_declarator_list($1,$3);}
    ;

// DONE
class_declarator:
    access_specifier declarator {$$ = create_class_declarator($1,$2);}
    | declarator {$$ = create_class_declarator(nullptr,$1);}
    ;

// DONE
class_declaration_list :
    class_declaration {$$ = create_class_declaration_list($1);}
    | class_declaration_list class_declaration {$$ = create_class_declaration_list($1,$2);}
    ;

// DONE
class_declaration:
    access_specifier COLON translation_unit {$$ = create_class_declaration($1,$3);}
    | translation_unit {$$ = create_class_declaration(nullptr,$1);}
    ;

// DONE
access_specifier:
    PUBLIC  {$$ = $1;}
    | PRIVATE {$$ = $1;}
    | PROTECTED {$$ = $1;}
    ; 

// DONE
struct_declaration:
    specifier_qualifier_list struct_declarator_list SEMICOLON {$$ = create_struct_declaration($1,$2);}
    ;

// DONE
specifier_qualifier_list:
    type_specifier {$$ = create_specifier_qualifier_list($1);}
    | specifier_qualifier_list type_specifier {$$ = create_specifier_qualifier_list($1,$2);}                      
	| type_qualifier specifier_qualifier_list {$$ = create_specifier_qualifier_list($2,$1);}
    | specifier_qualifier_list type_qualifier {$$ = create_specifier_qualifier_list($1,$2);}                  
	;

// DONE
struct_declarator_list:
    struct_declarator { $$ = create_struct_declarator_list( $1 ); }
	| struct_declarator_list COMMA struct_declarator {$$ = create_struct_declarator_list($1, $3);}
	;

//DONE
struct_declarator:
    declarator {$$ = create_struct_declarator($1,nullptr);}
	| COLON conditional_expression {$$ = create_struct_declarator(nullptr,$2);}
	| declarator COLON conditional_expression {$$ = create_struct_declarator($1,$3);}
	;

// DONE
enum_specifier:
    ENUM LEFT_CURLY enumerator_list RIGHT_CURLY { $$ = create_enumerator_specifier($3); }                
	| ENUM IDENTIFIER LEFT_CURLY enumerator_list RIGHT_CURLY { $$ = create_enumerator_specifier($2, $4); }    
	| ENUM IDENTIFIER { $$ = create_enumerator_specifier($2, nullptr); }                        
	;

// DONE
enumerator_list:
    enumerator { $$ = create_enumerator_list($1);}
    | enumerator_list COMMA enumerator { $$ = create_enumerator_list($1, $3);}
    ;

// DONE
enumerator:
    IDENTIFIER {$$ = create_enumerator($1,nullptr);}
    | IDENTIFIER ASSIGN conditional_expression {$$ = create_enumerator($1,$3);} 
    ;

// DONE
type_qualifier:
    CONST  {$$ = $1;}
    | VOLATILE  {$$ = $1;}
    ;

// DONE
declarator:
    pointer direct_declarator { $$ = create_declarator( $1, $2 ); } 
    | direct_declarator  { $$ = create_declarator( nullptr, $1 ); }
    ;

// DONE 
direct_declarator:
    IDENTIFIER  { $$ = create_dir_declarator_id( $1 ); }
    | direct_declarator LEFT_SQUARE conditional_expression RIGHT_SQUARE {$$ = create_direct_declarator_array($1, $3);}
	/* | direct_declarator LEFT_SQUARE RIGHT_SQUARE {$$ = create_direct_declarator_array($1, nullptr);}  */
    | LEFT_PAREN declarator RIGHT_PAREN { $$ = create_direct_declarator($2); } 
    | direct_declarator LEFT_PAREN parameter_type_list RIGHT_PAREN {$$ = create_direct_declarator_function($1, $3);}
    | direct_declarator LEFT_PAREN RIGHT_PAREN {$$ = create_direct_declarator_function($1, nullptr);} 
    ;

// DONE
pointer:
    MULTIPLY {$$ = create_pointer(nullptr);}
    | MULTIPLY type_qualifier_list {$$ = create_pointer($2);}
    | MULTIPLY pointer {$$ = create_pointer($2,nullptr);}
    | MULTIPLY type_qualifier_list pointer {$$ = create_pointer($3,$2);}
    ;

// DONE
type_qualifier_list:
    type_qualifier  {$$ = create_type_qualifier_list($1);}
    | type_qualifier_list type_qualifier  {$$ = create_type_qualifier_list($1,$2);}
    ;

// DONE
parameter_type_list:
    parameter_list {$$ = create_parameter_type_list($1,false);}
    | parameter_list COMMA ELLIPSIS {$$ = create_parameter_type_list($1,true);}
    ;

// DONE
parameter_list:
    parameter_declaration {$$ = create_parameter_list($1);}
    | parameter_list COMMA parameter_declaration {$$ = create_parameter_list($1,$3);}
    ;

// DONE
parameter_declaration:
    declaration_specifiers declarator {$$ = create_parameter_declaration($1,$2);}
	| declaration_specifiers abstract_declarator {$$ = create_parameter_declaration($1,$2);}
	| declaration_specifiers {$$ = create_parameter_declaration($1);}                   
	;

// DONE
identifier_list:
    IDENTIFIER {$$ = create_identifier_list($1);}
	| identifier_list COMMA IDENTIFIER {$$ = create_identifier_list($1,$3);}
	;

// DONE
type_name:
    specifier_qualifier_list {$$ = create_type_name($1,nullptr);}
	| specifier_qualifier_list abstract_declarator {$$ = create_type_name($1,$2);}
	;

// DONE
abstract_declarator:
    pointer {$$ = create_abstract_declarator($1,nullptr);}
	| direct_abstract_declarator {$$ = create_abstract_declarator(nullptr,$1);}
	| pointer direct_abstract_declarator {$$ = create_abstract_declarator($1,$2);}
	;

// DONE
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

// DONE
initializer:
    assignment_expression {$$ = create_initializer($1);}
    // | LEFT_CURLY {symbolTable.enterScope();} initializer_list RIGHT_CURLY {symbolTable.exitScope();$$ = create_initializer($2);}
    // | LEFT_CURLY {symbolTable.enterScope();} initializer_list COMMA RIGHT_CURLY {symbolTable.exitScope(); {$$ = create_initializer($2);}}
    ;

// REMOVED
// initializer_list:
//     initializer
//     | initializer_list COMMA initializer
//     ;

statement:
    labeled_statement 
	| compound_statement
    | expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
    | error_case skip_until_semicolon
	;

// DONE
labeled_statement:
    IDENTIFIER COLON statement {$$ = create_labeled_statement_identifier($1,$3);}
	| CASE conditional_expression COLON statement {$$ = create_labeled_statement_case($2,$4);}
	| DEFAULT COLON statement {$$ = create_labeled_statement_default($3);}
	;

compound_statement:
    LEFT_CURLY {Type t(-1,0,false); if(function_flag == 1) function_flag = 0; else symbolTable.enterScope(t, "");} RIGHT_CURLY {symbolTable.exitScope(); $$ = create_compound_statement();}
    // left to implement will be done after remaining classes
    | LEFT_CURLY {Type t(-1,0,false); if(function_flag == 1) function_flag = 0; else symbolTable.enterScope(t, "");} declaration_statement_list RIGHT_CURLY {symbolTable.exitScope();} 
    ;

// DONE
declaration_statement_list:
    declaration_list {$$ = create_declaration_statement_list($1);}
    | statement_list {$$ = create_declaration_statement_list($1);}
    | declaration_statement_list declaration_list {$$ = create_declaration_statement_list($2);}
    | declaration_statement_list statement_list {$$ = create_declaration_statement_list($2);}
    ;

// DONE
declaration_list:
    declaration { $$ = create_declaration_list($1);}
    | declaration_list declaration { $$ = create_declaration_list($1, $2);}
    ;

// DONE
statement_list:
    statement { $$ = create_statement_list($1);}
    | statement_list statement { $$ = create_statement_list($1, $2);}
    ;

// DONE
expression_statement:
    SEMICOLON {$$ = create_expression_statement();}
    | expression SEMICOLON {$$ = create_expression_statement($1);}
    ;

// DONE
selection_statement:
    IF LEFT_PAREN expression RIGHT_PAREN statement {$$ = create_selection_statement_if($3,$5);}
    | IF LEFT_PAREN expression RIGHT_PAREN statement ELSE statement {$$ = create_selection_statement_if_else($3,$5,$7);}
    | SWITCH LEFT_PAREN expression RIGHT_PAREN statement {$$ = create_selection_statement_switch($3,$5);}

// DONE
iteration_statement:
    WHILE LEFT_PAREN expression RIGHT_PAREN statement {$$ = create_iteration_statement_while($3,$5);}
    | DO statement WHILE LEFT_PAREN expression RIGHT_PAREN SEMICOLON {$$ = create_iteration_statement_do_while($5,$2);}
    | FOR LEFT_PAREN expression_statement expression_statement RIGHT_PAREN statement {$$ = create_iteration_statement_for($3,$4,nullptr,$6);}
    | FOR LEFT_PAREN expression_statement expression_statement expression RIGHT_PAREN statement {$$ = create_iteration_statement_for($3,$4,$5,$7);}
    | FOR LEFT_PAREN declaration expression_statement RIGHT_PAREN statement {$$ = create_iteration_statement_for_dec($3,$4,nullptr,$6);}
    | FOR LEFT_PAREN declaration expression_statement expression RIGHT_PAREN statement {$$ = create_iteration_statement_for_dec($3,$4,$5,$7);}
    | UNTIL LEFT_PAREN expression RIGHT_PAREN statement {$$ = create_iteration_statement_until($3,$5);}
    ;

// DONE
jump_statement:
    GOTO IDENTIFIER SEMICOLON {$$ = create_jump_statement($1);}
	| CONTINUE SEMICOLON {$$ = create_jump_statement($1);}
	| BREAK SEMICOLON {$$ = create_jump_statement($1);}
	| RETURN SEMICOLON {$$ = create_jump_statement($1);}
	| RETURN expression SEMICOLON {$$ = create_jump_statement($2);}
	;

// DONE
translation_unit:
    external_declaration {$$ = create_translation_unit($1);}
    | translation_unit external_declaration {$$ = create_translation_unit($1,$2);}
    ;

// DONE
external_declaration:
	function_definition {create_external_declaration($1);}
	| declaration {create_external_declaration($1);}
	;

// DONE
function_definition:
    declaration_specifiers declarator {fd = create_function_definition($1,$2); function_flag=1;} compound_statement {$$ = create_function_definition(fd,$4);}
    ;

skip_until_semicolon:
    SEMICOLON   // Stop at semicolon and reset error handling
    | error 
    | skip_until_semicolon error  // Consume any unexpected token
    ;

%%

void yyerror(const char *msg) {
    /* fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, msg); */
    string error_msg = "Syntax error at line " + to_string(yylineno) + ": " + msg;
    has_error = 1;
    debug(error_msg, RED);
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
    if(has_error) {
        debug("Parsing failed due to errors.", RED);
        return 1;
    }
    symbolTable.print_defined_types();
    symbolTable.print_typedefs();
    symbolTable.print();
    printf("Parsing completed successfully.\n");
    return 0;
}
