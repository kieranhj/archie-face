/// ============================================================================
// Simple 2D fluid sim.
// From https://www.dgp.toronto.edu/public_user/stam/reality/Research/pdf/GDC03.pdf
// ============================================================================

#include "fluid2d.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "archie/utils.h"
#include "../lib/plot.h"

#define TOTAL_CELLS     (FLUID2D_NUM_CELLS+2)*(FLUID2D_NUM_CELLS+2)
#define IX(i,j)         ((i)+(N+2)*(j))
#define SWAP(x0,x)      {float *tmp=x0;x0=x;x=tmp;}

static float u[TOTAL_CELLS];
static float v[TOTAL_CELLS];
static float u_prev[TOTAL_CELLS];
static float v_prev[TOTAL_CELLS];
static float dens[TOTAL_CELLS];
static float dens_prev[TOTAL_CELLS]; 

// Discoveries from trying an online version of the code:
// As ITERATIONS gets smaller, the faster the fluid moves / dissapates
// Viscocity will likely be 0.0f - higher values make the fluid density equalise faster and more locally
// Diffusion should be very small e.g. 0.0004f or smaller
// As diffusion gets larger, the density dissapates faster.
// If diffusion==0.0f then 'the ink stays in the water' (never dissapates).
// - some questions around this being related to the grid size and whether this is correct?
//   (might be an issue with our very small grid?)
// Canvas example is 128x128=16384 (we are 16x16=256!)
// As timestep gets larger, things dissapate faster.

#define ITERATIONS      4

// ===> If these are zero then remove the diffusion calls altogether...
static float diff = 0.0f;      // density diffusion
static float visc = 0.0f;      // viscocity

void add_source( int N, float * x, float * s, float dt )
{
    int i, size=(N+2)*(N+2);
    for ( i=0 ; i<size ; i++ ) x[i] += dt*s[i];
}

void set_bnd ( int N, int b, float * x )
{
    int i;
    for ( i=1 ; i<=N ; i++ ) {
        x[IX(0  ,  i)] = b==1 ? -x[IX(1,i)] : x[IX(1,i)];
        x[IX(N+1,  i)] = b==1 ? -x[IX(N,i)] : x[IX(N,i)];
        x[IX(i  ,  0)] = b==2 ? -x[IX(i,1)] : x[IX(i,1)];
        x[IX(i  ,N+1)] = b==2 ? -x[IX(i,N)] : x[IX(i,N)];
    }
    x[IX(0  ,0  )] = 0.5f*(x[IX(1,0  )] + x[IX(0  ,1)]);
    x[IX(0  ,N+1)] = 0.5f*(x[IX(1,N+1)] + x[IX(0  ,N)]);
    x[IX(N+1,  0)] = 0.5f*(x[IX(N,0  )] + x[IX(N+1,1)]);
    x[IX(N+1,N+1)] = 0.5f*(x[IX(N,N+1)] + x[IX(N+1,N)]);
}

void linearSolver ( int N, int b, float * x, float * x0, float a, float c, int iter )
{
    int i, j, k;
    float invC = 1.0f / c;
    for ( k=0 ; k<iter ; k++ ) {
        for ( i=1 ; i<=N ; i++ ) {
            for ( j=1 ; j<=N ; j++ ) {
                x[IX(i,j)] = ( a * (x[IX(i-1,j)] + x[IX(i+1,j)]
                                  + x[IX(i,j-1)] + x[IX(i,j+1)])
                                 + x0[IX(i,j)] ) * invC;
            }
        }
        set_bnd ( N, b, x );
    }
}

void diffuse ( int N, int b, float * x, float * x0, float diff, float dt, int iter )
{
    float a=dt*diff*N*N;
    linearSolver( N, b, x, x0, a, 1 + 4*a, iter );
}

void advect ( int N, int b, float * d, float * d0, float * u, float * v, float dt )
{
    int i, j, i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;

    dt0 = dt*N;

    for ( i=1 ; i<=N ; i++ ) {
        for ( j=1 ; j<=N ; j++ ) {
            // go backwards through velocity field
            x = i - dt0 *u[IX(i,j)];
            y = j - dt0 *v[IX(i,j)];

            // interpolate results
            if (x<0.5f) x=0.5f;
            if (x>N+0.5f) x=N+0.5f;

            i0=(int)x;
            i1=i0+1;

            if (y<0.5f) y=0.5f;
            if (y>N+0.5f) y=N+0.5f;

            j0=(int)y;
            j1=j0+1;

            s1 = x-i0;
            s0 = 1-s1;
            t1 = y-j0;
            t0 = 1-t1;

            d[IX(i,j)] = s0 * (t0 * d0[IX(i0,j0)] + t1*d0[IX(i0,j1)])
                        +s1 * (t0 * d0[IX(i1,j0)] + t1*d0[IX(i1,j1)]);
        }
    }
    set_bnd ( N, b, d );
}

void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt )
{
    add_source ( N, x, x0, dt );
    SWAP ( x0, x );

    diffuse ( N, 0, x, x0, diff, dt, ITERATIONS );
    SWAP ( x0, x );

    advect ( N, 0, x, x0, u, v, dt );
}

void project ( int N, float * u, float * v, float * p, float * div, int iter )
{
    int i, j;
    float h=1.0f/N;

    for ( i=1 ; i<=N ; i++ ) {
        for ( j=1 ; j<=N ; j++ ) {
            div[IX(i,j)] = -0.5f * h *(u[IX(i+1,j)] - u[IX(i-1,j)] + v[IX(i,j+1)] - v[IX(i,j-1)]);
            p  [IX(i,j)] = 0.0f;
        }
    }
    
    set_bnd ( N, 0, div );
    set_bnd ( N, 0, p );

    linearSolver(N, 0, p, div, 1.0f, 4.0f, iter);

    for ( i=1 ; i<=N ; i++ ) {
        for ( j=1 ; j<=N ; j++ ) {
            u[IX(i,j)] -= 0.5f * (p[IX(i+1,  j)] - p[IX(i-1,  j)]) * N;
            v[IX(i,j)] -= 0.5f * (p[IX(i,  j+1)] - p[IX(i  ,j-1)]) * N;
        }
    }

    set_bnd ( N, 1, u );
    set_bnd ( N, 2, v );
}

void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt )
{
    add_source ( N, u, u0, dt );
    add_source ( N, v, v0, dt );

    SWAP ( u0, u );
    diffuse ( N, 1, u, u0, visc, dt, ITERATIONS );

    SWAP ( v0, v );
    diffuse ( N, 2, v, v0, visc, dt, ITERATIONS );
    project ( N, u, v, u0, v0, ITERATIONS );

    SWAP ( u0, u );
    SWAP ( v0, v );
    advect ( N, 1, u, u0, u0, v0, dt );
    advect ( N, 2, v, v0, u0, v0, dt );

    project ( N, u, v, u0, v0, ITERATIONS );
}

void Fluid2DClear(void) {
    memset (dens_prev, 0, sizeof(dens_prev) );
    memset (u_prev, 0, sizeof(u_prev) );
    memset (v_prev, 0, sizeof(v_prev) );
}

void Fluid2DAddDensity(int x, int y, float amount) {
    int N=FLUID2D_NUM_CELLS;
    if (x < 1 || x > N || y < 1 || y > N) return;
    dens_prev[IX(x,y)] += amount;
}

void Fluid2DAddVelocity(int x, int y, float amountu, float amountv) {
    int N=FLUID2D_NUM_CELLS;
    if (x < 1 || x > N || y < 1 || y > N) return;
    u_prev[IX(x,y)] += amountu;
    v_prev[IX(x,y)] += amountv;
}

void Fluid2DStep(float dt) {
    vel_step ( FLUID2D_NUM_CELLS, u, v, u_prev, v_prev, visc, dt );
    dens_step( FLUID2D_NUM_CELLS, dens, dens_prev, u, v, diff, dt );
}

void Fluid2DDraw() {
    int N=FLUID2D_NUM_CELLS;
    for(int j = 0; j < FLUID2D_NUM_CELLS+2; j++) {
        for(int i = 0; i < FLUID2D_NUM_CELLS+2; i++) {
            int col = dens[IX(i,j)]*255.0f;
            if (col < 0) col = 0;
            if (col > 255) col = 255;
            plotPoint(i * FLUID2D_CUBE_SIZE, j * FLUID2D_CUBE_SIZE, col);
        }
    }
}
