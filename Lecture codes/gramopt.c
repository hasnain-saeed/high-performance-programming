#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#define NUM_THREADS	6
int timer();
pthread_mutex_t barr;
pthread_cond_t mysignal;
int waiting=0, state=0;

void barrier_init(){
    pthread_mutex_init(&barr, NULL);
    pthread_cond_init(&mysignal,NULL);
}

void barrier(){
    int mystate;
    pthread_mutex_lock (&barr);
    mystate=state;
    waiting++;
    if (waiting==NUM_THREADS){
		waiting=0; state=1-mystate;
		pthread_cond_broadcast(&mysignal);
	}
    while (mystate==state)
		pthread_cond_wait(&mysignal,&barr);
    pthread_mutex_unlock (&barr);
}

double **V,**Q;
int n;

struct thread_data
{
	int	id;
};

struct thread_data thread_data_array[NUM_THREADS];
pthread_mutex_t *lock;

void *gs(void *threadarg);
double vecNorm(double *,int );
double scalarProd(double *,double *,int);


int main(int argc, char *argv[]) {
   int i,j,k,time,t;
   double sigma,temp_norm,norm;
	void *status;
	pthread_t thread[NUM_THREADS];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   n = atoi(argv[1]);

    barrier_init();
	lock=(pthread_mutex_t *)malloc(n*sizeof(pthread_mutex_t));
	for (i=0;i<n;i++) pthread_mutex_init(&lock[i], NULL);

   //Allocate and fill vectors
   V = (double **)malloc(n*sizeof(double *));
   Q = (double **)malloc(n*sizeof(double *));
   for(i=0;i<n;i++){
      V[i] = (double *)malloc(n*sizeof(double));
      }
    for(i=0;i<n;i++){
       Q[i] = (double *)malloc(n*sizeof(double));
       }


  for (i = 0; i<n; i++)
	  for(j=0;j<n;j++)
		  V[i][j] = rand() % 5 + 1;

	time=timer();

	/* 1:st Vector */
	temp_norm = vecNorm(V[0],n);
	for (k=0; k<n; k++)
		Q[0][k] = V[0][k]/temp_norm;

   for(t=0; t<NUM_THREADS-1; t++) {
		thread_data_array[t].id=t;
		pthread_create(&thread[t], &attr, gs, (void *)&thread_data_array[t]);

	   }

	   t=NUM_THREADS-1;
	   thread_data_array[t].id=t;
       gs((void *)&thread_data_array[t]);

	   for(t=0; t<NUM_THREADS-1; t++)
		   pthread_join(thread[t], &status);

	time=timer()-time;
    printf("Elapsed time: %f \n",time/1000000.0);
	printf("Check orthogonality: %e \n",scalarProd(Q[n/2], Q[n/3], n));

   return 0;

}

void *gs(void *threadarg){
	int k,j,i,id,start;
	double sigma,temp_norm;
	struct thread_data *my_data;

	my_data = (struct thread_data *) threadarg;
	id=my_data->id;

	for (j=id;j<n;j+=NUM_THREADS) pthread_mutex_lock(&lock[j]);
	barrier();
	if (id==0) pthread_mutex_unlock(&lock[0]);

	for (i=1;i<n;i++){
		pthread_mutex_lock(&lock[i-1]);
		pthread_mutex_unlock(&lock[i-1]);
		start=(i/NUM_THREADS)*NUM_THREADS;
	    for (j=start+id;j<n;j+=NUM_THREADS)
            if (j>i-1){
		    sigma = scalarProd(Q[i-1],V[j],n);
		    for(k=0;k<n;k++)
			    V[j][k] -=sigma*Q[i-1][k];
			if (i==j){
				temp_norm=vecNorm(V[i],n);
				for (k=0; k<n; k++)
					Q[i][k] = V[i][k]/temp_norm;
				pthread_mutex_unlock(&lock[i]);
			}
		}
	}
}

double vecNorm(double *vec,int n){
   int i;
   double local_norm = 0;

   for(i=0;i<n;i++){
      local_norm+= (vec[i]*vec[i]);
   }
   return sqrt(local_norm);
}

double scalarProd(double *a,double *b,int n){
   int i;
   double scalar =0.0;

   for(i=0;i<n;i++){
      scalar +=a[i]*b[i];
   }

   return scalar;
}



// The Gram‐Schmidt algorithm must orthogonalize a set of vectors. In its sequential form, you loop over the vectors, and for each vector you first normalize it (to form Q[i]) and then subtract from each subsequent vector its projection onto Q[i]. In pseudocode, it looks like this:

// ```
// for (i=0; i<n; i++){
//    // Normalize V[i] to get Q[i]
//    norm = VecNorm(V[i]);
//    for (k=0; k<n; k++)
//       Q[i][k] = V[i][k] / norm;

//    // For all subsequent vectors, remove the component in Q[i]
//    for (j=i+1; j<n; j++){
//       s = ScalarProd(Q[i], V[j]);
//       for (k=0; k<n; k++)
//          V[j][k] -= s * Q[i][k];
//    }
}
// ```

// **Where is the parallelism?**
// Notice that for each fixed i, once you’ve computed Q[i], the projections onto all the remaining vectors (j = i+1 to n) are independent. You can perform these orthogonal projections concurrently.

// ---

// ### **Two Parallel Solutions**

// #### **Solution 1: Manager-Worker (Block-Based) Approach**

// In this approach, for each iteration i:
// 1. **Normalize Q[i]** is done serially.
// 2. The projection step (updating each V[j] for j = i+1 to n) is divided into blocks.
//    For example, if you have NUM_THREADS threads available, you split the indices j=i+1 to n into NUM_THREADS contiguous blocks.
// 3. The main (manager) thread spawns NUM_THREADS worker threads (or reuses them) to compute the projections for their block.
// 4. The main thread then waits (joins) until all worker threads finish before moving on to the next i.

// **Pros:**
// - Easy to understand and implement.
// - Each iteration’s projection work is clearly divided into disjoint blocks.
// - Synchronization happens once per outer loop iteration (after all projections are done).

// **Cons:**
// - There is overhead in creating (or synchronizing) threads repeatedly for every i.
// - For smaller sub-tasks (when n is not very large), the overhead of thread management can dominate the actual computation.
// - The master thread remains idle during the join phase.

// ---

// #### **Solution 2: Fixed Threads with Cyclic Task Distribution and Fine-Grained Locking**

// Here the idea is to create a fixed set of threads once (say NUM_THREADS) and then let them process the work in a cyclic manner:
// 1. **Initialization:**
//    Allocate one mutex (lock) per vector. This helps ensure that a vector’s updates (or its normalization) are completed before another thread uses that result.
// 2. **Cyclic Distribution:**
//    Instead of splitting the range [i+1, n] into contiguous blocks, assign work cyclically. For example, thread t works on all indices j such that j % NUM_THREADS == t (or a similar cyclic distribution).
// 3. **Synchronization Using Locks and Barriers:**
//    - Before a thread processes its share of V[j] for a given i, it must be sure that Q[i] (which comes from V[i]) is completely computed.
//    - Each vector’s lock is used to protect its update. The threads use a barrier (or explicit lock-waiting) so that all threads move to the next iteration only after the current projections are finished.
// 4. **No Repeated Thread Creation:**
//    The fixed threads run a loop over the iterations and perform their tasks according to the cyclic schedule.

// **Pros:**
// - Threads are created only once. This minimizes thread management overhead.
// - Cyclic distribution tends to balance the load better across threads.
// - Better cache utilization: by keeping the same threads working on different parts of the problem, you can sometimes exploit locality.
// - In some cases, this approach achieves super-linear speedup due to reduced overhead and improved cache behavior.

// **Cons:**
// - The programming model is more complex; you must carefully design synchronization (using one lock per vector and barriers) to ensure that no thread starts processing a vector before it is ready.
// - Fine-grained locking and cyclic scheduling add extra overhead if the work per task is too small.
// - Debugging and reasoning about correctness is more challenging.

// ---

// ### **Which One Is Better and Why?**

// - **For larger problems (e.g., 1000 vectors of length 1000 or more):**
//   **Solution 2** is generally better.
//   - **Why?**
//     - The fixed-thread approach minimizes the overhead of thread creation and termination because the threads are created once and reused.
//     - Cyclic distribution can lead to more balanced work and better cache locality.
//     - Fine-grained locks ensure that each vector’s update is correctly synchronized, and the overall synchronization overhead is lower when tasks are large.

// - **For smaller problems or simpler implementations:**
//   **Solution 1** might suffice because its structure is simpler.
//   - However, if the overhead of spawning many small tasks dominates, you may not see any benefit from parallelization.

// **In summary:**
// - **Solution 1** (block-based manager-worker) is easier to implement but suffers from repeated thread management overhead and potentially load imbalance in each iteration.
// - **Solution 2** (fixed threads with cyclic distribution) is more complex but minimizes overhead and often scales better, making it preferable for larger problems where the computational work per projection is significant.

// Thus, for a Gram-Schmidt orthogonalization on large matrices or large sets of vectors, **Solution 2 is usually the better choice** due to its lower overhead and improved load balancing, even though it requires more careful programming and synchronization.

// ---

// This explanation covers the optimizations discussed in the slide and highlights the trade-offs between the two solutions.

/*
Both approaches aim to parallelize the orthogonal projection part of the Gram–Schmidt process, but they do so with different trade-offs:

---

### **Approach 1: Manager-Worker (Block-Based Thread Creation Per Iteration)**

- **How It Works:**
  For each vector i (which is being normalized to form Q[i]), the algorithm spawns a set of worker threads that each compute the projection of Q[i] onto a contiguous block of the subsequent vectors V[j] (for j = i+1 to n). The main (manager) thread creates these threads (NUM_THREADS–1) for each iteration i, does its share of the work, and then joins the workers before proceeding to the next iteration.

- **Characteristics:**
  - **Thread Creation Overhead:** Threads are created and joined at every iteration of i.
  - **Task Division:** The range of indices j is divided into contiguous blocks among the threads.
  - **Simplicity:** The division of work is relatively straightforward, and each iteration’s projection work is clearly partitioned.

- **Downsides:**
  - **High Overhead for Many Iterations:** Repeated thread creation and join operations can be expensive when n is large (i.e., many iterations), which can negate the benefits of parallelism if the per-iteration work is not heavy enough.
  - **Less Reuse of Threads:** Threads are not reused across iterations; a new set is spawned every time.

---

### **Approach 2: Fixed Threads with Cyclic Work Distribution and Fine-Grained Synchronization**

- **How It Works:**
  A fixed number of threads (NUM_THREADS) are created once and persist throughout the entire Gram–Schmidt process. The work of projecting Q[i] onto V[j] for each i is distributed cyclically among these threads. For example, thread t processes all indices j such that j starts at a certain offset and then increments by NUM_THREADS (i.e., a cyclic distribution).
  Additionally, this approach employs:
  - **Per-Vector Locks:** One lock per vector ensures that updates to each V[j] and the subsequent computation of Q[j] (when j equals i in a later iteration) are properly synchronized.
  - **Barriers:** A barrier (implemented with condition variables and mutexes) synchronizes the threads at key points. For instance, all threads must reach the barrier before any thread moves on to the next iteration. This ensures that Q[i] is fully computed before any thread uses it for further projections.

- **Characteristics:**
  - **Minimal Thread Management Overhead:** Threads are created only once, and the same threads work through all iterations, reducing creation and termination overhead.
  - **Cyclic Work Distribution:** This often leads to better load balancing among threads since each thread handles a roughly equal number of projection tasks spread over the entire range.
  - **Complex Synchronization:** The use of per-vector locks and barriers makes the code more complex but also more efficient in synchronizing fine-grained dependencies.

- **Downsides:**
  - **Programming Complexity:** The synchronization is more intricate. Correctly handling the locks and barriers requires careful design.
  - **Potential Contention:** If many threads contend for the same lock (e.g., on a vector that is updated by several threads), it could become a bottleneck. However, because the work is distributed cyclically, this tends to be less severe than creating many threads repeatedly.

---

### **Which Approach Is Better?**

- **For Large Problems:**
  **Approach 2** is generally better when you have a large number of vectors (or a large problem size). Its benefits include:
  - **Lower Overhead:** By creating a fixed set of threads once, you avoid the repeated cost of thread creation and joining in every iteration.
  - **Better Load Balancing:** The cyclic distribution of work helps ensure that all threads get a fair share of the workload.
  - **Improved Cache Locality:** Reusing the same threads across iterations can lead to better cache reuse and reduced thread scheduling overhead.

- **For Simpler or Smaller Problems:**
  **Approach 1** might be acceptable due to its simplicity, but if the number of iterations is high, the overhead of repeatedly creating and joining threads can outweigh the benefits of parallelism.

**In summary:**
- **Approach 1** is easier to understand and implement but suffers from high thread management overhead when iterated many times.
- **Approach 2** is more sophisticated—it reuses threads and synchronizes them with barriers and fine-grained locks—which usually results in better performance (and sometimes even super-linear speedup due to improved cache behavior) on larger problems, despite being more complex to code.

For most large-scale Gram–Schmidt orthogonalization tasks, **Approach 2 is preferable** because the fixed-thread, cyclic approach minimizes overhead and provides a more balanced distribution of work across iterations.
*/
