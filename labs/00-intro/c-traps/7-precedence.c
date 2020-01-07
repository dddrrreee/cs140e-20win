#include <stdio.h>

int main() { 
	unsigned a = 8;

	if((a & 8) != 0)
		printf("overlaps with 8! a=%d\n", a);

#if 0
	unsigned lo = 3;
	if((1U <<  (31U + lo)) != 0U)
		printf("hi! = %d\n", 1U <<  (31U + lo));
#endif
	return 0;
}
