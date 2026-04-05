// ============================================================================
// Sequence parts.
// TODO: Something more sophisticated when needed.
// ============================================================================

#include "seq-parts.h"
#include "globals.h"
#include "flow-field.h"
#include "emitter.h"
#include "colour.h"
#include "../lib/trig.h"
#include "platform/pc/params.h"

// ============================================================================
// PART 1.
// Emitters from left & right side of the screen.
// Colour ramp left-to-right across the screen.
// ============================================================================

static flow_field_t *field1;
static emitter_t *emitter1;
static emitter_t *emitter2;
static u8 colour_ramp[256];
static float emitter2_rot = 0.0f;

void seq_part1_init()
{
    // Register PC parameter widgets.
    params_clear();

    // Flow field init.
    field1 = flow_field_make(20, 16);
    flow_field_init_with_noise(field1, 0.02f);  // lower values are smoother on a coarse field.
    flow_field_init(field1);  // inits debug.

    // Setup Particle emitters.
    emitter1 = emitter_make(300, 1.0f, 64, 160, 128, 50, 500);
    emitter_set_delta(emitter1, (vec2fix16_t){.x=FLOAT_TO_FIX16(-0.5f), .y=FLOAT_TO_FIX16(0.0f)});
    emitter_set_field(emitter1, field1);
    emitter_set_radius(emitter1, 20, 40);

    emitter2 = emitter_make(300, 1.5f, 255, 256, 256, 30, 200);
    //emitter_set_mouse(emitter2, 1);
    emitter_set_rotation(emitter2, emitter2_rot);
    emitter_set_field(emitter2, field1);
    emitter_set_radius(emitter2, 20, 40);

    // Red to yellow to green to white ramp.
    colour_make_ramp(colour_ramp, 64, COLOUR_MAKE_RGB4(15,0,0), COLOUR_MAKE_RGB4(15,15,0));
    colour_make_ramp(colour_ramp+64, 64, COLOUR_MAKE_RGB4(15,15,0), COLOUR_MAKE_RGB4(0,15,0));
    colour_make_ramp(colour_ramp+128, 64, COLOUR_MAKE_RGB4(0,15,0), COLOUR_MAKE_RGB4(0,15,15));
}

void seq_part1_tick()
{
    vec2fix16_t emitter1_pos = {.x=INT_TO_FIX16(160), .y=INT_TO_FIX16(128)};

    //if (flow_field_rotate_grid) emitter_set_rotation(emitter2, emitter2_rot+=0.1f);

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

void seq_part1_draw()
{
    if (flow_field_show_grid) flow_field_draw(field1);

    //colour_draw_palette();
    emitter_draw_ramp_with_x(emitter2, colour_ramp, 192);
    emitter_draw_ramp_with_x(emitter1, colour_ramp, 192);
}

void seq_part1_kill()
{
    emitter1 = emitter_kill(emitter1);
    emitter2 = emitter_kill(emitter2);
    field1 = flow_field_kill(field1);
}

// ============================================================================
// Part 2.
// Two emitters circling the centre of the screen.
// Particles coloured by age on a rainbow ramp.
// ============================================================================

void seq_part2_init()
{
    // Register PC parameter widgets.
    params_clear();

    // Flow field init.
    field1 = flow_field_make(20, 16);
    flow_field_init_with_noise(field1, 0.02f);  // lower values are smoother on a coarse field.
    flow_field_init(field1);  // inits debug.

    // Setup Particle emitters.
    emitter1 = emitter_make(300, 1.0f, 64, 160, 128, 50, 500);
    emitter_set_delta(emitter1, (vec2fix16_t){.x=FLOAT_TO_FIX16(-0.5f), .y=FLOAT_TO_FIX16(0.0f)});
    emitter_set_field(emitter1, field1);

    emitter2 = emitter_make(300, 1.5f, 255, 256, 256, 30, 200);
    //emitter_set_mouse(emitter2, 1);
    emitter_set_rotation(emitter2, emitter2_rot);
    emitter_set_field(emitter2, field1);

    // Rainbow-ish ramp w/ 192 entries.
    colour_make_ramp(colour_ramp, 32, COLOUR_MAKE_RGB4(0,0,0), COLOUR_MAKE_RGB4(0,15,0));
    colour_make_ramp(colour_ramp+32, 32, COLOUR_MAKE_RGB4(0,15,0), COLOUR_MAKE_RGB4(15,15,0));
    colour_make_ramp(colour_ramp+64, 32, COLOUR_MAKE_RGB4(15,15,0), COLOUR_MAKE_RGB4(15,0,15));
    colour_make_ramp(colour_ramp+96, 32, COLOUR_MAKE_RGB4(15,0,15), COLOUR_MAKE_RGB4(0,0,15));
    colour_make_ramp(colour_ramp+128, 32, COLOUR_MAKE_RGB4(0,0,15), COLOUR_MAKE_RGB4(15,0,0));
    colour_make_ramp(colour_ramp+160, 32, COLOUR_MAKE_RGB4(15,0,0), COLOUR_MAKE_RGB4(15,15,15));
}

void seq_part2_tick()
{
    vec2fix16_t emitter1_pos = {.x=INT_TO_FIX16(160), .y=INT_TO_FIX16(128)};

    //if (flow_field_rotate_grid) emitter_set_rotation(emitter2, emitter2_rot+=0.1f);

    // Update any emitter properties.
    fix16_t a = FIX16_FRACTION(g_frame_count,2);  // Use frame count as brad.
    fix16_t r = INT_TO_FIX16(80);           // Radius
    emitter1_pos.x = INT_TO_FIX16(160) + FIX16_MUL(sin_fix16(a), r);
    emitter1_pos.y = INT_TO_FIX16(128) + FIX16_MUL(cos_fix16(a), r);
    emitter_set_origin(emitter1, emitter1_pos);

    r = INT_TO_FIX16(50);           // Radius
    emitter1_pos.x = INT_TO_FIX16(160) + FIX16_MUL(sin_fix16(-a), r);
    emitter1_pos.y = INT_TO_FIX16(128) + FIX16_MUL(cos_fix16(-a), r);
    emitter_set_origin(emitter2, emitter1_pos);
    
    // Tick the emitters to move the particles.
    emitter_tick(emitter1);
    emitter_tick(emitter2);
}

void seq_part2_draw()
{
    if (flow_field_show_grid) flow_field_draw(field1);

    //colour_draw_palette();
    emitter_draw_ramp_with_age(emitter2, colour_ramp, 192);
    emitter_draw_ramp_with_age(emitter1, colour_ramp, 192);
}

void seq_part2_kill()
{
    emitter1 = emitter_kill(emitter1);
    emitter2 = emitter_kill(emitter2);
    field1 = flow_field_kill(field1);
}

// ============================================================================
// Part 3.
// Two emitters circling the centre of the screen.
// New fields.
// ============================================================================

static void make_sine_field(flow_field_t *grid, fix16_t scale)
{
    for(int i=0; i<flow_field_get_cols(grid); i++)
    {
        for(int j=0; j<flow_field_get_rows(grid); j++)
        {
            fix16_t a = 256 * (sin_fix16(i*scale) + sin_fix16(j*scale));
            flow_field_set_angle(grid, i, j, a);
        }
    }
}

static void add_two_fields(flow_field_t *grid1, flow_field_t *grid2, fix16_t blend)
{
    for(int i=0; i<flow_field_get_cols(grid1); i++)
    {
        for(int j=0; j<flow_field_get_rows(grid1); j++)
        {
            fix16_t a = flow_field_get_angle(grid1, i, j) + FIX16_MUL(flow_field_get_angle(grid2, i, j), blend);
            flow_field_set_angle(grid1, i, j, a);
        }
    }
}

void seq_part3_init()
{
    // Register PC parameter widgets.
    params_clear();

    // Flow field init.
    field1 = flow_field_make(40, 32);
    flow_field_init(field1);  // inits debug.
    flow_field_init_with_noise(field1, 0.05f);
    //make_sine_field(field1, INT_TO_FIX16(5));

    flow_field_t *field2 = flow_field_make(40, 32);
    flow_field_init_with_noise(field2, 0.02f);
    //make_sine_field(field2, INT_TO_FIX16(10));
    add_two_fields(field1, field2, FLOAT_TO_FIX16(0.2f));
    flow_field_kill(field2);

    // Setup Particle emitters.
    emitter1 = emitter_make(300, 1.0f, 64, 160, 128, 50, 500);
    emitter_set_delta(emitter1, (vec2fix16_t){.x=FLOAT_TO_FIX16(-0.5f), .y=FLOAT_TO_FIX16(0.0f)});
    emitter_set_field(emitter1, field1);

    emitter2 = emitter_make(300, 1.5f, 255, 256, 256, 30, 200);
    //emitter_set_mouse(emitter2, 1);
    emitter_set_rotation(emitter2, emitter2_rot);
    emitter_set_field(emitter2, field1);

    // Rainbow-ish ramp w/ 192 entries.
    colour_make_ramp(colour_ramp, 32, COLOUR_MAKE_RGB4(0,0,0), COLOUR_MAKE_RGB4(0,15,0));
    colour_make_ramp(colour_ramp+32, 32, COLOUR_MAKE_RGB4(0,15,0), COLOUR_MAKE_RGB4(15,15,0));
    colour_make_ramp(colour_ramp+64, 32, COLOUR_MAKE_RGB4(15,15,0), COLOUR_MAKE_RGB4(15,0,15));
    colour_make_ramp(colour_ramp+96, 32, COLOUR_MAKE_RGB4(15,0,15), COLOUR_MAKE_RGB4(0,0,15));
    colour_make_ramp(colour_ramp+128, 32, COLOUR_MAKE_RGB4(0,0,15), COLOUR_MAKE_RGB4(15,0,0));
    colour_make_ramp(colour_ramp+160, 32, COLOUR_MAKE_RGB4(15,0,0), COLOUR_MAKE_RGB4(15,15,15));
}

void seq_part3_tick()
{
    vec2fix16_t emitter1_pos = {.x=INT_TO_FIX16(160), .y=INT_TO_FIX16(128)};

    //if (flow_field_rotate_grid) emitter_set_rotation(emitter2, emitter2_rot+=0.1f);

    // Update any emitter properties.
    fix16_t a = FIX16_FRACTION(g_frame_count,2);  // Use frame count as brad.
    fix16_t r = INT_TO_FIX16(80);           // Radius
    emitter1_pos.x = INT_TO_FIX16(160) + FIX16_MUL(sin_fix16(a), r);
    emitter1_pos.y = INT_TO_FIX16(128) + FIX16_MUL(cos_fix16(a), r);
    emitter_set_origin(emitter1, emitter1_pos);

    r = INT_TO_FIX16(50);           // Radius
    emitter1_pos.x = INT_TO_FIX16(160) + FIX16_MUL(sin_fix16(-a), r);
    emitter1_pos.y = INT_TO_FIX16(128) + FIX16_MUL(cos_fix16(-a), r);
    emitter_set_origin(emitter2, emitter1_pos);
    
    // Tick the emitters to move the particles.
    emitter_tick(emitter1);
    emitter_tick(emitter2);
}

void seq_part3_draw()
{
    if (flow_field_show_grid) flow_field_draw(field1);

    //colour_draw_palette();
    emitter_draw_ramp_with_age(emitter2, colour_ramp, 192);
    emitter_draw_ramp_with_age(emitter1, colour_ramp, 192);
}

void seq_part3_kill()
{
    emitter1 = emitter_kill(emitter1);
    emitter2 = emitter_kill(emitter2);
    field1 = flow_field_kill(field1);
}

// ============================================================================
// Part 4.
// Reuse part 2 emitters but project onto a plane.
//   Two emitters circling the centre of the screen.
//   Particles coloured by age on a rainbow ramp.
// ============================================================================

static float seq_part4_cam_pos_y = 64.0f;
static float seq_part4_cam_pos_z = 256.0f + 10.0f;

void seq_part4_init()
{
    // Register PC parameter widgets.
    params_clear();
    param_float("cam_pos_y", &seq_part4_cam_pos_y, 0.0f, 128.0f, 0.1f);
    param_float("cam_pos_z", &seq_part4_cam_pos_z, 0.0f, 512.0f, 0.1f);

    // Flow field init.
    field1 = flow_field_make(20, 16);
    flow_field_init_with_noise(field1, 0.02f);  // lower values are smoother on a coarse field.
    flow_field_init(field1);  // inits debug.

    // Setup Particle emitters.
    emitter1 = emitter_make(300, 1.0f, 64, 160, 128, 50, 500);
    emitter_set_delta(emitter1, (vec2fix16_t){.x=FLOAT_TO_FIX16(-0.5f), .y=FLOAT_TO_FIX16(0.0f)});
    emitter_set_field(emitter1, field1);

    emitter2 = emitter_make(300, 1.5f, 255, 256, 256, 30, 200);
    //emitter_set_mouse(emitter2, 1);
    //emitter_set_rotation(emitter2, emitter2_rot);
    emitter_set_field(emitter2, field1);

    // Rainbow-ish ramp w/ 192 entries.
    colour_make_ramp(colour_ramp, 32, COLOUR_MAKE_RGB4(0,0,0), COLOUR_MAKE_RGB4(0,15,0));
    colour_make_ramp(colour_ramp+32, 32, COLOUR_MAKE_RGB4(0,15,0), COLOUR_MAKE_RGB4(15,15,0));
    colour_make_ramp(colour_ramp+64, 32, COLOUR_MAKE_RGB4(15,15,0), COLOUR_MAKE_RGB4(15,0,15));
    colour_make_ramp(colour_ramp+96, 32, COLOUR_MAKE_RGB4(15,0,15), COLOUR_MAKE_RGB4(0,0,15));
    colour_make_ramp(colour_ramp+128, 32, COLOUR_MAKE_RGB4(0,0,15), COLOUR_MAKE_RGB4(15,0,0));
    colour_make_ramp(colour_ramp+160, 32, COLOUR_MAKE_RGB4(15,0,0), COLOUR_MAKE_RGB4(15,15,15));
}

void seq_part4_tick()
{
    vec2fix16_t emitter1_pos = {.x=INT_TO_FIX16(160), .y=INT_TO_FIX16(128)};

    //if (flow_field_rotate_grid) emitter_set_rotation(emitter2, emitter2_rot+=0.1f);

    // Update any emitter properties.
    fix16_t a = FIX16_FRACTION(g_frame_count,2);  // Use frame count as brad.
    fix16_t r = INT_TO_FIX16(80);           // Radius
    emitter1_pos.x = INT_TO_FIX16(160) + FIX16_MUL(sin_fix16(a), r);
    emitter1_pos.y = INT_TO_FIX16(128) + FIX16_MUL(cos_fix16(a), r);
    emitter_set_origin(emitter1, emitter1_pos);

    r = INT_TO_FIX16(50);           // Radius
    emitter1_pos.x = INT_TO_FIX16(160) + FIX16_MUL(sin_fix16(-a), r);
    emitter1_pos.y = INT_TO_FIX16(128) + FIX16_MUL(cos_fix16(-a), r);
    emitter_set_origin(emitter2, emitter1_pos);
    
    // Tick the emitters to move the particles.
    emitter_tick(emitter1);
    emitter_tick(emitter2);
}

void seq_part4_draw()
{
   if (flow_field_show_grid) flow_field_draw(field1);

    //colour_draw_palette();
    emitter_draw_as_plane(emitter2, seq_part4_cam_pos_y, seq_part4_cam_pos_z);
    emitter_draw_as_plane(emitter1, seq_part4_cam_pos_y, seq_part4_cam_pos_z);
}

void seq_part4_kill()
{
    emitter1 = emitter_kill(emitter1);
    emitter2 = emitter_kill(emitter2);
    field1 = flow_field_kill(field1);
}
