// ============================================================================
// Video lib (probably belongs in ArchieSDK.)
// ============================================================================

#include "archie/swi.h"
#include "video.h"

void v_setDisplayBank(int bank) {
    asm volatile("mov r0, " swiToConst(OSByte_WriteDisplayBank) "\n"
                 "mov r1, %0\n"
                 "swi " swiToConst(OS_Byte)
                :            // outputs
                : "r"(bank)  // inputs 
                : "r0", "r1", "cc"); // clobbers
}

void v_setWriteBank(int bank) {
    asm volatile("mov r0, " swiToConst(OSByte_WriteVDUBank) "\n"
                 "mov r1, %0\n"
                 "swi " swiToConst(OS_Byte)
                :            // outputs
                : "r"(bank)  // inputs 
                : "r0", "r1", "cc"); // clobbers
}

void v_claimEventHandler(event_handler func) {
    asm volatile("mov r0, " swiToConst(EventV) "\n"
                 "mov r1, %0\n"
                 "mov r2, #0\n"
                 "swi " swiToConst(OS_Claim)
                :            // outputs
                : "r"(func)  // inputs 
                : "r0", "r1", "r2", "cc"); // clobbers
}

void v_releaseEventHandler(event_handler func) {
    asm volatile("mov r0, " swiToConst(EventV) "\n"
                 "mov r1, %0\n"
                 "mov r2, #0\n"
                 "swi " swiToConst(OS_Release)
                :            // outputs
                : "r"(func)  // inputs 
                : "r0", "r1", "r2", "cc"); // clobbers
}
