# .fld field grid format (terrain of a level)

Tools: `tools/data/pck.py extract ..\ot-run\LEVEL.PCK <dir> .fld` (now decodes compression method 2),
`tools/data/fld.py dump <file.fld>` and `tools/data/fld.py flat <out.fld> <w> <h> [--like stock.fld] [--material N] [--height Q12] [--water Q12]`.

Legend: **[certain]** read from code and confirmed on all 55 stock .fld files; **[likely]** from code, not
exercised in the game; **[uncertain]** guess / not verified.

## 1. How the game gets a .fld

* The .lev names it implicitly: `.lev` header `+0xB0` `levelPathOffset` points at a UTF-16 string inside the
  .lev (`level\map08` for map08.lev). `FrontendScenarioSession_LoadOrRequestFieldGrid` (0x005443B0) and the
  campaign loader (0x00544AC0) set the extension to `.fld` and call `Package_LoadEntry`. The loaded pointer then
  replaces the offset in the .lev header (`levelPathOffsetOrLoadedFieldGrid`, > 0xFFFF = pointer). **[certain]**
* `Package_LoadEntry` (0x0040EE30): first `Package_FindEntryAcrossMounts`; if the path is in a mounted package
  it is decoded with `g_PckDecoderTable[entry->compressionMethod]` (0 Huffman/RLE, 1 stored, 2 field grid).
  **Only if no package has the path** it opens a loose file (exe directory + path, then the path as given),
  allocates its file size and reads it verbatim - **no decoder, no world-coordinate generation**. So a loose
  `level\<name>.fld` must be the fully expanded in-memory image (0x200 header + w*h*0x80 cells, worldX/worldY
  filled in). A loose file with a name that also exists in LEVEL.PCK is shadowed by the package. **[certain]**
* The game's own map editor writes exactly this loose, expanded form: `FieldGrid_SaveAssetImageFromRuntimeState`
  (0x00532B60) writes `allocationSizeBytes` bytes of the live image to `<exe dir>\level\<name>.fld`, after
  resetting runtime-only fields. Strong evidence that loose uncompressed .flds are a supported path. **[certain]**
* In LEVEL.PCK all 55 .fld entries use method 2 (`PckCodec_DecodeFieldGrid` 0x0040AAA0):
  stored = 0x10-byte prefix (dword 0 = compact image size, rest unused) + method-0 Huffman/RLE of the compact
  image = 0x200-byte header + 0x10 bytes per cell (`persistedAux54, terrainHeight, waterSurfaceDelta,
  flagsAndMaterial`). The decoder zeroes each 0x80-byte cell, puts the four dwords back at +0x54/+0x48/+0x4C/+0x50
  and regenerates worldX/worldY. All 55 decode to exactly their entry's unpacked size. **[certain]**
* Network: a host re-encodes the loaded grid with `PckCodec_EncodeFieldGrid` for clients without the level;
  a client with the level locally loads it itself (loose works there too). **[likely]**

## 2. Header (0x200 bytes, `FieldGridAsset`)

| off | size | field | value / meaning |
|---|---|---|---|
| 0x00 | 4 | magic | `"fld\0"` (0x00646C66). Checked by `TerrainVisualResources_LoadPrimary` and `WorldRuntime_AttachFieldGridAsset`. **[certain]** |
| 0x04 | 4 | allocationSizeBytes | = 0x200 + w*h*0x80 = file size (all stock). Used by the host transfer encode and the editor save. **[certain]** |
| 0x08 | 4 | formatVersion | 1 in all stock; not checked. |
| 0x0C | 4 | converterVersion | 0x00060006 (`PCK_CONVERTER_FLD_SHT_00060006`), checked; otherwise FATAL_ERROR_FIELD_ASSET_INVALID. **[certain]** |
| 0x10 | 0x18 | build timestamps | 3 x 8 bytes, not used by the game. |
| 0x28 | 8 | anchor | zero. |
| 0x30 / 0x70 | 0x40 each | producer names | UTF-16 machine names ("PLUTO", "MERKUR", ...), unused. |
| 0xB0 | 4 | fieldFlags | bit i = material i is used; the material texture set `<ground base><letter i>.gfx` (e.g. `gfx\boden\soilb.gfx` for 1) must load, others are optional. Equals the set of used cell materials in all 55 stock maps (the editor save rebuilds it that way). **[certain]** |
| 0xB4 | 4 | runtimeStateFlags | 0 in files; bit 0 = surface dirty (runtime). |
| 0xB8 | 4 | gridWidth | columns. |
| 0xBC | 4 | gridHeight | rows. |
| 0xC0 | 0x40 | reserved | zero except in lavam.fld (garbage); unused by code. |
| 0x100 | 0x100 | sourcePath | UTF-16 path of the editor's source .gfx (`d:\thandor\tobislevel\...gfx`); not read by any code. |

Ground/water texture base paths are **not** in the .fld; they come from the .lev (`groundTextureBasePathOffset`
`gfx\boden\soil`, `surfaceTextureBasePathOffset` `gfx\texturen\water`). All 26 `gfx\boden\soila..z.gfx` exist
in GRAPHIK.PCK, so any material 0..25 has a texture. Shots reference terrain materials
(`ShotDefinitions_ValidateTerrainMaterialReferences`): each material a shot definition names must have a loaded
texture set - optional sets load when present, so with the stock soil set this is satisfied regardless of
fieldFlags **[likely]**.

## 3. Cell (0x80 bytes, `FieldGridCell`), row-major, cells[row * width + column]

| off | field | in file? | at load |
|---|---|---|---|
| 0x00 | surfacePacketIndex | 0 | random (Init) |
| 0x04 | overlayColor | 0 | 0xFFFFFFFF (Init) |
| 0x08 | triangle0NormalAngles (elev<<16 \| azimuth) | 0 (pck) / 0x40000000 straight up (editor save) | recomputed for interior cells at attach (`FieldGrid_RecomputeInteriorTriangleNormalAngles`); border ring keeps the file value |
| 0x0C-0x3F | projected/view points, runtime20_2B | 0 | projection pass |
| 0x40 | worldX (Q12) | **must be set for a loose file**: column*0x901 + row*0x480 | regenerated only by the method-2 decoder |
| 0x44 | worldY (Q12) | **must be set**: row * -1999 | same |
| 0x48 | terrainHeight (Q12) | **file data** | kept |
| 0x4C | waterSurfaceDelta (Q12) | **file data**: water surface - ground; > 0 under water, <= 0 dry | kept |
| 0x50 | flagsAndMaterial | **file data**, see below | variant bits 8-10 randomized, edge bits rebuilt, 0x8000 cleared |
| 0x54 | persistedAux54 | 0 in all stock files | overwritten with an index 0..255 into g_TerrainDirectionRecordTable256 chosen by worldX/worldY low nibbles (a pointer before step 11) |
| 0x58-0x67 | directional light / shaded colors | 0 | lighting pass |
| 0x68 | visibilityLightingIndex (+3 bytes) | 0 | fog of war |
| 0x6C | armyRuntimeSavedOffset | 0 | 0 (Init) |
| 0x70 | occupancyMask (8 faction bytes) | 0 | runtime (fog/presence) |
| 0x78 | triangle1NormalAngles (water surface normal) | 0 | recomputed interior at attach |
| 0x7C | resourceExtractionDescriptor | 0 | runtime |

flagsAndMaterial (0x50): low byte = material 0..25; 0x700 random texture variant (runtime); 0x800 Xenite
resource; 0x1000 Tritium resource; 0x2000 first column, 0x4000 first row, 0x08000000 last column, 0x80000000 last
row (map edge, rebuilt at load by `FieldGrid_InitializeRuntimeCellsAndBoundaryFlags`, but present in the stock
files); 0x8000 editor debug mark (cleared); 0x10000 region-visited (transient); 0x10000000 unresolved;
0x20000000 / 0x40000000 fluid receiver / source excluded (editor water tool). Stock counts over all maps: 0x800
11236, 0x1000 10530, 0x40000000 1801, 0x20000000 750 cells.

## 4. Passability (what "flat and passable" needs)

`GridScratch_RebuildTerrainAndRuntimeClassificationMasks` (0x00533620) derives the terrain classes per cell
only from **waterSurfaceDelta, the two normal elevations and the edge bits** - the material plays no role
**[certain]**. Thresholds (image data 0x536F10..): class 24 (land) when waterSurfaceDelta <= 0; classes 25-27
when the selected normal elevation <= 10500 (straight up = 16384); classes 28-30 when waterSurfaceDelta >= 500
or triangle0 elevation <= 10500; edge cells are GRID_SCRATCH_BLOCKED. A flat, dry cell (delta <= 0, normals
straight up after the attach recompute) gets class 24 only. Water does not flow during play: the relaxation
passes run only from the editor command 0x3200 **[certain]**.

Resources: none when 0x800/0x1000 are clear (AI and harvesting test those bits) **[likely]**.

## 5. Flat ground in stock maps

Most frequent interior (height, delta, material) per map (fld.py analysis): the tutorials t00/t01 are
`h=1664, delta=-1664, material 1` (16 % of the cells, water level 0 everywhere, nothing under water);
kreuzgang `3584/-1920/1`, asgard `2560/-2560/5`, s02 `6016/-6016/0`. Heights in stock maps span about -12672..28614
Q12, so 0x1000 is well inside the normal range. Dry delta magnitude does not matter beyond its sign and the 500
threshold; `delta = -height` (water level 0) copies the tutorial convention.

## 6. Size limits and boundary

* The outermost ring (row 0, row h-1, column 0, column w-1) is border: never walkable (edge bits), height
  samplers fail there, normals/light are not computed. Playable area = (w-2) x (h-2). **[certain]**
* Every stock map is 8k+3 per side: 67, 75, 83, 91, 99, 107, 115, 123, 131, 139 (non-square allowed, e.g.
  139x107, 83x123). One reason found: `SelectionOverlay_DrawGridVertexMarkers` (0x0052F680, editor overlay)
  visits cells 1, 5, ..., 4*(n>>2)+1, which is inside the grid only for n = 4k+2 / 4k+3. Why 8k rather than 4k
  is unknown. **[uncertain]** fld.py requires 4k+3 and warns if not 8k+3.
* No fixed-size table depends on the grid size: scratch/pathing grids (4x4 per cell, 8 bytes each), the
  minimap composite texture (w x h ARGB, 3 planes) and the cell array are all allocated from the dimensions.
  No explicit upper limit in code; stay within the stock range (<= 139 per side) for safety. **[likely]**
  A packed PCK entry must fit PACKAGE_SCRATCH_BUFFER_BYTES; a loose file has no such limit. The host network
  transfer re-encodes into that scratch buffer (a flat map compresses to almost nothing).

## 7. Minimum for a flat test arena (what fld.py flat writes)

Header: magic, allocationSize = file size, formatVersion 1, converter 0x00060006, fieldFlags = 1 << material,
runtimeStateFlags 0, width, height (rest zero; `--like` copies timestamps/names, harmless).
Cells: worldX/worldY on the lattice, terrainHeight H, waterSurfaceDelta -H, flagsAndMaterial = material + edge
bits, normals 0x40000000 (like the editor save), everything else 0.

Still needed outside the .fld (not covered here): a `.lev` whose levelPath string names the new .fld (e.g. a
copy of a stock .lev with the string at `[+0xB0]` changed to `level\arena`) and whose unit/start positions lie
inside the new grid; the level must also be listed/selectable (level.dat or a campaign) - not investigated.
