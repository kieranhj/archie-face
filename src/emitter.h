// ============================================================================
// Particle Emitter.
// ============================================================================

#ifndef __EMITTER_H__
#define __EMITTER_H__

#include "archie/SDKTypes.h"
#include "flow-field.h"
#include "../lib/vector.h"

typedef struct emitter_s emitter_t;

emitter_t *emitter_make(int max_particles, float speed, u8 colour, int originx, int originy, int radius, int max_age);
void *emitter_kill(emitter_t *emitter);

void emitter_set_field(emitter_t *emitter, flow_field_t *field);
void emitter_set_mouse(emitter_t *emitter, int attach);
void emitter_set_origin(emitter_t *emitter, vec2fix16_t origin);
void emitter_set_delta(emitter_t *emitter, vec2fix16_t delta);
void emitter_set_rotation(emitter_t *emitter, float rot);       // Or fix16 brad?
void emitter_set_radius(emitter_t *emitter, int radius_x, int radius_y);

void emitter_tick(emitter_t *emitter);
void emitter_draw(emitter_t *emitter);
void emitter_draw_ramp_with_age(emitter_t *emitter, const u8 *ramp, int size);
void emitter_draw_ramp_with_x(emitter_t *emitter, const u8 *ramp, int size);

void emitter_draw_as_plane(emitter_t *emitter, float cam_y, float cam_z, int plane_width);

#endif // __EMITTER_H__
