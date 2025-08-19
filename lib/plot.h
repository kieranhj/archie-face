// ============================================================================
// Plot functions.
// ============================================================================

#ifndef __PLOT_H__
#define __PLOT_H__

#include "archie/SDKTypes.h"

void plotPoint(register int x, register int y, register u8 c);
void plotLine(int x0, int y0, int x1, int y1, u8 col);

void plotSinCos();  // debug

#endif // __PLOT_H__
