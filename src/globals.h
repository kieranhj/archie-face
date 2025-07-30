// ============================================================================
// Globals for this specific app - defines, includes, constants etc.
// ============================================================================

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "archie/SDKTypes.h"

#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#define INCBIN_PREFIX
#include "../lib/incbin.h"

#define Screen_Mode         13
#define Screen_Width        320
#define Screen_Height       256
#define Screen_Stride       320 // 8bpp
#define Screen_SizeBytes    (Screen_Stride*Screen_Height)
#define Screen_Banks        3
#define Screen_SizeTotal    (Screen_SizeBytes * Screen_Banks)

extern u8* g_framebuffer;

#endif // __GLOBALS_H__
