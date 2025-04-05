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
int a;
int a();
// class AccessTest {
// // int x;
//     void foo(){}
//     //     breakpoint;
//     //     x = 5;
//     //     breakpoint;
//     //     breakpoint_global;
    
// };
