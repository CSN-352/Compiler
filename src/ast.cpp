#include "ast.h"
#include <string>
#include <vector>
using namespace std;

static unsigned long long int id = 0;

Node :: Node(unsigned int line_no, unsigned int column_no) : id(id++), line_no(line_no), column_no(column_no) {}

NonTerminal :: NonTerminal(string name, unsigned int line_no, unsigned int column_no) : Node(line_no, column_no), name(name) {}

Terminal :: Terminal(string name, string value, unsigned int line_no, unsigned int column_no) : Node(line_no, column_no), name(name), value(value) {}

template<typename... Nodes>
void NonTerminal :: add_children(Nodes*... nodes){
    (children.push_back(nodes), ...);
}