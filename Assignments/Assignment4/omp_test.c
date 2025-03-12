#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 100

int main(void) {
    double *a = aligned_alloc(32, N * sizeof(double));
    for (int i = 0; i < N; i++) a[i] = 0.0;

    #pragma omp parallel for reduction(+: a[:N])
    for (int i = 0; i < N; i++) {
        a[i] += 1.0;
    }

    for (int i = 0; i < N; i++) {
        if(a[i] != 1.0) {
            printf("Error at %d: a[i] = %f\n", i, a[i]);
        }
    }
    free(a);
    return 0;
}
