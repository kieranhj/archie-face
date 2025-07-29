// ============================================================================
// Video lib (probably belongs in ArchieSDK.)
// ============================================================================

#include "archie/swi.h"
#include "video.h"

#define DynArea_Screen 2

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

u32 v_setScreenMemory(u32 new_size) {
    u32 alloc_size;

    asm volatile("mov r0, " swiToConst(DynArea_Screen) "\n"
                 "swi " swiToConst(OS_ReadDynamicArea) "\n"
                 "mov r2, %0\n"
                 "subs r1, r2, r1\n"
                 "mov r0, " swiToConst(DynArea_Screen) "\n"
                 "swi " swiToConst(OS_ChangeDynamicArea)
                :                           // outputs
                : "r"(new_size)             // inputs 
                : "r0", "r1", "r2", "cc", "memory");  // clobbers

    asm volatile("mov r0, " swiToConst(DynArea_Screen) "\n"
                 "swi " swiToConst(OS_ReadDynamicArea) "\n"
                 "mov %0, r1\n"
                : "=r"(alloc_size)          // outputs
                :                           // inputs 
                : "r0", "r1", "cc");        // clobbers

    return alloc_size;
}
