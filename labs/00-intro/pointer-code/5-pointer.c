// local function.
int x;
void bar(void);

// can compiler remove anything?
void foo(int *p) {
    x = 1;
    bar();
    x = 2;
    return;
}

// "what happens if we comment out?"
void bar(void) { }
