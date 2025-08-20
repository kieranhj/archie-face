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
#include "lib/mouse.h"
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
static u32 debug_grid = 0;
static u32 debug_update = 0;

static int frame_count = 0;
static int debug_frame_rate;
static int vsyncs_since_last_count;

static int mouseX;
static int mouseY;
u32 vortex_radius = 50;

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

void MakeVortex(u32 param1, u32 param2) {
    gridAddNode(mouseX, mouseY, param1, param2);
}

int main(int argc, char* argv[]){
    // Unused params.
    (void)argc;
    (void)argv;

    // App init.
    init();
    // NB. init() wipes BSS so can't init static vars before this!!

    // Lookup tables.
    printf("Init...");
    MakeSinus();

    // Debug init.
    debug_register_key(RMKey_D, debug_toggle_word, (u32)&debug_display, 0);
    debug_register_key(RMKey_R, debug_toggle_word, (u32)&debug_rasters, 0);
    debug_register_key(RMKey_S, debug_set_word, (u32)&debug_step, 1);
    debug_register_key(RMKey_Space, debug_toggle_word, (u32)&debug_do_tick, 0);
    debug_register_key(RMKey_M, MakeParticles, 0, 0);
    debug_register_key(RMKey_N, MakeNoiseGrid, 0, 0);
    debug_register_key(RMKey_Z, MakeZeroGrid, 0, 0);
    debug_register_key(RMKey_V, MakeVortex, 1, -1);
    debug_register_key(RMKey_B, MakeVortex, -1, 1);
    debug_register_key(RMKey_G, debug_toggle_word, (u32)&debug_grid, 0);
    debug_register_key(RMKey_U, debug_toggle_word, (u32)&debug_update, 0);
    debug_register_key(RMKey_ArrowUp, debug_word_add, (u32)&num_particles, 10);
    debug_register_key(RMKey_ArrowDown, debug_word_add, (u32)&num_particles, -10);
    debug_register_key(RMKey_1, debug_set_word, (u32)&vortex_radius, 20);
    debug_register_key(RMKey_2, debug_set_word, (u32)&vortex_radius, 40);
    debug_register_key(RMKey_3, debug_set_word, (u32)&vortex_radius, 60);
    debug_register_key(RMKey_4, debug_set_word, (u32)&vortex_radius, 80);
    debug_register_key(RMKey_5, debug_set_word, (u32)&vortex_radius, 100);
    debug_register_key(RMKey_6, debug_set_word, (u32)&vortex_radius, 12);
    debug_register_key(RMKey_7, debug_set_word, (u32)&vortex_radius, 140);
    debug_register_key(RMKey_8, debug_set_word, (u32)&vortex_radius, 160);
    debug_register_key(RMKey_9, debug_set_word, (u32)&vortex_radius, 180);

    // Flow field init.
    //MakeNoiseGrid();
    MakeZeroGrid();
    MakeParticles();

    // Triple screen buffering.
    displayed_bank = 0;
    pending_bank = 1;   // display next vsync.
    write_bank = 2;
    v_setDisplayBank(pending_bank);

    last_vsync = vsync_count;

    // ===============================
    // Main loop.
    // ===============================
    while(!k_checkKeypress(KEY_ESCAPE)){

        debug_do_keypress_callbacks();

        u8 mb;
        mouseRead(&mouseX, &mouseY, &mb);

        SET_BORDER(0x000f);

        // ===============================
        // Tick
        // ===============================
        if (debug_do_tick || debug_step) {
            debug_step = 0;

            if (debug_update) updateGrid();

            moveParticles();

            // Frame rate
            frame_count++;

            if ((frame_count % 32) == 0) {
                debug_frame_rate = 50 * 32 / (vsync_count - vsyncs_since_last_count);
                vsyncs_since_last_count = vsync_count;
            }
        }

        // ===============================
        // Vsync - formerly v_waitForVSync();
        // ===============================

        if (++write_bank > Screen_Banks) write_bank=1;      // get next bank for writing.
        while (write_bank == displayed_bank) {}         // block here if we're trying to write to the currently displayed bank.
        v_setWriteBank(write_bank);
        g_framebuffer = v_getScreenAddress();

        vsync_delta = vsync_count - last_vsync;
        last_vsync = vsync_count;

        // ===============================
        // Draw
        // ===============================

        // Clear screen
        SET_BORDER(0x0f00);
        memsetFast((u32*)g_framebuffer, 0, Screen_SizeBytes);

        // Draw screen
        SET_BORDER(0x00f0);

        //plotSinCos();

        if (debug_grid) drawGridDirs();

        //for(int i=0; i < 100; i++) {
        //    plotCurve(randomBetween(0,319), randomBetween(0,255), 32, 64+i);
        //}

        plotParticles();

        // Print some debug info.
        SET_BORDER(0x0fff);

        if (debug_display) {
            char vsync_str[16];
            //sprintf(vsync_str, "%d %d", vsync_delta, vsync_count);
            sprintf(vsync_str, "%d %x %d", debug_frame_rate, mb, num_particles);
            debug_plot_string_mode13(vsync_str);
        }

        plotPoint(mouseX, mouseY, 255);

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
