// ============================================================================
// Flow field.
// ============================================================================

#include "flow-field.h"
#include "noise.h"
#include "../lib/debug.h"
#include "../lib/mouse.h"
#include "../lib/plot.h"
#include "../lib/trig.h"
#include "../lib/vector.h"

#include <stdlib.h>
#include <math.h>


#define FF_ANGLE_FIX16(f,i,j)       ((f)->angle[(j)*(f)->cols+(i)])
#define ONE_OVER_TWO_TIMES_PI       (1.0f/(2.0f*M_PI_F))


static u32 vortex_radius = 50;      // debug.
u32 flow_field_show_grid;
u32 flow_field_rotate_grid;

static flow_field_t *flow_field_debug_grid;

struct flow_field_s {
    int     cols, rows;
    int     step_x, step_y;
    fix16_t rows_per_pixel, cols_per_pixel;
    fix16_t angle[0];  
};

// ============================================================================

void flow_field_debug_add_vortex(u32 param1, u32 param2);
void flow_field_debug_draw_curve(u32 param1, u32 param2);
void flow_field_debug_init_with_angle(u32 param1, u32 param2);
void flow_field_debug_init_with_noise(u32 param1, u32 param2);

void flow_field_init(flow_field_t *debug_field)
{
    flow_field_debug_grid = debug_field;

    debug_register_key(RMKey_N, flow_field_debug_init_with_noise, 0, 0);
    debug_register_key(RMKey_Z, flow_field_debug_init_with_angle, 0, 0);
    debug_register_key(RMKey_V, flow_field_debug_add_vortex, 1, -1);
    debug_register_key(RMKey_B, flow_field_debug_add_vortex, -1, 1);
    debug_register_key(RMKey_C, flow_field_debug_draw_curve, 0, 0);
    debug_register_key(RMKey_G, debug_toggle_word, (u32)&flow_field_show_grid, 0);
    debug_register_key(RMKey_U, debug_toggle_word, (u32)&flow_field_rotate_grid, 0);
    debug_register_key(RMKey_1, debug_set_word, (u32)&vortex_radius, 20);
    debug_register_key(RMKey_2, debug_set_word, (u32)&vortex_radius, 40);
    debug_register_key(RMKey_3, debug_set_word, (u32)&vortex_radius, 60);
    debug_register_key(RMKey_4, debug_set_word, (u32)&vortex_radius, 80);
    debug_register_key(RMKey_5, debug_set_word, (u32)&vortex_radius, 100);
    debug_register_key(RMKey_6, debug_set_word, (u32)&vortex_radius, 120);
    debug_register_key(RMKey_7, debug_set_word, (u32)&vortex_radius, 140);
    debug_register_key(RMKey_8, debug_set_word, (u32)&vortex_radius, 160);
    debug_register_key(RMKey_9, debug_set_word, (u32)&vortex_radius, 180);
}

flow_field_t *flow_field_make(int cols, int rows)
{
    flow_field_t *grid = malloc(sizeof(struct flow_field_s) + (cols * rows * sizeof(fix16_t)));

    grid->cols = cols;
    grid->rows = rows;
    grid->step_x = 320 / cols;
    grid->step_y = 256 / rows;
    grid->cols_per_pixel = FIX16_FRACTION(cols, 320);
    grid->rows_per_pixel = FIX16_FRACTION(rows, 256);
    
    flow_field_init_with_angle(grid, INT_TO_FIX16(32));

    return grid;
}

void *flow_field_kill(flow_field_t *grid)
{
    free(grid);
    return NULL;
}

// ============================================================================

void flow_field_draw_grid(flow_field_t *grid)
{
    for(int i = 0; i < grid->cols; i++)
    {
        for(int j = 0; j < grid->rows; j++)
        {
            int a = FIX16_TO_INT(FF_ANGLE_FIX16(grid,i,j));
            int x = GRID_OFFX + i*grid->step_x;
            int y = GRID_OFFY + j*grid->step_y;

            plot_point(x, y, a);
        }
    }
}

void flow_field_draw(flow_field_t *grid)
{
    for(int i = 0; i < grid->cols; i++)
    {
        for(int j = 0; j < grid->rows; j++)
        {
            fix16_t a = FF_ANGLE_FIX16(grid,i,j);
            int x0 = GRID_OFFX + i*grid->step_x;
            int y0 = GRID_OFFY + j*grid->step_y;
            int dx = cos_fix16(a) >> 14;
            int dy = sin_fix16(a) >> 14;
            plot_line(x0, y0, x0 + dx, y0 + dy, 255);
        }
    }
}

void flow_field_draw_curve(flow_field_t *grid, int x0, int y0, int num_steps, int col)
{
    for(int i = 0; i < num_steps; i++)
    {
        int col_idx = FIX16_TO_INT(x0 * grid->cols_per_pixel);    // BROKEN?
        int row_idx = FIX16_TO_INT(y0 * grid->rows_per_pixel);

        if (col_idx>=0 && col_idx<grid->cols && row_idx>=0 && row_idx<grid->rows)
        {
            fix16_t a = FF_ANGLE_FIX16(grid,col_idx,row_idx);
            int dx = cos_fix16(a) >> 14;
            int dy = sin_fix16(a) >> 14;
            int x1 = x0 + dx;
            int y1 = y0 + dy;

            plot_line(x0, y0, x1, y1, col);

            x0 = x1;
            y0 = y1;
        }
    }
}

// ============================================================================

inline int flow_field_get_angle(flow_field_t *grid, fix16_t x, fix16_t y, fix16_t *a)
{
    int col_idx = FIX16_TO_INT(FIX16_MUL(x, grid->cols_per_pixel));
    int row_idx = FIX16_TO_INT(FIX16_MUL(y, grid->rows_per_pixel));

    if (col_idx>=0 && col_idx<grid->cols && row_idx>=0 && row_idx<grid->rows)
    {
        *a = FF_ANGLE_FIX16(grid,col_idx,row_idx);//grid->angle[row_idx*grid->cols + col_idx];
        return 1;
    }

    // TODO: Wrap?
    
    return 0;
}

// ============================================================================

void flow_field_init_with_angle(flow_field_t *grid, fix16_t angle)
{
    // Init.
    for(int i = 0; i < grid->cols; i++)
    {
        for(int j = 0; j < grid->rows; j++)
        {
            FF_ANGLE_FIX16(grid,i,j) = angle;
            //grid->angle[j*grid->cols + i] = angle;
        }
    }
}

void flow_field_init_with_noise(flow_field_t *grid, float smoothing)    // 0.1f
{
    // Init.
    noise_init();
    for(int i = 0; i < grid->cols; i++)
    {
        for(int j = 0; j < grid->rows; j++)
        {
            float n = noise_sample_2d(i * smoothing, j * smoothing);  // NOISE SMOOTHING FACTOR
            n = (n + 1.0f) * 0.5f;
            FF_ANGLE_FIX16(grid,i,j) = FLOAT_TO_FIX16(256*n); // TODO: Optimise.
        }
    }
}

void flow_field_rotate_field(flow_field_t *grid, fix16_t angle)
{
    for(int i = 0; i < grid->cols; i++)
    {
        for(int j = 0; j < grid->rows; j++)
        {
            fix16_t a = grid->angle[j*grid->cols + i];
            FF_ANGLE_FIX16(grid,i,j) = (a + angle) & INT_TO_FIX16(255);    // Or just add a fixed offset at lookup!
        }
    }
}

// ============================================================================

void flow_field_insert_attractor(flow_field_t *grid, int x, int y, int radius)
{
    // Make grid points in radius R point towards (x,y)
    float r = radius;
    float r2 = r*r;

    for(int i = 0; i < grid->cols; i++)
    {
        for(int j = 0; j < grid->rows; j++)
        {
            // Grid point.
            int gx = i * grid->step_x;
            int gy = j * grid->step_y;

            // Delta from grid point to our point.
            int dx = x-gx;
            int dy = y-gy;

            float d2 = dx * dx + dy * dy;

            if (d2 <= r2) {
                // Bend grid angle towards the point.

                float f = 1.0f;// - sqrtf(d2)/r;             // f=1.0 at 0 and f=0.0 at r.
                float a = trig_fast_arctan2(dy, dx) * ONE_OVER_TWO_TIMES_PI;          // vec from grid point to target (-0.5f, 0.5f]

                if (a<0.0f) a=1.0f+a;

                FF_ANGLE_FIX16(grid,i,j) = FLOAT_TO_FIX16(256*a*f);
            }
        }
    }
}

void flow_field_insert_vortex(flow_field_t *grid, int x, int y, int fx, int fy, int radius)
{
    // Make grid points within radius R move around (x,y)
    float r = radius;
    float r2 = r*r;

    for(int i = 0; i < grid->cols; i++)
    {
        for(int j = 0; j < grid->rows; j++)
        {
            // Grid point.
            int gx = i * grid->step_x;
            int gy = j * grid->step_y;

            // Delta from grid point to our point.
            int dx = x-gx;
            int dy = y-gy;

            float d2 = dx * dx + dy * dy;

            if (d2 <= r2) {
                // Bend grid angle towards the point.

                float f = 1.0f;// - sqrtf(d2)/r;             // f=1.0 at 0 and f=0.0 at r.
                                                            // for this to work would need to compute the angle delta.
                float a = trig_fast_arctan2(fx*dx, fy*dy) * ONE_OVER_TWO_TIMES_PI;  // angle to point.

                if (a<0.0f) a=1.0f+a;

                FF_ANGLE_FIX16(grid,i,j) = FLOAT_TO_FIX16(256*a*f);
            }
        }
    }
}

// ============================================================================

void flow_field_debug_add_vortex(u32 param1, u32 param2)
{
    int mouseX, mouseY;
    u8 mb;
    mouse_read(&mouseX, &mouseY, &mb);
    flow_field_insert_vortex(flow_field_debug_grid, mouseX, mouseY, param1, param2, vortex_radius);
}

void flow_field_debug_draw_curve(u32 param1, u32 param2)
{
    (void)param1;
    (void)param2;
    int mouseX, mouseY;
    u8 mb;
    mouse_read(&mouseX, &mouseY, &mb);
    flow_field_draw_curve(flow_field_debug_grid, mouseX, mouseY, 128, rand_between(64,255));
}

void flow_field_debug_init_with_angle(u32 param1, u32 param2)
{
    (void)param2;
    flow_field_init_with_angle(flow_field_debug_grid, (fix16_t)param1);
}

void flow_field_debug_init_with_noise(u32 param1, u32 param2)
{
    (void)param1;
    (void)param2;
    flow_field_init_with_noise(flow_field_debug_grid, 0.1f);
}

// ============================================================================
