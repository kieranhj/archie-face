// ============================================================================
// Vector library.
// ============================================================================

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdlib.h>
#include "maths.h"

typedef struct vec2f_s {
    float x, y;
} vec2f;

typedef struct vec2fp_s {
    fix16_t x, y;
} vec2fp;

inline float dot(vec2f a, vec2f b) {
    return a.x * b.x + a.y * b.y;
}

inline int randomBetween(int min, int max) {            // can return negative values?
    return rand() % (max + 1 - min) + min;
}

#endif // __VECTOR_H__
