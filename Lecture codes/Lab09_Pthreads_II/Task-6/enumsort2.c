#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_THREADS 5
#define LEN 100000

static double get_wall_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec / 1000000;
}

double indata[LEN], outdata[LEN];

typedef struct {
    int start;  // starting index (inclusive)
    int end;    // ending index (exclusive)
} ThreadBlock;

void *findRanksBlock(void *arg) {
    ThreadBlock *tb = (ThreadBlock *) arg;
    for (int j = tb->start; j < tb->end; j++) {
        int rank = 0;
        // Count how many elements are smaller than indata[j]
        for (int i = 0; i < LEN; i++) {
            if (indata[i] < indata[j])
                rank++;
        }
        outdata[rank] = indata[j];
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    int i, t;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Initialize the data arrays.
    for (i = 0; i < LEN; i++) {
        indata[i] = drand48();
        outdata[i] = -1.0;
    }

    double startTime = get_wall_seconds();

    // Divide LEN elements among NUM_THREADS threads.
    ThreadBlock blocks[NUM_THREADS];
    int blockSize = LEN / NUM_THREADS;
    int remainder = LEN % NUM_THREADS;
    int start = 0;
    for (t = 0; t < NUM_THREADS; t++) {
        blocks[t].start = start;
        blocks[t].end = start + blockSize + (t < remainder ? 1 : 0);
        start = blocks[t].end;
        if (pthread_create(&threads[t], &attr, findRanksBlock, (void *)&blocks[t]) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }
    }

    pthread_attr_destroy(&attr);
    for (t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    double timeTaken = get_wall_seconds() - startTime;
    printf("Time: %f  NUM_THREADS: %d\n", timeTaken, NUM_THREADS);

    // Check that the output array is sorted.
    for (i = 0; i < LEN - 1; i++) {
        if (outdata[i] > outdata[i + 1] && outdata[i + 1] > -1)
            printf("ERROR: %f, %f\n", outdata[i], outdata[i + 1]);
    }

    return 0;
}


/*

### **Discussion**

1. **Task Division:**
   In the original approach, one thread was created for each element (or in small batches), which incurred a high overhead because thread creation is relatively expensive. In this improved version, we create only a fixed number of threads (defined by `NUM_THREADS`) and assign each thread a block of consecutive elements. This reduces the overhead significantly.

2. **Computational Complexity:**
   For each element, we count the number of elements that are smaller. Since this inner loop runs over all `LEN` elements for each of the `LEN` elements, the overall time complexity is
   \[
   O(n^2)
   \]
   where \(n = \text{LEN}\).

3. **Comparison with Merge Sort:**
   - **Enumeration Sort:**
     - **Time Complexity:** \(O(n^2)\)
     - **Parallelizability:** The ranking for each element is independent, so it can be parallelized; however, the quadratic nature of the algorithm remains a limiting factor.
   - **Merge Sort:**
     - **Time Complexity:** \(O(n \log n)\)
     - **Parallelizability:** Merge sort can also be parallelized (for example, by sorting subarrays concurrently and then merging them), and it has a lower asymptotic complexity.
   - **Conclusion:**
     Even though enumeration sort is “embarrassingly parallel” (each element’s rank is computed independently), its \(O(n^2)\) complexity makes it less attractive compared to \(O(n \log n)\) algorithms like merge sort, especially as \(n\) grows large.

---

### **Final Answer**

- **Improved Implementation:**
  Instead of creating a thread per element, we create a fixed number of tasks where each thread computes the ranks for a block of elements.

- **Computational Complexity:**
  The enumeration-sort algorithm runs in \(O(n^2)\) time, which is much less efficient than merge sort’s \(O(n \log n)\) complexity.

- **Is It a Good Algorithm?**
  In practice, for large arrays, enumeration sort is not a good algorithm compared to merge sort because its quadratic complexity makes it slower as \(n\) increases, even though the task of computing the rank is perfectly parallelizable.
*/
