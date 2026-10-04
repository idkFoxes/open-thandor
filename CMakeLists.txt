cmake_minimum_required(VERSION 3.20)
project(thandor_curated_reference C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The recovered code stores pointers in 32-bit fields (pointer32, (int)ptr casts).
# Decompiled code reads Ghidra's placeholders for unknown register halves (extraout_var, ...)
# before assigning them; the debug runtime checks would stop on each one.
string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

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

# The code needs MSVC (__stdcall, MSVC pragmas and intrinsics). Use a preset (CMakePresets.json: debug, release,
# test for 32-bit x86; x64-release, x64-test for 64-bit), which finds MSVC by itself, or configure from a vcvars
# prompt. In CLion: Settings > Build > CMake, enable the presets and disable the default MinGW profile.
if(NOT MSVC OR NOT (CMAKE_SIZEOF_VOID_P EQUAL 4 OR CMAKE_SIZEOF_VOID_P EQUAL 8))
    message(FATAL_ERROR "open-thandor must be built with MSVC (x86 or x64): use a CMake preset "
                        "(cmake --preset release) or a vcvars prompt. Found: ${CMAKE_CXX_COMPILER_ID}, "
                        "${CMAKE_SIZEOF_VOID_P}-byte pointers.")
endif()

add_library(thandor_curated STATIC
    src/assets/army/catalog.cpp
    src/assets/effect/catalog.cpp
    src/assets/model/definitions.cpp
    src/assets/package/codec.cpp
    src/assets/package/runtime.cpp
    src/assets/resource/runtime.cpp
    src/assets/rom/runtime.cpp
    src/assets/scenario/catalog.cpp
    src/assets/shot/catalog.cpp
    src/assets/sprite/catalog.cpp
    src/assets/text/resources.cpp
    src/assets/text/richtext.cpp
    src/audio/backend/runtime.cpp
    src/audio/codec/sam.cpp
    src/audio/spatial/runtime.cpp
    src/core/error/runtime.cpp
    src/core/layout_checks.cpp
    src/core/math/fixed.cpp
    src/core/math/geometry.cpp
    src/core/math/interpolation.cpp
    src/core/math/random.cpp
    src/core/memory/allocator.cpp
    src/core/memory/synchronization.cpp
    src/core/settings/persistent.cpp
    src/core/text/path.cpp
    src/core/text/string.cpp
    src/gameplay/ai/combat.cpp
    src/gameplay/ai/placement.cpp
    src/gameplay/ai/planning.cpp
    src/gameplay/ai/technology.cpp
    src/gameplay/ai/units.cpp
    src/gameplay/ai/workspaces.cpp
    src/gameplay/army/audio.cpp
    src/gameplay/army/combat.cpp
    src/gameplay/army/movement.cpp
    src/gameplay/army/placement.cpp
    src/gameplay/army/runtime.cpp
    src/gameplay/faction/relations.cpp
    src/gameplay/faction/runtime.cpp
    src/gameplay/input/world.cpp
    src/gameplay/selection/overlay.cpp
    src/gameplay/selection/runtime.cpp
    src/gameplay/session/level.cpp
    src/gameplay/session/runtime.cpp
    src/gameplay/session/savegame.cpp
    src/gameplay/technology/runtime.cpp
    src/graphics/backend/directdraw.cpp
    src/graphics/backend/software.cpp
    src/graphics/core/runtime.cpp
    src/graphics/render/model.cpp
    src/graphics/render/primitives.cpp
    src/graphics/render/projection.cpp
    src/graphics/render/shading.cpp
    src/graphics/resources/framebuffer.cpp
    src/graphics/resources/palette.cpp
    src/graphics/resources/pcx_read.cpp
    src/graphics/resources/pcx_write.cpp
    src/graphics/resources/texture.cpp
    src/movie/runtime/playback.cpp
    src/network/backend/fallback_udp.cpp
    src/network/backend/runtime.cpp
    src/network/protocol/commands.cpp
    src/network/protocol/transfer.cpp
    src/platform/bootstrap/runtime.cpp
    src/platform/filesystem/win32.cpp
    src/platform/input/devices.cpp
    src/platform/system/time_locale.cpp
    src/platform/system/win32.cpp
    src/ui/controls/buttons.cpp
    src/ui/controls/container.cpp
    src/ui/controls/gauge.cpp
    src/ui/controls/image.cpp
    src/ui/controls/input.cpp
    src/ui/controls/lists.cpp
    src/ui/controls/panels.cpp
    src/ui/controls/root_stack.cpp
    src/ui/controls/scrollable.cpp
    src/ui/controls/selectable.cpp
    src/ui/controls/slider.cpp
    src/ui/controls/text.cpp
    src/ui/controls/text_buttons.cpp
    src/ui/controls/text_edit.cpp
    src/ui/controls/tooltip.cpp
    src/ui/controls/tree_list.cpp
    src/ui/controls/window.cpp
    src/ui/core/frame_loop.cpp
    src/ui/core/model_tint.cpp
    src/ui/core/runtime.cpp
    src/ui/dialogs/display_settings.cpp
    src/ui/frontend/chat.cpp
    src/ui/frontend/debug_overlay.cpp
    src/ui/frontend/display_settings.cpp
    src/ui/frontend/end_movie.cpp
    src/ui/frontend/faction_setup.cpp
    src/ui/frontend/lifecycle.cpp
    src/ui/frontend/main_loop.cpp
    src/ui/frontend/menu_room.cpp
    src/ui/frontend/network.cpp
    src/ui/frontend/page_actions.cpp
    src/ui/frontend/player.cpp
    src/ui/frontend/results.cpp
    src/ui/frontend/scenario.cpp
    src/ui/frontend/session.cpp
    src/ui/frontend/settings.cpp
    src/ui/frontend/state.cpp
    src/ui/frontend/ui_template.cpp
    src/ui/ingame/catalog_entry.cpp
    src/ui/ingame/chat.cpp
    src/ui/ingame/commands.cpp
    src/ui/ingame/editor_keyboard.cpp
    src/ui/ingame/editor_tools.cpp
    src/ui/ingame/hud.cpp
    src/ui/ingame/key_commands.cpp
    src/ui/ingame/layout.cpp
    src/ui/ingame/music_choice.cpp
    src/ui/ingame/pages.cpp
    src/ui/ingame/savegame_page.cpp
    src/ui/ingame/selection_detail.cpp
    src/ui/ingame/settings.cpp
    src/ui/ingame/technology.cpp
    src/ui/support/runtime.cpp
    src/world/effects/maintenance.cpp
    src/world/effects/runtime.cpp
    src/world/model/hierarchy.cpp
    src/world/model/runtime.cpp
    src/world/model/slots.cpp
    src/world/motion/runtime.cpp
    src/world/pathing/grid.cpp
    src/world/pathing/influence.cpp
    src/world/runtime/core.cpp
    src/world/shots/maintenance.cpp
    src/world/shots/runtime.cpp
    src/world/terrain/editing.cpp
    src/world/terrain/grid.cpp
    src/world/terrain/height.cpp
    src/world/terrain/occupancy.cpp
    src/world/terrain/projection.cpp
    src/world/terrain/visuals.cpp
)
target_include_directories(thandor_curated PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Fixed base (REBUILT_IMAGE_BASE in platform/bootstrap/image.h): crash logs and the self-tests that run
# copies of original code rely on stable addresses.
add_executable(thandor WIN32
    src/platform/bootstrap/main.cpp
    src/platform/bootstrap/image.cpp
)
target_link_libraries(thandor PRIVATE thandor_curated kernel32 user32 dbghelp)
# The original layouts keep their pointers in 32-bit fields (Ptr32, core/ptr32.h): on x64 the process stays
# below 2 GB (/LARGEADDRESSAWARE:NO) so every pointer fits.
target_link_options(thandor PRIVATE /BASE:0x10000000 /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO)

# Platform backend. OFF (default): DirectDraw, DirectInput, DirectSound, WinMM timers and the Win32 message pump, as
# the original. ON: SDL3 (window, event pump, keyboard, mouse, timers, video presentation, audio; the software
# renderer is the same). SDL3 comes from find_package(SDL3), e.g. vcpkg: -DCMAKE_PREFIX_PATH=<vcpkg>/installed/x86-windows.
option(THANDOR_PLATFORM_SDL3 "Use SDL3 for window, input, timers, video presentation and audio" OFF)
if(THANDOR_PLATFORM_SDL3)
    # vcpkg ships Release and Debug only: RelWithDebInfo and MinSizeRel use the Release library
    set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release RelWithDebInfo)
    set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release MinSizeRel)
    find_package(SDL3 CONFIG REQUIRED)
    target_sources(thandor_curated PRIVATE
        src/platform/sdl3/audio.cpp
        src/platform/sdl3/input.cpp
        src/platform/sdl3/platform.cpp
        src/platform/sdl3/timer.cpp
        src/platform/sdl3/video.cpp
    )
    target_compile_definitions(thandor_curated PUBLIC THANDOR_PLATFORM_SDL3=1)
    target_link_libraries(thandor_curated PUBLIC SDL3::SDL3)
    # SDL3.dll next to thandor.exe
    add_custom_command(TARGET thandor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:thandor>
        VERBATIM)
endif()

# SDL3 stage 2 (needs THANDOR_PLATFORM_SDL3): the primitive queues rasterized on the GPU through SDL_GPU (D3D12),
# switched on at run time with OPEN_THANDOR_GPU=1 or -GPU; the software renderer stays the default. The shaders
# (src/platform/sdl3/shaders/primitives.hlsl) are compiled to DXBC shader model 5.1 with fxc from the Windows SDK.
option(THANDOR_RENDERER_SDL_GPU "Rasterize the primitive queues on the GPU with SDL_GPU (needs THANDOR_PLATFORM_SDL3)" OFF)
if(THANDOR_RENDERER_SDL_GPU)
    if(NOT THANDOR_PLATFORM_SDL3)
        message(FATAL_ERROR "THANDOR_RENDERER_SDL_GPU needs THANDOR_PLATFORM_SDL3=ON")
    endif()
    # fxc of the Windows SDK: on the PATH of a vcvars prompt, else the newest SDK's x64 tools
    set(THANDOR_PROGRAM_FILES_X86 "ProgramFiles(x86)")
    file(GLOB THANDOR_SDK_BIN_DIRS LIST_DIRECTORIES true "$ENV{${THANDOR_PROGRAM_FILES_X86}}/Windows Kits/10/bin/10.*/x64")
    list(SORT THANDOR_SDK_BIN_DIRS COMPARE NATURAL ORDER DESCENDING)
    find_program(THANDOR_FXC fxc HINTS ${THANDOR_SDK_BIN_DIRS} REQUIRED)
    set(THANDOR_GPU_SHADER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/sdl3/shaders/primitives.hlsl)
    set(THANDOR_GPU_SHADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/gpu_shaders)
    file(MAKE_DIRECTORY ${THANDOR_GPU_SHADER_DIR})
    set(THANDOR_GPU_SHADER_HEADERS)
    foreach(shader "vertex vs_5_1 VertexMain" "fragment ps_5_1 FragmentMain"
                   "fragment_alpha_test ps_5_1 FragmentAlphaTestMain")
        separate_arguments(shader)
        list(GET shader 0 shaderName)
        list(GET shader 1 shaderProfile)
        list(GET shader 2 shaderEntry)
        set(shaderHeader ${THANDOR_GPU_SHADER_DIR}/gpu_shader_${shaderName}.h)
        add_custom_command(OUTPUT ${shaderHeader}
            COMMAND ${THANDOR_FXC} /nologo /O3 /T ${shaderProfile} /E ${shaderEntry} /Vn g_${shaderEntry}
                    /Fh ${shaderHeader} ${THANDOR_GPU_SHADER_SOURCE}
            DEPENDS ${THANDOR_GPU_SHADER_SOURCE}
            COMMENT "fxc ${shaderEntry}"
            VERBATIM)
        list(APPEND THANDOR_GPU_SHADER_HEADERS ${shaderHeader})
    endforeach()
    target_sources(thandor_curated PRIVATE src/platform/sdl3/gpu_renderer.cpp ${THANDOR_GPU_SHADER_HEADERS})
    target_include_directories(thandor_curated PRIVATE ${THANDOR_GPU_SHADER_DIR})
    target_compile_definitions(thandor_curated PUBLIC THANDOR_RENDERER_SDL_GPU=1)
endif()

if(THANDOR_DEV_TOOLS)
    target_sources(thandor_curated PRIVATE
        src/platform/debug/autoshot.cpp
        src/platform/debug/campaign.cpp
        src/platform/debug/font.cpp
        src/platform/debug/hooks.cpp
        src/platform/debug/level_script.cpp
        src/platform/debug/movie_decoder.cpp
        src/platform/debug/movie_player.cpp
        src/platform/debug/original_code.cpp
        src/platform/debug/script.cpp
        src/platform/debug/statehash.cpp
        src/platform/debug/test_aids.cpp
        src/platform/debug/windowed.cpp
        src/platform/selftest/relax.cpp
        src/platform/selftest/selftests.cpp
    )
    target_compile_definitions(thandor_curated PUBLIC THANDOR_DEV_TOOLS=1)
endif()
