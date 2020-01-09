// Q: try removing the memory fences: what happens?

// simple routine to time multiplication.
unsigned mul_mul(volatile int *out, int a, int b) {
    unsigned volatile *timer = (void*)0x20003004;

    unsigned start = *timer;
        asm volatile ("" : : : "memory");
        *out = a * b;
        asm volatile ("" : : : "memory");
    unsigned end = *timer;
    return end - start;
}

// attempt to correct measured cost of multiply by
// removing overhead of the store and reading time.
unsigned null_cost(volatile int *out, int a, int b) {
    unsigned volatile *timer = (void*)0x20003004;

    unsigned start = *timer;
        asm volatile ("" : : : "memory");
        *out = 0;
        asm volatile ("" : : : "memory");
    unsigned end = *timer;
    return end - start;

}
