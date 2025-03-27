#include <iostream>
#include <unordered_map>
#include <list>
#include <iomanip>
#include <algorithm>
#include <assert.h>
#include <ast.h>
#include <sstream>
#include <vector>
#include <iostream>
#include <vector>
#include <utility>
#include <iterator>

#include "parser.tab.h"
#include "symbol_table.h"
#include "expression.h"
#include "ast.h"

using namespace std;

#define WORD_SIZE 4

extern void yyerror(const char *msg);

static unsigned int currAddress = 0;
SymbolTable symbolTable;

// ##############################################################################
// ################################## TYPE ######################################
// ##############################################################################

Type::Type()
{
    typeIndex = -1;
    ptr_level = -1;
    is_const_variable = false;
    is_const_literal = false;

    is_pointer = false;
    is_array = false;
    array_dim = 0;

    is_function = false;
    num_args = 0;
}

Type::Type(int idx, int p_lvl, bool is_con)
{
    typeIndex = idx;
    is_const_variable = is_con;

    ptr_level = p_lvl;
    is_pointer = ptr_level > 0 ? true : false;

    is_array = false;
    array_dim = 0;

    is_function = false;
    num_args = 0;
    is_defined_type = false;
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

bool Type::is_error()
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

int Type::get_size()
{
    if (is_array)
    {
        unsigned int p = 1;
        for (int i = 0; i < array_dim; i++)
        {
            p *= array_dims[i];
        }
        int size = 0;
        if (isPrimitive())
            size = primitive_type_size[typeIndex];
        else
        {
            if (is_defined_type && symbolTable.get_defined_type(defined_type_name).type_definition != nullptr)
                size = symbolTable.get_defined_type(defined_type_name).type_definition->get_size();
            else
            {
                string error_msg = "Type " + defined_type_name + " declared but not defined at ";
                yyerror(error_msg.c_str());
                symbolTable.set_error();
            }
        }
        return size * p;
    }
    else if (ptr_level > 0 || is_function) return WORD_SIZE;
    else if (!isPrimitive())
    {
        if (is_defined_type && symbolTable.get_defined_type(defined_type_name).type_definition != nullptr)
            return symbolTable.get_defined_type(defined_type_name).type_definition->get_size();
        else
        {
            string error_msg = "Type " + defined_type_name + " declared but not defined at ";
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return 0;
        }
    }
    else
        return primitive_type_size[typeIndex];
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

bool operator!=(Type &obj1, Type &obj2)
{
    return !(obj1 == obj2);
}

// ##############################################################################
// ################################## TYPE DEFINITION ###########################
// ##############################################################################

// Not fully implemented

bool TypeDefinition::get_member(Identifier* id)
{
    if (find(members.begin(),members.end(),id->name) != members.end()) return true;
    return false;
}

int TypeDefinition::get_size()
{
    int size = 0;
    for (string member : members){
        Symbol* sym = symbolTable.getSymbol(member);
        size += sym->type.get_size();
    }
    return size;
}

TypeDefinition* create_type_definition(TypeCategory type_category, StructDeclarationList* sdl)
{
    TypeDefinition* P = new TypeDefinition();
    P->type_category = type_category;
    if(sdl != nullptr){
        for(StructDeclaration* sd : sdl->struct_declaration_list){
            for(int i = 0; i < sd->struct_declarator_list->struct_declarator_list.size(); i++){
                StructDeclarator* d = sd->struct_declarator_list->struct_declarator_list[i];
                if(d != nullptr){
                    Identifier* id = d->declarator->direct_declarator->identifier;
                    int pointer_level = 0;
                    if(d->declarator->pointer != nullptr)pointer_level = d->declarator->pointer->pointer_level;
                    Type t = Type(sd->specifier_qualifier_list->type_index, pointer_level, sd->specifier_qualifier_list->is_const_variable);
                    if (d->declarator->direct_declarator->is_array){
                        t.is_array = true;
                        t.is_pointer = true;
                        t.array_dim = d->declarator->direct_declarator->array_dimensions.size();
                        t.array_dims = d->declarator->direct_declarator->array_dimensions;
                    }
                    else if (d->declarator->direct_declarator->is_function){
                        string error_msg = "Function cannot be part of Struct/Union at line " + to_string(sd->specifier_qualifier_list->type_specifiers[0]->line_no) + ", column " + to_string(sd->specifier_qualifier_list->type_specifiers[0]->column_no);
                        yyerror(error_msg.c_str());
                        symbolTable.set_error();
                        
                    }
                    if(d->bit_field_width == -1)symbolTable.insert(id->value, t, t.get_size());
                    else symbolTable.insert(id->value, t, d->bit_field_width);
                    P->members.push_back(id->value);
                }
                else currAddress += d->bit_field_width;
            }
        }
    }
    return P;
}

// ##############################################################################
// ################################## DEFINED TYPES ######################################
// ##############################################################################
int DefinedTypes::t_index_count = PrimitiveTypes::N_PRIMITIVE_TYPES;

DefinedTypes :: DefinedTypes(TypeCategory tc, TypeDefinition* td) : Type(t_index_count++, 0, false){
    is_defined_type = true;
    type_category = tc;
    type_definition = td;
}

// ##############################################################################
// ################################## TYPE QUALIFIER LIST ######################################
// ##############################################################################

TypeQualifierList ::TypeQualifierList() : NonTerminal("TYPE QUALIFIER LIST") {}

TypeQualifierList *create_type_qualifier_list(int typequalifier)
{
    TypeQualifierList *P = new TypeQualifierList();
    P->type_qualifier_list.push_back(typequalifier);
    return P;
}

TypeQualifierList *create_type_qualifier_list(TypeQualifierList *typequalifierlist, int typequalifier)
{
    typequalifierlist->type_qualifier_list.push_back(typequalifier);
    return typequalifierlist;
}

// ##############################################################################
// ############################# DECLARATION ####################################
// ##############################################################################

Declaration ::Declaration()
    : NonTerminal("declaration")
{
    declaration_specifiers = nullptr;
    init_declarator_list = nullptr;
}

Declaration *create_declaration(DeclarationSpecifiers *declaration_specifiers,
                             DeclaratorList *init_declarator_list)
{
    Declaration *P = new Declaration();

    P->declaration_specifiers = declaration_specifiers;
    P->init_declarator_list = init_declarator_list;

    if(init_declarator_list != nullptr){
        for (int index = 0; index < init_declarator_list->declarator_list.size(); index++)
        {
            Declarator *variable = init_declarator_list->declarator_list[index];
            int ptr_level = 0;
            if (variable->pointer != nullptr) ptr_level = variable->pointer->pointer_level;
            Type t = Type(P->declaration_specifiers->type_index, ptr_level, P->declaration_specifiers->is_const_variable);
            if (variable->direct_declarator->is_array){
                t.is_array = true;
                t.is_pointer = true;
                t.array_dim = variable->direct_declarator->array_dimensions.size();
                t.array_dims = variable->direct_declarator->array_dimensions;
            }
            else if (variable->direct_declarator->is_function){
                t.is_function = true;
                t.num_args = variable->direct_declarator->parameters->paramater_list->parameter_declarations.size();
                for (int i = 0; i < t.num_args; i++) t.arg_types.push_back(variable->direct_declarator->parameters->paramater_list->parameter_declarations[i]->type);
                
            }
            symbolTable.insert(variable->direct_declarator->identifier->value, t, t.get_size());
        }
        return P;
    }
}

// ##############################################################################
// ############################# DECLARATION LIST ####################################
// ##############################################################################

// ##############################################################################
// ############################# INITIALIZER LIST ####################################
// ##############################################################################

// ##############################################################################
// ################################## INITIALIZER ######################################
// ##############################################################################

// ##############################################################################
// ############################# IDENTIFIER LIST ####################################
// ##############################################################################

IdentifierList ::IdentifierList() : NonTerminal("IDENTIFIER LIST") {}

IdentifierList *create_identifier_list(Identifier *i)
{
    IdentifierList *P = new IdentifierList();
    P->identifiers.push_back(i);
    return P;
}

IdentifierList *create_identifier_list(IdentifierList *il, Identifier *i)
{
    il->identifiers.push_back(i);
    return il;
}

// ##############################################################################
// ################################## DECLARATION SPECIFIERS ######################################
// ##############################################################################

DeclarationSpecifiers ::DeclarationSpecifiers() : NonTerminal("DECLARATION SPECIFIERS") {}

void DeclarationSpecifiers ::set_type()
{
    is_const_variable = false;
    type_index = -1;

    int isUnsigned = 0;
    int isLong = 0;
    int isShort = 0;
    int isInt = 0;
    int isChar = 0;
    int isDouble = 0;
    int isFloat = 0;
    int isVoid = 0;
    int isStruct = 0;
    int isEnum = 0;
    int isUnion = 0;

    for (int i = 0; i < type_qualifiers.size(); i++)
    {
        if (type_qualifiers[i] == TypeQualifiers::TYPE_QUALIFIERS_CONST)
            is_const_variable = true;
        else if (type_qualifiers[i] == TypeQualifiers ::TYPE_QUALIFIERS_VOLATILE)
            ; // add something later}
    }

    for (int i = 0; i < type_specifiers.size(); i++)
    {
        if (type_specifiers[i]->primitive_type_specifier->name == "UNSIGNED")
        {
            isUnsigned = 1;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "SHORT")
        {
            isShort++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "INT")
        {
            isInt++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "LONG")
        {
            isLong++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "CHAR")
        {
            isChar++;
        }

        else if (type_specifiers[i]->primitive_type_specifier->name == "DOUBLE")
        {
            isDouble++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "FLOAT")
        {
            isFloat++;
        }

        else if (type_specifiers[i]->primitive_type_specifier->name == "VOID")
        {
            isVoid++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "ENUM")
        {
            isEnum++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "UNION")
        {
            isUnion++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "STRUCT")
        {
            isStruct++;
        }
    }
    if (type_specifiers.size() == 3)
    {
        if ((isLong == 2) && isUnsigned)
        {
            type_index = PrimitiveTypes::U_LONG_LONG_T;
        }
    }
    else if (type_specifiers.size() == 2)
    {
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
        else if (isChar && isUnsigned)
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
}

DeclarationSpecifiers *create_declaration_specifiers(SpecifierQualifierList *sql)
{
    DeclarationSpecifiers *P = new DeclarationSpecifiers();
    P->type_specifiers.insert(P->type_specifiers.end(), sql->type_specifiers.begin(), sql->type_specifiers.end());
    P->set_type();
    if (P->type_index == -1)
    {
        string error_msg = "Invalid Type at line " + to_string(P->type_specifiers[0]->line_no) + ", column " + to_string(P->type_specifiers[0]->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return P;
}

DeclarationSpecifiers *create_declaration_specifiers(DeclarationSpecifiers *ds, int storage_class)
{
    ds->storage_class_specifiers.push_back(storage_class);
    return ds;
}

DeclarationSpecifiers *create_declaration_specifiers(DeclarationSpecifiers *ds, SpecifierQualifierList *sql)
{
    ds->type_specifiers.insert(ds->type_specifiers.end(), sql->type_specifiers.begin(), sql->type_specifiers.end());
    ds->set_type();
    if (ds->type_index == -1)
    {
        string error_msg = "Invalid Type at line " + to_string(ds->type_specifiers[0]->line_no) + ", column " + to_string(ds->type_specifiers[0]->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return ds;
}

// ##############################################################################
// ################################## POINTER ######################################
// ##############################################################################
Pointer ::Pointer() : NonTerminal("POINTER")
{
    type_qualifier_list = nullptr;
    pointer_level = 0;
}

Pointer *create_pointer(TypeQualifierList *tql)
{
    Pointer *P = new Pointer();
    P->pointer_level++;
    P->type_qualifier_list = tql;
    return P;
}

Pointer *create_pointer(Pointer *p, TypeQualifierList *tql)
{
    p->pointer_level++;
    p->type_qualifier_list = tql;
    return p;
}

// ##############################################################################
// ############################ DIRECT DECLARATOR ###############################
// ##############################################################################

DirectDeclarator ::DirectDeclarator()
    : NonTerminal("direct_declarator"){
        declarator = nullptr;
        identifier = nullptr;
        parameters = nullptr;
        is_function = false;
        is_array = false;
    }

DirectDeclarator *create_dir_declarator_id(Identifier *i)
{
    // assert( type == ID );
    DirectDeclarator *dd = new DirectDeclarator();
    // dd->type = type;
    assert(i != nullptr);
    dd->identifier = i;
    dd->add_children(i);
    return dd;
}

DirectDeclarator *create_direct_declarator(Declarator *d)
{
    DirectDeclarator *P = new DirectDeclarator();
    P->declarator = d;
    return P;
}

DirectDeclarator *create_direct_declarator_array(DirectDeclarator *dd, Expression *e)
{
    if (dd->is_function)
    {
        string error_msg = "Type name cannot be an array of functions " + to_string(e->line_no) + ", column " + to_string(e->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return dd;
    }
    if (e == nullptr || (e->type.isInt() && e->type.is_const_literal))
    {
        PrimaryExpression *c_cast = dynamic_cast<PrimaryExpression *>(e);
        dd->array_dimensions.push_back(stoi(c_cast->constant->value));
    }
    else
    {
        string error_msg = "Array size must be a constant integer at line " + to_string(e->line_no) + ", column " + to_string(e->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return dd;
}

DirectDeclarator *create_direct_declarator_function(DirectDeclarator *dd, ParameterTypeList *ptl)
{
    if (dd->is_function)
    {
        string error_msg = "Type name cannot be a function returning a function " + to_string(ptl->line_no) + ", column " + to_string(ptl->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return dd;
    }
    if (dd->is_array)
    {
        string error_msg = "Type name cannot be a function returning an array " + to_string(ptl->line_no) + ", column " + to_string(ptl->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return dd;
    }
    dd->is_function = true;
    dd->parameters = ptl;
    return dd;
}

// ##############################################################################
// ############################ DECLARATOR ###############################
// ##############################################################################

Declarator ::Declarator()
    : NonTerminal("declarator"){
    pointer = nullptr;
    direct_declarator = nullptr;
};

Declarator *create_declarator(Pointer *p, DirectDeclarator *dd)
{
    if (dd == NULL)
    {
        return NULL;
    }
    Declarator *d = new Declarator();
    d->pointer = p;
    d->direct_declarator = dd;
    // d->add_children( direct_declarator );
    return d;
}

// ##############################################################################
// ################################## PARAMETER DECLARATION ######################################
// ##############################################################################

ParameterDeclaration ::ParameterDeclaration() : NonTerminal("PARAMETER DECLARATION")
{
    declarations_specifiers = nullptr;
    abstract_declarator = nullptr;
    declarator = nullptr;
}

ParameterDeclaration *create_parameter_declaration(DeclarationSpecifiers *ds, AbstractDeclarator *ad){
    ParameterDeclaration* P = new ParameterDeclaration();
    P->declarations_specifiers = ds;
    P->abstract_declarator = ad;
    int pointer_level = 0;
    if(ad->pointer != nullptr)pointer_level = ad->pointer->pointer_level;
    P->type = Type(ds->type_index,pointer_level ,ds->is_const_variable);
    return P;
}

ParameterDeclaration *create_parameter_declaration(DeclarationSpecifiers *ds, Declarator *d){
    ParameterDeclaration* P = new ParameterDeclaration();
    P->declarations_specifiers = ds;
    P->declarator = d;
    int pointer_level = 0;
    if(d->pointer != nullptr)pointer_level = d->pointer->pointer_level;
    P->type = Type(ds->type_index,pointer_level ,ds->is_const_variable);
    return P;
}

ParameterDeclaration* create_parameter_declaration(DeclarationSpecifiers *ds){
    ParameterDeclaration* P = new ParameterDeclaration();
    P->declarations_specifiers = ds;
    P->type = Type(ds->type_index,0,ds->is_const_variable);
    return P;
}

// ##############################################################################
// ################################## PARAMETER LIST ######################################
// ##############################################################################

ParameterList ::ParameterList() : NonTerminal("PARAMETER LIST") {}

ParameterList *create_parameter_list(ParameterDeclaration *pd)
{
    ParameterList *pl = new ParameterList();
    pl->parameter_declarations.push_back(pd);
    return pl;
}

ParameterList *create_parameter_list(ParameterList *pl, ParameterDeclaration *pd)
{
    pl->parameter_declarations.push_back(pd);
    return pl;
}

// ##############################################################################
// ################################## PARAMETER TYPE LIST ######################################
// ##############################################################################

ParameterTypeList ::ParameterTypeList() : NonTerminal("PARAMETER TYPE LIST")
{
    paramater_list = nullptr;
    is_variadic = false;
}

ParameterTypeList *create_parameter_type_list(ParameterList *pl, bool var)
{
    ParameterTypeList *P = new ParameterTypeList();
    P->paramater_list = pl;
    P->is_variadic = var;
    return P;
}

// ##############################################################################
// ############################# DECLARATOR LIST ################################
// ##############################################################################

DeclaratorList ::DeclaratorList() : NonTerminal("init_declarator_list") {};

DeclaratorList *create_init_declarator_list(Declarator *d)
{

    if (d == nullptr)
    {
        return nullptr;
    }
    DeclaratorList *dl = new DeclaratorList();
    dl->declarator_list.push_back(d);
    // dl->add_children( d );
    return dl;
}

DeclaratorList* add_to_init_declarator_list(DeclaratorList *init_declarator_list, Declarator *init_declarator)
{
    if (init_declarator == nullptr)
    {
        return init_declarator_list;
    }
    init_declarator_list->declarator_list.push_back(init_declarator);
    return init_declarator_list;
}

// ##############################################################################
// ################################## ABSTRACT DECLARATOR ######################################
// ##############################################################################

AbstractDeclarator ::AbstractDeclarator() : NonTerminal("ABSTRACT DECLARATOR")
{
    pointer = nullptr;
    direct_abstract_declarator = nullptr;
}

AbstractDeclarator *create_abstract_declarator(Pointer *p, DirectAbstractDeclarator *dad)
{
    AbstractDeclarator *P = new AbstractDeclarator();
    P->pointer = p;
    P->direct_abstract_declarator = dad;
    return P;
}

// ##############################################################################
// ################################## DIRECT ABSTRACT DECLARATOR ######################################
// ##############################################################################

DirectAbstractDeclarator ::DirectAbstractDeclarator() : NonTerminal("DIRECT ABSTRACT DECLARATOR")
{
    abstract_declarator = nullptr;
    is_function = false;
    parameters = nullptr;
    is_array = false;
}

DirectAbstractDeclarator *create_direct_abstract_declarator(AbstractDeclarator *x)
{
    DirectAbstractDeclarator *P = new DirectAbstractDeclarator();
    P->abstract_declarator = x;
    return P;
}

DirectAbstractDeclarator *create_direct_abstract_declarator_array(Expression *c)
{
    DirectAbstractDeclarator *P = new DirectAbstractDeclarator();
    P->is_array = true;
    if (c == nullptr || (c->type.isInt() && c->type.is_const_literal))
    {
        PrimaryExpression *c_cast = dynamic_cast<PrimaryExpression *>(c);
        P->array_dimensions.push_back(stoi(c_cast->constant->value));
    }
    else
    {
        string error_msg = "Array size must be a constant integer at line " + to_string(c->line_no) + ", column " + to_string(c->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return P;
}

DirectAbstractDeclarator *create_direct_abstract_declarator_function(ParameterTypeList *p)
{
    DirectAbstractDeclarator *P = new DirectAbstractDeclarator();
    P->is_function = true;
    P->parameters = p;
    return P;
}

DirectAbstractDeclarator *create_direct_abstract_declarator_array(DirectAbstractDeclarator *x, Expression *c)
{
    if (x->is_function || (x->abstract_declarator && x->abstract_declarator->direct_abstract_declarator && x->abstract_declarator->direct_abstract_declarator->is_function))
    {
        string error_msg = "Type name cannot be an array of functions " + to_string(c->line_no) + ", column " + to_string(c->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return x;
    }
    if (c == nullptr || (c->type.isInt() && c->type.is_const_literal))
    {
        PrimaryExpression *c_cast = dynamic_cast<PrimaryExpression *>(c);
        x->array_dimensions.push_back(stoi(c_cast->constant->value));
    }
    else
    {
        string error_msg = "Array size must be a constant integer at line " + to_string(c->line_no) + ", column " + to_string(c->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return x;
}

DirectAbstractDeclarator *create_direct_abstract_declarator_function(DirectAbstractDeclarator *x, ParameterTypeList *p)
{
    if (x->is_function || (x->abstract_declarator && x->abstract_declarator->direct_abstract_declarator && x->abstract_declarator->direct_abstract_declarator->is_function))
    {
        string error_msg = "Type name cannot be a function returning a function " + to_string(p->line_no) + ", column " + to_string(p->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return x;
    }
    if (x->is_array || (x->abstract_declarator && x->abstract_declarator->direct_abstract_declarator && x->abstract_declarator->direct_abstract_declarator->is_array))
    {
        string error_msg = "Type name cannot be a function returning an array " + to_string(p->line_no) + ", column " + to_string(p->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return x;
    }
    x->is_function = true;
    x->parameters = p;
    return x;
}

// ##############################################################################
// ############################ STRUCT UNION SPECIFIER #########################
// ##############################################################################
StructUnionSpecifier :: StructUnionSpecifier() : NonTerminal("STRUCT UNION SPECIFIER")
{
    identifier = nullptr;
    struct_declaration_list = nullptr;
    type_category = TYPE_CATEGORY_ERROR;
}

StructUnionSpecifier *create_struct_union_specifier(string struct_or_union, Identifier *id, StructDeclarationList *sdl)
{
    StructUnionSpecifier *P = new StructUnionSpecifier();
    if(struct_or_union == "STRUCT") P->type_category = TYPE_CATEGORY_STRUCT;
    else P->type_category = TYPE_CATEGORY_UNION;
    P->identifier = id;
    P->struct_declaration_list = sdl;
    TypeDefinition *td;
    if (sdl != nullptr)td = create_type_definition(P->type_category, sdl);
    else td = nullptr;
    if (id != nullptr){
        DefinedTypes dt = DefinedTypes(P->type_category, td);
        dt.defined_type_name = id->value;
        symbolTable.insert_defined_type(id->value, dt);
    }
    return P;
}

// ##############################################################################
// ############################ STRUCT DECLARATION LIST #########################
// ##############################################################################

StructDeclarationList::StructDeclarationList() : NonTerminal("STRUCT DECLARATION LIST") {}

StructDeclarationList *create_struct_declaration_list(StructDeclaration *sd)
{
    StructDeclarationList *P = new StructDeclarationList();
    P->struct_declaration_list.push_back(sd);
    return P;
}

StructDeclarationList *create_struct_declaration_list(StructDeclarationList *sdl, StructDeclaration *sd)
{
    sdl->struct_declaration_list.push_back(sd);
    return sdl;
}

// ##############################################################################
// ############################ STRUCT DECLARATION ###############################
// ##############################################################################

StructDeclaration::StructDeclaration(): NonTerminal("STRUCT DECLARATION")
{
    specifier_qualifier_list = nullptr;
    struct_declarator_list = nullptr;
}

StructDeclaration* create_struct_declaration(SpecifierQualifierList *sql, StructDeclaratorList *sdl)   {
    StructDeclaration* P = new StructDeclaration();
    P->specifier_qualifier_list = sql;
    P->struct_declarator_list = sdl;
    return P;
}

// ##############################################################################
// ############################ STRUCT DECLARATOR LIST ###############################
// ##############################################################################
StructDeclaratorList ::StructDeclaratorList() : NonTerminal("STRUCT DECLARATOR LIST") {}

StructDeclaratorList *create_struct_declarator_list(StructDeclarator *sd)
{
    StructDeclaratorList *P = new StructDeclaratorList();
    P->struct_declarator_list.push_back(sd);
    return P;
}

StructDeclaratorList *create_struct_declarator_list(StructDeclaratorList *sdl, StructDeclarator *sd)
{
    sdl->struct_declarator_list.push_back(sd);
    return sdl;
}

// ##############################################################################
// ############################ STRUCT DECLARATOR ###############################
// ##############################################################################

StructDeclarator ::StructDeclarator() : NonTerminal("STRUCT DECLARATOR"){
    declarator = nullptr;
    bit_field_width = -1;
}

StructDeclarator* create_struct_declarator(Declarator* d, Expression* e){
    StructDeclarator* P = new StructDeclarator();
    P->declarator = d;
    if(e != nullptr){
        if (e->type.isInt() && e->type.is_const_literal){
            PrimaryExpression *e_cast = dynamic_cast<PrimaryExpression *>(e);
            P->bit_field_width = stoi(e_cast->constant->value);
        }
        else{
            string error_msg = "Bit-field width must be a constant integer at line " + to_string(e->line_no) + ", column " + to_string(e->column_no);
            yyerror(error_msg.c_str());
            symbolTable.set_error();
        }
    }
    return P;
}



// ##############################################################################
// ############################ ENUMERATOR ###############################
// ##############################################################################
Enumerator::Enumerator() : NonTerminal("ENUMERATOR")
{
    identifier = nullptr;
    initializer_expression = nullptr;
}

Enumerator *create_enumerator(Identifier *id, Expression *e)
{
    Enumerator *P = new Enumerator();
    P->identifier = id;
    ConditionalExpression *e_cast = dynamic_cast<ConditionalExpression *>(e);
    if (e == nullptr || (e->type.isInt() && e->type.is_const_literal))
        P->initializer_expression = e_cast;
    else
    {
        string error_msg = "Enumerator value must be a constant integer at line  " + to_string(e->line_no) + ", column " + to_string(e->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return P;
}

// ##############################################################################
// ############################ ENUMERATOR LIST###############################
// ##############################################################################

EnumeratorList::EnumeratorList() : NonTerminal("ENUMERATOR LIST") {}

EnumeratorList *create_enumerator_list(Enumerator *e)
{
    EnumeratorList *P = new EnumeratorList();
    P->enumerator_list.push_back(e);
    return P;
}

EnumeratorList *create_enumerator_list(EnumeratorList *el, Enumerator *e)
{
    el->enumerator_list.push_back(e);
    return el;
}

// ##############################################################################
// ############################ ENUM SPECIFIER ###############################
// ##############################################################################

EnumSpecifier::EnumSpecifier() : NonTerminal("ENUM SPECIFIER")
{
    identifier = nullptr;
    enumerators = nullptr;
}

EnumSpecifier *create_enumerator_specifier(Identifier *id, EnumeratorList *el)
{
    EnumSpecifier *P = new EnumSpecifier();
    P->identifier = id;
    P->enumerators = el;
    if (el != nullptr)
    {
        Type type(PrimitiveTypes::INT_T, 0, true);
        for (Enumerator *e : el->enumerator_list)
            symbolTable.insert(e->identifier->value, type, 0);
    }
    return P;
}

EnumSpecifier *create_enumerator_specifier(EnumeratorList *el)
{
    EnumSpecifier *P = new EnumSpecifier();
    P->enumerators = el;
    if (el != nullptr)
    {
        Type type(PrimitiveTypes::INT_T, 0, true);
        for (Enumerator *e : el->enumerator_list)
            symbolTable.insert(e->identifier->value, type, 0);
    }
    return P;
}


// ##############################################################################
// ################################## TYPE SPECIFIER ############################
// ##############################################################################

TypeSpecifier ::TypeSpecifier() : NonTerminal("TYPE SPECIFIER"){
    primitive_type_specifier = nullptr;
    enum_specifier = nullptr;
    struct_union_specifier = nullptr;
    // class_specifier = nullptr;
}

TypeSpecifier *create_type_specifier(Terminal* t)
{
    TypeSpecifier* P = new TypeSpecifier();
    P->primitive_type_specifier = t;
    P->name += ": " + t->name; // for debugging purposes
    return P;
}

TypeSpecifier *create_type_specifier(EnumSpecifier* es)
{
    TypeSpecifier* P = new TypeSpecifier();
    P->enum_specifier = es;
    P->name += ": ENUM";
    return P;
}

TypeSpecifier *create_type_specifier(StructUnionSpecifier *sus)
{
    TypeSpecifier* P = new TypeSpecifier();
    P->struct_union_specifier = sus;
    P->name += ": STRUCT/UNION";
    // if(sus.is_struct)
    //     type_specifier_obj->name += ": Struct";
    // else
    // type_specifier_obj->name += ": Union";
    return P;
}

// ##############################################################################
// ################################## SPECIFIER QUALIFIER LIST ######################################
// ##############################################################################

SpecifierQualifierList ::SpecifierQualifierList() : NonTerminal("SPECIFIER QUALIFIER LIST")
{
}

void SpecifierQualifierList ::set_type()
{
    is_const_variable = false;
    type_index = -1;

    int isUnsigned = 0;
    int isLong = 0;
    int isShort = 0;
    int isInt = 0;
    int isChar = 0;
    int isDouble = 0;
    int isFloat = 0;
    int isVoid = 0;
    int isStruct = 0;
    int isEnum = 0;
    int isUnion = 0;

    for (int i = 0; i < type_qualifiers.size(); i++)
    {
        if (type_qualifiers[i] == TypeQualifiers::TYPE_QUALIFIERS_CONST)
            is_const_variable = true;
        else if (type_qualifiers[i] == TypeQualifiers ::TYPE_QUALIFIERS_VOLATILE)
            ; // add something later}
    }

    for (int i = 0; i < type_specifiers.size(); i++)
    {
        if (type_specifiers[i]->primitive_type_specifier->name == "UNSIGNED")
        {
            isUnsigned = 1;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "SHORT")
        {
            isShort++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "INT")
        {
            isInt++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "LONG")
        {
            isLong++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "CHAR")
        {
            isChar++;
        }

        else if (type_specifiers[i]->primitive_type_specifier->name == "DOUBLE")
        {
            isDouble++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "FLOAT")
        {
            isFloat++;
        }

        else if (type_specifiers[i]->primitive_type_specifier->name == "VOID")
        {
            isVoid++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "ENUM")
        {
            isEnum++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "UNION")
        {
            isUnion++;
        }
        else if (type_specifiers[i]->primitive_type_specifier->name == "STRUCT")
        {
            isStruct++;
        }
    }
    if (type_specifiers.size() == 3)
    {
        if ((isLong == 2) && isUnsigned)
        {
            type_index = PrimitiveTypes::U_LONG_LONG_T;
        }
    }
    else if (type_specifiers.size() == 2)
    {
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
        else if (isChar && isUnsigned)
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
}

SpecifierQualifierList *create_specifier_qualifier_list(TypeSpecifier *t)
{
    SpecifierQualifierList *P = new SpecifierQualifierList();
    P->type_specifiers.push_back(t);
    P->set_type();
    if (P->type_index == -1)
    {
        string error_msg = "Invalid Type " + t->name + " at line " + to_string(t->line_no) + ", column " + to_string(t->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return P;
}

SpecifierQualifierList *create_specifier_qualifier_list(SpecifierQualifierList *s, TypeSpecifier *t)
{
    s->type_specifiers.push_back(t);
    s->set_type();
    if (s->type_index == -1)
    {
        string error_msg = "Invalid Type at line " + to_string(s->type_specifiers[0]->line_no) + ", column " + to_string(s->type_specifiers[0]->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return s;
}

SpecifierQualifierList *create_specifier_qualifier_list(SpecifierQualifierList *s, int tq)
{
    s->type_qualifiers.push_back(tq);
    s->set_type();
    return s;
}

// ##############################################################################
// ################################## TYPE NAME ######################################
// ##############################################################################

TypeName ::TypeName() : NonTerminal("TYPE NAME")
{
    specifier_qualifier_list = nullptr;
    abstract_declarator = nullptr;
}

TypeName *create_type_name(SpecifierQualifierList *sql, AbstractDeclarator *ad)
{
    TypeName *P = new TypeName();
    P->specifier_qualifier_list = sql;
    P->abstract_declarator = ad;
    if (sql->type_index == -1)
    {
        P->type = ERROR_TYPE;
        return P;
    }
    if (ad == nullptr)
        P->type = Type(sql->type_index, 0, sql->is_const_variable);
    else
    {
        if (ad->direct_abstract_declarator == nullptr)
        {
            int pointer_level = 0;
            if (ad->pointer != nullptr)
                pointer_level += ad->pointer->pointer_level;
            P->type = Type(sql->type_index, pointer_level, sql->is_const_variable);
        }
        else
        {
            DirectAbstractDeclarator *dad = ad->direct_abstract_declarator;
            int pointer_level = ad->pointer->pointer_level;
            while (dad != nullptr && dad->abstract_declarator != nullptr)
            {
                if (dad->abstract_declarator->pointer != nullptr)
                    pointer_level += dad->abstract_declarator->pointer->pointer_level;
                dad = dad->abstract_declarator->direct_abstract_declarator;
            }
            P->type = Type(sql->type_index, pointer_level, sql->is_const_variable);
            if (dad->is_array)
            {
                P->type.is_array = true;
                P->type.array_dim = dad->array_dimensions.size();
                P->type.array_dims.insert(P->type.array_dims.begin(), dad->array_dimensions.begin(), dad->array_dimensions.end());
            }
            else if (dad->is_function)
            {
                P->type.is_function = true;
                vector<Type> arg_types;
                vector<ParameterDeclaration *> parameters = dad->parameters->paramater_list->parameter_declarations;
                for (int i = 0; i < parameters.size(); i++)
                {
                    arg_types.push_back(parameters[i]->type);
                }
                P->type.arg_types = arg_types;
                P->type.num_args = arg_types.size();
            }
        }
    }
    return P;
}

// ##############################################################################
// ################################## IDENTIFIER ######################################
// ##############################################################################

Identifier ::Identifier(string value, unsigned int line_no, unsigned int column_no)
    : Terminal("IDENTIFIER", value, line_no, column_no)
{
}

// ##############################################################################
// ################################## CONSTANT ######################################
// ##############################################################################

Constant ::Constant(string name, string value, unsigned int line_no, unsigned int column_no)
    : Terminal(name, value, line_no, column_no)
{
    this->constant_type = this->set_constant_type(value);
    this->value = this->convert_to_decimal();
}

Type Constant ::set_constant_type(string value)
{
    Type t(-1, 0, false);
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
        if (isFloat)
        {
            t.typeIndex = PrimitiveTypes::FLOAT_T;
        }
        else if (isDouble == 1)
        {
            t.typeIndex = PrimitiveTypes::LONG_DOUBLE_T;
        }
        else
        {
            t.typeIndex = PrimitiveTypes::DOUBLE_T;
        }
    }
    else
    {
        string error_msg = "Invalid type: " + name;
        yyerror(error_msg.c_str());
    }
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

SymbolTable::SymbolTable()
{
    currentScope = 0;
}

void SymbolTable::enterScope()
{
    currentScope++;
    cerr << currentScope << "hello\n";
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

void SymbolTable::insert(string name, Type type, int size)
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
    Symbol *sym = new Symbol(name, type, currentScope, currAddress);
    currAddress += size;
    table[name].push_front(sym);
}

void SymbolTable::insert_defined_type(std::string name, DefinedTypes type)
{
    for (auto defined_type : defined_types[name])
    {
        if (defined_type.first == currentScope)
        {
            string error_msg = " '" + name + "' already declared in this scope.";
            yyerror(error_msg.c_str());
            set_error();
            return;
        }
    }
    defined_types[name].push_front({currentScope, type});
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

bool SymbolTable::lookup_defined_type(string name)
{
    auto it = defined_types.find(name);
    if (it == defined_types.end())
        return false;

    for (pair<int, DefinedTypes> p : it->second)
    {
        if (p.first <= currentScope)
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

DefinedTypes SymbolTable::get_defined_type(std::string name)
{
    auto it = defined_types.find(name);

    DefinedTypes* types = nullptr;

    for (pair<int, DefinedTypes> p : it->second)
    {
        if (p.first <= currentScope)
        {
            types = &p.second;
            break;
        }
    }

    return *types;
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

// void SymbolTable::update_members(string name, Declarator* new_member){
//     Symbol* sym = getSymbol(name);
//     if(sym){
//         sym->members.push_back(new_member);
//     }
//     else{
//         string error_msg = "Symbol '" + name + "' not found.";
//         yyerror(error_msg.c_str());
//         set_error();
//     }
// }

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
         << "| " << setw(12) << left << "Offset" << " |\n";
    cout << "----------------------------------------------------------------------------\n";

    for (const auto &entry : table)
    {
        for (const auto symbol : entry.second)
        {
            cout << "| " << setw(20) << left << symbol->name
                 << "| " << setw(26) << left << symbol->type.typeIndex
                 // Aren ~ maine add kiya hai .typeIndex (isko change krna hai according to Type class)
                 << "| " << setw(8) << left << symbol->scope
                 << "| " << setw(12) << left << symbol->offset << " |\n";
        }
    }

    cout << "----------------------------------------------------------------------------\n";
}