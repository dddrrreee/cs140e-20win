// this is a boehm example without threads: can the compiler 
// re-order or remove a write?
void foo(int *p, int *q) {
    *q = 1;
    *p = 2;
    *q = 3;
    return;
}
