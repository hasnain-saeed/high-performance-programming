#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS    5
#define len 1000

struct thread_data
{
    int    thread_id;
    double *arr;
    int i1;
    int i2;
    double retpar;
};

void *globsum(void *threadarg)  // Without Mutex
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
    for (i=i1;i<i2;i++)
        locsum+=data[i];
    my_data->retpar=locsum;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    struct thread_data thread_data_array[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
    void* status;
    int i,t;
    double data[len],Gsum;
    
    Gsum=0;
    for (i=0;i<len;i++)
        data[i]=i+1.0;
        
    for(t=0;t<NUM_THREADS;t++){
        thread_data_array[t].thread_id = t;
        thread_data_array[t].arr = data;
        thread_data_array[t].i1=t*len/NUM_THREADS;
        thread_data_array[t].i2=(t+1)*len/NUM_THREADS;
        pthread_create(&threads[t], NULL, globsum, (void *)&thread_data_array[t]);
    }
    
    for(t=0;t<NUM_THREADS;t++){
        pthread_join(threads[t], &status);
        Gsum+=thread_data_array[t].retpar;
    }
    printf("Global sum: %f\n",Gsum);
    pthread_exit(NULL);
}

