// ============================================================================
// Plot functions.
// NB. Currently for MODE 13 only.
// ============================================================================

#include "plot.h"
#include "../src/globals.h"     // TODO: Rarr! Libs shouldn't depend on src? :S
#include "trig.h"

#include <stdlib.h>
#include <stdio.h>

void plot_point(register int x, register int y, register u8 c)
{
    if (x>=0 && x<320 && y>=0 && y<256) *(g_framebuffer + x + y*320) = c;
}

void plot_block(register int x, register int y, register u8 c)
{
    int x0=x&~0x3, y0=y&~0x3;
    u32 cw=c|(c<<8);cw=cw|(cw<<16);

    u32* fp=(u32 *)(g_framebuffer + x0 + y0*320);

    *fp = cw; fp+=80;
    *fp = cw; fp+=80;
    *fp = cw; fp+=80;
    *fp = cw;
}

void plot_line(int x0, int y0, int x1, int y1, u8 col)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    while(1)
    {
        if (x0==x1 && y0==y1) break;
        if (x0>=0 && x0<320 && y0>=0 && y0<256) *(g_framebuffer + x0 + y0*320) = col;

        int e2 = 2 * error;
        if (e2 >= dy)
        {
            error += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            error += dx;
            y0 += sy;
        }
    }
}
