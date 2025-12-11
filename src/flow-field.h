// ============================================================================
// Flow field.
// ============================================================================

#ifndef __FLOW_FIELD_H__
#define __FLOW_FIELD_H__

#define GRID_ROWS       16
#define GRID_COLS       20
#define GRID_STEPX      (320/GRID_COLS)
#define GRID_STEPY      (256/GRID_ROWS)
#define GRID_OFFX       (GRID_STEPX/2)
#define GRID_OFFY       (GRID_STEPY/2)

#define MAX_PARTICLES   1024

extern int num_particles;

void MakeNoiseGrid();
void MakeZeroGrid();
void noise_init();
void drawGrid();
void updateGrid();
void drawGridDirs();
void plotParticles();
void moveParticles();
void MakeParticles();
void KillGrid();

void gridAddAttractor(int x, int y);
void gridAddNode(int x, int y, int fx, int fy, int radius);
void plotCurve(int x0, int y0, int num_steps, int col);

#endif // __FLOW_FIELD_H__
