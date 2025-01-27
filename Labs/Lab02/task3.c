#include<stdio.h>
#include <stdlib.h>

int main(){
    int a, b;
    printf("1. Integers: ");
    scanf("%d %d", &a, &b);
    printf("Result: %d\n", a%2==0 && b%2==0 ? a+b: a*b);

    int x, y, z;
    printf("2. Real numbers: ");
    scanf("%d %d %d", &x, &y, &z);

    x = abs(x);
    y = abs(y);
    z = abs(z);
    int first, second;
    first = (x > y) ? x : y;
    second = (x > y) ? y : x;

    if (z > first){
        second = first;
        first = z;
    }
    else if(z > second){
        second = z;
    }
    printf("Largest value: %d\n", first);
    printf("Second largest value: %d\n", second);

    return 0;
}
