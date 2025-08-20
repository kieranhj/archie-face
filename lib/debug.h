// ============================================================================
// Debug lib.
// ============================================================================

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "archie/SDKTypes.h"
#include "archie.h"

typedef void (*key_callback)(u32 key_param1, u32 key_param2);

#define SET_BORDER(bgr) if(debug_rasters) debug_write_vidc(VIDC_Border | (bgr))

void debug_init();
void debug_plot_string_mode13(const char *string);
void debug_write_vidc(u32 vidc_reg);
void debug_register_key(u8 key_code, key_callback key_func, u32 key_param1, u32 key_param2);
void debug_handle_keypress(int key_up_or_down, u32 key_code);
void debug_do_keypress_callbacks();
void debug_toggle_word(u32 addr, u32 val);
void debug_set_word(u32 addr, u32 val);
void debug_word_add(u32 addr, u32 val);

#endif // __DEBUG_H__
