cmake_minimum_required(VERSION 3.20)
project(thandor_curated_reference C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# The recovered code stores pointers in 32-bit fields (pointer32, (int)ptr casts).
# Decompiled code reads Ghidra's placeholders for unknown register halves (extraout_var, ...)
# before assigning them; the debug runtime checks would stop on each one.
string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

# OFF (default): the original data is compiled in from src/generated/image_data.c.
# ON: map thandor_original.exe at 0x400000 and use its data there (the former runtime model).
option(THANDOR_MAPPED_IMAGE "Use the data of a mapped thandor_original.exe" OFF)
if(THANDOR_MAPPED_IMAGE)
    add_compile_definitions(THANDOR_MAPPED_IMAGE)
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "open-thandor must be built for 32-bit x86 (e.g. MSVC vcvars32 / -A Win32).")
endif()

add_library(thandor_curated STATIC
    src/assets/army/catalog.c
    src/assets/effect/catalog.c
    src/assets/fnc/runtime.c
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
    src/graphics/backend/direct3d.c
    src/graphics/backend/directdraw.c
    src/graphics/backend/glide.c
    src/graphics/backend/software.c
    src/graphics/core/runtime.c
    src/graphics/render/model.c
    src/graphics/render/primitives.c
    src/graphics/render/projection.c
    src/graphics/render/shading.c
    src/graphics/resources/framebuffer.c
    src/graphics/resources/palette.c
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

# The executable maps the original image at 0x400000 (see platform/bootstrap/image.h),
# so it must load elsewhere.
add_executable(thandor WIN32
    src/platform/bootstrap/main.c
    src/platform/bootstrap/image.c
    src/platform/bootstrap/selftest_raster.c
    src/generated/function_map.c
    src/generated/image_data.c
)
target_link_libraries(thandor PRIVATE thandor_curated kernel32 user32 dbghelp)
target_link_options(thandor PRIVATE /BASE:0x10000000 /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO)
