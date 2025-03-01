#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

const long int N1 = 1000000000;
const long int N2 = 7000000000;

static double get_wall_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
  return seconds;
}

void* the_thread_func(void* arg) {
  long int i;
  long int sum = 0;
  double startTime = get_wall_seconds();

  for(i = 0; i < N2; i++)
    sum += 7;
  /* OK, now we have computed sum. Now copy the result to the location given by arg. */
  long int * resultPtr;
  resultPtr = (long int *)arg;
  *resultPtr = sum;

  double secondsTaken = get_wall_seconds() - startTime;
  printf("secondsTaken by the_thread_func= %f\n", secondsTaken);
  return NULL;
}

int main() {
  double pstartTime = get_wall_seconds();
  printf("This is the main() function starting.\n");

  long int thread_result_value = 0;

  /* Start thread. */
  pthread_t thread;
  printf("the main() function now calling pthread_create().\n");
  pthread_create(&thread, NULL, the_thread_func, &thread_result_value);

  printf("This is the main() function after pthread_create()\n");
  double startTime = get_wall_seconds();
  long int i;
  long int sum = 0;
  for(i = 0; i < N1; i++)
    sum += 7;

  double secondsTaken = get_wall_seconds() - startTime;
  printf("secondsTaken by the main thread= %f\n", secondsTaken);
  /* Wait for thread to finish. */
  printf("the main() function now calling pthread_join().\n");
  pthread_join(thread, NULL);

  printf("sum computed by main() : %ld\n", sum);
  printf("sum computed by thread : %ld\n", thread_result_value);
  long int totalSum = sum + thread_result_value;
  printf("totalSum : %ld\n", totalSum);
  double psecondsTaken = get_wall_seconds() - pstartTime;
  printf("total seconds taken the program overall %f\n", psecondsTaken);
  return 0;
}


// const long int N1 = 1000000000;
// const long int N2 = 7000000000;
// secondsTaken by the main thread= 3.135529
// secondsTaken by the_thread_func= 21.651690
