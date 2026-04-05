// ============================================================================
// Nuklear implementation — single compilation unit.
//
// nuklear.h and the SDL2 renderer backend are header-only libraries; exactly
// one .c file must define NK_IMPLEMENTATION / NK_SDL_RENDERER_IMPLEMENTATION
// so that the function bodies are compiled once.
// ============================================================================

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#include "nuklear/nuklear.h"

#define NK_SDL_RENDERER_SDL_H <SDL2/SDL.h>
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear/nuklear_sdl_renderer.h"
