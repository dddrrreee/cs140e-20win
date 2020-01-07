#include <stdio.h>

int main() { 
      	int a = 1, b = 2, c = 3;

        if(a < b < c)
                printf("yea!  a<b<c\n");
        else
                printf("WAT!?!\n");


#if 0
        b = 1000000;
        if(a < b < c)
                printf("bad!!!!\n");
#endif

	return 0;
}
