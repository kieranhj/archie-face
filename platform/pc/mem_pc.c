// ============================================================================
// PC implementation of lib/mem.h.
// mem_set_fast() on Archimedes is a hand-rolled ARM STM loop. On PC we
// delegate straight to memset.
// ============================================================================

#include "lib/mem.h"
#include <string.h>

void mem_set_fast(u32 *dest, u32 c, u32 size)
{
    memset(dest, (int)c, (size_t)size);
}
