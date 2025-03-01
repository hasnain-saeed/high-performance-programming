#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

static double get_wall_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec / 1000000;
}

// Naively check if a number is prime by counting its divisors
int isPrime(int n) {
    if (n < 2) return 0;  // 0 and 1 are not prime
    int count = 0;
    for (int i = 1; i <= sqrt(n); i++) {
        if (n % i == 0)
            count++;
    }
    // A prime number has exactly two divisors: 1 and itself
    return (count == 2);
}

// Structure to pass the range and store the result for each thread
typedef struct {
    int start;
    int end;
    int count;
} ThreadData;

// Thread function: count primes in the assigned range
void *countPrimes(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    data->count = 0;
    for (int i = data->start; i <= data->end; i++) {
        if (isPrime(i))
            data->count++;
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s M numThreads\n", argv[0]);
        return 1;
    }
    int M = atoi(argv[1]);
    int numThreads = atoi(argv[2]);
    if (M < 1 || numThreads < 1) {
        printf("Please provide positive integers for M and numThreads.\n");
        return 1;
    }

    // Allocate arrays for thread handles and their corresponding data
    pthread_t *threads = malloc(numThreads * sizeof(pthread_t));
    ThreadData *threadData = malloc(numThreads * sizeof(ThreadData));
    if (threads == NULL || threadData == NULL) {
        perror("malloc failed");
        return 1;
    }

    double startTime = get_wall_seconds();

    // Divide the range [1, M] among the threads as evenly as possible
    int baseRange = M / numThreads;
    int remainder = M % numThreads;
    int currentStart = 1;
    for (int i = 0; i < numThreads; i++) {
        threadData[i].start = currentStart;
        // Distribute the remainder among the first few threads
        int currentRange = baseRange + (i < remainder ? 1 : 0);
        threadData[i].end = currentStart + currentRange - 1;
        currentStart = threadData[i].end + 1;
    }

    // Create the threads
    for (int i = 0; i < numThreads; i++) {
        if (pthread_create(&threads[i], NULL, countPrimes, &threadData[i]) != 0) {
            perror("pthread_create failed");
            free(threads);
            free(threadData);
            return 1;
        }
    }

    // Wait for all threads to complete
    int totalPrimeCount = 0;
    for (int i = 0; i < numThreads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join failed");
            free(threads);
            free(threadData);
            return 1;
        }
        totalPrimeCount += threadData[i].count;
    }

    double secondsTaken = get_wall_seconds() - startTime;
    printf("There are %d prime numbers between 1 and %d.\n", totalPrimeCount, M);
    printf("Time taken: %f seconds\n", secondsTaken);

    free(threads);
    free(threadData);
    return 0;
}
