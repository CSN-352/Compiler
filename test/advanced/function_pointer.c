struct Calculator
{
    int x;
};

int add(int a, int b)
{
    return a + b;
}
int (*funcPtr)(int, int) = add;
int main()
{
    int sum = funcPtr(3, 4);
    struct Calculator calc;
    calc.operation = add;

    printf("Sum: %d\n", calc.operation(3, 7));
    return 0;
}