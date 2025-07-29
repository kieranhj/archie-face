#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <swis.h>

#include "archie/swi.h"
#include "archie/video.h"
#include "archie/keyboard.h"

#define Screen_SizeBytes 320*256
#define GRID_ROWS 32
#define GRID_COLS 40
#define GRID_STEPX (320/GRID_COLS)
#define GRID_STEPY (256/GRID_ROWS)
#define GRID_OFFX (GRID_STEPX/2)
#define GRID_OFFY (GRID_STEPY/2)

// TODO: Fixed point Sin and Cos tables, either INCBIN or generate.
// Sin & Cos tables 256 entries as signed 8 bit
const int32_t sineLookupTable[] = {128 - 0x80, 128 - 0x83, 128 - 0x86, 128 - 0x89, 128 - 0x8c, 128 - 0x8f, 128 - 0x92, 128 - 0x95, 128 - 0x98, 128 - 0x9c, 128 - 0x9f, 128 - 0xa2, 128 - 0xa5, 128 - 0xa8, 128 - 0xab, 128 - 0xae, 128 - 0xb0, 128 - 0xb3, 128 - 0xb6, 128 - 0xb9, 128 - 0xbc, 128 - 0xbf, 128 - 0xc1, 128 - 0xc4, 128 - 0xc7, 128 - 0xc9, 128 - 0xcc, 128 - 0xce, 128 - 0xd1, 128 - 0xd3, 128 - 0xd5, 128 - 0xd8, 128 - 0xda, 128 - 0xdc, 128 - 0xde, 128 - 0xe0, 128 - 0xe2, 128 - 0xe4, 128 - 0xe6, 128 - 0xe8, 128 - 0xea, 128 - 0xeb, 128 - 0xed, 128 - 0xef, 128 - 0xf0, 128 - 0xf2, 128 - 0xf3, 128 - 0xf4, 128 - 0xf6, 128 - 0xf7, 128 - 0xf8, 128 - 0xf9, 128 - 0xfa, 128 - 0xfb, 128 - 0xfb, 128 - 0xfc, 128 - 0xfd, 128 - 0xfd, 128 - 0xfe, 128 - 0xfe, 128 - 0xfe, 128 - 0xff, 128 - 0xff, 128 - 0xff, 128 - 0xff, 128 - 0xff, 128 - 0xff, 128 - 0xff, 128 - 0xfe, 128 - 0xfe, 128 - 0xfd, 128 - 0xfd, 128 - 0xfc, 128 - 0xfc, 128 - 0xfb, 128 - 0xfa, 128 - 0xf9, 128 - 0xf8, 128 - 0xf7, 128 - 0xf6, 128 - 0xf5, 128 - 0xf4, 128 - 0xf2, 128 - 0xf1, 128 - 0xef, 128 - 0xee, 128 - 0xec, 128 - 0xeb, 128 - 0xe9, 128 - 0xe7, 128 - 0xe5, 128 - 0xe3, 128 - 0xe1, 128 - 0xdf, 128 - 0xdd, 128 - 0xdb, 128 - 0xd9, 128 - 0xd7, 128 - 0xd4, 128 - 0xd2, 128 - 0xcf, 128 - 0xcd, 128 - 0xca, 128 - 0xc8, 128 - 0xc5, 128 - 0xc3, 128 - 0xc0, 128 - 0xbd, 128 - 0xba, 128 - 0xb8, 128 - 0xb5, 128 - 0xb2, 128 - 0xaf, 128 - 0xac, 128 - 0xa9, 128 - 0xa6, 128 - 0xa3, 128 - 0xa0, 128 - 0x9d, 128 - 0x9a, 128 - 0x97, 128 - 0x94, 128 - 0x91, 128 - 0x8e, 128 - 0x8a, 128 - 0x87, 128 - 0x84, 128 - 0x81, 128 - 0x7e, 128 - 0x7b, 128 - 0x78, 128 - 0x75, 128 - 0x71, 128 - 0x6e, 128 - 0x6b, 128 - 0x68, 128 - 0x65, 128 - 0x62, 128 - 0x5f, 128 - 0x5c, 128 - 0x59, 128 - 0x56, 128 - 0x53, 128 - 0x50, 128 - 0x4d, 128 - 0x4a, 128 - 0x47, 128 - 0x45, 128 - 0x42, 128 - 0x3f, 128 - 0x3c, 128 - 0x3a, 128 - 0x37, 128 - 0x35, 128 - 0x32, 128 - 0x30, 128 - 0x2d, 128 - 0x2b, 128 - 0x28, 128 - 0x26, 128 - 0x24, 128 - 0x22, 128 - 0x20, 128 - 0x1e, 128 - 0x1c, 128 - 0x1a, 128 - 0x18, 128 - 0x16, 128 - 0x14, 128 - 0x13, 128 - 0x11, 128 - 0x10, 128 - 0x0e, 128 - 0x0d, 128 - 0x0b, 128 - 0x0a, 128 - 0x09, 128 - 0x08, 128 - 0x07, 128 - 0x06, 128 - 0x05, 128 - 0x04, 128 - 0x03, 128 - 0x03, 128 - 0x02, 128 - 0x02, 128 - 0x01, 128 - 0x01, 128 - 0x00, 128 - 0x00, 128 - 0x00, 128 - 0x00, 128 - 0x00, 128 - 0x00, 128 - 0x00, 128 - 0x01, 128 - 0x01, 128 - 0x01, 128 - 0x02, 128 - 0x02, 128 - 0x03, 128 - 0x04, 128 - 0x04, 128 - 0x05, 128 - 0x06, 128 - 0x07, 128 - 0x08, 128 - 0x09, 128 - 0x0b, 128 - 0x0c, 128 - 0x0d, 128 - 0x0f, 128 - 0x10, 128 - 0x12, 128 - 0x14, 128 - 0x15, 128 - 0x17, 128 - 0x19, 128 - 0x1b, 128 - 0x1d, 128 - 0x1f, 128 - 0x21, 128 - 0x23, 128 - 0x25, 128 - 0x27, 128 - 0x2a, 128 - 0x2c, 128 - 0x2e, 128 - 0x31, 128 - 0x33, 128 - 0x36, 128 - 0x38, 128 - 0x3b, 128 - 0x3e, 128 - 0x40, 128 - 0x43, 128 - 0x46, 128 - 0x49, 128 - 0x4c, 128 - 0x4f, 128 - 0x51, 128 - 0x54, 128 - 0x57, 128 - 0x5a, 128 - 0x5d, 128 - 0x60, 128 - 0x63, 128 - 0x67, 128 - 0x6a, 128 - 0x6d, 128 - 0x70, 128 - 0x73, 128 - 0x76, 128 - 0x79, 128 - 0x7c, 128 - 0x80,};
const int32_t cosLookupTable[] = {128 - 0xff, 128 - 0xff, 128 - 0xff, 128 - 0xff, 128 - 0xff, 128 - 0xfe, 128 - 0xfe, 128 - 0xfd, 128 - 0xfd, 128 - 0xfc, 128 - 0xfc, 128 - 0xfb, 128 - 0xfa, 128 - 0xf9, 128 - 0xf8, 128 - 0xf7, 128 - 0xf6, 128 - 0xf5, 128 - 0xf4, 128 - 0xf2, 128 - 0xf1, 128 - 0xef, 128 - 0xee, 128 - 0xec, 128 - 0xeb, 128 - 0xe9, 128 - 0xe7, 128 - 0xe5, 128 - 0xe3, 128 - 0xe1, 128 - 0xdf, 128 - 0xdd, 128 - 0xdb, 128 - 0xd9, 128 - 0xd7, 128 - 0xd4, 128 - 0xd2, 128 - 0xcf, 128 - 0xcd, 128 - 0xca, 128 - 0xc8, 128 - 0xc5, 128 - 0xc3, 128 - 0xc0, 128 - 0xbd, 128 - 0xba, 128 - 0xb8, 128 - 0xb5, 128 - 0xb2, 128 - 0xaf, 128 - 0xac, 128 - 0xa9, 128 - 0xa6, 128 - 0xa3, 128 - 0xa0, 128 - 0x9d, 128 - 0x9a, 128 - 0x97, 128 - 0x94, 128 - 0x91, 128 - 0x8e, 128 - 0x8a, 128 - 0x87, 128 - 0x84, 128 - 0x81, 128 - 0x7e, 128 - 0x7b, 128 - 0x78, 128 - 0x75, 128 - 0x71, 128 - 0x6e, 128 - 0x6b, 128 - 0x68, 128 - 0x65, 128 - 0x62, 128 - 0x5f, 128 - 0x5c, 128 - 0x59, 128 - 0x56, 128 - 0x53, 128 - 0x50, 128 - 0x4d, 128 - 0x4a, 128 - 0x47, 128 - 0x45, 128 - 0x42, 128 - 0x3f, 128 - 0x3c, 128 - 0x3a, 128 - 0x37, 128 - 0x35, 128 - 0x32, 128 - 0x30, 128 - 0x2d, 128 - 0x2b, 128 - 0x28, 128 - 0x26, 128 - 0x24, 128 - 0x22, 128 - 0x20, 128 - 0x1e, 128 - 0x1c, 128 - 0x1a, 128 - 0x18, 128 - 0x16, 128 - 0x14, 128 - 0x13, 128 - 0x11, 128 - 0x10, 128 - 0x0e, 128 - 0x0d, 128 - 0x0b, 128 - 0x0a, 128 - 0x09, 128 - 0x08, 128 - 0x07, 128 - 0x06, 128 - 0x05, 128 - 0x04, 128 - 0x03, 128 - 0x03, 128 - 0x02, 128 - 0x02, 128 - 0x01, 128 - 0x01, 128 - 0x00, 128 - 0x00, 128 - 0x00, 128 - 0x00, 128 - 0x00, 128 - 0x00, 128 - 0x00, 128 - 0x01, 128 - 0x01, 128 - 0x01, 128 - 0x02, 128 - 0x02, 128 - 0x03, 128 - 0x04, 128 - 0x04, 128 - 0x05, 128 - 0x06, 128 - 0x07, 128 - 0x08, 128 - 0x09, 128 - 0x0b, 128 - 0x0c, 128 - 0x0d, 128 - 0x0f, 128 - 0x10, 128 - 0x12, 128 - 0x14, 128 - 0x15, 128 - 0x17, 128 - 0x19, 128 - 0x1b, 128 - 0x1d, 128 - 0x1f, 128 - 0x21, 128 - 0x23, 128 - 0x25, 128 - 0x27, 128 - 0x2a, 128 - 0x2c, 128 - 0x2e, 128 - 0x31, 128 - 0x33, 128 - 0x36, 128 - 0x38, 128 - 0x3b, 128 - 0x3e, 128 - 0x40, 128 - 0x43, 128 - 0x46, 128 - 0x49, 128 - 0x4c, 128 - 0x4f, 128 - 0x51, 128 - 0x54, 128 - 0x57, 128 - 0x5a, 128 - 0x5d, 128 - 0x60, 128 - 0x63, 128 - 0x67, 128 - 0x6a, 128 - 0x6d, 128 - 0x70, 128 - 0x73, 128 - 0x76, 128 - 0x79, 128 - 0x7c, 128 - 0x80, 128 - 0x83, 128 - 0x86, 128 - 0x89, 128 - 0x8c, 128 - 0x8f, 128 - 0x92, 128 - 0x95, 128 - 0x98, 128 - 0x9c, 128 - 0x9f, 128 - 0xa2, 128 - 0xa5, 128 - 0xa8, 128 - 0xab, 128 - 0xae, 128 - 0xb0, 128 - 0xb3, 128 - 0xb6, 128 - 0xb9, 128 - 0xbc, 128 - 0xbf, 128 - 0xc1, 128 - 0xc4, 128 - 0xc7, 128 - 0xc9, 128 - 0xcc, 128 - 0xce, 128 - 0xd1, 128 - 0xd3, 128 - 0xd5, 128 - 0xd8, 128 - 0xda, 128 - 0xdc, 128 - 0xde, 128 - 0xe0, 128 - 0xe2, 128 - 0xe4, 128 - 0xe6, 128 - 0xe8, 128 - 0xea, 128 - 0xeb, 128 - 0xed, 128 - 0xef, 128 - 0xf0, 128 - 0xf2, 128 - 0xf3, 128 - 0xf4, 128 - 0xf6, 128 - 0xf7, 128 - 0xf8, 128 - 0xf9, 128 - 0xfa, 128 - 0xfb, 128 - 0xfb, 128 - 0xfc, 128 - 0xfd, 128 - 0xfd, 128 - 0xfe, 128 - 0xfe, 128 - 0xfe, 128 - 0xff, 128 - 0xff, 128 - 0xff,};

u8* framebuffer;
u8* grid;
int screen_bank;

struct vec2 {
    float x, y;
};

float dot(struct vec2 a, struct vec2 b) {
    return a.x * b.x + a.y * b.y;
}

i32 randomBetween(i32 min, i32 max){            // can return negative values?
    return rand() % (max + 1 - min) + min;
}

void shuffle(int *arrayToShuffle) {
	for(int e = 255; e > 0; e--) {
		u8 index = randomBetween(0,e-1);
		int temp = arrayToShuffle[e];
		
		arrayToShuffle[e] = arrayToShuffle[index];
		arrayToShuffle[index] = temp;
	}
}

static int Permutation[512];

void MakePermutation() {
	for(int i = 0; i < 256; i++) {
		Permutation[i]=i;
	}

	shuffle(Permutation);

    for(int i = 0; i < 256; i++) {
        Permutation[256+i] = Permutation[i];
    }
}

struct vec2 GetConstantVector(int v) {
    struct vec2 vec0 = {1.0f, 1.0f};
    struct vec2 vec1 = {-1.0f, 1.0f};
    struct vec2 vec2 = {-1.0f, -1.0f};
    struct vec2 vec3 = {1.0f, -1.0f};

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

	struct vec2 topRight = {xf-1.0f, yf-1.0f};
	struct vec2 topLeft = {xf, yf-1.0f};
	struct vec2 bottomRight = {xf-1.0f, yf};
	struct vec2 bottomLeft = {xf, yf};

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

void quit(){
    v_disableVSync();   // disables vsync event (but no handler?)
    //flush last vsync
    v_waitForVSync();
}

void init(){ 
    // TODO: Initialise screen RAM size for num_banks.

    v_setMode(13);
    v_disableTextCursor();
    v_enableVSync();    // enables vsync event (but no handler?)

    atexit(quit); //register exit callback
}

//make sure that dest is aligned and size (in bytes) can be divided by 40; feel free to re-use this in other projects :)
void __attribute__((noinline)) memsetFast(u32* dest, u32 c, u32 size){
    asm volatile("push {%0, %1}\n"
        "mov r0, %1\n"
        "mov r1, %1\n"
        "mov r2, %1\n"
        "mov r3, %1\n"
        "mov r4, %1\n"
        "mov r5, %1\n"
        "mov r6, %1\n"
        "mov r7, %1\n"
        "mov r8, %1\n"
    "%=:\n" //auto generated label
        "stm %0!, {r0-r8,%1}\n"
        "cmp %0, %2\n"
        "bne %=b\n" //jump *b*ackwards to auto generated label
        "pop {%0, %1}\n"
    :                                           // outputs
    : "r"(dest), "r"(c), "r"(dest + (size/4))   // inputs
    : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "cc", "memory"); // clobbers
}

void plotPoint(register int x, register int y, register u8 c) {
    if (x>=0 && x<320 && y>=0 && y<256) *(framebuffer + x + y*320) = c;
}

void plotSinCos() {
    for (int i=0; i < 256; i++) {
        // NB. tables are upside down because the screen is 0 at the top...
        plotPoint(i, 128+cosLookupTable[i], 255);
        plotPoint(i, 128+sineLookupTable[i], 64);
    }
}

void plotLine(int x0, int y0, int x1, int y1, u8 col) {
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    while(1) {
        if (x0==x1 && y0==y1) break;

        if (x0>=0 && x0<320 && y0>=0 && y0<256) *(framebuffer + x0 + y0*320) = col;

        int e2 = 2 * error;
        if (e2 >= dy) {
            error += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            error += dx;
            y0 += sy;
        }
    }
}

void drawGrid() {
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            int a = grid[j*GRID_COLS + i];

            plotPoint(GRID_OFFX + i*GRID_STEPX, GRID_OFFY + j*GRID_STEPY, a);
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

void v_setDisplayBank(int bank) {
    asm volatile("mov r0, " swiToConst(OSByte_WriteDisplayBank) "\n"
                 "mov r1, %0\n"
                 "swi " swiToConst(OS_Byte)
                :            // outputs
                : "r"(bank)  // inputs 
                : "r0", "r1", "cc"); // clobbers
}

void v_setWriteBank(int bank) {
    asm volatile("mov r0, " swiToConst(OSByte_WriteVDUBank) "\n"
                 "mov r1, %0\n"
                 "swi " swiToConst(OS_Byte)
                :            // outputs
                : "r"(bank)  // inputs 
                : "r0", "r1", "cc"); // clobbers
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

#define NUM_PARTICLES 256

static struct vec2 particles[NUM_PARTICLES];

void plotParticles() {
    for(int i = 0; i < NUM_PARTICLES; i++) {
       plotPoint(particles[i].x, particles[i].y, 64 + (i>>2));
    }
}

void moveParticles() {
    for(int i = 0; i < NUM_PARTICLES; i++) {
        int col_idx = particles[i].x / GRID_STEPX;
        int row_idx = particles[i].y / GRID_STEPY;

        if (col_idx>=0 && col_idx<GRID_COLS && row_idx>=0 && row_idx<GRID_ROWS) {
            int a = grid[row_idx*GRID_COLS + col_idx];
            int dx = (cosLookupTable[a] >> 5);
            int dy = (sineLookupTable[a] >> 5);

            particles[i].x += dx;
            particles[i].y += dy;
        }
        else {
            particles[i].x = randomBetween(0,319);
            particles[i].y = randomBetween(0,255);
        }
    }
}

void updateGrid(u8 *grid) {
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            int a = grid[j*GRID_COLS + i];
            grid[j*GRID_COLS + i] = (a + 1) & 255;
        }
    }
}

int main(int argc, char* argv[]){
    
    init();

    MakePermutation();

    grid = (u8*)malloc(GRID_ROWS * GRID_COLS);
    
    // Init.
    for(int i = 0; i < GRID_COLS; i++) {
        for(int j = 0; j < GRID_ROWS; j++) {
            //grid[j*GRID_COLS + i] = 256 * j / GRID_ROWS;            // default angle.
            float n = Noise2D(i * 0.1f, j * 0.1f);
            n = (n + 1.0f) * 0.5f;
            grid[j*GRID_COLS + i] = 255*n;
        }
    }

    for(int i = 0; i < NUM_PARTICLES; i++) {
        struct vec2 p = {randomBetween(0,319), randomBetween(0,255)};
        particles[i] = p;
    }

    screen_bank = 0;    // write bank

    while(!k_checkKeypress(KEY_ESCAPE)){

        // Flip screens
        v_setDisplayBank(1+screen_bank);
        screen_bank ^= 1;
        v_setWriteBank(1+screen_bank);

        // Tick
        updateGrid(grid);
        moveParticles();

        // Vsync
        v_waitForVSync();

        framebuffer = v_getScreenAddress();

        // Clear screen
        memsetFast((u32*)framebuffer, 0, Screen_SizeBytes);

        // Draw screen
        //plotSinCos();
        //drawGridDirs();

        //for(int i=0; i < 100; i++) {
        //    plotCurve(randomBetween(0,319), randomBetween(0,255), 32, 64+i);
        //}

        plotParticles();
    }

    free(grid);

    v_setDisplayBank(1+screen_bank);

	return 0;
}

// TODO: Linker script to include binary files.
// TOOD: Raster timing.
