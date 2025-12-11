// ============================================================================
// Perlin Noise fns.
// ============================================================================

#include "noise.h"
#include "globals.h"
#include "../lib/vector.h"

#include <math.h>

static int Permutation[512];

static void shuffle(int *arrayToShuffle) {
	for(int e = 255; e > 0; e--) {
		u8 index = rand_between(0,e-1);
		int temp = arrayToShuffle[e];
		
		arrayToShuffle[e] = arrayToShuffle[index];
		arrayToShuffle[index] = temp;
	}
}

void noise_init() {
	for(int i = 0; i < 256; i++) {
		Permutation[i]=i;
	}

	shuffle(Permutation);

    for(int i = 0; i < 256; i++) {
        Permutation[256+i] = Permutation[i];
    }
}

static vec2f GetConstantVector(int v) {
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

static float Fade(float t) {
	return ((6*t - 15)*t + 10)*t*t*t;
}

static float Lerp(float t, float a1, float a2) {
	return a1 + t*(a2-a1);
}

float noise_sample_2d(float x, float y) {
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
	
	float vec2f_dotTopRight = vec2f_dot(topRight, GetConstantVector(valueTopRight));
	float vec2f_dotTopLeft = vec2f_dot(topLeft, GetConstantVector(valueTopLeft));
	float vec2f_dotBottomRight = vec2f_dot(bottomRight, GetConstantVector(valueBottomRight));
	float vec2f_dotBottomLeft = vec2f_dot(bottomLeft, GetConstantVector(valueBottomLeft));
	
	float u = Fade(xf);
	float v = Fade(yf);
	
	return Lerp(u,
		Lerp(v, vec2f_dotBottomLeft, vec2f_dotTopLeft),
		Lerp(v, vec2f_dotBottomRight, vec2f_dotTopRight)
	);
}
