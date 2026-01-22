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

extern u8* g_framebuffer;           // Pointer to current framebuffer for drawing.
extern int g_frame_count;           // Number of frames displayed.
extern volatile int g_vsync_count;  // Number of vsyncs since launch.
extern u32 g_debug_rasters;         // Debug raster timing enabled.

#endif // __GLOBALS_H__
