// ============================================================================
// PC stub: RISC OS SWI system call definitions.
// swiToConst is only referenced inside #ifndef PLATFORM_PC guards, so this
// stub just needs to exist and be syntactically valid.
// ============================================================================

#ifndef __SWI_H__
#define __SWI_H__

// Provide a harmless definition so any accidental reference outside a guard
// produces an integer rather than a compile error.
#define swiToConst(x) 0

#endif // __SWI_H__
