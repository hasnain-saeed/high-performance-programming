#include <stdio.h>

int main(){
    const int size = 5;
    int matrix[size][size];

    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            if (i < j) {
                matrix[i][j] = 1;
            } else if (i > j) {
                matrix[i][j] = -1;
            } else {
                matrix[i][j] = 0;
            }
        }
    }

    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    return 0;
}
