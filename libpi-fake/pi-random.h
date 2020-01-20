#ifndef __PI_RANDOM_H__
#define __PI_RANDOM_H__

#include <stdint.h>

struct pi_random_data
  {
    int32_t *fptr;      /* Front pointer.  */
    int32_t *rptr;      /* Rear pointer.  */
    int32_t *state;     /* Array of state values.  */
    int rand_type;      /* Type of random number generator.  */
    int rand_deg;       /* Degree of random number generator.  */
    int rand_sep;       /* Distance between front and rear.  */
    int32_t *end_ptr;       /* Pointer behind state table.  */
  };

int pi_random_r(struct pi_random_data *buf, int32_t *result);
int pi_srandom_r (unsigned int seed, struct pi_random_data *buf);
int pi_initstate_r (unsigned int seed, char *arg_state, size_t n, struct pi_random_data *buf);
int pi_setstate_r (char *arg_state, struct pi_random_data *buf);

#endif
