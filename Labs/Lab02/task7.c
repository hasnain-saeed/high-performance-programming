#include <stdio.h>

int main() {
    int number, original, reverse = 0, remainder;

    printf("Enter an integer: ");
    scanf("%d", &number);
    original = number;

    while (number > 0)
    {
        remainder = number % 10;
        reverse = reverse*10 + remainder;
        printf("Reverse: %d\n", reverse);
        number = number / 10;
    }

    if (original == reverse) {
        printf("Output: It is a palindrome.\n");
    } else {
        printf("Output: It is not a palindrome.\n");
    }

    return 0;
}
