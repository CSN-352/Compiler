#include <iostream>
using namespace std;

class A{
    public:
        int x;    
};

class B : public A{
    public:
        int y;
};

int main(){
    B a;
    cout<<a.x<<endl;
}