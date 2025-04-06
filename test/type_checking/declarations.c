// [Passing]
// Test 1: Basic type checking
// int a;
// char b;
// double c;
// breakpoint;
// unsigned int d;

// [Passing]
// Test 2: List of declarations 
// int a, b, c;
// char x, y, z;

// [Passing]
// Test 3: Error redeclaration
// int a;
// int a;
// breakpoint

// [Passing]
// Test 4: Pointer declarations
// int *ptr1;
// char **ptr2;
// double ***ptr3;

// [Passing]
// Test 5: Array Declarations
// int arr[5];
// char str[256];
// double matrix[5][5];

// [Passing]
// Test 6: Struct Declaration
// struct A {
//     int x;
//     int y;
//     int z;
// };
 
// [Passing]
// // Test 7: Struct instantiation
// struct Point {
//     int arr[25];
//     int* x;
//     double* y;
// };
// struct Point obj;

// [Passing]
// Test 8: Struct Error 
// struct NonExistentType obj;

// [Passing]
// Test 9: Struct Variable Declaration
// struct Point p1, p2;

// [Passing]
// Test 10: Union Declaration
// union Data {
//     int i;
//     float f;
//     char str[20];
// };

// [Failing]
// Test 11: Union Declaration with instantiation
// union Data {
//     int i;
//     float f;
//     char str[20];
// };
// union Data obj;
// union Data obj2, obj3;


// [Passing]
// Test 12: Typedef Declaration 
// typedef unsigned int a;

// [Passing]
// Test 13: Struct Typedef
// struct Point {
//     int x;
//     int y;
// };
// typedef struct Point Point_t;

// [Failing]
// Test 14: Struct Typedef with resolution
// typedef struct Point {
//     int x;
//     int y;
// } Point_t;
// Point_t p1, p2;

// [Passing]
// Test 15: Function Declaration
// int foo() {
//     return 0;
// }
// int x;
// int main(){
//     x = foo();
//     return 0;
// }

// Test 16: Class Declaration
// class AccessTest {
//     int x;
//     int y;
//     int z;
//     void foo(int a){}
    //     breakpoint;
    //     x = 5;
    //     breakpoint;
    //     breakpoint_global;
    
// };

// class AccessTest obj, obj2;
// obj.foo();

// class A1 {
// public:
//     int x1;
// protected:
//     int y1;
// private:
//     int z1;
// };

// class A2 {
// public:
//     int x2;
// protected:
//     int y2;
// private:
//     int z2;
// };

// class A3 {
// public:
//     int x3;
// protected:
//     int y3;
// private:
//     int z3;
// };

// class B <- public A1, protected A2, private A3 {
// public:
//     int x4;
//     int func(int a, int b) {
//         return a + b;
//     }
// protected:
//     int y4;
// private:
//     int z4;
// };


// int main() {
//     class B obj;
//     int x;
//     breakpoint_global;
//     int sum = obj.func(obj.x1, obj.y1);
    // x = obj.x1;
    // x = obj.y1; // Error: y1 is protected in A1
    // x = obj.z1; // Error: z1 is private in A1
    // x = obj.x2; // Error: x2 is protected in A2
    // x = obj.y2; // Error: y2 is protected in A2
    // x = obj.z2; // Error: z2 is private in A2
    // x = obj.x3; // Error: x3 is private in A3
    // x = obj.y3; // Error: y3 is private in A3
    // x = obj.z3; // Error: z3 is private in A3
    // x = obj.x4;
    // x = obj.y4; // Error: y4 is protected in B
    // x = obj.z4; // Error: z4 is private in B
// }

void func(){
}

int main(){
    func();
    return 0;
}
