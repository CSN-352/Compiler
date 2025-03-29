#ifndef TAC_H
#define TAC_H

#include <unordered_map>
#include <string>
using namespace std;

static unsigned int instruction_id = 1;
static unsigned int temp_var_id = 1;
static unsigned int label_id = 1;

enum TACOperandType {
    TAC_OPERAND_TEMP_VAR,         // Temporary variables (e.g., t1, t2)
    TAC_OPERAND_IDENTIFIER,       // User-defined variables
    TAC_OPERAND_CONSTANT,         // Integer or float constants (e.g., 42, 3.14)
    TAC_OPERAND_LABEL,           // Jump targets (e.g., L1, L2)
    TAC_OPERAND_STRING_LITERAL,   // String literals (e.g., "Hello, World!")
    TAC_OPERAND_POINTER,          // Pointer dereferencing (e.g., *ptr)
    TAC_OPERAND_FUNCTION_CALL,    // Function calls (e.g., call func)
    TAC_OPERAND_ADDRESS           // Address-of operator (&var)
};

enum TACOperatorType {
    // Arithmetic Operators
    TAC_OPERATOR_ADD,        // +
    TAC_OPERATOR_SUB,        // -
    TAC_OPERATOR_MUL,        // *
    TAC_OPERATOR_DIV,        // /
    TAC_OPERATOR_MOD,        // %
    TAC_OPERATOR_UMINUS,     // Unary minus (-x)

    // Relational Operators
    TAC_OPERATOR_EQ,         // ==
    TAC_OPERATOR_NE,         // !=
    TAC_OPERATOR_GT,         // >
    TAC_OPERATOR_LT,         // <
    TAC_OPERATOR_GE,         // >=
    TAC_OPERATOR_LE,         // <=

    // Logical Operators
    TAC_OPERATOR_AND,        // &&
    TAC_OR,         // ||
    TAC_NOT,        // !

    // Bitwise Operators
    TAC_OPERATOR_BIT_AND,    // &
    TAC_OPERATOR_BIT_OR,     // |
    TAC_OPERATOR_BIT_XOR,    // ^
    TAC_OPERATOR_LEFT_SHIFT, // <<
    TAC_OPERATOR_RIGHT_SHIFT,// >>
    TAC_OPERATOR_BIT_NOT,    // ~

    // Assignment Operators
    TAC_OPERATOR_ASSIGN,     // =

    // Pointer and Memory Operators
    TAC_OPERATOR_ADDR_OF,    // & (Address-of)
    TAC_OPERATOR_DEREF,      // * (Dereference)

    // Control Flow (Branching & Jumps)
    TAC_OPERATOR_GOTO,       // goto label
    TAC_OPERATOR_IF,         // if (condition) goto label
    TAC_OPERATOR_LABEL,      // Label definition

    // Function and Procedure Handling
    TAC_OPERATOR_CALL,       // Function call
    TAC_OPERATOR_RETURN,     // return value
    TAC_OPERATOR_PARAM,      // Function parameter passing
    TAC_OPERATOR_FUNC_BEGIN, // Function prologue
    TAC_OPERATOR_FUNC_END,   // Function epilogue

    // Array and Indexing Operators
    TAC_OPERATOR_INDEX,        // Array access: T = a[i]
    TAC_OPERATOR_INDEX_ASSIGN, // Array assignment: a[i] = T

    // Miscellaneous
    TAC_OPERATOR_NOP         // No operation
};

static std::unordered_map<TACOperator, std::string> opMap = {
    {TAC_OPERATOR_ADD, "+"}, {TAC_OPERATOR_SUB, "-"}, {TAC_OPERATOR_MUL, "*"}, {TAC_OPERATOR_DIV, "/"},
    {TAC_OPERATOR_MOD, "%"}, {TAC_OPERATOR_UMINUS, "-"}, {TAC_OPERATOR_EQ, "=="}, {TAC_OPERATOR_NE, "!="},
    {TAC_OPERATOR_GT, ">"}, {TAC_OPERATOR_LT, "<"}, {TAC_OPERATOR_GE, ">="}, {TAC_OPERATOR_LE, "<="},
    {TAC_OPERATOR_AND, "&&"}, {TAC_OR, "||"}, {TAC_NOT, "!"}, {TAC_OPERATOR_BIT_AND, "&"},
    {TAC_OPERATOR_BIT_OR, "|"}, {TAC_OPERATOR_BIT_XOR, "^"}, {TAC_OPERATOR_LEFT_SHIFT, "<<"}, {TAC_OPERATOR_RIGHT_SHIFT, ">>"},
    {TAC_OPERATOR_BIT_NOT, "~"}, {TAC_OPERATOR_ASSIGN, "="}, {TAC_OPERATOR_ADDR_OF, "&"},
    {TAC_OPERATOR_DEREF, "*"},
    {TAC_OPERATOR_GOTO, "goto"}, {TAC_OPERATOR_IF, "if"},
    {TAC_OPERATOR_LABEL, "label"}, {TAC_OPERATOR_CALL, "call"}, {TAC_OPERATOR_RETURN, "return"},
    {TAC_OPERATOR_PARAM, "param"}, {TAC_OPERATOR_FUNC_BEGIN, "func_begin"}, {TAC_OPERATOR_FUNC_END, "func_end"},
    {TAC_OPERATOR_INDEX, "[]"}, {TAC_OPERATOR_INDEX_ASSIGN, "[]= "}, {TAC_OPERATOR_NOP, "nop"}
};

class TACOperand{
    public:
        TACOperandType type; // Type of the operand (e.g., TEMP_VAR, IDENTIFIER, CONSTANT)
        string value;        // Value of the operand (e.g., variable name, constant value)

        TACOperand(TACOperandType type, string value);

        
};

TACOperand new_temp_var(){
    string temp_var = "t" + to_string(temp_var_id++);
    return TACOperand(TAC_OPERAND_TEMP_VAR, temp_var);
}

TACOperand new_label(){
    string label = "L" + to_string(label_id++);
    return TACOperand(TAC_OPERAND_LABEL, label);
}

TACOperand new_constant(string value){
    return TACOperand(TAC_OPERAND_CONSTANT, value);
}

TACOperand new_identifier(string value){
    return TACOperand(TAC_OPERAND_IDENTIFIER, value);
}

class TACOperator{
    public:

};


#endif