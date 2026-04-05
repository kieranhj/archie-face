// ============================================================================
// PC parameter panel — Nuklear-based implementation.
//
// Each registered float becomes a slider, each int becomes a numeric property.
// The panel is a draggable, collapsible Nuklear window anchored top-right.
// ============================================================================

#include "platform/pc/params.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear/nuklear.h"

#include "src/globals.h"

#include <stdio.h>
#include <string.h>

// ============================================================================

#define PARAMS_MAX   32
#define PANEL_WIDTH  280
#define PANEL_HEIGHT 400

typedef enum { PARAM_FLOAT, PARAM_INT, PARAM_BOOL } param_type_t;

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

static param_entry_t      s_params[PARAMS_MAX];
static int                s_count;
static struct nk_context *s_nk;

// ============================================================================

void params_init(struct nk_context *ctx)
{
    s_nk    = ctx;
    s_count = 0;
}

void params_clear(void)
{
    s_count = 0;
}

// Return existing entry with this name, or allocate a new slot.
static param_entry_t *param_find_or_add(const char *name)
{
    for (int i = 0; i < s_count; i++)
        if (strncmp(s_params[i].name, name, sizeof(s_params[i].name)) == 0)
            return &s_params[i];

    if (s_count >= PARAMS_MAX) return NULL;
    param_entry_t *e = &s_params[s_count++];
    strncpy(e->name, name, sizeof(e->name) - 1);
    e->name[sizeof(e->name) - 1] = '\0';
    return e;
}

void param_float(const char *name, float *ptr, float min, float max, float step)
{
    param_entry_t *e = param_find_or_add(name);
    if (!e) return;
    e->type        = PARAM_FLOAT;
    e->ptr.f       = ptr;
    e->range.f.min  = min;
    e->range.f.max  = max;
    e->range.f.step = step;
}

void param_int(const char *name, int *ptr, int min, int max, int step)
{
    param_entry_t *e = param_find_or_add(name);
    if (!e) return;
    e->type        = PARAM_INT;
    e->ptr.i       = ptr;
    e->range.i.min  = min;
    e->range.i.max  = max;
    e->range.i.step = step;
}

void param_bool(const char *name, int *ptr)
{
    param_entry_t *e = param_find_or_add(name);
    if (!e) return;
    e->type  = PARAM_BOOL;
    e->ptr.i = ptr;
}

void param_remove(const char *name)
{
    for (int i = 0; i < s_count; i++) {
        if (strncmp(s_params[i].name, name, sizeof(s_params[i].name)) == 0) {
            s_params[i] = s_params[--s_count];
            return;
        }
    }
}

// ============================================================================

void params_draw(void)
{
    if (!s_nk || s_count == 0) return;

    // Anchor the panel to the top-right of the window.
    int win_w = Screen_Width * 3;   // PC_SCALE
    struct nk_rect bounds = nk_rect(
        (float)(win_w - PANEL_WIDTH - 10), 10,
        PANEL_WIDTH, PANEL_HEIGHT);

    if (nk_begin(s_nk, "Parameters", bounds,
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE |
                 NK_WINDOW_SCALABLE | NK_WINDOW_TITLE |
                 NK_WINDOW_MINIMIZABLE))
    {
        for (int i = 0; i < s_count; i++)
        {
            param_entry_t *e = &s_params[i];
            char val[16];

            if (e->type == PARAM_BOOL)
            {
                nk_layout_row_dynamic(s_nk, 20, 1);
                nk_checkbox_label(s_nk, e->name, e->ptr.i);
            }
            else
            {
                // Name and current value on one line.
                if (e->type == PARAM_FLOAT)
                    snprintf(val, sizeof(val), "%.4f", *e->ptr.f);
                else
                    snprintf(val, sizeof(val), "%d", *e->ptr.i);

                nk_layout_row_dynamic(s_nk, 16, 2);
                nk_label(s_nk, e->name, NK_TEXT_LEFT);
                nk_label(s_nk, val, NK_TEXT_RIGHT);

                // Slider.
                nk_layout_row_dynamic(s_nk, 20, 1);

                if (e->type == PARAM_FLOAT)
                {
                    nk_slider_float(s_nk,
                        e->range.f.min, e->ptr.f, e->range.f.max,
                        e->range.f.step);
                }
                else
                {
                    int v = *e->ptr.i;
                    nk_slider_int(s_nk,
                        e->range.i.min, &v, e->range.i.max,
                        e->range.i.step);
                    *e->ptr.i = v;
                }
            }
        }
    }
    nk_end(s_nk);
}
