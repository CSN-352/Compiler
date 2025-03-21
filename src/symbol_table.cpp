#include "symbol_table.h"
#include <iostream>
#include <unordered_map>
#include <list>
#include <iomanip>
#include <algorithm>
#include <assert.h>
#include <ast.h>
#include <sstream>
#include <expression.h>
#include <vector>
// #include <y.tab.h>
#include <iostream>
#include <vector>
#include <utility>
#include <iterator>
#include "ast.h"
#include "expression.h"
using namespace std;

extern void yyerror(const char *msg);

// std::vector<Types> defined_types;
extern unsigned int line_no;

// ##############################################################################
// ################################## TYPE ######################################
// ##############################################################################

Type::Type()
{
    typeIndex = -1;
    ptr_level = -1;
    is_const = false;

    is_pointer = false;
    is_array = false;
    array_dim = 0;

    is_function = false;
    num_args = 0;
}

Type::Type(int idx, int p_lvl, bool is_con)
{
    typeIndex = idx;
    is_const = is_con;

    ptr_level = p_lvl;
    is_pointer = ptr_level > 0 ? true : false;

    is_array = false;
    array_dim = 0;

    is_function = false;
    num_args = 0;
    is_defined = false;
}

bool Type::isPrimitive()
{
    if (typeIndex >= 0 && typeIndex <= VOID_T)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// std::string Type::get_name() {
//     std::stringstream ss;
//     ss << defined_types[typeIndex].name;

//     if ( is_array ) {
//         ss << " ";
//         for ( unsigned int i = 0; i < array_dim; i++ ) {
//             if ( array_dims[i] != 0 ) {
//                 ss << "[" << array_dims[i] << "]";
//             } else {
//                 ss << "[]";
//             }
//         }
//     } else if ( is_pointer ) {

//         for ( int i = 0; i < ptr_level; i++ ) {
//             ss << "*";
//         }
//     } else if ( is_function ) {
//         if ( num_args == 0 ) {
//             ss << "( )";
//         } else if ( num_args == 1 ) {
//             ss << "( ";
//             for ( auto it = arg_types.begin(); it != arg_types.end(); it++ ) {
//                 ss << ( *it ).get_name();
//             }
//             ss << " )";
//         } else {
//             ss << "( ";
//             auto it = arg_types.begin();
//             ss << ( *it ).get_name();
//             for ( auto it = arg_types.begin() + 1; it != arg_types.end();
//                   it++ ) {
//                 ss << ", " << ( *it ).get_name();
//             }
//             ss << " )";
//         }
//     }
//     return ss.str();
// }

// size_t Type::get_size() {
//     if ( is_array ) {
//         unsigned int p = 1;
//         for ( unsigned int i = 0; i < array_dim; i++ ) {
//             p *= array_dims[i];
//         }
//         size_t sz;
//         if ( isPrimitive() ) {
//             sz = defined_types[typeIndex].size;
//         } else {
//             if ( defined_types[typeIndex].struct_definition != nullptr ) {
//                 sz = defined_types[typeIndex].struct_definition->get_size();
//             } else {
//                 error_msg( "Size of " + defined_types[typeIndex].name +
//                                " isn't known",
//                            line_no );
//                 exit( 0 );
//             }
//         }
//         return sz * p;

//     } else if ( ptr_level > 0 || is_function ) {
//         return WORD_SIZE;
//     } else if ( !isPrimitive() ) {
//         if ( defined_types[typeIndex].struct_definition != nullptr ) {
//             return defined_types[typeIndex].struct_definition->get_size();
//         } else {
//             error_msg( "Size of " + defined_types[typeIndex].name +
//                            " isn't known",
//                        line_no );
//             exit( 0 );
//         }
//     }

//     else {
//         return defined_types[typeIndex].size;
//     }
// }

bool Type::isInt()
{
    if (typeIndex > U_CHAR_T && typeIndex <= LONG_LONG_T)
    {
        if (ptr_level == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool Type::isChar()
{
    if (typeIndex == U_CHAR_T || typeIndex == CHAR_T)
    {
        if (ptr_level == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool Type::isFloat()
{
    if (typeIndex >= 10 && typeIndex <= 12 && ptr_level == 0)
        return true;
    else
        return false;
}

bool Type::isIntorFloat()
{
    if (typeIndex <= 12 && ptr_level == 0)
        return true;
    else
        return false;
}

bool Type::isUnsigned()
{
    if (typeIndex == 0 || typeIndex == 2 || typeIndex == 4 || typeIndex == 6 || typeIndex == 8)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Type::isPointer()
{
    if (ptr_level)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Type::make_signed()
{
    if (typeIndex == 0 || typeIndex == 2 || typeIndex == 4 || typeIndex == 6 || typeIndex == 8)
    {
        typeIndex += 1;
    }
}

void Type::make_unsigned()
{
    if (typeIndex == 1 || typeIndex == 3 || typeIndex == 5 || typeIndex == 7 || typeIndex == 9)
    {
        typeIndex -= 1;
    }
}

bool Type::isVoid()
{
    if (typeIndex == VOID_T)
    {
        if (ptr_level == 0 || is_array)
        {
            return true;
        }
    }
    return false;
}

bool Type::is_invalid()
{

    if (typeIndex == -1)
    {
        return true;
    }

    return false;
}

bool Type::is_ea()
{

    if (is_array)
    {
        return true;
    }
    else if (!isPrimitive() && ptr_level == 0)
    {
        return true;
    }
    else
    {
        return false;
    }

    return false;
}

bool operator!=(Type &obj1, Type &obj2)
{
    return !(obj1 == obj2);
}

bool operator==(Type &obj1, Type &obj2)
{

    if (obj1.typeIndex != obj2.typeIndex)
    {
        return false;
    }
    else if (obj1.is_array == true && obj2.is_array == true)
    {
        if (obj1.array_dim != obj2.array_dim)
        {
            return false;
        }
        else
        {
            for (unsigned int i = 0; i < obj1.array_dim; i++)
            {
                if (obj1.array_dims[i] == 0 || obj2.array_dims[i] == 0 || obj1.array_dims[i] == obj2.array_dims[i])
                {
                    continue;
                }
                else
                {
                    return false;
                }
            }

            return true;
        }
    }
    else if (obj1.is_array != obj2.is_array)
    {
        if (obj1.ptr_level == 1 && obj2.ptr_level == 1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (obj1.is_pointer == true && obj2.is_pointer == true)
    {
        return obj1.ptr_level == obj2.ptr_level;
    }
    else if (obj1.is_pointer != obj2.is_pointer)
    {
        return false;
    }
    else if (obj1.is_function == true && obj2.is_function == true)
    {
        if (obj1.num_args != obj2.num_args)
        {
            return false;
        }
        else
        {
            for (unsigned int i = 0; i < obj1.num_args; i++)
            {
                if (obj1.arg_types[i] == obj2.arg_types[i])
                {
                    continue;
                }
                else
                {
                    return false;
                }
            }

            return true;
        }
    }
    else if (obj1.is_function != obj2.is_function)
    {
        return false;
    }

    else
    {
        return true;
    }

    return false;
}

// ##############################################################################
// ################################## PARAMETER DECLARATION ######################################
// ##############################################################################
ParameterDeclaration :: ParameterDeclaration(DeclarationSpecifiers* ds) : NonTerminal("PARAMETER DECLARATION"){
    declarations_specifiers = ds;
    declarator = nullptr;
    abstract_declarator = nullptr;
}

// ##############################################################################
// ################################## PARAMETER LIST ######################################
// ##############################################################################
ParameterList :: ParameterList() : NonTerminal("PARAMETER LIST"){}

ParameterList* create_paramater_list(ParameterDeclaration* pd){
    ParameterList* P = new ParameterList();
    P->parameter_declarations.push_back(pd);
    return P;
}

ParameterList* create_parameter_list(ParameterList* p, ParameterDeclaration* pd){
    p->parameter_declarations.push_back(pd);
    return p;
}

// ##############################################################################
// ################################## PARAMETER TYPE LIST ######################################
// ##############################################################################

ParameterTypeList :: ParameterTypeList() : NonTerminal("PARAMETER TYPE LIST"){
    paramater_list = nullptr;
    is_variadic = false;
}

ParameterTypeList* create_parameter_type_list(ParameterList* p, bool var){
    ParameterTypeList* P = new ParameterTypeList();
    P->paramater_list = p;
    P->is_variadic = var;
    return P;
}

// ##############################################################################
// ################################## DIRECT ABSTRACT DECLARATOR ######################################
// ##############################################################################

DirectAbstractDeclarator :: DirectAbstractDeclarator() : NonTerminal("DIRECT ABSTRACT DECLARATOR"){
    abstract_declarator = nullptr;
    is_function = false;
    is_array = false;
}

DirectAbstractDeclarator* create_direct_abstract_declarator(AbstractDeclarator* x){
    DirectAbstractDeclarator* P = new DirectAbstractDeclarator();
    P->abstract_declarator = x;
    return P;
}

DirectAbstractDeclarator* create_direct_abstract_declarator_array(Expression* c){
    DirectAbstractDeclarator* P = new DirectAbstractDeclarator();
    ConditionalExpression* c_cast = dynamic_cast<ConditionalExpression*>(c);
    P->is_array = true;
    if(c==nullptr || c->type.isInt()) P->array_dimensions.push_back(c_cast);
    else{
        string error_msg = "Array size must be an integer at line " + to_string(c->line_no) + ", column " + to_string(c->column_no);
		yyerror(error_msg.c_str());
        symbolTable.set_error();
    }    
    return P;
}

DirectAbstractDeclarator* create_direct_abstract_declarator_function(ParameterTypeList* p){
    DirectAbstractDeclarator* P = new DirectAbstractDeclarator();
    P->is_function = true;
    P->parameters.push_back(p);
    return P;
}

// DirectAbstractDeclarator* create_direct_abstract_declarator_array(DirectAbstractDeclarator* x, Expression* c){
//     ConditionalExpression* c_cast = dynamic_cast<ConditionalExpression*>(c);
//     if(c==nullptr || c->type.isInt()) x->array_dimensions.push_back(c_cast);
//     else{
//         string error_msg = "Array size must be an integer at line " + to_string(c->line_no) + ", column " + to_string(c->column_no);
// 		yyerror(error_msg.c_str());
//         symbolTable.set_error();
//     }   
//     return x;
// }

// DirectAbstractDeclarator* create_direct_abstract_declarator_function(DirectAbstractDeclarator* x, ParameterTypeList* p){
//     x->is_function = true;
//     x->parameters.push_back(p);
//     return x;
// }

// ##############################################################################
// ################################## SPECIFIER QUALIFIER LIST ######################################
// ##############################################################################

SpecifierQualifierList :: SpecifierQualifierList() : NonTerminal("SPECIFIER QUALIFIER LIST") {}

void SpecifierQualifierList :: set_type() {
    is_const = false;
    type_index = -1;

    int isUnsigned = 0;
    int isLong = 0;
    int isShort = 0;
    int isInt = 0;
    int isChar = 0;
    int isDouble = 0;
    int isFloat = 0;
    int isVoid =0;
    int isStruct =0;
    int isEnum =0;
    int isUnion =0;

    for(int i = 0; i < type_qualifiers.size(); i++){
        if(type_qualifiers[i] == TypeQualifiers:: CONST_) is_const = true;
        else if(type_qualifiers[i] == TypeQualifiers :: VOLATILE_) ; //add something later}
    }

    for (int i = 0; i < type_specifiers.size(); i++) {
        if (type_specifiers[i]->type_specifier == Tokens::UNSIGNED_)
        { 
            isUnsigned = 1;
        }
        else if (type_specifiers[i]->type_specifier == Tokens::SHORT_) {
            isShort++;
        }
        else if (type_specifiers[i]->type_specifier == Tokens::INT_)
        {
            isInt++;
        }
        else if (type_specifiers[i]->type_specifier == Tokens::LONG_)
        {
            isLong++;
        }
        else if (type_specifiers[i]->type_specifier == Tokens::CHAR_)
        {
            isChar++;
        }

        else if (type_specifiers[i]->type_specifier == Tokens::DOUBLE_)
        {
            isDouble++;
        }
        else if (type_specifiers[i]->type_specifier == Tokens::FLOAT_)
        {
            isFloat++;
        }

        else if (type_specifiers[i]->type_specifier == Tokens::VOID_)
        {
            isVoid++;
        }
        else if (type_specifiers[i]->type_specifier == Tokens::ENUM_)
        {
            isEnum++;
        }
        else if (type_specifiers[i]->type_specifier == Tokens::UNION_)
        {
            isUnion++;
        }
        else if (type_specifiers[i]->type_specifier == Tokens::STRUCT_)
        {
            isStruct++;
        } else {
            //string error_msg = "Invalid type";
            //yyerror(error_msg.c_str());
            //return;
        }
    }
    if (type_specifiers.size() == 3) {
        if ((isLong == 2) && isUnsigned)
        {
            type_index = PrimitiveTypes::U_LONG_LONG_T;
        }
    } 
    else if (type_specifiers.size() == 2) {
        if ((isLong >= 2) && !isUnsigned)
        {
            type_index = PrimitiveTypes::LONG_LONG_T;
        }
        else if ((isLong == 1) && isUnsigned)
        {
            type_index = PrimitiveTypes::U_LONG_T;
        }
        else if (isInt && isUnsigned)
        {
            type_index = PrimitiveTypes::U_INT_T;
        }
        else if (isShort && isUnsigned)
        {
            type_index = PrimitiveTypes::U_SHORT_T;
        }
        else if (isInt && isUnsigned)
        {
            type_index = PrimitiveTypes::U_INT_T;
        }
        else if (isChar&& isUnsigned)
        {
            type_index = PrimitiveTypes::U_CHAR_T;
        }
        else if (isLong && isDouble)
        {
            type_index = PrimitiveTypes::LONG_DOUBLE_T;
        }
    }
    else if (type_specifiers.size() == 1)
    {
        if ((isLong == 1) && !isUnsigned)
        {
            type_index = PrimitiveTypes::LONG_T;
        }
        else if (isInt && !isUnsigned)
        {
            type_index = PrimitiveTypes::INT_T;
        }
        
        else if (isShort && !isUnsigned)
        {
            type_index = PrimitiveTypes::SHORT_T;
        }
        else if (isInt && !isUnsigned)
        {
            type_index = PrimitiveTypes::INT_T;
        }
        else if (isChar && !isUnsigned)
        {
            type_index = PrimitiveTypes::CHAR_T;
        }
        else if (isFloat && !isUnsigned)
        {
            type_index = PrimitiveTypes::FLOAT_T;
        }
        else if (isDouble && !isUnsigned)
        {
            type_index = PrimitiveTypes::DOUBLE_T;
        }
    } 
    else {
        //string error_msg = "No type passed: ";
        //yyerror(error_msg.c_str());
        //type_index = Tokens::ERROR_;
        //return;
    }

}

SpecifierQualifierList* create_specifier_qualifier_list(TypeSpecifier* t){
    SpecifierQualifierList* P = new SpecifierQualifierList();
    P->type_specifiers.push_back(t);
    P->set_type();
    if(P->type_index == -1){
        string error_msg = "Invalid Type " + t->value + " at line " + to_string(t->line_no) + ", column " + to_string(t->column_no);
		yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return P;
}

SpecifierQualifierList* create_specifier_qualifier_list(SpecifierQualifierList* s, TypeSpecifier* t){
    s->type_specifiers.push_back(t);
    s->set_type();
    if(s->type_index == -1){
        string error_msg = "Invalid Type at line " + to_string(s->type_specifiers[0]->line_no) + ", column " + to_string(s->type_specifiers[0]->column_no);
		yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return s;
}

SpecifierQualifierList* create_specifier_qualifier_list(SpecifierQualifierList* s, string tq){
    if(tq == "CONST")s->type_qualifiers.push_back(0);
    else s->type_qualifiers.push_back(1);
    s->set_type();
    return s;
}

// ##############################################################################
// ################################## TYPE NAME ######################################
// ##############################################################################

TypeName :: TypeName() : NonTerminal("TYPE NAME") {}

TypeName :: TypeName(SpecifierQualifierList* sql, AbstractDeclarator* ad) : NonTerminal("TYPE NAME") {
    this->specifier_qualifier_list = sql;
    this->abstract_declarator = ad;
    if(sql->type_index == -1){
        string error_msg = "Invalid type name"; //add line_no and column no later
		yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    else{
        if(ad == NULL) this->type = Type(sql->type_index, 0, false);
        else{

        }
    }
}

// ##############################################################################
// ################################## IDENTIFIER ######################################
// ##############################################################################

Identifier ::Identifier(string value, unsigned int line_no, unsigned int column_no)
    : Terminal("IDENTIFIER", value, line_no, column_no)
{}

//##############################################################################
//############################ DIRECT DECLARATOR ###############################
//##############################################################################

DirectDeclarator ::DirectDeclarator() 
    : NonTerminal( "direct_declarator" ), identifier( nullptr )
{}

DirectDeclarator *create_dir_declarator_id( //DIRECT_DECLARATOR_TYPE type,
    Identifier *identifier ) {
    //assert( type == ID );
    DirectDeclarator *dd = new DirectDeclarator();
    //dd->type = type;
    assert( identifier != nullptr );
    dd->identifier = identifier;
    dd->add_children( identifier );
    return dd;
}
//##############################################################################
//############################ DECLARATOR ###############################
//##############################################################################

Declarator ::Declarator( DirectDeclarator *dd )
    : NonTerminal( "declarator" ), direct_declarator( dd ),
      identifier( nullptr ) {
    if ( dd == nullptr ) {
        identifier = nullptr;
    } else {
        assert( dd->identifier != nullptr );
        id = dd->id;
    }
};

Declarator *create_declarator( // Pointer *pointer,
    DirectDeclarator *direct_declarator ) {
    if ( direct_declarator == NULL ) {
        return NULL;
    }
        Declarator *d = new Declarator( direct_declarator );
        //d->add_children( direct_declarator );
    return d;
}


// ##############################################################################
// ################################## CONSTANT ######################################
// ##############################################################################

Constant ::Constant(string name, string value, unsigned int line_no, unsigned int column_no)
    : Terminal(name, value, line_no, column_no)
{
    Type *t = new Type(-1, 0, true);
    this->set_constant_type(value);
}

Type Constant ::set_constant_type(string value)
{   
    Type t = this->constant_type;
    int length = value.length();
    if (name == "I_CONSTANT")
    {
        int isLong = 0;
        bool isUnsigned = false;
        for (int i = 0; i < length; i++)
        {
            if (name[i] == 'l' || name[i] == 'L')
            {
                isLong++;
            }
            else if (name[i] == 'u' || name[i] == 'U')
            {
                isUnsigned = true;
            }
        }
        if ((isLong == 2) && isUnsigned)
        {
            t.typeIndex = PrimitiveTypes::U_LONG_LONG_T;
        }
        else if ((isLong == 2) && !isUnsigned)
        {
            t.typeIndex = PrimitiveTypes::LONG_LONG_T;
        }
        else if ((isLong == 1) && isUnsigned)
        {
            t.typeIndex = PrimitiveTypes::U_LONG_T;
        }
        else if ((isLong == 1) && !isUnsigned)
        {
            t.typeIndex = PrimitiveTypes::LONG_T;
        }
        else if (!isLong && isUnsigned)
        {
            t.typeIndex = PrimitiveTypes::U_SHORT_T;
        }
        else if (!isLong && !isUnsigned)
        {
            t.typeIndex = PrimitiveTypes::SHORT_T;
        }
    }
    else if (name == "CHAR_CONSTANT")
    {
        t.typeIndex = PrimitiveTypes::CHAR_T;
    }
    else if (name == "F_CONSTANT")
    {
        int isFloat = 0;
        int isDouble = 0;
        for (int i = 0; i < length; i++)
        {
            if (name[i] == 'l' || name[i] == 'L')
            {
                isDouble++;
            }
            else if (name[i] == 'f' || name[i] == 'F')
            {
                isFloat++;
            }
        }
        if (isFloat) {
            t.typeIndex = PrimitiveTypes::FLOAT_T;
        } else if (isDouble == 1) {
            t.typeIndex = PrimitiveTypes::LONG_DOUBLE_T;
        } else {
            t.typeIndex = PrimitiveTypes::DOUBLE_T;
        }
    }
    else
    {
        string error_msg = "Invalid type: " + name;
        yyerror(error_msg.c_str());
    }
    this->constant_type = t;
    return t;
}

Type Constant::get_constant_type() { return this->constant_type; }

// convert hex and octal to decimal
string Constant ::convert_to_decimal()
{
    string value = this->value;
    if (value[0] == '0')
    {
        if (value[1] == 'x' || value[1] == 'X')
        {
            return to_string(stoi(value, nullptr, 16));
        }
        else
        {
            return to_string(stoi(value, nullptr, 8));
        }
    }
    else
    {
        return value;
    }
}

// ##############################################################################
// ################################## STRINGLITERAL ######################################
// ##############################################################################

StringLiteral ::StringLiteral(string value, unsigned int line_no, unsigned int column_no)
    : Terminal("CONSTANT", value, line_no, column_no)
{
}

// ##############################################################################
// ################################## SYMBOLTABLE ######################################
// ##############################################################################

SymbolTable::SymbolTable() { currentScope = 0; }

void SymbolTable::enterScope()
{
    currentScope++;
}

void SymbolTable::set_error() { error = true; }

bool SymbolTable::has_error() { return error; }

void SymbolTable::exitScope()
{
    if (currentScope == 0)
        return;

    for (auto it = table.begin(); it != table.end();)
    {
        for (auto symIt = it->second.begin(); symIt != it->second.end();)
        {
            if ((*symIt)->scope == currentScope)
            {
                delete *symIt;
                symIt = it->second.erase(symIt);
            }
            else
            {
                ++symIt;
            }
        }
        if (it->second.empty())
            it = table.erase(it);
        else
            ++it;
    }
    if (currentScope > 0)
        currentScope--;
}

void SymbolTable::insert(string name, Type type, int memoryAddr)
{
    for (const Symbol *sym : table[name])
    {
        if (sym->scope == currentScope)
        {
            string error_msg = "Symbol '" + name + "' already declared in this scope.";
            yyerror(error_msg.c_str());
            set_error();
            return;
        }
    }

    Symbol *sym = new Symbol(name, type, currentScope, memoryAddr);
    table[name].push_front(sym);
}

bool SymbolTable::lookup(string name)
{
    auto it = table.find(name);
    if (it == table.end())
        return false;

    for (const Symbol *sym : it->second)
    {
        if (sym->scope <= currentScope)
            return true;
    }
    return false;
}

Symbol *SymbolTable::getSymbol(string name)
{
    auto it = table.find(name);
    if (it == table.end() || it->second.empty())
        return nullptr;

    Symbol *sym = nullptr;

    for (Symbol *_sym : it->second)
    {
        if (_sym->scope <= currentScope)
        {
            if (sym == nullptr || _sym->scope > sym->scope)
            {
                sym = _sym;
            }
        }
    }

    return sym;
}

void SymbolTable::update(string name, Type newType)
{
    Symbol *sym = getSymbol(name);
    if (sym)
    {
        sym->type = newType;
    }
    else
    {
        string error_msg = "Symbol '" + name + "' not found.";
        yyerror(error_msg.c_str());
        set_error();
    }
}

void SymbolTable::remove(string name)
{
    if (lookup(name))
    {
        table.erase(name);
    }
    else
    {
        string error_msg = "Symbol '" + name + "' not found.";
        yyerror(error_msg.c_str());
        set_error();
    }
}

void SymbolTable::print()
{
    cout << "\nSYMBOL TABLE:\n";
    cout << "----------------------------------------------------------------------------\n";
    cout << "| " << setw(20) << left << "Name"
         << "| " << setw(26) << left << "Type"
         << "| " << setw(8) << left << "Scope"
         << "| " << setw(12) << left << "Memory Addr" << " |\n";
    cout << "----------------------------------------------------------------------------\n";

    for (const auto &entry : table)
    {
        for (const auto symbol : entry.second)
        {
            cout << "| " << setw(20) << left << symbol->name
                 << "| " << setw(26) << left << symbol->type.typeIndex
                 // Aren ~ maine add kiya hai .typeIndex (isko change krna hai according to Type class)
                 << "| " << setw(8) << left << symbol->scope
                 << "| " << setw(12) << left << symbol->memoryAddr << " |\n";
        }
    }

    cout << "----------------------------------------------------------------------------\n";
}


SymbolTable symbolTable;
