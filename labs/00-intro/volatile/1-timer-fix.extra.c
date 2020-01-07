// simple routine to time multiplication.
int mul(int *time, int a, int b) {
	unsigned volatile *timer = (void*)0x20003004;

	unsigned start = *timer;
        asm volatile ("" : : : "memory");
		int c = a * b;
        asm volatile ("" : : : "memory");
	unsigned end = *timer;

	*time = end - start;
	
	return c;
}

// this is a problem too.
void mul2(int *out, int *time, int a, int b) {
	unsigned volatile *timer = (void*)0x20003004;

	unsigned start = *timer;
        //asm volatile ("" : : : "memory");
		int c = a * b;
		*out = c;
        asm volatile ("" : : : "memory");
	unsigned end = *timer;

	*time = end - start;
}
