#include <stdio.h>

int main(void) {
    printf("hello\n");
    fprintf(stderr, "world\n");
    printf("got character='%c'\n", getchar());
    return 0;
}
