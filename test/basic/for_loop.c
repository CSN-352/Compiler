int main()
{
    for (int i = 0; i < 10; i += 2)
    {
        if (i >= 10)
            break;
        printf("%d ", i);
    }
    for (int i = 0;; i++)
    {
        printf("%d ", i);
        if (i == 9)
            break;
    }
    for (float i = 0.5; i < 5; i += 0.5)
    {
        printf("%.1f ", i);
        for (float i = 0.5; i < 5; i += 0.5)
        {
            printf("%.1f ", i);
        }
    }
}