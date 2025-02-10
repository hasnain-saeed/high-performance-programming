#include <stdio.h>
#include <stdlib.h>

int rand_int(int N)
{
  int val = -1;
  while( val < 0 || val >= N )
    {
      val = (int)(N * (double)rand()/RAND_MAX);
    }
  return val;
}

void allocate_mem(int*** arr, int n)
{
  int i;
  *arr = (int**)malloc(n*sizeof(int*));
  for(i=0; i<n; i++)
    (*arr)[i] = (int*)malloc(n*sizeof(int));
}

void free_mem(int** arr, int n)
{
  int i;
  for(i=0; i<n; i++)
    free(arr[i]);
  free(arr);
}

/* kij */
void mul_kij(int n, int **a, int **b, int **c)
{
  int i, j, k;
  for (k=0; k<n; k++) {
    for (i=0; i<n; i++) {
      int x = a[i][k];
      for (j=0; j<n; j++)
	c[i][j] += x * b[k][j];
    }
  }
}

/* ijk */
void mul_ijk(int n, int **a, int **b, int **c)
{
  int i, j, k;
  for (i=0; i<n; i++)  {
    for (j=0; j<n; j++) {
      int sum = 0;
      for (k=0; k<n; k++)
	sum += a[i][k] * b[k][j];
      c[i][j] = sum;
    }
  }
}

/* jik */
void mul_jik(int n, int **a, int **b, int **c)
{
  int i, j, k;
  for (j=0; j<n; j++) {
    for (i=0; i<n; i++) {
      int sum = 0;
      for (k=0; k<n; k++)
	sum += a[i][k] * b[k][j];
      c[i][j] = sum;
    }
  }
}

int main()
{
  int i, j, n;
  int **a;
  int **b;
  int **c;
  int Nmax = 100; // random numbers in [0, N]

  printf("Enter the dimension of matrices\n");
  if(scanf("%d", &n) != 1) {
    printf("Error in scanf.\n");
    return -1;
  }

  allocate_mem(&a, n);

  for ( i = 0 ; i < n ; i++ )
    for ( j = 0 ; j < n ; j++ )
      a[i][j] = rand_int(Nmax);

  allocate_mem(&b, n);

  for ( i = 0 ; i < n ; i++ )
    for ( j = 0 ; j < n ; j++ )
      b[i][j] = rand_int(Nmax);

  allocate_mem(&c, n);

  mul_kij(n, a, b, c);
  printf("Version kij is finished...\n");
  mul_ijk(n, a, b, c);
  printf("Version ijk is finished...\n");
  mul_jik(n, a, b, c);
  printf("Version jik is finished...\n");

  /*
    printf("Product of entered matrices:\n");

    for ( i = 0 ; i < n ; i++ )
    {
    for ( j = 0 ; j < n ; j++ )
    printf("%d\t", c[i][j]);

    printf("\n");
    }
  */

  free_mem(a, n);
  free_mem(b, n);
  free_mem(c, n);

  return 0;
}



/*
Analysis of gprof Results

Based on the provided profiling data, here are the answers to the specific questions:

1️⃣ What are the most time-consuming operations?

The most time-consuming operations are:
	1.	mul_ijk (45.53%) - Consumes the most time.
	2.	mul_jik (45.53%) - Has the same runtime as mul_ijk.
	3.	mul_kij (8.76%) - Significantly faster than mul_ijk and mul_jik.

These operations dominate the execution time because they perform nested loops over large matrices, leading to extensive memory access and cache inefficiencies in some orders.

2️⃣ Which loop ordering is more efficient?

Based on execution times:
	•	Most efficient: mul_kij (1.40s)
	•	Least efficient: mul_ijk and mul_jik (7.28s each)

Why is mul_kij more efficient?
	•	kij provides better cache locality by ensuring stride-1 access for b[k][j] and c[i][j], reducing cache misses.
	•	ijk and jik suffer from poor memory access patterns, especially for b[k][j], which results in frequent cache misses.

📌 Conclusion: kij is the best loop ordering for performance.

3️⃣ What is the most called function?
	•	Most called function: rand_int() (called 1,620,000 times).
	•	rand_int() is used to initialize matrix values, which explains its high call count.
	•	However, it contributes very little to execution time (only 0.02s), meaning it’s computationally cheap.

4️⃣ Does the profiler output time include time while waiting for user input?
	•	No, gprof does not include user input wait times.
	•	gprof only profiles CPU time spent in executing functions, not time spent waiting for user input (e.g., scanf()).
	•	The main() function shows 0s self-time, meaning the time waiting for input was not measured.

5️⃣ Which metric is used to order functions in the call graph?
	•	Functions are sorted by the total execution time (including self and children).
	•	The most expensive functions appear first, making it easy to identify bottlenecks.
	•	In this case, mul_ijk and mul_jik appear first since they consume the most time.

6️⃣ What do the self and children columns represent?

Column	Meaning
self seconds	Time spent inside the function itself (excluding calls to other functions).
children seconds	Time spent in functions called by this function (propagated time).

For example:
	•	mul_ijk:
	•	self = 7.28s (all time spent in this function itself).
	•	children = 0s (no significant function calls inside).
	•	main:
	•	self = 0s (negligible computation in main itself).
	•	children = 15.99s (all computation happens in called functions like mul_ijk, mul_jik, etc.).

🚀 Final Summary
	•	Most time-consuming: mul_ijk and mul_jik (each 7.28s).
	•	Best loop ordering: mul_kij (only 1.40s).
	•	Most frequently called function: rand_int() (1,620,000 calls).
	•	Profiler does not measure user input wait time.
	•	Functions are sorted by total execution time in the call graph.
	•	self represents time spent in the function itself, while children represents time spent in functions it calls.

Would you like an analysis on how to further optimize performance? 🚀

*/
