// ============================================================================
// PC parameter panel implementation.
// Renders a list of registered float/int parameters as a text overlay in the
// top-right corner of the framebuffer using the existing debug font.
// ============================================================================

#include "platform/pc/params.h"
#include "src/globals.h"
#include "lib/debug.h"

#include <stdio.h>
#include <string.h>

// ============================================================================

#define PARAMS_MAX 32

typedef enum { PARAM_FLOAT, PARAM_INT } param_type_t;

typedef struct {
    char         name[24];
    param_type_t type;
    union {
        float *f;
        int   *i;
    } ptr;
    union {
        struct { float min, max, step; } f;
        struct { int   min, max, step; } i;
    } range;
} param_entry_t;

static param_entry_t s_params[PARAMS_MAX];
static int           s_count;
static int           s_selected;

// ============================================================================

void params_init(void)
{
    s_count    = 0;
    s_selected = 0;
}

void params_clear(void)
{
    s_count    = 0;
    s_selected = 0;
}

void param_float(const char *name, float *ptr, float min, float max, float step)
{
    if (s_count >= PARAMS_MAX) return;
    param_entry_t *e = &s_params[s_count++];
    strncpy(e->name, name, sizeof(e->name) - 1);
    e->name[sizeof(e->name) - 1] = '\0';
    e->type        = PARAM_FLOAT;
    e->ptr.f       = ptr;
    e->range.f.min  = min;
    e->range.f.max  = max;
    e->range.f.step = step;
}

void param_int(const char *name, int *ptr, int min, int max, int step)
{
    if (s_count >= PARAMS_MAX) return;
    param_entry_t *e = &s_params[s_count++];
    strncpy(e->name, name, sizeof(e->name) - 1);
    e->name[sizeof(e->name) - 1] = '\0';
    e->type        = PARAM_INT;
    e->ptr.i       = ptr;
    e->range.i.min  = min;
    e->range.i.max  = max;
    e->range.i.step = step;
}

// ============================================================================

void params_next(void)
{
    if (s_count > 0) s_selected = (s_selected + 1) % s_count;
}

void params_prev(void)
{
    if (s_count > 0) s_selected = (s_selected + s_count - 1) % s_count;
}

void params_inc(void)
{
    if (s_count == 0) return;
    param_entry_t *e = &s_params[s_selected];
    if (e->type == PARAM_FLOAT) {
        *e->ptr.f += e->range.f.step;
        if (*e->ptr.f > e->range.f.max) *e->ptr.f = e->range.f.max;
    } else {
        *e->ptr.i += e->range.i.step;
        if (*e->ptr.i > e->range.i.max) *e->ptr.i = e->range.i.max;
    }
}

void params_dec(void)
{
    if (s_count == 0) return;
    param_entry_t *e = &s_params[s_selected];
    if (e->type == PARAM_FLOAT) {
        *e->ptr.f -= e->range.f.step;
        if (*e->ptr.f < e->range.f.min) *e->ptr.f = e->range.f.min;
    } else {
        *e->ptr.i -= e->range.i.step;
        if (*e->ptr.i < e->range.i.min) *e->ptr.i = e->range.i.min;
    }
}

// ============================================================================
// Text rendering — draw each param as one line using the debug font.
// Uses debug_plot_string_at() which is exposed under PLATFORM_PC in debug.h.
// Panel is anchored to the top-right corner; selected entry is highlighted
// by inverting the glyph colour (0xff background becomes 0x00, and vice versa).
// ============================================================================

// Characters per display line, including name + "= " + value.
#define LINE_CHARS 30
// X pixel position of the panel (from right edge).
#define PANEL_X (Screen_Width - LINE_CHARS * 8)
// Vertical spacing between lines in pixels (one font row = 8px).
#define LINE_H 9

void params_draw(void)
{
    if (s_count == 0) return;

    for (int idx = 0; idx < s_count; idx++)
    {
        param_entry_t *e = &s_params[idx];
        char line[LINE_CHARS + 1];
        char val[12];

        if (e->type == PARAM_FLOAT)
            snprintf(val, sizeof(val), "%.4f", *e->ptr.f);
        else
            snprintf(val, sizeof(val), "%d", *e->ptr.i);

        snprintf(line, sizeof(line), "%-16s%s", e->name, val);

        int y = idx * LINE_H;

        if (idx == s_selected)
            debug_plot_string_at_inv(line, PANEL_X, y);
        else
            debug_plot_string_at(line, PANEL_X, y);
    }
}
