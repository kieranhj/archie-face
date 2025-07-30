// ============================================================================
// Debug lib.
// ============================================================================

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "archie.h"

#define SET_BORDER(bgr) debug_write_vidc(VIDC_Border | (bgr))

void debug_init();
void debug_plot_string_mode13(const char *string);
void debug_write_vidc(u32 vidc_reg);

#endif // __DEBUG_H__
