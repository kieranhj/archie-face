// ============================================================================
// Flow field.
// ============================================================================

#include "flow-field.h"
#include "noise.h"
#include "../lib/mouse.h"
#include "../lib/plot.h"
#include "../lib/trig.h"
#include "../lib/vector.h"

#include <stdlib.h>
#include <math.h>

#define _USE_MOUSE_POS_TO_SPAWN             1
#define _PARTICLE_COLOUR_FROM_DIRECTION     0

static vec2fp particles[MAX_PARTICLES];

int num_particles = MAX_PARTICLES / 2;

static fix16_t grid[GRID_ROWS * GRID_COLS];

void drawGrid() {
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            int a = FIX16_TO_INT(grid[j*GRID_COLS + i]);
            int x = GRID_OFFX + i*GRID_STEPX;
            int y = GRID_OFFY + j*GRID_STEPY;

            plot_point(x, y, a);
        }
    }
}

void drawGridDirs() {
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            fix16_t a = grid[j*GRID_COLS + i];
            int x0 = GRID_OFFX + i*GRID_STEPX;
            int y0 = GRID_OFFY + j*GRID_STEPY;
            int dx = cos_fix16(a) >> 14;
            int dy = sin_fix16(a) >> 14;
            plot_line(x0, y0, x0 + dx, y0 + dy, 255);
        }
    }
}

void plotCurve(int x0, int y0, int num_steps, int col) {
    for(int i = 0; i < num_steps; i++) {
        int col_idx = x0 / GRID_STEPX;
        int row_idx = y0 / GRID_STEPY;

        if (col_idx>=0 && col_idx<GRID_COLS && row_idx>=0 && row_idx<GRID_ROWS) {
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

void plotParticles() {
    for(int i = 0; i < num_particles; i++) {
        #if _PARTICLE_COLOUR_FROM_DIRECTION
        int col_idx = FIX16_TO_INT(particles[i].x) / GRID_STEPX;
        int row_idx = FIX16_TO_INT(particles[i].y) / GRID_STEPY;

        if (col_idx>=0 && col_idx<GRID_COLS && row_idx>=0 && row_idx<GRID_ROWS) {
            fix16_t a = grid[row_idx*GRID_COLS + col_idx];
            plot_point(FIX16_TO_INT(particles[i].x), FIX16_TO_INT(particles[i].y), FIX16_TO_INT(a));
        }
        #else
        plot_point(FIX16_TO_INT(particles[i].x), FIX16_TO_INT(particles[i].y), 64 + (i>>2));
        #endif
    }
}

extern u32 vortex_radius;

void moveParticles()
{
    #if _USE_MOUSE_POS_TO_SPAWN
    int mouseX, mouseY;
    u8 mb;
    mouse_read(&mouseX, &mouseY, &mb);
    #endif

    for(int i = 0; i < num_particles; i++) {
        int col_idx = FIX16_TO_INT(particles[i].x) / GRID_STEPX;
        int row_idx = FIX16_TO_INT(particles[i].y) / GRID_STEPY;

        if (col_idx>=0 && col_idx<GRID_COLS && row_idx>=0 && row_idx<GRID_ROWS) {
            fix16_t a = grid[row_idx*GRID_COLS + col_idx];

            fix16_t dx = cos_fix16(a);             // [-1.0, 1.0]  [s1.16]
            fix16_t dy = sin_fix16(a);             // [-1.0, 1.0]  [s1.16]

            particles[i].x += FIX16_MUL(dx, FLOAT_TO_FIX16(0.8f));
            particles[i].y += FIX16_MUL(dy, FLOAT_TO_FIX16(0.8f));
        }
        else {
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

void updateGrid() {
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            fix16_t a = grid[j*GRID_COLS + i];
            grid[j*GRID_COLS + i] = (a + FIX16_ONE) & INT_TO_FIX16(255);
        }
    }
}

void MakeZeroGrid() {
    // Init.
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            //grid[j*GRID_COLS + i] = 256 * j / GRID_ROWS;            // default angle.
            grid[j*GRID_COLS + i] = INT_TO_FIX16(32);
        }
    }
}

void MakeNoiseGrid() {
    // Init.
    noise_init();
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            float n = noise_sample_2d(i * 0.1f, j * 0.1f);  // NOISE SMOOTHING FACTOR
            n = (n + 1.0f) * 0.5f;
            grid[j*GRID_COLS + i] = FLOAT_TO_FIX16(256*n);
        }
    }
}

void MakeParticles() {
    for(int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].x = INT_TO_FIX16(rand_between(0,319));
        particles[i].y = INT_TO_FIX16(rand_between(0,255));
    }
}

void KillGrid() {
}

void gridAddAttractor(int x, int y) {
    // Make grid points in radius R point towards (x,y)
    float r = 50.0f;
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
                float a = trig_fast_arctan2(dy, dx)/(2.0f*M_PI);          // vec from grid point to target (-0.5f, 0.5f]

                if (a<0.0f) a=1.0f+a;

                grid[j*GRID_COLS + i] = FLOAT_TO_FIX16(256*a*f);
            }
        }
    }
}

void gridAddNode(int x, int y, int fx, int fy, int radius) {
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
                float a = trig_fast_arctan2(fx*dx, fy*dy)/(2*M_PI);  // angle to point.

                if (a<0.0f) a=1.0f+a;

                grid[j*GRID_COLS + i] = FLOAT_TO_FIX16(256*a*f);
            }
        }
    }
}
