// ============================================================================
// Flow field.
// ============================================================================

#ifndef __FLOW_FIELD_H__
#define __FLOW_FIELD_H__

#include "archie/SDKTypes.h"
#include "../lib/maths.h"

typedef struct flow_field_s flow_field_t;

extern u32 flow_field_show_grid;

void flow_field_init(flow_field_t *debug_grid);
flow_field_t *flow_field_make(int cols, int rows);
flow_field_t *flow_field_make_ext(int cols, int rows, int screen_width, int screen_height);
void *flow_field_kill(flow_field_t *grid);

void flow_field_init_with_noise(flow_field_t *grid, float smoothing);
void flow_field_init_with_angle(flow_field_t *grid, fix16_t angle);

void flow_field_draw(flow_field_t *grid);
void flow_field_draw_grid(flow_field_t *grid);
void flow_field_draw_curve(flow_field_t *grid, int x0, int y0, int num_steps, int col);

void flow_field_insert_attractor(flow_field_t *grid, int x, int y, int radius);
void flow_field_insert_vortex(flow_field_t *grid, int x, int y, int fx, int fy, int radius);

int flow_field_get_nearest_angle(flow_field_t *grid, fix16_t x, fix16_t y, fix16_t *a);
void flow_field_set_angle(flow_field_t *grid, int i, int j, fix16_t a);
fix16_t flow_field_get_angle(flow_field_t *grid, int i, int j);

int flow_field_get_rows(flow_field_t *grid);
int flow_field_get_cols(flow_field_t *grid);

#endif // __FLOW_FIELD_H__
