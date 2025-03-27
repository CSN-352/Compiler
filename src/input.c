#include<stdio.h>
int func(){
    return 0;
}

int func1(int x){
    return 1;
}
int main(){
    int a;
    int var = func1(func());
    printf("%d", var);
}

