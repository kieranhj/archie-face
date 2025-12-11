// ============================================================================
// Plot functions.
// NB. Currently for MODE 13 only.
// ============================================================================

#ifndef __PLOT_H__
#define __PLOT_H__

#include "archie/SDKTypes.h"

void plot_point(register int x, register int y, register u8 c);
void plot_line(int x0, int y0, int x1, int y1, u8 col);
void plot_block(int x, int y, u8 col);

#endif // __PLOT_H__
