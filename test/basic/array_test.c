// // Passing
// // TAC DONE

// struct foo
// {
//     int a;
//     int b;
//     struct foo *next;
// };

// // Enum definition
// enum Color
// {
//     RED,
//     GREEN,
//     BLUE
// };

// // Struct definition
// struct Point
// {
//     int x;
//     int y;
// };

// // Union definition with array
// union U
// {
//     int i[4];
//     float f;
// };

// void test_basic_arrays()
// {
//     // int arr[5] = {1, 2, 3, 4, 5};
//     // arr[2] = 10;
//     // int val = arr[2];
//     int arr[5][2];
//     int arr1[3];

//     int x = 8;
//     arr[0][0] = x;
//     arr[0][1] = 2;
//     arr[1][0] = 3;
//     arr[1][1] = 4;
//     arr[2][0] = 5;
//     arr[2][1] = 6;
//     arr[3][0] = 7;
//     arr[3][1] = 8;
//     arr[4][0] = 9;
//     arr[4][1] = 10;
//     arr1[0] = 1;
//     arr1[1] = 2;
//     arr1[2] = 3;
//     arr1[3] = 4;

    // int arrr[10][20][30];
    // *(arr + 2) = 20;
// }

// void test_pointer_access()
// {
//     int arr[3];
//     int *p = arr;
//     *(p + 1) = 80;
// }

// void test_multidim_array()
// {
//     // int mat[2][3] = {{1, 2, 3}, {4, 5, 6}};
//     // mat[1][2] = 99;
// }

// void test_struct_array()
// {
//     // struct Point points[2];
//     // points[0].x = 10;
//     // points[1].y = 40;
// }

// void test_enum_array()
// {
//     // enum Color colors[3] = {RED, GREEN, BLUE};
// }

// void test_union_with_array()
// {
//     union U u;
//     u.i[0] = 100;
//     u.i[3] = 400;
// }

int main()
{

    int arrr[10][20][30];
    ***(arrr + 2) = 20;
//     test_basic_arrays();
//     test_pointer_access();
//     test_multidim_array();
//     test_struct_array();
//     test_enum_array();
//     test_union_with_array();


//     // breakpoint;
//     // breakpoint_global;

//     // struct foo newS[5];

//     // // int arr2[];  // Fails on this
//     // char* str = "Hello";
//     // str[0] = 'H';
//     // int k = str[4];
//     // int k1 = str[6];

}