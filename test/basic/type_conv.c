void test_int_family_implicit() {

    char c = 10;
    short s = c;         // char -> short
    int i = s;           // short -> int
    long l = i;          // int -> long
    unsigned int ui = i; // int -> unsigned int
    unsigned long ul = l; // long -> unsigned long

    // printf("char to short: %d\n", s);
    // printf("short to int: %d\n", i);
    // printf("int to long: %ld\n", l);
    // printf("int to unsigned int: %u\n", ui);
    // printf("long to unsigned long: %lu\n", ul);
}

void test_int_family_explicit() {

    int i = 1000;
    char c = (char)i;     // int -> char
    short s = (short)i;   // int -> short
    long l = (long)i;     // int -> long

    unsigned int ui = (unsigned int)i;
    unsigned long ul = (unsigned long)l;

    // printf("int to char (explicit): %d\n", c);
    // printf("int to short (explicit): %d\n", s);
    // printf("int to long (explicit): %ld\n", l);
    // printf("int to unsigned int (explicit): %u\n", ui);
    // printf("long to unsigned long (explicit): %lu\n", ul);
}

void test_int_to_float_implicit() {

    int i = 42;
    float f = i;
    double d = i;
    long l = 1000;
    double d2 = l;

    // printf("int to float: %f\n", f);
    // printf("int to double: %f\n", d);
    // printf("long to double: %f\n", d2);
}

void test_int_to_float_explicit() {

    int i = 42;
    float f = (float)i;
    double d = (double)i;
    long l = 1000;
    double d2 = (double)l;

    // printf("int to float (explicit): %f\n", f);
    // printf("int to double (explicit): %f\n", d);
    // printf("long to double (explicit): %f\n", d2);
}

void test_float_to_int_implicit() {
    float f = 3.14f;
    int i = f;
    long l = f;
    double d = 3.14;
    int i2 = d;

    // printf("float to int: %d\n", i);
    // printf("float to long: %ld\n", l);
    // printf("double to int: %d\n", i2);
}

void test_float_to_int_explicit() {
    float f = 3.14f;
    int i = (int)f;
    long l = (long)f;
    double d = 3.14;
    int i2 = (int)d;

    // printf("float to int (explicit): %d\n", i);
    // printf("float to long (explicit): %ld\n", l);
    // printf("double to int (explicit): %d\n", i2);
}

void test_char_to_int_implicit() {
    char c = 'A';
    int i = c;
    short s = c;

    // printf("char to int: %d\n", i);
    // printf("char to short: %d\n", s);
}

void test_int_to_char_implicit() {
    int i = 65;
    char c = i;

    // printf("int to char: %c (%d)\n", c, c);
}

void test_int_to_char_explicit() {
    int i = 300;
    char c = (char)i;

    // printf("int to char (explicit, data loss): %d\n", c);
}

void test_unsigned_signed() {
    unsigned int ui = 4000000000U;
    int i = (int)ui;

    // printf("unsigned int to int (explicit): %d\n", i);
}

int main() {
    test_int_family_implicit();
    test_int_family_explicit();
    test_int_to_float_implicit();
    test_int_to_float_explicit();
    test_float_to_int_implicit();
    test_float_to_int_explicit();
    test_char_to_int_implicit();
    test_int_to_char_implicit();
    test_int_to_char_explicit();
    test_unsigned_signed();
    return 0;
}
