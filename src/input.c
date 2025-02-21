// struct Calculator
// {
//     int x;
// };

// int add(int a, int b)
// {
//     return a + b;
// }
// int (*funcPtr)(int, int) = add;
// int main()
// {
//     int sum = funcPtr(3, 4);
//     struct Calculator calc;
//     calc.operation = add;

//     printf("Sum: %d\n", calc.operation(3, 7));
//     return 0;
// }
struct iobuf
{
    char*   _ptr;
    int _cnt;
    char*   _base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char*   _tmpfname;
} FILE;
int main() {
    struct FILE *f = fopen("test.txt", "w+"); fprintf(f, "Hello, File!"); rewind(f);
    char buf[20]; fgets(buf, 20, f); printf("%s", buf);
    fclose(f);
}

// struct Point {
//     int x;
//     int y;
// };
// struct Point p1;
// int a=10;
// int main(){
//     struct Point p1;
//     int a;
//     int b
//     int c
//     int d=10;
//     struct Point p1;
//     int a;
//     int b
//     int c

//     int d=10;
//     struct Point p1;
//     int a;
//     int b
//     int c

//     int d=10;
// }

// class Basic_Class {
//     private{
//         int a;
//         int b;
//     }
// };

// class Basic_Class obj;

// int add(int a, int b) {
//     return a + b;
// }
// class Basic_Class {

// };
// class Basic_Class {
//     private{
//         int a;
//         int b;
//     }
//     public{
//         int x;
//     }
// };

// class B <- A, C {
//     public{
//         int x;
//     }
// };

// int arr[][];
// int p[];

// class Basic_Class obj;

// class Basic_Class {
//     private{
//         int x;
//     }
//     private{
        
//         int func(int a, int b){
//             return a+b;
//         }
        
//     }
// };

// int func(int x, ...){
//     int x;
// }

// class Basic_Class obj;


// class Basic_Class obj;
 

// int add(int a, int b) {
//     return a + b;
// }
// int (*funcPtr)(int, int) = add;
// int sum = funcPtr(3, 4); 
// int main(){ 
//     int y = 5;
//     int z = 5;
//     do {
//         static int k;
//         y--;
//     } while (y > 0);
// }
// int add(int a, int b) {
//     return a + b;
// }
// int sum = add(5, 10);

// int n = 0;
// until (n == 5) { 
//     printf("%d ", n);
//     n++;
// }

// int void func(long long int a1,float* d){
//     int a2 =10;
//     int b=20;
//     float c=30;
//     char*** str ="hello";
//     return &str;
// }
// enum ALPHA{
//     HAPPY,
//     SAD
// } beta;
// enum S{
//     HAPPY,
//     SAD
// }tt1;
// struct TT{
//         int p;
// } tt;
// struct GAMMA{
//     int a,c;
//     const float b;
//     int dd;
//     const char** st;
//     struct TT tt;
   
// }fac;
// struct TT1{
//         long double p;
// };
// int main(){
//     const unsigned long a2=10;
//     int a1;
//     float d;
//     float a2;
//     func(a1,&d);
// }

