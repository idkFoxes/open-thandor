cmake_minimum_required(VERSION 3.20)
project(thandor_curated_reference C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# The recovered code stores pointers in 32-bit fields (pointer32, (int)ptr casts).
# Decompiled code reads Ghidra's placeholders for unknown register halves (extraout_var, ...)
# before assigning them; the debug runtime checks would stop on each one.
string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

# OFF (default): the build matches the original's code. ON: compile in the runtime test aids for a
# local two-instance multiplayer test and windowed mode (OPEN_THANDOR_WINDOWED, OPEN_THANDOR_MULTI_INSTANCE,
# OPEN_THANDOR_NET_PORT, OPEN_THANDOR_NETLOG; see docs/BUILDING.md and tools/test/run_multiplayer.py).
option(THANDOR_TEST_AIDS "Build the local multiplayer/windowed test aids" OFF)

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
    src/assets/army/data.c
    src/assets/effect/catalog.c
    src/assets/effect/data.c
    src/assets/fnc/runtime.c
    src/assets/model/data.c
    src/assets/model/definitions.c
    src/assets/package/codec.c
    src/assets/package/data.c
    src/assets/package/runtime.c
    src/assets/resource/data.c
    src/assets/resource/runtime.c
    src/assets/rom/data.c
    src/assets/rom/runtime.c
    src/assets/scenario/catalog.c
    src/assets/scenario/data.c
    src/assets/shot/catalog.c
    src/assets/shot/data.c
    src/assets/sprite/catalog.c
    src/assets/sprite/data.c
    src/assets/text/data.c
    src/assets/text/resources.c
    src/assets/text/richtext.c
    src/audio/backend/data.c
    src/audio/backend/runtime.c
    src/audio/codec/data.c
    src/audio/codec/sam.c
    src/audio/spatial/data.c
    src/audio/spatial/runtime.c
    src/core/error/data.c
    src/core/error/runtime.c
    src/core/math/data.c
    src/core/math/fixed.c
    src/core/math/geometry.c
    src/core/math/interpolation.c
    src/core/math/random.c
    src/core/memory/allocator.c
    src/core/memory/data.c
    src/core/memory/synchronization.c
    src/core/settings/data.c
    src/core/settings/persistent.c
    src/core/text/data.c
    src/core/text/path.c
    src/core/text/string.c
    src/gameplay/ai/combat.c
    src/gameplay/ai/data.c
    src/gameplay/ai/placement.c
    src/gameplay/ai/planning.c
    src/gameplay/ai/technology.c
    src/gameplay/ai/units.c
    src/gameplay/ai/workspaces.c
    src/gameplay/army/audio.c
    src/gameplay/army/combat.c
    src/gameplay/army/data.c
    src/gameplay/army/movement.c
    src/gameplay/army/placement.c
    src/gameplay/army/runtime.c
    src/gameplay/faction/data.c
    src/gameplay/faction/relations.c
    src/gameplay/faction/runtime.c
    src/gameplay/input/data.c
    src/gameplay/input/world.c
    src/gameplay/selection/data.c
    src/gameplay/selection/overlay.c
    src/gameplay/selection/runtime.c
    src/gameplay/session/data.c
    src/gameplay/session/level.c
    src/gameplay/session/runtime.c
    src/gameplay/session/savegame.c
    src/gameplay/technology/data.c
    src/gameplay/technology/runtime.c
    src/graphics/backend/data.c
    src/graphics/backend/direct3d.c
    src/graphics/backend/directdraw.c
    src/graphics/backend/glide.c
    src/graphics/backend/software.c
    src/graphics/core/data.c
    src/graphics/core/runtime.c
    src/graphics/render/data.c
    src/graphics/render/model.c
    src/graphics/render/primitives.c
    src/graphics/render/projection.c
    src/graphics/render/shading.c
    src/graphics/resources/data.c
    src/graphics/resources/framebuffer.c
    src/graphics/resources/palette.c
    src/graphics/resources/pcx_read.c
    src/graphics/resources/pcx_write.c
    src/graphics/resources/texture.c
    src/movie/runtime/data.c
    src/movie/runtime/playback.c
    src/network/backend/data.c
    src/network/backend/fallback_udp.c
    src/network/backend/runtime.c
    src/network/protocol/commands.c
    src/network/protocol/data.c
    src/network/protocol/transfer.c
    src/platform/bootstrap/data.c
    src/platform/bootstrap/runtime.c
    src/platform/debug/autoshot.c
    src/platform/debug/campaign.c
    src/platform/debug/data.c
    src/platform/debug/font.c
    src/platform/debug/movie_decoder.c
    src/platform/debug/movie_player.c
    src/platform/debug/script.c
    src/platform/debug/statehash.c
    src/platform/debug/test_aids.c
    src/platform/filesystem/data.c
    src/platform/filesystem/win32.c
    src/platform/input/data.c
    src/platform/input/devices.c
    src/platform/system/data.c
    src/platform/system/time_locale.c
    src/platform/system/win32.c
    src/ui/controls/buttons.c
    src/ui/controls/data.c
    src/ui/controls/input.c
    src/ui/controls/layout.c
    src/ui/controls/lists.c
    src/ui/controls/misc.c
    src/ui/controls/text.c
    src/ui/core/data.c
    src/ui/core/runtime.c
    src/ui/frontend/data.c
    src/ui/frontend/network.c
    src/ui/frontend/player.c
    src/ui/frontend/results.c
    src/ui/frontend/runtime.c
    src/ui/frontend/scenario.c
    src/ui/frontend/session.c
    src/ui/frontend/settings.c
    src/ui/ingame/commands.c
    src/ui/ingame/data.c
    src/ui/ingame/runtime.c
    src/ui/ingame/settings.c
    src/ui/ingame/technology.c
    src/ui/support/data.c
    src/ui/support/runtime.c
    src/world/effects/data.c
    src/world/effects/maintenance.c
    src/world/effects/runtime.c
    src/world/model/data.c
    src/world/model/hierarchy.c
    src/world/model/runtime.c
    src/world/model/slots.c
    src/world/motion/data.c
    src/world/motion/runtime.c
    src/world/pathing/data.c
    src/world/pathing/grid.c
    src/world/pathing/influence.c
    src/world/runtime/core.c
    src/world/runtime/data.c
    src/world/shots/data.c
    src/world/shots/maintenance.c
    src/world/shots/runtime.c
    src/world/terrain/data.c
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
    src/platform/selftest/selftests.c
    src/platform/selftest/relax.c
    src/generated/function_map.c
    src/generated/image_data.c
)
target_link_libraries(thandor PRIVATE thandor_curated kernel32 user32 dbghelp)
target_link_options(thandor PRIVATE /BASE:0x10000000 /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO)

if(THANDOR_TEST_AIDS)
    target_compile_definitions(thandor_curated PRIVATE THANDOR_TEST_AIDS=1)
    target_compile_definitions(thandor PRIVATE THANDOR_TEST_AIDS=1)
endif()
