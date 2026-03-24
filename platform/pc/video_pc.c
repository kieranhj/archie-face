// ============================================================================
// PC implementation of lib/video.h and archie/video.h.
// Triple-buffering and bank switching are not needed on PC; we use a single
// static framebuffer that is blitted to the SDL window each frame.
// ============================================================================

#include "lib/video.h"
#include "src/globals.h"

static u8 s_framebuffer[Screen_SizeBytes];

// ---- lib/video.h -----------------------------------------------------------

void v_setDisplayBank(int bank)              { (void)bank; }
void v_setWriteBank(int bank)                { (void)bank; }
void v_claimEventHandler(event_handler func) { (void)func; }
void v_releaseEventHandler(event_handler func){ (void)func; }
void v_enableEvent(int event_no)             { (void)event_no; }
void v_disableEvent(int event_no)            { (void)event_no; }

u32 v_setScreenMemory(u32 new_size)
{
    (void)new_size;
    return Screen_SizeBytes;
}

// ---- archie/video.h --------------------------------------------------------

u8 *v_getScreenAddress(void)
{
    return s_framebuffer;
}
