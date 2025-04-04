#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <list>
#include "ast.h"
#include "utils.h"

class Expression;
class PrimaryExpression;
class ConditionalExpression;
class AssignmentExpression;
class Type; 
class TypeDefinition;
class DefinedTypes;
class TypeQualifierList;
class DeclarationList;
class Declaration;
class InitializerList;
class Initializer;
class IdentifierList;
class DeclarationSpecifiers;
class Pointer;
class InitDeclaratorList;
class InitDeclarator;
class DirectDeclarator;
class Declarator;
class ParameterDeclaration;
class ParameterList;
class ParameterTypeList;
class AbstractDeclarator;
class DirectAbstractDeclarator;
class StructUnionSpecifier;
class StructDeclarator;
class StructDeclaration;
class ClassSpecifier;
class ClassDeclarationList;
class ClassDeclarator;
class ClassDeclaration;
class ClassDeclaratorList;
class StructDeclarationSet;
class StructDeclarationListAccess;
class StructDeclarationList;
class StructDeclaratorList;
class Enumerator;
class EnumeratorList;
class TypeSpecifier;
class SpecifierQualifierList;
class TypeName;
class Initializer;
class FunctionDefinition;
class ExternalDeclaration;
class TranslationUnit;
class Identifier;
class Constant;
class StringLiteral;
class Symbol;
class SymbolTable;
class Statement;
class LabeledStatement;
class CompoundStatement;
class DeclarationStatementList;
class StatementList;
class ExpressionStatement;
class SelectionStatement;
class IterationStatement;
class JumpStatement;

extern Type ERROR_TYPE;

enum PrimitiveTypes
{
    TYPE_ERROR_T = -1,
    U_CHAR_T = 0,
    CHAR_T = 1,
    U_SHORT_T = 2,
    SHORT_T = 3,
    U_INT_T = 4,
    INT_T = 5,
    U_LONG_T = 6,
    LONG_T = 7,
    U_LONG_LONG_T = 8,
    LONG_LONG_T = 9,
    FLOAT_T = 10,
    DOUBLE_T = 11,
    LONG_DOUBLE_T = 12,
    VOID_T = 13,
    N_PRIMITIVE_TYPES = 14,
    VOID_STATEMENT_T = 15,
};

static unordered_map<int, string> primitive_type_name = {
    {TYPE_ERROR_T, "error"},
    {U_CHAR_T, "unsigned char"},
    {CHAR_T, "char"},
    {U_SHORT_T, "unsigned short"},
    {SHORT_T, "short"},
    {U_INT_T, "unsigned int"},
    {INT_T, "int"},
    {U_LONG_T, "unsigned long"},
    {LONG_T, "long"},
    {U_LONG_LONG_T, "unsigned long long"},
    {LONG_LONG_T, "long long"},
    {FLOAT_T, "float"},
    {DOUBLE_T, "double"},
    {LONG_DOUBLE_T, "long double"},
    {VOID_T, "void"}
};

static unordered_map<int,int> primitive_type_size = {
    {U_CHAR_T, 1},
    {CHAR_T, 1},
    {U_SHORT_T, 2},
    {SHORT_T, 2},
    {U_INT_T, 4},
    {INT_T, 4},
    {U_LONG_T, 4},
    {LONG_T, 4},
    {U_LONG_LONG_T, 8},
    {LONG_LONG_T, 8},
    {FLOAT_T, 4},
    {DOUBLE_T, 8},
    {LONG_DOUBLE_T, 16},
};

enum TypeCategory {
    TYPE_CATEGORY_ERROR = -1,
    TYPE_CATEGORY_PRIMITIVE,
    TYPE_CATEGORY_POINTER,
    TYPE_CATEGORY_ARRAY,
    TYPE_CATEGORY_FUNCTION,
    TYPE_CATEGORY_CLASS,
    TYPE_CATEGORY_STRUCT,
    TYPE_CATEGORY_UNION,
    N_TYPE_CATEGORIES,
};

static unordered_map<int, string> type_category_name = {
    {TYPE_CATEGORY_ERROR, "error"},
    {TYPE_CATEGORY_PRIMITIVE, "primitive"},
    {TYPE_CATEGORY_POINTER, "pointer"},
    {TYPE_CATEGORY_ARRAY, "array"},
    {TYPE_CATEGORY_FUNCTION, "function"},
    {TYPE_CATEGORY_CLASS, "class"},
    {TYPE_CATEGORY_STRUCT, "struct"},
    {TYPE_CATEGORY_UNION, "union"}
};

// enum TypeQualifiers
// {
//     TYPE_QUALIFIERS_CONST = 0,
//     TYPE_QUALIFIERS_VOLATILE = 1
// };

// enum StorageClassSpecifiers
// {
//     STORAGE_CLASS_TYPEDEF = 0,
//     STORAGE_CLASS_EXTERN,
//     STORAGE_CLASS_STATIC,
//     STORAGE_CLASS_AUTO,
//     STORAGE_CLASS_REGISTER,
// };

enum AccessSpecifiers{
    ACCESS_SPECIFIER_PUBLIC = 0,
    ACCESS_SPECIFIER_PRIVATE,
    ACCESS_SPECIFIER_PROTECTED,
};

// ##############################################################################
// ################################## TYPE ######################################
// ##############################################################################

class Type
{
public:
    int typeIndex;

    bool is_pointer;
    int ptr_level;

    bool is_array;
    int array_dim;
    vector<int> array_dims;

    bool is_function;
    int num_args;
    vector<Type> arg_types;

    bool is_defined_type;
    string defined_type_name;

    bool is_const_variable;
    bool is_const_literal;

    Type();

    Type(int idx, int p_lvl, bool is_con);
    bool isPrimitive();
    bool isInt();
    bool isChar();
    bool isFloat();
    bool isIntorFloat();
    bool isUnsigned();
    bool isPointer();
    bool is_error();
    void make_signed();
    void make_unsigned();
    bool isVoid();
    bool is_ea(); // is effective array
    bool is_convertible_to(Type t); // whether implicit conversion is possible
    Type promote_to_int(Type t); // IMPLEMENT
    int get_size();
    void debug_type();

    friend bool operator==(const Type &obj1, const Type &obj2);
    friend bool operator!=(const Type &obj1, const Type &obj2);
};

// ##############################################################################
// ################################## DEFINED TYPES ######################################
// ##############################################################################

class DefinedTypes : public Type
{
    public:
        static int t_index_count;
        TypeCategory type_category;
        TypeDefinition *type_definition;
        int relative_offset = 0;
        int size = 0;
        DefinedTypes(TypeCategory tc, TypeDefinition *td);
};

// ##############################################################################
// ################################## SYMBOL ######################################
// ##############################################################################

class Symbol
{
public:
    std::string name;
    Type type;
    int scope;
    int offset;
    FunctionDefinition* function_definition;

    Symbol(string n, Type t, int s, int o) : name(n), type(t), scope(s), offset(o) {
        function_definition = nullptr;
    }
};

// ##############################################################################
// ################################## SYMBOL TABLE ######################################
// ##############################################################################

class SymbolTable
{
public:
    std::unordered_map<std::string, std::list<Symbol *>> table;
    std::unordered_map<std::string, std::list<std::pair<int, DefinedTypes*>>> defined_types;
    std::unordered_map<std::string, std::list<Symbol *>> typedefs;
    int currentScope;
    bool error;
    stack<pair<int,pair<Type,string>>> scope_stack;

    SymbolTable();
    void enterScope(Type t, string name);
    void exitScope();
    void insert(std::string name, Type type, int offset, int overloaded);
    void insert_defined_type(std::string name, DefinedTypes* type);
    void insert_typedef(std::string name, Type type, int offset);
    bool lookup(std::string name);
    bool lookup_function(std::string name, vector<Type> arg_types);
    bool lookup_defined_type(string name);
    bool lookup_typedef(string name);
    bool check_member_variable(string name, string member);
    Type get_type_of_member_variable(string name, string member);
    Type get_type_of_member_variable(string name, string member, vector<Type> arg_types);
    Symbol *getSymbol(std::string name);
    Symbol *getFunction(std::string name, vector<Type> arg_types);
    Symbol* getTypedef(std::string name);
    DefinedTypes* get_defined_type(std::string name);
    void update(std::string name, Type newType);
    void remove(std::string name);
    void print();
    void print_typedefs();
    void print_defined_types();
    void set_error();
    bool has_error();
};

extern SymbolTable symbolTable;

// ##############################################################################
// ################################## TYPE DEFINITION ######################################
// ##############################################################################

class TypeDefinition 
{
public:
    TypeCategory type_category;
    unordered_map<string,AccessSpecifiers> members;
    bool get_member(string member);
    AccessSpecifiers get_member_access_specifier(string member);
    SymbolTable type_symbol_table;
    int get_size();
    TypeDefinition(TypeCategory tc);
};

TypeDefinition *create_type_definition(TypeDefinition* td, StructDeclarationSet *sd);
TypeDefinition *create_type_definition(TypeDefinition* td, ClassDeclaratorList* idl, ClassDeclarationList* cdl);

// typedef enum direct_declarator_enum {
//     IDENTIFIER,
//     DECLARATOR,
//     ARRAY,
//     FUNCTION,
//     NUM_DIRECT_DECLARATORS

// } DIRECT_DECLARATOR_TYPE;

// ##############################################################################
// ################################## TYPE QUALIFIER LIST ######################################
// ##############################################################################

class TypeQualifierList : public NonTerminal
{
    // Fully implemented
public:
    vector<int> type_qualifier_list;
    TypeQualifierList();
};

TypeQualifierList *create_type_qualifier_list(int typequalifier);
TypeQualifierList *create_type_qualifier_list(TypeQualifierList *typequalifierlist, int typequalifier);

// ##############################################################################
// ################################## DECLARATION ######################################
// ##############################################################################

class Declaration : public NonTerminal
{
public:
    DeclarationSpecifiers *declaration_specifiers;
    InitDeclaratorList *init_declarator_list;
    Declaration();
};

Declaration* create_declaration(DeclarationSpecifiers *declaraion_specifiers, InitDeclaratorList *init_declarator_list);

// ##############################################################################
// ################################## DECLARATION LIST ######################################
// ##############################################################################

class DeclarationList : public NonTerminal
{   
public:
    vector<Declaration*> declaration_list;
    DeclarationList();
};

DeclarationList* create_declaration_list(DeclarationList* dl, Declaration* d);
DeclarationList* create_declaration_list(Declaration* d);

// ##############################################################################
// ################################## INITIALIZER LIST ######################################
// ##############################################################################

// ##############################################################################
// ################################## IDENTIFIER LIST ######################################
// ##############################################################################

class IdentifierList : public NonTerminal
{
    // Fully Implemented
public:
    vector<Identifier *> identifiers;
    IdentifierList();
};

IdentifierList *create_identifier_list(Identifier* i);
IdentifierList *create_identifier_list(IdentifierList* il, Identifier* i);

// ##############################################################################
// ################################## DECLARATION SPECIFIERS ######################################
// ##############################################################################

class DeclarationSpecifiers : public NonTerminal
{
public:
    vector<int> storage_class_specifiers;
    vector<TypeSpecifier *> type_specifiers;
    vector<int> type_qualifiers;
    bool is_const_variable;
    bool is_typedef;
    bool is_type_name;
    int type_index;
    DeclarationSpecifiers();
    void set_type();
};

DeclarationSpecifiers *create_declaration_specifiers(SpecifierQualifierList *sql);
DeclarationSpecifiers *create_declaration_specifiers(DeclarationSpecifiers *ds, int storage_class);
DeclarationSpecifiers *create_declaration_specifiers(DeclarationSpecifiers *ds, SpecifierQualifierList *sql);

// ##############################################################################
// ################################## POINTER ######################################
// ##############################################################################

class Pointer : public NonTerminal
{
    // Fully Implemented
public:
    TypeQualifierList *type_qualifier_list;
    int pointer_level;
    Pointer();
};

Pointer *create_pointer(TypeQualifierList *tql);
Pointer *create_pointer(Pointer *p, TypeQualifierList *tql);

// ##############################################################################
// ################################## INIT DECLARATOR ######################################
// ##############################################################################

class InitDeclarator : public NonTerminal
{
    // Implement after Declarator, Initializer
    public:
        Declarator* declarator;
        Initializer* initializer;
        InitDeclarator();
};

InitDeclarator* create_init_declarator(Declarator* d, Initializer* i);

// ##############################################################################
// ################################## DIRECT DECLARATOR ######################################
// ##############################################################################

class DirectDeclarator : public NonTerminal
{
    // Implement after identifier_list and conditional expression
public:
    // DIRECT_DECLARATOR_TYPE direct_declarator_type;
    Declarator *declarator;
    Identifier *identifier;
    ParameterTypeList *parameters; // Stores function parameters if applicable
    bool is_function;              // Flag to check if it's a function
    bool is_array;                 // Flag to check if it's an array
    vector<int> array_dimensions;  // Stores array dimensions if applicable
    DirectDeclarator();
};

DirectDeclarator *create_dir_declarator_id(Identifier *i);
DirectDeclarator *create_direct_declarator(Declarator *d);
DirectDeclarator *create_direct_declarator_array(DirectDeclarator *dd, Expression *e);
DirectDeclarator *create_direct_declarator_function(DirectDeclarator *dd, ParameterTypeList *ptl);
  
// ##############################################################################
// ################################## DECLARATOR ######################################
// ##############################################################################

class Declarator : public NonTerminal
{
    // Fully Implemented
public:
    Pointer* pointer;
    DirectDeclarator *direct_declarator;
    Declarator();
};

Declarator *create_declarator( Pointer *pointer, DirectDeclarator *direct_declarator);

// ##############################################################################
// ################################## PARAMETER DECLARATION ######################################
// ##############################################################################

class ParameterDeclaration : public NonTerminal
{
    // Fully Implemented
public:
    DeclarationSpecifiers *declarations_specifiers;
    AbstractDeclarator *abstract_declarator;
    Declarator *declarator;
    Type type;
    ParameterDeclaration();
};

ParameterDeclaration *create_parameter_declaration(DeclarationSpecifiers *ds, AbstractDeclarator *ad);
ParameterDeclaration *create_parameter_declaration(DeclarationSpecifiers *ds, Declarator *d);
ParameterDeclaration *create_parameter_declaration(DeclarationSpecifiers *ds);

// ##############################################################################
// ################################## PARAMETER LIST ######################################
// ##############################################################################

class ParameterList : public NonTerminal
{
    // Fully Implemented
public:
    vector<ParameterDeclaration *> parameter_declarations;
    ParameterList();
};

ParameterList *create_parameter_list(ParameterDeclaration *pd);
ParameterList *create_parameter_list(ParameterList *p, ParameterDeclaration *pd);

// ##############################################################################
// ################################## PARAMETER TYPE LIST ######################################
// ##############################################################################

class ParameterTypeList : public NonTerminal
{
    // Fully Implemented
public:
    ParameterList *paramater_list;
    bool is_variadic;
    ParameterTypeList();
};

ParameterTypeList *create_parameter_type_list(ParameterList *p, bool var);

// ##############################################################################
// ################################## DECLARATOR LIST ######################################
// ##############################################################################

class InitDeclaratorList : public NonTerminal
{
public:
    vector<InitDeclarator *> init_declarator_list;
    InitDeclaratorList();
};

InitDeclaratorList *create_init_declarator_list(InitDeclarator *init_declarator);
InitDeclaratorList *create_init_declarator_list(InitDeclaratorList *init_declarator_list, InitDeclarator *init_declarator);

// ##############################################################################
// ################################## ABSTRACY DECLARATOR######################################
// ##############################################################################

class AbstractDeclarator : public NonTerminal
{
    // Fully Implemented
public:
    Pointer *pointer;
    DirectAbstractDeclarator *direct_abstract_declarator;
    AbstractDeclarator();
};

AbstractDeclarator *create_abstract_declarator(Pointer *p, DirectAbstractDeclarator *dad);

// ##############################################################################
// ################################## DIRECT ABSTRACT DECLARATOR ######################################
// ##############################################################################

class DirectAbstractDeclarator : public NonTerminal
{
    // Fully Implemented
public:
    AbstractDeclarator *abstract_declarator;
    bool is_function;              // Flag to check if it's a function
    ParameterTypeList *parameters; // Stores function parameters if applicable
    bool is_array;                 // Flag to check if it's an array
    vector<int> array_dimensions;  // Stores array dimensions if applicable
    DirectAbstractDeclarator();
};

DirectAbstractDeclarator *create_direct_abstract_declarator(AbstractDeclarator *x);
DirectAbstractDeclarator *create_direct_abstract_declarator_array(Expression *c);
DirectAbstractDeclarator *create_direct_abstract_declarator_function(ParameterTypeList *p);
DirectAbstractDeclarator *create_direct_abstract_declarator_array(DirectAbstractDeclarator *x, Expression *c);
DirectAbstractDeclarator *create_direct_abstract_declarator_function(DirectAbstractDeclarator *x, ParameterTypeList *p);

// ##############################################################################
// ################################## STRUCT UNION SPECIFIER ######################################
// ##############################################################################

class StructUnionSpecifier : public NonTerminal
{
    public:
        TypeCategory type_category;
        Identifier *identifier;
        StructDeclarationSet *struct_declaration_set;

        StructUnionSpecifier();
};

StructUnionSpecifier *create_struct_union_specifier(string struct_or_union, Identifier *id);
StructUnionSpecifier *create_struct_union_specifier(StructUnionSpecifier* sus, StructDeclarationSet *sds);
StructUnionSpecifier* create_struct_union_specifier(string struct_or_union, Identifier* id, StructDeclarationSet* sds);

// ##############################################################################
// ################################## CLASS SPECIFIER ######################################
// ##############################################################################

class ClassSpecifier : public NonTerminal
{
    public:
        TypeCategory type_category;
        Identifier *identifier;
        ClassDeclarationList *class_declaration_list;
        ClassDeclaratorList* class_declarator_list;
        ClassSpecifier();
};

ClassSpecifier *create_class_specifier(Identifier *id, ClassDeclaratorList* idl, ClassDeclarationList *cdl);
ClassSpecifier* create_class_specifier(Identifier* id);
ClassSpecifier* create_class_specifier(ClassSpecifier* cs, ClassDeclaratorList* idl, ClassDeclarationList* cdl);

// ##############################################################################
// ################################## CLASS DECLARATOR LIST ######################################
// ##############################################################################

class ClassDeclaratorList : public NonTerminal{
    public:
        vector<ClassDeclarator*> class_declarator_list;
        ClassDeclaratorList();
};

ClassDeclaratorList* create_class_declarator_list(ClassDeclarator* cd);
ClassDeclaratorList* create_class_declarator_list(ClassDeclaratorList* cdl, ClassDeclarator* cd);

// ##############################################################################
// ################################## CLASS DECLARATOR ######################################
// ##############################################################################

class ClassDeclarator : public NonTerminal
{
    // Fully Implemented
    public:
        Terminal* access_specifier;
        Declarator* declarator;
        ClassDeclarator();
};

ClassDeclarator *create_class_declarator(Terminal* access_specifier, Declarator* d);

// ##############################################################################
// ################################## CLASS DECLARATION LIST ######################################
// ##############################################################################

class ClassDeclarationList : public NonTerminal
{
    // Fully Implemented
    public:
        vector<ClassDeclaration*> class_declaration_list;
        ClassDeclarationList();
};

ClassDeclarationList* create_class_declaration_list(ClassDeclaration* cd);
ClassDeclarationList* create_class_declaration_list(ClassDeclarationList* cdl, ClassDeclaration* cd);

// ##############################################################################
// ################################## CLASS DECLARATION ######################################
// ##############################################################################

class ClassDeclaration : public NonTerminal
{
    // Fully Implemented
    public:
        Terminal* access_specifier;
        TranslationUnit* translation_unit;
        ClassDeclaration();
};

ClassDeclaration* create_class_declaration(Terminal* access_specifier, TranslationUnit* tu);

// ##############################################################################
// ################################## STRUCT DECLARATION SET ######################################
// ##############################################################################

class StructDeclarationSet : public NonTerminal{
    public:
        vector<StructDeclarationListAccess*> struct_declaration_lists;
        StructDeclarationSet();
};

StructDeclarationSet* create_struct_declaration_set(StructDeclarationListAccess* sdla);
StructDeclarationSet* create_struct_declaration_set(StructDeclarationSet* sds, StructDeclarationListAccess* sdla);

// ##############################################################################
// ################################## STRUCT DECLARATION LIST ACCESS ######################################
// ##############################################################################

class StructDeclarationListAccess : public NonTerminal{
    public:
        StructDeclarationList* struct_declaration_list;
        Terminal* access_specifier;
        StructDeclarationListAccess();
}; 

StructDeclarationListAccess* create_struct_declaration_list_access(Terminal* access_specifier, StructDeclarationList* sdl);

// ##############################################################################
// ################################## STRUCT DECLARATION LIST ######################################
// ##############################################################################

class StructDeclarationList : public NonTerminal
{
    // Fully Implemented
public:
    vector<StructDeclaration *> struct_declaration_list;
    StructDeclarationList();
};

StructDeclarationList *create_struct_declaration_list(StructDeclaration *sd);
StructDeclarationList *create_struct_declaration_list(StructDeclarationList *sdl, StructDeclaration *sd);

// ##############################################################################
// ################################## STRUCT DECLARATION ######################################
// ##############################################################################

class StructDeclaration : public NonTerminal
{
public:
    // Fully Implemented
    SpecifierQualifierList *specifier_qualifier_list;
    StructDeclaratorList* struct_declarator_list;
    StructDeclaration();
    // void add_to_struct_definition(StructDefinition *);
};

StructDeclaration* create_struct_declaration(SpecifierQualifierList *sql, StructDeclaratorList *dl);

// ##############################################################################
// ################################## STRUCT DECLARATOR LIST ######################################
// ##############################################################################

class StructDeclaratorList : public NonTerminal
{
    // Fully Implemented
    public:
        vector<StructDeclarator*> struct_declarator_list;
        StructDeclaratorList();
};

StructDeclaratorList *create_struct_declarator_list(StructDeclarator* sd);
StructDeclaratorList *create_struct_declarator_list(StructDeclaratorList* sdl, StructDeclarator* sd);

// ##############################################################################
// ################################## STRUCT DECLARATOR ######################################
// ##############################################################################

class StructDeclarator : public NonTerminal
{
    // Fully Implemented
    public:
        Declarator* declarator;
        int bit_field_width;
        StructDeclarator();
};

StructDeclarator *create_struct_declarator(Declarator *sd, Expression *e);

// ##############################################################################
// ################################## ENUMERATOR ######################################
// ##############################################################################

class Enumerator : public NonTerminal
{
    // Fully Implemented
public:
    Identifier *identifier;
    ConditionalExpression *initializer_expression;
    Enumerator();
};

Enumerator *create_enumerator(Identifier *id, Expression *e);

// ##############################################################################
// ################################## ENUMERATOR LIST ######################################
// ##############################################################################

class EnumeratorList : public NonTerminal
{
    // Fully Implemented
public:
    vector<Enumerator *> enumerator_list;
    EnumeratorList();
};
EnumeratorList *create_enumerator_list(Enumerator *e);
EnumeratorList *create_enumerator_list(EnumeratorList *el, Enumerator *e);

// ##############################################################################
// ################################## ENUM SPECIFIER ######################################
// ##############################################################################

class EnumSpecifier : public NonTerminal
{
    public:
        Identifier *identifier;
        EnumeratorList *enumerators;
        EnumSpecifier();
};
EnumSpecifier *create_enumerator_specifier(EnumeratorList *enum_list);
EnumSpecifier *create_enumerator_specifier(Identifier *id, EnumeratorList *enum_list);

// ##############################################################################
// ################################## TYPE SPECIFIER ######################################
// ##############################################################################

class TypeSpecifier : public NonTerminal
{
    // Fully Implemented
public:
    Terminal* primitive_type_specifier;
    string type_name;
    EnumSpecifier *enum_specifier;
    StructUnionSpecifier *struct_union_specifier;
    ClassSpecifier *class_specifier;
    // int type_index;

    TypeSpecifier();
};

TypeSpecifier *create_type_specifier(Terminal* t);
TypeSpecifier *create_type_specifier(Terminal* t, bool is_type_name);
TypeSpecifier *create_type_specifier(EnumSpecifier* es);
TypeSpecifier *create_type_specifier(StructUnionSpecifier* sus);
TypeSpecifier* create_type_specifier(ClassSpecifier* cs);

// ##############################################################################
// ################################## SPECIFIER QUALIFIER LIST ######################################
// ##############################################################################

class SpecifierQualifierList : public NonTerminal
{
    // Fully Implemented
public:
    vector<TypeSpecifier *> type_specifiers;
    vector<int> type_qualifiers;
    bool is_const_variable;
    bool is_type_name;
    int type_index;
    void set_type();
    SpecifierQualifierList();
};

SpecifierQualifierList *create_specifier_qualifier_list(TypeSpecifier *t);
SpecifierQualifierList *create_specifier_qualifier_list(SpecifierQualifierList *s, TypeSpecifier *t);
SpecifierQualifierList *create_specifier_qualifier_list(SpecifierQualifierList *s, int tq);

// ##############################################################################
// ################################## TYPE NAME ######################################
// ##############################################################################

class TypeName : public NonTerminal
{
    // Fully Implemented
public:
    SpecifierQualifierList *specifier_qualifier_list;
    AbstractDeclarator *abstract_declarator;
    Type type;
    TypeName();
};

TypeName *create_type_name(SpecifierQualifierList *sql, AbstractDeclarator *ad);

// ##############################################################################
// ################################## INITIALIZER ######################################
// ##############################################################################

class Initializer : public NonTerminal
{
    // 
    public:
        AssignmentExpression *assignment_expression;
        Initializer();
};

Initializer *create_initializer(Expression *e);

// ##############################################################################
// ################################## TRANSLATION UNIT ######################################
// ##############################################################################

class TranslationUnit : public NonTerminal
{
    // Fully Implemented
    public: 
        vector<ExternalDeclaration*> external_declarations;
        TranslationUnit();
};

TranslationUnit *create_translation_unit(ExternalDeclaration* ed);
TranslationUnit *create_translation_unit(TranslationUnit* tu, ExternalDeclaration* ed);

// ##############################################################################
// ################################## EXTERNAL DECLARATION ######################################
// ##############################################################################

class ExternalDeclaration : public NonTerminal{
    // Fully Implemented
    public:
        FunctionDefinition* function_definition;
        Declaration* declaration;
        ExternalDeclaration();
};

ExternalDeclaration* create_external_declaration(FunctionDefinition* fd);
ExternalDeclaration* create_external_declaration(Declaration* d);

// ##############################################################################
// ################################## FUNCTION DEFINITION ######################################
// ##############################################################################

class FunctionDefinition : public NonTerminal{
    // Fully Implemented
    public:
        DeclarationSpecifiers *declaration_specifiers;
        Declarator *declarator;
        CompoundStatement* compound_statement;
        SymbolTable function_symbol_table;
        FunctionDefinition();
};

FunctionDefinition *create_function_definition(DeclarationSpecifiers *ds, Declarator *d);
FunctionDefinition* create_function_definition(Declarator* declarator, FunctionDefinition* fd, Statement* cs);

// ##############################################################################
// ################################ IDENTIFIER ##################################
// ##############################################################################

class Identifier : public Terminal
{
public:
    Identifier(string value, unsigned int line_no, unsigned int column_no);
};

// ##############################################################################
// ################################## CONSTANT ######################################
// ##############################################################################

class Constant : public Terminal
{
public:
    Type constant_type;
    Type set_constant_type(string value);
    Type get_constant_type();
    string convert_to_decimal();
    Constant(string name, string value, unsigned int line_no, unsigned int column_no);
};

// ##############################################################################
// ################################## STRING LITERAL ######################################
// ##############################################################################

class StringLiteral : public Terminal
{
public:
    StringLiteral(string value, unsigned int line_no, unsigned int column_no);
};


#endif
