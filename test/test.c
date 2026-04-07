#include <stdio.h>

int main() {
    int a = 5;
    int b = 10;
    int c;

    c = a + b;
    c = c * 2;

    while(c > 0) {
        c = c - 1;
    }

    printf("%d\n", c);
    return 0;
}
