// ============================================================================
// Mem lib (probably belongs in ArchieSDK.)
// ============================================================================

#include "mem.h"

//make sure that dest is aligned and size (in bytes) can be divided by 40; feel free to re-use this in other projects :)
void __attribute__((noinline)) memsetFast(u32* dest, u32 c, u32 size) {
    asm volatile("push {%0, %1}\n"
        "mov r0, %1\n"
        "mov r1, %1\n"
        "mov r2, %1\n"
        "mov r3, %1\n"
        "mov r4, %1\n"
        "mov r5, %1\n"
        "mov r6, %1\n"
        "mov r7, %1\n"
        "mov r8, %1\n"
    "%=:\n" //auto generated label
        "stm %0!, {r0-r8,%1}\n"
        "cmp %0, %2\n"
        "bne %=b\n" //jump *b*ackwards to auto generated label
        "pop {%0, %1}\n"
    :                                           // outputs
    : "r"(dest), "r"(c), "r"(dest + (size/4))   // inputs
    : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "cc", "memory"); // clobbers
}
