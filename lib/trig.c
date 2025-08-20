// ============================================================================
// Trig lib.
// ============================================================================

#include "trig.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static int sinusTable[SINUS_TABLE_ENTRIES];

void MakeSinus() {
    assert(SINUS_TABLE_ENTRIES == 8192);
    for(int i=0; i < SINUS_TABLE_ENTRIES; i++) {
        float r=(float) i / SINUS_TABLE_ENTRIES;
        float rad=2.0f * M_PI * r;
        sinusTable[i] = FLOAT_TO_FP(sinf(rad));

        // if ((i&255)==0) printf("[%d] r=%f rad=%f s=%f tab=%08lx\n\r",i,r,rad,sinf(rad),sinusTable[i]);
    }
}

// brad = [0.0, 256.0) in fixed point s8.16
// returns sin(brad) also in fixed point s15.16 format.
int sin_fp(int32_t brad) {
    int i=(brad >> 11) & (SINUS_TABLE_ENTRIES-1);
    int v=sinusTable[i];
    //printf("sin_fp(%08lx)=%08lx i=%d\n\r", brad, v, i);
    return v;
}

int cos_fp(int32_t brad) {
    return sinusTable[((brad + FLOAT_TO_FP(64)) >> 11) & (SINUS_TABLE_ENTRIES-1)];
}

float FastArcTan2(float y, float x) {
	if (x >= 0) { // -pi/2 .. pi/2
		if (y >= 0) { // 0 .. pi/2
			if (y < x) { // 0 .. pi/4
				return atanf(y / x);
			} else { // pi/4 .. pi/2
				return M_PI_2 - atanf(x / y);
			}
		} else {
			if (-y < x) { // -pi/4 .. 0
				return atanf(y / x);
			} else { // -pi/2 .. -pi/4
				return -M_PI_2 - atanf(x / y);
			}
		}
	} else { // -pi..-pi/2, pi/2..pi
		if (y >= 0) { // pi/2 .. pi
			if (y < -x) { // pi*3/4 .. pi
				return atanf(y / x) + M_PI;
			} else { // pi/2 .. pi*3/4
				return M_PI_2 - atanf(x / y);
			}
		} else { // -pi .. -pi/2
			if (-y < -x) { // -pi .. -pi*3/4
				return atanf(y / x) - M_PI;
			} else { // -pi*3/4 .. -pi/2
				return -M_PI_2 - atanf(x / y);
			}
		}
	}
}
