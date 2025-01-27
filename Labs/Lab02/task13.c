#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file;
    char product[100];
    double price;
    int count;

    file = fopen("data.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file 'data.txt'.\n");
        return 1;
    }

    if (fscanf(file, "%d", &count) != 1) {
        fprintf(stderr, "Error: Failed to read the number of entries.\n");
        fclose(file);
        return 1;
    }

    printf("%-15s%s\n", "Product", "Price");
    printf("=========================\n");

    for (int i = 0; i < count; i++) {
        if (fscanf(file, "%s %lf", product, &price) == 2) {
            printf("%-15s%-10.2lf\n", product, price);
        } else {
            fprintf(stderr, "Error: Failed to read data on line %d.\n", i + 2);
            fclose(file);
            return 1;
        }
    }

    fclose(file);

    return 0;
}
