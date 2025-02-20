int void func(int a1,float* d){
    int a2 =10;
    int b=20;
    float c=30;
    char* str ="hello";
    return &str;
}
enum ALPHA{
    HAPPY,
    SAD
} beta;
enum S{
    HAPPY,
    SAD
}tt1;
struct TT{
        int p;
} tt;
struct GAMMA{
    int a,c;
    const float b;
    int dd;
    char** st;
    struct TT tt;
   
} 
struct TT1{
        long double p;
};
int main(){
    const unsigned long a2=10;
    int a1;
    float d;
    float a2;
    func(a1,&d);
}