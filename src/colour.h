// ============================================================================
// 256 Colour Palette Experiments.
// ============================================================================

#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "archie/SDKTypes.h"

// TODO: Add RGB4 type?
// TODO: Decide if we use RGB or BGR!!

#define COLOUR_MAKE_RGB4(r,g,b)     ((r<<8)|(g<<4)|(b<<0))

void colour_init_palette();
void colour_draw_palette();

u8 colour_rgb4_to_index(u16 rgb);
u16 colour_hsv_to_rgb4(float h, float s, float v);

void colour_make_ramp(u8 *ramp, int size, u16 from, u16 to);

#endif // __COLOUR_H__
