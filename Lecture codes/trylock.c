/*****************************************************************************
* FILE: mutex
 ******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM_THREADS	5
double sum=0;
pthread_mutex_t mutexsum;

void *addone(void *arg)
{
    int i;
    long id=(long)arg;
    while (1){
        if (!pthread_mutex_trylock(&mutexsum)){
            printf("Thread %ld have the lock\n",id);
            for (i=0;i<100000;i++)    sum = sum + 1.0;
            pthread_mutex_unlock (&mutexsum);
            break;
        }
        else {
            printf("Didn't get the lock %ld\n",id);
            sleep(1);  // Do something "useful" in parallel while waiting
        }
    }
    pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   pthread_t thread[NUM_THREADS];
   long t;
   void *status;

   pthread_mutex_init(&mutexsum, NULL);
   for(t=0; t<NUM_THREADS; t++) 
	   pthread_create(&thread[t], NULL, addone, (void *)t);
   
   for(t=0; t<NUM_THREADS; t++) 
	   pthread_join(thread[t], &status);
 
    printf("Sum: %f\n",sum);
	pthread_mutex_destroy (&mutexsum);
    pthread_exit(NULL);
}

