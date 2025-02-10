#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE * fp;
    int intVal;
    double doubleVal;
    char charVal;
    float floatVal;

    fp = fopen("little_bin_file", "rb");
    if (fp == NULL){
        printf("Error opening file");
        return 1;
    }

    if (fread(&intVal, sizeof(int), 1, fp) != 1){
        printf("Error reading integer value");
        fclose(fp);
        return 1;
    }

    if (fread(&doubleVal, sizeof(double), 1, fp) != 1){
        printf("Error reading double value");
        fclose(fp);
        return 1;
    }

    if (fread(&charVal, sizeof(char), 1, fp) != 1){
        printf("Error reading character value");
        fclose(fp);
        return 1;
    }

    if (fread(&floatVal, sizeof(float), 1, fp) != 1){
        printf("Error reading float value");
        fclose(fp);
        return 1;
    }

    printf("%d\n", intVal);
    printf("%lf\n", doubleVal);
    printf("%c\n", charVal);
    printf("%f\n", floatVal);

    fclose(fp);
    return 0;
}
