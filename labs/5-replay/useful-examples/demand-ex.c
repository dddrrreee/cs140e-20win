#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "demand.h"

int main(void) {
	int i = 1;

	// AssertNow will generate compile error message.
	AssertNow(4==4);
	// prints file:file:lineno to stderr.
	debug("about to test demand:\n\t");
	// better version of assert() with an error message.
	demand(i==2, i has bogus value);
	return 0;
}
