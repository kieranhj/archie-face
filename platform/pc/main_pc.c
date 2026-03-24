// ============================================================================
// PC main loop — replaces main.c for the SDL2 build.
//
// Display:  320x256 8bpp surface blitted at PC_SCALE× into an SDL window.
// Palette:  derived from archie256[] built by colour_init_palette().
// Timing:   software-capped to TARGET_HZ (50 Hz).
// Keys:     SDL events → RMKey_* values → debug_handle_keypress()
//           so that all debug_register_key() callbacks work identically
//           to the Archimedes build.
// Params:   Tab/Shift+Tab to select, ]/[ to adjust live parameters.
// ============================================================================

// App modules.
#include "src/globals.h"
#include "src/colour.h"
#include "src/sequence.h"

// Lib.
#include "lib/debug.h"
#include "lib/mem.h"
#include "lib/mouse.h"
#include "lib/trig.h"
#include "lib/video.h"
#include "lib/archie.h"

// PC platform.
#include "platform/pc/params.h"

// SDL2.
#include <SDL2/SDL.h>

// C.
#include <stdio.h>
#include <string.h>

// ============================================================================
// Configuration
// ============================================================================

#define PC_SCALE    3                           // window = 960×768
#define TARGET_HZ   50
#define TARGET_MS   (1000 / TARGET_HZ)          // 20 ms per frame

// ============================================================================
// Globals declared extern in src/globals.h
// ============================================================================

u8            *g_framebuffer  = NULL;
int            g_frame_count  = 0;
volatile int   g_vsync_count  = 0;
u32            g_debug_rasters = 1;

// ============================================================================
// Internal state
// ============================================================================

static SDL_Window   *s_window;
static SDL_Surface  *s_screen;          // INDEX8, 320×256
static SDL_Surface  *s_window_surface;

static u32  s_debug_display  = 1;
static u32  s_debug_do_tick  = 1;
static u32  s_debug_step     = 0;

// ============================================================================
// SDL → RMKey translation
// ============================================================================

typedef struct { SDL_Scancode sdl; u8 rmkey; } key_map_t;

static const key_map_t s_key_map[] = {
    { SDL_SCANCODE_A,     RMKey_A     },
    { SDL_SCANCODE_B,     RMKey_B     },
    { SDL_SCANCODE_C,     RMKey_C     },
    { SDL_SCANCODE_D,     RMKey_D     },
    { SDL_SCANCODE_E,     RMKey_E     },
    { SDL_SCANCODE_F,     RMKey_F     },
    { SDL_SCANCODE_G,     RMKey_G     },
    { SDL_SCANCODE_H,     RMKey_H     },
    { SDL_SCANCODE_I,     RMKey_I     },
    { SDL_SCANCODE_J,     RMKey_J     },
    { SDL_SCANCODE_K,     RMKey_K     },
    { SDL_SCANCODE_L,     RMKey_L     },
    { SDL_SCANCODE_M,     RMKey_M     },
    { SDL_SCANCODE_N,     RMKey_N     },
    { SDL_SCANCODE_O,     RMKey_O     },
    { SDL_SCANCODE_P,     RMKey_P     },
    { SDL_SCANCODE_Q,     RMKey_Q     },
    { SDL_SCANCODE_R,     RMKey_R     },
    { SDL_SCANCODE_S,     RMKey_S     },
    { SDL_SCANCODE_T,     RMKey_T     },
    { SDL_SCANCODE_U,     RMKey_U     },
    { SDL_SCANCODE_V,     RMKey_V     },
    { SDL_SCANCODE_W,     RMKey_W     },
    { SDL_SCANCODE_X,     RMKey_X     },
    { SDL_SCANCODE_Y,     RMKey_Y     },
    { SDL_SCANCODE_Z,     RMKey_Z     },
    { SDL_SCANCODE_SPACE, RMKey_Space },
    { SDL_SCANCODE_RETURN,RMKey_Return},
    { SDL_SCANCODE_LEFT,  RMKey_ArrowLeft  },
    { SDL_SCANCODE_RIGHT, RMKey_ArrowRight },
    { SDL_SCANCODE_UP,    RMKey_ArrowUp    },
    { SDL_SCANCODE_DOWN,  RMKey_ArrowDown  },
};
static const int s_key_map_size = (int)(sizeof(s_key_map) / sizeof(s_key_map[0]));

static u8 sdl_to_rmkey(SDL_Scancode sc)
{
    for (int i = 0; i < s_key_map_size; i++)
        if (s_key_map[i].sdl == sc) return s_key_map[i].rmkey;
    return 0;
}

// ============================================================================
// Palette
// ============================================================================

static void pc_set_sdl_palette(void)
{
    const u16   *archie256 = colour_get_archie256();
    SDL_Color    sdl_pal[256];

    for (int i = 0; i < 256; i++) {
        u16 rgb4    = archie256[i];
        // Each nibble is a 4-bit channel; scale to 8-bit by multiplying by 17.
        sdl_pal[i].r = (u8)(((rgb4 >> 8) & 0xf) * 17);
        sdl_pal[i].g = (u8)(((rgb4 >> 4) & 0xf) * 17);
        sdl_pal[i].b = (u8)(((rgb4 >> 0) & 0xf) * 17);
        sdl_pal[i].a = 255;
    }

    SDL_SetPaletteColors(s_screen->format->palette, sdl_pal, 0, 256);
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // ---- SDL init ----------------------------------------------------------
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    s_window = SDL_CreateWindow(
        "archie-face (PC)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        Screen_Width  * PC_SCALE,
        Screen_Height * PC_SCALE,
        0);
    if (!s_window) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    s_window_surface = SDL_GetWindowSurface(s_window);

    // 8bpp palettized surface — exact semantic match for the Archimedes framebuffer.
    s_screen = SDL_CreateRGBSurfaceWithFormat(
        0, Screen_Width, Screen_Height, 8, SDL_PIXELFORMAT_INDEX8);
    if (!s_screen) {
        fprintf(stderr, "SDL_CreateRGBSurfaceWithFormat: %s\n", SDL_GetError());
        SDL_DestroyWindow(s_window);
        SDL_Quit();
        return 1;
    }

    // ---- Demo init ---------------------------------------------------------
    colour_init_palette();
    pc_set_sdl_palette();
    trig_init();
    debug_init();
    params_init();

    // Mirror the debug key registrations from main.c.
    debug_register_key(RMKey_D,     debug_toggle_word, (u32)&s_debug_display, 0);
    debug_register_key(RMKey_R,     debug_toggle_word, (u32)&g_debug_rasters, 0);
    debug_register_key(RMKey_S,     debug_set_word,    (u32)&s_debug_step, 1);
    debug_register_key(RMKey_Space, debug_toggle_word, (u32)&s_debug_do_tick, 0);

    sequence_init();

    g_framebuffer = (u8 *)s_screen->pixels;

    // ---- Main loop ---------------------------------------------------------
    int running = 1;
    while (running)
    {
        Uint32 frame_start = SDL_GetTicks();

        // ---- Events --------------------------------------------------------
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) {
                running = 0;
                break;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    running = 0;
                    break;
                }
                // Params navigation (PC-only, bypass debug key system).
                if (e.key.keysym.scancode == SDL_SCANCODE_TAB) {
                    if (e.key.keysym.mod & KMOD_SHIFT) params_prev();
                    else                                params_next();
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_RIGHTBRACKET) params_inc();
                if (e.key.keysym.scancode == SDL_SCANCODE_LEFTBRACKET)  params_dec();

                // Demo control keys → RMKey dispatch.
                u8 rmkey = sdl_to_rmkey(e.key.keysym.scancode);
                if (rmkey) debug_handle_keypress(1, rmkey);
            }
            if (e.type == SDL_KEYUP) {
                u8 rmkey = sdl_to_rmkey(e.key.keysym.scancode);
                if (rmkey) debug_handle_keypress(0, rmkey);
            }
        }
        if (!running) break;

        // ---- Tick ----------------------------------------------------------
        mouse_tick();
        debug_do_keypress_callbacks();

        if (s_debug_do_tick || s_debug_step) {
            s_debug_step = 0;
            sequence_tick();
            g_frame_count++;
            g_vsync_count++;
        }

        // ---- Draw ----------------------------------------------------------
        SDL_LockSurface(s_screen);

        mem_set_fast((u32 *)g_framebuffer, 0, Screen_SizeBytes);
        sequence_draw();

        if (s_debug_display) {
            char info[32];
            snprintf(info, sizeof(info), "PC %d", seq_part_no);
            debug_plot_string_mode13(info);
        }

        params_draw();

        SDL_UnlockSurface(s_screen);

        // ---- Present -------------------------------------------------------
        SDL_Rect dst = { 0, 0, Screen_Width * PC_SCALE, Screen_Height * PC_SCALE };
        SDL_BlitScaled(s_screen, NULL, s_window_surface, &dst);
        SDL_UpdateWindowSurface(s_window);

        // ---- Frame cap -----------------------------------------------------
        Uint32 elapsed = SDL_GetTicks() - frame_start;
        if (elapsed < TARGET_MS)
            SDL_Delay(TARGET_MS - elapsed);
    }

    // ---- Shutdown ----------------------------------------------------------
    sequence_kill();
    SDL_FreeSurface(s_screen);
    SDL_DestroyWindow(s_window);
    SDL_Quit();
    return 0;
}
