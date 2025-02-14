void increment(int* a) {
    (*a)++;
}

int a = 5;
increment(&a);