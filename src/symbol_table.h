#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <list>
#include "ast.h"
#include "expression.h"

class Expression;
class PrimaryExpression;
class Type;
class Identifier;

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
    ERROR = -1,
    UNSIGNED = 0,
    SHORT = 1,
    INT = 2,
    LONG = 3,
    CHAR = 4,
    DOUBLE = 5,
    FLOAT = 6,
    VOID = 7,
    ENUM = 8,
    UNION = 9,
    STRUCT = 10,
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

typedef enum direct_declarator_enum {
    IDENTIFIER,
    DECLARATOR,
    ARRAY,
    FUNCTION,
    NUM_DIRECT_DECLARATORS

} DIRECT_DECLARATOR_TYPE;

class TypeQualifierList : public NonTerminal{
    vector<int> type_qualifier_list;
    TypeQualifierList();
    //~TypeQualifierList();
};

class Pointer : public NonTerminal{
    public:
        TypeQualifierList* type_qualifier_list;
        Pointer *pointer;
        Pointer();
        Pointer( TypeQualifierList *type_qualifier_list, Pointer *pointer );
};

class DirectDeclarator : public NonTerminal{
    public:
        // DIRECT_DECLARATOR_TYPE direct_declarator_type;
        Identifier* identifier;
        // vector<unsigned int> array_dimensions;
        // ParameterTypeList* parameter_list;
        DirectDeclarator();
};

class Declarator : NonTerminal{
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

class ParameterTypeList : public NonTerminal{

};

class DeclaratorList : public NonTerminal{
    public:
        vector<Declarator*> declarator_list;
        DeclaratorList();
};

class AbstractDeclarator : public NonTerminal{

};

class DirectAbstractDeclarator : public NonTerminal{
    public:
        DirectAbstractDeclarator* base_expression;
        AbstractDeclarator* abstract_declarator;
        bool is_function; // Flag to check if it's a function
        ParameterTypeList* parameters; // Stores function parameters if applicable
        vector<ConditionalExpression* > array_dimensions; // Stores array dimensions if applicable

        DirectAbstractDeclarator();
        
};

DirectAbstractDeclarator* create_direct_abstract_declarator(AbstractDeclarator* x);
DirectAbstractDeclarator* create_direct_abstract_declarator(ConditionalExpression* x);
DirectAbstractDeclarator* create_direct_abstract_declarator(ParameterTypeList* p);
DirectAbstractDeclarator* create_direct_abstract_declarator(DirectAbstractDeclarator* x, ParameterTypeList* p);

class StructDefinition{
    public:
        unordered_map<string,Type> struct_members;
        int is_union;
        int is_struct;
        StructDefinition();
        Type get_member_type();
};

class StructDeclaration : public NonTerminal{
    public:
        SpecifierQualifierList* specifier_qualifier_list;
        DeclaratorList* declarator_list;

        StructDeclaration(SpecifierQualifierList* specifier_qualifier_list, DeclaratorList* declarator_list);
        void add_to_struct_definition(StructDefinition* );
};

class StructDeclarationList : public NonTerminal{
    public:
        vector<StructDeclaration*> struct_declaration_list;
        StructDeclarationList();
};

class Enumerator : public NonTerminal{
    public:
        Identifier* identifier;
        Node* initializer_expression;
        Enumerator(Identifier* identifier, Node* initializer_expression);
};

class EnumeratorList : public NonTerminal{
    public:
        vector<Enumerator*> enumerator_list;
        EnumeratorList();
};

class TypeSpecifier : public Terminal{
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
        SpecifierQualifierList();
        SpecifierQualifierList(vector<TypeSpecifier*> ts);
};

class TypeName : public NonTerminal{
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
