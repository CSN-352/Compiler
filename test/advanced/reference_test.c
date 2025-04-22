
class MyClass
{
public:
    void sayHello()
    {
        int k = 0;
    }

    void callFunction()
    {
        int x = 0;
    }
};

void increment(int* a) {
    (*a)++;
}

int main(){
    MyClass myObj;
    myObj.callFunction();
    int a = 5;
    increment(&a);
    void (*fp)(int *) = increment;
    fp();
}