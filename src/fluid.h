// ============================================================================
// Simple fluid sim.
// From https://mikeash.com/pyblog/fluid-simulation-for-dummies.html
// ============================================================================

#ifndef __FLUID_H__
#define __FLUID_H__

typedef struct FluidCube FluidCube;

FluidCube *FluidCubeCreate(int size, int diffusion, int viscosity, float dt);
void FluidCubeFree(FluidCube *cube);
void FluidCubeAddDensity(FluidCube *cube, int x, int y, int z, float amount);
void FluidCubeAddVelocity(FluidCube *cube, int x, int y, int z, float amountX, float amountY, float amountZ);
void FluidCubeStep(FluidCube *cube);

#endif // __FLUID_H__
