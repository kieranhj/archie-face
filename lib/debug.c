// ============================================================================
// Debug lib.
// ============================================================================

#include "archie/SDKTypes.h"
#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#define INCBIN_PREFIX
#include "incbin.h"

INCBIN_EXTERN(debug_font);

extern u8* framebuffer;     // TODO: Where does this get defined?
#define Screen_Stride       320 // 8bpp

#define Debug_MaxGlyphs 96

static u8 debug_font_mode13[Debug_MaxGlyphs * 8 * 8];

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
    u8 *scr_ptr = framebuffer;
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
