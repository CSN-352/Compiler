#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <type_traits>
using namespace std;

// Utility function to check if class T is derived from class B (for implicit type casting)
template<typename B, typename T>
constexpr bool instanceof(const T*) {
    return std::is_base_of<B, T>::value;
}
class Node;
class NonTemrinal;
class Terminal;

// AST Node
class Node {
public:
    unsigned long long int id;
    unsigned int line_no;
    unsigned int column_no;
    Node(unsigned int line_no, unsigned int column_no);
    Node();
    //virtual void add_children(Node* node1);

    virtual ~Node() {}  // Virtual destructor for proper cleanup
};

class NonTerminal : public Node {
public:
    string name; // Type of non-terminal 
    vector <Node*> children;
    NonTerminal(string name, unsigned int line_no, unsigned int column_no);
    NonTerminal(string name);
    // template<typename... Nodes>
    //void add_children(Node* node1);
};

class Terminal : public Node {
public:
    string name; // Type of terminal (e.g., "IDENTIFIER", "CONSTANT")
    string value;  // The actual value (e.g., "x", "42")
    Terminal(string name, string value);
    Terminal(string name, string value, unsigned int line_no, unsigned int column_no);
};

#endif