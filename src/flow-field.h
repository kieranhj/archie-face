// ============================================================================
// Flow field.
// ============================================================================

#ifndef __FLOW_FIELD_H__
#define __FLOW_FIELD_H__

#define GRID_ROWS 32
#define GRID_COLS 40
#define GRID_STEPX (320/GRID_COLS)
#define GRID_STEPY (256/GRID_ROWS)
#define GRID_OFFX (GRID_STEPX/2)
#define GRID_OFFY (GRID_STEPY/2)

#define NUM_PARTICLES 256

void MakeGrid();
void MakePermutation();
void drawGrid();
void updateGrid();
void drawGridDirs();
void plotParticles();
void moveParticles();
void MakeParticles();
void KillGrid();

#endif // __FLOW_FIELD_H__
