#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    FILE *file;
    char product[100];
    char filename[50];
    double price;
    int count;

    if (argc < 2) {
        fprintf(stderr, "Error: No file name provided.\n");
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    strcpy(filename, argv[1]);
    file = fopen(filename, "r");
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
