#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <list>
#include "ast.h"

class Expression;
class PrimaryExpression;
class Type;

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

class Type {
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
  
      Type( int idx, int p_lvl, bool is_con );
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
  
      friend bool operator==( Type &obj1, Type &obj2 );
      friend bool operator!=( Type &obj1, Type &obj2 );
};

extern Type ERROR_TYPE;
class Identifier : public Terminal{
    public:
        Identifier(string value, unsigned int line_no, unsigned int column_no);
};

class Constant : public Terminal{
    public:
        Type constant_type;
        Type set_constant_type(string value);
        Type get_constant_type() {return constant_type;}
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

    Symbol(std::string n, Type t, int s, int m) : name(n), type(t), scope(s), memoryAddr(m) {}
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
