// ============================================================================
// maths library.
// ============================================================================

#ifndef __MATHS_H__
#define __MATHS_H__

#include <stdint.h>

typedef int32_t fix16_t;

#define FIX16_ONE               (1<<16)
#define FIX16_HALF              (1<<15)
#define FIX16_QUARTER           (1<<14)
#define FIX16_PI                (205887)

#define FLOAT_TO_FIX16(a)       (fix16_t)((a)*FIX16_ONE)
#define INT_TO_FIX16(a)         (fix16_t)((a)<<16)
#define FIX16_TO_INT(a)         (int)((a)>>16)
#define FIX16_TO_FLOAT(a)       ((float)(a)/FIX16_ONE)

#define FIX16_MUL(a,b)          (fix16_t)(((a)>>8)*((b)>>8))
#define FIX16_DIV(a,b)          (fix16_t)(((a)<<8)/((b)<<8))

#endif // __MATHS_H__
