// ============================================================================
// Flow field.
// ============================================================================

#include "flow-field.h"
#include <stdlib.h>
#include <math.h>
#include "../lib/plot.h"
#include "../lib/trig.h"
#include "../lib/vector.h"

static int Permutation[512];

static vec2fp particles[MAX_PARTICLES];

int num_particles = MAX_PARTICLES / 2;

static int grid[GRID_ROWS * GRID_COLS];

void shuffle(int *arrayToShuffle) {
	for(int e = 255; e > 0; e--) {
		u8 index = randomBetween(0,e-1);
		int temp = arrayToShuffle[e];
		
		arrayToShuffle[e] = arrayToShuffle[index];
		arrayToShuffle[index] = temp;
	}
}

void MakePermutation() {
	for(int i = 0; i < 256; i++) {
		Permutation[i]=i;
	}

	shuffle(Permutation);

    for(int i = 0; i < 256; i++) {
        Permutation[256+i] = Permutation[i];
    }
}

vec2f GetConstantVector(int v) {
    vec2f vec0 = {1.0f, 1.0f};
    vec2f vec1 = {-1.0f, 1.0f};
    vec2f vec2 = {-1.0f, -1.0f};
    vec2f vec3 = {1.0f, -1.0f};

	// v is the value from the permutation table
	int h = v & 3;
	if(h == 0)
		return vec0;
	else if(h == 1)
		return vec1;
	else if(h == 2)
		return vec2;
	else
		return vec3;
}

float Fade(float t) {
	return ((6*t - 15)*t + 10)*t*t*t;
}

float Lerp(float t, float a1, float a2) {
	return a1 + t*(a2-a1);
}

float Noise2D(float x, float y) {
	int X = (int)floorf(x) & 255;
	int Y = (int)floorf(y) & 255;

	float xf = x-floorf(x);
	float yf = y-floorf(y);

	vec2f topRight = {xf-1.0f, yf-1.0f};
	vec2f topLeft = {xf, yf-1.0f};
	vec2f bottomRight = {xf-1.0f, yf};
	vec2f bottomLeft = {xf, yf};

	// Select a value from the permutation array for each of the 4 corners
	int valueTopRight = Permutation[Permutation[X+1]+Y+1];
	int valueTopLeft = Permutation[Permutation[X]+Y+1];
	int valueBottomRight = Permutation[Permutation[X+1]+Y];
	int valueBottomLeft = Permutation[Permutation[X]+Y];
	
	float dotTopRight = dot(topRight, GetConstantVector(valueTopRight));
	float dotTopLeft = dot(topLeft, GetConstantVector(valueTopLeft));
	float dotBottomRight = dot(bottomRight, GetConstantVector(valueBottomRight));
	float dotBottomLeft = dot(bottomLeft, GetConstantVector(valueBottomLeft));
	
	float u = Fade(xf);
	float v = Fade(yf);
	
	return Lerp(u,
		Lerp(v, dotBottomLeft, dotTopLeft),
		Lerp(v, dotBottomRight, dotTopRight)
	);
}

void drawGrid() {
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            int a = grid[j*GRID_COLS + i];
            int x = GRID_OFFX + i*GRID_STEPX;
            int y = GRID_OFFY + j*GRID_STEPY;

            plotPoint(x, y, a);
        }
    }
}

void drawGridDirs() {
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            int a = grid[j*GRID_COLS + i];
            int x0 = GRID_OFFX + i*GRID_STEPX;
            int y0 = GRID_OFFY + j*GRID_STEPY;
            int dx = cos_fp(a) >> 14;
            int dy = sin_fp(a) >> 14;
            plotLine(x0, y0, x0 + dx, y0 + dy, 255);
        }
    }
}

void plotCurve(int x0, int y0, int num_steps, int col) {
    for(int i = 0; i < num_steps; i++) {
        int col_idx = x0 / GRID_STEPX;
        int row_idx = y0 / GRID_STEPY;

        if (col_idx>=0 && col_idx<GRID_COLS && row_idx>=0 && row_idx<GRID_ROWS) {
            int a = grid[row_idx*GRID_COLS + col_idx];
            int dx = cos_fp(a) >> 14;
            int dy = sin_fp(a) >> 14;
            int x1 = x0 + dx;
            int y1 = y0 + dy;

            plotLine(x0, y0, x1, y1, col);

            x0 = x1;
            y0 = y1;
        }
    }
}

void plotParticles() {
    for(int i = 0; i < num_particles; i++) {
       plotPoint(FP_TO_INT(particles[i].x), FP_TO_INT(particles[i].y), 64 + (i>>2));
    }
}

void moveParticles() {
    for(int i = 0; i < num_particles; i++) {
        int col_idx = FP_TO_INT(particles[i].x) / GRID_STEPX;
        int row_idx = FP_TO_INT(particles[i].y) / GRID_STEPY;

        if (col_idx>=0 && col_idx<GRID_COLS && row_idx>=0 && row_idx<GRID_ROWS) {
            int a = grid[row_idx*GRID_COLS + col_idx];

            int dx = cos_fp(a);             // [-1.0, 1.0]  [s1.16]
            int dy = sin_fp(a);             // [-1.0, 1.0]  [s1.16]

            particles[i].x += dx;
            particles[i].y += dy;
        }
        else {
            particles[i].x = FLOAT_TO_FP(randomBetween(0,319));
            particles[i].y = FLOAT_TO_FP(randomBetween(0,255));
        }
    }
}

void updateGrid() {
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            int a = grid[j*GRID_COLS + i];
            grid[j*GRID_COLS + i] = (a + (1<<16)) & (255<<16);
        }
    }
}

void MakeZeroGrid() {
    // Init.
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            //grid[j*GRID_COLS + i] = 256 * j / GRID_ROWS;            // default angle.
            grid[j*GRID_COLS + i] = FLOAT_TO_FP(32);
        }
    }
}

void MakeNoiseGrid() {
    // Init.
    MakePermutation();
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            float n = Noise2D(i * 0.1f, j * 0.1f);
            n = (n + 1.0f) * 0.5f;
            grid[j*GRID_COLS + i] = FLOAT_TO_FP(256*n);
        }
    }
}

void MakeParticles() {
    for(int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].x = FLOAT_TO_FP(randomBetween(0,319));
        particles[i].y = FLOAT_TO_FP(randomBetween(0,255));
    }
}

void KillGrid() {
}

void gridAddAttractor(int x, int y) {
    // Make grid points in radius R point towards (x,y)
    float r2 = 50.0f * 50.0f;

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

                float f = 1.0f;//1.0f - dist/r;                // f=1.0 at 0 and f=0.0 at r.
                float a = atan2f(dy, dx)/(2.0f*M_PI);          // vec from grid point to target (-0.5f, 0.5f]

                //if (a<0.0f) a=1.0f+a;

                grid[j*GRID_COLS + i] = FLOAT_TO_FP(256*a*f);
            }
        }
    }
}

void gridAddNode(int x, int y) {
    // Make grid points within radius R move around (x,y)
    float r2 = 50.0f * 50.0f;

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

                float f = 1.0f;//1.0f - dist/r;                // f=1.0 at 0 and f=0.0 at r.
                float a = atan2f(dx, -dy)/(2*M_PI);  // angle to point.

                //if (a<0.0f) a=1.0f+a;

                grid[j*GRID_COLS + i] = FLOAT_TO_FP(256*a*f);
            }
        }
    }
}
