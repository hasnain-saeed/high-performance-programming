#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *HelloWorld(void *arg)
{
    long thrid=(long)arg;
    printf("Hello World %ld!\n",thrid);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int NUM_THREADS;
    NUM_THREADS = atoi(argv[1]);
    pthread_t *threads;
    long t;
    
    threads=(pthread_t *)malloc(NUM_THREADS*sizeof(pthread_t));
    for(t=0;t<NUM_THREADS;t++)
        pthread_create(&threads[t], NULL, HelloWorld,(void *)t);
    pthread_exit(NULL);
}
