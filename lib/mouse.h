// ============================================================================
// Mouse
// ============================================================================

#ifndef __MOUSE_H__
#define __MOUSE_H__

#include "archie/SDKTypes.h"

void mouseUpdate();
void mouseRead(int *mouseX, int *mouseY, u8 *mouseButtons);

#endif
