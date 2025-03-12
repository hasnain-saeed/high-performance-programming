#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct {
    int *list_to_sort;
    int N;
} ms_args;

typedef int intType;

#define MIN_SIZE_FOR_THREAD 1000  // threshold, adjust as necessary

void* parallel_merge_sort(void* args) {
    ms_args *pargs = (ms_args*) args;
    int* list_to_sort = pargs->list_to_sort;
    int N = pargs->N;

    if (N <= 1) {
        return NULL;  // Base case, no sorting needed
    }

    int n1 = N / 2;
    int n2 = N - n1;

    int* list1 = (int*)malloc(n1 * sizeof(int));
    int* list2 = (int*)malloc(n2 * sizeof(int));

    for(int i = 0; i < n1; i++)
        list1[i] = list_to_sort[i];
    for(int i = 0; i < n2; i++)
        list2[i] = list_to_sort[n1 + i];

    ms_args left_args = { .list_to_sort = list1, .N = n1 };
    ms_args right_args = { .list_to_sort = list2, .N = n2 };

    pthread_t thread;
    int thread_created = 0;

    // Only create thread if array size is sufficiently large
    if (N > MIN_SIZE_FOR_THREAD) {
        if (pthread_create(&thread, NULL, parallel_merge_sort, &right_args) == 0) {
            thread_created = 1;
        } else {
            parallel_merge_sort(&right_args);  // Thread creation failed, fallback
        }
    } else {
        parallel_merge_sort(&right_args);  // Below threshold, no thread
    }

    // Always sort left half in the current thread
    parallel_merge_sort(&left_args);

    if (thread_created)
        pthread_join(thread, NULL);

    // Merge sorted halves
    int i = 0, i1 = 0, i2 = 0;
    while (i1 < n1 && i2 < n2) {
        if (list1[i1] < list2[i2]) {
            list_to_sort[i++] = list1[i1++];
        } else {
            list_to_sort[i++] = list2[i2++];
        }
    }
    while (i1 < n1)
        list_to_sort[i++] = list1[i1++];
    while (i2 < n2)
        list_to_sort[i++] = list2[i2++];

    free(list1);
    free(list2);

    return NULL;
}


static double get_wall_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
  return seconds;
}

static int count_values(const intType* list, int n, intType x) {
  int count = 0;
  int i;
  for(i = 0; i < n; i++) {
    if(list[i] == x)
      count++;
  }
  return count;
}

int main(int argc, char* argv[]) {
  if(argc != 2) {
    printf("Please give 1 argument: N (number of elements to sort).\n");
    return -1;
  }
  int N = atoi(argv[1]);
  printf("N = %d\n", N);
  if(N < 1) {
    printf("Error: (N < 1).\n");
    return -1;
  }
  intType* list_to_sort = (intType*)malloc(N*sizeof(intType));
  // Fill list with random numbers
  int i;
  for(i = 0; i < N; i++)
    list_to_sort[i] = rand() % 100;

  // Count how many times the number 7 exists in the list.
  int count7 = count_values(list_to_sort, N, 7);
  printf("Before sort: the number 7 occurs %d times in the list.\n", count7);

  // Sort list
  double time1 = get_wall_seconds();
  ms_args *args = malloc(sizeof(ms_args));
  args->list_to_sort = list_to_sort;
  args->N = N;

  pthread_t thread;
  pthread_create(&thread, NULL, parallel_merge_sort, args);

  pthread_join(thread, NULL);
  free(args);
  printf("Sorting list with length %d took %7.3f wall seconds.\n", N, get_wall_seconds()-time1);

  int count7_again = count_values(list_to_sort, N, 7);
  printf("After sort : the number 7 occurs %d times in the list.\n", count7_again);

  // Check that list is really sorted
  for(i = 0; i < N-1; i++) {
    if(list_to_sort[i] > list_to_sort[i+1]) {
      printf("Error! List not sorted!\n");
      return -1;
    }
  }
  printf("OK, list is sorted!\n");


  free(list_to_sort);

  return 0;
}
