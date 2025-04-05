int var = 10;
int *ptr = &var;
int **ptr2 = &ptr;
int ***ptr3 = &ptr2;
int main(){
    ***ptr3 = 20; 
}