// int func(int x){
//     int y = 1;
    
//     return 0;
// }


// int main(){
//     int x = 1;
//     int y = 2;
//     func(x);
//     return 2;
// }

// int func(int x){
//     int a = 1+2;
//     return a+2;
// }

// struct A {
//     int x;
//     int y;
// };

// int main(){
//     struct A a;
//     a.x = 10;
//     a.y = 5;
//     return 1;
// }

// int main(){
//     int x = 1;
//     goto L1;
//     x=3;
//     L1 : x = 2;
//     return 0;
// }

// int main(){
//     int x = 1;
//     int y = 2;
//     int z = x + y++;
// }

// int main(){
//     int a[5];
//     int b[1][2];
//     int x = a[0];
//     int y = b[0][1];
// }

// class A{
// public:
//     int x;
//     int func(){
//         x=1;
//         return 0;
//     }
// };

// int main(){
//     class A a;
//     a.x = 1;
//     a.func();
//     return 0;
// }

// int main(){
//     int x=1;
//     int y=2;
//     int z=x+y;
//     z++;
// }

// int main(int argc, char *argv[5])
// {
//     for (int i = 1; i < argc; i++)
//     {
//         //printf("Argument %d: %s\n", i, argv[i]);
//         for (int j = 0; argv[i][j] != '\0'; j++)
//         {
//             i++;
//         }
//     }
//     return 0;
// }

// char c = 'a';
// char d = '_';
// char e = '\r';

// int arr[10];
// int x = arr[5];

// int arr1[10][20];
// int y = arr1[5][10];

// double main() {
//     int a = 10, b = 5;
//     int sum = (a + b);
//     int diff = a - b;
//     int prod = a * b;
//     int quot = a / b;
//     int mod = a % b;
//     int result = (((a > b) && (b < 15)) || !(a == b));
//     int complea = (a + b) * (a - b) / 2;
//     int logic = (a > b) && (b != 0) || (a < 50);
//     float c = 5.5, d = 2.2;
//     float f_prod = c * d;
//     float f_quot = c / d;
//     char ch = 'A';
//     char next_ch = ch + 1;
    // breakpoint;
    // breakpoint_global;
//}

// int main() {
//     // Write C code here
//     int x=1;
//     int y=2;
//     int z=(x + y++)*x;
//     return 0;
// }

// int add(int a, int b) {
//     breakpoint_global;
//     return a + b;
// }

// int add(int a, int b, int c) {
//     return a + b + c;
// }

// int add(float a, float b){
//     breakpoint;
//     return a + b;
// }

// int main(){
//     int x;
//     label1:
//     for (int i = 0; i < 5; i++) {
//         // if (i == 2) continue;
//         if (i == 4) break;
//         //printf("%d ", i);
//     }
//     goto label;
//     label: x = 1;
//     //printf("Jumped here\n");
//     // goto label1;
// }

// // int main(){
// //     int a = 10;
// //     int b = 5;
// //     int c = 0;
// //     int d = a<b && a<c;
// }

// int main(){
//     int a = 10;
//     (--a)--;
// }

//int printf(const char *format, ...){}

// int main(){
//     int b = -1,x;
//     switch (b)
//     {
//         case 100:
//             x = 1;
//         case 1000:
//             x = 2;
//             break;
//         default:
//             x = 3;
//             break;
//     }
// }

// struct Point {
//     int x;
//     int y;
// };

// int main(){
//     struct Point points[2];
//     points[0].x = 1;
// }

// int main(){
//     int arr[5];
//     *(arr+2) = 10;
// }

// struct A {
//     int x;
//     int y;
// };

// int main(){
//     static struct A static_a;
//     static_a.x = 1;
// }

// class Incomplete;

// typedef struct Student
// {
//    int id;
//    char name[20];
// } student;

// typedef student st;

// int main(){
//     st s;
//     s.id = 1;
// // }

// void func(int a){
// }

// int arr[5];
// void func(int x){
//     x++;
// }

// int main(){
//     int x = 1;
//     func(x);
//     return 0;
// }

// int main(){
//     short x = 0xF;
//     short y = -2;
//     short z = x + y;
//     return 0;
// }

// char x = '\n';
// int y = 5;
// long long z= 10000000000;
// float f = 3.14;
// double d = 3.14;
// char* s = "Hello, World!";
// char c;

// int sub(int a, int b) {
//     int diff = a - b;
//     return diff;
// }

// int add(int a, int b) {
//     int sum = a + b;
//     return sum;
// }

// breakpoint_global;

// int main(){
//     int a = 1; // tested and working
//     a++; // tested and working
//     --a; // tested and working
//     int b = 2;
//     int c = a + b; // tested and working
//     int d = a - b; // tested and working
//     int e = a * b; // tested and working
//     int f = a / b; // tested and working
//     int g = a % b; // tested and working
//     int h = a & b; // tested and working
//     int i = a | b; // tested and working
//     int j = a ^ b; // tested and working
//     int k = a << b; // tested and working
//     int l = a >> b; // tested and working
// }

// int add(int a, int b) {
//     return a + b;
// }

// int main(){
//     int sum = add(1,2);
//     return 0;
// }

// class A{
//     public:
//     int x;
//     int y;
// };

// class B{
//     public:
//     int x;
//     int y;
// };

// class A obj;
// class B obj2;

// int main(){
//     obj.x = 1;
//     obj.y = 2;
//     return 0;
// }

// class A{
//     public:
//     int x;
//     int y;
// };

// int main(){
//     class A obj;
//     obj.x = 1;
//     return 0;
// }

// int main(){
//     int arr[5];
//     arr[2] = 10;
//     return 0;
// }

// int main(){
//     char* c = "hello";
//     char* d = c;
// }

// int main(){
//     int* x;
//     int* y = x+1;
// }
// int a = 1;

// int add(int a, int b) {
//     return a + b;
// }

// char c = 'B';

// int main(){
//     float a = 1.1;
//     float b = 2.5;
//     float c = a + b;
//     return 0;
// }



// void printf(char* str, int d, int e, int f){

// }

// int main(){
//     int p = 8;
//     int q = 10;
//     int r = p+q;
//     printf("The sum of %d and %d is %d\n", p, q, r);
//     return 0;
// }

// int main(){
//     int a = 0;
//     for(int i=0;i<5;i++){
//         a = 4;
//     }
//     return 0;
// }

// void printf(char* str, ...){
   
// }

// int main(){
//     int x = 5;
//     printf("x = %d", x);
//     return 0;
// }

// int main(){
//     int x = 1;
//     int y;
//     switch(x){
//         case 1:
//             y = 2;
//         case 2:
//             y = 3;
//             break;
//         default:
//             y = 4;
//     }
// }

// int factorial(int n){
//     if(n == 0) return 1;
//     return n * factorial(n-1);
// }

// int main(){
//     int x = factorial(4);
//     return 0;
// }

// enum DAYS {MONDAY, TUESDAY, WEDNESDAY=4, THURSDAY, FRIDAY, SATURDAY, SUNDAY};

// signed int i = 0;

// static int x = 1;
// class A {
// public:
//     int x1;
//     int x2;
//     void func() {
//         static int z = 1;
//     }
// };
// int add(int a, int b) {
//     static int y = 2;
//     return a + b;
// }

// int main(){
//     int c = add(1,2);
// }
// breakpoint_global;

// int main()
// {
//     int a = 1;
//     for(int i=5; i<8; i++){
//         if(i == 7) continue;
//         --a;
//         // if(i == 1) break;
//     }
// }

void printf(char* str, ...){
    
}

int main(){
    int a = 1;
    for(int j=12;j<15;j++){
        if(j == 14) continue;
        a--;
    }
    printf("a = %d\n", a);
    return 0;
}  