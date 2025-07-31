// ============================================================================
// Archie defs (missing from ArchieSDK) - SWIs, RISCOS constants, hw registers.
// ============================================================================

#ifndef __ARCHIE_H__
#define __ARCHIE_H__

// ============================================================================
// Archimedes low-level internal key numbers transmitted by IOC.
// Found in RISCOS PRMs pp 1-156.
// Used by RasterMan and OS_Event Event_KeyPressed (11)

#define RMKey_ArrowUp 0x59
#define RMKey_ArrowLeft 0x62
#define RMKey_ArrowDown 0x63
#define RMKey_ArrowRight 0x64
#define RMKey_Return 0x47
#define RMKey_Space 0x5f
#define RMKey_LeftClick 0x70
#define RMKey_RightClick 0x72
#define RMKey_PageUp 0x21
#define RMKey_PageDown 0x36
#define RMKey_A 0x3c
#define RMKey_B 0x52
#define RMKey_C 0x50
#define RMKey_D 0x3e
#define RMKey_E 0x29
#define RMKey_F 0x3f
#define RMKey_G 0x40
#define RMKey_H 0x41
#define RMKey_I 0x2e
#define RMKey_J 0x42
#define RMKey_K 0x43
#define RMKey_L 0x44
#define RMKey_M 0x54
#define RMKey_N 0x53
#define RMKey_O 0x2f
#define RMKey_P 0x30
#define RMKey_Q 0x27
#define RMKey_R 0x2a
#define RMKey_S 0x3d
#define RMKey_T 0x2b
#define RMKey_U 0x2d
#define RMKey_V 0x51
#define RMKey_W 0x28
#define RMKey_X 0x4f
#define RMKey_Y 0x2c
#define RMKey_Z 0x4e
#define RMKey_1 0x11
#define RMKey_2 0x12
#define RMKey_3 0x13
#define RMKey_4 0x14
#define RMKey_5 0x15
#define RMKey_6 0x16
#define RMKey_7 0x17
#define RMKey_8 0x18
#define RMKey_9 0x19
#define RMKey_0 0x1a

// ============================================================================

#define Event_VSync         4
#define Event_KeyPressed    11

// ============================================================================

#define VIDC_Col0   0x00000000
#define VIDC_Col1   0x04000000              // index << 26
#define VIDC_Col2   0x08000000
#define VIDC_Col3   0x0c000000
#define VIDC_Col4   0x10000000
#define VIDC_Col5   0x14000000
#define VIDC_Col6   0x18000000
#define VIDC_Col7   0x1c000000
#define VIDC_Col8   0x20000000
#define VIDC_Col9   0x24000000
#define VIDC_Col10  0x28000000
#define VIDC_Col11  0x2c000000
#define VIDC_Col12  0x30000000
#define VIDC_Col13  0x34000000
#define VIDC_Col14  0x38000000
#define VIDC_Col15  0x3c000000
#define VIDC_Border 0x40000000
#define VIDC_Write  0x03400000

// ============================================================================

#endif // __ARCHIE_H__
