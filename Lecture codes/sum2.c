#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Global variables
pthread_mutex_t synch;
double Gsum;

struct thread_data
{
    int    thread_id;
    double *arr;
    int i1;
    int i2;
    double retpar;
};

void *globsum(void *threadarg)   // with Mutex
{
    struct thread_data *my_data;
    double *data;
    double locsum=0;
    int i,taskid,i1,i2;
    
    my_data = (struct thread_data *) threadarg;
    taskid = my_data->thread_id;
    data = my_data->arr;
    i1 =  my_data->i1;
    i2 =  my_data->i2;
    
    // Don't add to Gsum here, why?
    for (i=i1;i<i2;i++)
        locsum+=data[i];
    
    pthread_mutex_lock (&synch);
    Gsum+=locsum;
    pthread_mutex_unlock (&synch);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    struct thread_data *thread_data_array;
    pthread_t *threads;
    void* status;
    int i,t,NUM_THREADS,len;
    double *data;
    
    if (argc!=3) {
        printf("Usage: ./a.out len nthreads\n");
        return 0;
    }
    
    len=atoi(argv[1]);
    data=(double *)malloc(len*sizeof(double));
    
    NUM_THREADS=atoi(argv[2]);
    threads=(pthread_t *)malloc(NUM_THREADS*sizeof(pthread_t));
    thread_data_array=(struct thread_data *)malloc(NUM_THREADS*sizeof(struct thread_data));
    
    pthread_mutex_init(&synch, NULL);
    Gsum=0;
    for (i=0;i<len;i++)
        data[i]=i+1.0;
        
    for(t=0;t<NUM_THREADS;t++){
        thread_data_array[t].thread_id = t;
        thread_data_array[t].arr = data;
        thread_data_array[t].i1=t*len/NUM_THREADS;
        thread_data_array[t].i2=(t+1)*len/NUM_THREADS;
    //thread_data_array[t].i2=len/NUM_THREADS*(t+1);  /* Why is this not correct - Hint: integer div, try 1000 7 */
        printf("thread: %d   i1= %d    i2= %d\n",t,thread_data_array[t].i1,thread_data_array[t].i2);
        pthread_create(&threads[t], NULL, globsum, (void *)&thread_data_array[t]);
    }
    
    // NOTE: Join is necessary here!
    for(t=0;t<NUM_THREADS;t++){
        pthread_join(threads[t], &status);
    }
    printf("Global sum: %f\n",Gsum);
    pthread_exit(NULL);
}

