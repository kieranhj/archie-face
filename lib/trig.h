// ============================================================================
// Trig lib.
// ============================================================================

#ifndef __TRIG_H__
#define __TRIG_H__

#include "maths.h"

#define SINUS_TABLE_ENTRIES 8192

void MakeSinus();

fix16_t sin_fix16(fix16_t brad);
fix16_t cos_fix16(fix16_t brad);

float FastArcTan2(float y, float x);

#endif
