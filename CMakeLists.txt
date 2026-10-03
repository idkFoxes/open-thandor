cmake_minimum_required(VERSION 3.20)
project(thandor_curated_reference C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# The recovered code stores pointers in 32-bit fields (pointer32, (int)ptr casts).
# Decompiled code reads Ghidra's placeholders for unknown register halves (extraout_var, ...)
# before assigning them; the debug runtime checks would stop on each one.
string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

# Developer tools (src/platform/debug, src/platform/selftest): self-tests, input scripts, automatic screenshots,
# the determinism state hash, campaign starts and AUTOWIN, level-script log, movie player/dump/compare, windowed
# mode, second instance, UDP port and datagram log, watchdog - all switched on at run time by OPEN_THANDOR_*
# environment variables (docs/BUILDING.md). The game reaches them only through
# include/thandor/platform/debug/hooks.h. OFF (default): none of it is compiled, the hooks expand to nothing and
# the game runs the original code paths. ON: the test build (preset "test", build-test) that tools/test drives.
option(THANDOR_DEV_TOOLS "Build the developer tools (test aids, debug switches, self-tests)" OFF)
# THANDOR_TEST_AIDS was the old name of this option (ON switches the developer tools on).
if(DEFINED THANDOR_TEST_AIDS)
    if(THANDOR_TEST_AIDS)
        set(THANDOR_DEV_TOOLS ON CACHE BOOL "Build the developer tools (test aids, debug switches, self-tests)" FORCE)
    endif()
    message(STATUS "THANDOR_TEST_AIDS is obsolete, use THANDOR_DEV_TOOLS (now ${THANDOR_DEV_TOOLS})")
    unset(THANDOR_TEST_AIDS CACHE)
endif()

# The code needs MSVC (inline __asm, __stdcall/__fastcall, MSVC pragmas) targeting 32-bit x86.
# Use a preset (CMakePresets.json: debug, release, test), which finds MSVC x86 by itself, or
# configure from a vcvars32 prompt. In CLion: Settings > Build > CMake, enable the presets and
# disable the default MinGW profile.
if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "open-thandor must be built with MSVC for 32-bit x86: use a CMake preset "
                        "(cmake --preset release) or a vcvars32 prompt. Found: ${CMAKE_C_COMPILER_ID}, "
                        "${CMAKE_SIZEOF_VOID_P}-byte pointers.")
endif()

add_library(thandor_curated STATIC
    src/assets/army/catalog.c
    src/assets/effect/catalog.c
    src/assets/model/definitions.c
    src/assets/package/codec.c
    src/assets/package/runtime.c
    src/assets/resource/runtime.c
    src/assets/rom/runtime.c
    src/assets/scenario/catalog.c
    src/assets/shot/catalog.c
    src/assets/sprite/catalog.c
    src/assets/text/resources.c
    src/assets/text/richtext.c
    src/audio/backend/runtime.c
    src/audio/codec/sam.c
    src/audio/spatial/runtime.c
    src/core/error/runtime.c
    src/core/math/fixed.c
    src/core/math/geometry.c
    src/core/math/interpolation.c
    src/core/math/random.c
    src/core/memory/allocator.c
    src/core/memory/synchronization.c
    src/core/settings/persistent.c
    src/core/text/path.c
    src/core/text/string.c
    src/gameplay/ai/combat.c
    src/gameplay/ai/placement.c
    src/gameplay/ai/planning.c
    src/gameplay/ai/technology.c
    src/gameplay/ai/units.c
    src/gameplay/ai/workspaces.c
    src/gameplay/army/audio.c
    src/gameplay/army/combat.c
    src/gameplay/army/movement.c
    src/gameplay/army/placement.c
    src/gameplay/army/runtime.c
    src/gameplay/faction/relations.c
    src/gameplay/faction/runtime.c
    src/gameplay/input/world.c
    src/gameplay/selection/overlay.c
    src/gameplay/selection/runtime.c
    src/gameplay/session/level.c
    src/gameplay/session/runtime.c
    src/gameplay/session/savegame.c
    src/gameplay/technology/runtime.c
    src/graphics/backend/directdraw.c
    src/graphics/backend/software.c
    src/graphics/core/runtime.c
    src/graphics/render/model.c
    src/graphics/render/primitives.c
    src/graphics/render/projection.c
    src/graphics/render/shading.c
    src/graphics/resources/framebuffer.c
    src/graphics/resources/palette.c
    src/graphics/resources/pcx_read.c
    src/graphics/resources/pcx_write.c
    src/graphics/resources/texture.c
    src/movie/runtime/playback.c
    src/network/backend/fallback_udp.c
    src/network/backend/runtime.c
    src/network/protocol/commands.c
    src/network/protocol/transfer.c
    src/platform/bootstrap/runtime.c
    src/platform/filesystem/win32.c
    src/platform/input/devices.c
    src/platform/system/time_locale.c
    src/platform/system/win32.c
    src/ui/controls/buttons.c
    src/ui/controls/input.c
    src/ui/controls/layout.c
    src/ui/controls/lists.c
    src/ui/controls/misc.c
    src/ui/controls/text.c
    src/ui/core/runtime.c
    src/ui/frontend/network.c
    src/ui/frontend/player.c
    src/ui/frontend/results.c
    src/ui/frontend/runtime.c
    src/ui/frontend/scenario.c
    src/ui/frontend/session.c
    src/ui/frontend/settings.c
    src/ui/ingame/commands.c
    src/ui/ingame/runtime.c
    src/ui/ingame/settings.c
    src/ui/ingame/technology.c
    src/ui/support/runtime.c
    src/world/effects/maintenance.c
    src/world/effects/runtime.c
    src/world/model/hierarchy.c
    src/world/model/runtime.c
    src/world/model/slots.c
    src/world/motion/runtime.c
    src/world/pathing/grid.c
    src/world/pathing/influence.c
    src/world/runtime/core.c
    src/world/shots/maintenance.c
    src/world/shots/runtime.c
    src/world/terrain/editing.c
    src/world/terrain/grid.c
    src/world/terrain/height.c
    src/world/terrain/occupancy.c
    src/world/terrain/projection.c
    src/world/terrain/visuals.c
)
target_include_directories(thandor_curated PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Fixed base (REBUILT_IMAGE_BASE in platform/bootstrap/image.h): crash logs and the self-tests that run
# copies of original code rely on stable addresses.
add_executable(thandor WIN32
    src/platform/bootstrap/main.c
    src/platform/bootstrap/image.c
)
target_link_libraries(thandor PRIVATE thandor_curated kernel32 user32 dbghelp)
target_link_options(thandor PRIVATE /BASE:0x10000000 /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO)

if(THANDOR_DEV_TOOLS)
    target_sources(thandor_curated PRIVATE
        src/platform/debug/autoshot.c
        src/platform/debug/campaign.c
        src/platform/debug/font.c
        src/platform/debug/hooks.c
        src/platform/debug/level_script.c
        src/platform/debug/movie_decoder.c
        src/platform/debug/movie_player.c
        src/platform/debug/original_code.c
        src/platform/debug/script.c
        src/platform/debug/statehash.c
        src/platform/debug/test_aids.c
        src/platform/debug/windowed.c
        src/platform/selftest/relax.c
        src/platform/selftest/selftests.c
    )
    target_compile_definitions(thandor_curated PUBLIC THANDOR_DEV_TOOLS=1)
endif()
