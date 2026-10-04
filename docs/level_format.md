# Thandor level file format (.lev)

Sources: `src/gameplay/session/level.cpp` (InGameLevelRuntime_LoadResourcesAfterDefaultReset 0x005311D0, the
savegame loader 0x00532020, the editor saver InGameLevelRuntime_SaveLevelAssetImageFromWorldState 0x00532CA0,
LevelAsset_PrepareEndingMoviePath 0x00531080), `src/gameplay/session/runtime.cpp`
(InGameConditionRuntime_UpdateScheduledRecords, InGameRuntime_InitializeNewSession), `src/assets/scenario/catalog.cpp`,
`src/ui/frontend/player.cpp`, `src/world/runtime/core.cpp`; types in `include/thandor/generated/types.h`
(LevelAssetHeader, LevelPlayerSlotRecord, LevelWorldSettings, InGameConditionSchedule,
LevelInitialArmyPlacementRecord20). Verified against all 55 stock levels of LEVEL.PCK (19 skirmish, 25 hansolo,
4 luke, 5 nimm2, 3 tutorial) with `tools/data/lev.py check`: every file parses, every section fits, and every file
is rebuilt byte-identically by `lev.build()`.

Legend: **certain** = read by the loader in a way that fixes the meaning, and consistent in all stock files;
*uncertain* = inferred from data or naming only.

All values little endian. "Path" = offset (from the start of the file) of a UTF-16LE string, NUL terminated, stored
in a 0x40-byte slot (32 code units) in stock files. The loaders overwrite the extension in place
(WidePath_SetExtensionCode), so paths are stored **without** extension.

## Overall layout (stock layout, identical in all 55 files)

| Offset | Size | Content |
|---|---|---|
| 0x000 | 0x100 | header (common asset prefix, path offsets, counts, table offsets) |
| 0x100 | 0x100 | scenario catalog record (byte-identical to the level's record in `level\level.dat`) |
| 0x200 | 7 x 0x20 | player slots for factions 1..7 |
| 0x2E0 | 0x90 | world settings (lighting, faction counts, relations, intro movie, music) |
| 0x370 | 0x10 | reserved, zero in all files (never accessed) |
| 0x380 | 64 x 0x10 | level script: condition records |
| 0x780 | 16 x 8 | level script: end triggers |
| 0x800 | n x 0x40 | MDL path table (18 records; tutorial 16) |
| ... | n x 0x40 | ARM path table (11) |
| ... | n x 0x40 | SHT path table (4) |
| ... | n x 0x40 | EFF path table (10) |
| ... | 10 x 0x40 | path strings: field, ground, surface, sky, army tex, shot tex, effect tex, sound, technology, movie |
| [0x0DC] | count x 0x20 | initial army placements (end of file) |

The loader does not require this order: everything is reached through offsets. It only requires:

* `[0x0DC]` (prefix size) is both the number of bytes copied into the mutable level storage
  (InGameLevelConditionStorage: 0x370 image + 0x10 reserved + schedule at 0x380, so it must be >= 0x800, and a
  multiple of 4 - the copy is dword-wise) **and** the file offset of the placement table.
* the file size = `[0x004]` = `[0x0DC]` + count * 0x20 (the editor saver rebuilds it that way; the frontend packs
  `allocationSize` bytes for network transfer).
* tables and path strings lie inside the prefix (stock: all between 0x800 and `[0x0DC]`).

## Header 0x000..0x0FF

| Off | Size | Type | Meaning | Stock values | |
|---|---|---|---|---|---|
| 0x000 | 4 | magic | `'lev\0'` = 0x0076656C (ASSET_MAGIC_LEV), checked | always | certain |
| 0x004 | 4 | u32 | allocation size = file size | 5312+count*32 | certain |
| 0x008 | 4 | u32 | format version | 1 | certain (value); not checked |
| 0x00C | 4 | u32 | converter version 0x00070001 (PCK_CONVERTER_LEV_00070001), checked by both loaders and the movie helper | always | certain |
| 0x010 | 24 | 3 x (time, date) | build timestamps: time = h<<16 \| m<<8 \| s, date = year<<16 \| month<<8 \| day (e.g. 0x00132F0D 19:47:13, 0x07CF0C13 1999-12-19); the three pairs are equal | | certain (format), not read by the game |
| 0x028 | 8 | | zero (asset anchor, only its address is used) | 0 | certain |
| 0x030 | 64 | UTF-16[32] | producer name | "SATURN" / "PLUTO" | not read |
| 0x070 | 64 | UTF-16[32] | source name | same as producer | not read |
| 0x0B0 | 4 | path | field grid (FLD) path, e.g. `level\Asgard`; the frontend sets the extension to `.fld`, loads the grid (package, then loose file) and **replaces this dword with the FieldGridAsset pointer** before the level loader runs | | certain |
| 0x0B4 | 4 | path | ground texture base `gfx\boden\soil` (TerrainVisualResources_LoadPrimary) | 1 value | certain |
| 0x0B8 | 4 | path | surface texture base: `gfx\texturen\water` / `ice` / `lava` | | certain |
| 0x0BC | 4 | path | sky texture base `gfx\texturen\sky` | 1 value | *uncertain: no reader of this field in src/ (probably unused)* |
| 0x0C0 | 4 | path | army texture base `gfx\mdl\army` or `gfx\mdl\sarmy` (snow) (ArmyRuntime_InitializePoolAndGraphics) | | certain |
| 0x0C4 | 4 | path | shot texture base `gfx\texturen\shot` | | certain |
| 0x0C8 | 4 | path | effect texture base `gfx\texturen\effect` | | certain |
| 0x0CC | 4 | path | loading movie `flm\erde` / `eis` / `lava` / `wüste` (extension set to flm). UTF-16 chars 4+5 also select the end movie `flm\ende000N.flm`: "wü" -> 2, "ei" -> 3, "la" -> 4, else 0 | | certain |
| 0x0D0 | 4 | path | sound pattern `sound\sound??` (extension .sam): every sam file in that directory whose name ends in a number n becomes spatial sound slot n (<256) | 1 value | certain |
| 0x0D4 | 4 | path | technology file `engine\tech` (`engine\technimm4` paradis, `level\tutorial\tech` tutorials), must be a valid 'tec' 0x20000 asset or the load fails | | certain |
| 0x0D8 | 4 | u32 | number of initial army placements | 0..503 | certain |
| 0x0DC | 4 | u32 | prefix size = placement table offset | 0x1540 (tutorial 0x14C0) | certain |
| 0x0E0 | 4+4 | u32,u32 | ARM table: count, offset | 11 | certain |
| 0x0E8 | 4+4 | | MDL table: count, offset | 18 (tut 16) | certain |
| 0x0F0 | 4+4 | | EFF table: count, offset | 10 | certain |
| 0x0F8 | 4+4 | | SHT table: count, offset | 4 | certain |

Path tables: records of 0x40 bytes (a 32-unit UTF-16 path; the loader advances by 32 units). Load order: EFF,
SHT, (cross references), MDL, ARM. Total files limited by INGAME_LOADED_RESOURCE_CAPACITY. Stock lists (identical
in every non-tutorial level):

* MDL: `mdl\aufbau1..3, building1..3, ruinen, unterbau1..3, lbaum, nbaum, busch, farne, kaktus, palmen, stein, rohstoff`
* ARM: `arm\unit, building, ruinen, lbaum, nbaum, busch, farne, kaktus, palmen, stein, rohstoff`
* SHT: `sht\shot1..4`
* EFF: `eff\destroy, flash, ground, hitunit, nograv, particle, rauch, bauen, baum, arbeit`

A generator should copy these lists verbatim (an army asset id in a placement must be registered by one of the
loaded ARM files, and each ARM needs its MDL models, effects and shots).

## Catalog record 0x100..0x1FF

Byte-identical to the level's 0x100-byte record in `level\level.dat` (checked for all 19 skirmish levels).
ScenarioCatalog_Rebuild reads `level\level.dat` plus `level\level00.dat .. level99.dat` (records merged by name);
the Single game list shows these records. The .lev copy itself is only read for +0x170 and +0x190.

| Off | Size | Meaning | Stock | |
|---|---|---|---|---|
| 0x100 | UTF-16[32] | identifier = file base name **with trailing dot**, e.g. `asgard.`; the frontend builds `level\` + identifier + `lev` | | certain (for level.dat); .lev copy not read |
| 0x140 | UTF-16[2] | player count as text: one digit = assignableFactionCount ('1'..'5') | | *uncertain (data match only)* |
| 0x148 | UTF-16[2] | second digit ('2'..'5'), = assignable count in skirmish; often assignable+1 in campaigns | | *uncertain* |
| 0x150 | u32 | list column text TEXT_ID_LEVEL_COLUMN50_BASE (0x220A) + v, v 0..7; matches the landscape (0 earth, 2 desert, 6 ice, 7 lava, 1 tutorial/other) | | certain (text lookup), meaning *uncertain* |
| 0x160 | u32 | column text 0x2200 + v, v 0..4 | | certain lookup, meaning unknown |
| 0x170 | u32 | **title text index**: TEXT_ID_LEVEL_TITLE_BASE (0x2230) + index (list title, in-game session name, briefing). Stock 10..77 | | certain |
| 0x180 | u32 | column text 0x2205 + v, v 0..4 | | certain lookup, meaning unknown |
| 0x190 | i32 | campaign association index (g_InGameLevelCampaignAssociationIndex) | 0 in all files | certain (read), meaning *uncertain* |
| 0x1C0 | UTF-16 | timestamp text "19.12.1999, 19:47" (catalog column) | | not read by the level loader |
| 0x1F0 | 8 | packed date, time copy | | not read |
| rest | | zero | | |

## Player slots 0x200 + 0x20 * (faction - 1), factions 1..7

| Off | Type | Meaning | Stock | |
|---|---|---|---|---|
| +0x00 | Q12 | start camera X (= camera bookmark n X) | e.g. 14.867 | certain |
| +0x04 | Q12 | start camera Y (negative in stock maps, same axis as placement +0x0C) | | certain |
| +0x08 | Q12 | start camera Z (height, ~12..16) | | certain |
| +0x0C | UQ12 | camera distance / magnitude | 4096 (1.0) or 0 for unused slots | certain |
| +0x10 | u32 | heading angle16 (low 16) and pitch angle16 (high 16); pitch 0xD8B0..0xE800 (clamped to -0x3C00..-0x1800) | | certain |
| +0x14 | u32 Q4 | start Xenite (value * 16), e.g. 64000 = 4000 | 0..104000 | certain |
| +0x18 | u32 Q4 | start Tritium | 0..80000 | certain |
| +0x1C | i32 | added to the faction number to form the faction's colorIndex (`records[n].colorIndex = value + n`); 0 in skirmish, 1..6 in campaigns (enemy colour/"AI class") | 0..6 | *meaning uncertain*; editor saver writes the playing faction index into slot 7 here |

The local player's camera comes from its own slot. Unused slots are all zero.

## World settings 0x2E0..0x36F

| Off | Type | Meaning | Stock | |
|---|---|---|---|---|
| 0x2E0 | u32 | light direction A: low16 / high16 angles, passed as (low, high) to WorldRuntime_RecomputeFieldRegionNormalsAndLighting. The type calls it "field region origin", but the editor saver stores `lightAzimuth \| lightElevation << 16` here | e.g. 0xE7D02000 | *name uncertain; treat as light azimuth/elevation* |
| 0x2E4 | ARGB | terrain ramp-step colour | | certain (passed to SetTerrainLightingConfiguration) |
| 0x2E8 | ARGB | terrain base colour | | certain |
| 0x2EC | u32 | lighting cycle ticks; 0 = static, else colours/light interpolate towards the alt block (0x324..) with this period | 0, 3000..10000 | certain |
| 0x2F0 | u32 | light direction B (editor: auxiliary azimuth \| elevation << 16), "field region size" in types.h | 0xD8302000 | *uncertain* |
| 0x2F4 | ARGB | lighting colour 128 | | certain |
| 0x2F8 | ARGB | secondary colour (alpha kept) | 0x80FFFFFF | certain |
| 0x2FC | u32 | reserved | 0 | |
| 0x300..0x30C | 4 x ARGB | lighting colours 130/134/138/13C | | certain |
| 0x310 | u32 | assignableFactionCount: factions 1..n a human may take (frontend faction setup). Must be >= 1 (do-while loops) | 1..5 | certain |
| 0x314 | u32 | activeFactionCount: factions 1..n that exist (the rest are computer-only); placements of inactive factions are skipped | 2..5 | certain |
| 0x318 | u32 | relationUiFlags: 1 freeze allied (>=8) relations, 2 freeze friendly (>=4), 4 freeze all; skirmish 0, campaigns 1 | 0,1,7 | certain |
| 0x31C | u32 | four 8-bit faction masks: every pair inside one mask starts ALLIED (state 8) | 0, 6, 0xC, 0x1C | certain |
| 0x320 | u32 | four 8-bit faction masks: every pair starts FRIENDLY (state 4) | | certain |
| 0x324 | u32 | intro notification movie id; ids n..n+4 are queued at start (tutorial 925/930/935) | 0 | certain |
| 0x328..0x34C | 10 x u32 | alternate values (light A, light B, 8 colours) for the lighting cycle | copies of the primary block | certain |
| 0x350 | 4 x u32 | music sample numbers -> `sound\music%02d.sam` (0 = none) | (1,43,67,73) or 0 | certain |
| 0x360 | 4 x u32 | level effect sample numbers -> `sound\level%02d.sam` | all 0 | certain |

## Level script 0x380..0x7FF (copied, then modified at runtime)

64 condition records of 16 bytes, evaluated every 20 simulation steps (byte 0 = kind, bit 0 = satisfied, rewritten
at runtime; store with bit 0 clear). Operand dwords at +4, +8, +0xC (op0, op1, op2):

| Kind | Name | Operands |
|---|---|---|
| 0 | unused | |
| 2 | faction op0 has no model | |
| 4 | faction op0 has no "command group A" model (units) | |
| 6 | faction op0 has no army of asset op2 | |
| 8 | faction op1 or op0 inactive, or relation(op1 -> op0) >= 8 (allied) | |
| 10 / 12 | Xenite / Tritium of faction op0 >= op1 (Q4) | |
| 14 | Tritium extraction rate of op0 >= op1 | |
| 16 | faction op0 owns >= op1 armies of asset op2 | |
| 18 | occupancy of cells (byte offset op0 in FieldGridCell) >= op1 % | *uncertain* |
| 20 | countdown: op1 -= step ticks, true at <= 0 | |
| 22 | Xenite storage limit of op0 <= 250 | |
| 24 | no model of class op0 outside command group A | |
| 26 | postfix boolean expression in bytes 1..15: 0xFC end, 0xFD NOT, 0xFE AND, 0xFF OR, other byte = push condition[byte] | |

Kinds used by stock files: 2, 4, 6, 8, 14, 16, 20, 26.

16 end triggers of 8 bytes at 0x780: +0 state flags (1 = active; 2 = processed, runtime), +1 movie variant, +2 skip
army disable (1 = go straight to the end movie), +3 0, +4 faction, +5 end movie selection, +6 condition index, +7 0.
The first active trigger whose condition holds ends its faction (units destroyed, local player loses input); when no
two active factions remain non-allied the end movie plays and the session ends.

Standard skirmish script: bytes 0x380..0x7FF are **identical in all 19 skirmish levels** (also the 2- and
3-player ones; conditions on inactive factions are simply true). Conditions 1..6 = kind 8 for the pairs of factions
1..4 (op0 > op1); 21..24 = kind 26 "AND over the three pairs containing faction k" (all others allied or gone =
win); 31..34 = kind 2 (faction k has no model = lose); triggers 2k-2 = {1,0,0,0,k,0,30+k,0} (defeat), 2k-1 =
{1,1,0,0,k,1,20+k,0} (victory). A generator can copy 0x380..0x7FF from any skirmish level (e.g. asgard.lev).
Without any triggers the game never ends (usable for a test arena).

## Initial army placements [0x0DC] + 0x20 * i

| Off | Type | Meaning | |
|---|---|---|---|
| +0x00 | u32 | army asset id (enum PckArmyAssetIdCatalog: UNIT 1..295, BUILDING 300..395, RUINEN 400.., trees 500..752, STEIN 620..658, ROHSTOFF 800..855 resource deposits) | certain |
| +0x04 | u32 | owner faction 0..7; 0 = neutral scenery/resources (always spawned); 1..7 spawned only when that faction is ACTIVE | certain |
| +0x08 | Q12 | world X of the node (editor saver: node worldXQ12); the loader passes it as the `worldYQ12` parameter of ArmyRuntime_CreateInstanceFromAsset (parameter name swap, the round trip is consistent) | certain (round trip) |
| +0x0C | Q12 | world Y of the node (negative in stock maps: X 0..~100, Y ~-100..0) | certain |
| +0x10 | u32 | rotation angle16 (0..0xFFFF; stromschnelle has sign-extended negatives 0xFFFFxxxx) | certain |
| +0x14 | 12 | zero | certain |

Spawned with ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION | ARMY_CREATE_UNLOCK_TECHNOLOGY (placed buildings unlock their
tech). Stock skirmish levels give every player only asset 50 (ARM_0050_UNIT_MDL0103, the construction vehicle) plus
some units/buildings; neutral placements are trees, stones and ROHSTOFF deposits. Player-owned ids seen in stock
files: units 1, 2, 10-16, 31, 50, 61-64, 81, 84, 101, 110-114, 130-138, 150, 171, 180, 187, 250-295; buildings
300-305, 310, 320-323, 330-333, 340, 345, 350-352, 360-366, 370-377, 380-382, 391, 392, 395.

After spawning: a faction owning class-18 models but no structure gets a default build list; relations from
0x31C/0x320 are applied.

## What a playable generated level needs

1. The .lev (this format) in `level\<name>.lev` (package or loose file next to the exe; Package_LoadEntry falls back
   to loose files).
2. A field grid `<fieldpath>.fld` - either a new flat FLD (separate format) or an existing one (point 0x0B0 at e.g.
   `level\Asgard`; placements and cameras must then fit that map's extent, X 0..~100, Y -100..0 for asgard).
3. A catalog record in `level\levelNN.dat` (0x100 bytes, same bytes as the .lev's 0x100..0x1FF, identifier
   `<name>.`) so it appears in Single game; `KARTE="<name>."` on the command line selects it by that name.
   Reuse an existing title index (0x170) - the text must exist (TEXT_ID_LEVEL_TITLE_BASE + index).
4. Two factions: assignable = active = 2; slots 1 and 2 with camera (distance 4096, pitch ~0xE000) and start
   resources (e.g. 4000 << 4 each).
5. Copy the stock path strings (textures, `sound\sound??`, `engine\tech`, movie `flm\erde`) and the stock
   MDL/ARM/SHT/EFF tables; music (1,43,67,73) or zero.
6. Lighting: copy a stock world-settings block (e.g. asgard's), cycle 0.
7. Placements: any mix of unit/building ids for factions 1 and 2 plus optional ROHSTOFF deposits for faction 0.
8. Script: optional; copy the skirmish script block (0x380..0x7FF) for victory/defeat, or leave it all zero.

May be zero/empty: timestamps, producer/source names, 0x028, sky path (still give a string), catalog text columns
(0x140..0x180 other than the title), 0x190, 0x1C0.., unused player slots 3..7, alt lighting block when the cycle is 0,
effect samples, intro movie, relation masks, conditions/triggers, 0x370.., placement padding.

## Open points

* Sky texture path: no reader found in the level loader.
* 0x2E0/0x2F0 (+ alt): called field region origin/size in types.h, but the editor stores light angles; the
  generator should just copy stock values.
* Slot +0x1C semantics (colour vs AI class) and catalog columns 0x140/0x148/0x150/0x160/0x180.
* Exact world coordinate extent per FLD (derive from the FLD grid size; not analysed here).
