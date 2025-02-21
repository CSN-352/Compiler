int main(){
    label1:
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (i == 4) break;
        printf("%d ", i);
    }
    goto label;
    label:
    printf("Jumped here\n");
    goto label1;
}
