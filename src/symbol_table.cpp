#include <iostream>
#include <unordered_map>
#include <list>
#include <iomanip>
#include <algorithm>
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
#include "utils.h"
#include "tac.h"

using namespace std;

#define WORD_SIZE 4

extern void yyerror(const char* msg);

SymbolTable symbolTable;

// ##############################################################################
// ################################## TYPE ######################################
// ##############################################################################

Type::Type()
{
    type_index = -1;
    ptr_level = 0;
    is_const_variable = false;
    is_const_literal = false;

    is_pointer = false;
    is_array = false;
    array_dim = 0;
    is_defined_type = false;
    defined_type_name = "";
    is_function = false;
    is_variadic = false;
    num_args = 0;
}

Type::Type(int idx, int p_lvl, bool is_con)
{
    type_index = idx;
    is_const_variable = is_con;
    is_const_literal = false;

    ptr_level = p_lvl;
    is_pointer = ptr_level > 0 ? true : false;

    is_array = false;
    array_dim = 0;

    is_function = false;
    is_variadic = false;
    num_args = 0;
    is_defined_type = false;
}

void Type::debug_type() {
    debug("Type Index: " + primitive_type_name[type_index], BLUE);
    debug("Pointer Level: " + ::to_string(ptr_level), BLUE);
    debug("Is Pointer: " + ::to_string(is_pointer), BLUE);
    debug("Is Array: " + ::to_string(is_array), BLUE);
    debug("Array Dimension: " + ::to_string(array_dim), BLUE);
    debug("Is Function: " + ::to_string(is_function), BLUE);
    debug("Number of Arguments: " + ::to_string(num_args), BLUE);
    debug("Is Defined Type: " + ::to_string(is_defined_type), BLUE);
    debug("Defined Type Name: " + defined_type_name, BLUE);
}

bool Type::isPrimitive()
{
    if (type_index >= 0 && type_index < VOID_T)
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
    if (type_index > U_CHAR_T && type_index <= LONG_LONG_T)
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
    if (type_index == U_CHAR_T || type_index == CHAR_T)
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
    if (type_index >= 10 && type_index <= 12 && ptr_level == 0)
        return true;
    else
        return false;
}

bool Type::isIntorFloat()
{
    if (type_index >= 0 && type_index <= 12 && ptr_level == 0)
        return true;
    else
        return false;
}

bool Type::isUnsigned()
{
    if (type_index == 0 || type_index == 2 || type_index == 4 || type_index == 6 || type_index == 8)
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
    if (type_index == 0 || type_index == 2 || type_index == 4 || type_index == 6 || type_index == 8)
    {
        type_index += 1;
    }
}

void Type::make_unsigned()
{
    if (type_index == 1 || type_index == 3 || type_index == 5 || type_index == 7 || type_index == 9)
    {
        type_index -= 1;
    }
}

bool Type::isVoid()
{
    if (type_index == VOID_T)
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

    if (type_index == -1)
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
    return false;
}

bool Type::is_convertible_to(Type t)
{ // CHECK
    if (*this == t){
        return true;
    }

    if (isPrimitive() && !is_pointer){
        return true;
    }
    if (is_pointer && t.is_pointer && ptr_level == t.ptr_level)
    {
        if (type_index == VOID_T || t.type_index == VOID_T){
            return true;
    }
    }
    return false;
}

Type Type::promote_to_int(Type t)
{
    if (t.type_index >= PrimitiveTypes::U_CHAR_T && t.type_index <= PrimitiveTypes::SHORT_T)
    {
        Type promoted(INT_T, 0, false); // or UNSIGNED_INT_T based on signedness
        if (t.isUnsigned())
        {
            promoted.type_index = PrimitiveTypes::U_INT_T;
        }
        return promoted;
    }
    return t;
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
            size = primitive_type_size[type_index];
        else
        {
            if (is_defined_type && symbolTable.get_defined_type(defined_type_name) != nullptr && symbolTable.get_defined_type(defined_type_name)->type_definition != nullptr)
                size = symbolTable.get_defined_type(defined_type_name)->type_definition->get_size();
            else
            {
                string error_msg = "Type " + defined_type_name + " declared but not defined at ";
                yyerror(error_msg.c_str());
                symbolTable.set_error();
            }
        }
        return size * p;
    }
    else if (ptr_level > 0 || is_function)
        return WORD_SIZE;
    else if (!isPrimitive())
    {
        DefinedTypes* dt = symbolTable.get_defined_type(defined_type_name);
        if (is_defined_type && dt != nullptr && dt->type_definition != nullptr) {
            return dt->type_definition->get_size();
        }
        else
        {
            string error_msg = "Type " + defined_type_name + " declared but not defined at ";
            yyerror(error_msg.c_str());
            symbolTable.set_error();
            return 0;
        }
    }
    else
        return primitive_type_size[type_index];
}

string Type::to_string() {
    stringstream ss;
    if (is_defined_type)
    {
        ss << defined_type_name;
    }
    else
    {
        switch (type_index)
        {
        case U_CHAR_T:
            ss << "unsigned_char";
            break;
        case CHAR_T:
            ss << "char";
            break;
        case U_SHORT_T:
            ss << "unsigned_short";
            break;
        case SHORT_T:
            ss << "short";
            break;
        case U_INT_T:
            ss << "unsigned_int";
            break;
        case INT_T:
            ss << "int";
            break;
        case U_LONG_T:
            ss << "unsigned_long";
            break;
        case LONG_T:
            ss << "long";
            break;
        case U_LONG_LONG_T:
            ss << "unsigned_long_long";
            break;
        case LONG_LONG_T:
            ss << "long_long";
            break;
        case FLOAT_T:
            ss << "float";
            break;
        case DOUBLE_T:
            ss << "double";
            break;
        case LONG_DOUBLE_T:
            ss << "long_double";
            break;
        case VOID_T:
            ss << "void";
            break;
        }
    }
    if (is_pointer) ss << "_ptr";
    return ss.str();
}

bool operator==(const Type& obj1, const Type& obj2)
{

    if (obj1.type_index != obj2.type_index)
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

bool operator!=(const Type& obj1, const Type& obj2)
{
    return !(obj1 == obj2);
}

// ##############################################################################
// ################################## TYPE DEFINITION ###########################
// ##############################################################################

// Not fully implemented

TypeDefinition::TypeDefinition(TypeCategory tc)
{
    type_category = tc;
}

bool TypeDefinition::lookup_member(string member)
{
    for (const auto& m : members) {
        if (m.name == member) {
            return true;
        }
    }
    return false;
}

int TypeDefinition::get_size()
{
    int size = 0;
    for (auto member : members)
    {
        if(member.kind == MEMBER_KIND_FUNCTION) {
            continue;
        }
        if (type_category == TYPE_CATEGORY_UNION) {
            size = max(size, member.type.get_size());
        }
        else {
            size += member.type.get_size();
        }
    }
    return size;
}

AccessSpecifiers TypeDefinition::get_member_access_specifier(const string& member)
{
    for(const auto& m : members) {
        if (m.name == member) {
            return m.access_specifier;
        }
    }
    string error_msg = "Member " + member + " not found in type definition";
    yyerror(error_msg.c_str());
    symbolTable.set_error();
    return ACCESS_SPECIFIER_PRIVATE; // Default access specifier
}

vector<MemberInfo> TypeDefinition::get_members_by_name(const string& member) {
    vector<MemberInfo> result;
    for (const auto& m : members) {
        if (m.name == member) {
            result.push_back(m);
        }
    }
    return result;
}

TypeDefinition* create_type_definition(TypeDefinition* P, StructDeclarationSet* sds)
{
    if (sds == nullptr) {
        return P;
    }
    for (StructDeclarationListAccess* sdla : sds->struct_declaration_lists)
    {
        for (int j = 0; j < sdla->struct_declaration_list->struct_declaration_list.size(); j++)
        {
            StructDeclaration* sd = sdla->struct_declaration_list->struct_declaration_list[j];
            for (int i = 0; i < sd->struct_declarator_list->struct_declarator_list.size(); i++)
            {
                StructDeclarator* d = sd->struct_declarator_list->struct_declarator_list[i];
                if (d == nullptr) {
                    // symbolTable.currAddress += d->bit_field_width;
                    // Check if this intended, I have commented it out for now
                    // I have just refactored the code :- DJ
                    return P;
                }

                Identifier* id = d->declarator->direct_declarator->identifier;
                int pointer_level = 0;
                if (d->declarator->pointer != nullptr)
                    pointer_level = d->declarator->pointer->pointer_level;
                Type t = Type(sd->specifier_qualifier_list->type_index, pointer_level, sd->specifier_qualifier_list->is_const_variable);
                if (d->declarator->direct_declarator->is_array)
                {
                    t.is_array = true;
                    t.is_pointer = true;
                    t.array_dim = d->declarator->direct_declarator->array_dimensions.size();
                    t.array_dims = d->declarator->direct_declarator->array_dimensions;
                    pointer_level += t.array_dim;
                }
                else if (d->declarator->direct_declarator->is_function)
                {
                    string error_msg = "Function cannot be part of Struct/Union at line " + to_string(sd->specifier_qualifier_list->type_specifiers[0]->line_no) + ", column " + to_string(sd->specifier_qualifier_list->type_specifiers[0]->column_no);
                    yyerror(error_msg.c_str());
                    symbolTable.set_error();
                    return P;
                }
                if (d->bit_field_width == -1)
                    symbolTable.insert(id->value, t, t.get_size(), 0);
                else
                    symbolTable.insert(id->value, t, d->bit_field_width, 0);
                MemberInfo member_info;
                member_info.name = id->value;
                member_info.type = t;
                member_info.kind = MEMBER_KIND_DATA;

                if (sdla->access_specifier->name == "PUBLIC") {
                    member_info.access_specifier = ACCESS_SPECIFIER_PUBLIC;
                    P->members.push_back(member_info);
                }
                else if (sdla->access_specifier->name == "PROTECTED") {
                    member_info.access_specifier = ACCESS_SPECIFIER_PROTECTED;
                    P->members.push_back(member_info);
                }
                else if (sdla->access_specifier->name == "PRIVATE") {
                    member_info.access_specifier = ACCESS_SPECIFIER_PRIVATE;
                    P->members.push_back(member_info);
                }
                else {
                    member_info.access_specifier = ACCESS_SPECIFIER_PUBLIC; // default access specifier
                    P->members.push_back(member_info);
                }
            }
        }
    }
    return P;
}

TypeDefinition* create_type_definition(TypeDefinition* P, ClassDeclaratorList* idl, ClassDeclarationList* cdl)
{
    if (cdl == nullptr)
        return P;
    for (ClassDeclaration* cd : cdl->class_declaration_list)
    {
        for (int i = 0; i < cd->translation_unit->external_declarations.size(); i++)
        {
            Declaration* d = cd->translation_unit->external_declarations[i]->declaration;
            FunctionDefinition* fd = cd->translation_unit->external_declarations[i]->function_definition;

            if (d == nullptr && fd == nullptr) {
                string error_msg = "Invalid external declaration";
                yyerror(error_msg.c_str());
                symbolTable.set_error();
                return P;
            }

            if (d != nullptr) {
                if (d->init_declarator_list == nullptr)
                {
                    string error_msg = "Struct/Union/Class cannot be part of Class";
                    yyerror(error_msg.c_str());
                    symbolTable.set_error();
                    return P;
                }
                for (int j = 0; j < d->init_declarator_list->init_declarator_list.size(); j++)
                {
                    Declarator* dec = d->init_declarator_list->init_declarator_list[j]->declarator;
                    if (dec == nullptr) continue;

                    DirectDeclarator* dd = dec->direct_declarator;
                    if (dd == nullptr) {
                        string error_msg = "Direct  Declarator is null in a class";
                        yyerror(error_msg.c_str());
                        symbolTable.set_error();
                        return P;
                    }
                    Identifier* id = dd->identifier;
                    int ptr_lvl = 0;
                    if (dec->pointer) {
                        ptr_lvl = dec->pointer->pointer_level;
                    }

                    Type t = Type(d->declaration_specifiers->type_index, ptr_lvl, d->declaration_specifiers->is_const_variable);
                    if (dec->direct_declarator->is_array)
                    {
                        t.is_array = true;
                        t.is_pointer = true;
                        t.array_dim = dec->direct_declarator->array_dimensions.size();
                        t.array_dims = dec->direct_declarator->array_dimensions;
                        t.ptr_level += t.array_dim;
                    }

                    MemberInfo member_info;
                    member_info.name = id->value;
                    member_info.type = t;
                    member_info.kind = MEMBER_KIND_DATA;

                    if (cd->access_specifier->name == "PUBLIC") {
                        member_info.access_specifier = ACCESS_SPECIFIER_PUBLIC;
                        P->members.push_back(member_info);
                    }
                    else if (cd->access_specifier->name == "PROTECTED") {
                        member_info.access_specifier = ACCESS_SPECIFIER_PROTECTED;
                        P->members.push_back(member_info);
                    }
                    else if (cd->access_specifier->name == "PRIVATE") {
                        member_info.access_specifier = ACCESS_SPECIFIER_PRIVATE;
                        P->members.push_back(member_info);
                    }
                    else {
                        member_info.access_specifier = ACCESS_SPECIFIER_PRIVATE; // default access specifier
                        P->members.push_back(member_info);
                    }
                    debug("Member: " + member_info.name + " Access: " + to_string(member_info.access_specifier), BLUE);

                }
            }
            else if (fd != nullptr)
            {
                Type t;
                t.is_function = true;
                if (fd->declaration_specifiers == nullptr) {
                    string error_msg = "Function declaration specifiers are null";
                    yyerror(error_msg.c_str());
                    symbolTable.set_error();
                    return P;
                }
                t.type_index = fd->declaration_specifiers->type_index;
                if (fd->declarator == nullptr || fd->declarator->direct_declarator == nullptr) {
                    string error_msg = "Function declarator is null";
                    yyerror(error_msg.c_str());
                    symbolTable.set_error();
                    return P;
                }
                if (fd->declarator->direct_declarator->parameters == nullptr || fd->declarator->direct_declarator->parameters->paramater_list == nullptr) {
                    t.num_args = 0;
                } else {
                    t.num_args = fd->declarator->direct_declarator->parameters->paramater_list->parameter_declarations.size();
                }
                for (int i = 0; i < t.num_args; i++)
                    t.arg_types.push_back(fd->declarator->direct_declarator->parameters->paramater_list->parameter_declarations[i]->type);

                Identifier* id = fd->declarator->direct_declarator->identifier;
                if (id == nullptr) {
                    string error_msg = "Function identifier is null";
                    yyerror(error_msg.c_str());
                    symbolTable.set_error();
                    return P;
                }
                MemberInfo member_info;
                member_info.name = id->value;
                member_info.type = t;
                member_info.kind = MEMBER_KIND_FUNCTION;

                if (cd->access_specifier->name == "PUBLIC") {
                    member_info.access_specifier = ACCESS_SPECIFIER_PUBLIC;
                    P->members.push_back(member_info);
                }
                else if (cd->access_specifier->name == "PROTECTED") {
                    member_info.access_specifier = ACCESS_SPECIFIER_PROTECTED;
                    P->members.push_back(member_info);
                }
                else if (cd->access_specifier->name == "PRIVATE") {
                    member_info.access_specifier = ACCESS_SPECIFIER_PRIVATE;
                    P->members.push_back(member_info);
                }
                else {
                    member_info.access_specifier = ACCESS_SPECIFIER_PRIVATE; // default access specifier
                    P->members.push_back(member_info);
                }

                debug("Member function: " + member_info.name + " Access: " + to_string(member_info.access_specifier), BLUE);
            }
        }
    }
    if (idl != nullptr)
    {
        for (ClassDeclarator* cd : idl->class_declarator_list)
        {
            std::string type_name = cd->declarator->direct_declarator->identifier->value;
            DefinedTypes* dt = symbolTable.get_defined_type(type_name);
            TypeDefinition* t = nullptr;
            if (dt != nullptr)
                t = dt->type_definition;

            if (t == nullptr) {
                string error_msg = "Type " + type_name + " not defined";
                yyerror(error_msg.c_str());
                symbolTable.set_error();
                continue;
            }

            for (auto& member_info : t->members)
            {
                auto& [member_name, member_type, member_kind, access_specifier] = member_info;
                if(P->lookup_member(member_name)) {
                    debug("Member: " + member_name + " already exists in type " + type_name, BLUE);
                    continue;
                }

                Symbol* sym = t->type_symbol_table.getSymbol(member_name);
                bool is_typedef = false;

                if (sym == nullptr)
                {
                    sym = t->type_symbol_table.getTypedef(member_name);
                    is_typedef = true;
                }

                if (sym == nullptr)
                    continue;

                AccessSpecifiers declared_access = ACCESS_SPECIFIER_PRIVATE;

                if (access_specifier == ACCESS_SPECIFIER_PUBLIC)
                {
                    if (cd->access_specifier->name == "PUBLIC")
                        declared_access = ACCESS_SPECIFIER_PUBLIC;
                    else if (cd->access_specifier->name == "PROTECTED")
                        declared_access = ACCESS_SPECIFIER_PROTECTED;
                    else
                        declared_access = ACCESS_SPECIFIER_PRIVATE;
                }
                else if (access_specifier == ACCESS_SPECIFIER_PROTECTED)
                {
                    if (cd->access_specifier->name == "PUBLIC" || cd->access_specifier->name == "PROTECTED")
                        declared_access = ACCESS_SPECIFIER_PROTECTED;
                    else
                        declared_access = ACCESS_SPECIFIER_PRIVATE;
                }
                else
                {
                    declared_access = ACCESS_SPECIFIER_PRIVATE;
                }

                // Add member info
                MemberInfo m_member_info;
                m_member_info.name = member_name;
                m_member_info.type = sym->type;
                m_member_info.kind = sym->type.is_function ? MEMBER_KIND_FUNCTION : MEMBER_KIND_DATA;
                m_member_info.access_specifier = declared_access;

                P->members.push_back(m_member_info);

                // Add to symbol table
                Symbol* sym_copy = new Symbol(*sym);

                if (is_typedef)
                {
                    if (!P->type_symbol_table.lookup_typedef(member_name))
                        P->type_symbol_table.typedefs[member_name].push_front(sym_copy);
                }
                else
                {
                    if (!P->type_symbol_table.lookup_function(member_name, sym->type.arg_types)){
                        P->type_symbol_table.table[member_name].push_front(sym_copy);
                    }
                }
            }
        }
    }

    return P;
}

// ##############################################################################
// ################################## DEFINED TYPES ######################################
// ##############################################################################
int DefinedTypes::t_index_count = PrimitiveTypes::N_PRIMITIVE_TYPES;

DefinedTypes::DefinedTypes(TypeCategory tc, TypeDefinition* td) : Type(t_index_count++, 0, false)
{
    is_defined_type = true;
    type_category = tc;
    type_definition = td;
}

// ##############################################################################
// ################################## TYPE QUALIFIER LIST ######################################
// ##############################################################################

TypeQualifierList::TypeQualifierList() : NonTerminal("TYPE QUALIFIER LIST") {}

TypeQualifierList* create_type_qualifier_list(int typequalifier)
{
    TypeQualifierList* P = new TypeQualifierList();
    P->type_qualifier_list.push_back(typequalifier);
    return P;
}

TypeQualifierList* create_type_qualifier_list(TypeQualifierList* typequalifierlist, int typequalifier)
{
    typequalifierlist->type_qualifier_list.push_back(typequalifier);
    return typequalifierlist;
}

// ##############################################################################
// ############################# DECLARATION ####################################
// ##############################################################################

Declaration::Declaration()
    : NonTerminal("DECLARATION")
{
    declaration_specifiers = nullptr;
    init_declarator_list = nullptr;
}

Declaration* create_declaration(DeclarationSpecifiers* declaration_specifiers,
    InitDeclaratorList* init_declarator_list)
{
    Declaration* P = new Declaration();

    P->declaration_specifiers = declaration_specifiers;
    P->init_declarator_list = init_declarator_list;

    if (init_declarator_list == nullptr)
    {
        return P;
    }
    for (int index = 0; index < init_declarator_list->init_declarator_list.size(); index++)
    {
        Declarator* variable = init_declarator_list->init_declarator_list[index]->declarator;
        int ptr_level = 0;
        int overloaded = 0;
        if (variable->pointer != nullptr)
            ptr_level = variable->pointer->pointer_level;
        Type t;
        if (declaration_specifiers->is_type_name)
        {
            t = symbolTable.getTypedef(declaration_specifiers->type_specifiers[0]->type_name)->type;
            t.ptr_level += ptr_level;
            if (t.ptr_level > 0)
                t.is_pointer = true;
            if (P->declaration_specifiers->is_const_variable)
                t.is_const_variable = true;
        }
        else
            t = Type(P->declaration_specifiers->type_index, ptr_level, P->declaration_specifiers->is_const_variable);
        if(P->declaration_specifiers->is_static) t.is_static = true;
        if (variable->direct_declarator->is_array)
        {
            t.is_array = true;
            t.is_pointer = true;
            t.array_dim = variable->direct_declarator->array_dimensions.size();
            t.array_dims = variable->direct_declarator->array_dimensions;
            t.ptr_level += t.array_dim;
        }
        if (variable->direct_declarator->is_function)
        {
            t.is_function = true;
            t.num_args = variable->direct_declarator->parameters->paramater_list->parameter_declarations.size();
            for (int i = 0; i < t.num_args; i++)
                t.arg_types.push_back(variable->direct_declarator->parameters->paramater_list->parameter_declarations[i]->type);
            overloaded = 1;
        }
        if (!t.isPrimitive()) {
            t.is_defined_type = true;
            if (declaration_specifiers->type_specifiers[0]->struct_union_specifier != nullptr)t.defined_type_name = declaration_specifiers->type_specifiers[0]->struct_union_specifier->identifier->value;
            else if (declaration_specifiers->type_specifiers[0]->class_specifier != nullptr)t.defined_type_name = declaration_specifiers->type_specifiers[0]->class_specifier->identifier->value;
        }
        // if (ptr_level == 0 && declaration_specifiers->type_index == 13)
        // {
        //     string error_msg = "Variable of field '" + variable->direct_declarator->identifier->value + "' declared void at line " + to_string(variable->direct_declarator->identifier->line_no) + ", column " + to_string(variable->direct_declarator->identifier->column_no);
        //     yyerror(error_msg.c_str());
        //     symbolTable.set_error();
        //     return P;
        // }
        if (init_declarator_list->init_declarator_list[index]->initializer != nullptr)
        {
            bool compatible = init_declarator_list->init_declarator_list[index]->initializer->assignment_expression->type.is_convertible_to(t);
            if (!compatible)
            {
                string error_msg = "Incompatible types while initializing variable '" + variable->direct_declarator->identifier->value + "' at line " + to_string(variable->direct_declarator->identifier->line_no) + ", column " + to_string(variable->direct_declarator->identifier->column_no);
                yyerror(error_msg.c_str());
                symbolTable.set_error();
                return P;
            }
            else{
                auto i = init_declarator_list->init_declarator_list[index]->initializer;
                TACOperand* id = new_identifier(variable->direct_declarator->identifier->value); // TAC
                init_declarator_list->init_declarator_list[index]->code.insert(init_declarator_list->init_declarator_list[index]->code.end(), i->assignment_expression->code.begin(), i->assignment_expression->code.end()); // TAC
                if(i->assignment_expression->result->type != TAC_OPERAND_TEMP_VAR){
                    TACOperand* t1 = new_temp_var(); // TAC
                    TACInstruction* i0;
                    if(t.type_index != i->assignment_expression->type.type_index) i0 = emit(TACOperator(TAC_OPERATOR_CAST),t1, new_type(t.to_string()) , i->assignment_expression->result, 0); // TAC
                    else i0 = emit(TACOperator(TAC_OPERATOR_NOP),t1, i->assignment_expression->result, new_empty_var(), 0); // TAC
                    TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), id, t1, new_empty_var(), 0); // TAC
                    init_declarator_list->init_declarator_list[index]->code.push_back(i0); // TAC
                    init_declarator_list->init_declarator_list[index]->code.push_back(i1); // TAC
                    for(auto l:i->assignment_expression->jump_next_list){
                        init_declarator_list->init_declarator_list[index]->code.erase(remove(init_declarator_list->init_declarator_list[index]->code.begin(), init_declarator_list->init_declarator_list[index]->code.end(), l), init_declarator_list->init_declarator_list[index]->code.end()); // TAC
                    }
                    backpatch(i->assignment_expression->next_list, i0->label); // TAC
                    backpatch(i->assignment_expression->jump_next_list, i0->label); // TAC
                    backpatch(i->assignment_expression->true_list, i0->label); // TAC
                    backpatch(i->assignment_expression->false_list, i0->label); // TAC
                    backpatch(i->assignment_expression->jump_true_list, i0->label); // TAC
                    backpatch(i->assignment_expression->jump_false_list, i0->label); // TAC
                }
                else{
                    TACInstruction* i1;
                    if(t.type_index != i->assignment_expression->type.type_index)i1 = emit(TACOperator(TAC_OPERATOR_CAST), id, new_type(t.to_string()), i->assignment_expression->result, 0); // TAC
                    else i1 = emit(TACOperator(TAC_OPERATOR_NOP), id, i->assignment_expression->result, new_empty_var(), 0); // TAC
                    init_declarator_list->init_declarator_list[index]->code.push_back(i1); // TAC
                    for(auto l:i->assignment_expression->jump_next_list){
                        init_declarator_list->init_declarator_list[index]->code.erase(remove(init_declarator_list->init_declarator_list[index]->code.begin(), init_declarator_list->init_declarator_list[index]->code.end(), l), init_declarator_list->init_declarator_list[index]->code.end()); // TAC
                    }
                    backpatch(i->assignment_expression->next_list, i1->label); // TAC
                    backpatch(i->assignment_expression->jump_next_list, i1->label); // TAC
                    backpatch(i->assignment_expression->true_list, i1->label); // TAC
                    backpatch(i->assignment_expression->false_list, i1->label); // TAC
                    backpatch(i->assignment_expression->jump_true_list, i1->label); // TAC
                    backpatch(i->assignment_expression->jump_false_list, i1->label); // TAC
                }
            }
        }
        else if(symbolTable.currentScope == 0){
            TACOperand* id = new_identifier(variable->direct_declarator->identifier->value); // TAC
            TACOperand* t1 = new_temp_var(); // TAC
            Constant* c = new Constant("I_CONSTANT", "0", init_declarator_list->init_declarator_list[index]->declarator->direct_declarator->identifier->line_no, init_declarator_list->init_declarator_list[index]->declarator->direct_declarator->identifier->column_no); // TAC
            TACInstruction* i0;
            if(t.type_index != c->constant_type.type_index) i0 = emit(TACOperator(TAC_OPERATOR_CAST),t1, new_type(t.to_string()) , new_constant("0"), 0); // TAC
            else i0 = emit(TACOperator(TAC_OPERATOR_NOP),t1, new_constant("0"), new_empty_var(), 0); // TAC
            TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), id, t1, new_empty_var(), 0); // TAC
            init_declarator_list->init_declarator_list[index]->code.push_back(i0); // TAC
            init_declarator_list->init_declarator_list[index]->code.push_back(i1); // TAC
        }
        if (declaration_specifiers->is_typedef)
            symbolTable.insert_typedef(variable->direct_declarator->identifier->value, t, t.get_size());
        else {
            symbolTable.insert(variable->direct_declarator->identifier->value, t, t.get_size(), overloaded);
        }
    }
    return P;
}

// ##############################################################################
// ############################# DECLARATION LIST ####################################
// ##############################################################################

DeclarationList::DeclarationList() : NonTerminal("DECLARATION LIST") {}

DeclarationList* create_declaration_list(Declaration* d)
{
    DeclarationList* P = new DeclarationList();
    P->declaration_list.push_back(d);
    return P;
}

DeclarationList* create_declaration_list(DeclarationList* dl, Declaration* d)
{
    dl->declaration_list.push_back(d);
    return dl;
}

// ##############################################################################
// ############################# INITIALIZER LIST ####################################
// ##############################################################################

// ##############################################################################
// ################################## INITIALIZER ######################################
// ##############################################################################

// ##############################################################################
// ############################# IDENTIFIER LIST ####################################
// ##############################################################################

IdentifierList::IdentifierList() : NonTerminal("IDENTIFIER LIST") {}

IdentifierList* create_identifier_list(Identifier* i)
{
    IdentifierList* P = new IdentifierList();
    P->identifiers.push_back(i);
    return P;
}

IdentifierList* create_identifier_list(IdentifierList* il, Identifier* i)
{
    il->identifiers.push_back(i);
    return il;
}

// ##############################################################################
// ################################## DECLARATION SPECIFIERS ######################################
// ##############################################################################

DeclarationSpecifiers::DeclarationSpecifiers() : NonTerminal("DECLARATION SPECIFIERS") {
    is_const_variable = false;
    is_typedef = false;
    is_type_name = false;
    type_index = PrimitiveTypes::TYPE_ERROR_T;
}

void DeclarationSpecifiers::set_type()
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
    int isClass = 0;
    int isEnum = 0;
    int isUnionOrStruct = 0;
    int isTypeName = 0;

    for (int i = 0; i < type_qualifiers.size(); i++)
    {
        if (type_qualifiers[i] == CONST)
            is_const_variable = true;
        else if (type_qualifiers[i] == VOLATILE)
            ; // add something later}
    }
    for (int i = 0; i < type_specifiers.size(); i++)
    {
        if (type_specifiers[i]->primitive_type_specifier != nullptr)
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
        }
        else if (type_specifiers[i]->enum_specifier != nullptr)
        {
            isEnum++;
        }
        else if (type_specifiers[i]->struct_union_specifier != nullptr)
        {
            isUnionOrStruct++;
        }
        else if (type_specifiers[i]->class_specifier != nullptr)
        {
            isClass++;
        }
        else if (type_specifiers[i]->type_name != "")
        {
            isTypeName++;
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
        else if (isVoid)
        {
            type_index = PrimitiveTypes::VOID_T;
        }
        else if (isEnum)
        {
            type_index = PrimitiveTypes::INT_T;
        }
        else if (isUnionOrStruct)
        {
            string name = type_specifiers[0]->struct_union_specifier->identifier->value;
            DefinedTypes* dt = symbolTable.get_defined_type(name);
            type_index = PrimitiveTypes::TYPE_ERROR_T;
            if (dt != nullptr) type_index = dt->type_index;
        }
        else if (isClass)
        {
            string name = type_specifiers[0]->class_specifier->identifier->value;
            DefinedTypes* dt = symbolTable.get_defined_type(name);
            type_index = PrimitiveTypes::TYPE_ERROR_T; 
            if(dt != nullptr) type_index = dt->type_index;
        }
        else if (isTypeName)
        {
            Symbol* sym = symbolTable.getTypedef(type_specifiers[0]->type_name);
            if(sym == nullptr){
                string error_msg = "Type name " + type_specifiers[0]->type_name + " not found";
                yyerror(error_msg.c_str());
                symbolTable.set_error();
                return;
            }
            type_index = sym->type.type_index;
            is_type_name = true;
        }
    }
}

DeclarationSpecifiers* create_declaration_specifiers(SpecifierQualifierList* sql)
{
    DeclarationSpecifiers* P = new DeclarationSpecifiers();
    P->type_specifiers.insert(P->type_specifiers.end(), sql->type_specifiers.begin(), sql->type_specifiers.end());
    if (sql) sql->set_type();
    P->set_type();
    if (P->type_index == -1)
    {
        string error_msg = "Invalid Type";
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return P;
}

DeclarationSpecifiers* create_declaration_specifiers(DeclarationSpecifiers* ds, int storage_class)
{
    ds->storage_class_specifiers.push_back(storage_class);
    if (storage_class == TYPEDEF)
        ds->is_typedef = true;
    if (storage_class == STATIC)
        ds->is_static = true;
    return ds;
}

DeclarationSpecifiers* create_declaration_specifiers(DeclarationSpecifiers* ds, SpecifierQualifierList* sql)
{
    ds->type_specifiers.insert(ds->type_specifiers.end(), sql->type_specifiers.begin(), sql->type_specifiers.end());
    if (sql) sql->set_type();
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
Pointer::Pointer() : NonTerminal("POINTER")
{
    type_qualifier_list = nullptr;
    pointer_level = 0;
}

Pointer* create_pointer(TypeQualifierList* tql)
{
    Pointer* P = new Pointer();
    P->pointer_level++;
    P->type_qualifier_list = tql;
    return P;
}

Pointer* create_pointer(Pointer* p, TypeQualifierList* tql)
{
    p->pointer_level++;
    p->type_qualifier_list = tql;
    return p;
}

// ##############################################################################
// ############################ DIRECT DECLARATOR ###############################
// ##############################################################################

DirectDeclarator::DirectDeclarator()
    : NonTerminal("direct_declarator")
{
    declarator = nullptr;
    identifier = nullptr;
    parameters = nullptr;
    is_function = false;
    is_array = false;
}

DirectDeclarator* create_dir_declarator_id(Identifier* i)
{
    DirectDeclarator* dd = new DirectDeclarator();
    dd->identifier = i;
    return dd;
}

DirectDeclarator* create_direct_declarator(Declarator* d)
{
    DirectDeclarator* P = new DirectDeclarator();
    P->declarator = d;
    return P;
}

DirectDeclarator* create_direct_declarator_array(DirectDeclarator* dd, Expression* e)
{
    if (dd->is_function)
    {
        string error_msg = "Type name cannot be an array of functions " + to_string(e->line_no) + ", column " + to_string(e->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return dd;
    }
    if ((e->type.isInt() && e->type.is_const_literal))
    {
        ConditionalExpression* c_cast = dynamic_cast<ConditionalExpression*>(e);
        int arr_dim = stoi(c_cast->logical_or_expression->logical_and_expression->or_expression->xor_expression->and_expression->equality_expression->relational_expression->shift_expression->additive_expression->multiplicative_expression->cast_expression->unary_expression->postfix_expression->primary_expression->constant->value);
        dd->array_dimensions.push_back(arr_dim);
        dd->is_array = true;
    }
    else
    {
        string error_msg = "Array size must be a constant integer at line " + to_string(e->line_no) + ", column " + to_string(e->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return dd;
}

DirectDeclarator* create_direct_declarator_function(DirectDeclarator* dd, ParameterTypeList* ptl)
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

Declarator::Declarator()
    : NonTerminal("declarator")
{
    pointer = nullptr;
    direct_declarator = nullptr;
};

Declarator* create_declarator(Pointer* p, DirectDeclarator* dd)
{
    Declarator* d = new Declarator();
    d->pointer = p;
    d->direct_declarator = dd;
    // d->add_children( direct_declarator );
    return d;
}

// ##############################################################################
// ################################## PARAMETER DECLARATION ######################################
// ##############################################################################

ParameterDeclaration::ParameterDeclaration() : NonTerminal("PARAMETER DECLARATION")
{
    declarations_specifiers = nullptr;
    abstract_declarator = nullptr;
    declarator = nullptr;
    type = Type();
}

ParameterDeclaration* create_parameter_declaration(DeclarationSpecifiers* ds, AbstractDeclarator* ad)
{
    ParameterDeclaration* P = new ParameterDeclaration();
    P->declarations_specifiers = ds;
    P->abstract_declarator = ad;
    int pointer_level = 0;
    if (ad->pointer != nullptr)
        pointer_level = ad->pointer->pointer_level;
    if(ad->direct_abstract_declarator->is_array)
        pointer_level+= ad->direct_abstract_declarator->array_dimensions.size();
    P->type = Type(ds->type_index, pointer_level, ds->is_const_variable);
    return P;
}

ParameterDeclaration* create_parameter_declaration(DeclarationSpecifiers* ds, Declarator* d)
{
    ParameterDeclaration* P = new ParameterDeclaration();
    P->declarations_specifiers = ds;
    P->declarator = d;
    int pointer_level = 0;
    if (d->pointer != nullptr)
        pointer_level = d->pointer->pointer_level;
    if(d->direct_declarator->is_array)
        pointer_level+= d->direct_declarator->array_dimensions.size();
    P->type = Type(ds->type_index, pointer_level, ds->is_const_variable);
    return P;
}

ParameterDeclaration* create_parameter_declaration(DeclarationSpecifiers* ds)
{
    ParameterDeclaration* P = new ParameterDeclaration();
    P->declarations_specifiers = ds;
    P->type = Type(ds->type_index, 0, ds->is_const_variable);
    return P;
}

// ##############################################################################
// ################################## PARAMETER LIST ######################################
// ##############################################################################

ParameterList::ParameterList() : NonTerminal("PARAMETER LIST") {}

ParameterList* create_parameter_list(ParameterDeclaration* pd)
{
    ParameterList* pl = new ParameterList();
    pl->parameter_declarations.push_back(pd);
    return pl;
}

ParameterList* create_parameter_list(ParameterList* pl, ParameterDeclaration* pd)
{
    pl->parameter_declarations.push_back(pd);
    return pl;
}

// ##############################################################################
// ################################## PARAMETER TYPE LIST ######################################
// ##############################################################################

ParameterTypeList::ParameterTypeList() : NonTerminal("PARAMETER TYPE LIST")
{
    paramater_list = nullptr;
    is_variadic = false;
}

ParameterTypeList* create_parameter_type_list(ParameterList* pl, bool var)
{
    ParameterTypeList* P = new ParameterTypeList();
    P->paramater_list = pl;
    P->is_variadic = var;
    return P;
}

// ##############################################################################
// ############################# INIT DECLARATOR LIST ################################
// ##############################################################################

InitDeclaratorList::InitDeclaratorList() : NonTerminal("INIT DECLARATOR LIST") {};

InitDeclaratorList* create_init_declarator_list(InitDeclarator* id)
{
    InitDeclaratorList* P = new InitDeclaratorList();
    P->init_declarator_list.push_back(id);
    return P;
}

InitDeclaratorList* create_init_declarator_list(InitDeclaratorList* idl, InitDeclarator* id)
{
    idl->init_declarator_list.push_back(id);
    return idl;
}

// ##############################################################################
// ################################## ABSTRACT DECLARATOR ######################################
// ##############################################################################

AbstractDeclarator::AbstractDeclarator() : NonTerminal("ABSTRACT DECLARATOR")
{
    pointer = nullptr;
    direct_abstract_declarator = nullptr;
}

AbstractDeclarator* create_abstract_declarator(Pointer* p, DirectAbstractDeclarator* dad)
{
    AbstractDeclarator* P = new AbstractDeclarator();
    P->pointer = p;
    P->direct_abstract_declarator = dad;
    return P;
}

// ##############################################################################
// ################################## DIRECT ABSTRACT DECLARATOR ######################################
// ##############################################################################

DirectAbstractDeclarator::DirectAbstractDeclarator() : NonTerminal("DIRECT ABSTRACT DECLARATOR")
{
    abstract_declarator = nullptr;
    is_function = false;
    parameters = nullptr;
    is_array = false;
}

DirectAbstractDeclarator* create_direct_abstract_declarator(AbstractDeclarator* x)
{
    DirectAbstractDeclarator* P = new DirectAbstractDeclarator();
    P->abstract_declarator = x;
    return P;
}

DirectAbstractDeclarator* create_direct_abstract_declarator_array(Expression* c)
{
    DirectAbstractDeclarator* P = new DirectAbstractDeclarator();
    P->is_array = true;
    if (c == nullptr || (c->type.isInt() && c->type.is_const_literal))
    {
        ConditionalExpression* c_cast = dynamic_cast<ConditionalExpression*>(c);
        P->array_dimensions.push_back(stoi(c_cast->logical_or_expression->logical_and_expression->or_expression->xor_expression->and_expression->equality_expression->relational_expression->shift_expression->additive_expression->multiplicative_expression->cast_expression->unary_expression->postfix_expression->primary_expression->constant->value));
    }
    else
    {
        string error_msg = "Array size must be a constant integer at line " + to_string(c->line_no) + ", column " + to_string(c->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
    }
    return P;
}

DirectAbstractDeclarator* create_direct_abstract_declarator_function(ParameterTypeList* p)
{
    DirectAbstractDeclarator* P = new DirectAbstractDeclarator();
    P->is_function = true;
    P->parameters = p;
    return P;
}

DirectAbstractDeclarator* create_direct_abstract_declarator_array(DirectAbstractDeclarator* x, Expression* c)
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
        PrimaryExpression* c_cast = dynamic_cast<PrimaryExpression*>(c);
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

DirectAbstractDeclarator* create_direct_abstract_declarator_function(DirectAbstractDeclarator* x, ParameterTypeList* p)
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
StructUnionSpecifier::StructUnionSpecifier() : NonTerminal("STRUCT UNION SPECIFIER")
{
    identifier = nullptr;
    struct_declaration_set = nullptr;
    type_category = TYPE_CATEGORY_ERROR;
}

StructUnionSpecifier* create_struct_union_specifier(string struct_or_union, Identifier* id)
{
    StructUnionSpecifier* P = new StructUnionSpecifier();
    if (struct_or_union == "STRUCT")
        P->type_category = TYPE_CATEGORY_STRUCT;
    else
        P->type_category = TYPE_CATEGORY_UNION;

    P->identifier = id;
    TypeDefinition* td = new TypeDefinition(P->type_category);
    DefinedTypes* dt = new DefinedTypes(P->type_category, td);
    dt->defined_type_name = id->value;
    symbolTable.insert_defined_type(id->value, dt);
    return P;
}

StructUnionSpecifier* create_struct_union_specifier(StructUnionSpecifier* sus, StructDeclarationSet* sds)
{
    DefinedTypes* dt = symbolTable.get_defined_type(sus->identifier->value);
    TypeDefinition* td = dt->type_definition;
    sus->struct_declaration_set = sds;
    if (sds != nullptr)
    {
        td = create_type_definition(td, sds);
        dt->type_definition = td;
        // td->type_symbol_table.print();
        // TypeDefinition *td;
        // DefinedTypes dt = DefinedTypes(sus->type_category, td);
        // dt.defined_type_name = id->value;
        // symbolTable.insert_defined_type(id->value, dt);
        // create_type_definition(P->type_category, sds);
    }
    return sus;
}

StructUnionSpecifier* create_struct_union_specifier(string struct_or_union, Identifier* id, StructDeclarationSet* sds)
{
    StructUnionSpecifier* P = new StructUnionSpecifier();
    if (struct_or_union == "STRUCT")
        P->type_category = TYPE_CATEGORY_STRUCT;
    else
        P->type_category = TYPE_CATEGORY_UNION;
    P->identifier = id;

    // TypeDefinition* td = symbolTable.get_defined_type(id->value)->type_definition;
    // Unused variable

    return P;
}

// ##############################################################################
// ############################ CLASS SPECIFIER #########################
// ##############################################################################
ClassSpecifier::ClassSpecifier() : NonTerminal("CLASS SPECIFIER")
{
    type_category = TYPE_CATEGORY_CLASS;
    identifier = nullptr;
    class_declarator_list = nullptr;
    class_declaration_list = nullptr;
}

ClassSpecifier* create_class_specifier(Identifier* id)
{
    ClassSpecifier* P = new ClassSpecifier();
    P->identifier = id;
    TypeDefinition* td = new TypeDefinition(TYPE_CATEGORY_CLASS);
    DefinedTypes* dt = new DefinedTypes(TYPE_CATEGORY_CLASS, td);
    dt->defined_type_name = id->value;
    symbolTable.insert_defined_type(id->value, dt);
    return P;
}

ClassSpecifier* create_class_specifier(ClassSpecifier* cs, ClassDeclaratorList* idl, ClassDeclarationList* cdl)
{
    DefinedTypes* dt = symbolTable.get_defined_type(cs->identifier->value);
    if (dt == nullptr) {
        string error_msg = "Class '" + cs->identifier->value + "' not defined at line " + to_string(cs->identifier->line_no) + ", column " + to_string(cs->identifier->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return cs;
    }
    TypeDefinition* td = dt->type_definition;
    if(td == nullptr){
        string error_msg = "Class '" + cs->identifier->value + "' not defined at line " + to_string(cs->identifier->line_no) + ", column " + to_string(cs->identifier->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return cs;
    }
    cs->class_declarator_list = idl;
    cs->class_declaration_list = cdl;
    if (cdl != nullptr) {
        td = create_type_definition(td, idl, cdl);
        dt->type_definition = td;
    }
    return cs;
}

ClassSpecifier* create_class_specifier(Identifier* id, ClassDeclaratorList* idl, ClassDeclarationList* cdl)
{
    ClassSpecifier* P = new ClassSpecifier();
    P->identifier = id;
    DefinedTypes* dt = symbolTable.get_defined_type(id->value);
    if (dt == nullptr)
    {
        string error_msg = "Class '" + id->value + "' not defined at line " + to_string(id->line_no) + ", column " + to_string(id->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    TypeDefinition* td = dt->type_definition;
    return P;
}

// ##############################################################################
// ############################ CLASS DECLARATOR LIST #########################
// ##############################################################################
ClassDeclaratorList::ClassDeclaratorList() : NonTerminal("CLASS DECLARATOR LIST") {}

ClassDeclaratorList* create_class_declarator_list(ClassDeclarator* cd)
{
    ClassDeclaratorList* P = new ClassDeclaratorList();
    P->class_declarator_list.push_back(cd);
    return P;
}

ClassDeclaratorList* create_class_declarator_list(ClassDeclaratorList* cdl, ClassDeclarator* cd)
{
    cdl->class_declarator_list.push_back(cd);
    return cdl;
}

// ##############################################################################
// ############################ CLASS DECLARATOR #########################
// ##############################################################################
ClassDeclarator::ClassDeclarator() : NonTerminal("CLASS DECLARATOR")
{
    access_specifier = nullptr;
    declarator = nullptr;
}

ClassDeclarator* create_class_declarator(Terminal* access_specifier, Declarator* d)
{
    ClassDeclarator* P = new ClassDeclarator();
    if (access_specifier == nullptr)
        access_specifier = new Terminal("PRIVATE", "private", 0, 0);
    P->access_specifier = access_specifier;
    P->declarator = d;
    return P;
}

// ##############################################################################
// ############################ CLASS DECLARATION LIST #########################
// ##############################################################################
ClassDeclarationList::ClassDeclarationList() : NonTerminal("CLASS DECLARATION LIST") {}

ClassDeclarationList* create_class_declaration_list(ClassDeclaration* cd)
{
    ClassDeclarationList* P = new ClassDeclarationList();
    P->class_declaration_list.push_back(cd);
    return P;
}

ClassDeclarationList* create_class_declaration_list(ClassDeclarationList* cdl, ClassDeclaration* cd)
{
    cdl->class_declaration_list.push_back(cd);
    return cdl;
}

// ##############################################################################
// ############################ CLASS DECLARATION #########################
// ##############################################################################
ClassDeclaration::ClassDeclaration() : NonTerminal("CLASS DECLARATION")
{
    access_specifier = nullptr;
    translation_unit = nullptr;
}

ClassDeclaration* create_class_declaration(Terminal* access_specifier, TranslationUnit* tu)
{
    ClassDeclaration* P = new ClassDeclaration();
    if (access_specifier == nullptr)
        access_specifier = new Terminal("PRIVATE", "private", 0, 0);
    P->access_specifier = access_specifier;
    P->translation_unit = tu;
    return P;
}

// ##############################################################################
// ############################ STRUCT DECLARATION SET #########################
// ##############################################################################
StructDeclarationSet::StructDeclarationSet() : NonTerminal("STRUCT DECLARATION SET") {}

StructDeclarationSet* create_struct_declaration_set(StructDeclarationListAccess* sdla)
{
    StructDeclarationSet* P = new StructDeclarationSet();
    P->struct_declaration_lists.push_back(sdla);
    return P;
}

StructDeclarationSet* create_struct_declaration_set(StructDeclarationSet* sds, StructDeclarationListAccess* sdla)
{
    sds->struct_declaration_lists.push_back(sdla);
    return sds;
}

// ##############################################################################
// ############################ STRUCT DECLARATION LIST ACCESS #########################
// ##############################################################################
StructDeclarationListAccess::StructDeclarationListAccess() : NonTerminal("STRUCT DECLARATION LIST ACCESS")
{
    access_specifier = nullptr;
    struct_declaration_list = nullptr;
}

StructDeclarationListAccess* create_struct_declaration_list_access(Terminal* access_specifier, StructDeclarationList* sdl)
{
    StructDeclarationListAccess* P = new StructDeclarationListAccess();
    if (access_specifier == nullptr)
        access_specifier = new Terminal("PUBLIC", "public", 0, 0);
    P->access_specifier = access_specifier;
    P->struct_declaration_list = sdl;
    return P;
}

// ##############################################################################
// ############################ STRUCT DECLARATION LIST #########################
// ##############################################################################

StructDeclarationList::StructDeclarationList() : NonTerminal("STRUCT DECLARATION LIST") {}

StructDeclarationList* create_struct_declaration_list(StructDeclaration* sd)
{
    StructDeclarationList* P = new StructDeclarationList();
    P->struct_declaration_list.push_back(sd);
    return P;
}

StructDeclarationList* create_struct_declaration_list(StructDeclarationList* sdl, StructDeclaration* sd)
{
    sdl->struct_declaration_list.push_back(sd);
    return sdl;
}

// ##############################################################################
// ############################ STRUCT DECLARATION ###############################
// ##############################################################################

StructDeclaration::StructDeclaration() : NonTerminal("STRUCT DECLARATION")
{
    specifier_qualifier_list = nullptr;
    struct_declarator_list = nullptr;
}

StructDeclaration* create_struct_declaration(SpecifierQualifierList* sql, StructDeclaratorList* sdl)
{
    StructDeclaration* P = new StructDeclaration();
    P->specifier_qualifier_list = sql;
    P->struct_declarator_list = sdl;
    if (sql) sql->set_type();
    return P;
}

// ##############################################################################
// ############################ STRUCT DECLARATOR LIST ###############################
// ##############################################################################
StructDeclaratorList::StructDeclaratorList() : NonTerminal("STRUCT DECLARATOR LIST") {}

StructDeclaratorList* create_struct_declarator_list(StructDeclarator* sd)
{
    StructDeclaratorList* P = new StructDeclaratorList();
    P->struct_declarator_list.push_back(sd);
    return P;
}

StructDeclaratorList* create_struct_declarator_list(StructDeclaratorList* sdl, StructDeclarator* sd)
{
    sdl->struct_declarator_list.push_back(sd);
    return sdl;
}

// ##############################################################################
// ############################ STRUCT DECLARATOR ###############################
// ##############################################################################

StructDeclarator::StructDeclarator() : NonTerminal("STRUCT DECLARATOR")
{
    declarator = nullptr;
    bit_field_width = -1;
}

StructDeclarator* create_struct_declarator(Declarator* d, Expression* e)
{
    StructDeclarator* P = new StructDeclarator();
    P->declarator = d;
    if (e != nullptr)
    {
        if (e->type.isInt() && e->type.is_const_literal)
        {
            PrimaryExpression* e_cast = dynamic_cast<PrimaryExpression*>(e);
            P->bit_field_width = stoi(e_cast->constant->value);
        }
        else
        {
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

Enumerator* create_enumerator(Identifier* id, Expression* e)
{
    Enumerator* P = new Enumerator();
    P->identifier = id;
    ConditionalExpression* e_cast = dynamic_cast<ConditionalExpression*>(e);
    if (e_cast == nullptr || (e_cast->type.isInt() && e_cast->type.is_const_literal))
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

EnumeratorList* create_enumerator_list(Enumerator* e)
{
    EnumeratorList* P = new EnumeratorList();
    TACOperand* id = new_identifier(e->identifier->value); // TAC
    TACOperand* t1 = new_temp_var();
    if(e->initializer_expression == nullptr){
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), t1, new_constant("0"), new_empty_var(), 0); // TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), id, t1, new_empty_var(), 0); // TAC
        P->last_constant_value = "0"; // TAC
        P->code.push_back(i1); // TAC
        P->code.push_back(i2); // TAC
    }
    else{
        string value = e->initializer_expression->logical_or_expression->logical_and_expression->or_expression->xor_expression->and_expression->equality_expression->relational_expression->shift_expression->additive_expression->multiplicative_expression->cast_expression->unary_expression->postfix_expression->primary_expression->constant->value;
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), t1, new_constant(value), new_empty_var(), 0); // TAC
        TACInstruction* i0 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type("int"),new_constant(value), 0); // TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), id, t1, new_empty_var(), 0); // TAC
        P->last_constant_value = value; // TAC
        P->code.push_back(i1); // TAC
        P->code.push_back(i0); // TAC
        P->code.push_back(i2); // TAC
        backpatch(e->initializer_expression->next_list, i1->label); // TAC
    }
    P->enumerator_list.push_back(e);
    return P;
}

EnumeratorList* create_enumerator_list(EnumeratorList* el, Enumerator* e)
{
    el->enumerator_list.push_back(e);
    TACOperand* id = new_identifier(e->identifier->value); // TAC
    TACOperand* t1 = new_temp_var();
    if(e->initializer_expression == nullptr){
        el->last_constant_value = to_string(stoi(el->last_constant_value)+1); // TAC
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), t1, new_constant(el->last_constant_value), new_empty_var(), 0); // TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), id, t1, new_empty_var(), 0); // TAC
        el->code.push_back(i1); // TAC
        el->code.push_back(i2); // TAC
    }
    else{
        string value = e->initializer_expression->logical_or_expression->logical_and_expression->or_expression->xor_expression->and_expression->equality_expression->relational_expression->shift_expression->additive_expression->multiplicative_expression->cast_expression->unary_expression->postfix_expression->primary_expression->constant->value;
        TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_NOP), t1, new_constant(value), new_empty_var(), 0); // TAC
        TACInstruction* i0 = emit(TACOperator(TAC_OPERATOR_CAST), t1, new_type("int"),new_constant(value), 0); // TAC
        TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_NOP), id, t1, new_empty_var(), 0); // TAC
        el->last_constant_value = value; // TAC
        el->code.push_back(i1); // TAC
        el->code.push_back(i0); // TAC
        el->code.push_back(i2); // TAC
        backpatch(e->initializer_expression->next_list, i1->label); // TAC
    }
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

EnumSpecifier* create_enumerator_specifier(Identifier* id, EnumeratorList* el)
{
    EnumSpecifier* P = new EnumSpecifier();
    P->identifier = id;
    P->enumerators = el;
    if (el != nullptr)
    {
        Type type(PrimitiveTypes::INT_T, 0, true);
        for (Enumerator* e : el->enumerator_list)
            symbolTable.insert(e->identifier->value, type, 4, 0);
    }
    return P;
}

EnumSpecifier* create_enumerator_specifier(EnumeratorList* el)
{
    EnumSpecifier* P = new EnumSpecifier();
    P->enumerators = el;

    if (el != nullptr)
    {
        Type type(PrimitiveTypes::INT_T, 0, true);
        for (Enumerator* e : el->enumerator_list)
            symbolTable.insert(e->identifier->value, type, 4, 0);
    }
    return P;
}

// ##############################################################################
// ################################## INIT DECLARATOR ############################
// ##############################################################################
InitDeclarator::InitDeclarator() : NonTerminal("INIT DECLARATOR")
{
    declarator = nullptr;
    initializer = nullptr;
}

InitDeclarator* create_init_declarator(Declarator* d, Initializer* i)
{
    InitDeclarator* P = new InitDeclarator();
    P->declarator = d;
    P->initializer = i;
    return P;
}

// ##############################################################################
// ################################## TYPE SPECIFIER ############################
// ##############################################################################

TypeSpecifier::TypeSpecifier() : NonTerminal("TYPE SPECIFIER")
{
    primitive_type_specifier = nullptr;
    enum_specifier = nullptr;
    struct_union_specifier = nullptr;
    class_specifier = nullptr;
    type_name = "";
}

TypeSpecifier* create_type_specifier(Terminal* t)
{
    TypeSpecifier* P = new TypeSpecifier();
    P->primitive_type_specifier = t;
    P->name += ": " + t->name; // for debugging purposes
    return P;
}

TypeSpecifier* create_type_specifier(Terminal* t, bool is_type_name)
{
    TypeSpecifier* P = new TypeSpecifier();
    P->name += ": " + t->name; // for debugging purposes
    P->type_name = t->value;
    return P;
}

TypeSpecifier* create_type_specifier(EnumSpecifier* es)
{
    TypeSpecifier* P = new TypeSpecifier();
    P->enum_specifier = es;
    P->name += ": ENUM";
    return P;
}

TypeSpecifier* create_type_specifier(StructUnionSpecifier* sus)
{
    TypeSpecifier* P = new TypeSpecifier();
    DefinedTypes* dt = symbolTable.get_defined_type(sus->identifier->value);
    if (dt == nullptr) {
        string error_msg = "Undefined type " + sus->identifier->value + " at line " + to_string(sus->identifier->line_no) + ", column " + to_string(sus->identifier->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    P->struct_union_specifier = sus;
    P->name += ": STRUCT/UNION";
    // if(sus.is_struct)
    //     type_specifier_obj->name += ": Struct";
    // else
    // type_specifier_obj->name += ": Union";
    return P;
}

TypeSpecifier* create_type_specifier(ClassSpecifier* cs)
{
    TypeSpecifier* P = new TypeSpecifier();
    P->class_specifier = cs;
    P->name += ": CLASS";
    return P;
}

// ##############################################################################
// ################################## SPECIFIER QUALIFIER LIST ######################################
// ##############################################################################

SpecifierQualifierList::SpecifierQualifierList() : NonTerminal("SPECIFIER QUALIFIER LIST")
{
    is_type_name = false;
    is_const_variable = false;
    type_index = PrimitiveTypes::TYPE_ERROR_T;
}

void SpecifierQualifierList::set_type()
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
    int isClass = 0;
    int isEnum = 0;
    int isUnionOrStruct = 0;
    int isTypeName = 0;

    for (int i = 0; i < type_qualifiers.size(); i++)
    {
        if (type_qualifiers[i] == CONST)
            is_const_variable = true;
        else if (type_qualifiers[i] == VOLATILE)
            ; // add something later}
    }
    for (int i = 0; i < type_specifiers.size(); i++)
    {
        if (type_specifiers[i]->primitive_type_specifier != nullptr)
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
        }
        else if (type_specifiers[i]->enum_specifier != nullptr)
        {
            isEnum++;
        }
        else if (type_specifiers[i]->struct_union_specifier != nullptr)
        {
            isUnionOrStruct++;
        }
        else if (type_specifiers[i]->class_specifier != nullptr)
        {
            isClass++;
        }
        else if (type_specifiers[i]->type_name != "")
        {
            isTypeName++;
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
        else if (isVoid)
        {
            type_index = PrimitiveTypes::VOID_T;
        }
        else if (isEnum)
        {
            type_index = PrimitiveTypes::INT_T;
        }
        else if (isUnionOrStruct)
        {
            string name = type_specifiers[0]->struct_union_specifier->identifier->value;
            DefinedTypes* dt = symbolTable.get_defined_type(name);
            type_index = PrimitiveTypes::TYPE_ERROR_T;
            if (dt) type_index = dt->type_index;
        }
        else if (isClass)
        {
            string name = type_specifiers[0]->class_specifier->identifier->value;
            DefinedTypes* dt = symbolTable.get_defined_type(name);
            type_index = PrimitiveTypes::TYPE_ERROR_T; 
            if(dt != nullptr) type_index = dt->type_index;
        }
        else if (isTypeName)
        {
            Symbol* sym = symbolTable.getTypedef(type_specifiers[0]->type_name);
            if(sym == nullptr){
                string error_msg = "Type name " + type_specifiers[0]->type_name + " not found";
                yyerror(error_msg.c_str());
                symbolTable.set_error();
                return;
            }
            type_index = sym->type.type_index;
            is_type_name = true;
        }
    }
}

SpecifierQualifierList* create_specifier_qualifier_list(TypeSpecifier* t)
{
    SpecifierQualifierList* P = new SpecifierQualifierList();
    P->type_specifiers.push_back(t);
    return P;
}

SpecifierQualifierList* create_specifier_qualifier_list(SpecifierQualifierList* s, TypeSpecifier* t)
{
    s->type_specifiers.push_back(t);
    // s->set_type();
    // if (s->type_index == -1)
    // {
    //     string error_msg = "Invalid Type at line " + to_string(s->type_specifiers[0]->line_no) + ", column " + to_string(s->type_specifiers[0]->column_no);
    //     yyerror(error_msg.c_str());
    //     symbolTable.set_error();
    // }
    return s;
}

SpecifierQualifierList* create_specifier_qualifier_list(SpecifierQualifierList* s, int tq)
{
    s->type_qualifiers.push_back(tq);
    s->set_type();
    return s;
}

// ##############################################################################
// ################################## TYPE NAME ######################################
// ##############################################################################

TypeName::TypeName() : NonTerminal("TYPE NAME")
{
    specifier_qualifier_list = nullptr;
    abstract_declarator = nullptr;
    type = Type();
}

TypeName* create_type_name(SpecifierQualifierList* sql, AbstractDeclarator* ad)
{
    TypeName* P = new TypeName();
    P->specifier_qualifier_list = sql;
    P->abstract_declarator = ad;
    if (sql) sql->set_type();
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
            DirectAbstractDeclarator* dad = ad->direct_abstract_declarator;
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
                P->type.ptr_level+=dad->array_dimensions.size();
                P->type.array_dim = dad->array_dimensions.size();
                P->type.array_dims.insert(P->type.array_dims.begin(), dad->array_dimensions.begin(), dad->array_dimensions.end());
            }
            else if (dad->is_function)
            {
                P->type.is_function = true;
                vector<Type> arg_types;
                vector<ParameterDeclaration*> parameters = dad->parameters->paramater_list->parameter_declarations;
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
// ################################## INITIALIZER ######################################
// ##############################################################################
Initializer::Initializer() : NonTerminal("INITIALIZER")
{
    assignment_expression = nullptr;
}

Initializer* create_initializer(Expression* e)
{
    Initializer* P = new Initializer();
    AssignmentExpression* e_cast = dynamic_cast<AssignmentExpression*>(e);
    P->assignment_expression = e_cast;
    return P;
}

// ##############################################################################
// ################################## TRANSLATION UNIT ######################################
// ##############################################################################
TranslationUnit::TranslationUnit() : NonTerminal("TRANSLATION UNIT") {}

TranslationUnit* create_translation_unit(ExternalDeclaration* ed)
{
    TranslationUnit* P = new TranslationUnit();
    P->external_declarations.push_back(ed);
    return P;
}

TranslationUnit* create_translation_unit(TranslationUnit* tu, ExternalDeclaration* ed)
{
    tu->external_declarations.push_back(ed);
    return tu;
}

// ##############################################################################
// ################################## EXTERNAL DECLARATION ######################################
// ##############################################################################
ExternalDeclaration::ExternalDeclaration() : NonTerminal("EXTERNAL DECLARATION")
{
    function_definition = nullptr;
    declaration = nullptr;
}

ExternalDeclaration* create_external_declaration(FunctionDefinition* fd)
{
    ExternalDeclaration* P = new ExternalDeclaration();
    P->function_definition = fd;
    TAC_CODE.insert(TAC_CODE.end(), fd->code.begin(), fd->code.end()); // TAC
    return P;
}

ExternalDeclaration* create_external_declaration(Declaration* d)
{
    ExternalDeclaration* P = new ExternalDeclaration();
    P->declaration = d;
    if(d->init_declarator_list != nullptr){
        for (auto id : d->init_declarator_list->init_declarator_list) {
            TAC_CODE.insert(TAC_CODE.end(), id->code.begin(), id->code.end()); //TAC
        }
    }
    else{
        if(d->declaration_specifiers->type_specifiers[0]->enum_specifier != nullptr){
            auto el = d->declaration_specifiers->type_specifiers[0]->enum_specifier->enumerators;
            TAC_CODE.insert(TAC_CODE.end(), el->code.begin(), el->code.end()); //TAC
        }
    }
    return P;
}

// ##############################################################################
// ################################## FUNCTION DEFINITION ######################################
// ##############################################################################
FunctionDefinition::FunctionDefinition() : NonTerminal("FUNCTION DEFINITION")
{
    declaration_specifiers = nullptr;
    declarator = nullptr;
    compound_statement = nullptr;
}

FunctionDefinition* create_function_definition(DeclarationSpecifiers* ds, Declarator* d)
{
    FunctionDefinition* P = new FunctionDefinition();
    P->declaration_specifiers = ds;
    P->declarator = d;
    if (d->direct_declarator->is_function)
    {
        string function_name = d->direct_declarator->identifier->value;
        int pointer_level = 0;
        if (d->pointer != nullptr)
            pointer_level = d->pointer->pointer_level;
        Type type = Type(ds->type_index, pointer_level, ds->is_const_variable);
        vector<Type> arg_types;
        if (d->direct_declarator->parameters != nullptr)
        {
            vector<ParameterDeclaration*> parameters = d->direct_declarator->parameters->paramater_list->parameter_declarations;
            for (int i = 0; i < parameters.size(); i++)
            {
                arg_types.push_back(parameters[i]->type);
            }
        }
        if (d != nullptr && d->direct_declarator != nullptr && d->direct_declarator->parameters != nullptr && d->direct_declarator->parameters->is_variadic) {
            if (arg_types.size() > 0) {
                arg_types[arg_types.size() - 1].is_variadic = true;
            }
            // arg_types[arg_types.size()-1].is_variadic = false;
        }
        type.is_function = true;
        type.arg_types = arg_types;
        type.num_args = arg_types.size();
        Symbol* sym = symbolTable.getFunction(function_name, arg_types);
        if (sym != nullptr)
        {
            if (sym->function_definition == nullptr)
            {
                symbolTable.add_function_definition(sym,P);
                symbolTable.enterScope(type, function_name);
                if (d->direct_declarator->parameters != nullptr)
                {
                    for (int i = 0; i < d->direct_declarator->parameters->paramater_list->parameter_declarations.size(); i++)
                    {
                        ParameterDeclaration* pd = d->direct_declarator->parameters->paramater_list->parameter_declarations[i];
                        symbolTable.insert(pd->declarator->direct_declarator->identifier->value, pd->type, pd->type.get_size(), 0);
                    }
                }
            }
            else if (sym->type.arg_types == arg_types)
            {
                string error_msg = "Function " + function_name + " redefined at line " + to_string(d->direct_declarator->identifier->line_no) + ", column " + to_string(d->direct_declarator->identifier->column_no);
                yyerror(error_msg.c_str());
                symbolTable.set_error();
                return P;
            }
        }
        else
        {
            symbolTable.insert(function_name, type, type.get_size(), 1);
            Symbol* sym = symbolTable.getSymbol(function_name);
            symbolTable.add_function_definition(sym,P);
            symbolTable.enterScope(type, function_name);
            if (d->direct_declarator->parameters != nullptr)
            {
                for (int i = 0; i < d->direct_declarator->parameters->paramater_list->parameter_declarations.size(); i++)
                {
                    ParameterDeclaration* pd = d->direct_declarator->parameters->paramater_list->parameter_declarations[i];
                    symbolTable.insert(pd->declarator->direct_declarator->identifier->value, pd->type, pd->type.get_size(), 0);
                }
            }
        }
    }
    else
    {
        string error_msg = "Function definition must have a function declarator at line " + to_string(d->direct_declarator->identifier->line_no) + ", column " + to_string(d->direct_declarator->identifier->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return P;
    }
    return P;
}

FunctionDefinition* create_function_definition(Declarator* declarator, FunctionDefinition* fd, Statement* cs)
{
    CompoundStatement* cs_cast = dynamic_cast<CompoundStatement*>(cs);
    fd->compound_statement = cs_cast;
    Symbol* function = symbolTable.getSymbol(declarator->direct_declarator->identifier->value);
    Type t1 = Type(function->type.type_index,function->type.ptr_level,function->type.is_const_variable);
    Type t2 = Type(PrimitiveTypes::VOID_T, 0, false);
    if(!cs->return_type.empty()) t2 = cs->return_type[0];
    if(cs_cast->declaration_statement_list != nullptr) fd->code.insert(fd->code.end(), cs_cast->declaration_statement_list->static_declaration_code.begin(), cs_cast->declaration_statement_list->static_declaration_code.end()); // TAC
    TACInstruction* i1 = emit(TACOperator(TAC_OPERATOR_FUNC_BEGIN), new_identifier(declarator->direct_declarator->identifier->value), new_empty_var(), new_empty_var(), 0); // TAC
    fd->code.push_back(i1); // TAC
    if(declarator->direct_declarator->parameters != nullptr) {
         for(auto pd:declarator->direct_declarator->parameters->paramater_list->parameter_declarations) {
             if(pd->declarator != nullptr && pd->declarator->direct_declarator != nullptr) {
                 TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_PARAM), new_identifier(pd->declarator->direct_declarator->identifier->value), new_empty_var(), new_empty_var(), 0); // TAC
                 fd->code.push_back(i2); // TAC
             }
         }
    }
    
    TACInstruction* i2 = emit(TACOperator(TAC_OPERATOR_FUNC_END), new_identifier(declarator->direct_declarator->identifier->value), new_empty_var(), new_empty_var(), 0); // TAC
    backpatch(cs_cast->next_list, i2->label); // TAC
    backpatch(cs_cast->break_list, i2->label); // TAC
    fd->code.insert(fd->code.end(), cs_cast->code.begin(), cs_cast->code.end()); // TAC
    fd->code.push_back(i2); // TAC

    if (!(t1.is_convertible_to(t2))) {
        string error_msg = "Function is returning incorrect data type " + to_string(declarator->direct_declarator->identifier->line_no) + ", column " + to_string(declarator->direct_declarator->identifier->column_no);
        yyerror(error_msg.c_str());
        symbolTable.set_error();
        return fd;
    }
    return fd;
}


// ##############################################################################
// ################################## IDENTIFIER ######################################
// ##############################################################################

Identifier::Identifier(string value, unsigned int line_no, unsigned int column_no)
    : Terminal("IDENTIFIER", value, line_no, column_no)
{
}

// ##############################################################################
// ################################## CONSTANT ######################################
// ##############################################################################

Constant::Constant(string name, string value, unsigned int line_no, unsigned int column_no)
    : Terminal(name, value, line_no, column_no)
{
    this->constant_type = this->set_constant_type(value);
    this->value = this->convert_to_decimal();
}

Type Constant::set_constant_type(string value)
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
            t.type_index = PrimitiveTypes::U_LONG_LONG_T;
        }
        else if ((isLong == 2) && !isUnsigned)
        {
            t.type_index = PrimitiveTypes::LONG_LONG_T;
        }
        else if ((isLong == 1) && isUnsigned)
        {
            t.type_index = PrimitiveTypes::U_LONG_T;
        }
        else if ((isLong == 1) && !isUnsigned)
        {
            t.type_index = PrimitiveTypes::LONG_T;
        }
        else if (!isLong && isUnsigned)
        {
            t.type_index = PrimitiveTypes::U_SHORT_T;
        }
        else if (!isLong && !isUnsigned)
        {
            t.type_index = PrimitiveTypes::SHORT_T;
        }
    }
    else if (name == "CHAR_CONSTANT")
    {
        t.type_index = PrimitiveTypes::CHAR_T;
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
            t.type_index = PrimitiveTypes::FLOAT_T;
        }
        else if (isDouble == 1)
        {
            t.type_index = PrimitiveTypes::LONG_DOUBLE_T;
        }
        else
        {
            t.type_index = PrimitiveTypes::DOUBLE_T;
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
string Constant::convert_to_decimal()
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

StringLiteral::StringLiteral(string value, unsigned int line_no, unsigned int column_no)
    : Terminal("CONSTANT", value, line_no, column_no)
{
}

// ##############################################################################
// ################################## SYMBOL TABLE ######################################
// ##############################################################################

SymbolTable::SymbolTable()
{
    currentScope = 0;
    error = false;
}

void SymbolTable::enterScope(Type type, string name)
{
    currentScope++;
    if (type.is_function)
    {
        scope_stack.push({ currentScope, {type, name} });
    }
    else if (type.is_defined_type)
    {
        scope_stack.push({ currentScope, {type, name} });
    }
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
                // print();
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

    for (auto it = table.begin(); it != table.end();)
    {
        for (auto symIt = it->second.begin(); symIt != it->second.end();)
        {
            if ((*symIt)->scope == currentScope)
            {
                // print();
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

    // for (auto it = defined_types.begin(); it != defined_types.end();)
    // {
    //     for (auto symIt = it->second.begin(); symIt != it->second.end();)
    //     {
    //         if (symIt->first == currentScope)
    //         {
    //             // delete symIt->second;  // Free the allocated memory
    //             symIt = it->second.erase(symIt);
    //         }
    //         else
    //         {
    //             ++symIt;
    //         }
    //     }
    //     if (it->second.empty())
    //         it = defined_types.erase(it);
    //     else
    //         ++it;
    // }

    for (auto it = typedefs.begin(); it != typedefs.end();)
    {
        for (auto symIt = it->second.begin(); symIt != it->second.end();)
        {
            if ((*symIt)->scope == currentScope)
            {
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

    auto top = scope_stack.top();
    if (top.first == currentScope)
        scope_stack.pop();
    currentScope--;
}

void SymbolTable::insert(string name, Type type, int size, int overloaded)
{
    pair<int, pair<Type, string>> top = { 0, {Type(), name} };
    if (!scope_stack.empty())
    {
        top = scope_stack.top();
    }
    for (const Symbol* sym : table[name])
    {
        if (sym->scope == currentScope)
        {
            if (overloaded == 1)
            {
                if (sym->type.arg_types != type.arg_types)
                if (sym->type.is_function && sym->type.arg_types != type.arg_types)
                {
                    continue;
                }
            }
            else
            {
                string error_msg = "Symbol '" + name + "' already declared in this scope.";
                yyerror(error_msg.c_str());
                set_error();
                return;
            }
        }
    }
    Symbol* sym = new Symbol(name, type, currentScope, this->currAddress);
    table[name].push_front(sym);
    if (top.second.first.is_function)
    {
        FunctionDefinition* func = getFunction(top.second.second, top.second.first.arg_types)->function_definition;
        if (func == nullptr) {
            string error_msg = "Function " + top.second.second + " not defined";
            yyerror(error_msg.c_str());
            set_error();
            return;
        }
        Symbol* sym_f = new Symbol(name, type, currentScope, func->function_symbol_table.currAddress);
        func->function_symbol_table.table[sym->name].push_front(sym_f);
        func->function_symbol_table.currAddress += size;
    }
    else if (top.second.first.is_defined_type)
    {
        DefinedTypes* dt = get_defined_type(top.second.second);
        if (dt == nullptr || dt->type_definition == nullptr) {
            string error_msg = "Undefined type " + top.second.second;
            yyerror(error_msg.c_str());
            set_error();
            return;
        }
        Symbol* sym_c = new Symbol(name, type, currentScope - 1, dt->type_definition->type_symbol_table.currAddress);
        dt->type_definition->type_symbol_table.table[sym->name].push_front(sym_c);
        if (dt->type_category != TYPE_CATEGORY_UNION) {
            dt->type_definition->type_symbol_table.currAddress += size;
        }
    }
    else {
        this->currAddress += size;
    }
}

void SymbolTable :: add_function_definition(Symbol* sym, FunctionDefinition* fd){
    sym->function_definition = fd;
    pair<int, pair<Type, string>> top = { 0, {Type(), ""} };
    if (!scope_stack.empty())
    {
        top = scope_stack.top();
    }
    if (top.second.first.is_function)
    {
        FunctionDefinition* func = getFunction(top.second.second, top.second.first.arg_types)->function_definition;
        if (func == nullptr) {
            string error_msg = "Function " + top.second.second + " not defined";
            yyerror(error_msg.c_str());
            set_error();
            return;
        }
        Symbol* sym_f = func->function_symbol_table.getSymbol(sym->name);
        sym_f->function_definition = fd;
    }
    else if (top.second.first.is_defined_type)
    {
        DefinedTypes* dt = get_defined_type(top.second.second);
        if (dt == nullptr || dt->type_definition == nullptr) {
            string error_msg = "Undefined type " + top.second.second;
            yyerror(error_msg.c_str());
            set_error();
            return;
        }
        Symbol* sym_c = dt->type_definition->type_symbol_table.getSymbol(sym->name);
        sym_c->function_definition = fd;   
    }
}

void SymbolTable::insert_defined_type(std::string name, DefinedTypes* type)
{
    pair<int, pair<Type, string>> top = { 0, {Type(), name} };
    if (!scope_stack.empty())
    {
        top = scope_stack.top();
    }
    for (auto defined_type : defined_types[name])
    {
        if (defined_type.first == currentScope)
        {
            // delete type;
            string error_msg = " '" + name + "' already declared in this scope.";
            yyerror(error_msg.c_str());
            set_error();
            return;
        }
    }
    defined_types[name].push_front({ currentScope, type });
    if (top.second.first.is_function)
    {
        FunctionDefinition* func = getFunction(top.second.second, top.second.first.arg_types)->function_definition;
        func->function_symbol_table.defined_types[name].push_front({ currentScope, type });
    }
    else if (top.second.first.is_defined_type)
    {
        DefinedTypes* dt = get_defined_type(top.second.second);
        if (dt == nullptr) {
            string error_msg = "Undefined type " + top.second.second;
            yyerror(error_msg.c_str());
            set_error();
            return;
        }
        dt->type_definition->type_symbol_table.defined_types[name].push_front({ currentScope - 1, type });
    }
}

void SymbolTable::insert_typedef(std::string name, Type type, int offset)
{
    pair<int, pair<Type, string>>top = { 0, {Type(), name} };
    if (!scope_stack.empty())
    {
        top = scope_stack.top();
    }
    for (const Symbol* sym : typedefs[name])
    {
        if (sym->scope == currentScope)
        {
            string error_msg = "Symbol '" + name + "' already declared in this scope.";
            yyerror(error_msg.c_str());
            set_error();
            return;
        }
    }
    Symbol* sym = new Symbol(name, type, currentScope, this->currAddress);
    this->currAddress += offset;
    typedefs[name].push_front(sym);
    if (top.second.first.is_function)
    {
        FunctionDefinition* func = getFunction(top.second.second, top.second.first.arg_types)->function_definition;
        func->function_symbol_table.typedefs[name].push_front(sym);
    }
    else if (top.second.first.is_defined_type)
    {
        DefinedTypes* dt = get_defined_type(top.second.second);
        Symbol* sym_c = new Symbol(name, type, currentScope - 1, this->currAddress);
        if (dt == nullptr) {
            string error_msg = "Undefined type " + top.second.second;
            yyerror(error_msg.c_str());
            set_error();
            return;
        }
        dt->type_definition->type_symbol_table.typedefs[name].push_front(sym_c);
    }
}

bool SymbolTable::lookup(string name)
{
    auto it = table.find(name);
    if (it == table.end())
        return false;

    for (const Symbol* sym : it->second)
    {
        if (sym->scope <= currentScope)
            return true;
    }
    return false;
}

bool SymbolTable::lookup_symbol_using_mangled_name(std::string name)
{
    // Add logic
}

bool SymbolTable::lookup_function(std::string name, vector<Type> arg_types)
{
    auto it = table.find(name);
    if (it == table.end())
        return false;

    for (const Symbol* sym : it->second)
    {
        if (sym->scope <= currentScope && arg_types.size() == sym->type.arg_types.size()) {
            for (int i = 0; i < arg_types.size(); i++) {
                if (arg_types[i].is_convertible_to(sym->type.arg_types[i]) == false) {
                    return false;
                }
            }
            return true;
        }
        else if (arg_types.size() > 0 && sym->type.arg_types.size() > 0 && sym->type.arg_types[sym->type.arg_types.size() - 1].is_variadic && arg_types.size() >= sym->type.arg_types.size()) {
            for (int i = 0;i < sym->type.arg_types.size();i++) {
                if (arg_types[i].is_convertible_to(sym->type.arg_types[i]) == false) {
                    return false;
                }
            }
            // for (int i = sym->type.arg_types.size();i < arg_types.size();i++) {
            //     if (arg_types[i].is_convertible_to(sym->type.arg_types[sym->type.arg_types.size() - 1]) == false) {
            //         return false;
            //     }
            // }
            return true;
        }
    }
    return false;
}

bool SymbolTable::lookup_defined_type(string name)
{
    auto it = defined_types.find(name);
    if (it == defined_types.end())
        return false;

    for (pair<int, DefinedTypes*> p : it->second)
    {
        if (p.first <= currentScope)
            return true;
    }
    return false;
}

bool SymbolTable::lookup_typedef(string name)
{
    auto it = typedefs.find(name);
    if (it == typedefs.end())
        return false;

    for (const Symbol* sym : it->second)
    {
        if (sym->scope <= currentScope)
            return true;
    }
    return false;
}

bool SymbolTable::check_member_variable(string name, string member)
{
    auto t = get_defined_type(name);
    if (t == nullptr) {
        string error_msg = "Undefined type " + name;
        yyerror(error_msg.c_str());
        set_error();
        return false;
    }
    if (t->type_definition == nullptr) {
        string error_msg = "Undefined type " + name;
        yyerror(error_msg.c_str());
        set_error();
        return false;
    }

    bool isMember = t->type_definition->lookup_member(member);
    if (!isMember) {
        string error_msg = "Member variable '" + member + "' not found in class '" + name + "'";
        yyerror(error_msg.c_str());
        set_error();
        return false;
    }

    AccessSpecifiers access_specifier = t->type_definition->get_member_access_specifier(member);
    if (access_specifier == ACCESS_SPECIFIER_PRIVATE)
    {
        string error_msg = "Member variable '" + member + "' is private in class '" + name;
        yyerror(error_msg.c_str());
        set_error();
        return false;
    }
    if(access_specifier == ACCESS_SPECIFIER_PROTECTED)
    {
        string error_msg = "Member variable '" + member + "' is protected in class '" + name;
        yyerror(error_msg.c_str());
        set_error();
        return false;
    }

    return true;
}

Type SymbolTable::get_type_of_member_variable(string name, string member)
{
    auto dt = get_defined_type(name);
    if (dt == nullptr) {
        string error_msg = "Undefined type " + name;
        yyerror(error_msg.c_str());
        set_error();
        return ERROR_TYPE;
    }

    Symbol* sym = dt->type_definition->type_symbol_table.getSymbol(member);
    if(sym == nullptr) {
        string error_msg = "Member variable '" + member + "' not found in class '" + name + "'";
        yyerror(error_msg.c_str());
        set_error();
        return ERROR_TYPE;
    }
    return sym->type;
}

Type SymbolTable::get_type_of_member_variable(string name, string member, vector<Type> arg_types)
{
    auto dt = get_defined_type(name);
    if (dt == nullptr) {
        string error_msg = "Undefined type " + name;
        yyerror(error_msg.c_str());
        set_error();
        return ERROR_TYPE;
    }
    Symbol* sym = dt->type_definition->type_symbol_table.getFunction(member, arg_types);
    return sym->type;
}

Symbol* SymbolTable:: get_symbol_using_mangled_name(std::string name){
    // Add logic
}

Symbol* SymbolTable::getSymbol(string name)
{
    auto it = table.find(name);
    if (it == table.end() || it->second.empty())
        return nullptr;

    Symbol* sym = nullptr;

    for (Symbol* _sym : it->second)
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

Symbol* SymbolTable::getFunction(std::string name, vector<Type> arg_types)
{
    auto it = table.find(name);
    if (it == table.end() || it->second.empty())
        return nullptr;

    Symbol* sym = nullptr;

    for (Symbol* _sym : it->second)
    {
        if (_sym->scope <= currentScope && (int)_sym->type.arg_types.size() == (int)arg_types.size())
        {
            for (int i = 0; i < arg_types.size(); i++)
            {
                if (arg_types[i].is_convertible_to(_sym->type.arg_types[i]) == false)
                {
                    return nullptr;
                }
            }
            if (sym == nullptr || _sym->scope > sym->scope)
            {
                sym = _sym;
            }
        }
        else if (arg_types.size() > 0 && _sym->type.arg_types.size() > 0 && arg_types[arg_types.size() - 1].is_variadic && arg_types.size() >= _sym->type.arg_types.size()) {

            for (int i = 0;i < _sym->type.arg_types.size();i++) {
                if (arg_types[i].is_convertible_to(_sym->type.arg_types[i]) == false)
                {
                    return nullptr;
                }
            }
            for (int i = _sym->type.arg_types.size();i < arg_types.size();i++) {
                if (arg_types[i].is_convertible_to(_sym->type.arg_types[_sym->type.arg_types.size() - 1]) == false) {
                    return nullptr;
                }
            }
            if (sym == nullptr || _sym->scope > sym->scope)
            {
                sym = _sym;
            }
        }
    }

    return sym;
}

DefinedTypes* SymbolTable::get_defined_type(std::string name)
{
    auto it = defined_types[name];
    DefinedTypes* types = nullptr;
    for (pair<int, DefinedTypes*> p : it)
    {
        if (p.first <= currentScope)
        {
            types = p.second;
            break;
        }
    }
    return types;
}

Symbol* SymbolTable::getTypedef(std::string name)
{
    auto it = typedefs.find(name);
    Symbol* sym = nullptr;

    for (Symbol* _sym : it->second)
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
    Symbol* sym = getSymbol(name);
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
    if (table.empty()) {
        cout << "The Symbol Table is empty!\n";
        return;
    }

    cout << "\nSYMBOL TABLE:\n";
    cout << "----------------------------------------------------------------------------\n";
    cout << "| " << setw(20) << left << "Name"
        << "| " << setw(26) << left << "Type"
        << "| " << setw(8) << left << "Scope"
        << "| " << setw(12) << left << "Offset" << " |\n";
    cout << "----------------------------------------------------------------------------\n";

    for (const auto& entry : table)
    {
        for (const auto symbol : entry.second)
        {
            cout << "| " << setw(20) << left << symbol->name
                << "| " << setw(26) << left << symbol->type.type_index
                // Aren ~ maine add kiya hai .type_index (isko change krna hai according to Type class)
                << "| " << setw(8) << left << symbol->scope
                << "| " << setw(12) << left << symbol->offset << " |\n";
        }
    }

    cout << "----------------------------------------------------------------------------\n";
}

void SymbolTable::print_typedefs()
{
    if (typedefs.empty()) {
        cout << "The Typedef Table is empty.\n";
        return;
    }

    cout << "\nTYPEDEFS:\n";
    cout << "----------------------------------------------------------------------------\n";
    cout << "| " << setw(20) << left << "Name"
        << "| " << setw(26) << left << "Type"
        << "| " << setw(8) << left << "Scope"
        << "| " << setw(12) << left << "Offset" << " |\n";
    cout << "----------------------------------------------------------------------------\n";

    for (const auto& entry : typedefs)
    {
        for (const auto symbol : entry.second)
        {
            cout << "| " << setw(20) << left << symbol->name
                << "| " << setw(26) << left << symbol->type.type_index
                << "| " << setw(8) << left << symbol->scope
                << "| " << setw(12) << left << symbol->offset << " |\n";
        }
    }

    cout << "----------------------------------------------------------------------------\n";
}

void SymbolTable::print_defined_types()
{
    if (defined_types.empty()) {
        cout << "The Defined Types Table is empty.\n";
        return;
    }

    cout << "\nDEFINED TYPES:\n";
    cout << "----------------------------------------------------------------------------\n";
    cout << "| " << setw(20) << left << "Name"
        << "| " << setw(12) << left << "Type Index"
        << "| " << setw(26) << left << "Type Category"
        << "| " << setw(8) << left << "Scope" << " |\n";
    cout << "----------------------------------------------------------------------------\n";

    for (const auto& entry : defined_types)
    {
        for (const auto& symbol : entry.second)
        {
            cout << "| " << setw(20) << left << entry.first
                << "| " << setw(12) << left
                << (symbol.second ? to_string(symbol.second->type_index) : "In")
                << "| " << setw(26) << left
                << (symbol.second ? type_category_name[symbol.second->type_category] : "Unknown")
                << "| " << setw(8) << left << symbol.first << " |\n";
        }
    }

    cout << "----------------------------------------------------------------------------\n";
}