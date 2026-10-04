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
# the determinism state hash, campaign starts and AUTOWIN, level-script log, movie player/export/dump, windowed
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

# Compilers: MSVC (presets release, debug, test, gpu-test; cmake/msvc-x64.cmake finds MSVC x64 without vcvars) or
# MinGW-w64 GCC (presets mingw-release, mingw-test; cmake/mingw-x64.cmake), both for 64-bit x64 Windows with SDL3.
# In CLion: Settings > Build > CMake, enable the presets.
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8 OR NOT WIN32 OR NOT (MSVC OR (MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")))
    message(FATAL_ERROR "open-thandor must be built with MSVC or MinGW-w64 GCC for 64-bit x64 Windows (32-bit x86 is "
                        "no longer supported): use a CMake preset (cmake --preset release, cmake --preset "
                        "mingw-release) or a vcvars64 prompt. Found: ${CMAKE_CXX_COMPILER_ID}, "
                        "${CMAKE_SIZEOF_VOID_P}-byte pointers.")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # The same results as MSVC: no type-based alias analysis (the code reinterprets memory freely), signed overflow
    # wraps, no fused multiply-add (MSVC x64 /fp:precise does not contract), MSVC struct and bit-field layout
    # (-mms-bitfields, the MinGW default; layout_checks.cpp proves the layouts). char is signed in both.
    # -fpermissive: the recovered code casts pointers to 32-bit integers in places ((int)pointer, MSVC warnings
    # C4311/C4312), which g++ rejects otherwise; the cast truncates exactly as in the MSVC build and stays a warning.
    # -Wall without the style warnings the decompiled code triggers by the hundreds (parentheses, sign compare,
    # '/*' in comments).
    add_compile_options(-fno-strict-aliasing -fwrapv -ffp-contract=off -mms-bitfields -fpermissive
                        -Wall -Wno-unknown-pragmas -Wno-parentheses -Wno-sign-compare -Wno-comment)
endif()

add_library(thandor_curated STATIC
    src/assets/army/catalog.cpp
    src/assets/effect/catalog.cpp
    src/assets/model/definitions.cpp
    src/assets/package/archive_write.cpp
    src/assets/package/codec.cpp
    src/assets/package/resource_loader.cpp
    src/assets/package/runtime.cpp
    src/assets/rom/runtime.cpp
    src/assets/scenario/catalog.cpp
    src/assets/shot/catalog.cpp
    src/assets/sprite/catalog.cpp
    src/assets/text/resources.cpp
    src/assets/text/richtext.cpp
    src/assets/text/richtext_markup.cpp
    src/audio/backend/null_backend.cpp
    src/audio/backend/runtime.cpp
    src/audio/codec/sam.cpp
    src/audio/codec/sam_encoder.cpp
    src/audio/spatial/runtime.cpp
    src/core/error/runtime.cpp
    src/core/layout_checks.cpp
    src/core/math/fixed_transform.cpp
    src/core/math/fixed_trig.cpp
    src/core/math/fixed_vector.cpp
    src/core/math/geometry.cpp
    src/core/math/random.cpp
    src/core/math/spline.cpp
    src/core/memory/allocator.cpp
    src/core/memory/synchronization.cpp
    src/core/settings/persistent.cpp
    src/core/text/path.cpp
    src/core/text/string.cpp
    src/gameplay/ai/combat.cpp
    src/gameplay/ai/construction.cpp
    src/gameplay/ai/perception.cpp
    src/gameplay/ai/placement.cpp
    src/gameplay/ai/planning.cpp
    src/gameplay/ai/purchasing.cpp
    src/gameplay/ai/technology.cpp
    src/gameplay/ai/units.cpp
    src/gameplay/ai/workspaces.cpp
    src/gameplay/army/aircraft.cpp
    src/gameplay/army/audio.cpp
    src/gameplay/army/class_dispatch.cpp
    src/gameplay/army/class_updates.cpp
    src/gameplay/army/collision.cpp
    src/gameplay/army/combat.cpp
    src/gameplay/army/damage.cpp
    src/gameplay/army/drive_banking.cpp
    src/gameplay/army/drive_common.cpp
    src/gameplay/army/drive_ground.cpp
    src/gameplay/army/factory.cpp
    src/gameplay/army/model_rules.cpp
    src/gameplay/army/model_slots.cpp
    src/gameplay/army/move_orders.cpp
    src/gameplay/army/placement.cpp
    src/gameplay/army/placement_contact.cpp
    src/gameplay/army/placement_release.cpp
    src/gameplay/army/pool.cpp
    src/gameplay/army/preview.cpp
    src/gameplay/army/turrets.cpp
    src/gameplay/army/walker.cpp
    src/gameplay/army/weapons.cpp
    src/gameplay/faction/army_stock.cpp
    src/gameplay/faction/economy.cpp
    src/gameplay/faction/relations.cpp
    src/gameplay/faction/runtime.cpp
    src/gameplay/selection/commands.cpp
    src/gameplay/selection/queries.cpp
    src/gameplay/selection/selection_set.cpp
    src/gameplay/session/campaign_carryover.cpp
    src/gameplay/session/level.cpp
    src/gameplay/session/level_new.cpp
    src/gameplay/session/level_saved.cpp
    src/gameplay/session/level_script.cpp
    src/gameplay/session/loaded_session.cpp
    src/gameplay/session/loading_movie.cpp
    src/gameplay/session/new_session.cpp
    src/gameplay/session/runtime.cpp
    src/gameplay/session/savegame.cpp
    src/gameplay/session/savegame_load.cpp
    src/gameplay/session/scenario_load.cpp
    src/gameplay/session/startup.cpp
    src/gameplay/session/tick.cpp
    src/gameplay/technology/runtime.cpp
    src/graphics/backend/directdraw.cpp
    src/graphics/backend/software.cpp
    src/graphics/backend/software_blit.cpp
    src/graphics/backend/software_display_mode.cpp
    src/graphics/backend/software_rasterizer.cpp
    src/graphics/backend/software_texture_scale.cpp
    src/graphics/core/cursor.cpp
    src/graphics/core/device.cpp
    src/graphics/render/light_records.cpp
    src/graphics/render/light_transitions.cpp
    src/graphics/render/model_draw.cpp
    src/graphics/render/model_lighting.cpp
    src/graphics/render/model_submit.cpp
    src/graphics/render/offscreen.cpp
    src/graphics/render/primitives.cpp
    src/graphics/render/projection.cpp
    src/graphics/render/shadow_texture.cpp
    src/graphics/resources/framebuffer.cpp
    src/graphics/resources/palette.cpp
    src/graphics/resources/palette_optimizer.cpp
    src/graphics/resources/pcx_read.cpp
    src/graphics/resources/pcx_write.cpp
    src/graphics/resources/texture_decompose.cpp
    src/graphics/resources/texture_set.cpp
    src/graphics/resources/texture_source.cpp
    src/graphics/resources/tiled_blit.cpp
    src/graphics/terrain/terrain_render.cpp
    src/graphics/terrain/terrain_resources.cpp
    src/movie/runtime/flm_decoder.cpp
    src/movie/runtime/flm_encoder.cpp
    src/movie/runtime/playback.cpp
    src/network/backend/fallback_udp.cpp
    src/network/backend/runtime.cpp
    src/network/protocol/cipher.cpp
    src/network/protocol/command_exchange.cpp
    src/network/protocol/commands.cpp
    src/network/protocol/frontend_session.cpp
    src/network/protocol/lobby.cpp
    src/network/protocol/mailbox.cpp
    src/network/protocol/scenario_transfer.cpp
    src/platform/bootstrap/runtime.cpp
    src/platform/filesystem/win32.cpp
    src/platform/input/devices.cpp
    src/platform/system/time_locale.cpp
    src/platform/system/win32.cpp
    src/ui/controls/buttons.cpp
    src/ui/controls/container.cpp
    src/ui/controls/focus_proxy.cpp
    src/ui/controls/gauge.cpp
    src/ui/controls/image.cpp
    src/ui/controls/input.cpp
    src/ui/controls/lists.cpp
    src/ui/controls/minimap.cpp
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
    src/ui/core/chat_history.cpp
    src/ui/core/frame_loop.cpp
    src/ui/core/model_tint.cpp
    src/ui/core/pcx_preview.cpp
    src/ui/core/runtime.cpp
    src/ui/dialogs/display_settings.cpp
    src/ui/dialogs/fatal_error.cpp
    src/ui/frontend/chat.cpp
    src/ui/frontend/credits.cpp
    src/ui/frontend/credits_mask.cpp
    src/ui/frontend/debug_overlay.cpp
    src/ui/frontend/display_settings.cpp
    src/ui/frontend/end_movie.cpp
    src/ui/frontend/end_movie_commands.cpp
    src/ui/frontend/faction_setup.cpp
    src/ui/frontend/lifecycle.cpp
    src/ui/frontend/main_loop.cpp
    src/ui/frontend/menu_room.cpp
    src/ui/frontend/menu_room_scene.cpp
    src/ui/frontend/mission_briefing.cpp
    src/ui/frontend/network.cpp
    src/ui/frontend/page_actions.cpp
    src/ui/frontend/player.cpp
    src/ui/frontend/results.cpp
    src/ui/frontend/scenario_selection.cpp
    src/ui/frontend/session.cpp
    src/ui/frontend/settings.cpp
    src/ui/frontend/state.cpp
    src/ui/frontend/task_assignment.cpp
    src/ui/frontend/ui_template.cpp
    src/ui/ingame/army_stock.cpp
    src/ui/ingame/build_catalog.cpp
    src/ui/ingame/camera_commands.cpp
    src/ui/ingame/catalog_entry.cpp
    src/ui/ingame/chat.cpp
    src/ui/ingame/command_buttons.cpp
    src/ui/ingame/commands.cpp
    src/ui/ingame/editor_army_cycling.cpp
    src/ui/ingame/editor_keyboard.cpp
    src/ui/ingame/editor_tool_selection.cpp
    src/ui/ingame/editor_tools.cpp
    src/ui/ingame/hotkeys.cpp
    src/ui/ingame/hud.cpp
    src/ui/ingame/key_commands.cpp
    src/ui/ingame/layout.cpp
    src/ui/ingame/minimap_texture.cpp
    src/ui/ingame/music_choice.cpp
    src/ui/ingame/notifications.cpp
    src/ui/ingame/pages.cpp
    src/ui/ingame/preview_markers.cpp
    src/ui/ingame/root_frame.cpp
    src/ui/ingame/savegame_page.cpp
    src/ui/ingame/selection_detail.cpp
    src/ui/ingame/selection_overlay.cpp
    src/ui/ingame/selection_panel_cells.cpp
    src/ui/ingame/selection_panel_metrics.cpp
    src/ui/ingame/selection_panel_resources.cpp
    src/ui/ingame/settings.cpp
    src/ui/ingame/targeting.cpp
    src/ui/ingame/technology.cpp
    src/ui/ingame/ui_template.cpp
    src/ui/ingame/world_input.cpp
    src/ui/text/font.cpp
    src/ui/text/richtext_render.cpp
    src/world/camera/camera.cpp
    src/world/camera/camera_input.cpp
    src/world/camera/motion_spline.cpp
    src/world/effects/lifecycle.cpp
    src/world/effects/pool.cpp
    src/world/model/hierarchy.cpp
    src/world/model/mesh_raycast.cpp
    src/world/model/picking.cpp
    src/world/model/pool.cpp
    src/world/pathing/influence.cpp
    src/world/pathing/path_cost.cpp
    src/world/pathing/reachability.cpp
    src/world/pathing/route.cpp
    src/world/pathing/scratch_grid.cpp
    src/world/runtime/core.cpp
    src/world/runtime/entity_registry.cpp
    src/world/shots/ballistics.cpp
    src/world/shots/flight.cpp
    src/world/shots/pool.cpp
    src/world/terrain/editing.cpp
    src/world/terrain/field_deformation.cpp
    src/world/terrain/field_edit_commands.cpp
    src/world/terrain/field_lifecycle.cpp
    src/world/terrain/field_lighting.cpp
    src/world/terrain/field_raycast.cpp
    src/world/terrain/field_sampling.cpp
    src/world/terrain/fog_visibility.cpp
    src/world/terrain/hex_scan.cpp
    src/world/terrain/occupancy.cpp
    src/world/terrain/overlay_marking.cpp
    src/world/terrain/placement_tests.cpp
    src/world/terrain/sight.cpp
    src/world/terrain/water_relaxation.cpp
)
target_include_directories(thandor_curated PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Fixed base (REBUILT_IMAGE_BASE in platform/bootstrap/image.h): the crash logs rely on stable code addresses.
add_executable(thandor WIN32
    src/platform/bootstrap/main.cpp
    src/platform/bootstrap/image.cpp
)
target_link_libraries(thandor PRIVATE thandor_curated kernel32 user32 dbghelp)
# The original layouts keep their pointers in 32-bit fields (Ptr32, core/ptr32.h): the process stays below 2 GB
# (/LARGEADDRESSAWARE:NO) so every pointer fits.
if(MSVC)
    target_link_options(thandor PRIVATE /BASE:0x10000000 /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO)
else()
    # GNU ld: the same fixed base, no ASLR, not large-address aware; the C and C++ runtimes linked in statically
    # (only SDL3.dll ships next to thandor.exe); thandor.map for symbolizing crash logs (docs/BUILDING.md).
    target_link_options(thandor PRIVATE -Wl,--image-base=0x10000000 -Wl,--disable-dynamicbase
                        -Wl,--disable-high-entropy-va -Wl,--disable-large-address-aware
                        -static -static-libgcc -static-libstdc++ -Wl,-Map=$<TARGET_FILE_DIR:thandor>/thandor.map)
endif()

# Platform backend: SDL3 (window, event pump, keyboard, mouse, timers, video presentation, audio; the game's software
# renderer draws the frames). SDL3 comes from find_package(SDL3): pass its install prefix in CMAKE_PREFIX_PATH (vcpkg:
# -DCMAKE_PREFIX_PATH=<vcpkg>/installed/x64-windows, MinGW: .../x64-mingw-dynamic), or set the environment variable
# VCPKG_ROOT, whose installed/x64-windows (MinGW: installed/x64-mingw-dynamic) is searched as well.
if(MSVC)
    set(THANDOR_VCPKG_TRIPLET x64-windows)
else()
    set(THANDOR_VCPKG_TRIPLET x64-mingw-dynamic)
endif()
if(DEFINED ENV{VCPKG_ROOT})
    file(TO_CMAKE_PATH "$ENV{VCPKG_ROOT}/installed/${THANDOR_VCPKG_TRIPLET}" THANDOR_VCPKG_PREFIX)
    list(APPEND CMAKE_PREFIX_PATH "${THANDOR_VCPKG_PREFIX}")
endif()
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
target_link_libraries(thandor_curated PUBLIC SDL3::SDL3)
# SDL3.dll next to thandor.exe
add_custom_command(TARGET thandor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:thandor>
    VERBATIM)

# SDL3 stage 2: the primitive queues rasterized on the GPU through SDL_GPU (D3D12),
# switched on at run time with OPEN_THANDOR_GPU=1 or -GPU; the software renderer stays the default. The shaders
# (src/platform/sdl3/shaders/primitives.hlsl) are compiled to DXBC shader model 5.1 with fxc from the Windows SDK.
option(THANDOR_RENDERER_SDL_GPU "Rasterize the primitive queues on the GPU with SDL_GPU" OFF)
if(THANDOR_RENDERER_SDL_GPU)
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
        src/platform/debug/script.cpp
        src/platform/debug/statehash.cpp
        src/platform/debug/test_aids.cpp
        src/platform/selftest/selftests.cpp
    )
    target_compile_definitions(thandor_curated PUBLIC THANDOR_DEV_TOOLS=1)
endif()
