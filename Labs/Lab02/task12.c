#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int is_prime(int num) {
    if (num <= 1) {
        return 0;
    }
    for (int i = 2; i <= sqrt(num); i++) {
        if (num % i == 0) {
            return 0;
        }
    }
    return 1;
}

int main() {
    int n;

    printf("Enter the number of integers: ");
    scanf("%d", &n);

    int *array = (int *)malloc(n * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed for array.\n");
        return 1;
    }

    printf("Enter %d integers:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &array[i]);
    }

    int non_prime_count = 0;
    for (int i = 0; i < n; i++) {
        if (!is_prime(array[i])) {
            array[non_prime_count++] = array[i];
        }
    }

    array = (int *)realloc(array, non_prime_count * sizeof(int));
    if (array == NULL && non_prime_count > 0) {
        fprintf(stderr, "Memory reallocation failed for array.\n");
        return 1;
    }

    printf("New array (non-prime numbers): ");
    for (int i = 0; i < non_prime_count; i++) {
        printf("%d ", array[i]);
    }
    printf("\nSize of the new array: %d\n", non_prime_count);

    free(array);

    return 0;
}
