// ============================================================================
// Debug lib.
// ============================================================================

#include "debug.h"
#include "archie/swi.h"
#include "../src/globals.h"     // TODO: Rarr! Libs shouldn't depend on src? :S

INCBIN_EXTERN(debug_font);

#define Debug_MaxGlyphs 96
#define Debug_MaxKeys   32

static u8 debug_font_mode13[Debug_MaxGlyphs * 8 * 8];

struct debug_key {
    u8 key_code;
    key_callback key_func;
    u32 key_param1;
    u32 key_param2;
};

static struct debug_key debug_key_list[Debug_MaxKeys];
static int debug_num_keys;

void debug_init() {
    // Convert 1bpp debug font into MODE-appropriate data for fast copy.

    // Font packed as 8 bytes per glyph: 76543210
    const u8 *debug_font_p = &debug_font_data[0];
    u8 *debug_font_mode13_p = &debug_font_mode13[0];

    // Convert to MODE 13 = 64 bytes per glyph.
    for(int i=0; i < Debug_MaxGlyphs; i++) {
        for(int row=0; row < 8; row++) {
            u8 glyph_byte = *debug_font_p++;
            for(int col=0; col < 8; col++) {
                *debug_font_mode13_p++ = glyph_byte & 0x80 ? 0xff : 0x00;
                glyph_byte <<= 1;
            }
        }
    }
}

void debug_plot_string_mode13(const char *string) {
    u8 *scr_ptr = g_framebuffer;
    char ascii;

    while((ascii = *string++)) {
        int glyph = ascii - 32;
        u8 *debug_glyph_p = &debug_font_mode13[glyph * 64];

        // Plot row. TODO: Inline asm fast plot.
        for(int row = 0; row < 8; row++) {
            *scr_ptr++ = *debug_glyph_p++;
            *scr_ptr++ = *debug_glyph_p++;
            *scr_ptr++ = *debug_glyph_p++;
            *scr_ptr++ = *debug_glyph_p++;
            *scr_ptr++ = *debug_glyph_p++;
            *scr_ptr++ = *debug_glyph_p++;
            *scr_ptr++ = *debug_glyph_p++;
            *scr_ptr++ = *debug_glyph_p++;
            scr_ptr += Screen_Stride - 8;
        }

        scr_ptr += 8 - (Screen_Stride * 8);
    }
}

void debug_write_vidc(u32 vidc_reg) {
    asm volatile("swi " swiToConst(OS_EnterOS) "\n"
                 "mov r1, " swiToConst(VIDC_Write) "\n"
                 "str %0, [r1]\n"
                 "teqp pc, #0\n"
                 "mov r0, r0"
                :            // outputs
                : "r"(vidc_reg)  // inputs 
                : "r0", "r1", "cc"); // clobbers
}

void debug_register_key(u8 key_code, key_callback key_func, u32 key_param1, u32 key_param2) {
    if (debug_num_keys < Debug_MaxKeys) {
        debug_key_list[debug_num_keys].key_code = key_code;
        debug_key_list[debug_num_keys].key_func = key_func;
        debug_key_list[debug_num_keys].key_param1 = key_param1;
        debug_key_list[debug_num_keys++].key_param2 = key_param2;
    }
}

u32 debug_pressed_mask = 0;
u32 debug_prev_mask = 0;

// R1=0 key up or 1 key down
// R2=internal key number (RMKey_*)
void debug_handle_keypress(int key_up_or_down, u32 key_code) {
    for(int i=0; i < debug_num_keys; i++) {
        if (debug_key_list[i].key_code == key_code) {
            if (key_up_or_down) {
                debug_pressed_mask |= (1<<i);
            } else {
                debug_pressed_mask &= ~(1<<i);
            }
            break;
        }
    }
}

void debug_do_keypress_callbacks() {
    u32 diff_bits = (debug_pressed_mask & ~debug_prev_mask) & debug_pressed_mask;
    debug_prev_mask = debug_pressed_mask;

    for(int i=0; i < debug_num_keys; i++) {
        if (diff_bits & (1<<i)) {
            (debug_key_list[i].key_func)(debug_key_list[i].key_param1, debug_key_list[i].key_param2);
        }
    }
}

void debug_toggle_byte(u32 addr, u32 val) {
    (void)val;
    *((u32*)addr) ^= 1;
}

void debug_set_byte(u32 addr, u32 val) {
    *((u32*)addr) = val;
}
