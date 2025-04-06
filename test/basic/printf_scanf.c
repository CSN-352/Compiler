// Fix the variadic function signature for printf and scanf
int printf(const char *format, ...){}
int scanf(const char *format, ...){}

int main(){
    int val;
    float num;
    char ch;
    scanf("%d", &val);
    scanf("%f", &num);
    scanf(" %c", &ch); 
    printf("You entered: %c\n", ch);
}
