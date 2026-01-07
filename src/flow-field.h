// ============================================================================
// Flow field.
// ============================================================================

#ifndef __FLOW_FIELD_H__
#define __FLOW_FIELD_H__

#include "archie/SDKTypes.h"
#include "../lib/maths.h"

#define GRID_ROWS       16
#define GRID_COLS       20
#define GRID_STEPX      (320/GRID_COLS)
#define GRID_STEPY      (256/GRID_ROWS)
#define GRID_OFFX       (GRID_STEPX/2)
#define GRID_OFFY       (GRID_STEPY/2)

extern u32 flow_field_show_grid;
extern u32 flow_field_rotate_grid;

void flow_field_init();
void flow_field_init_with_noise();
void flow_field_init_with_zero();
void flow_field_rotate_field();

void flow_field_draw();
void flow_field_draw_grid();
void flow_field_draw_curve(int x0, int y0, int num_steps, int col);

void flow_field_insert_attractor(int x, int y);
void flow_field_insert_vortex(int x, int y, int fx, int fy, int radius);

void flow_field_debug_add_vortex(u32 param1, u32 param2);
void flow_field_debug_draw_curve(u32 param1, u32 param2);

int flow_field_get_angle(fix16_t x, fix16_t y, fix16_t *a);

#endif // __FLOW_FIELD_H__
