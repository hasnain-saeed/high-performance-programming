/*****************************************************************************
* FILE: mutex
 ******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS	10
double sum=0;
pthread_mutex_t mutexsum;

void *addone(void *arg)
{
	int i;
	pthread_mutex_lock (&mutexsum);
	for (i=0;i<100000;i++)    sum = sum + 1.0;
    pthread_mutex_unlock (&mutexsum);
    pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   pthread_t thread[NUM_THREADS];
   int t;
   void *status;

   /* Initialize the mutex */
   pthread_mutex_init(&mutexsum, NULL);

   for(t=0; t<NUM_THREADS; t++)
	   pthread_create(&thread[t], NULL, addone, NULL);

   for(t=0; t<NUM_THREADS; t++)
	   pthread_join(thread[t], &status);

    printf("Sum: %f\n",sum);
	pthread_mutex_destroy (&mutexsum);
    pthread_exit(NULL);
}
