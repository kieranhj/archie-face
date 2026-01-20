// ============================================================================
// Archie-Face: a Acorn Archimedes demo/trackmo framework in C!
// ============================================================================

// TODO: _DEBUG define :)

#include "src/globals.h"

// App modules.
#include "src/flow-field.h"
#include "src/colour.h"
#include "src/emitter.h"

// My libraries. :)
#include "lib/debug.h"
#include "lib/mem.h"
#include "lib/mouse.h"
#include "lib/plot.h"
#include "lib/trig.h"
#include "lib/video.h"
#include "lib/vector.h"

// ArchieSDK libraries.
#include "archie/swi.h"
#include "archie/video.h"
#include "archie/keyboard.h"

// C libraries.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// ============================================================================

u8* g_framebuffer = NULL;               // TODO: Should this be const?
static int write_bank;
volatile int pending_bank = 0;          // updated during interrupt!
volatile int displayed_bank;            // updated during interrupt!
volatile int g_vsync_count = 0;         // updated during interrupt!
static int vsync_delta;
static int last_vsync;

// ============================================================================

// TODO: Put these somewhere? (Or in a debug struct for context passing?)
static u32 debug_display = 1;
static u32 debug_do_tick = 1;
static u32 debug_step = 0;
u32 debug_rasters = 1;

// Main loop vars.
int g_frame_count = 0;
static int debug_frame_rate;
static int vsyncs_since_last_count;

// ============================================================================

void eventv_handler(int event_no, int event_param1, int event_param2, int event_param3, int event_param4)
{
    // TODO: Probably want to preserve all registers used in the event handler?
    (void) event_param3;
    (void) event_param4;
    if (event_no == Event_VSync) {
        g_vsync_count++;

        // Keep track of which screen bank we are displaying.
        if (pending_bank) {
            displayed_bank = pending_bank;
            pending_bank = 0;
        }
    } else if (event_no == Event_KeyPressed) {
        debug_handle_keypress(event_param1, event_param2);
    }
}

void quit()
{
    v_setDisplayBank(write_bank);
    v_setWriteBank(write_bank);

    v_disableEvent(Event_VSync);
    v_disableEvent(Event_KeyPressed);
    v_releaseEventHandler(eventv_handler);

    v_waitForVSync();

    mouse_cursor(0);
}

void init()
{
    // Need to init BSS section.
	extern char __bss_start__[];
	extern char __bss_end__[];

	memset(__bss_start__, 0, (__bss_end__ - __bss_start__));

    // Screen stuffs.
    v_setMode(Screen_Mode);
    u32 screen_ram = v_setScreenMemory(Screen_SizeTotal);
    assert(screen_ram >= Screen_SizeTotal);
    v_disableTextCursor();

    // Events.
    v_claimEventHandler(eventv_handler);
    v_enableEvent(Event_VSync);
    v_enableEvent(Event_KeyPressed);

    // Debug.
    debug_init();

    // Mouse.
    mouse_cursor(1);

    // Register exit callback.
    atexit(quit);
}

// ============================================================================

static u8 colour_ramp[256];

int main(int argc, char* argv[])
{
    // Unused params.
    (void)argc;
    (void)argv;

    // App init.
    init();
    // NB. init() wipes BSS so can't init static vars before this!!

    // Lookup tables.
    printf("Init...   ");
    colour_init_palette();
    trig_init();

    // Debug init.
    debug_register_key(RMKey_D, debug_toggle_word, (u32)&debug_display, 0);
    debug_register_key(RMKey_R, debug_toggle_word, (u32)&debug_rasters, 0);
    debug_register_key(RMKey_S, debug_set_word, (u32)&debug_step, 1);
    debug_register_key(RMKey_Space, debug_toggle_word, (u32)&debug_do_tick, 0);

    // FX init.

        // Flow field init.
        flow_field_t *field1 = flow_field_make(20, 16);
        flow_field_init_with_noise(field1, 0.1f);
        flow_field_init(field1);  // inits debug.

        // Setup Particle emitters.
        emitter_t *emitter1 = emitter_make(300, 1.0f, 64, 160, 128, 50, 500);
        vec2fix16_t emitter1_pos = {.x=INT_TO_FIX16(160), .y=INT_TO_FIX16(128)};
        emitter_set_delta(emitter1, (vec2fix16_t){.x=FLOAT_TO_FIX16(-0.5f), .y=FLOAT_TO_FIX16(0.0f)});
        emitter_set_field(emitter1, field1);

        emitter_t *emitter2 = emitter_make(300, 1.5f, 255, 256, 256, 30, 200);
        float emitter2_rot = 0.0f;
        //emitter_set_mouse(emitter2, 1);
        emitter_set_rotation(emitter2, emitter2_rot);
        emitter_set_field(emitter2, field1);

        colour_make_ramp(colour_ramp, 32, COLOUR_MAKE_RGB4(0,0,0), COLOUR_MAKE_RGB4(0,15,0));
        colour_make_ramp(colour_ramp+32, 32, COLOUR_MAKE_RGB4(0,15,0), COLOUR_MAKE_RGB4(15,15,0));
        colour_make_ramp(colour_ramp+64, 32, COLOUR_MAKE_RGB4(15,15,0), COLOUR_MAKE_RGB4(15,0,15));
        colour_make_ramp(colour_ramp+96, 32, COLOUR_MAKE_RGB4(15,0,15), COLOUR_MAKE_RGB4(0,0,15));
        colour_make_ramp(colour_ramp+128, 32, COLOUR_MAKE_RGB4(0,0,15), COLOUR_MAKE_RGB4(15,0,0));
        colour_make_ramp(colour_ramp+160, 32, COLOUR_MAKE_RGB4(15,0,0), COLOUR_MAKE_RGB4(15,15,15));

    // Triple screen buffering.
    displayed_bank = 0;
    pending_bank = 1;   // display next vsync.
    write_bank = 2;
    v_setDisplayBank(pending_bank);

    last_vsync = g_vsync_count;

    // ===============================
    // Main loop.
    // ===============================
    while(/*g_vsync_count==last_vsync &&*/ !k_checkKeypress(KEY_ESCAPE))
    {
        mouse_tick();
        debug_do_keypress_callbacks();

        SET_BORDER(0x000f);

        // ===============================
        // Tick
        // ===============================
        if (debug_do_tick || debug_step)
        {
            debug_step = 0;

            // FX tick.
            {
                if (flow_field_rotate_grid) emitter_set_rotation(emitter2, emitter2_rot+=0.1f);

                // Update any emitter properties.
                fix16_t a = FIX16_FRACTION(g_frame_count,2);  // Use frame count as brad.
                fix16_t r = INT_TO_FIX16(80);           // Radius
                emitter1_pos.x = INT_TO_FIX16(160) + FIX16_MUL(sin_fix16(a), r);
                emitter1_pos.y = INT_TO_FIX16(128) + FIX16_MUL(cos_fix16(a), r);
                emitter_set_origin(emitter1, emitter1_pos);

                r = INT_TO_FIX16(40);           // Radius
                emitter1_pos.x = INT_TO_FIX16(160) + FIX16_MUL(sin_fix16(-a), r);
                emitter1_pos.y = INT_TO_FIX16(128) + FIX16_MUL(cos_fix16(-a), r);
                emitter_set_origin(emitter2, emitter1_pos);
                
                // Tick the emitters to move the particles.
                emitter_tick(emitter1);
                emitter_tick(emitter2);
            }

            // Frame rate
            g_frame_count++;

            if ((g_frame_count % 32) == 0) {
                debug_frame_rate = 50 * 32 / (g_vsync_count - vsyncs_since_last_count);
                vsyncs_since_last_count = g_vsync_count;
            }
        }

        // ===============================
        // Vsync - formerly v_waitForVSync();
        // ===============================

        if (++write_bank > Screen_Banks) write_bank=1;      // get next bank for writing.
        while (write_bank == displayed_bank) {}         // block here if we're trying to write to the currently displayed bank.
        v_setWriteBank(write_bank);
        g_framebuffer = v_getScreenAddress();

        vsync_delta = g_vsync_count - last_vsync;
        last_vsync = g_vsync_count;

        // ===============================
        // Draw
        // ===============================

        // Clear screen
        SET_BORDER(0x0f00);
        mem_set_fast((u32*)g_framebuffer, 0, Screen_SizeBytes);

        // Draw screen
        SET_BORDER(0x00f0);

        // FX draw.
        {
            if (flow_field_show_grid) flow_field_draw(field1);

            //colour_draw_palette();
            emitter_draw_with_ramp(emitter2, colour_ramp, 192);
            emitter_draw_with_ramp(emitter1, colour_ramp, 192);
        }

        // Print some debug info.
        SET_BORDER(0x0fff);

        int mouseX, mouseY;
        u8 mb;
        mouse_read(&mouseX, &mouseY, &mb);

        if (debug_display)
        {
            char vsync_str[16];
            //sprintf(vsync_str, "%d %d", vsync_delta, g_vsync_count);
            sprintf(vsync_str, "%d %x", debug_frame_rate, mb);
            debug_plot_string_mode13(vsync_str);
        }

        SET_BORDER(0x0000);

        // Flip screens
        while (pending_bank) {}         // block here if pending display at next vsync (we got too far ahead).
        pending_bank = write_bank;
        v_setDisplayBank(write_bank);   // screen won't be displayed until vsync.
    }

    emitter1 = emitter_kill(emitter1);
    emitter2 = emitter_kill(emitter2);
    field1 = flow_field_kill(field1);

	return 0;
}

// ============================================================================

INCBIN(debug_font, "data/lib/Spectrum.bin");

// ============================================================================
