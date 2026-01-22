// ============================================================================
// Sequence.
// TODO: Something more sophisticated when needed.
// ============================================================================

#include "seq.h"
#include "flow-field.h"
#include "globals.h"
#include "emitter.h"
#include "colour.h"
#include "../lib/trig.h"

static void seq_part1_init();
static void seq_part1_tick();
static void seq_part1_draw();
static void seq_part1_kill();

typedef void (*seq_func)();

#define SEQ_NUM_PARTS 1

static seq_func seq_part_table[SEQ_NUM_PARTS][4] =
{
    { seq_part1_init, seq_part1_tick, seq_part1_draw, seq_part1_kill }
};

static int seq_part_no = -1;
static int seq_new_part_no = -1;

void sequence_init()
{
    sequence_set_part(0);   // First part.
    // TODO: Debug keys to change part.
}

void sequence_tick()
{
    if (seq_new_part_no != -1)
    {
        if (seq_part_no != -1) (seq_part_table)[seq_part_no][3]();      // Kill previous part.
        seq_part_no = seq_new_part_no;
        seq_new_part_no = -1;
        if (seq_part_no != -1) (seq_part_table)[seq_part_no][0]();      // Init new part.
    }

    if (seq_part_no != -1) (seq_part_table)[seq_part_no][1]();          // Tick part.
}

void sequence_draw()
{
    if (seq_part_no != -1) (seq_part_table)[seq_part_no][2]();          // Draw part.
}

void sequence_set_part(int new_part_no)
{
    seq_new_part_no = new_part_no;
}

void sequence_kill()
{
    sequence_set_part(-1);
}

// ============================================================================

static flow_field_t *seq_part1_field1;
static emitter_t *seq_part1_emitter1;
static emitter_t *seq_part1_emitter2;
static u8 colour_ramp[256];
static float emitter2_rot = 0.0f;

static void seq_part1_init()
{
    // Flow field init.
    seq_part1_field1 = flow_field_make(20, 16);
    flow_field_t *field1 = seq_part1_field1;
    flow_field_init_with_noise(field1, 0.02f);  // lower values are smoother on a coarse field.
    flow_field_init(field1);  // inits debug.

    // Setup Particle emitters.
    seq_part1_emitter1 = emitter_make(300, 1.0f, 64, 160, 128, 50, 500);
    emitter_t *emitter1 = seq_part1_emitter1;
    emitter_set_delta(emitter1, (vec2fix16_t){.x=FLOAT_TO_FIX16(-0.5f), .y=FLOAT_TO_FIX16(0.0f)});
    emitter_set_field(emitter1, field1);
    emitter_set_radius(emitter1, 20, 40);


    seq_part1_emitter2 = emitter_make(300, 1.5f, 255, 256, 256, 30, 200);
    emitter_t *emitter2 = seq_part1_emitter2;
    //emitter_set_mouse(emitter2, 1);
    emitter_set_rotation(emitter2, emitter2_rot);
    emitter_set_field(emitter2, field1);
    emitter_set_radius(emitter2, 20, 40);

    #if 0
    colour_make_ramp(colour_ramp, 32, COLOUR_MAKE_RGB4(0,0,0), COLOUR_MAKE_RGB4(0,15,0));
    colour_make_ramp(colour_ramp+32, 32, COLOUR_MAKE_RGB4(0,15,0), COLOUR_MAKE_RGB4(15,15,0));
    colour_make_ramp(colour_ramp+64, 32, COLOUR_MAKE_RGB4(15,15,0), COLOUR_MAKE_RGB4(15,0,15));
    colour_make_ramp(colour_ramp+96, 32, COLOUR_MAKE_RGB4(15,0,15), COLOUR_MAKE_RGB4(0,0,15));
    colour_make_ramp(colour_ramp+128, 32, COLOUR_MAKE_RGB4(0,0,15), COLOUR_MAKE_RGB4(15,0,0));
    colour_make_ramp(colour_ramp+160, 32, COLOUR_MAKE_RGB4(15,0,0), COLOUR_MAKE_RGB4(15,15,15));
    #else
    colour_make_ramp(colour_ramp, 64, COLOUR_MAKE_RGB4(15,0,0), COLOUR_MAKE_RGB4(15,15,0));
    colour_make_ramp(colour_ramp+64, 64, COLOUR_MAKE_RGB4(15,15,0), COLOUR_MAKE_RGB4(0,15,0));
    colour_make_ramp(colour_ramp+128, 64, COLOUR_MAKE_RGB4(0,15,0), COLOUR_MAKE_RGB4(0,15,15));
    #endif
}

static void seq_part1_tick()
{
    emitter_t *emitter1 = seq_part1_emitter1;
    emitter_t *emitter2 = seq_part1_emitter2;
    flow_field_t *field1 = seq_part1_field1;
    vec2fix16_t emitter1_pos = {.x=INT_TO_FIX16(160), .y=INT_TO_FIX16(128)};

    if (flow_field_rotate_grid) emitter_set_rotation(emitter2, emitter2_rot+=0.1f);

    // Update any emitter properties.
    fix16_t a = FIX16_FRACTION(g_frame_count,2);  // Use frame count as brad.
    fix16_t r = INT_TO_FIX16(80);           // Radius
    emitter1_pos.x = INT_TO_FIX16(300);// + FIX16_MUL(sin_fix16(a), r);
    emitter1_pos.y = INT_TO_FIX16(128) + FIX16_MUL(cos_fix16(a), r);
    emitter_set_origin(emitter1, emitter1_pos);

    r = INT_TO_FIX16(80);           // Radius
    emitter1_pos.x = INT_TO_FIX16(20);// + FIX16_MUL(sin_fix16(-a), r);
    emitter1_pos.y = INT_TO_FIX16(128) + FIX16_MUL(cos_fix16(-a), r);
    emitter_set_origin(emitter2, emitter1_pos);
    
    // Tick the emitters to move the particles.
    emitter_tick(emitter1);
    emitter_tick(emitter2);
}

static void seq_part1_draw()
{
    emitter_t *emitter1 = seq_part1_emitter1;
    emitter_t *emitter2 = seq_part1_emitter2;
    flow_field_t *field1 = seq_part1_field1;

    if (flow_field_show_grid) flow_field_draw(field1);

    //colour_draw_palette();
    emitter_draw_with_ramp(emitter2, colour_ramp, 192);
    emitter_draw_with_ramp(emitter1, colour_ramp, 192);
}

static void seq_part1_kill()
{
    seq_part1_emitter1 = emitter_kill(seq_part1_emitter1);
    seq_part1_emitter2 = emitter_kill(seq_part1_emitter2);
    seq_part1_field1 = flow_field_kill(seq_part1_field1);
}

// ============================================================================

