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

static u32 vortex_radius = 50;      // debug.
u32 flow_field_show_grid;
u32 flow_field_rotate_grid;

static fix16_t grid[GRID_ROWS * GRID_COLS];

// ============================================================================

void flow_field_init()
{
    flow_field_init_with_zero();

    debug_register_key(RMKey_N, flow_field_init_with_noise, 0, 0);
    debug_register_key(RMKey_Z, flow_field_init_with_zero, 0, 0);
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

// ============================================================================

void flow_field_draw_grid()
{
    for(int i = 0; i < GRID_COLS; i++)
    {
        for(int j = 0; j < GRID_ROWS; j++)
        {
            int a = FIX16_TO_INT(grid[j*GRID_COLS + i]);
            int x = GRID_OFFX + i*GRID_STEPX;
            int y = GRID_OFFY + j*GRID_STEPY;

            plot_point(x, y, a);
        }
    }
}

void flow_field_draw()
{
    for(int i = 0; i < GRID_COLS; i++)
    {
        for(int j = 0; j < GRID_ROWS; j++)
        {
            fix16_t a = grid[j*GRID_COLS + i];
            int x0 = GRID_OFFX + i*GRID_STEPX;
            int y0 = GRID_OFFY + j*GRID_STEPY;
            int dx = cos_fix16(a) >> 14;
            int dy = sin_fix16(a) >> 14;
            plot_line(x0, y0, x0 + dx, y0 + dy, 255);
        }
    }
}

void flow_field_draw_curve(int x0, int y0, int num_steps, int col)
{
    for(int i = 0; i < num_steps; i++)
    {
        int col_idx = x0 / GRID_STEPX;
        int row_idx = y0 / GRID_STEPY;

        if (col_idx>=0 && col_idx<GRID_COLS && row_idx>=0 && row_idx<GRID_ROWS)
        {
            fix16_t a = grid[row_idx*GRID_COLS + col_idx];
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

int flow_field_get_angle(fix16_t x, fix16_t y, fix16_t *a)
{
    int col_idx = FIX16_TO_INT(x) / GRID_STEPX;
    int row_idx = FIX16_TO_INT(y) / GRID_STEPY;

    if (col_idx>=0 && col_idx<GRID_COLS && row_idx>=0 && row_idx<GRID_ROWS)
    {
        *a = grid[row_idx*GRID_COLS + col_idx];
        return 1;
    }
    
    return 0;
}

// ============================================================================

void flow_field_init_with_zero()
{
    // Init.
    for(int i = 0; i < GRID_COLS; i++)
    {
        for(int j = 0; j < GRID_ROWS; j++)
        {
            //grid[j*GRID_COLS + i] = 256 * j / GRID_ROWS;            // default angle.
            grid[j*GRID_COLS + i] = INT_TO_FIX16(32);
        }
    }
}

void flow_field_init_with_noise()
{
    // Init.
    noise_init();
    for(int i = 0; i < GRID_COLS; i++)
    {
        for(int j = 0; j < GRID_ROWS; j++)
        {
            float n = noise_sample_2d(i * 0.1f, j * 0.1f);  // NOISE SMOOTHING FACTOR
            n = (n + 1.0f) * 0.5f;
            grid[j*GRID_COLS + i] = FLOAT_TO_FIX16(256*n);
        }
    }
}

void flow_field_rotate_field()
{
    for(int i = 0; i < GRID_COLS; i++)
    {
        for(int j = 0; j < GRID_ROWS; j++)
        {
            fix16_t a = grid[j*GRID_COLS + i];
            grid[j*GRID_COLS + i] = (a + FIX16_ONE) & INT_TO_FIX16(255);    // Or just add a fixed offset at lookup!
        }
    }
}

// ============================================================================

void flow_field_insert_attractor(int x, int y)
{
    // Make grid points in radius R point towards (x,y)
    float r = 50.0f;
    float r2 = r*r;

    for(int i = 0; i < GRID_COLS; i++)
    {
        for(int j = 0; j < GRID_ROWS; j++)
        {
            // Grid point.
            int gx = i * GRID_STEPX;
            int gy = j * GRID_STEPY;

            // Delta from grid point to our point.
            int dx = x-gx;
            int dy = y-gy;

            float d2 = dx * dx + dy * dy;

            if (d2 <= r2) {
                // Bend grid angle towards the point.

                float f = 1.0f;// - sqrtf(d2)/r;             // f=1.0 at 0 and f=0.0 at r.
                float a = trig_fast_arctan2(dy, dx)/(2.0f*M_PI_F);          // vec from grid point to target (-0.5f, 0.5f]

                if (a<0.0f) a=1.0f+a;

                grid[j*GRID_COLS + i] = FLOAT_TO_FIX16(256*a*f);
            }
        }
    }
}

void flow_field_insert_vortex(int x, int y, int fx, int fy, int radius)
{
    // Make grid points within radius R move around (x,y)
    float r = radius;
    float r2 = r*r;

    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            // Grid point.
            int gx = i * GRID_STEPX;
            int gy = j * GRID_STEPY;

            // Delta from grid point to our point.
            int dx = x-gx;
            int dy = y-gy;

            float d2 = dx * dx + dy * dy;

            if (d2 <= r2) {
                // Bend grid angle towards the point.

                float f = 1.0f;// - sqrtf(d2)/r;             // f=1.0 at 0 and f=0.0 at r.
                                                            // for this to work would need to compute the angle delta.
                float a = trig_fast_arctan2(fx*dx, fy*dy)/(2.0f*M_PI_F);  // angle to point.

                if (a<0.0f) a=1.0f+a;

                grid[j*GRID_COLS + i] = FLOAT_TO_FIX16(256*a*f);
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
    flow_field_insert_vortex(mouseX, mouseY, param1, param2, vortex_radius);
}

void flow_field_debug_draw_curve(u32 param1, u32 param2)
{
    (void)param1;
    (void)param2;
    int mouseX, mouseY;
    u8 mb;
    mouse_read(&mouseX, &mouseY, &mb);
    flow_field_draw_curve(mouseX, mouseY, 128, rand_between(64,255));
}

// ============================================================================
