#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <list>
#include "ast.h"

class Expression;
class PrimaryExpression;
class ConditionalExpression;
class Type;
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
class DeclaratorList;
class AbstractDeclarator;
class DirectAbstractDeclarator;
class StructDefinition;
class StructUnionSpecifier;
class StructDeclarator;
class StructDeclaration;
class ClassSpecifier;
class ClassDeclarationList;
class ClassDeclaration;
class StructDeclarationList;
class StructDeclaratorList;
class Enumerator;
class EnumeratorList;
class TypeSpecifier;
class SpecifierQualifierList;
class TypeName;
class Identifier;
class Constant;
class StringLiteral;
class Symbol;
class SymbolTable;

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

enum TypeQualifiers
{
    TYPE_QUALIFIERS_CONST = 0,
    TYPE_QUALIFIERS_VOLATILE = 1
};

enum StorageClassSpecifiers
{
    STORAGE_CLASS_TYPEDEF = 0,
    STORAGE_CLASS_EXTERN,
    STORAGE_CLASS_STATIC,
    STORAGE_CLASS_AUTO,
    STORAGE_CLASS_REGISTER,
};

static unordered_map<int,int> primitive_type_size = {{0,2},{1,2},{2,2},{3,2},{4,4},{5,4},{6,4},{7,4},{8,8},{9,8},{10,4},{11,8},{12,4}};

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
    bool is_ea();
    int get_size();

    friend bool operator==(Type &obj1, Type &obj2);
    friend bool operator!=(Type &obj1, Type &obj2);
};

// ##############################################################################
// ################################## TYPE DEFINITION ######################################
// ##############################################################################

class TypeDefinition 
{
public:
    TypeCategory type_category;
    vector<string> members;
    bool get_member(Identifier* id);
    int get_size();
};

TypeDefinition *create_type_definition(TypeCategory tc, StructDeclarationList *sdl);

// ##############################################################################
// ################################## DEFINED TYPES ######################################
// ##############################################################################

class DefinedTypes : public Type
{
public:
    static int t_index_count;
    TypeCategory type_category;
    TypeDefinition *type_definition;
    DefinedTypes(TypeCategory tc, TypeDefinition *td);
};

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
    DeclaratorList *init_declarator_list;
    Declaration();
};

Declaration* create_declaration(DeclarationSpecifiers *declaraion_specifiers,DeclaratorList *init_declarator_list);

// ##############################################################################
// ################################## DECLARATION LIST ######################################
// ##############################################################################

class DeclarationList : public NonTerminal
{
    // implement after declaration
};

// ##############################################################################
// ################################## INITIALIZER LIST ######################################
// ##############################################################################

class InitializerList : public NonTerminal
{
    // Implement after initializer
};

// ##############################################################################
// ################################## INITIALIZER ######################################
// ##############################################################################

class Initializer : public NonTerminal
{
    // Implement after assignment expression
};

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
// ################################## INIT DECLARATOR LIST ######################################
// ##############################################################################

class InitDeclaratorList : public NonTerminal
{
    // Implement after InitDeclarator
};

// ##############################################################################
// ################################## INIT DECLARATOR ######################################
// ##############################################################################

class InitDeclarator : public NonTerminal
{
    // Implement after Declarator, Initializer
};

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
    // Implement after Pointer and DirectDeclarator
public:
    Pointer* pointer;
    DirectDeclarator *direct_declarator;
    // Expression* initialising_expression;
    // Terminal* eq;
    // int get_pointer_level();
    Declarator();
    // Declarator(Pointer* p, DirectDeclarator* direct_declarator);
};

Declarator *create_declarator( Pointer *pointer, DirectDeclarator *direct_declarator);

// Declarator *create_declarator( // Pointer *pointer,
//     DirectDeclarator *direct_declarator);

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

class DeclaratorList : public NonTerminal
{
public:
    vector<Declarator *> declarator_list;
    DeclaratorList();
};

DeclaratorList *create_init_declarator_list(Declarator *init_declarator);
DeclaratorList *add_to_init_declarator_list(DeclaratorList *init_declarator_list, Declarator *init_declarator);

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

class StructDefinition
{
public:
    unordered_map<string, Type> struct_members;
    int is_union;
    int is_struct;
    StructDefinition();
    Type get_member_type();
};

class StructUnionSpecifier : public NonTerminal
{
    public:
        TypeCategory type_category;
        Identifier *identifier;
        StructDeclarationList *struct_declaration_list;

        StructUnionSpecifier();
};

StructUnionSpecifier *create_struct_union_specifier(string struct_or_union, Identifier *id, StructDeclarationList *sdl);

class ClassSpecifier : public NonTerminal
{
    // Implement after ClassDeclarationList, InitDeclaratorList
};

class ClassDeclarationList : public NonTerminal
{
    // Implement after ClassDeclaration
};

class ClassDeclaration : public NonTerminal
{
    // Implement after access specifier and TranslationUnit
};

class StructDeclarationList : public NonTerminal
{
    // Fully Implemented
public:
    vector<StructDeclaration *> struct_declaration_list;
    StructDeclarationList();
};

StructDeclarationList *create_struct_declaration_list(StructDeclaration *sd);
StructDeclarationList *create_struct_declaration_list(StructDeclarationList *sdl, StructDeclaration *sd);

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

class StructDeclaratorList : public NonTerminal
{
    // Fully Implemented
    public:
        vector<StructDeclarator*> struct_declarator_list;
        StructDeclaratorList();
};

StructDeclaratorList *create_struct_declarator_list(StructDeclarator* sd);
StructDeclaratorList *create_struct_declarator_list(StructDeclaratorList* sdl, StructDeclarator* sd);

class StructDeclarator : public NonTerminal
{
    // Fully Implemented
    public:
        Declarator* declarator;
        int bit_field_width;
        StructDeclarator();
};

StructDeclarator *create_struct_declarator(Declarator *sd, Expression *e);

class Enumerator : public NonTerminal
{
    // Fully Implemented
public:
    Identifier *identifier;
    ConditionalExpression *initializer_expression;
    Enumerator();
};

Enumerator *create_enumerator(Identifier *id, Expression *e);

class EnumeratorList : public NonTerminal
{
    // Fully Implemented
public:
    vector<Enumerator *> enumerator_list;
    EnumeratorList();
};
EnumeratorList *create_enumerator_list(Enumerator *e);
EnumeratorList *create_enumerator_list(EnumeratorList *el, Enumerator *e);

class EnumSpecifier : public NonTerminal
{
    public:
        Identifier *identifier;
        EnumeratorList *enumerators;
        EnumSpecifier();
};
EnumSpecifier *create_enumerator_specifier(EnumeratorList *enum_list);
EnumSpecifier *create_enumerator_specifier(Identifier *id, EnumeratorList *enum_list);

class TypeSpecifier : public NonTerminal
{
    // Fully Implemented
public:
    Terminal* primitive_type_specifier;
    EnumSpecifier *enum_specifier;
    StructUnionSpecifier *struct_union_specifier;
    // ClassSpecifier *class_specifier;
    // int type_index;

    TypeSpecifier();
    // TypeSpecifier(ClassSpecifier* class_specifier);
};

TypeSpecifier *create_type_specifier(Terminal* t);
TypeSpecifier *create_type_specifier(EnumSpecifier* es);
TypeSpecifier *create_type_specifier(StructUnionSpecifier* sus);
// TypeSpecifier* create_type_specifier(ClassSpecifier* class_specifier);

class SpecifierQualifierList : public NonTerminal
{
    // Fully Implemented
public:
    vector<TypeSpecifier *> type_specifiers;
    vector<int> type_qualifiers;
    bool is_const_variable;
    int type_index;
    void set_type();
    SpecifierQualifierList();
};

SpecifierQualifierList *create_specifier_qualifier_list(TypeSpecifier *t);
SpecifierQualifierList *create_specifier_qualifier_list(SpecifierQualifierList *s, TypeSpecifier *t);
SpecifierQualifierList *create_specifier_qualifier_list(SpecifierQualifierList *s, int tq);

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
// ################################ IDENTIFIER ##################################
// ##############################################################################

class Identifier : public Terminal
{
public:
    Identifier(string value, unsigned int line_no, unsigned int column_no);
};

class Constant : public Terminal
{
public:
    Type constant_type;
    Type set_constant_type(string value);
    Type get_constant_type();
    string convert_to_decimal();
    Constant(string name, string value, unsigned int line_no, unsigned int column_no);
};

class StringLiteral : public Terminal
{
public:
    StringLiteral(string value, unsigned int line_no, unsigned int column_no);
};

class Symbol
{
public:
    std::string name;
    Type type;
    int scope;
    int offset;

    Symbol(string n, Type t, int s, int o) : name(n), type(t), scope(s), offset(o) {}
};

class SymbolTable
{
public:
    std::unordered_map<std::string, std::list<Symbol *>> table;
    std::unordered_map<std::string, std::list<std::pair<int, DefinedTypes>>> defined_types;
    int currentScope;
    bool error;

    SymbolTable();
    void enterScope();
    void exitScope();
    void insert(std::string name, Type type, int offset);
    void insert_defined_type(std::string name, DefinedTypes type);
    bool lookup(std::string name);
    bool lookup_defined_type(string name);
    Symbol *getSymbol(std::string name);
    DefinedTypes get_defined_type(std::string name);
    void update(std::string name, Type newType);
    void remove(std::string name);
    void print();
    void set_error();
    bool has_error();
};

extern SymbolTable symbolTable;

#endif;
