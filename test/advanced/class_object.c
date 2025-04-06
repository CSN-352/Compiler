class A{
public :
    int x;
    int y;
    int add(int a, int c){
        return a+c;
    }
    int sub(int k, int l){
        return k-l;
    }
};
class A obj;

int main(){
    obj.y= 10;
    int a = obj.add(5, 10);
}
 