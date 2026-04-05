// ============================================================================
// Live parameter tweaking — PC only.
//
// Call param_float() / param_int() from a seq_partN_init() to expose knobs.
// On Archimedes all calls compile to nothing, so the same seq-parts code
// builds for both targets.
//
// On PC the parameters are rendered as a draggable Nuklear panel with sliders
// and numeric properties.  Mouse interaction is handled by Nuklear directly.
// ============================================================================

#pragma once

#ifdef PLATFORM_PC

struct nk_context;  // forward decl — avoids pulling nuklear.h into every user

void params_init(struct nk_context *ctx);

// Call params_clear() at the top of each seq_partN_init() so stale entries
// from the previous part are removed.
void params_clear(void);

void param_float(const char *name, float *ptr, float min, float max, float step);
void param_int  (const char *name, int   *ptr, int   min, int   max, int   step);

// Build the Nuklear parameter panel.  Call each frame between nk_input_end()
// and nk_sdl_render().
void params_draw(void);

#else // ---- Archimedes no-ops -----------------------------------------------

#define params_init(ctx)
#define params_clear()
#define param_float(n, p, lo, hi, s)  ((void)0)
#define param_int(n,   p, lo, hi, s)  ((void)0)

#endif // PLATFORM_PC
