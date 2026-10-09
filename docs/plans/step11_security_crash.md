# Step 11: security and crash fixes (local input)

Status: done (2026-10-09; final check: all eight run_checks pass, also with all 56 maps and the full campaign
chain; gpu_compare Vulkan and D3D12 23/23; GCC and MSVC 0 warnings; goldens identical; old savegames (end of
step 13, pre-step-13 and the three stock saves) load; none of the new rejection log lines appears on stock data or
valid saves). See "Result" at the end. The step 8 code review found more security and crash bugs than step 8 fixed. The owner
decided (2026-10-04) to move every open one out of step 8: the multiplayer/network ones go to
[step 12](step12_multiplayer_security.md), everything else is this step: file, asset, savegame and level input, local
crashes, thread races, undefined behaviour, developer-tool hardening, and the non-network owner decisions of the
review.

Already done and merged in step 8 (not repeated here): P01, P02, P06, P09, P10, P11, P18. Package ids are those of the
review's triage list (a working note outside the repository); line numbers there refer to `dev` of 2026-10-04 and
will have moved - search by function name.

## Rules

The ground rules of [step 8](step8_idiomatic_cpp.md#ground-rules) apply unchanged:

- **Formats** (PCK/LEV/FLD/MDL/ROM, savegame, network protocol, asserted layouts) stay the same.
- **Determinism**: no `Random_*` call moved, no iteration/tie order or arithmetic changed; state hash and AI hash
  identical.
- **Arena order and sizes** unchanged on valid paths; only malformed input is rejected or clamped (one log line).
- **Quirks**: an original quirk that is not memory corruption stays and is marked "Original quirk"; a corrupting or
  crashing one is bounded and commented "The original ...; bounded here because ...".
- One package = one file group and one kind of change (about 15-30 minutes); within a wave no two packages touch the
  same file. Every package states why valid data stays identical and runs its checks (`tools/test/run_checks.py`:
  determinism, AI hash, pixels, save/load, all maps, campaign chain, self-tests); both compilers build with 0 warnings.
- Several packages first measure on stock data (one-shot log run) whether a stock asset hits the new check; if one
  does, that case stays as a marked quirk instead.

## Packages

"Started" = a branch from step 8 exists but is not merged; check it against current `dev` before reuse.

### Assets (file input)

| Id | Title | Files | Change |
|---|---|---|---|
| P12 | Model + ROM record walkers, NULL model root | assets/model/definitions.cpp, assets/rom/runtime.cpp | size checks as for the army catalog; reject a model root of 0 at registration (first confirm no stock MDL has one). Uncommitted step 8 work, redo |
| P13 | Package mount lookups + PCK magic | assets/package/runtime.cpp | bound the wildcard `*` scan; lookups via `Package_FindMountSlot`; lowercase into a local buffer; **owner: check the PCK magic on mount**. Started, branch `step8-p13-mount` (includes the magic check) |
| P14 | Package writer guards | assets/package/codec.cpp, archive_write.cpp | `EncodeFieldGrid` fails on 0 cells / too small capacity; stored upsert tail copy or "no stored caller". Uncommitted step 8 work, redo |
| O1 | Archive writer transactional (owner) | assets/package/archive_write.cpp | write a temporary file, then replace the target by rename. After P14 |
| P51 | SAM decode trusts the block count | platform/sdl3/audio.cpp + the 5 voice-set callers | pass the asset byte count, reject impossible block counts, decode with an end pointer. After P16, P19, P50 |

### Savegame and level input

| Id | Title | Files | Change |
|---|---|---|---|
| P17 | AI workspace 13 saved count | gameplay/ai/workspaces.cpp | clamp the saved entry count to 3 on load |
| P19 | Level images: faction index, prefix size, terrain path offsets | gameplay/session/level_new.cpp, level_saved.cpp | faction 1..8, prefix >= 0x800 inside the image, terrain path offsets inside the LEV with room for the 6-unit suffix. Started, branch `step8-p19-levelimg` |
| P20 | Level script operands | gameplay/session/level_script.cpp | reject faction >= 8, condition index >= 64, postfix operands out of range at load |
| P22 | Savegame writer I/O results | gameplay/session/savegame.cpp | propagate upsert / write failures |
| O2 | Locked .sve shows an error (owner) | savegame load/save path | error message instead of exiting |
| P44 | Shot/effect savegame rebase | world/shots/pool.cpp, world/effects/pool.cpp | bound each saved offset to its pool, drop the slot otherwise |
| P45 | Model pool savegame rebase + NULL root | world/model/pool.cpp | same bound; Destroy tolerates a NULL root (behind P12) |

### Graphics

| Id | Title | Files | Change |
|---|---|---|---|
| P28 | Terrain soil/surface packet tables | graphics/terrain/terrain_render.cpp, terrain_resources.cpp | record payload sizes; reject a too small surface .dat; skip a soil triangle whose material is outside the table |
| P29 | Bilinear subresource scale | graphics/backend/software_texture_scale.cpp | the guards of the direct-colour bilinear stretch; clamp the last-row pointer |
| P30 | Cursor timer race + cursor count 0 | graphics/core/cursor.cpp, platform/input/devices.cpp | snapshot the frame index; atomic input clock; skip count-0 loops |
| P31 | Display mode stride, raster clip, pack-table cursor | graphics/backend/software_display_mode.cpp, software_rasterizer.cpp | stride only after the allocation succeeds; index instead of out-of-range pointer; clamp clipMax once |
| P32 | Model submit base index + child walk | graphics/render/model_submit.cpp | `index + base < count`; NULL check; child walk as index loop |
| P33 | Model lighting div0 / divisor / punning | graphics/render/model_lighting.cpp | scale 0 -> row 0; 64-bit divisor and clamped table index; shift instead of pointer pun |
| P34 | Shadow extent div0 + light record UB | graphics/render/shadow_texture.cpp, light_transitions.cpp, light_records.cpp | extent 0 skips the shadow; punning as P33; memset instead of dword walk |
| P35 | Display-mode lookup, Clone error, PCX size | graphics/backend/display_modes.cpp, graphics/resources/palette.cpp, texture_source.cpp, pcx_read.cpp | count 0 = not found; Clone returns nullptr + error; PCX size plausibility |

### UI

| Id | Title | Files | Change |
|---|---|---|---|
| P39 | Catalog div0, command loops, minimap destroy | ui/ingame/catalog_entry.cpp, commands.cpp, minimap_texture.cpp | no percent for build ticks 0; plain `for` loops; destroy tolerates NULL and clears the global |
| P40 | select.gfx / info.gfx patching | ui/ingame/selection_panel_resources.cpp | require >= 52 subresources and every patched record inside the allocation |
| P41 | Chat phrase compare + selection name copy | ui/ingame/chat.cpp, selection_detail.cpp | `memcmp` of 64; copy up to the NUL then zero-fill |
| P52 | Action queue null slots + ring slot | ui/core/runtime.cpp | skip null slots; copy the ring slot under the lock (or mark the quirk); memmove shift |
| P53 | Text edit `textBuffer[10]` accessor | ui/controls/types.h + the textBuffer sites | span accessor, layout unchanged |
| O3 | Text override table placeholder (owner) | ui/text/font.cpp | remove the inert table, keep a same-size dummy allocation at the same point (arena order unchanged, no re-baseline) |

### Gameplay and world

| Id | Title | Files | Change |
|---|---|---|---|
| P21 | Session tick counters + loaded session checks | gameplay/session/tick.cpp, new_session.cpp, loaded_session.cpp, ui/frontend/session.cpp | `atomic_ref` for the timer-shared counters; faction 1..8; propagate seek/read results (checks include MP) |
| P23 | Index-64 queue reads | gameplay/faction/army_stock.cpp, gameplay/army/factory.cpp | explicit read of the real next field; quirk markers; typed view |
| P24 | Weapons destruction effects | gameplay/army/weapons.cpp | index {effect,value} pairs via an array view; quirk marker |
| P25 | Move orders run-on | gameplay/army/move_orders.cpp | explicit loop over the 8 waypoints; named field |
| P26 | classState run-on | gameplay/army/placement_release.cpp, model_slots.cpp | 13-dword view |
| P27 | Aircraft | gameplay/army/aircraft.cpp | typed divisor + assert; assert slot < 13; quirk marker |
| P49 | Gameplay pointer truncations | gameplay/faction/economy.cpp, army_stock.cpp, gameplay/army/preview.cpp, placement.cpp, gameplay/selection/*, ui/ingame/editor_keyboard.cpp | `Thandor_PointerToU32` / army tokens; `uintptr_t` preview root. After P23 |
| P42 | Pathing cleanups | world/pathing/scratch_grid.cpp, route.cpp, gameplay/ai/construction.cpp | index loop instead of out-of-range pointer; initialise a flag; quirk comments + debug asserts |
| P43 | Pathing recursion depth | CMakeLists.txt | measure the max DFS depth on the largest map, reserve a 16 MiB stack |
| P54 | `persistedAux54` holds a pointer | world/terrain/field_lifecycle.cpp, assets/package/codec.cpp, graphics/terrain/terrain_render.cpp | store an index 0..255; check the original peer never reads it; owner decides a state hash re-baseline if the hash includes it |

### Core, audio, movie

| Id | Title | Files | Change |
|---|---|---|---|
| P36 | Fatal exit path | core/error/runtime.cpp, core/memory/synchronization.cpp, allocator.cpp, platform/sdl3/platform.cpp | re-entrancy flag; shutdown tolerates a null heap; `SdlPlatform_Quit`; one `[[noreturn]]` exit helper |
| P37 | Signed overflow in Atan2 / pmaddwd | core/math/fixed_trig.cpp, core/x86_emulation.h | arithmetic in `uint32_t`, same bits as `-fwrapv` |
| P38 | WideNumber digit scratch + UInt64Sqrt | core/text/string.cpp, core/text/types.h, core/math/fixed_vector.cpp | named scratch field filled downward (layout unchanged); estimate for shift 32 |
| P15 | SAM unaligned loads + address rounding | audio/sam.cpp | `memcpy` loads; aligned-base assert |
| P16 | Movie worker/main thread | movie/runtime/playback.cpp | `atomic_ref` for shared fields; check seek, semaphore and thread creation (synchronous fallback); shared package handle |
| P50 | Intro movie counter + playback small UB | platform/bootstrap/runtime.cpp, movie/runtime/playback.cpp, platform/debug/movie_player.cpp | atomic pending ticks; `uintptr_t` arithmetic; assert one movie; exact memmove. After P16 |
| O4 | High instead of realtime priority (owner) | platform bootstrap | `HIGH_PRIORITY_CLASS` instead of `REALTIME_PRIORITY_CLASS` |

### Developer tools

| Id | Title | Files | Change |
|---|---|---|---|
| P07 | Hang detector deadlock | platform/debug/image.cpp | only context + raw stack copy while the thread is suspended; resume, then symbolize; same for the dev watchdog |
| P08 | Dev-tool hardening | platform/sdl3/video.cpp, platform/debug/movie_player.cpp, font.cpp, platform/selftest/selftests.cpp | clip the capture region; `snprintf`; framebuffer access fields instead of a pun |
| O5 | Remove the `scanaddr` self-test (owner) | platform/selftest/selftests.cpp | delete it |
| O6 | Mark the kept tools (owner) | texture_decompose.cpp, FLM encoder | comment as kept converter / encoder tools (like the palette optimiser); no deletion |

Ordering constraints across waves: P14 before O1; P16 before P50 before P51 (P51 also after P19); P23 before P49;
P12 before P45; P52/P53 need the step 8 typed-slot (`UI_SLOT`) packages merged. P36 must be merged before step 12's
transfer thread package (P48).

Total: 47 packages (41 review packages + 6 owner-decision packages; the PCK magic check is part of P13).

## Result (2026-10-09)

All 47 packages above were done, plus 7 packages for the count-0 do-while loops step 13 had marked
"D8: kept for step 11" (none is left) and 28 follow-up packages (F1-F28) for problems the packages found on the
way. Highlights beyond the plan:

- Savegame input is checked on load: pool offsets (shots, effects, models, army slots, class links), the world
  record graph (allocated targets, no child cycles), the owner list chain, the field-grid edge ring, saved counts
  (army lists, waypoints, AI workspace), the campaign entry, and a raw shot-definition pointer of terrain-impact
  effects.
- Asset input: model/ROM record walkers and sprite paths, ROM action tables (keyframes, lights), PCK magic,
  terrain packet tables, select/info.gfx, PCX player pictures, SAM block counts, credits texture, mouse.dat
  frames, local campaign (.cgn) files, level images (prefix, path offsets, script operands).
- A missing text resolves to an empty string instead of the pointer value 0x33 (95 call sites dereferenced it).
- The pathing flood fills keep an explicit heap stack instead of recursing (up to 28 MiB of stack on the largest
  stock maps in the original); the main thread reserves 8 MiB, our own threads 1 MiB.
- Saving is transactional (temporary file, then replace); a locked .sve shows the error dialog.
- One saved-value change: field cell +0x54 (persistedAux54) holds a table index instead of an address; nothing
  reads the saved value.

Kept as marked original quirks (owner decision open): a shot reuses a world record without resetting its child
links (F22). Network findings are collected for step 12. clang-tidy: 889 -> 791 diagnostics; do_while 297 -> 209.
