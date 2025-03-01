#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

static double get_wall_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
    return seconds;
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
    if (argc != 2) {
        printf("Usage: %s M\n", argv[0]);
        return 1;
    }
    int M = atoi(argv[1]);

    pthread_t threads[2];
    ThreadData threadData[2];
    double startTime = get_wall_seconds();

    // Divide the work into two halves
    threadData[0].start = 1;
    threadData[0].end = M / 2;
    threadData[1].start = M / 2 + 1;
    threadData[1].end = M;

    // Create two threads
    for (int i = 0; i < 2; i++) {
        if (pthread_create(&threads[i], NULL, countPrimes, &threadData[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // Wait for both threads to finish
    for (int i = 0; i < 2; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }

    // Combine the results from both threads
    int totalPrimeCount = threadData[0].count + threadData[1].count;
    printf("There are %d prime numbers between 1 and %d.\n", totalPrimeCount, M);
    double secondsTaken = get_wall_seconds() - startTime;
    printf("secondsTaken by program= %f\n", secondsTaken);

    return 0;
}
