// ============================================================================
// Mouse
// ============================================================================

#include "mouse.h"
#include "archie/swi.h"

void mouseRead(int *mouseX, int *mouseY, u8 *mouseButtons) {
    int x, y, b;
    asm volatile("swi " swiToConst(OS_Mouse) "\n"
                 "mov %0, r0\n"
                 "mov %1, r1\n"
                 "mov %2, r2\n"
                : "=r"(x), "=r"(y), "=r"(b)          // outputs
                : 
                : "r0", "r1", "r2", "cc"); // clobbers
    *mouseX = x/4;
    *mouseY = (1023-y)/4;
    *mouseButtons = b;
}
