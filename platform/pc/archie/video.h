// ============================================================================
// PC stub: ArchieSDK video functions.
// v_getScreenAddress() is implemented in platform/pc/video_pc.c.
// All others are no-ops — the PC build uses a single static framebuffer.
// ============================================================================

#ifndef __ARCHIE_VIDEO_H__
#define __ARCHIE_VIDEO_H__

#include "SDKTypes.h"

static inline void v_setMode(int mode)       { (void)mode; }
static inline void v_disableTextCursor(void) {}
static inline void v_waitForVSync(void)      {}

u8 *v_getScreenAddress(void);

#endif // __ARCHIE_VIDEO_H__
