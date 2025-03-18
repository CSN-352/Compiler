#include "symbol_table.h"
#include <iostream>
#include <unordered_map>
#include <list>
#include <iomanip>
using namespace std;

extern void yyerror(const char* msg);

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

void SymbolTable::insert(string name, string type, int memoryAddr)
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
		for (const auto symbol : entry.second)
		{
			cout << "| " << setw(20) << left << symbol->name
			     << "| " << setw(26) << left << symbol->type
			     << "| " << setw(8) << left << symbol->scope
			     << "| " << setw(12) << left << symbol->memoryAddr << " |\n";
		}
	}

	cout << "----------------------------------------------------------------------------\n";
}

SymbolTable symbolTable;