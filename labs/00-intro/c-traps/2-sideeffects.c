#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() { 
	int i;

#if 0
	for(i=0; i < 10; i) 
		printf("i=%d\n", i);

#endif
    printf("main=%p\n", main);

#if 0
	i = 0;
	i = !!!(i==4);

	printf("i=%d\n", i);
	0;

	0,
	i,
	main,
	i==4;
	(void)0;

#endif
#if 0

	if(setuid != 0)
		printf("you are root!\n");
#endif

	return 0;
}
