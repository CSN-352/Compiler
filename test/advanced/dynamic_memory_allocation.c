int main(){
    int* ptr;
    int n;
    ptr = (int*)malloc(n*sizeof(int));
    if(ptr==NULL){
        printf("Memory not allocated");
    }
}