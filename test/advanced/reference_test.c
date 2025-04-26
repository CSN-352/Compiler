// void increment(int *a)
// {

//     breakpoint;
//     breakpoint_global;

//     (*a)++;

//     breakpoint;
//     breakpoint_global;
// }


//  Add function call be reference

void test()
{
}

int main()
{
    // int a = 5;
    // increment(&a);
    void (*fp)() = test;
    fp();
    breakpoint;
    breakpoint_global;
}