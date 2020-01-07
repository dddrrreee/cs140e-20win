// local function.
static int x;
void bar(void) {
}

// can the compiler remove either write to <x>?
// anything else?
void foo(int *p) {
    x = 1;
    bar();
    x = 2;
    return;
}
