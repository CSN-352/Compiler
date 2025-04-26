// Passing
// TAC DONE
int add(int a, int b)
{
    return add(a, b + 1);
}
void add(int a, char b)
{
    // printf("%d", a);
    b = 'a';
    return;
    // add(a,b+1);
    // return 100;
}
void add()
{
}

int main()
{
    int sum = add(5, 10);
    // breakpoint;
    // {
    //     breakpoint_global;
    // }
    // int k = add(5, 'a');
    // add(5, 'a');
    // add();
}