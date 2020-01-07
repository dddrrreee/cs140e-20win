#include <stdlib.h>
int *ialloc(unsigned n);
// what do you think gcc output is?
int main(void) {
    int *p = ialloc(4);
    *p = 10;
    return 0;
}
