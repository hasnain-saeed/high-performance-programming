#include "pascal.h"

int main(int argc, char const *argv[]){
    // validate command line arguments
    if (argc != 2){
        printf("Usage: %s <n>\n", argv[0]);
        printf("Expected only one argument for the pascal triangle size\n");
        return 1;
    }

    // convert string to integer and validate
    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("Please enter a positive integer for the pascal triangle size\n");
        return 1;
    }
    printPascal(n);

    return 0;
}

// prints pascal's triangle with 'n' rows
void printPascal(int n){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < i+1; j++){
            printf("%d ", binomialCoeff(i, j));
        }
        printf("\n");
    }
}

// calculates the binomial coefficient C(n,k)
int binomialCoeff(int n, int k){
    if(n<k) return 0;
    if(n==k) return 1;
    // efficient binomial calculation e.g binomialCoeff(5,3) = 5*4*3/3*2*1
    return factorial(n,n-k+1)/factorial(k,1);
}

// calculates partial factorial from n down to t. e.g. factorial(5,3) = 5 * 4 * 3
int factorial(int n, int t) {
    if (n == 0) return 1;
    int result = 1;
    for (int i = n; i >= t; i--) {
        result *= i;
    }
    return result;
}
