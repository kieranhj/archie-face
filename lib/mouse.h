// ============================================================================
// Mouse
// ============================================================================

#ifndef __MOUSE_H__
#define __MOUSE_H__

#include "archie/SDKTypes.h"

void mouse_tick();
void mouse_read(int *mouseX, int *mouseY, u8 *mouseButtons);
void mouse_cursor(int on);

#endif
