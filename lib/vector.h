// ============================================================================
// Vector library.
// ============================================================================

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "maths.h"
#include <stdlib.h>
#include <assert.h>

#define VEC2FIX16_INIT_FROM_INT(x,y)    {.x=INT_TO_FIX16(x), .y=INT_TO_FIX16(y)}
#define VEC2FIX16_INIT_FROM_FLOAT(x,y)  {.x=FLOAT_TO_FIX16(x), .y=FLOAT_TO_FIX16(y)}

typedef struct vec2f_s {
    float x, y;
} vec2f;

typedef struct vec2fix16_s {
    fix16_t x, y;
} vec2fix16_t;

static inline float vec2f_dot(vec2f a, vec2f b) {
    return a.x * b.x + a.y * b.y;
}

// TODO: Probably shouldn't be here?
static inline int rand_between(int min, int max) {
    return min + (unsigned int)rand() % (max + 1 - min);    // NB. CLib standard rand() is unsigned int.
}

#endif // __VECTOR_H__
