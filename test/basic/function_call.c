// Passing
// TAC DONE
int add(int a, int b)
{
    return add(a, b + 1);
}
void add(int a, char b)
{
    // breakpoint;
    // printf("%d", a);
    b = 'a';
    return;
    // add(a,b+1);
    // return 100;
}
void add3(){
    return;
}
int main(){
    int sum = add(5, 10);
    // breakpoint;
    // breakpoint_global;
    add(5, 'a');
    // add2(5, 'a');
    // add3();
}