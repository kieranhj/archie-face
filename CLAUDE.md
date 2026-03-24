# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Archie-Face ("Fast Archimedes C Environment") is an Acorn Archimedes demo/trackmo framework written in C, built on top of ArchieSDK. It targets RISC OS and runs in Screen Mode 13 (320x256, 8bpp) with triple-buffered rendering at 50Hz.

No Makefile was found in this repo — the project is built via the ArchieSDK toolchain (cross-compiled or run in an Archimedes emulator). Check ArchieSDK documentation for build commands.

## Architecture

### Main Loop (`main.c`)
Triple-buffered rendering loop with VSync interrupt handling. The pattern is:
1. `sequence_tick()` — update state
2. Wait for free write bank
3. `mem_set_fast()` — clear framebuffer
4. `sequence_draw()` — render to `g_framebuffer`
5. Flip display bank at next VSync

Global state exposed from `main.c`: `g_framebuffer`, `g_frame_count`, `g_vsync_count`, `g_debug_rasters`.

Screen constants live in `src/globals.h` (`Screen_Width`, `Screen_Height`, `Screen_Stride`, `Screen_Banks`, etc.).

### Sequence System (`src/sequence.{h,c}`, `src/seq-parts.{h,c}`)
Demo "parts" are managed via a dispatch table. Each part implements four functions:
- `seq_partN_init()` — allocate and set up resources
- `seq_partN_tick()` — update state each frame
- `seq_partN_draw()` — render to framebuffer
- `seq_partN_kill()` — free resources

To add a new part: declare functions in `seq-parts.h`, implement in `seq-parts.c`, and add a row to `seq_part_table[]` in `sequence.c`. Update `SEQ_MAX_PARTS` accordingly.

### Flow Fields (`src/flow-field.{h,c}`)
A 2D grid of angles (fix16_t "brads") that guide particle movement. Initialized either from 2D Perlin noise (`flow_field_init_with_noise()`) or a fixed angle. Fields can be blended, have attractors/vortices inserted, or be queried for the nearest angle at a given position. The struct is opaque — use accessor functions (`flow_field_get_angle`, `flow_field_set_angle`, `flow_field_get_rows`, `flow_field_get_cols`).

### Particle Emitter (`src/emitter.{h,c}`)
Emitters spawn particles and move them each tick guided by a flow field. Per-emitter properties: origin, speed, colour, radius, max age, delta vector, optional rotation offset. Draw modes: flat colour (`emitter_draw`), colour ramp by age (`emitter_draw_ramp_with_age`), or colour ramp by x-position (`emitter_draw_ramp_with_x`). The struct is opaque.

### Supporting Systems
- `src/colour.{h,c}` — 256-colour palette initialisation; `colour_make_ramp()` fills a `u8[]` with palette indices between two RGB4 colours. `COLOUR_MAKE_RGB4(r,g,b)` packs 4-bit channels.
- `src/noise.{h,c}` — 2D Perlin noise used by flow field initialisation.
- `lib/maths.h` — Fixed-point type `fix16_t` ([s15.16]). Key macros: `INT_TO_FIX16`, `FLOAT_TO_FIX16`, `FIX16_TO_INT`, `FIX16_MUL`, `FIX16_FRACTION`.
- `lib/trig.{h,c}` — LUT-based `sin_fix16(brad)` / `cos_fix16(brad)`. Angles are in "brads" (full circle = 65536 = FIX16_ONE).
- `lib/vector.h` — `vec2fix16_t` struct and `vec2f` float struct. Also `rand_between(min, max)`.
- `lib/debug.{h,c}` — `debug_register_key()` for keyboard callbacks, `SET_BORDER(bgr)` macro for raster-bar timing, `debug_plot_string_mode13()` for on-screen text.
- `lib/mem.{h,c}` — Fast `mem_set_fast()` for clearing the framebuffer.
- `lib/video.{h,c}` — Screen bank switching (`v_setDisplayBank`, `v_setWriteBank`), event claiming.
- `lib/archie.h` — RISC OS constants: `VIDC_*` registers, `RMKey_*` internal key codes, `Event_VSync` / `Event_KeyPressed`.
- `ext/CVector.h` — External CVector library.
- `data/lib/Spectrum.bin` — Spectrum font binary, embedded via `INCBIN` macro into `main.c`.

## Coding Conventions (from `ref/archie-face.txt`)

- **Include order**: module header → local headers → SDK headers → CLib headers
- **Naming**: all lowercase with `_`, prefixed by module name (e.g. `flow_field_make`, `emitter_tick`)
- **Types**: use `_t` suffix for typedefs, `_s` suffix for struct tags; opaque structs use `typedef struct foo_s foo_t`
- **Fixed-point**: use `fix16_t` ([s15.16]) for position/angle arithmetic, `vec2fix16_t` for 2D
- **Lifecycle pattern**: `_init`, `_tick`, `_draw`, `_kill` — `_make` / `_kill` for heap-allocated objects (kill returns `NULL` for easy nulling: `field1 = flow_field_kill(field1)`)

## PC Build (`platform/pc/`)

A standalone SDL2 executable that runs the same demo logic on a development PC without needing the Arculator emulator. Used for rapid iteration on effects and parameters.

### How it works

- **Shadow headers** — `platform/pc/archie/` is listed first on the include path, so every `#include "archie/SDKTypes.h"` etc. picks up the PC stub instead of the ArchieSDK version. No `#ifdef` pollution in shared headers.
- **Platform source files** — `video_pc.c`, `mem_pc.c`, `mouse_pc.c`, `file_pc.c` provide PC implementations of the four RISC OS-specific libraries. The original `.c` files are not compiled.
- **Font data** — `data/lib/Spectrum.bin` is converted to a C array at CMake configure time by `platform/pc/gen_font.cmake`, replacing the `INCBIN` assembler macro used in `main.c`.
- **Framebuffer** — an `SDL_Surface` with `SDL_PIXELFORMAT_INDEX8` at 320×256 is displayed scaled 3× in the window. The shared rendering code writes bytes to `g_framebuffer` identically to on-hardware.
- **Palette** — after `colour_init_palette()`, `colour_get_archie256()` is walked to populate the SDL palette so colour indices match the Archimedes output exactly.
- **Key dispatch** — SDL key events are translated to `RMKey_*` values and fed to the existing `debug_handle_keypress()` / `debug_do_keypress_callbacks()` system, so all `debug_register_key()` calls in `sequence.c` and `seq-parts.c` work identically on both targets.

### Building (Windows, MinGW + vcpkg)

```bash
vcpkg install sdl2
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
cmake --build build
./build/archie_face_pc
```

### Files changed vs. Archimedes build

| File | Change |
|------|--------|
| `lib/debug.c` | `#ifndef PLATFORM_PC` guard around `debug_write_vidc()` ARM asm; new `debug_plot_string_at()` / `debug_plot_string_at_inv()` functions under `#ifdef PLATFORM_PC` |
| `lib/debug.h` | Declare the two new PC-only string functions |
| `src/colour.c` | Add `colour_get_archie256()` getter under `#ifdef PLATFORM_PC` |
| `src/colour.h` | Declare `colour_get_archie256()` |

All other shared source files are compiled **unchanged** for both targets.

### Live parameter tweaking

`platform/pc/params.h` provides a parameter registry. Call from any `seq_partN_init()`:

```c
#include "platform/pc/params.h"
// ...
params_clear();
param_float("noise_scale", &g_noise_scale, 0.001f, 0.1f, 0.001f);
param_int("emitter_count", &g_count, 1, 16, 1);
```

On Archimedes these calls expand to `((void)0)`. On PC a panel appears in the top-right corner of the window.

| Key | Action |
|-----|--------|
| Tab / Shift+Tab | Select next / previous parameter |
| ] | Increase selected parameter by one step |
| [ | Decrease selected parameter by one step |

## Runtime Debug Keys

| Key | Archimedes | PC |
|-----|------------|----|
| Escape | Quit | Quit |
| Space | Pause/resume tick | Pause/resume tick |
| S | Step one frame | Step one frame |
| D | Toggle debug text overlay | Toggle debug text overlay |
| R | Toggle raster timing (border colour) | no-op (SET_BORDER does nothing) |
| Arrow Right | Next sequence part | Next sequence part |
| Arrow Left | Restart current part | Restart current part |
| A | Reboot (OS_CLI "grid") | — (not mapped) |
