#include "preprocess.h"

#define TWO 2
#define SQR(x)((x)*(x))

int main()
{
   int two = TWO;
   int sum = add(two, __LINE__);
   int sqr = SQR(two);
}

int add(int a, int b)
{
   return a + b;
}
