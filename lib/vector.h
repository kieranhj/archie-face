// ============================================================================
// Vector library.
// ============================================================================

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "archie/SDKTypes.h"
#include <stdlib.h>

#define FLOAT_TO_FP(val)    (int)((val)*(1<<16))
#define FP_TO_INT(val)      ((val)>>16)

typedef struct vec2f_s {
    float x, y;
} vec2f;

typedef struct vec2fp_s {
    int x, y;
} vec2fp;

inline float dot(vec2f a, vec2f b) {
    return a.x * b.x + a.y * b.y;
}

inline i32 randomBetween(i32 min, i32 max) {            // can return negative values?
    return rand() % (max + 1 - min) + min;
}

#endif // __VECTOR_H__
