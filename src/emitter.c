// ============================================================================
// Particle Emitter.
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
    fix16_t     ox, oy;     // TODO: Replace with vec2fp.
    // TODO: Offset (normalise?)
    int         radius;
    particle_t  particles[0];
};

static inline void emitter_particle_init(emitter_t *e, particle_t *p)
{
    p->x = e->ox + INT_TO_FIX16(rand_between(0, 2*e->radius) - e->radius);
    p->y = e->oy + INT_TO_FIX16(rand_between(0, 2*e->radius) - e->radius);
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
    emitter->ox = INT_TO_FIX16(ox);
    emitter->oy = INT_TO_FIX16(oy);
    emitter->radius = radius;

    for(int i = 0; i < max_particles; i++)
    {
        particle_t *p = &emitter->particles[i];
        emitter_particle_init(emitter, p);
    }

    return emitter;
}

void emitter_set_mouse(emitter_t *emitter, int mouse)
{
    emitter->mouse = mouse;
}

void *emitter_kill(emitter_t *emitter)
{
    free(emitter);
    return NULL;
}

void emitter_tick(emitter_t *emitter)
{
    if (emitter->mouse)
    {
        int mouseX, mouseY;
        u8 mb;
        mouse_read(&mouseX, &mouseY, &mb);
        emitter->ox = INT_TO_FIX16(mouseX);
        emitter->oy = INT_TO_FIX16(mouseY);
    }

    for(int i = 0; i < emitter->max_particles; i++)
    {
        particle_t *p = &emitter->particles[i];
        fix16_t a;
        
        if (flow_field_get_angle(p->x, p->y, &a))
        {
            fix16_t dx = cos_fix16(a);             // [-1.0, 1.0]  [s1.16]
            fix16_t dy = sin_fix16(a);             // [-1.0, 1.0]  [s1.16]

            p->x += FIX16_MUL(dx, emitter->speed);
            p->y += FIX16_MUL(dy, emitter->speed);
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
