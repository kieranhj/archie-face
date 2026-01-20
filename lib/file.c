// ============================================================================
// File handling.
// ============================================================================

#include "file.h"
#include "archie/swi.h"
#include <stddef.h>
#include <stdlib.h>

int file_get_size(const char *filename)
{
    int size;
    asm volatile("mov r0, #5\n"                 // get file size
                 "mov r1, %1\n"                 // filename
                 "swi "swiToConst(OS_File) "\n"
                 "cmp r0, #1\n"                 // object type
                 "movne r4, #0\n"               // not a file?
                 "mov %0, r4\n"                 // file size
                : "=r"(size)                    // outputs
                : "r"(filename)                 // inputs
                : "r0", "r1", "r4", "cc");      // clobbers
    return size;
}

void *file_load_with_alloc(const char *filename)
{
    int size = file_get_size(filename);
    void *data = malloc(size);
    int objtype = file_load_at_address(filename, malloc(size));
    if (objtype != 1)   // failed to load.
    {
        free(data);
        return NULL;
    }
    return data;
}

int file_load_at_address(const char *filename, void *load_to)
{
    if (!file_get_size(filename))
        return 0;

    int objtype;
    asm volatile("mov r0, #0xff\n"                  // load file
                 "mov r1, %1\n"                     // filename
                 "mov r2, %2\n"                     // address
                 "mov r3, #0\n"                     // flags
                 "swi "swiToConst(OS_File) "\n"
                 "mov %0, r0\n"                     // object type
                : "=r"(objtype)                     // outputs
                : "r"(filename), "r"(load_to)       // inputs
                : "r0", "r1", "r2", "r3", "cc");    // clobbers
    return objtype;
}

void file_save(const char *filename, void *address, int size)
{
    void *endaddr = address+size;
    asm volatile("mov r0, #0\n"                     // save file
                 "mov r1, %0\n"                     // filename
                 "mov r2, #0\n"                     // load address
                 "mov r3, #0\n"                     // exec address
                 "mov r4, %1\n"                     // data start
                 "mov r5, %2\n"                     // data end
                 "swi "swiToConst(OS_File) "\n"
                :                                   // outputs
                : "r"(filename), "r"(address), "r"(endaddr)       // inputs
                : "r0", "r1", "r2", "r3", "r4", "r5", "cc");      // clobbers
}
