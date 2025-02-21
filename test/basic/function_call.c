int add(int a, int b) {
    return a + b;
}
void add(int a, char b)
{
    printf("%d", a);
}
int main(){
    int sum = add(5, 10);
    add(5, 'a');
}