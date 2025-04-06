// Passing

typedef struct iobuf
{
    char*   _ptr;
    int _cnt;
    char*   _base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char*   _tmpfname;
} FILE;

FILE* fopen(const char* filename, const char* mode){
    return (FILE*)0x12345678; // Dummy file pointer for example
}
int fclose(FILE* stream){
    return 0; // Dummy return value for example
}
void fprintf(FILE* stream, const char* format, ...){}
void rewind(FILE* stream){}
void printf(const char* format, ...){}
char* fgets(char* str, int num, FILE* stream){
    return str; // Dummy implementation for example
}

int main() {
    FILE *f = fopen("test.txt", "w+"); fprintf(f, "Hello, File!"); rewind(f);
    char buf[20]; fgets(buf, 20, f); printf("%s", buf);
    fclose(f);
}
