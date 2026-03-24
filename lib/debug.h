// ============================================================================
// Debug lib.
// ============================================================================

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "archie/SDKTypes.h"
#include "archie.h"
#include <stdint.h>

// uintptr_t is pointer-sized: 32 bits on Archimedes, 64 bits on PC.
// Using it for callback params lets callers safely pass variable addresses
// via (uintptr_t)&var without truncation on either target.
typedef void (*key_callback)(uintptr_t key_param1, uintptr_t key_param2);

#define SET_BORDER(bgr) if(g_debug_rasters) debug_write_vidc(VIDC_Border | (bgr))

void debug_init();
void debug_plot_string_mode13(const char *string);
void debug_write_vidc(u32 vidc_reg);
void debug_register_key(u8 key_code, key_callback key_func, uintptr_t key_param1, uintptr_t key_param2);
void debug_handle_keypress(int key_up_or_down, u32 key_code);
void debug_do_keypress_callbacks();
void debug_toggle_word(uintptr_t addr, uintptr_t val);
void debug_set_word(uintptr_t addr, uintptr_t val);
void debug_word_add(uintptr_t addr, uintptr_t val);

// PC-only: position-aware string render for the params panel overlay.
#ifdef PLATFORM_PC
void debug_plot_string_at(const char *string, int x, int y);
void debug_plot_string_at_inv(const char *string, int x, int y);
#endif

#endif // __DEBUG_H__
