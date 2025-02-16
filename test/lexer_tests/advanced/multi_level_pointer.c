int var = 10;
int *ptr = &var;
int **ptr2 = &ptr;
int ***ptr3 = &ptr2;
***ptr3 = 20; 