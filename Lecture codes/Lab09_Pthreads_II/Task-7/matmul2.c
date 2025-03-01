#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

static double get_wall_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec / 1000000;
}

double **A, **B, **C;
int n, NUM_THREADS;

typedef struct {
    int start_row;  // First row (inclusive) for this thread to compute
    int end_row;    // Last row (exclusive) for this thread
} ThreadData;

void *matmul_thread(void *arg) {
    ThreadData *data = (ThreadData *) arg;
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage: %s matrix_size num_threads\n", argv[0]);
        return -1;
    }

    n = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);

    // Allocate matrices as arrays of pointers.
    A = malloc(n * sizeof(double *));
    B = malloc(n * sizeof(double *));
    C = malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        A[i] = malloc(n * sizeof(double));
        B[i] = malloc(n * sizeof(double));
        C[i] = malloc(n * sizeof(double));
    }

    // Initialize matrices A and B with random values; C is set to 0.
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            A[i][j] = rand() % 5 + 1;
            B[i][j] = rand() % 5 + 1;
            C[i][j] = 0.0;
        }
    }

    printf("Doing matrix-matrix multiplication with %d threads...\n", NUM_THREADS);
    double startTime = get_wall_seconds();

    pthread_t *threads = malloc(NUM_THREADS * sizeof(pthread_t));
    ThreadData *tdata = malloc(NUM_THREADS * sizeof(ThreadData));

    // Divide the rows of C among the threads.
    int rows_per_thread = n / NUM_THREADS;
    int extra = n % NUM_THREADS;  // Extra rows to distribute among the first few threads
    int start = 0;
    for (int t = 0; t < NUM_THREADS; t++) {
        tdata[t].start_row = start;
        tdata[t].end_row = start + rows_per_thread + (t < extra ? 1 : 0);
        start = tdata[t].end_row;
        if(pthread_create(&threads[t], NULL, matmul_thread, (void *)&tdata[t]) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish.
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    double timeTaken = get_wall_seconds() - startTime;
    printf("Elapsed time: %f wall seconds\n", timeTaken);

    int i, j, k;
    // Correctness check (let this part remain serial)
    printf("Verifying result correctness for a few result matrix elements...\n");
    int nElementsToVerify = 5*n;
    double max_abs_diff = 0;
    for(int el = 0; el < nElementsToVerify; el++) {
        i = rand() % n;
        j = rand() % n;
        double Cij = 0;
        for(k = 0; k < n; k++)
        Cij += A[i][k] * B[k][j];
        double abs_diff = fabs(C[i][j] - Cij);
        if(abs_diff > max_abs_diff)
        max_abs_diff = abs_diff;
    }
    printf("max_abs_diff = %g\n", max_abs_diff);
    if(max_abs_diff > 1e-10) {
        for(i = 0; i < 10; i++)
        printf("ERROR: TOO LARGE DIFF. SOMETHING IS WRONG.\n");
        return -1;
    }
    printf("OK -- result seems correct!\n");

    // Free allocated memory.
    for (int i = 0; i < n; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
    free(threads);
    free(tdata);

    return 0;
}
