// ============================================================================
// PC stub: ArchieSDK keyboard functions.
// main_pc.c handles key events via SDL directly; this stub covers any
// header-only reference to k_checkKeypress / KEY_ESCAPE.
// ============================================================================

#ifndef __ARCHIE_KEYBOARD_H__
#define __ARCHIE_KEYBOARD_H__

#include <SDL2/SDL.h>

#define KEY_ESCAPE SDL_SCANCODE_ESCAPE

static inline int k_checkKeypress(SDL_Scancode sc)
{
    return SDL_GetKeyboardState(NULL)[sc];
}

#endif // __ARCHIE_KEYBOARD_H__
