// Passing
// TAC DONE

int add(int a, int b)
{
    return a + b;
}

int main()
{
    // breakpoint;
    // breakpoint_global;
    for (int i = 0; i < 10; i++)
    {
        // breakpoint;
        if (i >= 10)
            break;
        // printf("%d ", i);
    }
    for (int i = 0;; i++)
    {
        // breakpoint;
        // breakpoint_global;
        // printf("%d ", i);
        if (i == 9)
            break;
    }
    for (float i = 0.5; i < 5; i += 0.5)
    {
        // printf("%.1f ", i);
        for (float j = 0.5; j < 5; j += 0.5)
        {
            breakpoint_global;
            breakpoint;
            // printf("%.1f ", i);
        }
    }
}