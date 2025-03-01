#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define INTERVALS 500000000

typedef struct {
    int tid;
    long start;
    long end;
} ThreadData;

int num_threads;
double dx;
double *partial_sums;

void *compute_pi(void *arg) {
    ThreadData *data = (ThreadData *) arg;
    double sum = 0.0;
    for (long i = data->start; i <= data->end; i++) {
        double x = dx * (i - 0.5);
        sum += dx * 4.0 / (1.0 + x * x);
    }
    partial_sums[data->tid] = sum;
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s num_threads\n", argv[0]);
        return 1;
    }
    num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        printf("Please provide a positive number of threads.\n");
        return 1;
    }

    dx = 1.0 / INTERVALS;
    partial_sums = malloc(num_threads * sizeof(double));
    if (partial_sums == NULL) {
        perror("malloc failed");
        return 1;
    }

    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    ThreadData *threadData = malloc(num_threads * sizeof(ThreadData));
    if (threads == NULL || threadData == NULL) {
        perror("malloc failed");
        free(partial_sums);
        return 1;
    }

    // Divide the INTERVALS among the threads as evenly as possible.
    long intervals_per_thread = INTERVALS / num_threads;
    long remainder = INTERVALS % num_threads;
    long start = 1;

    for (int t = 0; t < num_threads; t++) {
        threadData[t].tid = t;
        threadData[t].start = start;
        // Distribute the extra intervals among the first 'remainder' threads.
        long extra = (t < remainder) ? 1 : 0;
        threadData[t].end = start + intervals_per_thread + extra - 1;
        start = threadData[t].end + 1;
        if (pthread_create(&threads[t], NULL, compute_pi, (void *) &threadData[t]) != 0) {
            perror("pthread_create failed");
            free(partial_sums);
            free(threads);
            free(threadData);
            return 1;
        }
    }

    // Wait for all threads to complete.
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    // Sum up the partial sums computed by each thread.
    double pi = 0.0;
    for (int t = 0; t < num_threads; t++) {
        pi += partial_sums[t];
    }

    printf("PI is approx. %.16f\n", pi);

    free(partial_sums);
    free(threads);
    free(threadData);
    return 0;
}
