// ============================================================================
// Sequence.
// ============================================================================

#ifndef __SEQUENCE_H__
#define __SEQUENCE_H__

extern int seq_part_no;

void sequence_init();
void sequence_tick();
void sequence_draw();
void sequence_kill();

void sequence_set_part(int new_part_no);

#endif // __SEQUENCE_H__
