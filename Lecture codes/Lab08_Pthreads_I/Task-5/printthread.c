#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Structure to hold thread index
typedef struct {
    int thread_id;
} ThreadData;

// Thread function that prints the thread index
void *printThreadIndex(void *arg) {
    ThreadData *data = (ThreadData *)arg;  // Convert void pointer to ThreadData pointer
    printf("Hello from thread %d!\n", data->thread_id);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s N\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);  // Convert input argument to an integer
    if (N <= 0) {
        printf("Please provide a valid number of threads (N > 0).\n");
        return 1;
    }

    pthread_t threads[N];  // Array to store thread IDs
    ThreadData threadData[N];  // Array to store thread data

    // Create N threads
    for (int i = 0; i < N; i++) {
        threadData[i].thread_id = i;  // Assign unique index to each thread
        if (pthread_create(&threads[i], NULL, printThreadIndex, &threadData[i]) != 0) {
            perror("pthread_create failed");
            return 1;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads have completed.\n");
    return 0;
}
