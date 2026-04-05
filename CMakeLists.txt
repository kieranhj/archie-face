cmake_minimum_required(VERSION 3.16)
project(archie_face_pc C)

set(CMAKE_C_STANDARD 11)

# ============================================================================
# Font binary → C source (avoids .incbin assembler directive on PC)
# ============================================================================

set(FONT_BIN "${CMAKE_SOURCE_DIR}/data/lib/Spectrum.bin")
set(FONT_GEN "${CMAKE_BINARY_DIR}/spectrum_font_gen.c")

add_custom_command(
    OUTPUT  "${FONT_GEN}"
    COMMAND ${CMAKE_COMMAND}
            -DFONT_BIN=${FONT_BIN}
            -DOUT_C=${FONT_GEN}
            -P "${CMAKE_SOURCE_DIR}/platform/pc/gen_font.cmake"
    DEPENDS "${FONT_BIN}"
    COMMENT "Generating spectrum_font_gen.c from Spectrum.bin"
)

# ============================================================================
# Executable
# ============================================================================

add_executable(archie_face_pc
    # PC platform layer
    platform/pc/main_pc.c
    platform/pc/video_pc.c
    platform/pc/mem_pc.c
    platform/pc/mouse_pc.c
    platform/pc/file_pc.c
    platform/pc/params.c
    platform/pc/nuklear_impl.c

    # Generated font data (satisfies INCBIN_EXTERN(debug_font) in debug.c)
    ${FONT_GEN}

    # Shared demo logic — compiled unchanged
    src/colour.c
    src/sequence.c
    src/seq-parts.c
    src/flow-field.c
    src/emitter.c
    src/noise.c

    # Shared lib — portable subset
    lib/debug.c
    lib/plot.c
    lib/trig.c
)

# ============================================================================
# Include paths
# ============================================================================

target_include_directories(archie_face_pc PRIVATE
    # Shadow ArchieSDK headers — must be listed before any real SDK directory.
    # CMake will find platform/pc/archie/SDKTypes.h when code does
    # #include "archie/SDKTypes.h".
    platform/pc
    # Project root so that #include "src/foo.h", "lib/bar.h", "ext/..." all resolve.
    .
)

# ============================================================================
# Compile definitions
# ============================================================================

target_compile_definitions(archie_face_pc PRIVATE
    PLATFORM_PC
    # M_PI_F / M_PI_2_F are ARM GCC extensions, not available on UCRT.
    "M_PI_F=3.14159265358979323846f"
    "M_PI_2_F=1.57079632679489661923f"
)

# ============================================================================
# SDL2
# ============================================================================

find_package(SDL2 REQUIRED)
target_link_libraries(archie_face_pc PRIVATE SDL2::SDL2 m)

# SDL2main provides WinMain on Windows; SDL_MAIN_HANDLED tells SDL2 not to
# redefine main so our own main() is the entry point.
if(WIN32)
    # SDL_MAIN_HANDLED tells SDL not to redefine main() — our main() is the
    # entry point directly.  SDL2main is not needed and causes link order
    # issues so we don't link it.
    target_compile_definitions(archie_face_pc PRIVATE SDL_MAIN_HANDLED)
endif()

# ============================================================================
# Warnings
# ============================================================================

if(MSVC)
    target_compile_options(archie_face_pc PRIVATE /W3)
else()
    target_compile_options(archie_face_pc PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()
