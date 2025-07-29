// ============================================================================
// Archie-Face: a Acorn Archimedes demo/trackmo framework in C!
// ============================================================================

// C libraries.
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <swis.h>
#include <assert.h>

// ArchieSDK libraries.
#include "archie/swi.h"
#include "archie/video.h"
#include "archie/keyboard.h"

// My libraries. :)
#include "lib/mem.h"
#include "lib/plot.h"
#include "lib/trig.h"
#include "lib/video.h"
#include "lib/vector.h"

// App modules.
#include "src/flow-field.h"

#define Screen_SizeBytes 320*256
#define Screen_Banks 3
#define Screen_SizeTotal (Screen_SizeBytes * Screen_Banks)

u8* framebuffer = NULL;
int write_bank = 0;
volatile int pending_bank = 0;          // updated during interrupt!
volatile int displayed_bank = 0;        // updated during interrupt!
volatile int vsync_count = 0;           // updated during interrupt!

void eventv_handler(int event_no, int event_param1, int event_param2, int event_param3, int event_param4) {
    // TODO: Probably want to preserve all registers used in the event handler?
    (void) event_param1;
    (void) event_param2;
    (void) event_param3;
    (void) event_param4;
    if (event_no != 4) return;

    vsync_count++;

    // Keep track of which screen bank we are displaying.
    if (pending_bank) {
        displayed_bank = pending_bank;
        pending_bank = 0;
    }
}

void quit(){
    v_disableVSync();   // disables vsync event
    v_releaseEventHandler(eventv_handler);
    //flush last vsync
    v_waitForVSync();
}

void init(){ 
    v_setMode(13);
    u32 screen_ram = v_setScreenMemory(Screen_SizeTotal);
    assert(screen_ram >= Screen_SizeTotal);
    
    v_disableTextCursor();
    v_claimEventHandler(eventv_handler);
    v_enableVSync();    // enables vsync event

    atexit(quit); //register exit callback
}

int main(int argc, char* argv[]){
    // Unused params.
    (void)argc;
    (void)argv;

    // App init.
    init();

    // Flow field init.
    MakePermutation();
    MakeGrid();
    MakeParticles();

    // Triple screen buffering.
    displayed_bank = 0;
    pending_bank = 1;   // display next vsync.
    write_bank = 2;
    v_setDisplayBank(pending_bank);

    // Main loop.
    while(!k_checkKeypress(KEY_ESCAPE)){

        // Tick
        updateGrid();
        moveParticles();

        // Vsync
        // v_waitForVSync();

        if (++write_bank > Screen_Banks) write_bank=1;      // get next bank for writing.
        while (write_bank == displayed_bank) {}         // block here if we're trying to write to the currently displayed bank.
        v_setWriteBank(write_bank);
        framebuffer = v_getScreenAddress();

        // Clear screen
        memsetFast((u32*)framebuffer, 0, Screen_SizeBytes);

        // Draw screen
        //plotSinCos();
        //drawGridDirs();

        //for(int i=0; i < 100; i++) {
        //    plotCurve(randomBetween(0,319), randomBetween(0,255), 32, 64+i);
        //}

        plotParticles();

        // Flip screens
        while (pending_bank) {}         // block here if pending display at next vsync (we got too far ahead).
        pending_bank = write_bank;
        v_setDisplayBank(write_bank);   // screen won't be displayed until vsync.
    }

    v_setWriteBank(write_bank);

    KillGrid();

	return 0;
}
