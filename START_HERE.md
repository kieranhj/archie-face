# archie-face: start here

Archie-Face ("Fast Archimedes C Environment"): an Acorn Archimedes demo framework in C on ArchieSDK (GCC 8.5 for ARM2), MODE 13, triple-buffered at 50 Hz, currently doing flow-field particle experiments. Branch wip/pc-build adds an SDL2 PC build with a Nuklear parameter panel for fast iteration.

| | |
|---|---|
| GitHub | https://github.com/kieranhj/archie-face (public) |
| Local path | D:\archie\mine\archie-face (moved from OneDrive\Archie\Repos\archie-face on 2026-11-22) |
| Branch | wip/pc-build (current); other branches: main, wip/flow-fields (all on origin) |
| Last commit | 2026-04-06: Make flow field grids larger than screen dimensions. |

## What's in here

- `main.c` (main loop), `src/` (flow-field, emitter, noise, colour, sequence / seq-parts), `lib/` (debug, mem, plot, trig, video, ...), `ext/CVector.h`.
- `platform/pc/`: PC shadow headers and SDL2 implementations, `params.h` (live tweak panel).
- `Makefile` (Archie build via ArchieSDK), `CMakeLists.txt` (PC build), `CLAUDE.md` (architecture and conventions, detailed), `ref/`.

## Build and run

- `make` (Archie): blocked on 2026-11-22. It needs the `ARCHIESDK` environment variable (SDK is at `D:\tools\archie\ArchieSDK`, moved from `C:\Dev\ArchieSDK`) and the SDK's gcc needs MSYS2 UCRT DLLs (`C:\msys64\ucrt64\bin` on PATH). With both set, the compile on this branch fails: `src/flow-field.c` and `src/seq-parts.c` include `platform/pc/params.h`, which needs `-I.` in CFLAGS. The last recipe line (`cp -f ... | true`) is sh-only, and the SDK's `config.mk` still has `ARCHIECOPYPATH=/c/Users/khcon/OneDrive/Archie/Repos/arculator/hostfs`.
- To try: `$env:ARCHIESDK='D:/tools/archie/ArchieSDK'; $env:PATH="C:\msys64\ucrt64\bin;$env:PATH"; make` -> `build/grid,ff8`.
- PC build (from `CLAUDE.md`): `cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake` then `cmake --build build` -> `build/archie_face_pc.exe` (MSYS2 UCRT gcc, SDL2). The existing `build/CMakeCache.txt` points at the old OneDrive source dir, so configure into a fresh build dir.

## Local changes

- No uncommitted changes, no stashes.
- wip/pc-build is 28 commits ahead of main; wip/flow-fields 21 ahead (both pushed). main's last commit: 2025-12-06 "Stash fluid prototype."

## Status and next steps

- Active spring 2026: PC build working with live parameters; flow-field particles projected onto a plane. No PLAN/TODO file; `CLAUDE.md` is the reference. Next likely: fix the Archie build on wip/pc-build, then merge it into main.

## See also

- `CLAUDE.md`, `README.md`; ArchieSDK `README` in `D:\tools\archie\ArchieSDK`.
