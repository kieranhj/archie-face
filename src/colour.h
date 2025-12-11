// ============================================================================
// 256 Colour Palette Experiments.
// ============================================================================

#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "archie/SDKTypes.h"

void colour_init_palette();
void colour_draw_palette();

u8 colour_rgb4_to_index(u16 rgb);
u16 colour_hsv_to_rgb4(float h, float s, float v);

#endif // __COLOUR_H__
