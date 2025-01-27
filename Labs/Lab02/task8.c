#include <stdio.h>

int main() {
    double myDouble = 3.14;
    int myInt = 42;
    char myChar = 'A';

    printf("Variable: myDouble\n");
    printf("Value: %lf\n", myDouble);
    printf("Address: %p\n", (void*)&myDouble);
    printf("Size: %lu bytes\n\n", sizeof(myDouble));

    printf("Variable: myInt\n");
    printf("Value: %d\n", myInt);
    printf("Address: %p\n", (void*)&myInt);
    printf("Size: %lu bytes\n\n", sizeof(myInt));

    printf("Variable: myChar\n");
    printf("Value: %c\n", myChar);
    printf("Address: %p\n", (void*)&myChar);
    printf("Size: %lu bytes\n", sizeof(myChar));

    return 0;
}
