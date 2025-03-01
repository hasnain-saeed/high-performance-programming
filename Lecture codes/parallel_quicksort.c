// Below is one example of a parallel Quick Sort in C using Pthreads. In this version, we use a recursive strategy that spawns a new thread for one partition (say, the right side) and lets the calling (parent) thread sort the left side. We also use a cutoff threshold (when the subarray is small) and a maximum recursion (or thread‐spawning) depth so that we do not create too many threads. We also use a “median‐of‐three” pivot selection to help with load balance.

// Compile with something like:
// ```
// gcc -pthread -O2 parallel_quicksort.c -o parallel_quicksort
// ```

// ---

// ```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

#define THRESHOLD 1000  // For small subarrays, do sequential sort
#define MAX_DEPTH 4     // Maximum recursion depth for thread spawning

// Timer function: returns current time in seconds.
static double get_wall_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

// Structure to pass arguments to the parallel quicksort function.
typedef struct {
    int *array;
    int left;
    int right;
    int depth;
} qs_args;

// Swap two integers.
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Returns the median-of-three index among left, mid, and right.
int median_of_three(int *arr, int left, int right) {
    int mid = left + (right - left) / 2;
    if (arr[left] > arr[mid])
        swap(&arr[left], &arr[mid]);
    if (arr[left] > arr[right])
        swap(&arr[left], &arr[right]);
    if (arr[mid] > arr[right])
        swap(&arr[mid], &arr[right]);
    return mid;
}

// Lomuto partition scheme using median-of-three pivot selection.
int partition(int *arr, int left, int right) {
    int pivotIndex = median_of_three(arr, left, right);
    swap(&arr[pivotIndex], &arr[right]); // Move pivot to end.
    int pivot = arr[right];
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

// A simple sequential quicksort (recursive) for small subarrays.
void sequential_quicksort(int *arr, int left, int right) {
    if (left < right) {
        int pivot = partition(arr, left, right);
        sequential_quicksort(arr, left, pivot - 1);
        sequential_quicksort(arr, pivot + 1, right);
    }
}

// The parallel quicksort function.
void *parallel_quicksort(void *args) {
    qs_args *pargs = (qs_args*) args;
    int left = pargs->left;
    int right = pargs->right;
    int depth = pargs->depth;
    int *arr = pargs->array;

    if (left < right) {
        // If subarray is small or maximum depth reached, do sequential sort.
        if ((right - left) < THRESHOLD || depth >= MAX_DEPTH) {
            sequential_quicksort(arr, left, right);
        } else {
            int pivot = partition(arr, left, right);

            // Prepare arguments for the left partition.
            qs_args left_args;
            left_args.array = arr;
            left_args.left = left;
            left_args.right = pivot - 1;
            left_args.depth = depth + 1;

            // Allocate arguments for the right partition (to be passed to a new thread).
            qs_args *right_args = malloc(sizeof(qs_args));
            if (!right_args) {
                // If allocation fails, fall back to sequential sort.
                sequential_quicksort(arr, pivot + 1, right);
                parallel_quicksort(&left_args);
                return NULL;
            }
            right_args->array = arr;
            right_args->left = pivot + 1;
            right_args->right = right;
            right_args->depth = depth + 1;

            pthread_t thread;
            int rc = pthread_create(&thread, NULL, parallel_quicksort, right_args);
            if (rc) {
                // If thread creation fails, sort right sequentially.
                sequential_quicksort(arr, pivot + 1, right);
                free(right_args);
                parallel_quicksort(&left_args);
            } else {
                // Sort left partition in the current thread.
                parallel_quicksort(&left_args);
                // Wait for the right partition to finish.
                pthread_join(thread, NULL);
                free(right_args);
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s array_size\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    int *arr = malloc(n * sizeof(int));
    if (!arr) {
        perror("malloc");
        exit(1);
    }

    // Initialize the array with random integers.
    for (int i = 0; i < n; i++) {
        arr[i] = rand();
    }

    qs_args args;
    args.array = arr;
    args.left = 0;
    args.right = n - 1;
    args.depth = 0;

    double start_time = get_wall_seconds();
    parallel_quicksort(&args);
    double elapsed = get_wall_seconds() - start_time;

    printf("Elapsed time: %f seconds\n", elapsed);

    // Optionally, verify that the array is sorted.
    for (int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            printf("Error: array not sorted at index %d: %d > %d\n", i, arr[i-1], arr[i]);
            break;
        }
    }
    free(arr);
    return 0;
}

// ```

// ---

// ### **Explanation**

// 1. **Pivot Selection:**
//    We use a median-of-three method to choose a pivot. This helps avoid worst-case performance for already sorted (or nearly sorted) data.

// 2. **Partitioning:**
//    The array is partitioned using the Lomuto scheme, with the pivot moved to the end and then swapped into its correct place.

// 3. **Parallelism Strategy:**
//    - The function `parallel_quicksort` checks the size of the subarray and the current recursion depth.
//    - If the subarray is small (less than THRESHOLD) or we’ve reached MAX_DEPTH, it calls a sequential quicksort.
//    - Otherwise, it partitions the array and spawns a new thread to sort the right partition while the current thread sorts the left partition.
//    - By doing so, we create more tasks at deeper levels but limit the total number of threads (using MAX_DEPTH) to reduce overhead.

// 4. **Load Balancing and Thread Overhead:**
//    - In the early levels, the number of threads is low (and the work is unbalanced), but as the algorithm recurses, more threads are used.
//    - The optimal number of levels (MAX_DEPTH) depends on your system and the problem size. Too many threads can lead to overhead; too few may not fully utilize available cores.

// 5. **Improvements:**
//    - Better pivot selection (using median-of-three) improves load balance.
//    - By letting the parent thread handle one partition (here the left side) and only spawning one new thread per recursion level, we reduce thread overhead and improve cache locality.

// This code demonstrates one practical way to parallelize Quick Sort using Pthreads while being mindful of load imbalance and thread overhead.
