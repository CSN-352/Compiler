#include "ast.h"
#include <string>
#include <vector>
using namespace std;

static unsigned long long int id = 0;
//##############################################################################
//################################## NODE ######################################
//##############################################################################
Node :: Node(unsigned int line_no, unsigned int column_no) : id(id++), line_no(line_no), column_no(column_no) {}
Node :: Node(): id(id++), line_no(0), column_no(0) {}

// void Node::add_children (Node * node) {
// 	assert(0);
// }
//##############################################################################
//################################## NONTERMINAL ######################################
//##############################################################################
NonTerminal :: NonTerminal(string name, unsigned int line_no, unsigned int column_no) : Node(line_no, column_no), name(name) {}
NonTerminal :: NonTerminal(string name) : Node(), name(name) {}

// void NonTerminal :: add_children(Node* node1){
//     (children.push_back(node1));
// }

//##############################################################################
//################################## TERMINAL ######################################
//##############################################################################
Terminal :: Terminal(string name, string value, unsigned int line_no, unsigned int column_no) : Node(line_no, column_no), name(name), value(value) {}
Terminal :: Terminal(string name, string value) : Node(), name(name), value(value) {}

// template<typename... Nodes>


// Node* create_terminal(string name, string value, unsigned int line_no, unsigned int column_no){
//     Terminal* P = new Terminal(name, value, line_no, column_no);
//     return P;
// }