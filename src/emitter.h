// ============================================================================
// Particle Emitter.
// ============================================================================

#ifndef __EMITTER_H__
#define __EMITTER_H__

#include "archie/SDKTypes.h"

typedef struct emitter_s emitter_t;

emitter_t *emitter_make(int max_particles, float speed, u8 colour, int ox, int oy, int radius);
void emitter_set_mouse(emitter_t *emitter, int attach);
void *emitter_kill(emitter_t *emitter);

void emitter_tick(emitter_t *emitter);
void emitter_draw(emitter_t *emitter);

#endif // __EMITTER_H__
