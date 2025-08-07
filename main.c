// ============================================================================
// Archie-Face: a Acorn Archimedes demo/trackmo framework in C!
// ============================================================================

// TODO: _DEBUG define :)

#include "src/globals.h"

// C libraries.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// ArchieSDK libraries.
#include "archie/swi.h"
#include "archie/video.h"
#include "archie/keyboard.h"

// My libraries. :)
#include "lib/debug.h"
#include "lib/mem.h"
#include "lib/plot.h"
#include "lib/trig.h"
#include "lib/video.h"
#include "lib/vector.h"

// App modules.
#include "src/flow-field.h"

u8* g_framebuffer = NULL;               // TODO: Should this be const?
static int write_bank;
volatile int pending_bank = 0;          // updated during interrupt!
volatile int displayed_bank;            // updated during interrupt!
volatile int vsync_count = 0;           // updated during interrupt!
static int vsync_delta;
static int last_vsync;

// TODO: Put these somewhere?
static u32 debug_display = 1;
static u32 debug_do_tick = 1;
static u32 debug_step = 0;
u32 debug_rasters = 1;

static int frame_count = 0;
static int debug_frame_rate;
static int vsyncs_since_last_count ;

void eventv_handler(int event_no, int event_param1, int event_param2, int event_param3, int event_param4) {
    // TODO: Probably want to preserve all registers used in the event handler?
    (void) event_param3;
    (void) event_param4;
    if (event_no == Event_VSync) {
        vsync_count++;

        // Keep track of which screen bank we are displaying.
        if (pending_bank) {
            displayed_bank = pending_bank;
            pending_bank = 0;
        }
    } else if (event_no == Event_KeyPressed) {
        debug_handle_keypress(event_param1, event_param2);
    }
}

void quit() {
    v_setDisplayBank(write_bank);
    v_setWriteBank(write_bank);

    v_disableEvent(Event_VSync);
    v_disableEvent(Event_KeyPressed);
    v_releaseEventHandler(eventv_handler);

    v_waitForVSync();
}

void init() {
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

    // Register exit callback.
    atexit(quit);
}

int main(int argc, char* argv[]){
    // Unused params.
    (void)argc;
    (void)argv;

    // App init.
    init();

    // Debug init.
    debug_register_key(RMKey_D, debug_toggle_byte, (u32)&debug_display, 0);
    debug_register_key(RMKey_R, debug_toggle_byte, (u32)&debug_rasters, 0);
    debug_register_key(RMKey_S, debug_set_byte, (u32)&debug_step, 1);
    debug_register_key(RMKey_Space, debug_toggle_byte, (u32)&debug_do_tick, 0);

    // Flow field init.
    MakePermutation();
    MakeGrid();
    MakeParticles();

    // Triple screen buffering.
    displayed_bank = 0;
    pending_bank = 1;   // display next vsync.
    write_bank = 2;
    v_setDisplayBank(pending_bank);

    last_vsync = vsync_count;

    // Main loop.
    while(!k_checkKeypress(KEY_ESCAPE)){

        debug_do_keypress_callbacks();

        SET_BORDER(0x000f);

        if (debug_do_tick || debug_step) {
            debug_step = 0;

            // Tick
            updateGrid();
            moveParticles();

            // Frame rate
            frame_count++;

            if ((frame_count % 32) == 0) {
                debug_frame_rate = 50 * 32 / (vsync_count - vsyncs_since_last_count);
                vsyncs_since_last_count = vsync_count;
            }
        }

        // Vsync - formerly v_waitForVSync();
        if (++write_bank > Screen_Banks) write_bank=1;      // get next bank for writing.
        while (write_bank == displayed_bank) {}         // block here if we're trying to write to the currently displayed bank.
        v_setWriteBank(write_bank);
        g_framebuffer = v_getScreenAddress();

        vsync_delta = vsync_count - last_vsync;
        last_vsync = vsync_count;

        SET_BORDER(0x0f00);

        // Clear screen
        memsetFast((u32*)g_framebuffer, 0, Screen_SizeBytes);

        SET_BORDER(0x00f0);

        // Draw screen
        //plotSinCos();
        //drawGridDirs();

        //for(int i=0; i < 100; i++) {
        //    plotCurve(randomBetween(0,319), randomBetween(0,255), 32, 64+i);
        //}

        plotParticles();

        SET_BORDER(0x0fff);

        // Print some debug info.
        if (debug_display) {
            char vsync_str[16];
            //sprintf(vsync_str, "%d %d", vsync_delta, vsync_count);
            sprintf(vsync_str, "%d", debug_frame_rate);
            debug_plot_string_mode13(vsync_str);
        }

        SET_BORDER(0x0000);

        // Flip screens
        while (pending_bank) {}         // block here if pending display at next vsync (we got too far ahead).
        pending_bank = write_bank;
        v_setDisplayBank(write_bank);   // screen won't be displayed until vsync.
    }

    KillGrid();

	return 0;
}

INCBIN(debug_font, "data/lib/Spectrum.bin");
