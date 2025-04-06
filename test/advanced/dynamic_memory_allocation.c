// Passing

void* malloc(unsigned long long size){
    return (void*)0;
}

const int NULL = 0;

int main(){
    int* ptr;
    int n;
    n = (int)sizeof(long long);
    ptr = (int*)malloc(n*sizeof(int));
    if(ptr==(int*)NULL){
        // printf("Memory not allocated");
        int x = 0;
        x++;
    }
}