#define AssertNow(expr)  switch(expr) { case 0: ; case expr: ; }


	
int main() { 
	// AssertNow(0 == 1);
	AssertNow(1 == 1);
	// AssertNow(0 == 1);


	unsigned i;

	AssertNow(sizeof i == 4);
	AssertNow(sizeof i == 8);
}
