// ============================================================================
// Flow field.
// ============================================================================

#include "flow-field.h"
#include "globals.h"
#include "noise.h"
#include "../lib/debug.h"
#include "../lib/mouse.h"
#include "../lib/plot.h"
#include "../lib/trig.h"
#include "../lib/vector.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

#define FF_ANGLE_FIX16(f,i,j)       ((f)->angle[(j)*(f)->cols+(i)])
#define ONE_OVER_TWO_TIMES_PI       (1.0f/(2.0f*M_PI_F))

#define FF_DEFAULT_NOISE_SMOOTHING  0.05f
#define FF_DEFAULT_ANGLE            0

static u32 vortex_radius = 50;      // debug.
u32 flow_field_show_grid;

static flow_field_t *flow_field_debug_grid;

struct flow_field_s {
    int     cols, rows;
    int     step_x, step_y;
    fix16_t rows_per_pixel, cols_per_pixel;
    fix16_t angle[0];  
};

// ============================================================================

static void flow_field_debug_add_vortex(uintptr_t param1, uintptr_t param2);
static void flow_field_debug_draw_curve(uintptr_t param1, uintptr_t param2);
static void flow_field_debug_init_with_angle(uintptr_t param1, uintptr_t param2);
static void flow_field_debug_init_with_noise(uintptr_t param1, uintptr_t param2);

// ============================================================================

int flow_field_get_nearest_angle(flow_field_t *grid, fix16_t x, fix16_t y, fix16_t *a)
{
    int col_idx = FIX16_TO_INT(FIX16_MUL(x, grid->cols_per_pixel));
    int row_idx = FIX16_TO_INT(FIX16_MUL(y, grid->rows_per_pixel));

    if (col_idx>=0 && col_idx<grid->cols && row_idx>=0 && row_idx<grid->rows)
    {
        *a = FF_ANGLE_FIX16(grid,col_idx,row_idx);
        return 1;
    }
    
    return 0;
}

// ============================================================================

void flow_field_init(flow_field_t *debug_field)
{
    flow_field_debug_grid = debug_field;

    debug_register_key(RMKey_N, flow_field_debug_init_with_noise, 0, 0);
    debug_register_key(RMKey_Z, flow_field_debug_init_with_angle, FF_DEFAULT_ANGLE, 0);
    debug_register_key(RMKey_V, flow_field_debug_add_vortex, 1, -1);
    debug_register_key(RMKey_B, flow_field_debug_add_vortex, -1, 1);
    debug_register_key(RMKey_C, flow_field_debug_draw_curve, 0, 0);
    debug_register_key(RMKey_G, debug_toggle_word, (uintptr_t)&flow_field_show_grid, 0);
    debug_register_key(RMKey_1, debug_set_word, (uintptr_t)&vortex_radius, 20);
    debug_register_key(RMKey_2, debug_set_word, (uintptr_t)&vortex_radius, 40);
    debug_register_key(RMKey_3, debug_set_word, (uintptr_t)&vortex_radius, 60);
    debug_register_key(RMKey_4, debug_set_word, (uintptr_t)&vortex_radius, 80);
    debug_register_key(RMKey_5, debug_set_word, (uintptr_t)&vortex_radius, 100);
    debug_register_key(RMKey_6, debug_set_word, (uintptr_t)&vortex_radius, 120);
    debug_register_key(RMKey_7, debug_set_word, (uintptr_t)&vortex_radius, 140);
    debug_register_key(RMKey_8, debug_set_word, (uintptr_t)&vortex_radius, 160);
    debug_register_key(RMKey_9, debug_set_word, (uintptr_t)&vortex_radius, 180);
}

flow_field_t *flow_field_make(int cols, int rows)
{
    flow_field_t *grid = malloc(sizeof(struct flow_field_s) + (cols * rows * sizeof(fix16_t)));

    grid->cols = cols;
    grid->rows = rows;
    grid->step_x = Screen_Width / cols;
    grid->step_y = Screen_Height / rows;
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

int flow_field_get_rows(flow_field_t *grid)
{
    return grid->rows;
}

int flow_field_get_cols(flow_field_t *grid)
{
    return grid->cols;
}

// ============================================================================

void flow_field_draw_grid(flow_field_t *grid)
{
    int sx = grid->step_x;
    int sy = grid->step_y;
    for(int i = 0; i < grid->cols; i++)
    {
        for(int j = 0; j < grid->rows; j++)
        {
            int a = FIX16_TO_INT(FF_ANGLE_FIX16(grid,i,j));
            int x = (sx/2) + i*sx;
            int y = (sy/2) + j*sy;

            plot_point(x, y, a);
        }
    }
}

void flow_field_draw(flow_field_t *grid)
{
    int sx = grid->step_x;
    int sy = grid->step_y;
    for(int i = 0; i < grid->cols; i++)
    {
        for(int j = 0; j < grid->rows; j++)
        {
            fix16_t a = FF_ANGLE_FIX16(grid,i,j);
            int x0 = (sx/2) + i*sx;
            int y0 = (sy/2) + j*sy;
            int dx = cos_fix16(a) >> 14;
            int dy = sin_fix16(a) >> 14;
            plot_line(x0, y0, x0 + dx, y0 + dy, 255);
        }
    }
}

void flow_field_draw_curve(flow_field_t *grid, int x0, int y0, int num_steps, int col)
{
    const fix16_t step_size = INT_TO_FIX16(4);
    fix16_t x=INT_TO_FIX16(x0);
    fix16_t y=INT_TO_FIX16(y0);

    for(int i = 0; i < num_steps; i++)
    {
        fix16_t a;

        if (flow_field_get_nearest_angle(grid, x, y, &a))
        {
            fix16_t dx = FIX16_MUL(cos_fix16(a), step_size);
            fix16_t dy = FIX16_MUL(sin_fix16(a), step_size);

            fix16_t x1 = x + dx;
            fix16_t y1 = y + dy;

            plot_line(FIX16_TO_INT(x), FIX16_TO_INT(y), FIX16_TO_INT(x1), FIX16_TO_INT(y1), col);

            x = x1;
            y = y1;
        }
        else
        {
            break;
        }
    }
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
        }
    }
}

void flow_field_init_with_noise(flow_field_t *grid, float smoothing)    // 0.1f
{
    float init_x = 64.0f;        // Can sample from within the larger noise field.
    float init_y = 64.0f;

    //init_x = rand_between(0, 255);
    //init_y = rand_between(0, 255);

    // Init.
    noise_init();

    for(int i = 0; i < grid->cols; i++)
    {
        for(int j = 0; j < grid->rows; j++)
        {
            #define NOISE_TO_BRAD 181.02f   // 128.0f
            // Returns [-1.0f, 1.0f] or +/-sqrt(N/4) so for 2D => (-0.707, 0.707) ?
            // So to expand to [-128.0f, 128.0f] multiply by 128/sqrt(0.5) ~= 181.02f

            float n = NOISE_TO_BRAD * noise_sample_2d(init_x + i * smoothing, init_y + j * smoothing);
            if (n<0.0f) n=256.0f+n;
            FF_ANGLE_FIX16(grid,i,j) = FLOAT_TO_FIX16(n);
        }
    }
}

void flow_field_set_angle(flow_field_t *grid, int i, int j, fix16_t a)
{
    assert(i >= 0 && i < grid->cols);
    assert(j >= 0 && j < grid->rows);
    FF_ANGLE_FIX16(grid,i,j) = a & 0x00ffffff;
}

fix16_t flow_field_get_angle(flow_field_t *grid, int i, int j)
{
    assert(i >= 0 && i < grid->cols);
    assert(j >= 0 && j < grid->rows);
    return FF_ANGLE_FIX16(grid,i,j);
}

#if 0
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
#endif

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

static void flow_field_debug_add_vortex(uintptr_t param1, uintptr_t param2)
{
    int mouseX, mouseY;
    u8 mb;
    mouse_read(&mouseX, &mouseY, &mb);
    flow_field_insert_vortex(flow_field_debug_grid, mouseX, mouseY, param1, param2, vortex_radius);
}

static void flow_field_debug_draw_curve(uintptr_t param1, uintptr_t param2)
{
    (void)param1;
    (void)param2;
    int mouseX, mouseY;
    u8 mb;
    mouse_read(&mouseX, &mouseY, &mb);
    flow_field_draw_curve(flow_field_debug_grid, mouseX, mouseY, 128, rand_between(64,255));
}

static void flow_field_debug_init_with_angle(uintptr_t param1, uintptr_t param2)
{
    (void)param2;
    flow_field_init_with_angle(flow_field_debug_grid, (fix16_t)param1);
}

static void flow_field_debug_init_with_noise(uintptr_t param1, uintptr_t param2)
{
    (void)param1;
    (void)param2;
    flow_field_init_with_noise(flow_field_debug_grid, FF_DEFAULT_NOISE_SMOOTHING);
}

// ============================================================================
