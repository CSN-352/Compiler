int add(int a, int b) {
    return a + b;
}
int (*funcPtr)(int, int) = add;
int sum = funcPtr(3, 4); 