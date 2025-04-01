// [Passing]
// Test 1: Basic type checking
// int a;
// char b;
// double c;
// unsigned int d;

// [Passing]
// Test 2: List of declarations 
// int a, b, c;
// char x, y, z;

// [Passing]
// Test 3: Error redeclaration
// int a;
// int a;

// [Passing]
// Test 4: Pointer declarations
// int *ptr1;
// char **ptr2;
// double ***ptr3;

// [Passing]
// Test 5: Array Declarations
int arr[5];
char str[256];
double matrix[5][5];

// [Failing]
// Test 6: Struct Declaration
// struct Point {
//     int x;
//     int y;
// };
 
// [Failing]
// Test 7: Struct instantiation
// struct Point {
//     int x;
//     int y;
// };
// struct Point obj;

// [Failing]
// Test 8: Struct Error 
// struct NonExistentType obj;

// [Failing]
// Test 9: Struct Variable Declaration
// struct Point p1, p2;

// Test 10: Union Declaration
// union Data {
//     int i;
//     float f;
//     char str[20];
// };

// Test 11: Typedef Declaration 
// typedef unsigned int a;
// typedef struct Point Point_t;
