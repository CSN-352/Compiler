#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <list>
#include "ast.h"

class Symbol
{
public:
    std::string name;
    std::string type;
    int scope;
    int memoryAddr;

    Symbol(std::string n, std::string t, int s, int m) : name(n), type(t), scope(s), memoryAddr(m) {}
};

class SymbolTable
{
private:
    std::unordered_map<std::string, std::list<Symbol*>> table;
    int currentScope;
    bool error;

public:
    SymbolTable();
    void enterScope();
    void exitScope();
    void insert(std::string name, std::string type, int memoryAddr);
    bool lookup(std::string name);
    Symbol* getSymbol(std::string name);
    void update(std::string name, std::string newType);
    void remove(std::string name);
    void print();
    void set_error();
    bool has_error();
};

class Identifier : public Terminal{
    public:
        Identifier(string value, unsigned int line_no, unsigned int column_no);
};

class Constant : public Terminal{
    public:
        Type get_constant_type();
        string convert_to_decimal();
        Constant(string value, unsigned int line_no, unsigned int column_no);
};

class StringLiteral : public Terminal{
    public:
        StringLiteral(string value,unsigned int line_no, unsigned int column_no);
};

extern SymbolTable symbolTable;

#endif
