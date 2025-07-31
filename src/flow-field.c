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

static vec2fp particles[NUM_PARTICLES];

static u8 grid[GRID_ROWS * GRID_COLS];

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
            int dx = cosLookupTable[a];
            int dy = sineLookupTable[a];
            plotLine(x0, y0, x0 + (dx>>5), y0 + (dy>>5), 255);
        }
    }
}

void plotCurve(int x0, int y0, int num_steps, int col) {
    for(int i = 0; i < num_steps; i++) {
        int col_idx = x0 / GRID_STEPX;
        int row_idx = y0 / GRID_STEPY;

        if (col_idx>=0 && col_idx<GRID_COLS && row_idx>=0 && row_idx<GRID_ROWS) {
            int a = grid[row_idx*GRID_COLS + col_idx];
            int dx = (cosLookupTable[a] >> 5);
            int dy = (sineLookupTable[a] >> 5);
            int x1 = x0 + dx;
            int y1 = y0 + dy;

            plotLine(x0, y0, x1, y1, col);

            x0 = x1;
            y0 = y1;
        }
    }
}

void plotParticles() {
    for(int i = 0; i < NUM_PARTICLES; i++) {
       plotPoint(FP_TO_INT(particles[i].x), FP_TO_INT(particles[i].y), 64 + (i>>2));
    }
}

void moveParticles() {
    for(int i = 0; i < NUM_PARTICLES; i++) {
        int col_idx = FP_TO_INT(particles[i].x) / GRID_STEPX;
        int row_idx = FP_TO_INT(particles[i].y) / GRID_STEPY;

        if (col_idx>=0 && col_idx<GRID_COLS && row_idx>=0 && row_idx<GRID_ROWS) {
            int a = grid[row_idx*GRID_COLS + col_idx];

            int dx = cosLookupTable[a];     // [-128,127]   [s0.7]
            int dy = sineLookupTable[a];    // [-128,127]   [s0.7]

            particles[i].x += dx << (16-7);
            particles[i].y += dy << (16-7);
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
            grid[j*GRID_COLS + i] = (a + 1) & 255;
        }
    }
}

void MakeGrid() {
    // Init.
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            //grid[j*GRID_COLS + i] = 256 * j / GRID_ROWS;            // default angle.
            float n = Noise2D(i * 0.1f, j * 0.1f);
            n = (n + 1.0f) * 0.5f;
            grid[j*GRID_COLS + i] = (int)(255*n);
        }
    }
}

void MakeParticles() {
    for(int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].x = FLOAT_TO_FP(randomBetween(0,319));
        particles[i].y = FLOAT_TO_FP(randomBetween(0,255));
    }
}

void KillGrid() {
}
