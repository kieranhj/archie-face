// ============================================================================
// Live parameter tweaking — PC only.
//
// Call param_float() / param_int() from a seq_partN_init() to expose knobs.
// On Archimedes all calls compile to nothing, so the same seq-parts code
// builds for both targets.
//
// Navigation keys (PC):
//   Tab / Shift+Tab  — next / previous parameter
//   ]                — increase selected parameter by one step
//   [                — decrease selected parameter by one step
// ============================================================================

#pragma once

#ifdef PLATFORM_PC

void params_init(void);

// Call params_clear() at the top of each seq_partN_init() so stale entries
// from the previous part are removed.
void params_clear(void);

void param_float(const char *name, float *ptr, float min, float max, float step);
void param_int  (const char *name, int   *ptr, int   min, int   max, int   step);

// Called from main_pc.c in response to navigation keypresses.
void params_next(void);
void params_prev(void);
void params_inc (void);
void params_dec (void);

// Render the parameter panel into g_framebuffer.  Call after sequence_draw().
void params_draw(void);

#else // ---- Archimedes no-ops -----------------------------------------------

#define params_init()
#define params_clear()
#define param_float(n, p, lo, hi, s)  ((void)0)
#define param_int(n,   p, lo, hi, s)  ((void)0)

#endif // PLATFORM_PC
