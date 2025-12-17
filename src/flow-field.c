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

#define _USE_MOUSE_POS_TO_SPAWN             1
#define _PARTICLE_COLOUR_FROM_DIRECTION     0

static u32 vortex_radius = 50;      // debug.
int flow_field_num_particles = MAX_PARTICLES / 2;
u32 flow_field_show_grid;
u32 flow_field_rotate_grid;

static vec2fp particles[MAX_PARTICLES];
static fix16_t grid[GRID_ROWS * GRID_COLS];

// ============================================================================

void flow_field_init()
{
    flow_field_init_with_zero();
    flow_field_init_particles();

    debug_register_key(RMKey_M, flow_field_init_particles, 0, 0);
    debug_register_key(RMKey_N, flow_field_init_with_noise, 0, 0);
    debug_register_key(RMKey_Z, flow_field_init_with_zero, 0, 0);
    debug_register_key(RMKey_V, flow_field_debug_add_vortex, 1, -1);
    debug_register_key(RMKey_B, flow_field_debug_add_vortex, -1, 1);
    debug_register_key(RMKey_C, flow_field_debug_draw_curve, 0, 0);
    debug_register_key(RMKey_G, debug_toggle_word, (u32)&flow_field_show_grid, 0);
    debug_register_key(RMKey_U, debug_toggle_word, (u32)&flow_field_rotate_grid, 0);
    debug_register_key(RMKey_ArrowUp, debug_word_add, (u32)&flow_field_num_particles, 10);
    debug_register_key(RMKey_ArrowDown, debug_word_add, (u32)&flow_field_num_particles, -10);
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

void flow_field_init_particles()
{
    for(int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].x = INT_TO_FIX16(rand_between(0,319));
        particles[i].y = INT_TO_FIX16(rand_between(0,255));
    }
}

void flow_field_rotate_field_particles()
{
    #if _USE_MOUSE_POS_TO_SPAWN
    int mouseX, mouseY;
    u8 mb;
    mouse_read(&mouseX, &mouseY, &mb);
    #endif

    for(int i = 0; i < flow_field_num_particles; i++)
    {
        int col_idx = FIX16_TO_INT(particles[i].x) / GRID_STEPX;
        int row_idx = FIX16_TO_INT(particles[i].y) / GRID_STEPY;

        if (col_idx>=0 && col_idx<GRID_COLS && row_idx>=0 && row_idx<GRID_ROWS)
        {
            fix16_t a = grid[row_idx*GRID_COLS + col_idx];

            fix16_t dx = cos_fix16(a);             // [-1.0, 1.0]  [s1.16]
            fix16_t dy = sin_fix16(a);             // [-1.0, 1.0]  [s1.16]

            particles[i].x += FIX16_MUL(dx, FLOAT_TO_FIX16(0.8f));
            particles[i].y += FIX16_MUL(dy, FLOAT_TO_FIX16(0.8f));
        }
        else
        {
            #if _USE_MOUSE_POS_TO_SPAWN
            particles[i].x = INT_TO_FIX16(mouseX + rand_between(0,vortex_radius)-vortex_radius/2);
            particles[i].y = INT_TO_FIX16(mouseY + rand_between(0,vortex_radius)-vortex_radius/2);
            #else
            particles[i].x = INT_TO_FIX16(rand_between(0,319));
            particles[i].y = INT_TO_FIX16(rand_between(0,255));
            #endif
        }
    }
}

void flow_field_draw_particles()
{
    for(int i = 0; i < flow_field_num_particles; i++)
    {
        #if _PARTICLE_COLOUR_FROM_DIRECTION
        int col_idx = FIX16_TO_INT(particles[i].x) / GRID_STEPX;
        int row_idx = FIX16_TO_INT(particles[i].y) / GRID_STEPY;

        if (col_idx>=0 && col_idx<GRID_COLS && row_idx>=0 && row_idx<GRID_ROWS)
        {
            fix16_t a = grid[row_idx*GRID_COLS + col_idx];
            plot_point(FIX16_TO_INT(particles[i].x), FIX16_TO_INT(particles[i].y), FIX16_TO_INT(a));
        }
        #else
        plot_point(FIX16_TO_INT(particles[i].x), FIX16_TO_INT(particles[i].y), 64 + (i>>2));
        #endif
    }
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
            grid[j*GRID_COLS + i] = (a + FIX16_ONE) & INT_TO_FIX16(255);
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
                float a = trig_fast_arctan2(dy, dx)/(M_PI_2_F);          // vec from grid point to target (-0.5f, 0.5f]

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
                float a = trig_fast_arctan2(fx*dx, fy*dy)/(M_PI_2_F);  // angle to point.

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
