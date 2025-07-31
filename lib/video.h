// ============================================================================
// Video lib (probably belongs in ArchieSDK.)
// ============================================================================

#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "archie/SDKTypes.h"

typedef void (*event_handler)(int event_no, int event_param1, int event_param2, int event_param3, int event_param4);

void v_setDisplayBank(int bank);
void v_setWriteBank(int bank);

void v_claimEventHandler(event_handler func);
void v_releaseEventHandler(event_handler func);

void v_enableEvent(int event_no);
void v_disableEvent(int event_no);

u32 v_setScreenMemory(u32 new_size);

#endif // __VIDEO_H__
