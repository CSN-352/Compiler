int main(int argc, char *argv[8])
{
    for (int i = 1; i < argc; i++)
    {
        // printf("Argument %d: %s\n", i, argv[i]);
        for (int j = 0; argv[i][j] != '\0'; j++)
        {
            // printf("  Character: %c\n", argv[i][j]);
        }
    }
    return 0;
}
