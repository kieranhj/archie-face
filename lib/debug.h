// ============================================================================
// Debug lib.
// ============================================================================

#ifndef __DEBUG_H__
#define __DEBUG_H__

#define VIDC_Col0 0x00000000
#define VIDC_Col1 0x04000000              // index << 26
#define VIDC_Col2 0x08000000
#define VIDC_Col3 0x0c000000
#define VIDC_Col4 0x10000000
#define VIDC_Col5 0x14000000
#define VIDC_Col6 0x18000000
#define VIDC_Col7 0x1c000000
#define VIDC_Col8 0x20000000
#define VIDC_Col9 0x24000000
#define VIDC_Col10 0x28000000
#define VIDC_Col11 0x2c000000
#define VIDC_Col12 0x30000000
#define VIDC_Col13 0x34000000
#define VIDC_Col14 0x38000000
#define VIDC_Col15 0x3c000000
#define VIDC_Border 0x40000000

#define SET_BORDER(bgr) debug_write_vidc(VIDC_Border | (bgr))

void debug_init();
void debug_plot_string_mode13(const char *string);
void debug_write_vidc(u32 vidc_reg);

#endif // __DEBUG_H__
