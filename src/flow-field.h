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

typedef struct flow_field_s flow_field_t;

extern u32 flow_field_show_grid;
extern u32 flow_field_rotate_grid;

void flow_field_init(flow_field_t *debug_grid);
flow_field_t *flow_field_make(int cols, int rows);
void *flow_field_kill(flow_field_t *grid);

void flow_field_init_with_noise(flow_field_t *grid, float smoothing);
void flow_field_init_with_angle(flow_field_t *grid, fix16_t angle);
//void flow_field_rotate_field(flow_field_t *grid, fix16_t angle);

void flow_field_draw(flow_field_t *grid);
void flow_field_draw_grid(flow_field_t *grid);
void flow_field_draw_curve(flow_field_t *grid, int x0, int y0, int num_steps, int col);

void flow_field_insert_attractor(flow_field_t *grid, int x, int y, int radius);
void flow_field_insert_vortex(flow_field_t *grid, int x, int y, int fx, int fy, int radius);

int flow_field_get_angle(flow_field_t *grid, fix16_t x, fix16_t y, fix16_t *a);

#endif // __FLOW_FIELD_H__
