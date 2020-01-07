
#include <stdio.h>

int main() { 
	int *p;
	int i = 1;

	i = 10;
	p[1] = 10;
	if(i >= 10 | p[i] != 10)
		printf("not found\n");
	else
		printf("found!  %d\n", i);

	return 0;
}
