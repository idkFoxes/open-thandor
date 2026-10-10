# Campaign map: mission selection with progress

Status: plan (2026-10-10, branch `experimental/campaign-map` from `dev` 113ce57f). This is a read-only analysis
with no code changes. Line numbers refer to 113ce57f and will move, so search by function name. Items marked
**(guess)** are estimates and were not measured. Data facts about the stock campaigns were measured with a dump of
`LEVEL.PCK` (`level\*.cgn`, `level\campagne.dat`) and with `tools/data/lev.py` on the levels of all four campaigns
(`tools/data/cgn.py dump <level dir> --triggers`, CM1).

On the campaign tab of the "Spiel auswählen" page, the plain list of campaigns is replaced by:

- a row of campaign buttons that show the progress, and
- a mission map of the selected campaign.

From the map, any mission that is done or unlocked can be started. Progress is kept in a small file next to the
saves. The file also holds the units a mission takes over from its predecessor, so a mission that needs them can
be started directly.

## 1. Goal and non-goals

**Goal**

- **Campaign tab.** It shows a row of campaign buttons with progress (`Tutorial 3/3`, `Die Invasion 7/24`,
  `Kyrrdon 2/5`, `Piraten 4/4`) and the mission map of the selected campaign.
- **Mission map.**
  - Nodes carry the original mission numbers.
  - Paths come from the campaign's successor tables. Branches are stacked.
  - Each node has a state: done, unlocked, locked, plus a selection.
  - Carry-over edges are drawn as a double line.
- **Below the map.** `Mission N: <title>`, the description and a status line, with the campaign title right-aligned.
- **Progress.** It is recorded without savegames: every win marks the mission done and unlocks its successors.
  Progress survives quitting the game.
- **Start any mission.** Every done or unlocked mission can be started. A mission that takes over units gets the
  units of the last win of its predecessor.
- **Original flow kept.** The original campaign flow is unchanged: after a win the game goes on to the next mission
  through the briefing room, exactly as today.

**Non-goals**

- No change to the simulation, the savegame format (`.sve`, `campagne.hex`, `oldunit.hex`), the level and campaign
  formats or the network protocol.
- No new campaign content and no campaign editor.
- No keyboard navigation on the map (mouse only; owner decision Q9).
- No import of progress from existing savegames (owner decision Q5).
- No cloud or per-user profile folder: the file lives in `save\` next to the executable, like the saves.

## 2. Owner decisions (approved concept, not changed by this plan)

| # | Decision |
|---|---|
| O1 | The campaign tab of the original "Spiel auswählen" page shows a row of campaign buttons with progress (`done/visible missions`) instead of the list. Below it is the mission map of the selected campaign. The campaigns are the ones in `LEVEL.PCK` `level\campagne.dat`: `tutorial.cgn`, `hansolo.cgn` (Thandor - Die Invasion), `nimm2.cgn` (Die Unterwerfung von Kyrrdon), `luke.cgn` (Kampf der Piraten). |
| O2 | Map nodes are missions with their original numbers. Paths come from `successorLevelIds`, and branches are stacked. States: done = green, unlocked = raised button, locked = dimmed with no padlock, selected = pressed. Paths are thin and light. Carry-over edges are a double line. The legend has one item, `Einheitenübernahme`, and is shown only when the campaign has a carry-over edge. |
| O3 | Below the map: `Mission N: <title>`, the description and a status line, left-aligned with the map frame. The campaign title is right-aligned. `Abbrechen` and `Starten` stay in their original places. |
| O4 | Missions without a level file are hidden: hansolo mission 0 (`s00_tut`) and mission 19 (`s19_forschung`). |
| O5 | No savegames are needed for progress. Every mission won is done and unlocks its successor(s) for the end selection it was won with. Progress survives quitting the game. |
| O6 | Any done or unlocked mission can be (re)started. Replaying keeps all progress behind it. Finishing a branching mission with a different end also unlocks the other branch. Carry-over units are replaced by the newest result. |
| O7 | A mission that takes over units (hansolo 7->8, 11->12, 21->22; tutorial t00->t01->t02) gets the units of the predecessor's last win when it is started from the map. The carry-over snapshot is saved with the progress. |
| O8 | The start-point copies (`hansolo4/8/11/16/21.cgn`, titles "Thandor - Die Invasion (07)/(10)/..."; also `nimm2_2..5.cgn`) become unnecessary. This plan decides whether to hide them (section 5.6). |
| O9 | Compatibility is kept for savegames, levels, the network protocol, the original continue-after-a-win flow, and network campaigns. |

The approved mockups are `ot-scratch\campaign-mockup\mockup_in_page_v4_1280.png` (Die Invasion) and
`mockup_in_page_v4_nimm2_1280.png` (Kyrrdon). Their generator is `tools\compose.py` (`page_v3`). Section 6 converts
its layout numbers into template offsets.

## 3. Ground rules

The ground rules of [step 8](step8_idiomatic_cpp.md#ground-rules) and [step 11](step11_security_crash.md#rules)
apply. For this feature they mean:

- **Formats stay.**
  - Unchanged: PCK, LEV, FLD, CGN, `campagne.dat`, `.sve` (including the `campagne.hex` and `oldunit.hex` entries),
    and every layout asserted in `src/core/layout_checks.cpp`.
  - The only new file is the progress file. It is our own and versioned.
- **Network protocol stays.**
  - No new command code and no new packet.
  - A start mission is passed in an argument dword of the existing command `FRONTEND_COMMAND_LOAD_CAMPAIGN`. That
    dword is unused today and an original peer ignores it (section 7.4).
  - An original client still gets a normal campaign bundle.
- **Simulation stays.**
  - No `Random_*` call moves and no simulation code changes.
  - Progress is recorded after the end movie, from values the session already left behind.
  - A snapshot restore writes the same tables, with the same values, that the original level change writes
    (`OldUnitRuntime_RebuildScenarioReplayTables`, then `OldUnitRuntime_MergeMasksAndReplayRecords`).
- **Arena order stays.**
  - All new I/O and all caches use native memory (`std::vector`, `std::string`) and native file calls, never
    `g_MemoryApi.alloc/free`.
  - The progress file, the level header reads for the map and the snapshot copies are therefore invisible to the
    arena.
  - `Resource_Load`, `Package_LoadEntry` and `FileSystem_ReadWholeFile`-style helpers (`src/platform/filesystem/win32.cpp:219`)
    allocate from the arena, so the new code must not use them. See CM2 and CM4.
  - Measured (CM2): `Package_FindEntryAcrossMounts` + `Package_DecodeEntryInto` into a caller buffer make no
    `g_MemoryApi` call for compression methods 0 and 1 (all 55 stock `.lev` entries are method 0); only method 2
    (field grids, `.fld`) allocates. Details in 5.4.
- **Untrusted input.** The progress file is user-editable.
  - It is validated on load as in step 11: a malformed item is ignored with one log line (`Thandor_Log`, context
    `campaign progress: ...`).
  - An unreadable file is kept as `*.bad` and replaced by empty progress.
  - Nothing from the file reaches the simulation unchecked (section 4.4).
- **Original quirks stay** (section 7.6). Examples: hansolo mission 15 continues to mission 17 after a loss; the
  in-game restart of the first tutorial mission leaves the campaign. The map makes them recoverable but does not
  change them.
- **Work mode** follows the earlier steps: small packages (S about 15 min, M about 25-30 min), one kind of change and
  one file group each, every package in its own worktree, merged wave by wave. No push. Per package: GCC and MSVC
  with 0 new warnings, the self-tests, and `golden_cmp.ps1`. The full `tools/test/run_checks.py` (all eight checks)
  runs at the end, with spot checks during the work (section 10).

## 4. Data model

### 4.1 What the campaign data says (measured)

`CampaignAsset` is defined in `include/thandor/assets/scenario/catalog.h`:

- a 0x200-byte header holding `firstLevelId` (+0xB4), `levelRecordCount` (+0xB8) and `currentLevelId` (+0xC4);
- followed by `levelRecordCount` records of type `CampaignLevelRecord`, 0x180 bytes each.

Each record holds:

- `successorLevelIds[8]` (one per end selection);
- end movie numbers;
- per-faction exit zones;
- `levelId`;
- `carryOverMask` and `skipMask` (one bit per end selection);
- `levelFileName`.

| Campaign | first | records | Notes |
|---|---|---|---|
| `tutorial` | 1 | 3 | t00 succ `[0,2]` carry `0x2` skip `0x1`; t01 succ `[0,3]` carry `0x2` skip `0x1`; t02 succ `[3,0]` carry 0 skip `0x1` |
| `hansolo` | 2 | 26 | level id = mission number + 1. Almost every level: succ `[self, next]`. Branches: id 5 (m4) `[5,6,7]`, id 13 (m12) `[13,14,15]`. Carry `0x2` + skip `0x1` on ids 8, 12, 22 (m7, m11, m21); skip `0x1` on ids 9, 13, 23 (m8, m12, m22). Odd ones: id 16 (m15) `[18,17]`, id 19 (m18) `[19,21]` (skips m19), id 26 (m25) `[25,0]`. Nothing leads to id 20 (m19) and id 1 (m0); their level files are missing. |
| `nimm2` | 1 | 5 | succ `[self, next]`, last `[5,0]`; no carry-over |
| `luke` | 1 | 4 | succ `[self, next]`, last `[4,0]`; no carry-over |

`campagne.dat` has four 0x100-byte catalog records. Each holds the file name, the minimum and maximum players and the
campaign title index:

| File | Players | Title index |
|---|---|---|
| hansolo | 1-6 | 1 |
| luke | 2-3 | 12 |
| nimm2 | 2-3 | 7 |
| tutorial | 1-2 | 0 |

So **nimm2 and luke are network campaigns**, and hansolo can be played over the network with up to 6 players.

The start-point copies are in `LEVEL.PCK` but **not listed in the stock `campagne.dat`**:

- `hansolo4/8/11/16/21.cgn`: the same successors, masks, exit zones and level files as `hansolo.cgn`, `firstLevelId`
  4/8/11/16/21; they differ only in the success movie numbers;
- `nimm2_2..5.cgn`: `firstLevelId` 2..5.

Their titles exist in `level.str` (campaign title indices 2-6, 8-11, 13-15). They can only appear through an add-on
`level\campagne00..99.dat` (`ScenarioCatalog_MergeAddOnFiles`, `src/assets/scenario/catalog.cpp`).

The end triggers in the level scripts were measured with `tools/data/lev.py`:

- **hansolo.** Every level has `(faction 1, selection 0, variant 0)`, which is the loss: the player's faction
  ended with the failure movie. It also has `(faction 1, selection 1[,2], variant 1)` for the wins.
- **nimm2.** The same pairs for factions 1 and 2.
- **tutorial.** Every level has `(faction 1, selection 0, variant 0)` and `(faction 1, selection 1, variant 1)`.
- **luke.** The same pairs for factions 1 and 2, like nimm2.

In all four campaigns selection 0 always comes with variant 0 and every other selection with variant 1. The dump
flags no exception. The missing hansolo levels m0 and m19 have no triggers to check.

The other record fields, measured with `tools/data/cgn.py`:

- **End movies.** Failure movie 1 on every selection, success movie per level (0 = none). In the tutorial both are
  1000.
- **Exit zones.** Only the levels with carry-over have them (t00, t01, m7, m11, m21). Each zone has centre (0, 0),
  radius 1000000 Q12 (the whole map) and destination (0, 0). There are 5 zones (indices 0-4), and 3 on m7. So the
  carry-over takes every unit of those factions and keeps its position.

So in the stock data **end selection 0 is the loss outcome**, and its successor is the level itself (retry). The
exceptions are hansolo m15 (loss continues to m17), m25 (loss goes back to m24) and tutorial t00 and t01 (loss ends
the campaign: both have `successorLevelIds[0] = 0`).

### 4.2 Outcome 0 and `successorLevelIds[0]`: how retry works today (the step 11 open question)

The end of a session takes one of three paths in `InGameRuntime_RunSessionUntilExit`
(`src/gameplay/session/startup.cpp`).

1. **End trigger (win or loss).**
   - The level script sets `g_EndMovieSelectionIndex` and `g_EndMovieVariantIndex` in
     `InGameConditionRuntime_RequestEndMovie` (`level_script.cpp`), then sets
     `UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING`.
   - The loop plays the end movie, calls `OldUnitRuntime_RebuildScenarioReplayTables()` with that selection and
     clears `g_FrontendScenarioPathScratchUtf16`.
   - `FrontendMainLoop_ContinueAfterSession` then calls `FrontendMainLoop_SelectCampaignSuccessorLevel`, which sets
     `currentLevelId = successorLevelIds[selection]`. That is the next mission on a win, and the same mission on a
     loss (selection 0 = retry).
2. **In-game "Mission neu starten"** (`InGameQuitMenu_RestartMission`, `src/ui/ingame/pages.cpp`; offered only in
   local games).
   - It issues command 0x150 with `INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION`, which sets
     `UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED`.
   - The loop sets `g_EndMovieSelectionIndex = 0` and calls `OldUnitRuntime_RebuildScenarioReplayTables()`. It does
     **not** clear the scenario path.
   - The successor is again `successorLevelIds[0]`, so a restart *is* a retry through the loss outcome.
3. **"Mission abbrechen" / player left** (`LOCAL_PLAYER_LEFT`). The campaign asset is released (`commands.cpp`,
   `InGameCommand_HandlePlayerDeparture`) and the scenario path is cleared, so the game returns to the menu.

**How the carry-over survives a retry.** For selection 0, `RebuildScenarioReplayTables` reads bit 0 of
`skipMask`/`carryOverMask`. Every mission that **receives** units (t01, t02, m8, m12, m22) has skip bit 0 set. With
skip set, the function returns before it touches the primary records or the count, and it leaves the secondary
(technology) table alone. The retry therefore gets the same incoming units again.

Missions that receive nothing have skip 0 and carry 0 for selection 0. The tables are cleared, which is harmless.

So recording carry-over "for outcome 0" on a restart is correct by design. Following `successorLevelIds[0]` is
correct as well, because it is the retry in the stock data.

The four exceptions are original quirks and stay (section 7.6):

- **m15:** restart or loss goes on to m17;
- **m25:** restart or loss goes back to m24;
- **t00:** `successorLevelIds[0] = 0` ends the campaign. Because the restart path keeps the scenario path, the
  briefing reloads t00 as a *single* level without the campaign. A win then carries nothing into t01.
- **t01:** the same as t00 (`successorLevelIds[0] = 0`, measured in CM1); a win after the restart carries nothing
  into t02.

**Consequences for this plan**

- Progress is recorded **only** on path 1 and **only** for a win (section 4.3). A restart (path 2) and an abort
  (path 3) never change progress.
- A retry after a loss of a map-started mission keeps the snapshot that was restored at its start, through the same
  skip bit. Nothing extra is needed.
- The win rule must not rely on "selection != 0" alone. Hansolo m15 continues after a loss.

### 4.3 When a session counts as a win

A session counts as a **win** of mission `L` (the campaign's `currentLevelId` at the end of the session) when all of
the following hold:

- it ended through the end trigger path (1);
- `g_EndMovieVariantIndex != 0` (the success movie: the triggers of the local player's wins have
  `movieVariantSelector` 1, the losses have 0; a coop partner's win gives the local player variant 1 through the
  relation rule in `InGameConditionRuntime_RequestEndMovie`);
- `successorLevelIds[selection] != L`.

The selection itself is stored with the win, because it decides which branch is unlocked.

When the successor is 0 or not a level of the campaign, the win finishes the campaign (finale). The mission is done
and nothing new is unlocked.

**Unlocking.** A mission is unlocked when any of these holds:

- it is the base campaign's first level;
- it is the successor of a recorded win (for that win's selection);
- **a session of it started** in this campaign (any path, including a loss that continues forward like m15 -> m17,
  and a loaded savegame).

The third rule keeps the map consistent with what the original chain lets the player play (Q2).

### 4.4 The progress file

**Location.** `save\campaign_progress.txt` under `g_ExecutableDirectoryUtf16`, built like the save paths
(`WidePath_CombineDirectoryAndLeaf` with `g_SaveDirectoryUtf16`). The directory is created when it is missing
(`g_FileSystemCreateDirectoryRecursive`, as `InGameSaveGame_OpenNewPackage` does). The test aid
`OPEN_THANDOR_CAMPAIGN_PROGRESS=<path>` overrides the path (CM9).

**Why text.** Text is readable, easy to repair, and diffable in tests. Its size is small: about 20 KB with three
full snapshots of 0x200 units (guess). It needs no arena (unlike a PCK package written with `Package_UpsertEntry`,
whose `ArenaScoped` blocks and mount slots would add arena traffic between sessions). A missing key never breaks a
reader: unknown lines are ignored.

**Format (version 1).** UTF-8, one item per line, `#` comments. The file name is ASCII. Example:

```
open-thandor campaign progress 1
[campaign hansolo]
fingerprint 9f3a12c4 26
done 2 1
done 3 1
done 5 2
unlocked 2 3 4 5 6 7
carry 9 from 8 sel 1 units 37
unit 0x00000041 1 1234567 -234567 512
...
tech 1 0 0 0 0 0 0 0 0
...
end
```

| Item | Meaning | Validation on load |
|---|---|---|
| header line | magic text plus version | a wrong magic or a newer version means the whole file is ignored: one log line, the file is renamed to `campaign_progress.bad` and progress starts empty. Without the rename, the next win would overwrite it. |
| `[campaign <name>]` | campaign key: the `.cgn` base name of the base campaign (5.6) | name of 1-31 ASCII characters `[A-Za-z0-9_]`, otherwise the section is skipped (one log line) |
| `fingerprint <crc32> <records>` | CRC32 over the level records without the header (`levelId`, `successorLevelIds`, masks, `levelFileName`) plus the record count | when the loaded `.cgn` differs (an add-on changed the campaign), the section is kept in the file but not applied, with one log line. Only the matching levels would be valid, so the whole section waits. |
| `done <levelId> <selection>` | a won mission and the end selection of its **last** win; every selection ever won is also kept (`done 5 1` and `done 5 2` both) so both branches stay unlocked | level id must be a record of the campaign (checked when applied), selection 0..7 |
| `unlocked <ids...>` | explicitly unlocked levels (rule 3 of 4.3; successors of wins are derived and not stored) | ids must be records of the campaign |
| `carry <target> from <source> sel <s> units <n>` | the carry-over snapshot for starting `<target>` directly: the old-unit tables as `OldUnitRuntime_RebuildScenarioReplayTables` left them after the last win of `<source>` | target must be `successorLevelIds[s]` of source with `carryOverMask` bit `s` set; `n <= OLD_UNIT_PRIMARY_RECORD_CAPACITY` (0x200); exactly `n` `unit` lines and 8 `tech` lines follow, otherwise the snapshot is dropped (one log line) |
| `unit <asset> <faction> <x> <y> <rotation>` | primary record dwords [0..4] (`campaign_carryover.cpp`: asset id, faction, X, Y in Q12, rotation) | asset id: any `int32` (decimal or `0x` hex); there is no numeric bound to check (CM2, see "Army asset ids" below); faction 0..7; X and Y inside the level's world bounds (checked when applied, against the loaded level's field size); rotation masked like the engine's angle |
| `tech <faction> <w0..w7>` | secondary table, 8 dwords per faction | faction 0..7, each faction at most once; missing factions are 0 |
| `end` | end of the section | a section without `end` (a truncated file) is dropped (one log line) |

**Army asset ids (measured in CM2).** The engine has no numeric range for army asset ids:

- `PckArmyAssetIdCatalog` is a plain `int` (`include/thandor/core/types.h:397`).
- `ArmyRuntime_CreateInstanceFromAsset` (`src/gameplay/army/pool.cpp:281`) looks the id up with
  `ArmyAssetRegistry_FindRecordById` (`src/assets/army/catalog.cpp:265`). That function compares `registryId` over
  the 768 slots of `g_ArmyAssetRecordRegistry` (`ARMY_ASSET_REGISTRY_SLOT_COUNT`,
  `include/thandor/assets/army/catalog.h:17`).
- The registry is filled per level from the ARM files in the level's ARM path table:
  `NewLevel_LoadAssetList` (`src/gameplay/session/level_new.cpp:833`) -> `ArmyAsset_PrepareRecords` ->
  `ArmyAssetRecord_RegisterAndRelocate` (`catalog.cpp:52`, `:232`). It is cleared when the level is released
  (`pool.cpp:186-193`). So the valid set exists only while the target level is loaded, and it is not a range.
- An unknown id is harmless:
  - `CreateInstanceFromAsset` returns `nullptr` with `FATAL_ERROR_ARMY_ID_NOT_FOUND` before it takes the free
    slot. `ArmyRuntimePool_FindFreeSlot` (`pool.cpp:243`) only finds the slot and does not mark it.
  - The only side effect is the id written as text into `g_PackageLastErrorPath`.
  - `OldUnitRuntime_MergeMasksAndReplayRecords` passes `outError = nullptr` and ignores the result
    (`src/gameplay/session/campaign_carryover.cpp:188-191`).
  - The original chain already behaves this way for a carried unit whose ARM is missing in the next level.
- The faction is clamped to 7 inside `CreateInstanceFromAsset` (`pool.cpp:321`). The pre-clamp value is only
  compared with the active faction, so the 0..7 check above is enough.
- Consequence for CM3: the load-time check of `unit` lines is syntactic only (the value fits `int32`). The test
  "bad asset id" means a non-numeric or out-of-`int32` token, which drops the snapshot. CM8 needs no id check at
  restore: the registry of the target level does not exist yet at that point (`scenario_load.cpp:449`); it is
  built before the merge (`new_session.cpp:238`).

**Writing.** The whole file is rewritten after every recorded change:

1. format it into a `std::string`;
2. write `campaign_progress.txt.tmp` (`Package_MakeTemporaryPath` makes the name);
3. replace the target with `Win32File_Replace` (`MoveFileExA` with REPLACE_EXISTING | WRITE_THROUGH,
   `src/platform/filesystem/win32.cpp:376`);
4. on failure, delete the temporary file and write one log line.

The game never fails because of the progress file. A failed write only loses that update.

`FileSystem_WriteBufferToPath` writes a caller buffer without the arena. `win32.cpp` has no native read helper (its
whole-file reader allocates from the arena), so CM4 adds one.

**In memory.** `CampaignProgress` (new, `include/thandor/gameplay/session/campaign_progress.h`) is a native
`std::vector` of campaign sections. Each section holds `std::map<int32_t, ...>`-style sorted containers. Every
container is sorted, so the file is written in a stable order and two runs produce identical files. The file is
read once, when the scenario selection page first opens, and on every write.

### 4.5 What a snapshot holds and how it is taken and restored

The tables are persistent bootstrap allocations (`CoreAssets_AllocateRuntimeBuffers`,
`src/platform/bootstrap/runtime.cpp`):

- `g_OldUnitPrimaryTable`: 0x4000 bytes = 0x200 records of 8 dwords;
- `g_OldUnitSecondaryTable`: 0x100 bytes = 8 factions x 8 dwords of technology masks;
- `g_OldUnitRecordCount`.

They contain no pointers. Dwords [5..7] of a primary record are never written by the rebuild and never read by the
merge.

- **Take.** In the end trigger path, right after `OldUnitRuntime_RebuildScenarioReplayTables()` and only for a win
  whose selection has `carryOverMask` set and skip clear, copy the count, records [0..count) dwords [0..4] and the
  secondary table into the progress section as `carry <successor> from <L> sel <s>`. This replaces the earlier
  snapshot for that target (O6: the newest result).
- **Restore.** In `FrontendScenarioSession_LoadOrRequestCampaignBundle`, after its `OldUnitRuntime_ResetPendingTables()`
  (`scenario_load.cpp:449`), when the start level has a valid snapshot:
  - write it back: count, records with dwords [5..7] set to 0, and the secondary table;
  - `InGameNewSession_FinishWorldUnderTickLock` then runs `OldUnitRuntime_MergeMasksAndReplayRecords` as it does in
    the chain.
- **Equality with the chain.** The merge reads only the count, dwords [0..4] and the secondary table, so the started
  level is identical to the chain's. The test CT2 compares the carry-over log lines
  (`DebugCampaign_LogCarryOver`) and the state hash at a fixed tick.

The `oldunit.hex` entry of a savegame made later can differ in the never-read dwords [5..7]. The format is unchanged.

## 5. Campaign graph (what the map shows)

### 5.1 Nodes

- One node per level record whose level file exists. The lookup order is the one
  `FrontendScenarioSession_LoadCheckedLevel` uses: package entry through `Package_FindEntryAcrossMounts`, then a
  loose file.
- Levels that no existing level can reach are dropped too. For the stock data this hides exactly hansolo m0 (id 1,
  file missing) and m19 (id 20, file missing and unreachable), as O4 asks.
- **Mission number** = `levelId - firstLevelId(base) + 1`. This gives hansolo `s01` = 1 ... `s25` = 25 (the file
  name numbers), and 1..n for tutorial, nimm2 and luke. Q3 asks whether tutorial should show `t00..t02` as 0..2.
- **Visible count** (the `/24`) = number of nodes. **Done count** = nodes with a recorded win.

### 5.2 Edges

- An edge `L -> successorLevelIds[s]` exists for every selection `s` that the level's own end triggers reach with
  `movieVariantSelector != 0` (a win), when the successor is another visible node.
- A **carry edge** has `carryOverMask` bit `s` set and skip bit `s` clear.
- The win selections come from the level file's end-trigger table, read with the level header (5.4). This is why
  hansolo m15 shows only `15 -> 16`, and `15 -> 17` (its loss outcome) is not a path, as in the mockup.
- When a level's triggers cannot be read, the edges fall back to all selections 1..7 whose successor is another node
  (one log line).

### 5.3 Layout (matches mockup v4)

**Columns and lanes**

- Column = longest path from the first node over the edges (topological order; the graph is acyclic once
  self-edges are removed, and a cycle is broken at the back edge with a log line).
- A node whose only successor is a merge node is pushed right to the latest column before that merge. This
  as-late-as-possible rule puts hansolo m17 above m16, as in the mockup.
- Nodes in the same column share it in lanes, centred on the row line (`LANE` 19 px apart). Their order is the
  selection order of the edges that lead there.

**Rows**

- When the columns do not fit the frame width, the chain wraps into a second row. The wrap edge runs right, down
  along the row gap, then left to the next row's first node (mockup `edge_points` with `WRAP`).
- Hansolo: row 1 = 1-4, 5/6, 7-11; row 2 = 12, 13/14, 15, 17/16, 18, 20-25.
- Fewer columns (nimm2, luke, tutorial): one row, centred vertically, spacing `DX = min(86, frame / columns)`.

**Measures** (mockup v4, logical pixels at 1280x720):

| Measure | Value |
|---|---|
| node | 28x16 |
| column step | 43 |
| first column | frame left + 28 |
| row lines | frame top + 40 and + 120 |
| lane | ±19 |
| wrap | 7 |

The layout is a pure function `CampaignMap_Layout(const CampaignGraph &, frameWidth, frameHeight)`. Its own
self-test (`campaignmap`) pins the node positions of the four stock graphs to the mockup.

### 5.4 Titles and descriptions

| Text | Text id | Source |
|---|---|---|
| title | `TEXT_ID_LEVEL_TITLE_BASE` (0x2230) + `titleTextIndex` | `level.str` |
| description | `TEXT_ID_LEVEL_DESCRIPTION_BASE` (0x230010) + `TEXT_ID_LEVEL_DESCRIPTION_STRIDE` (0x10) x `titleTextIndex` | `inhalt.str` |

- Both are resolved with `TextResource_Resolve`, as the single-game tab does
  (`ScenarioCatalog_SelectLevelAndShowDescription`) and `mission_briefing.cpp` does with
  `header.titleTextResourceIndex`.
- `titleTextIndex` is the LEV dword at +0x170. The leading blank of the `level.str` titles is trimmed for the
  `Mission N: <title>` line.
- **Header reads.** The map needs `titleTextIndex` and the end-trigger table of every level. They are read once per
  campaign when the campaign is first shown, and cached in native memory (`CampaignLevelInfo { titleTextIndex,
  winSelectionMask, fileExists }`).
- **No arena.** The entry is looked up with `Package_FindEntryAcrossMounts` and decoded with
  `Package_DecodeEntryInto` into a `std::vector`. CM2 checked this path (see the next block): it does not touch the
  arena for LEV entries.

**Measured (CM2, 2026-10-10; code trace plus a directory scan of the stock PCKs).**

- **Lookup.** `Package_FindEntryAcrossMounts` (`src/assets/package/runtime.cpp:680`) scans the in-memory mount
  slots and has no allocation and no file I/O.
  - Original quirk: it lowercases the caller's path buffer in place (`runtime.cpp:716-718`), so pass a local copy.
  - The path is `level\<levelFileName>.lev`, built as `FrontendScenarioSession_LoadOrRequestCampaignBundle` builds
    it: `WidePath_CombineDirectoryAndLeaf` with `g_ScenarioLevelDirectoryUtf16` (`scenario_load.cpp:19`), then
    `WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_LEV, ...)` (`scenario_load.cpp:358-362`).
- **Decode.** `Package_DecodeEntryInto` (`runtime.cpp:535`) does three things:
  - it seeks and reads the packed bytes with `g_FileSystemSeek` / `g_FileSystemReadExact` (`runtime.cpp:562-564`).
    These are `Win32File_Seek` / `Win32File_ReadExact` (`src/platform/filesystem/win32.cpp:345`, `:540`), which
    are plain `SetFilePointer` / `ReadFile` calls;
  - the read goes into `g_PackageScratchBuffer` (8 MB, allocated once at startup with `ArenaHeap_Alloc`,
    `win32.cpp:156`). Reusing it is no arena traffic. It is overwritten, which is safe because no frontend caller
    keeps data in it between handlers;
  - it decodes through `g_PckDecoderTable` (`src/assets/package/codec.cpp:795`):
    - method 0 `PckCodec_DecodeHuffmanRle` (`codec.cpp:698`) works in the static workspaces
      `g_PckHuffmanSymbolWorkspace256` / `g_PckHuffmanNodeWorkspace` (`codec.cpp:19`, `:23`) and is bounded by
      the output size;
    - method 1 `PckCodec_DecodeStored` (`codec.cpp:275`) copies dwords;
    - only method 2 `PckCodec_DecodeFieldGrid` calls `g_MemoryApi.alloc/free` (`codec.cpp:214-238`).
  - **Result: no `g_MemoryApi` call for methods 0 and 1.**
- **Stock data.** `LEVEL.PCK` has 55 `.lev` entries, all method 0 (decoded at most 21 536 bytes, packed at most
  8 128). The 54 `.fld` entries are method 2, the 13 `.cgn` entries method 0. `PATCH00.PCK` has no level entries.
  - CM6 rejects an entry whose method is not 0 or 1 (one log line). That also keeps an add-on `.lev` stored as
    method 2 away from the arena.
- **CM6 caller duties.** These are the guards that `Package_LoadEntryIntoBuffer` (`runtime.cpp:77`) runs before it
  calls the decoder:
  - `packedSize <= PACKAGE_SCRATCH_BUFFER_BYTES`;
  - destination `!= g_PackageScratchBuffer`;
  - a vector of `(unpackedSize + 3) & ~3` bytes. The stored decoder writes whole dwords; the Huffman decoder writes
    exactly `unpackedSize`.
- **Loose files.** `Package_LoadEntryWithSize` falls back to a loose `level\*.lev` next to the game when no
  package has the entry (`runtime.cpp:86ff`); the test levels in `level\` are loose files.
  - CM6 does the same with the native whole-file reader of CM4.
  - A level found in neither place gives `fileExists = false`.
- **Header checks** (mirror the loaders):
  - `levelAsset[0] == ASSET_MAGIC_LEV` and `levelAsset[3] == PCK_CONVERTER_LEV_00070001` (`src/gameplay/session/level.cpp:266`);
  - decoded size `>= sizeof(InGameLevelConditionStorage)` (0x800, `src/core/layout_checks.cpp:1742`);
  - `resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset >= 0x800` and `<=
    header.common.allocationSizeBytes <= decoded size`. These are the checks of `NewLevel_ValidateImage`
    (`level_new.cpp:111-117`) and `FrontendLevelAsset_LoadedImageFits` (`scenario_load.cpp:40`).
- **`titleTextIndex`** is the dword at LEV +0x170: `LevelAssetHeader.titleTextResourceIndex`,
  `include/thandor/gameplay/session/types.h:130`. The offset is `GeneratedAssetCommonPrefix` 0xB0
  (`layout_checks.cpp:161`) + path offsets 0x28 + placement count 4 + resource tables 0x24 + file name 0x70.
  `LevelPackage_ValidateAndMount` reads the same dword as `levelAsset[92]` (`level.cpp:268`).
- **End-trigger table.** The level loader copies the first `runtimePrefixByteSizeAndInitialArmyPlacementOffset`
  bytes of the decoded LEV image unchanged into `InGameLevelConditionStorage` (`NewLevel_CopyRuntimePrefix`,
  `level_new.cpp:144-172`). `level_script.cpp` then reads `conditionStorage->schedule` (`level_script.cpp:400`).
  So the file offsets equal the struct offsets (`types.h:410-419`):

  | Item | LEV offset | Layout |
  |---|---|---|
  | `LevelAssetRuntimePrefix` | 0x000 | 0x370 bytes (`layout_checks.cpp:429`) |
  | reserved | 0x370 | 0x10 bytes |
  | `schedule.conditions[64]` | 0x380 | 0x10 bytes each |
  | `schedule.triggers[16]` | **0x780** | 8 bytes each (`InGameEndConditionTriggerRecord8`, `types.h:371`): +0 `stateFlags` (0 none, 1 active), +1 `movieVariantSelector`, +2 `skipArmyDisableWhenOne`, +4 `factionRuntimeIndex`, +5 `endMovieSelectionIndex`, +6 `conditionIndex` |
  | end of `InGameConditionSchedule` | 0x800 | `sizeof(InGameConditionSchedule)` 0x480 (`layout_checks.cpp:851`) |

  - CM6 can run `InGameLevelScript_Validate` (`level_script.cpp:304`; const, no arena) on
    `reinterpret_cast<const InGameConditionSchedule *>(bytes + 0x380)` to reject a level the game would reject.
  - The win-selection mask is the OR of `1 << endMovieSelectionIndex` over the triggers with `stateFlags` active
    and `movieVariantSelector == 1` (rule of 4.3; CM1 confirms it per level).
- **Cost (guess).** 24 hansolo levels of 30-200 KB each take under 100 ms on first open. They are not re-read later.

### 5.5 Status line texts (German project texts, `TEXT_ID_PROJECT_BASE` range, `src/assets/text/resources.cpp`)

| State | Line |
|---|---|
| done | `Abgeschlossen - erneut spielbar.` + carry target: ` Übernimmt Einheiten aus Mission N (<title>).` |
| unlocked | `Freigeschaltet - folgt auf Mission N (<title>)`; a branch: `Freigeschaltet - alternativer Zweig nach Mission N (<title>)`; the first mission: `Freigeschaltet` |
| locked | `Gesperrt - folgt auf Mission N (<title>)` |
| carry target without a valid snapshot | `Benötigt die Einheiten aus Mission N (<title>) - Mission N gewinnen.` (Start disabled) |
| network game, carry target | `Im Netzwerkspiel nur über Mission N erreichbar.` (Start disabled, 7.4) |

The button row labels are `<short name> <done>/<visible>`. A finished campaign (`done == visible`) gets a check
mark after the count (Q11). The game font has no check glyph, so the control draws it as a small two-stroke
line sprite in the label colour (selected colour when the button is selected), right of the text; the label is cut
so the mark always fits. Short names come from a project-text table keyed by the
`.cgn` base name (`tutorial` Tutorial, `hansolo` Die Invasion, `nimm2` Kyrrdon, `luke` Piraten). Any other campaign
uses its full title (`TEXT_ID_CAMPAIGN_TITLE_BASE` + index), cut to the button width. Other languages: Q8.

### 5.6 Start-point copies (O8): hide them

**Decision: hide them.** The campaign buttons group catalog records by the campaign fingerprint (4.4), which ignores
the header and therefore `firstLevelId`. Records with the same fingerprint are one campaign.

The button stands for the record with the lowest `firstLevelId`. If several have it, the lowest title index wins, so
it is `hansolo` (first 2) and not `hansolo4`. Its progress is shared.

The copies are not listed in the stock `campagne.dat`, so this changes nothing with stock data. It only matters
when an add-on lists them. A save made from a copy is not affected: it carries its own `campagne.hex`.

The hidden `campaignsList` keeps every record as a row, so the test aid `OPEN_THANDOR_CAMPAIGN=<name>` still finds
copies by name.

## 6. UI design mapped to the real controls

### 6.1 The page today (from `src/ui/frontend/ui_template.cpp:723-912`, `include/thandor/ui/frontend/types.h:1103-1125`)

All offsets are pixel offsets from the parent centre (anchors 0x40000000), given as L,T,R,B. At 1280x720 the
panel's centre is (640,360).

| Member | Type | Offsets | Use on the new campaign tab |
|---|---|---|---|
| `gameSelectPage` +1C38 | `UiImagePanelControl` (menue.gfx #8) | full | unchanged |
| `gameSelectTitleLabel` +1E74 | `UiFocusProxyControl` | -288,-164,288,-140 | unchanged (`Spiel auswählen`) |
| `loadGameTabButton`, `singleGameTabButton`, `campaignsTabButton` | `UiFramedTextButtonControl` | -304,-112..-24 | unchanged |
| `gameSelectCancelButton` / `gameSelectStartButton` | `UiFramedTextButtonControl` | -256,128,-144,152 / 144,128,256,152 | unchanged; Start disabled per 5.5 |
| `gameSelectTabStack` +1ED0 | `UiLayoutContainerControl<3>` | full | page 2 (campaigns) |
| `campaignsScroller` +22D4 / `campaignsList` +2364 | `UiScrollableControl` / `UiListControl` (3 columns) | -176,-112,304,64 | **kept and filled as today, but suppressed** (`UI_NODE_SUPPRESSED`) while the map is shown; its rows stay the source of the row index for Start and for the test aids |
| `campaignsLabel` +23EC (`Kampagnen:`) | text | -160,-128,288,-112 | suppressed (the button row takes its place) |
| `campaignDescriptionText` +2448 | `UiWrappedTextControl` | -160,72,288,112 | suppressed (replaced by the three text nodes below) |

The mockup v4 numbers (`compose.py page_v3`: panel at 318,178 = centre 640,360) convert to these offsets:

| New node | Mockup pixels | Template offsets L,T,R,B |
|---|---|---|
| campaign button row (up to 6 buttons, gap 6) | x 464..943, y 223..242 | -176,-137,304,-117, split evenly by label width |
| map frame | 464,248..943,408 | -176,-112,304,49 |
| `Mission N: <title>` line | x 468, y 420 | -172,60,128,74 |
| campaign title, right-aligned | right edge 939, y 420 | 0,60,300,74 |
| description | y 435 | -172,75,300,89 |
| status line | y 450 | -172,90,300,104 |

### 6.2 New template nodes (appended to `FrontendUiImage`, like the "Erweitert" page)

The "Erweitert" page was appended in step 9/10 at the end of `FrontendUiImage` (`types.h:1282-1304`, +71A0..+788C)
and of the template (`ui_template.cpp:2343-2551`), with `UI_TEMPLATE_LINK(offset)` and asserts in
`layout_checks.cpp`. The new nodes are added the same way, as children of the campaign page of
`gameSelectTabStack`:

- **`campaignButtons[6]`: `UiFramedTextButtonControl`**, as radio buttons (the stateFlags of the tab buttons:
  persistent select).
  - Labels are built at runtime into a native UTF-16 buffer per button. The text pointer points there, the way the
    list rows point at catalog text.
  - Unused buttons are suppressed. Selected = pressed frame (the button's own selected frame, win.gfx 0x4A + 8),
    exactly as in the mockup.
  - Six covers the four stock campaigns plus two add-ons. With more distinct campaigns the tab falls back to the
    old list (Q6).
- **`campaignMap`: a new control type `FrontendCampaignMapControl`** (`UiNodeBase` + a small fields block) with its
  own vtable `g_FrontendCampaignMapControlVtable`:
  - `drawClipped`: frame, paths, nodes, legend;
  - `nonRightPress`: hit test plus select;
  - press twice within the double-click time: start;
  - `pointerMove`: no hover effect in v1;
  - `hitTest`;
  - the other slots: those of the image panel.

  The graph and the layout live in native memory. The template node holds one dword index into a module-level
  table, not a pointer: no pointer in a template image. Why one control instead of 26 button nodes: hansolo needs
  24 nodes plus paths and wrap lines that live between the nodes, and a fixed pool of button nodes would cap the
  mission count (guess: a cap of 32 would fit every stock campaign but not every add-on).
- **`campaignMissionTitleText`, `campaignTitleText` (right-aligned), `campaignMissionDescriptionText`,
  `campaignMissionStatusText`**: `UiSingleLineTextControl` / `UiWrappedTextControl`, styled like
  `campaignDescriptionText`. The mission title uses the selected style (brighter, `ACTIVE` 222 in the mockup).

The `uitemplate` self-test (step 13 W0.4) hashes the template images. CM10 changes them on purpose and updates its
reference in the same package.

### 6.3 Drawing the map with the game's graphics

- **Node frame.**
  - Done and unlocked nodes use the framed-button frame of win.gfx (`UI_WINDOW_SUBRESOURCE_BUTTON_FRAME` 0x4A, +8 for
    pressed). Locked nodes use the disabled frame 0x8C with `g_UiTextStyleDisabled`.
  - The frame drawing is today inside `UiFramedTextButtonControl_DrawClipped`
    (`src/ui/controls/text_buttons.cpp:156-349`). CM11 moves it into `UiFramedFrame_Draw(rect, frameBase, clip)`,
    which the button keeps calling, so the pixels stay identical (golden).
- **Done fill.** `g_GraphicsFramebufferFillRectArgb` inside the frame with ARGB `0xCC466846` (green 70,104,70 at
  80 %, mockup `GREEN`/`GREEN_A`).
- **Label.** The number is centred with `RichTextCommandStream_DrawSingleLine`:
  - `g_UiTextStyleSelected` for done and selected nodes;
  - `g_UiTextStyleNormal` for unlocked nodes;
  - `g_UiTextStyleDisabled` for locked nodes.
- **Paths.**
  - There is no line primitive in the engine. Paths are axis-aligned only (horizontal, vertical, the elbow of a
    branch, the wrap), so they are 1-px `g_GraphicsFramebufferFillRectArgb` rectangles.
  - Colour: light grey 174 at 55 % when lit (source done and target not locked), 16 % otherwise, with no shadow.
    These are the calm v3/v4 values.
  - A carry edge is two parallel 1-px lines, at -1 and +1 px.
- **Legend.** Only when a carry edge exists: a short double line plus `Einheitenübernahme` at the frame's
  bottom-right (mockup: text at frame right - 2, frame bottom - 17, alpha 0.75).
- **Sunken frame.** The frame is drawn the way the list's scroller frame is drawn today (the same frame pieces as
  `campaignsScroller`), so it looks like the old list box.

### 6.4 Scale and renderers

The frontend lays out at the logical framebuffer size (`g_FramebufferWidth/Height`). It is centred on the panel, and
controls sit within about ±304 x ±164 of the centre, so the page fits 640x480.

- **GPU renderers.** The logical size is the mode divided by the UI scale N (`UiScaleFor`,
  `src/platform/sdl3/video.cpp`). All 2D items, fills included (`RecordFillRectArgb` -> `DRAW2D_OP_FILL`), are
  recorded in logical pixels and scaled by N at vertex time (`gpu_renderer.cpp` `s_uiScale`). The map, its 1-px
  paths and the double lines therefore scale exactly like the frames of the existing buttons: N physical pixels per
  logical pixel. No code is needed for scale.
- **Software renderer.** N = 1, the same calls.
- **Compact layout** (width <= 640). The panel is the same and the menu room is hidden; nothing changes.
- **Verification.** CM15 adds a pixel golden of the campaign tab with the software renderer and a `gpu_compare`
  scenario for Vulkan and D3D12 at UI scale 1 and 2.

## 7. Flow changes

### 7.1 Opening the campaign tab

`FrontendScenarioPage_OpenCampaignRecordsAndRefresh` and `ScenarioCatalog_RebuildCampaignRecordListPage`
(`src/ui/frontend/scenario_selection.cpp`) stay. They still fill and sort `campaignsList`. Then the new
`FrontendCampaignTab_Refresh` runs:

1. load the progress (first time only);
2. group the rows into campaigns (5.6);
3. build the graph and level info of the selected campaign (cached);
4. fill the buttons, the map and the texts;
5. suppress the list, its label and the old description.

**Selected campaign.**
- Default: the list's selected row, which is row 0 after the sort, i.e. the tutorial.
- Later: the last one used in this process (a module variable, not saved).

**Selected mission.** The first unlocked mission that is not done, otherwise the last done one.

### 7.2 Selecting

- **Clicking a campaign button** selects that campaign's base row in `campaignsList`
  (`UiPointerList_SelectColumnListIndex`) and issues `ScenarioCatalog_SelectCampaignAndShowDescription(row)` as the
  list does today (`FrontendCommand_Issue`, mirrored on network peers). The map is then rebuilt.
- **Clicking a mission node** selects it and updates the texts and the Start button state:
  - Start is enabled for done or unlocked missions;
  - Start is disabled for locked missions and for carry targets without a snapshot (5.5).
- **A second click within the double-click time** on a startable node starts it, like a list double-click
  (`FrontendScenarioSelection_SelectOrStartCampaign`).

### 7.3 Starting a mission from the map (local game)

`FrontendScenarioSelection_ActivateSelectedRecord`, tab 2, passes the selected mission's level id in the first
argument dword of the existing command:

```
FrontendCommand_Issue<FrontendScenarioSession_LoadOrRequestCampaignBundle>(startLevelId, 0, selectedRowIndex);
```

In `FrontendScenarioSession_LoadOrRequestCampaignBundle` (host or local branch), after `CampaignAsset_Fits`:

- `startLevelId == 0` means today's behaviour.
- Otherwise the id is checked (`CampaignStart_Prepare`, CM8): it must be a record of this campaign with a valid
  level file and done or unlocked in the local progress (the section whose fingerprint matches the loaded
  campaign, preferring the one named after the list row). A carry target needs a valid snapshot in a local game
  and is refused in a network game (7.4).
  - Valid: set `campaignAsset->firstLevelId = startLevelId` in the **loaded copy** (never in the file).
  - Invalid: one log line and `firstLevelId` stays.
- Then, as today: `currentLevelId = firstLevelId`, find the record, load the LEV and FLD, and build the bundle
  (host).
- After `OldUnitRuntime_ResetPendingTables()`, restore the snapshot for `startLevelId` if it has one (4.5).

This is exactly what the test aid `DebugCampaign_SelectCampaignLevel` (`src/platform/debug/campaign.cpp`) has done
since step 8 for the campaign check. The side effect is known: the briefing shows the opponent settings when
`currentLevelId == firstLevelId` (`mission_briefing.cpp:181`), so a mission started from the map offers them as the
first mission does (Q7). The `campagne.hex` entry of a later save holds the patched `firstLevelId`. It is valid
(`CampaignAsset_Fits` finds its record) and the original reads it only for the same briefing rule.

### 7.4 Network games

- **The command.** The host issues the command with `startLevelId` in an argument dword that every peer receives and
  ignores today (the client branch only marks the transfer state). The original game also ignores it.
- **The client.** It takes the level from the bundle, `scenario_transfer.cpp:368`
  `currentLevelId = firstLevelId`, so an original client and our client start the same mission with no protocol
  change.
- **Carry-over cannot be restored in a network game.** Every peer resets its old-unit tables at campaign start
  (`OldUnitRuntime_ResetPendingTables` runs on clients too), and a host-only snapshot would desynchronise the
  peers.
- **So in network games:**
  - carry targets cannot be started from the map (status line, Start disabled; Q4);
  - every other unlocked mission can;
  - the chain (win, then next level) carries units as today, because every peer rebuilds its tables from its own
    simulation.
- **Progress.** Every peer records its own wins in its own file (4.3). The host's file decides what the host can
  start.
- **Client view.** Clients show the host's selected campaign (the mirrored select command) with their own progress.
  The mission the host selects is shown via the select command's unused first argument: the level id; an original
  peer ignores it (CM13). Clients cannot start (the Start button is already suppressed for clients).

### 7.5 After a session

`FrontendMainLoop_ContinueAfterSession` and `FrontendMainLoop_SelectCampaignSuccessorLevel` are **unchanged**. The
recording sits in `InGameRuntime_RunSessionUntilExit` (`startup.cpp`), in the END_MOVIE_PENDING branch, after
`OldUnitRuntime_RebuildScenarioReplayTables()` and before `InGameRuntime_ShutdownAndReleaseResources()`:

```
CampaignProgress_RecordSessionEnd(g_FrontendLoadedCampaignAsset, g_EndMovieSelectionIndex, g_EndMovieVariantIndex);
```

It reads only those values and the old-unit tables, and writes the file natively. A session start in a campaign
(`InGameNewSession_FinishWorldUnderTickLock`, after the merge, and the loaded-session path) calls
`CampaignProgress_NoteLevelStarted(campaign)` (unlock rule 3). The SESSION_CLOSED (restart) and LOCAL_PLAYER_LEFT
branches record nothing.

### 7.6 Retry, branches, replay

- **Retry after a loss.** As today: `successorLevelIds[0]` is the mission itself, and the skip bit keeps the incoming
  units (4.2). No progress change.
- **Branch.** A win of hansolo m4 with selection 1 records `done 5 1` and unlocks m5. A later replay won with
  selection 2 adds `done 5 2` and unlocks m6. Both stay unlocked.
- **Replay keeps progress.** Recording only adds `done`/`unlocked` items, and a snapshot is replaced only for the
  successor of the replayed win. Nothing is ever removed.
- **The chain after a map start** continues as the original does, from the started mission on.
- **Original quirks kept:**
  - m15 loss -> m17, m25 loss -> m24 (they still mark m17/m24 unlocked by rule 3);
  - the t00 and t01 restart leaves the campaign (4.2, measured by CM1). With the map the player can start the mission again from the menu.

## 8. Compatibility and determinism

| Area | Effect | How it is ensured |
|---|---|---|
| Simulation | none | no simulation code changes; the snapshot restore writes the same values the chain writes (4.5, test CT2); determinism and AI hash checks |
| Arena | none | only native memory and native file calls in the new code; the LEV decode path has no arena calls for methods 0/1 (CM2, 5.4); the determinism check and the campaign chain stay identical |
| Savegame | format unchanged | `campagne.hex` may hold a patched `firstLevelId` (valid); `oldunit.hex` dwords [5..7] may be 0; old saves load (saveload check plus a stock save) |
| Network | no new command or packet | start level in an unused argument dword; bundle unchanged; no carry-over restore in network games (7.4); multiplayer check plus a manual host test with the original client if available |
| Original campaign play | unchanged | continue-after-win, retry and restart code untouched; the campaign check (quick and `--full-campaign`) |
| Test aids | kept | `OPEN_THANDOR_CAMPAIGN` / `_CAMPAIGN_LEVEL` use the hidden list rows and `firstLevelId` as today |
| `-KARTE=` / `-HOST` / `-CLIENT=` | unchanged | they switch to tab 1 or the network flow before the campaign tab |
| Settings | none | the progress file is separate; `thandor.ini` is not touched |
| Installer | uninstall must keep `save\campaign_progress.txt` like the saves | CM16 checks `tools/installer` |

## 9. Work packages

Sizes: **S** about 15 min, **M** about 25-30 min. "Verify" lists checks beyond the standard (both compilers, 0 new
warnings, self-tests, golden). Within a wave, no two packages touch the same file.

### Wave A: measurements and helpers (parallel)

| Id | Title | Files | Change | Size | Verify |
|---|---|---|---|---|---|
| CM1 | Campaign dump tool | `tools/data/cgn.py` (new) | Dumps `.cgn` records (successors, masks, exit zones) and, with `--triggers`, the end triggers of each level through `lev.py`. Measures tutorial and luke: is selection 0 always the variant-0 loss? Result into this plan (4.1). | S | run on `LEVEL.PCK` (extract with `tools/data/pck.py`) |
| CM2 | Measure the native LEV header read | scratch test only, no commit, or a self-test stub | Checks whether `Package_FindEntryAcrossMounts` + `Package_DecodeEntryInto` into a `std::vector` calls `g_MemoryApi` (breakpoint/log counter); finds the end-trigger table offset in the LEV the way `level_script.cpp` loads it; finds the army asset id bound for 4.4. **Done 2026-10-10:** no arena for methods 0/1 (all stock LEVs are method 0), triggers at LEV +0x780, title at +0x170, no numeric army id bound (5.4 "Measured", 4.4 "Army asset ids") | S | report in the package notes |
| CM4 | Native file helpers | `src/platform/filesystem/win32.cpp`, its header | `FileSystem_ReadWholeFileNative(path) -> std::optional<std::vector<uint8_t>>` (size limit 1 MB) and `FileSystem_WriteFileReplacing(path, bytes)` (tmp + `Win32File_Replace`, delete tmp on failure). No arena. | S | `hexscan`-style self-test on a temp file |
| CM11 | Frame drawing helper | `src/ui/controls/text_buttons.cpp`, `include/thandor/ui/controls/text_buttons.h` | Move the frame part of `UiFramedTextButtonControl_DrawClipped` into `UiFramedFrame_Draw(...)`; the button calls it | S | golden identical, `gpu_compare` identical |
| CM14 | Project texts | `src/assets/text/resources.cpp`, `include/thandor/assets/text/resources.h` | Status texts, legend text, short campaign names (5.5) as project texts | S | `uiatlas` self-test unchanged |

### Wave B: model (after A)

| Id | Title | Files | Change | Size | Verify |
|---|---|---|---|---|---|
| CM3 | Progress model and file format | `include/thandor/gameplay/session/campaign_progress.h`, `src/gameplay/session/campaign_progress.cpp` (new), CMake | `CampaignProgress` (4.4): parse, validate, format, `.bad` rename, version 1; no game hooks yet | M | new self-test `campaignprogress`: round trip, truncated file, wrong version, garbage lines, count > 0x200, bad asset id, duplicate sections, unknown keys |
| CM5 | Campaign graph and layout | `include/thandor/ui/frontend/campaign_map.h`, `src/ui/frontend/campaign_graph.cpp` (new) | `CampaignGraph_Build(asset, levelInfo)` (5.1, 5.2), fingerprint, base-campaign grouping (5.6), `CampaignMap_Layout` (5.3) | M | new self-test `campaignmap`: the four stock graphs (built from embedded record tables, no game data) give the mockup v4 positions and edges |
| CM6 | Level info reader | `src/ui/frontend/campaign_level_info.cpp` (new) | `CampaignLevelInfo_Read(campaign)`: existence, `titleTextIndex`, win-selection mask, native cache (5.4), per CM2 | S | log dump on stock data equals the CM1 tool output |

### Wave C: flow hooks and template (after B)

| Id | Title | Files | Change | Size | Verify |
|---|---|---|---|---|---|
| CM7 | Record wins and starts | `src/gameplay/session/startup.cpp`, `new_session.cpp`, `loaded_session.cpp` (one call each) | `CampaignProgress_RecordSessionEnd` in the END_MOVIE branch (4.3, 4.5 take); `CampaignProgress_NoteLevelStarted` | S | campaign check `quick`: the file holds the expected `done` and `carry` items; determinism unchanged |
| CM8 | Start level and snapshot restore | `src/gameplay/session/scenario_load.cpp` | `startLevelId` argument, validation, `firstLevelId` patch, restore after `ResetPendingTables`, network rule (7.3, 7.4). **Done 2026-10-10:** logic in `campaign_start.cpp`/`.h` (new; `CampaignStart_Prepare`, `CampaignStart_RestoreSnapshot`, plus the pure queries CM13 needs); a carry target is also rejected in a local game when the progress has no valid snapshot for it (the UI disables Start then, 5.5); unit positions are checked against the loaded field grid at restore, a bad one drops the whole snapshot | M | CT2, CT4 |
| CM9 | Test aids | `src/platform/debug/campaign.cpp`, `include/thandor/platform/debug/campaign.h`, `docs/BUILDING.md` | `OPEN_THANDOR_CAMPAIGN_PROGRESS=<path>`; `OPEN_THANDOR_CAMPAIGN_MISSION=<levelId>` (starts through the new CM8 path, not the `firstLevelId` aid); `OPEN_THANDOR_AUTOWIN_SELECTION=<n>` (prefer this end selection); `OPEN_THANDOR_AUTOLOSE=<s>` (fire the local loss trigger); `OPEN_THANDOR_CAMPAIGN_PROGRESS_DUMP=1` (log the parsed progress) | M | used by CT1-CT7 |
| CM10 | Template nodes | `include/thandor/ui/frontend/types.h`, `src/ui/frontend/ui_template.cpp`, `src/core/layout_checks.cpp`, `src/platform/selftest/uitemplate_selftest.cpp` (reference) | Append `campaignButtons[6]`, `campaignMap`, the four text nodes (6.1, 6.2) as children of the campaign page; asserts; nodes suppressed by default (no visible change yet) | M | `uitemplate` new reference; golden of all other pages identical |

### Wave D: UI (after C)

| Id | Title | Files | Change | Size | Verify |
|---|---|---|---|---|---|
| CM12 | Map control | `src/ui/frontend/campaign_map_control.cpp` (new), vtable registration | Draw (6.3), hit test, select, double-click start; the check mark of a finished campaign on its button (5.5, Q11) | M | golden of the new page (CT8) |
| CM13 | Campaign tab logic | `src/ui/frontend/scenario_selection.cpp`, `src/ui/frontend/page_actions.cpp` (button action slots), `src/ui/frontend/lifecycle.cpp` (bind textures/sounds of the new nodes) | `FrontendCampaignTab_Refresh`, button handler, texts, Start state, start-level argument, client view, fallback to the list (> 6 campaigns) | M | CT1-CT8; the `textedit` and `pixels` checks unchanged on the other tabs |

### Wave E: tests and docs (after D)

| Id | Title | Files | Change | Size | Verify |
|---|---|---|---|---|---|
| CM15 | Campaign map test script and check | `tools/test/run_campaign_map.py` (new), `tools/test/run_checks.py` (new check `campaignmap` or a mode of `campaign`), `tools/test/choose_campaign.txt` (new script), `tools/test/run_campaign_chain.py` (per-worker progress file via `OPEN_THANDOR_CAMPAIGN_PROGRESS` so linked worker copies never share `save\`) | CT1-CT8 (section 10) | M | the script passes; all eight run_checks pass |
| CM16 | Docs and notes | `docs/PATCH_NOTES.md`, `README.md` (feature line), `docs/BUILDING.md` (test aids, check), `tools/installer` (keep the file on uninstall if it deletes `save\` content), `tools/docs/gen_docs.py` run | text only | S | docs regenerate |
| CM17 | Final gate (coordinator) | - | full `run_checks.py` (all eight, also `--full-campaign`), `gpu_compare` Vulkan and D3D12, both compilers, old savegames (three stock saves plus one pre-feature save) load, a campaign save made from a map start loads in the build of `dev` before this feature | M | - |

Total: 17 packages (CM1-CM17), about 6 S and 11 M, roughly 6-7 agent-hours plus the gates (guess).

Ordering: CM2 before CM6 and CM3 (asset id bound); CM4 before CM3; CM11 before CM12; CM14 before CM13; CM5/CM6
before CM13; CM3 before CM7/CM8; CM9 before CM15.

## 10. Test plan

Every scripted test uses the dev-tools build (`mingw-test` preset) in a test copy of the game, with
`OPEN_THANDOR_CAMPAIGN_PROGRESS` pointing into the run folder. Results go to `GAME_DIR/campaignmap/results.txt`.

| Id | Test | How | Pass when |
|---|---|---|---|
| CT1 | Campaign chain records progress | empty progress; `OPEN_THANDOR_CAMPAIGN=tutorial`, `OPEN_THANDOR_AUTOWIN=40`, three levels | the file has `done 1 1`, `done 2 1`, `done 3 1` (t02 selection 1 ends the campaign), `carry 2 from 1` and `carry 3 from 2` with the unit counts the chain log reports |
| CT2 | Start a carry target directly | progress from CT1 (and from a hansolo run of m21 -> m22); `OPEN_THANDOR_CAMPAIGN_MISSION=3` (tutorial t02), `=23` (hansolo m22), no auto-win, 2 minutes | `level start: campaign carries over N units` equals CT1's chain value; the per-faction log line equals the chain's; the level does not end at once (it is in `EXPECTED_ENDED` today when started without units); the state hash at tick 600 equals a chain run's at its tick 600 (statehash test aid, fixed seed) |
| CT3 | Corrupt progress files | a set of broken files (truncated, wrong magic, version 2, a `carry` with 600 units, a unit with asset id 0xFFFF, faction 9, coordinates outside the map, duplicate sections, binary garbage, empty, 2 MB) | the game reaches the campaign tab with no crash; each case writes its one log line; a wrong-version file is renamed `.bad`; invalid snapshots disable Start on the target |
| CT4 | Replay keeps progress | progress with hansolo done up to m9; start m4 from the map, auto-win with selection 1 | after the run m5-m9 are still done and their snapshots unchanged (byte compare of the sections) |
| CT5 | Branch unlock | hansolo progress with m4 won by selection 1 (m6 locked); replay m4 with `OPEN_THANDOR_AUTOWIN_SELECTION=2` | `done 5 2` is added and m6 is unlocked; m5 still unlocked |
| CT6 | Retry and restart record nothing | start m8 from the map; `OPEN_THANDOR_AUTOLOSE`; then a script click on "Mission neu starten" | the retry level gets the same carried units (log line); the progress file is byte-identical before and after |
| CT7 | Network campaign | `run_multiplayer.py` with a host script that starts nimm2 mission 3 from the map (new `mp_host_campaign.txt`) and the client join script | both reach the game at mission 3 (level name in both logs); a carry target shows Start disabled on the host |
| CT8 | Pixels | `choose_campaign.txt`: open the page, click Kampagnen, click Die Invasion, select mission 9, screenshot; fixed progress file | software golden; `gpu_compare` Vulkan and D3D12 at UI scale 1 and 2 |
| CT9 | Existing checks | `run_checks.py` all eight; `--full-campaign` once at the end | all pass (the campaign check still finds the campaigns by name through the hidden list) |

Self-tests (per package): `campaignprogress` (CM3), `campaignmap` (CM5), `uitemplate` (CM10, new reference).

## 11. Risks

| Risk | Effect | Mitigation |
|---|---|---|
| The LEV decode or the progress I/O touches the arena | arena order shifts, so the determinism or campaign chain changes | CM2 measured the decode path (no arena for methods 0/1; CM6 rejects method 2); native-only rule; CT2 compares state hashes |
| A snapshot restored from a user-edited file places units at bad positions or with bad asset ids | crash in `ArmyRuntime_CreateInstanceFromAsset` or in the occupancy rebuild | validation in 4.4 (faction and the loaded level's bounds); an unknown asset id is a safe no-op in the engine (CM2, 4.4); CT3 |
| Win rule wrong for a level (variant semantics in coop, tutorial triggers) | a loss marked as a win, or a win not recorded | CM1 measures every stock level; the rule is one function with a self-test table |
| The network argument dword is not transported as assumed | client starts the wrong mission | **Verified (CM8, code trace):** in a network game `FrontendCommand_Issue` queues `(code << 8 \| player, payload1, payload2, payload3)` as one 16-byte `UiCommandQueueRecord` (`FrontendCommandQueue_EnqueueLocalPlayerCommand`, `src/network/protocol/commands.cpp`); the host takes its own record into slot 0 (`FrontendTransfer_BroadcastAndExecuteLobbyCommands`, `src/network/protocol/lobby.cpp`), sends the batch to the clients and executes it; every peer runs `CommandDispatch_ExecuteRecord`, whose validation table has `COMMAND_CHECK_NONE` for payload1 of `FRONTEND_COMMAND_LOAD_CAMPAIGN`, so the handler gets the dword unchanged on every peer. Only the host (or a local game) reads it; a client takes the mission from the bundle, which already holds the patched `firstLevelId` (`scenario_transfer.cpp` `currentLevelId = firstLevelId`), so an original client starts the same mission. CT7 confirms at run time |
| Opponent settings appear on a map-started mission (`currentLevelId == firstLevelId`) | a harder or easier campaign mid-way | owner decision Q7; if unwanted, keep a separate "map start" flag and leave the briefing rule on the original first level |
| Linked worker copies in the chain tests share `save\` | parallel runs race on the progress file | CM15 sets a per-worker progress path |
| Template growth | `uitemplate` reference, layout asserts | appended members only, as the "Erweitert" page did |
| More than 6 campaigns from add-ons | buttons overflow | fallback to the old list view (Q6) |
| Paths look heavy at UI scale 2+ | 1 logical px = N physical px | accepted (same as every frame); optional thin-line mode later |

## 12. Owner decisions on the open questions (answered 2026-10-10)

| # | Question | Decision |
|---|---|---|
| Q1 | Win rule: variant != 0 and the successor is not the mission itself (4.3). | yes |
| Q2 | Unlock missions the original chain reaches without a win (hansolo m15 loss -> m17), and missions of loaded campaign saves (rule 3 of 4.3). | yes |
| Q3 | Mission numbers `levelId - firstLevelId + 1` (hansolo 1-25, tutorial/nimm2/luke 1..n). | yes, 1..n |
| Q4 | Network games: carry targets cannot be started from the map. | yes: Start disabled, with the status line |
| Q5 | Import progress from existing campaign savegames. | no import; rule 3 unlocks a mission as soon as its save is played |
| Q6 | More than 6 campaigns (add-ons). | fall back to the old list |
| Q7 | Opponent settings in the briefing of a map-started mission. | keep |
| Q8 | Other game languages. | full title cut to the button width until texts exist |
| Q9 | Keyboard navigation on the map. | not needed (no keyboard control for the new selection) |
| Q10 | The t00 restart leaves the campaign (original quirk; CM1 found t01 does the same). | keep, for t00 and t01 |
| Q11 | Marker for a finished campaign. | yes: a check mark on the campaign button (5.5, CM12) |
