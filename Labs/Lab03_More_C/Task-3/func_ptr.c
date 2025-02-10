#include<stdio.h>

void print_int_1(int x) {
    printf("Here is the number: %d\n", x);
}

void print_int_2(int x) {
    printf("Wow, %d is really an impressive number!\n", x);
}

int main(){
    void (*func)(int);
    func = print_int_1;
    func(6);
    func = print_int_2;
    func(9);
    return 0;
}
