#include "symbol_table.h"
#include <iostream>
#include <unordered_map>
#include <list>
#include <iomanip>
using namespace std;

extern void yyerror(const char *msg);

SymbolTable::SymbolTable() { currentScope = 0; }

void SymbolTable::enterScope()
{
	currentScope++;
	error = false;
}

void SymbolTable::set_error() { error = true; }

bool SymbolTable::has_error() { return error; }

void SymbolTable::exitScope()
{
	if (currentScope == 0)
		return;

	for (auto it = table.begin(); it != table.end();)
	{
		it->second.remove_if([this](Symbol& s)
		    { return s.scope == currentScope; });
		if (it->second.empty())
			it = table.erase(it);
		else
			++it;
	}
	currentScope--;
}

void SymbolTable::insert(string name, string type, int memoryAddr)
{
	if (lookup(name))
	{
		string error_msg = "Symbol '" + name + "' already declared in this scope.";
		yyerror(error_msg.c_str());
        set_error();
		return;
	}
	table[name].push_front(Symbol(name, type, currentScope, memoryAddr));
}

bool SymbolTable::lookup(std::string name)
{
	auto it = table.find(name);
	if (it == table.end())
		return false;

	for (const Symbol& sym : it->second)
	{
		if (sym.scope <= currentScope)
			return true;
	}
	return false;
}

Symbol* SymbolTable::getSymbol(std::string name)
{
	auto it = table.find(name);
	if (it == table.end() || it->second.empty())
		return nullptr;

	return &(it->second.front());
}

void SymbolTable::update(string name, string newType)
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
		for (const auto& symbol : entry.second)
		{
			cout << "| " << setw(20) << left << symbol.name
			     << "| " << setw(26) << left << symbol.type
			     << "| " << setw(8) << left << symbol.scope
			     << "| " << setw(12) << left << symbol.memoryAddr << " |\n";
		}
	}

	cout << "----------------------------------------------------------------------------\n";
}

SymbolTable symbolTable;