#include <stdio.h>

int foo(void) { printf("called foo\n"); return 1; }
int bar(void) { printf("called bar\n"); return 0; }

int main() { 
	int p[10];
	int i = 100000;

	if(foo() || bar())
		printf("true\n");
	else
		printf("false\n");

	return 0;
}
