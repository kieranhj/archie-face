// ============================================================================
// Particle Emitter.
// ============================================================================

#ifndef __EMITTER_H__
#define __EMITTER_H__

#include "archie/SDKTypes.h"
#include "flow-field.h"

typedef struct emitter_s emitter_t;

emitter_t *emitter_make(int max_particles, float speed, u8 colour, int originx, int originy, int radius);
void *emitter_kill(emitter_t *emitter);

void emitter_set_field(emitter_t *emitter, flow_field_t *field);
void emitter_set_mouse(emitter_t *emitter, int attach);
void emitter_set_offset(emitter_t *emitter, float offx, float offy);
void emitter_set_rotation(emitter_t *emitter, float rot);

void emitter_tick(emitter_t *emitter);
void emitter_draw(emitter_t *emitter);

#endif // __EMITTER_H__
