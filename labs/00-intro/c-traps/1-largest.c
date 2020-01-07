#include <stdio.h>

int main() { 
    int a = 1, b = 2;

#if 0
    a--;
    --b;

    -a;

    1;
    0;
    10;
    20+20;


	// a---b; // a-- -b;
	printf("a=%d, b=%d\n", a, b);


	a = 1; b = 1;
	a-- - --b;
    printf("expr=%d\n", a-- - --b);
	// printf("a=%d, b=%d\n", a, b);

	b-1;
	b+1;
	b-1;
	b+1;
	b-1;
	b+1;
	b-1;
	printf("a=%d, b=%d\n", a, b);

#endif
#if 0
	a = 1;
	a =- 2;

	a =   - 2;
	printf("a = %d\n", a);
#endif

	a = 2;  b = 20;
	int *p = &a;
	b = a /*p          /* p points at the divisor */;
	printf("b = %d\n", b);

	return 0;
}
