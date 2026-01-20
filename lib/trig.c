// ============================================================================
// Trig lib.
// ============================================================================

#include "trig.h"
#include "vector.h"
#include "file.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static fix16_t sinusTable[SINUS_TABLE_ENTRIES];

#ifdef _DEBUG
const char sinus_filename[16]="sinus\0";
#endif

void trig_init()
{
    #ifdef _DEBUG
    if (file_load_at_address(sinus_filename, sinusTable) == 1)
        return;
    #endif

    // Generate sine table.
    assert(SINUS_TABLE_ENTRIES == 8192);
    for(int i=0; i < SINUS_TABLE_ENTRIES; i++)
    {
        float r=(float) i / SINUS_TABLE_ENTRIES;
        float rad=2.0f * M_PI_F * r;
        sinusTable[i] = FLOAT_TO_FIX16(sinf(rad));

        // if ((i&255)==0) printf("[%d] r=%f rad=%f s=%f tab=%08lx\n\r",i,r,rad,sinf(rad),sinusTable[i]);
    }

    #ifdef _DEBUG
    file_save(sinus_filename, sinusTable, sizeof(sinusTable));
    #endif
}

// brad = [0.0, 256.0) in fixed point s8.16
// returns sin(brad) also in fixed point s15.16 format.
fix16_t sin_fix16(fix16_t brad)
{
    return sinusTable[(brad >> 11) & (SINUS_TABLE_ENTRIES-1)];
}

fix16_t cos_fix16(fix16_t brad)
{
    return sinusTable[((brad + FLOAT_TO_FIX16(64)) >> 11) & (SINUS_TABLE_ENTRIES-1)];
}

float trig_fast_arctan2(float y, float x)
{
	if (x >= 0) { // -pi/2 .. pi/2
		if (y >= 0) { // 0 .. pi/2
			if (y < x) { // 0 .. pi/4
				return atanf(y / x);
			} else { // pi/4 .. pi/2
				return M_PI_2_F - atanf(x / y);
			}
		} else {
			if (-y < x) { // -pi/4 .. 0
				return atanf(y / x);
			} else { // -pi/2 .. -pi/4
				return -M_PI_2_F - atanf(x / y);
			}
		}
	} else { // -pi..-pi/2, pi/2..pi
		if (y >= 0) { // pi/2 .. pi
			if (y < -x) { // pi*3/4 .. pi
				return atanf(y / x) + M_PI_F;
			} else { // pi/2 .. pi*3/4
				return M_PI_2_F - atanf(x / y);
			}
		} else { // -pi .. -pi/2
			if (-y < -x) { // -pi .. -pi*3/4
				return atanf(y / x) - M_PI_F;
			} else { // -pi*3/4 .. -pi/2
				return -M_PI_2_F - atanf(x / y);
			}
		}
	}
}

// ============================================================================

#if 0
void plotSinCos()
{
    for (int i=0; i < 256; i++)
    {
        // NB. tables are upside down because the screen is 0 at the top...
        //plot_point(i, 128+cosLookupTable[i], 255);
        //plot_point(i, 128+sineLookupTable[i], 64);

        int32_t c=cos_fix16(i<<16);
        int32_t s=sin_fix16(i<<16);

        plot_point(i, 128+(c>>9), 255);
        plot_point(i, 128+(s>>9), 64);
    }
}
#endif

#if 0
void ArcTanTest()
{
    for(int a=0; a<256; a+=8)
    {
        int r=10;
        float dx=cosf(a*M_PI/128.0f)*r;
        float dy=sinf(a*M_PI/128.0f)*r;
        float at2=atan2f(dy,dx);
        float a2=at2/4.0f;
        float fat2=trig_fast_arctan2(dy,dx);
        //float a2=fat2/(2.0f*M_PI);
        //if (a2<0.0f) a2=1.0f+a2;
        a2*=256;
        printf("a=%d (%f,%f) at2=%f fat=%f a2=%d\n\r",a,dx,dy,at2,fat2,(int)a2);
    }
    int a=0.0f,b=5.0f;
    printf("atan2f(5.0,0.0)=%f\n\r",atan2f(a,b));
    a=2.0f;b=0.0f;
    printf("atan2f(0.0,2.0)=%f\n\r",atan2f(a,b));
    a=0.0f;b=-5.0f;
    printf("atan2f(-5.0,0.0)=%f\n\r",atan2f(a,b));
    a=-2.0f;b=0.0f;
    printf("atan2f(0.0,-2.0)=%f\n\r",atan2f(a,b));
}
#endif
