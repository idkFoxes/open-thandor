# Call graph: `platform/bootstrap/runtime`

17 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00585D40` **ProcessEntry** — local: `CommandLine_Parse`, `DynAPI_Bootstrap`, `CommandLine_FindOption`, `Game_Run`; cross: `ArenaHeap_Init` → `core/memory/allocator`, `FileSystem_Init` → `platform/filesystem/win32`, `Locale_Init` → `platform/system/time_locale`, `ErrorSystem_Init` → `core/error/runtime`, `TimerSystem_Init` → `platform/system/time_locale`, `Graphics_Init` → `graphics/core/runtime`, `DirectInputMouse_Init` → `platform/input/devices`, `DirectSound_Init` → `audio/backend/runtime`, `Network_Init` → `network/backend/runtime`, `PersistentSettings_Load` → `core/settings/persistent`, `PersistentSettings_ReadDword` → `core/settings/persistent`, `UiRuntime_Initialize` → `ui/core/runtime`, `Runtime_Shutdown` → `core/memory/synchronization`
- `0x00512E70` **GameData_ResetDefaults**
- `0x00512F60` **GameData_LoadExternalTables** — cross: `Package_LoadEntryIntoBuffer` → `assets/package/runtime`, `Package_LoadEntry` → `assets/package/runtime`, `Resource_Release` → `assets/resource/runtime`
- `0x00573BC0` **DynAPI_Resolve** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`
- `0x00573C50` **DynDLL_Load** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`
- `0x00573CD0` **DynDLL_Unload** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`
- `0x00573D40` **BootstrapApi_ResolveBindingByDestination** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`
- `0x00573EB0` **DynDLL_UnloadAll**
- `0x00585F50` **MainWindowProc** — cross: `Keyboard_OnKeyDown` → `platform/input/devices`, `Keyboard_OnKeyUp` → `platform/input/devices`, `Keyboard_OnChar` → `platform/input/devices`
- `0x00587370` **CPU_DetectFeatures**
- `0x00573070` **Game_Run** — local: `GameRuntime_InitializeSpatialAudioAndRenderingCf`, `Game_LoadCoreAssets`, `Game_PlayIntroMovies`; cross: `PersistentSettings_Load` → `core/settings/persistent`, `PersistentSettings_ReadDword` → `core/settings/persistent`, `PersistentSettings_WriteDword` → `core/settings/persistent`, `Frontend_MainLoop` → `ui/frontend/runtime`
- `0x0050BB10` **GameRuntime_InitializeSpatialAudioAndRenderingCf** — cross: `SpatialSoundPool_Init` → `audio/spatial/runtime`, `TerrainByteClampLookup_Initialize` → `world/terrain/visuals`, `GraphicsIntensityClampTable_InitializeCf` → `graphics/render/shading`, `SoftwareRenderer_InstallDisplayModeHook` → `graphics/backend/software`, `GraphicsPrimitiveQueue_AllocateGlobalPool` → `graphics/render/primitives`
- `0x00573140` **Game_LoadCoreAssets** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`, `WidePath_CombineDirectoryAndLeaf` → `core/text/path`, `Package_Mount` → `assets/package/runtime`, `LevelPackage_ValidateAndMount` → `assets/package/runtime`, `Resource_Load` → `assets/resource/runtime`, `Resource_Release` → `assets/resource/runtime`, `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `UiActionHandlers_SetPageCf` → `ui/core/runtime`, `TextResourcePage_Load` → `assets/text/resources`, `RichTextCommandStream_BindTextureSource` → `assets/text/richtext`, `PersistentSettings_ReadDword` → `core/settings/persistent`, `AiRuntime_InitWorkspace` → `gameplay/ai/workspaces`, `Package_LoadEntry` → `assets/package/runtime`, `FncModule_LoadAndRelocateCf` → `assets/fnc/runtime`, `FncModule_GetExportByIndexCf` → `assets/fnc/runtime`
- `0x005739D0` **Game_PlayIntroMovies** — cross: `Movie_Open` → `movie/runtime/playback`, `Movie_AdvanceFrame` → `movie/runtime/playback`, `Movie_Close` → `movie/runtime/playback`, `UiFrame_FlushInputAndResetPendingTicks` → `ui/controls/layout`, `Movie_GetFrameDimensions` → `movie/runtime/playback`
- `0x00573DB0` **DynAPI_Bootstrap** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`
- `0x00586110` **CommandLine_FindOption**
- `0x00586170` **CommandLine_Parse** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`
