// ============================================================================
// Mouse
// ============================================================================

#include "mouse.h"
#include "archie/swi.h"

static int MouseX;
static int MouseY;
static u8 MouseButtons;

void mouse_read(int *mouseX, int *mouseY, u8 *mouseButtons)
{
    *mouseX = MouseX;
    *mouseY = MouseY;
    *mouseButtons = MouseButtons;
}

void mouse_tick()
{
    int x, y, b;
    asm volatile("swi " swiToConst(OS_Mouse) "\n"
                 "mov %0, r0\n"
                 "mov %1, r1\n"
                 "mov %2, r2\n"
                : "=r"(x), "=r"(y), "=r"(b)          // outputs
                : 
                : "r0", "r1", "r2", "cc"); // clobbers
    MouseX = x/4;
    MouseY = (1023-y)/4;
    MouseButtons = b;
}
