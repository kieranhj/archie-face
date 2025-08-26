// ============================================================================
// Plot functions.
// ============================================================================

#include "plot.h"
#include <stdlib.h>
#include <stdio.h>
#include "../src/globals.h"     // TODO: Rarr! Libs shouldn't depend on src? :S
#include "trig.h"

void plotPoint(register int x, register int y, register u8 c) {
    if (x>=0 && x<320 && y>=0 && y<256) *(g_framebuffer + x + y*320) = c;
}

void plotSinCos() {
    for (int i=0; i < 256; i++) {
        // NB. tables are upside down because the screen is 0 at the top...
        //plotPoint(i, 128+cosLookupTable[i], 255);
        //plotPoint(i, 128+sineLookupTable[i], 64);

        int32_t c=cos_fix16(i<<16);
        int32_t s=sin_fix16(i<<16);

        plotPoint(i, 128+(c>>9), 255);
        plotPoint(i, 128+(s>>9), 64);
    }
}

void plotLine(int x0, int y0, int x1, int y1, u8 col) {
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    while(1) {
        if (x0==x1 && y0==y1) break;

        if (x0>=0 && x0<320 && y0>=0 && y0<256) *(g_framebuffer + x0 + y0*320) = col;

        int e2 = 2 * error;
        if (e2 >= dy) {
            error += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            error += dx;
            y0 += sy;
        }
    }
}
