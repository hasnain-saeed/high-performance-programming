#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define TASK_SIZE_THRESHOLD 1000

void bubble_sort(int* list, int N) {
    int i, j;
    for(i = 0; i < N-1; i++)
        for(j = i+1; j < N; j++) {
            if(list[i] > list[j]) {
                int tmp = list[i];
                list[i] = list[j];
                list[j] = tmp;
            }
        }
}

void merge_sort_with_buffer(int* list_to_sort, int N, int* work_buffer) {
    if(N < 10) {
        bubble_sort(list_to_sort, N);
        return;
    }

    if(N == 1)
        return;

    int n1 = N / 2;
    int n2 = N - n1;

    if (N >= TASK_SIZE_THRESHOLD) {
        #pragma omp task shared(list_to_sort, work_buffer)
        merge_sort_with_buffer(list_to_sort, n1, work_buffer);

        #pragma omp task shared(list_to_sort, work_buffer)
        merge_sort_with_buffer(list_to_sort + n1, n2, work_buffer + n1);

        #pragma omp taskwait
    } else {
        merge_sort_with_buffer(list_to_sort, n1, work_buffer);
        merge_sort_with_buffer(list_to_sort + n1, n2, work_buffer + n1);
    }

    // Merge step
    for(int i = 0; i < N; i++)
        work_buffer[i] = list_to_sort[i];

    int i1 = 0;
    int i2 = n1;
    int i = 0;

    while(i1 < n1 && i2 < N)
        list_to_sort[i++] = (work_buffer[i1] <= work_buffer[i2]) ? work_buffer[i1++] : work_buffer[i2++];

    while(i1 < n1)
        list_to_sort[i++] = work_buffer[i1++];

    while(i2 < N)
        list_to_sort[i++] = work_buffer[i2++];
}

void parallel_merge_sort(int* list_to_sort, int N) {
    int* work_buffer = malloc(N * sizeof(int));
    #pragma omp parallel
    {
        #pragma omp single
        merge_sort_with_buffer(list_to_sort, N, work_buffer);
    }
    free(work_buffer);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Usage: %s N\n", argv[0]);
        return -1;
    }

    int N = atoi(argv[1]);
    int* data = malloc(N * sizeof(int));

    for(int i = 0; i < N; i++)
        data[i] = rand() % 100000;

    double start = omp_get_wtime();
    parallel_merge_sort(data, N);
    double end = omp_get_wtime();

    printf("Sorted %d elements in %.4f seconds\n", N, end - start);

    // Optionally verify correctness
    for(int i = 0; i < N - 1; i++) {
        if(data[i] > data[i + 1]) {
            printf("Sort failed!\n");
            break;
        }
    }

    free(data);
    return 0;
}
