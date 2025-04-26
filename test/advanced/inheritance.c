// Passing

class A1 {
public:
    int x1;
protected:
    int y1;
private:
    int z1;
};

class A2 {
public:
    int x2;
protected:
    int y2;
private:
    int z2;
};

class A3 {
public:
    int x3;
protected:
    int y3;
private:
    int z3;
    int inherited_from_A3() {
        return 0;
    }
};

// class B {
class B <- public A1, protected A2, private A3 {
public:
    int x4;
    int func(int a, int b) {
        return a + b;
    }
protected:
    int y4;
    int protected_func() {
        return 0;
    }
private:
    int z4;
};

// Multi level inheritence
class C < -public B
{
    void deeperTest()
    {
        x1 = 1; //  from A1
        x4 = 2; //  from B
        y2 = 3; //  A2 is protected in B, y2 is protected in A2 → not accessible
    }
};

int main() {
    class B obj;
    int x;
    // breakpoint_global;
    int sum = obj.func(1, 2);
    x = obj.x1;
    // x = obj.y1; // Error: y1 is protected in A1
    // x = obj.z1; // Error: z1 is private in A1
    // x = obj.x2; // Error: x2 is protected in A2
    // x = obj.y2; // Error: y2 is protected in A2
    // x = obj.z2; // Error: z2 is private in A2
    // x = obj.x3; // Error: x3 is private in A3
    // x = obj.y3; // Error: y3 is private in A3
    // x = obj.z3; // Error: z3 is private in A3
    x = obj.x4;
    // x = obj.y4; // Error: y4 is protected in B
    // x = obj.z4; // Error: z4 is private in B

    class C c;
    c.deeperTest();
}
