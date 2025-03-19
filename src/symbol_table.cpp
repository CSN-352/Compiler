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
using namespace std;

extern void yyerror(const char* msg);

// std::vector<Types> defined_types;
extern unsigned int line_no;

//##############################################################################
//################################## SYMBOLTABLE ######################################
//##############################################################################

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
	for (const Symbol* sym : table[name])
	{
		if (sym->scope == currentScope)
		{
			string error_msg = "Symbol '" + name + "' already declared in this scope.";
			yyerror(error_msg.c_str());
			set_error();
			return;
		}
	}

	Symbol* sym = new Symbol(name, type, currentScope, memoryAddr);
	table[name].push_front(sym);
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

	for (const auto& entry : table)
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

Identifier :: Identifier(string value, unsigned int line_no, unsigned int column_no) : Terminal("IDENTIFIER", value, line_no, column_no) {}

Constant :: Constant(string value, unsigned int line_no, unsigned int column_no){
    Terminal("CONSTANT", value, line_no, column_no);
    constant_type = set_constant_type(value);
} 

Type Constant :: set_constant_type(string value){
    
}

StringLiteral :: StringLiteral(string value, unsigned int line_no, unsigned int column_no) : Terminal("CONSTANT", value, line_no, column_no) {}

SymbolTable symbolTable;


//##############################################################################
//################################## TYPE ######################################
//##############################################################################

Type::Type() {
    typeIndex = -1;
    ptr_level = -1;
    is_const = false;

    is_pointer = false;
    is_array = false;
    array_dim = 0;

    is_function = false;
    num_args = 0;
}

Type::Type( int idx, int p_lvl, bool is_con ) {
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
bool Type::isPrimitive() {
    if ( typeIndex >= 0 && typeIndex <= VOID_T ) {
        return true;
    } else {
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

bool Type::isInt() {
    if ( typeIndex >= U_CHAR_T && typeIndex <= LONG_LONG_T ) {
        if ( ptr_level == 0 ) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool Type::isChar() {
    if ( typeIndex == U_CHAR_T || typeIndex == CHAR_T ) {
        if ( ptr_level == 0 ) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool Type::isFloat() {
    if ( typeIndex >= 10 && typeIndex <= 12 && ptr_level == 0 )
        return true;
    else
        return false;
}
bool Type::isIntorFloat() {
    if ( typeIndex <= 12 && ptr_level == 0 )
        return true;
    else
        return false;
}

bool Type::isUnsigned() {
    if ( typeIndex == 0 || typeIndex == 2 || typeIndex == 4 ||
         typeIndex == 6 || typeIndex == 8 ) {
        return true;
    } else {
        return false;
    }
}

bool Type::isPointer() {
    if ( ptr_level ) {
        return true;
    } else {
        return false;
    }
}

void Type::make_signed() {
    if ( typeIndex == 0 || typeIndex == 2 || typeIndex == 4 ||
        typeIndex == 6 || typeIndex == 8 ) {
        typeIndex += 1;
    }
}

void Type::make_unsigned() {
    if ( typeIndex == 1 || typeIndex == 3 || typeIndex == 5 ||
         typeIndex == 7 || typeIndex == 9) {
        typeIndex -= 1;
    }
}

bool Type::isVoid() {
    if ( typeIndex == VOID_T ) {
        if ( ptr_level == 0 || is_array ) {
            return true;
        }
    }
    return false;
}

bool Type::is_invalid() {

    if ( typeIndex == -1 ) {
        return true;
    }

    return false;
}

bool Type::is_ea() {
	
	if ( is_array ) {
		return true;
	} else if ( !isPrimitive() && ptr_level == 0 ) {
		return true;
	} else {
		return false;
	}

    return false;
}

bool operator!=( Type &obj1, Type &obj2 ) {
	return !(obj1 == obj2);
}
bool operator==( Type &obj1, Type &obj2 ) {

    if ( obj1.typeIndex != obj2.typeIndex ) {
        return false;
    } else if ( obj1.is_array == true && obj2.is_array == true ) {
        if ( obj1.array_dim != obj2.array_dim ) {
            return false;
        } else {
            for ( unsigned int i = 0; i < obj1.array_dim; i++ ) {
                if ( obj1.array_dims[i] == 0 || obj2.array_dims[i] == 0 ||
                     obj1.array_dims[i] == obj2.array_dims[i] ) {
                    continue;
                } else {
                    return false;
                }
            }

            return true;
        }
    } else if ( obj1.is_array != obj2.is_array ) {
        if ( obj1.ptr_level == 1 && obj2.ptr_level == 1 ) {
            return true;
        } else {
            return false;
        }
    } else if ( obj1.is_pointer == true && obj2.is_pointer == true ) {
        return obj1.ptr_level == obj2.ptr_level;

    } else if ( obj1.is_pointer != obj2.is_pointer ) {
        return false;
    } else if ( obj1.is_function == true && obj2.is_function == true ) {
        if ( obj1.num_args != obj2.num_args ) {
            return false;
        } else {
            for ( unsigned int i = 0; i < obj1.num_args; i++ ) {
                if ( obj1.arg_types[i] == obj2.arg_types[i] ) {
                    continue;
                } else {
                    return false;
                }
            }

            return true;
        }

    } else if ( obj1.is_function != obj2.is_function ) {
        return false;
    }

    else {
        return true;
    }

    return false;
}
