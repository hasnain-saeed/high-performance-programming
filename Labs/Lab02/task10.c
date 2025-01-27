#include<stdio.h>
#include<stdlib.h>

void print_array(int *arr, int n){
    for (int i = 0; i < n; i++){
        printf("%d, ", arr[i]);
    }
    printf("\n");

}

int main()
{
    int *arr;
    int n;
    printf("Enter number: ");
    scanf("%d", &n);
    arr = (int *)malloc(n*sizeof(int));
    for(int i=0; i<n; ++i) arr[i] = rand() % 100; // random number from 0 to 99
    print_array(arr, n);
    free(arr);
    return 0;
}
