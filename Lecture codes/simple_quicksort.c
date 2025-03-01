#include <stdio.h>
#include <stdlib.h>

// Swap two elements in the array.
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Partition the array using the Lomuto partition scheme.
int partition(int arr[], int left, int right) {
    int pivot = arr[right]; // choose the last element as pivot
    int i = left;
    for (int j = left; j < right; j++) {
        if (arr[j] < pivot) {
            swap(&arr[i], &arr[j]);
            i++;
        }
    }
    swap(&arr[i], &arr[right]);
    return i;
}

// The recursive Quick Sort function.
void quicksort(int arr[], int left, int right) {
    if (left < right) {
        int p = partition(arr, left, right);
        quicksort(arr, left, p - 1);
        quicksort(arr, p + 1, right);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s array_size\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    int *arr = malloc(n * sizeof(int));
    if (!arr) {
        perror("malloc failed");
        exit(1);
    }

    // Initialize the array with random integers.
    for (int i = 0; i < n; i++) {
        arr[i] = rand();
    }

    // Optionally, print the unsorted array.
    printf("Before sorting:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Sort the array.
    quicksort(arr, 0, n - 1);

    // Print the sorted array.
    printf("After sorting:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);
    return 0;
}
