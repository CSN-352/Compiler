struct iobuf
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
int main() {
    struct FILE *f = fopen("test.txt", "w+"); fprintf(f, "Hello, File!"); rewind(f);
    char buf[20]; fgets(buf, 20, f); printf("%s", buf);
    fclose(f);
}
