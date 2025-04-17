int x;  // global x

int foo(int a) {
    int x;  // shadowing global x
    return a + x;
}

int foo(float a) {
    int x;  // another foo scope
    return x;
}

class A {
    int x;  // class member x

    void method(int a) {
        int x;  // local x inside method
    }

    void method(float a) {
        int x;  // another overload
    }
};

class B {
    int x;  // different class, same member name
};

int main() {
    int x;  // x inside main
}
