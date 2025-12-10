// ============================================================================
// 256 Colour Palette Experiments.
// ============================================================================

#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "archie/SDKTypes.h"

void MakeArchie256Palette(const u16* basePalette, u16* palette256);
void MakeDefaultPalette();
void plotColours();

u8 rgbToArchie(u16 rgb);
u16 hsvToRgb(float h, float s, float v);

#endif // __PLOT_H__
