// ============================================================================
// PC implementation of lib/mouse.h via SDL2.
// ============================================================================

#include "lib/mouse.h"
#include <SDL2/SDL.h>

// Must match the scale factor used in main_pc.c.
#define PC_SCALE 3

static int s_mouse_x;
static int s_mouse_y;
static u8  s_mouse_buttons;

void mouse_tick(void)
{
    int wx, wy;
    u32 b        = SDL_GetMouseState(&wx, &wy);
    s_mouse_x    = wx / PC_SCALE;
    s_mouse_y    = wy / PC_SCALE;
    // Map SDL button mask to a byte (matches RISC OS button ordering well enough).
    s_mouse_buttons = (u8)(b & 0x07);
}

void mouse_read(int *mouseX, int *mouseY, u8 *mouseButtons)
{
    *mouseX       = s_mouse_x;
    *mouseY       = s_mouse_y;
    *mouseButtons = s_mouse_buttons;
}

void mouse_cursor(int on)
{
    SDL_ShowCursor(on ? SDL_ENABLE : SDL_DISABLE);
}
