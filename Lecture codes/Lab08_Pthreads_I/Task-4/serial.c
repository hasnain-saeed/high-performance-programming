#include <math.h>
#include <stdio.h>
#include <stdlib.h>
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s M\n", argv[0]);
        return 1;
    }
    int M = atoi(argv[1]);
    int primeCount = 0;
    double startTime = get_wall_seconds();
    // Check every number from 1 to M
    for (int i = 1; i <= M; i++) {
        if (isPrime(i))
            primeCount++;
    }
    double secondsTaken = get_wall_seconds() - startTime;
    printf("secondsTaken by program= %f\n", secondsTaken);
    printf("There are %d prime numbers between 1 and %d.\n", primeCount, M);
    return 0;
}
