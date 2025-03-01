#include <stdio.h>
#include <math.h>

void test_overflow() {
    printf("\n=== Testing Overflow ===\n");

    // Test float overflow
    float f = 1.0f;
    printf("Starting with float: %e\n", f);

    for (int i = 0; i < 20; i++) {
        f *= 100.0f;
        printf("After multiplying by 100: %e\n", f);
        if (isinf(f)) {
            printf("Reached infinity after %d iterations\n", i + 1);
            break;
        }
    }

    // Test double overflow
    double d = 1.0;
    printf("\nStarting with double: %e\n", d);

    for (int i = 0; i < 20; i++) {
        d *= 1000000000000000.0;
        printf("After multiplying by 1000000000000000: %e\n", d);
        if (isinf(d)) {
            printf("Reached infinity after %d iterations\n", i + 1);
            break;
        }
    }
    for (int i = 0; i < 20; i++) {
        d *= 100.0;
        printf("After multiplying by 100: %e\n", d);
        if (isinf(d)) {
            printf("Reached infinity after %d iterations\n", i + 1);
            break;
        }
    }
}

void test_nan() {
    printf("\n=== Testing NaN ===\n");

    double x = sqrt(-1.0);
    printf("sqrt(-1) = %f\n", x);

    // Test operations with NaN
    printf("NaN + 5 = %f\n", x + 5);
    printf("NaN * 2 = %f\n", x * 2);
}

void test_precision() {
    printf("\n=== Testing Precision ===\n");

    // Test float precision
    float epsilon_f = 1.0f;
    int count_f = 0;

    printf("Testing float precision:\n");
    while ((1.0f + epsilon_f) > 1.0f) {
        epsilon_f *= 0.5f;
        count_f++;
        printf("Iteration %d: epsilon = %e, 1 + epsilon = %f\n",
               count_f, epsilon_f, 1.0f + epsilon_f);
    }
    printf("Float lost precision after %d halvings\n", count_f);

    // Test double precision
    double epsilon_d = 1.0;
    int count_d = 0;

    printf("\nTesting double precision:\n");
    while ((1.0 + epsilon_d) > 1.0) {
        epsilon_d *= 0.5;
        count_d++;
        printf("Iteration %d: epsilon = %e, 1 + epsilon = %.17f\n",
               count_d, epsilon_d, 1.0 + epsilon_d);
    }
    printf("Double lost precision after %d halvings\n", count_d);
}

int main() {
    test_overflow();
    test_nan();
    test_precision();

    return 0;
}
