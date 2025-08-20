// ============================================================================
// Trig lib.
// ============================================================================

#ifndef __TRIG_H__
#define __TRIG_H__

#include "archie/SDKTypes.h"

#define SINUS_TABLE_ENTRIES 8192

void MakeSinus();

int sin_fp(int32_t brad);
int cos_fp(int32_t brad);

float FastArcTan2(float y, float x);

#endif
