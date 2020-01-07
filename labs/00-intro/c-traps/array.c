#include <stdio.h>

#if 0
#define concat(_x,_y) _x ## _y

#define CompileAssert(x) enum { concat(_x, __LINE__)  = 1/((x) == 1) }
#endif




#define _assert_now2(y) _ ##  y
#define UNIQUE_NAME(y) _assert_now2(y)

#define IF_YOU_SEE_THIS_ASSERT_FAILED(y) _assert_now2(y)


#if 0

#define CompileAssert(x) \
	struct UNIQUE_NAME(__LINE__) { char buf[1/((x) == 1)]; };

// compilers don't have to warn about this [i think]  they do have to warn
// about array size.
#define CompileAssert(x) \
	const int UNIQUE_NAME(__LINE__) = 1/((x) == 1);

#define CompileAssert(x) \
	extern char IF_YOU_SEE_THIS_ASSERT_FAILED(__LINE__)[1/ ((x) == 1)];

you can also just declare an array, but you may get compiler warnings since
its not used.

// i like this one.
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

#endif

#define CompileAssert(x) \
  extern char _if_you_see_this_assert_failed ## __COUNTER__ [1 - 2*((x) == 0)]

// extern char _if_you_see_this_assert_failed ## __COUNTER__ [1/ ((x) == 1)];

CompileAssert(sizeof(int) == 4);
//CompileAssert(sizeof(int) == 8);

extern int a[0];

int main() { 
      int i, v[10];

        i = 0;
        i[v] = 1;

	printf("v[0]=%d\n", v[0]);
	return 0;
}

// put in the  printf hacks :)

#if 0
The 0 width field tells that the following bitfields should be set on the next atomic entity (char)

power of 2
	n & -n

mod power of two 
	& (n-1)
#endif
