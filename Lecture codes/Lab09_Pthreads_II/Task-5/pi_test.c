#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 8
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
    dx = 1.0 / INTERVALS;
    partial_sums = malloc(NUM_THREADS * sizeof(double));
    if (partial_sums == NULL) {
        perror("malloc failed");
        return 1;
    }

    pthread_t *threads = malloc(NUM_THREADS * sizeof(pthread_t));
    ThreadData *threadData = malloc(NUM_THREADS * sizeof(ThreadData));
    if (threads == NULL || threadData == NULL) {
        perror("malloc failed");
        free(partial_sums);
        return 1;
    }

    // divide the INTERVALS among the threads as evenly as possible.
    long intervals_per_thread = INTERVALS / NUM_THREADS;
    long remainder = INTERVALS % NUM_THREADS;
    long start = 1;

    for (int t = 0; t < NUM_THREADS; t++) {
        threadData[t].tid = t;
        threadData[t].start = start;
        // distribute the extra intervals among the first 'remainder' threads.
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

    // wait for all threads to complete.
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    // sum up the partial sums computed by each thread.
    double pi = 0.0;
    for (int t = 0; t < NUM_THREADS; t++) {
        pi += partial_sums[t];
    }

    printf("PI is approx. %.16f\n", pi);

    free(partial_sums);
    free(threads);
    free(threadData);
}
