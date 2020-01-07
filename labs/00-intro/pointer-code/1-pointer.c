void bar(void);

// can compiler remove the first write to <*p>? 
void foo(int *p) {
    *p = 4;
    bar();
    *p = 5;
    return;
}
