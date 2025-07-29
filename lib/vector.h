// ============================================================================
// Vector library.
// ============================================================================

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "archie/SDKTypes.h"

struct vec2 {
    float x, y;
};

inline float dot(struct vec2 a, struct vec2 b) {
    return a.x * b.x + a.y * b.y;
}

inline i32 randomBetween(i32 min, i32 max) {            // can return negative values?
    return rand() % (max + 1 - min) + min;
}

#endif // __VECTOR_H__
