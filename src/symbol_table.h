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
class StructDeclaration;
class ClassSpecifier;
class ClassDeclarationList;
class ClassDeclaration;
class StructDeclarationList;
class StructDeclaratorList;
class EnumSpecifier;
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

enum PrimitiveTypes {
    ERROR_T = -1,
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
};

enum Tokens
{
    ERROR_ = -1,
    UNSIGNED_ = 0,
    SHORT_ = 1,
    INT_ = 2,
    LONG_ = 3,
    CHAR_ = 4,
    DOUBLE_ = 5,
    FLOAT_ = 6,
    VOID_ = 7,
    ENUM_ = 8,
    UNION_ = 9,
    STRUCT_ = 10,
};

enum TypeQualifiers{
    CONST_ = 0,
    VOLATILE_ = 1
};

enum StorageClassSpecifiers{
    TYPEDEF_ = 0,
    EXTERN_ = 1,
    STATIC_ = 2,
    AUTO_ = 3,
    REGISTER_ = 4
};

class Type
{
public:
    int typeIndex;

    bool is_pointer;
    int ptr_level;

    bool is_array;
    unsigned int array_dim;
    std::vector<unsigned int> array_dims;

    bool is_function;
    unsigned int num_args;
    std::vector<Type> arg_types;
    bool is_defined;

    Type();

    Type(int idx, int p_lvl, bool is_con);
    bool is_const;
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
    bool is_invalid();
    bool is_ea();
    std::string get_name();
    size_t get_size();

    friend bool operator==(Type &obj1, Type &obj2);
    friend bool operator!=(Type &obj1, Type &obj2);
};

extern Type ERROR_TYPE;

// typedef enum direct_declarator_enum {
//     IDENTIFIER,
//     DECLARATOR,
//     ARRAY,
//     FUNCTION,
//     NUM_DIRECT_DECLARATORS

// } DIRECT_DECLARATOR_TYPE;

class TypeQualifierList : public NonTerminal{
    // Fully implemented
    public:
        vector<int> type_qualifier_list;
        TypeQualifierList();
};

TypeQualifierList* create_type_qualifier_list(int tq);
TypeQualifierList* create_type_qualifier_list(TypeQualifierList* x, int tq);

class DeclarationList : public NonTerminal{
    //implement after declaration
};

class Declaration : public NonTerminal{
    //Implement after DeclarationSpecifiers, InitDeclaratorList
};

class InitializerList : public NonTerminal{
    //Implement after initializer
};

class Initializer : public NonTerminal{
    //Implement after assignment expression
};

class IdentifierList : public NonTerminal{
    // Fully Implemented
    public:
        vector<Identifier*> identifiers;
        IdentifierList();
};

IdentifierList* create_identifier_list(Identifier* id);
IdentifierList* create_identifier_list(IdentifierList* x, Identifier* id);

class DeclarationSpecifiers : public NonTerminal{
    // Fully Implemented
    public:
        vector<int> storage_class_specifiers;
        vector<TypeSpecifier*> type_specifiers;
        vector<int> type_qualifiers;
        bool is_const;
        bool type_index;
        void set_type();
        DeclarationSpecifiers();
};

DeclarationSpecifiers* create_declaration_specifiers(SpecifierQualifierList* sql);
DeclarationSpecifiers* create_declaration_specifiers(DeclarationSpecifiers* x, int sc);
DeclarationSpecifiers* create_declaration_specifiers(DeclarationSpecifiers* x, SpecifierQualifierList* sql);


class Pointer : public NonTerminal{
    // Implement Now
    public:
        TypeQualifierList* type_qualifier_list;
        int pointer_level;
        Pointer();
};

Pointer* create_pointer(TypeQualifierList* tql);
Pointer* create_pointer(Pointer* x, TypeQualifierList* tql);

class InitDeclaratorList : public NonTerminal{
    //Implement after InitDeclarator
};

class InitDeclarator : public NonTerminal{
    // Implement Declarator, Initializer
};

class DirectDeclarator : public NonTerminal{
    // Implement after identifier_list and conditional expression
    public:
        // DIRECT_DECLARATOR_TYPE direct_declarator_type;
        Identifier* identifier;
        // vector<unsigned int> array_dimensions;
        // ParameterTypeList* parameter_list;
        DirectDeclarator();
};

DirectDeclarator *create_dir_declarator_id( // DIRECT_DECLARATOR_TYPE type,
    Identifier *id );

class Declarator : NonTerminal{
    // Implement after Pointer and DirectDeclarator
    public:
        Identifier* identifier;
        // Pointer* pointer;
        DirectDeclarator* direct_declarator;
        // Expression* initialising_expression;
        // Terminal* eq;
        // int get_pointer_level();
        Declarator(DirectDeclarator *direct_declarator);
        // Declarator(Pointer* p, DirectDeclarator* direct_declarator);
};

class ParameterDeclaration : public NonTerminal{
    //Implement after Declaration Specifiers and Declarator
    public:
        DeclarationSpecifiers* declarations_specifiers;
        AbstractDeclarator* abstract_declarator;
        Declarator* declarator;
        Type type;
        Type set_type(DeclarationSpecifiers* ds);
        ParameterDeclaration(DeclarationSpecifiers* ds);
};

ParameterDeclaration* create_parameter_declaration(DeclarationSpecifiers* ds, AbstractDeclarator* d);
ParameterDeclaration* create_paramater_declaration(DeclarationSpecifiers* ds, Declarator* d);

class ParameterList : public NonTerminal{
    public:
        vector<ParameterDeclaration*> parameter_declarations;
        ParameterList();
};

ParameterList* create_parameter_list(ParameterDeclaration* pd);
ParameterList* create_parameter_list(ParameterList* p, ParameterDeclaration* pd);


class ParameterTypeList : public NonTerminal{
    public:
        ParameterList* paramater_list;
        bool is_variadic;
        ParameterTypeList();
};

ParameterTypeList* create_parameter_type_list(ParameterList* p, bool var);

class DeclaratorList : public NonTerminal{
    public:
        vector<Declarator*> declarator_list;
        DeclaratorList();
};

class AbstractDeclarator : public NonTerminal{
    public:
        Pointer* pointer;
        DirectAbstractDeclarator* direct_abstract_declarator;
        AbstractDeclarator();
};

AbstractDeclarator* create_abstract_declarator(Pointer* p, DirectAbstractDeclarator* dad);

class DirectAbstractDeclarator : public NonTerminal{
    public:
        AbstractDeclarator* abstract_declarator;
        bool is_function; // Flag to check if it's a function
        bool is_array; // Flag to check if it's an array
        vector<ParameterTypeList*> parameters; // Stores function parameters if applicable
        vector<ConditionalExpression* > array_dimensions; // Stores array dimensions if applicable

        DirectAbstractDeclarator();
        
};

DirectAbstractDeclarator* create_direct_abstract_declarator(AbstractDeclarator* x);
DirectAbstractDeclarator* create_direct_abstract_declarator_array(Expression* c);
DirectAbstractDeclarator* create_direct_abstract_declarator_function(ParameterTypeList* p);
//DirectAbstractDeclarator* create_direct_abstract_declarator_array(DirectAbstractDeclarator* x, Expression* c);
//DirectAbstractDeclarator* create_direct_abstract_declarator_function(DirectAbstractDeclarator* x, ParameterTypeList* p);

class StructDefinition{
    public:
        unordered_map<string,Type> struct_members;
        int is_union;
        int is_struct;
        StructDefinition();
        Type get_member_type();
};

class StructUnionSpecifier : public NonTerminal{
    // Implement after StructDeclarationList
};

class StructDeclaration : public NonTerminal{
    // Implement after StructDeclaratorList
    public:
        SpecifierQualifierList* specifier_qualifier_list;
        StructDeclaratorList* declarator_list;

        StructDeclaration(SpecifierQualifierList* specifier_qualifier_list, DeclaratorList* declarator_list);
        void add_to_struct_definition(StructDefinition* );
};

class ClassSpecifier : public NonTerminal{
    // Implement after ClassDeclarationList, InitDeclaratorList
};

class ClassDeclarationList : public NonTerminal{
    // Implement after ClassDeclaration
};

class ClassDeclaration : public NonTerminal{
    // Implement after access specifier and TranslationUnit
};

class StructDeclarationList : public NonTerminal{
    //Implement after StrcuctDeclaration
    public:
        vector<StructDeclaration*> struct_declaration_list;
        StructDeclarationList();
};

class StructDeclaratorList : public NonTerminal{
    // Implement after StructDeclarator
};

class StructDeclarator : public NonTerminal{
    // Implement after Declarator, ConditionalExpression
};

class EnumSpecifier : public NonTerminal{
    // Implement after EnumeratorList
};

class Enumerator : public NonTerminal{
    // Implement after ConditionalExpression
    public:
        Identifier* identifier;
        Node* initializer_expression;
        Enumerator(Identifier* identifier, Node* initializer_expression);
};

class EnumeratorList : public NonTerminal{
    //Implement after Enumerator
    public:
        vector<Enumerator*> enumerator_list;
        EnumeratorList();
};

class TypeSpecifier : public Terminal{
    // Implement after StructUnionSpecifier, EnumSpecifier, ClassSpecifier
    public: 
        int type_specifier;
        Identifier* identifier;
        StructDeclarationList* struct_declaration_list;
        EnumeratorList* enumerator_list;
        int type_index;

        TypeSpecifier(int type_specifier, unsigned int line_no, unsigned int column_no);
        TypeSpecifier(int type_specifier, Identifier* identifier, StructDeclarationList* struct_declaration_list);
        TypeSpecifier(int type_specifier, Identifier* identifier, EnumeratorList* enumerator_list);

};

class SpecifierQualifierList : public NonTerminal{
    public:
        vector<TypeSpecifier*> type_specifiers;
        vector<int> type_qualifiers;
        bool is_const;
        int type_index;
        void set_type();
        SpecifierQualifierList();
};

SpecifierQualifierList* create_specifier_qualifier_list(TypeSpecifier* t);
SpecifierQualifierList* create_specifier_qualifier_list(SpecifierQualifierList* s, TypeSpecifier* t);
SpecifierQualifierList* create_specifier_qualifier_list(SpecifierQualifierList* s, int tq);

class TypeName : public NonTerminal{
    //ImplementNow
    public:
        SpecifierQualifierList* specifier_qualifier_list;
        AbstractDeclarator* abstract_declarator;
        Type type;
        TypeName();
        TypeName(SpecifierQualifierList* sql, AbstractDeclarator* ad);
};

//##############################################################################
//################################ IDENTIFIER ##################################
//##############################################################################

class Identifier : public Terminal{
    public:
        Identifier(string value, unsigned int line_no, unsigned int column_no);
};

class Constant : public Terminal{
    public:
        Type constant_type;
        Type set_constant_type(string value);
        Type get_constant_type();
        string convert_to_decimal();
        Constant(string name, string value, unsigned int line_no, unsigned int column_no);
};

class StringLiteral : public Terminal{
    public:
        StringLiteral(string value,unsigned int line_no, unsigned int column_no);
};

class Symbol
{
public:
    std::string name;
    Type type;
    int scope;
    int memoryAddr;

    Symbol(string n, Type t, int s, int m) : name(n), type(t), scope(s), memoryAddr(m) {}
};

class SymbolTable
{
private:
    std::unordered_map<std::string, std::list<Symbol*> > table;
    int currentScope;
    bool error;

public:
    SymbolTable();
    void enterScope();
    void exitScope();
    void insert(std::string name, Type type, int memoryAddr);
    bool lookup(std::string name);
    Symbol* getSymbol(std::string name);
    void update(std::string name, Type newType);
    void remove(std::string name);
    void print();
    void set_error();
    bool has_error();
};

extern SymbolTable symbolTable;

#endif
