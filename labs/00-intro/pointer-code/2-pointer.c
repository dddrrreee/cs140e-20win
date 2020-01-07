// static example
static int x;
void bar();

// can the compiler remove the first write to <x>?
void foo(int *p) {
    x = 1;
    bar();
    x = 2;
    return;
}
