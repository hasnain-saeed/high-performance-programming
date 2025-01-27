#include <stdio.h>
#include <stdlib.h>

int main() {
    int *array = NULL;       // Pointer to the dynamic array
    int size = 0;            // Number of elements currently in the array
    int capacity = 0;        // Current allocated capacity of the array
    int chunk_size = 10;     // Size of each allocation chunk
    int num;                 // Variable to store the input number
    int sum = 0;             // Variable to store the sum of numbers

    printf("Input: ");

    while (scanf("%d", &num) == 1) {
        if (size == capacity) {
            capacity += chunk_size;
            array = realloc(array, capacity * sizeof(int));
            if (array == NULL) {
                fprintf(stderr, "Memory allocation failed.\n");
                return 1;
            }
        }
        array[size++] = num;
    }

    printf("Output:\n");
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
        sum += array[i];
    }
    printf("\n");
    printf("Sum: %d\n", sum);
    free(array);

    return 0;
}
