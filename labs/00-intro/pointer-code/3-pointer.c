// static example
static int x;
void bar(int *);

// can the compiler remove either write to <x>?
void foo(int *p) {
    x = 1;
    bar(&x);
    x = 2;
    return;
}
