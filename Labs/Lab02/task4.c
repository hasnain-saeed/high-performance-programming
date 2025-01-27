#include<stdio.h>

int main(){
    char op;
    double a, b, result;
    printf("Input: ");
    if (scanf("%lf%c%lf", &a, &op, &b) != 3) {
        printf("Invalid input. Please enter in the format: number operator number (e.g., 3+5)\n");
        return 1;
    }

    switch (op)
    {
    case '+':
        result = a + b;
        break;
    case '-':
        result = a - b;
        break;
    case '/':
        if (b != 0) {
            result = a / b;
        } else {
            printf("Error: Division by zero is not allowed.\n");
            return 1;
        }
        break;
    case '*':
        result = a * b;
        break;

    default:
        printf("Error: Unsupported operation '%c'.\n", op);
        return 1;
    }

    printf("Result: %lf\n", result);
    return 0;
}
