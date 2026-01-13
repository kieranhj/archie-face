// ============================================================================
// Particle Emitter.
// TODO: Make sure only used features are paid for at runtime, eg. offset, rotation etc.
// ============================================================================

#include "emitter.h"
#include "flow-field.h"
#include "../lib/maths.h"
#include "../lib/trig.h"
#include "../lib/plot.h"
#include "../lib/mouse.h"
#include "../lib/vector.h"

#include <stdlib.h>

typedef struct particle_s particle_t;

struct particle_s
{
    fix16_t x;  // TODO: Use vec2fp?
    fix16_t y;
};

struct emitter_s
{
    int         max_particles;
    u8          colour;
    u8          mouse;      // Use mouse pos.
    fix16_t     speed;
    vec2fp      origin;
    vec2fp      offset;
    fix16_t     rotation;
    int         radius;
    flow_field_t *field;
    particle_t  particles[0];
};

static inline void emitter_particle_init(emitter_t *e, particle_t *p)
{
    p->x = e->origin.x + INT_TO_FIX16(rand_between(0, 2*e->radius) - e->radius);
    p->y = e->origin.y + INT_TO_FIX16(rand_between(0, 2*e->radius) - e->radius);
}

// TODO: Debug.
//    debug_register_key(RMKey_ArrowUp, debug_word_add, (u32)&flow_field_num_particles, 10);
//    debug_register_key(RMKey_ArrowDown, debug_word_add, (u32)&flow_field_num_particles, -10);

emitter_t *emitter_make(int max_particles, float speed, u8 colour, int ox, int oy, int radius)
{
    emitter_t *emitter = malloc(sizeof(struct emitter_s) + max_particles * sizeof(struct particle_s));

    emitter->max_particles = max_particles;
    emitter->colour = colour;
    emitter->mouse = 0;
    emitter->speed = FLOAT_TO_FIX16(speed);
    emitter->origin.x = INT_TO_FIX16(ox);
    emitter->origin.y = INT_TO_FIX16(oy);
    emitter->offset.x = 0;
    emitter->offset.y = 0;
    emitter->rotation = 0;
    emitter->radius = radius;
    emitter->field = NULL;

    for(int i = 0; i < max_particles; i++)
    {
        particle_t *p = &emitter->particles[i];
        emitter_particle_init(emitter, p);
    }

    return emitter;
}

void *emitter_kill(emitter_t *emitter)
{
    free(emitter);
    return NULL;
}

void emitter_set_mouse(emitter_t *emitter, int mouse)
{
    emitter->mouse = mouse;
}

void emitter_set_offset(emitter_t *emitter, float offx, float offy)
{
    emitter->offset.x = FLOAT_TO_FIX16(offx);
    emitter->offset.y = FLOAT_TO_FIX16(offy);
}

void emitter_set_rotation(emitter_t *emitter, float rot)
{
    emitter->rotation = FLOAT_TO_FIX16(rot) & 0x00ffffff;
}

void emitter_set_field(emitter_t *emitter, flow_field_t *field)
{
    emitter->field = field;
}

void emitter_tick(emitter_t *emitter)
{
    if (emitter->mouse)
    {
        int mouseX, mouseY;
        u8 mb;
        mouse_read(&mouseX, &mouseY, &mb);
        emitter->origin.x = INT_TO_FIX16(mouseX);
        emitter->origin.y = INT_TO_FIX16(mouseY);
    }

    fix16_t offx = emitter->offset.x;
    fix16_t offy = emitter->offset.y;
    fix16_t speed = emitter->speed;
    flow_field_t *f = emitter->field;
    fix16_t rotation = emitter->rotation;

    for(int i = 0; i < emitter->max_particles; i++)
    {
        particle_t *p = &emitter->particles[i];
        fix16_t a;
        
        if (flow_field_get_angle(f, p->x, p->y, &a))
        {
            fix16_t dx = cos_fix16(a + rotation);             // [-1.0, 1.0]  [s1.16]
            fix16_t dy = sin_fix16(a + rotation);             // [-1.0, 1.0]  [s1.16]

            p->x += FIX16_MUL(dx, speed) + offx;
            p->y += FIX16_MUL(dy, speed) + offy;
        }
        else
        {
            emitter_particle_init(emitter, p);
        }
    }
}

void emitter_draw(emitter_t *emitter)
{
    u8 col = emitter->colour;       // FIX16_TO_INT(a) for colour from direction.

    for(int i = 0; i < emitter->max_particles; i++)
    {
        particle_t *p = &emitter->particles[i];
        plot_point(FIX16_TO_INT(p->x), FIX16_TO_INT(p->y), col);
    }
}
