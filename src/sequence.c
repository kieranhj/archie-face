// ============================================================================
// Sequence.
// TODO: Something more sophisticated when needed.
// ============================================================================

#include "sequence.h"
#include "globals.h"
#include "seq-parts.h"
#include "../lib/debug.h"

// ============================================================================

#define SEQ_DEFAULT_PART    2
#define SEQ_MAX_PARTS       3

typedef void (*seq_func)();

static seq_func seq_part_table[SEQ_MAX_PARTS][4] =
{
    // TODO: Macro magic to fill the table using std naming?
    { seq_part1_init, seq_part1_tick, seq_part1_draw, seq_part1_kill },
    { seq_part2_init, seq_part2_tick, seq_part2_draw, seq_part2_kill },
    { seq_part3_init, seq_part3_tick, seq_part3_draw, seq_part3_kill },
};

// ============================================================================

int seq_part_no = -1;
static int seq_new_part_no = -1;

// ============================================================================

static void seq_debug_next_part(u32 param1, u32 param2);
static void seq_debug_restart_part(u32 param1, u32 param2);

// ============================================================================

void sequence_init()
{
    sequence_set_part(SEQ_DEFAULT_PART);   // First part.
    debug_register_key(RMKey_ArrowRight, seq_debug_next_part, 0, 0);
    debug_register_key(RMKey_ArrowLeft, seq_debug_restart_part, 0, 0);
}

void sequence_tick()
{
    // Change part if triggered.

    if (seq_new_part_no != -1)
    {
        if (seq_part_no != -1) (seq_part_table)[seq_part_no][3]();      // Kill previous part.
        seq_part_no = seq_new_part_no;
        seq_new_part_no = -1;
        if (seq_part_no != -1) (seq_part_table)[seq_part_no][0]();      // Init new part.
    }

    if (seq_part_no != -1) (seq_part_table)[seq_part_no][1]();          // Tick part.
}

void sequence_draw()
{
    if (seq_part_no != -1) (seq_part_table)[seq_part_no][2]();          // Draw part.
}

void sequence_set_part(int new_part_no)
{
    // Loop parts for now.
    if (new_part_no >= SEQ_MAX_PARTS) new_part_no = 0;
    seq_new_part_no = new_part_no;
}

void sequence_kill()
{
    sequence_set_part(-1);
}

// ============================================================================
// Debug code.
// ============================================================================

static void seq_debug_next_part(u32 param1, u32 param2)
{
    (void)param1;
    (void)param2;
    sequence_set_part(seq_part_no+1);
}

static void seq_debug_restart_part(u32 param1, u32 param2)
{
    (void)param1;
    (void)param2;
    sequence_set_part(seq_part_no);
}
