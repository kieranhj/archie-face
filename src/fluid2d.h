// ============================================================================
// Simple 2D fluid sim.
// From https://www.dgp.toronto.edu/public_user/stam/reality/Research/pdf/GDC03.pdf
// ============================================================================

#ifndef __FLUID2D_H__
#define __FLUID2D_H__

#define FLUID2D_NUM_CELLS   16
#define FLUID2D_CUBE_SIZE   (256/FLUID2D_NUM_CELLS)

void Fluid2DClear(void);
void Fluid2DAddDensity(int x, int y, float amount);
void Fluid2DAddVelocity(int x, int y, float amountu, float amountv);

void Fluid2DStep(float dt);
void Fluid2DDraw();

#endif // __FLUID2D_H__
