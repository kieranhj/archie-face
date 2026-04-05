// ============================================================================
// PC main loop — replaces main.c for the SDL2 build.
//
// Display:  8bpp framebuffer converted to ARGB each frame, uploaded to an
//           SDL_Texture and rendered scaled into the window via SDL_Renderer.
//           Nuklear UI is rendered on top at native window resolution.
// Timing:   software-capped to TARGET_HZ (50 Hz).
// Keys:     SDL events → RMKey_* values → debug_handle_keypress()
//           so that all debug_register_key() callbacks work identically
//           to the Archimedes build.
// Params:   draggable Nuklear panel with sliders / numeric properties.
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

// PC archie stubs (for v_getScreenAddress).
#include "archie/video.h"

// PC platform.
#include "platform/pc/params.h"

// Nuklear (header-only; implementation lives in nuklear_impl.c).
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear/nuklear.h"
#define NK_SDL_RENDERER_SDL_H <SDL2/SDL.h>
#include "nuklear/nuklear_sdl_renderer.h"

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
static SDL_Renderer *s_renderer;
static SDL_Texture  *s_texture;         // ARGB8888, 320×256, streaming

static struct nk_context *s_nk;         // Nuklear context

// Per-frame ARGB conversion buffer — 8bpp indices → 32bpp via archie256[].
static u32 s_pixels32[Screen_Width * Screen_Height];

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
// Frame present — convert 8bpp framebuffer to ARGB and push to texture.
// Does NOT call SDL_RenderPresent — Nuklear renders on top first.
// ============================================================================

static void pc_present_frame(void)
{
    const u16 *archie256 = colour_get_archie256();

    for (int i = 0; i < Screen_Width * Screen_Height; i++) {
        u16 rgb4 = archie256[g_framebuffer[i]];
        u8  r    = (u8)(((rgb4 >> 8) & 0xf) * 17);
        u8  g    = (u8)(((rgb4 >> 4) & 0xf) * 17);
        u8  b    = (u8)(((rgb4 >> 0) & 0xf) * 17);
        s_pixels32[i] = (0xFF000000u) | ((u32)r << 16) | ((u32)g << 8) | b;
    }

    SDL_UpdateTexture(s_texture, NULL, s_pixels32, Screen_Width * (int)sizeof(u32));
    SDL_RenderClear(s_renderer);
    SDL_RenderCopy(s_renderer, s_texture, NULL, NULL);
    // SDL_RenderPresent is called after Nuklear renders on top.
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

    s_renderer = SDL_CreateRenderer(s_window, -1, SDL_RENDERER_ACCELERATED);
    if (!s_renderer) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(s_window);
        SDL_Quit();
        return 1;
    }

    // Nearest-neighbour scaling keeps pixels sharp at integer multiples.
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    s_texture = SDL_CreateTexture(
        s_renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        Screen_Width, Screen_Height);
    if (!s_texture) {
        fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(s_renderer);
        SDL_DestroyWindow(s_window);
        SDL_Quit();
        return 1;
    }

    // ---- Nuklear init ------------------------------------------------------
    s_nk = nk_sdl_init(s_window, s_renderer);
    {
        struct nk_font_atlas *atlas;
        nk_sdl_font_stash_begin(&atlas);
        // NULL = use built-in default font.
        nk_sdl_font_stash_end();
    }

    // ---- Demo init ---------------------------------------------------------
    colour_init_palette();
    trig_init();
    debug_init();
    params_init(s_nk);

    // Note: toggle callbacks (D/R/S/Space) are handled directly in the event
    // loop below — using debug_register_key with (u32)&ptr would truncate the
    // 64-bit address and segfault on dereference.
    // sequence_init() registers arrow-key callbacks with param=0, which is safe.
    sequence_init();

    g_framebuffer = v_getScreenAddress();   // static buffer in video_pc.c

    // ---- Main loop ---------------------------------------------------------
    int running = 1;
    while (running)
    {
        Uint32 frame_start = SDL_GetTicks();

        // ---- Events --------------------------------------------------------
        nk_input_begin(s_nk);
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) { running = 0; break; }

            // Let Nuklear see every event for widget interaction.
            nk_sdl_handle_event(&e);

            if (e.type == SDL_KEYDOWN) {
                SDL_Scancode sc = e.key.keysym.scancode;

                if (sc == SDL_SCANCODE_ESCAPE) { running = 0; break; }

                // Toggle keys handled directly — avoids (u32)&ptr truncation
                // on 64-bit which segfaults inside debug_toggle_word.
                switch (sc) {
                    case SDL_SCANCODE_D:     s_debug_display ^= 1; break;
                    case SDL_SCANCODE_R:     g_debug_rasters ^= 1; break;
                    case SDL_SCANCODE_SPACE: s_debug_do_tick ^= 1; break;
                    case SDL_SCANCODE_S:     s_debug_step     = 1; break;
                    default: break;
                }

                // Forward to debug system for sequence.c's arrow-key callbacks.
                u8 rmkey = sdl_to_rmkey(sc);
                if (rmkey) debug_handle_keypress(1, rmkey);
            }
            if (e.type == SDL_KEYUP) {
                u8 rmkey = sdl_to_rmkey(e.key.keysym.scancode);
                if (rmkey) debug_handle_keypress(0, rmkey);
            }
        }
        nk_input_end(s_nk);
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
        mem_set_fast((u32 *)g_framebuffer, 0, Screen_SizeBytes);
        sequence_draw();

        if (s_debug_display) {
            char info[32];
            snprintf(info, sizeof(info), "PC %d", seq_part_no);
            debug_plot_string_mode13(info);
        }

        // ---- Present -------------------------------------------------------
        // 1. Upload 8bpp framebuffer as scaled texture.
        pc_present_frame();

        // 2. Build & render Nuklear UI on top at native window resolution.
        params_draw();
        nk_sdl_render(NK_ANTI_ALIASING_ON);

        // 3. Flip.
        SDL_RenderPresent(s_renderer);

        // ---- Frame cap -----------------------------------------------------
        Uint32 elapsed = SDL_GetTicks() - frame_start;
        if (elapsed < TARGET_MS)
            SDL_Delay(TARGET_MS - elapsed);
    }

    // ---- Shutdown ----------------------------------------------------------
    sequence_kill();
    nk_sdl_shutdown();
    SDL_DestroyTexture(s_texture);
    SDL_DestroyRenderer(s_renderer);
    SDL_DestroyWindow(s_window);
    SDL_Quit();
    return 0;
}
