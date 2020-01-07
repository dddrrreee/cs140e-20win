#include <stdio.h>
 
void foo(void) { printf("foo\n"); }
double  bar(void) { printf("bar\n"); }

int main( void ) {
	int a = 12;

  	int b = sizeof( ++a );
	printf("a=%d,b=%d\n", a,b);


	int c = sizeof foo(); 

	printf("a=%d,c=%d\n", a,c);

	int d = sizeof bar(); 

  	printf( "a=%d, b=%d c=%d, d=%d\n", a , b, c, d);
#if 0
#endif
  	return 0;
}
