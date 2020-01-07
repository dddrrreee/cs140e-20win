#define AssertNow(x) switch(1) { case (x): case 0: ; }

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!(condition)]))

int main() { 
	AssertNow(0==1);

	BUILD_BUG_ON(0==0);
	BUILD_BUG_ON(1==0);
}
