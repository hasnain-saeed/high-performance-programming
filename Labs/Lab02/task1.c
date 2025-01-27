#include<stdio.h>

int main(){
    for (int i = 100; i >= 0; i-=4){
        printf("%d, ", i);
    }
    printf("\n");

    int j=100;
    while (j>=0)
    {
        printf("%d, ", j);
        j -= 4;
    }
    printf("\n");

    j=100;
    do
    {
        printf("%d, ", j);
        j -= 4;
    } while (j>=0);

    printf("\n");
    return 0;
}
