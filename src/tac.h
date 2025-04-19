#ifndef TAC_H
#define TAC_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
using namespace std;

class Type;

static unsigned int instruction_id = 1;
static unsigned int temp_var_id = 1;
static unsigned int label_id = 1;
const int MAX_CODE_SIZE = 1e6; // Maximum number of TAC instructions

//##############################################################################
//################################## TACOperand ######################################
//##############################################################################

enum TACOperandType {
    TAC_OPERAND_TEMP_VAR,         // Temporary variables (e.g., t1, t2)
    TAC_OPERAND_IDENTIFIER,       // User-defined variables
    TAC_OPERAND_CONSTANT,         // Integer or float or string constants (e.g., 42, 3.14)
    TAC_OPERAND_LABEL,            // Jump targets (e.g., L1, L2)
    TAC_OPERAND_POINTER,          // Pointer (e.g., int*)
    TAC_OPERAND_TYPE,            // Type (e.g., int, float)
    TAC_OPERAND_EMPTY,           // Empty operand (used for NOP or no operation)
    TAC_OPERAND_STRING,          // String literal (e.g., "Hello")
};
class TACOperand {
public:
    TACOperandType type; // Type of the operand (e.g., TEMP_VAR, IDENTIFIER, CONSTANT)
    string value;        // Value of the operand (e.g., variable name, constant value)

    TACOperand() : type(TACOperandType::TAC_OPERAND_EMPTY), value("") {}

    TACOperand(TACOperandType type, string value);
};

extern unordered_map<string, TACOperand* > identifiers; // Map to store identifiers and their corresponding TAC operands

TACOperand* new_temp_var();

TACOperand* new_empty_var();

TACOperand* new_label();

TACOperand* new_constant(string value);

TACOperand* new_identifier(string value);

TACOperand* new_type(string value);

TACOperand* new_string(string value);

//##############################################################################
//################################## TACOperator ######################################
//##############################################################################

enum TACOperatorType {
    // Arithmetic Operators
    TAC_OPERATOR_ADD = 0,        // + #codegen done (add)
    TAC_OPERATOR_SUB,        // - #codegen done (sub)
    TAC_OPERATOR_MUL,        // * #codegen done (mul)
    TAC_OPERATOR_DIV,        // / #codegen done (div)
    TAC_OPERATOR_MOD,        // % #codegen done (mod)
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
    TAC_OPERATOR_OR,         // ||
    TAC_OPERATOR_NOT,        // !

    // Bitwise Operators
    TAC_OPERATOR_BIT_AND,    // &
    TAC_OPERATOR_BIT_OR,     // |
    TAC_OPERATOR_BIT_XOR,    // ^
    TAC_OPERATOR_LEFT_SHIFT, // <<
    TAC_OPERATOR_RIGHT_SHIFT,// >>
    TAC_OPERATOR_BIT_NOT,    // ~

    // // Assignment Operators
    TAC_OPERATOR_ASSIGN,     // = #codegen done (load)

    // Pointer and Memory Operators
    TAC_OPERATOR_ADDR_OF,    // & (Address-of) #codegen done (la)
    TAC_OPERATOR_DEREF,      // * (Dereference) #codegen done (load)

    // Casting Operators
    TAC_OPERATOR_CAST,       // Type casting (e.g., (int)x) #codegen done (cast)

    // Control Flow (Branching & Jumps)
    TAC_OPERATOR_GOTO,       // goto label
    TAC_OPERATOR_IF_GOTO,         // if (condition) goto label
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

class TACOperator {
public:
    TACOperatorType type; // Type of the operator (e.g., ADD, SUB, MUL)
    // string value;        // Value of the operator (e.g., "+", "-", "*")
    TACOperator(); // Default constructor
    TACOperator(TACOperatorType type);
};

// static std::unordered_map<TACOperatorType, std::string> opMap = {
//     {TAC_OPERATOR_ADD, "+"}, {TAC_OPERATOR_SUB, "-"}, {TAC_OPERATOR_MUL, "*"}, {TAC_OPERATOR_DIV, "/"},
//     {TAC_OPERATOR_MOD, "%"}, {TAC_OPERATOR_UMINUS, "-"}, {TAC_OPERATOR_EQ, "=="}, {TAC_OPERATOR_NE, "!="},
//     {TAC_OPERATOR_GT, ">"}, {TAC_OPERATOR_LT, "<"}, {TAC_OPERATOR_GE, ">="}, {TAC_OPERATOR_LE, "<="},
//     {TAC_OPERATOR_AND, "&&"}, {TAC_OR, "||"}, {TAC_NOT, "!"}, {TAC_OPERATOR_BIT_AND, "&"},
//     {TAC_OPERATOR_BIT_OR, "|"}, {TAC_OPERATOR_BIT_XOR, "^"}, {TAC_OPERATOR_LEFT_SHIFT, "<<"}, {TAC_OPERATOR_RIGHT_SHIFT, ">>"},
//     {TAC_OPERATOR_BIT_NOT, "~"}, {TAC_OPERATOR_ASSIGN, "="}, {TAC_OPERATOR_ADDR_OF, "&"},
//     {TAC_OPERATOR_DEREF, "*"},
//     {TAC_OPERATOR_GOTO, "goto"}, {TAC_OPERATOR_IF_GOTO, "if_goto"},
//     {TAC_OPERATOR_LABEL, "label"}, {TAC_OPERATOR_CALL, "call"}, {TAC_OPERATOR_RETURN, "return"},
//     // {TAC_OPERATOR_PARAM, "param"}, {TAC_OPERATOR_FUNC_BEGIN, "func_begin"}, {TAC_OPERATOR_FUNC_END, "func_end"},
//     {TAC_OPERATOR_INDEX, "[]"}, {TAC_OPERATOR_INDEX_ASSIGN, "[]= "}, {TAC_OPERATOR_NOP, "nop"}
// };

//##############################################################################
//################################## TACInstruction ######################################
//##############################################################################

class TACInstruction {
public:
    TACOperand* label; // Unique instruction label (instruction number)
    int flag; // if 0, then it is a normal instruction, if 1 then it is a goto instruction, if 2 then it is an if goto instruction
    TACOperator op; // Operator (e.g., ADD, SUB)
    TACOperand* arg1; // First operand (e.g., t1, a, 5)
    TACOperand* arg2; // Second operand (e.g., t2, b, 10)
    TACOperand* result; // Result operand (e.g., t3, c)

    // Default constructor
    TACInstruction() : label(nullptr), flag(0), op(TACOperator()), arg1(nullptr), arg2(nullptr), result(nullptr) {}
    TACInstruction(TACOperator op, TACOperand* result, TACOperand* arg1, TACOperand* arg2, int flag);
};

bool is_assignment(TACInstruction* instruction);

extern vector<TACInstruction*> TAC_CODE; // Array of TAC instructions

TACInstruction* emit(TACOperator op, TACOperand* result, TACOperand* arg1, TACOperand* arg2, int flag);

void backpatch(unordered_set<TACInstruction*> list, TACOperand* label);

unordered_set<TACInstruction*> merge_lists(unordered_set<TACInstruction*>& list1, unordered_set<TACInstruction*>& list2);

void print_TAC_instruction(TACInstruction* instruction);

void print_TAC();

void fix_labels_temps();

//##############################################################################
//################################## PRINT TACInstruction ######################################
//##############################################################################

string get_operand_string(TACOperand* operand);

#endif
