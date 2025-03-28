#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include "ast.h"
using namespace std;

class Statement : public NonTerminal{
    public:
        // Statement();
};

class CompoundStatement : public Statement{
    public: 
        // CompoundStatement();
};

#endif