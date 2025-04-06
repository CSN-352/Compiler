// Passing

class AccessTest {
    private:
        int secret;
        int foo(int x){
            return x*x;
        }

    public:
        int value;
        int add(int x, int y){
            return x+y;
        }

    protected:
        int hidden;
};


int main() {
    class AccessTest obj1, obj2;


    // obj1.value = 10; // Accessing public member
    // obj1.secret = 20; // Error: Accessing private member
    // obj1.hidden = 30; // Error: Accessing protected member
    int sum = obj1.add(5, 10); // Accessing public method
    int x = sum;
    x++;
        breakpoint;
    //     obj1.foo(5); // Error: Accessing private method
    return x;
}