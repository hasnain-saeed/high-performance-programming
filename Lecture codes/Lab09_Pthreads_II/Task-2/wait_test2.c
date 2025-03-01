#include <stdio.h>
#include <pthread.h>

int DoItNow = 0;  // Shared variable

pthread_mutex_t m;   // Mutex to protect DoItNow
pthread_cond_t c;    // Condition variable to signal when DoItNow is set

void* thread_func(void* arg) {
    printf("This is thread_func() starting, now waiting until DoItNow is set...\n");

    while(1) {
        /* Check if DoItNow has been set to 1. */
        int shouldBreakLoop = 0;
        pthread_mutex_lock(&m);
        pthread_cond_wait(&c, &m);
        if(DoItNow == 1)
            shouldBreakLoop = 1;
        pthread_mutex_unlock(&m);
        if(shouldBreakLoop == 1)
            break;
    }

    printf("This is thread_func() after being signaled.\n");
    return NULL;
}

int main() {
    printf("This is the main() function starting.\n");

    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&c, NULL);

    /* Start thread. */
    pthread_t thread;
    printf("The main() function now calling pthread_create().\n");
    pthread_create(&thread, NULL, thread_func, NULL);
    printf("This is the main() function after pthread_create()\n");

    /* Simulate main thread doing some work */
    long int k;
    double x = 1;
    for (k = 0; k < 2000000000; k++)
        x *= 1.00000000001;
    printf("Main thread did some work, x = %f\n", x);

    /* Signal the waiting thread */
    pthread_mutex_lock(&m);
    DoItNow = 1;
    pthread_cond_signal(&c);  // Wake up the waiting thread
    pthread_mutex_unlock(&m);

    /* Wait for thread to finish. */
    printf("The main() function now calling pthread_join().\n");
    pthread_join(thread, NULL);
    printf("This is the main() function after calling pthread_join().\n");

    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&c);

    return 0;
}
