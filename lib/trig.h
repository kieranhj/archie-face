// ============================================================================
// Trig lib.
// ============================================================================

#ifndef __TRIG_H__
#define __TRIG_H__

#include "maths.h"

#define SINUS_TABLE_ENTRIES 8192

void trig_init();

fix16_t sin_fix16(fix16_t brad);
fix16_t cos_fix16(fix16_t brad);

// TODO: Probably shouldn't be here?
float trig_fast_arctan2(float y, float x);

#endif
