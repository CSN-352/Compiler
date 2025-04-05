// Passing

void counter() {
    static int count = 0;
    count++;
    // printf("%d\n", count);
}
int main(){
    counter(); 
    counter();
}
