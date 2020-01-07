// simple routine to time multiplication.
int mul(int *time, int *out, int a, int b) {
	volatile unsigned *timer = (void*)0x20003004;

	unsigned start = *timer;

        asm volatile ("" : : : "memory");
		// change to *out
		int c = a * b;
        asm volatile ("" : : : "memory");

	unsigned end = *timer;

	*time = end - start;
	
	return c;
}
