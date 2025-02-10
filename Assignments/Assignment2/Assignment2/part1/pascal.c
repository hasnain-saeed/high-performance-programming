#include <stdio.h>
#include <stdlib.h>

// function declarations
void printPascal(int n);

int main(int argc, char const *argv[]) {
    // validate command line arguments
    if (argc != 2) {
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

// prints pascal's triangle with 'n' rows using dynamic programming
void printPascal(int n) {
    // Create an array to store the current row
    int *currentRow = (int *)malloc((n + 1) * sizeof(int));

    // Initialize first value
    currentRow[0] = 1;
    printf("%d \n", 1);

    for (int line = 1; line < n; line++) {
        // Calculate current row values using previous row
        currentRow[line] = 0;
        for (int i = line; i > 0; i--) {
            currentRow[i] = currentRow[i] + currentRow[i - 1];
        }

        // Print current row
        for (int i = 0; i <= line; i++) {
            printf("%d ", currentRow[i]);
        }
        printf("\n");
    }

    free(currentRow);
}
