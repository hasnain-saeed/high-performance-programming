#include <stdio.h>
#include <math.h>

int isPerfectSquare(int num) {
    if (num < 0) {
        return 0;
    }

    int s = (int)sqrt(num);
    return (s * s == num);
}

int main() {
    int num;
    printf("Enter a number: ");
    scanf("%d", &num);

    if (isPerfectSquare(num)) {
        printf("%d is a perfect square.\n", num);
    } else {
        printf("%d is not a perfect square.\n", num);
    }

    return 0;
}
