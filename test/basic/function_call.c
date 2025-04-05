// Passing

int add(int a, int b) {
    return a + b;
}
void add2(int a, char b)
{
    // printf("%d", a);
}
int main(){
    int sum = add(5, 10, 69);
    breakpoint;
    {
        breakpoint_global;
    }
    add(5, 'a');
}