// Passing

int add(int a, int b) {
    return a + b;
}
void add2(int a, char b)
{
    // printf("%d", a);
}
void add3(){
    return;
}
int main(){
    int sum = add(5, 10);
    breakpoint;
    {
        breakpoint_global;
    }
    add(5, 'a');
    add2(5, 'a');
    // add3();
}