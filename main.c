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

u8* framebuffer = NULL;
int screen_bank = 0;
int vsync_count = 0;

void eventv_handler(int event_no, int event_param1, int event_param2, int event_param3, int event_param4) {
    (void) event_param1;
    (void) event_param2;
    (void) event_param3;
    (void) event_param4;
    if (event_no != 4) return;
    vsync_count++;
}

void quit(){
    v_disableVSync();   // disables vsync event
    v_releaseEventHandler(eventv_handler);
    //flush last vsync
    v_waitForVSync();
}

void init(){ 
    // TODO: Initialise screen RAM size for num_banks.

    v_setMode(13);
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

    screen_bank = 0;    // write bank

    // Main loop.
    while(!k_checkKeypress(KEY_ESCAPE)){

        // Flip screens
        v_setDisplayBank(1+screen_bank);
        screen_bank ^= 1;
        v_setWriteBank(1+screen_bank);

        // Tick
        updateGrid();
        moveParticles();

        // Vsync
        v_waitForVSync();

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
    }

    KillGrid();

    v_setDisplayBank(1+screen_bank);

	return 0;
}
