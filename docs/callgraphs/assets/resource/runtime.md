# Call graph: `assets/resource/runtime`

8 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0040E2E0` **ResourceRegistration_OpenSourceCf** — cross: `FileSystem_WriteBufferToPathCf` → `platform/filesystem/win32`, `Package_Mount` → `assets/package/runtime`
- `0x0040F000` **Resource_Load** — cross: `Package_FindEntryAcrossMounts` → `assets/package/runtime`, `WidePath_CombineDirectoryAndLeaf` → `core/text/path`, `Package_DecodeEntryInto` → `assets/package/runtime`
- `0x0040F1D0` **Resource_Release**
- `0x0050E890` **ResourceRegistration_SelectDomainPair**
- `0x00513020` **ResourceRegistration_QueryDomain0Pair**
- `0x0051E2B0` **ResourceRegistration_QueryDomain1Pair**
- `0x0052B6D0` **ResourceRegistration_QueryDomain2Pair**
- `0x00532B00` **ResourceRegistration_ResolveRuntimeRecord** — cross: `WorldRuntime_GetVector1Regs` → `world/runtime/core`, `WorldRuntime_GetVector0Regs` → `world/runtime/core`
