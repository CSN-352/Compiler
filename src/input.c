// struct Point {
//     int x;
//     int y;
// };
// struct Point p1;
// p1.x = 10;
// p1.y = 20;
// int add(int a, int b) {
//     return a + b;
// }
int (*funcPtr)(int, int) = add;
int sum = funcPtr(3, 4); 

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
   
// } 
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

