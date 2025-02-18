void increment(int* a) {
    (*a)++;
}

int main(){
    int a = 5;
    increment(&a);
}