// ============================================================================
// Particle Emitter.
// TODO: Make sure only used features are paid for at runtime, eg. delta, rotation etc.
// ============================================================================

#include "emitter.h"
#include "globals.h"
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
    vec2fix16_t     pos;
    int             birth;
};

struct emitter_s
{
    int             max_particles;
    u8              colour;
    u8              mouse;      // Use mouse pos.
    fix16_t         speed;      // NB. negative speed works!
    vec2fix16_t     origin;
    vec2fix16_t     delta;
    fix16_t         rotation;
    int             radius_x;
    int             radius_y;
    int             max_age;
    flow_field_t *  field;
    particle_t      particles[0];
};

extern int g_frame_count;

static inline void emitter_particle_init(emitter_t *e, particle_t *p)
{
    p->pos.x = e->origin.x + INT_TO_FIX16(rand_between(0, 2*e->radius_x) - e->radius_x);
    p->pos.y = e->origin.y + INT_TO_FIX16(rand_between(0, 2*e->radius_y) - e->radius_y);
    p->birth = g_frame_count;
}

// TODO: Debug.
//    debug_register_key(RMKey_ArrowUp, debug_word_add, (u32)&flow_field_num_particles, 10);
//    debug_register_key(RMKey_ArrowDown, debug_word_add, (u32)&flow_field_num_particles, -10);

emitter_t *emitter_make(int max_particles, float speed, u8 colour, int ox, int oy, int radius, int max_age)
{
    emitter_t *emitter = malloc(sizeof(struct emitter_s) + max_particles * sizeof(struct particle_s));

    emitter->max_particles = max_particles;
    emitter->colour = colour;
    emitter->mouse = 0;
    emitter->speed = FLOAT_TO_FIX16(speed);
    emitter->origin.x = INT_TO_FIX16(ox);
    emitter->origin.y = INT_TO_FIX16(oy);
    emitter->delta.x = 0;
    emitter->delta.y = 0;
    emitter->rotation = 0;
    emitter->radius_x = radius;
    emitter->radius_y = radius;
    emitter->max_age = max_age;
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

void emitter_set_origin(emitter_t *emitter, vec2fix16_t origin)
{
    emitter->origin.x = origin.x;
    emitter->origin.y = origin.y;
}

void emitter_set_delta(emitter_t *emitter, vec2fix16_t delta)
{
    emitter->delta.x = delta.x;
    emitter->delta.y = delta.y;
}

void emitter_set_rotation(emitter_t *emitter, float rot)
{
    emitter->rotation = FLOAT_TO_FIX16(rot) & 0x00ffffff;
}

void emitter_set_field(emitter_t *emitter, flow_field_t *field)
{
    emitter->field = field;
}

void emitter_set_radius(emitter_t *emitter, int radius_x, int radius_y)
{
    emitter->radius_x = radius_x;
    emitter->radius_y = radius_y;
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

    fix16_t offx = emitter->delta.x;
    fix16_t offy = emitter->delta.y;
    fix16_t speed = emitter->speed;
    flow_field_t *f = emitter->field;
    fix16_t rotation = emitter->rotation;
    int max_age = emitter->max_age;

    for(int i = 0; i < emitter->max_particles; i++)
    {
        particle_t *p = &emitter->particles[i];
        fix16_t a;

        int age = g_frame_count - p->birth;

        if (age < max_age && flow_field_get_nearest_angle(f, p->pos.x, p->pos.y, &a))
        {
            fix16_t dx = cos_fix16(a + rotation);             // [-1.0, 1.0]  [s1.16]
            fix16_t dy = sin_fix16(a + rotation);             // [-1.0, 1.0]  [s1.16]

            p->pos.x += FIX16_MUL(dx, speed) + offx;
            p->pos.y += FIX16_MUL(dy, speed) + offy;
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
        plot_point(FIX16_TO_INT(p->pos.x), FIX16_TO_INT(p->pos.y), col);
    }
}

void emitter_draw_ramp_with_age(emitter_t *emitter, const u8 *ramp, int size)
{
    for(int i = 0; i < emitter->max_particles; i++)
    {
        particle_t *p = &emitter->particles[i];
        int age = g_frame_count - p->birth;
        if (age>=size) age=size-1;
        plot_point(FIX16_TO_INT(p->pos.x), FIX16_TO_INT(p->pos.y), ramp[age]);
    }
}

void emitter_draw_ramp_with_x(emitter_t *emitter, const u8 *ramp, int size)
{
    for(int i = 0; i < emitter->max_particles; i++)
    {
        particle_t *p = &emitter->particles[i];
        int px = FIX16_TO_INT(p->pos.x);
        int x = px / 2;//size * px / Screen_Width;
        if (x>size) x=size-1;
        plot_point(px, FIX16_TO_INT(p->pos.y), ramp[x]);
    }
}
