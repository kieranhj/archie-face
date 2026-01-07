// ============================================================================
// Vector library.
// ============================================================================

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "maths.h"
#include <stdlib.h>
#include <assert.h>

typedef struct vec2f_s {
    float x, y;
} vec2f;

typedef struct vec2fp_s {       // TODO: Rename vec2fix16_t?
    fix16_t x, y;
} vec2fp;

inline float vec2f_dot(vec2f a, vec2f b) {
    return a.x * b.x + a.y * b.y;
}

// TODO: Probably shouldn't be here?
inline int rand_between(int min, int max) {
    return min + (unsigned int)rand() % (max + 1 - min);    // NB. CLib standard rand() is unsigned int.
}

#endif // __VECTOR_H__
