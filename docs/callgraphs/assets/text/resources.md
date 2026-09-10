# Call graph: `assets/text/resources`

11 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0041CD30` **TextResourcePage_LoadCompatibilityAliases** — local: `TextResourcePage_Load`, `TextResource_Resolve`, `TextResourceOverride_Register`
- `0x0041B080` **FontRuntime_Init**
- `0x0041CCB0` **TextResourcePage_Unload** — cross: `Resource_Release` → `assets/resource/runtime`
- `0x0041CDB0` **TextResourceAsset_GetLocaleBlockCount**
- `0x0041CEB0` **FontGlyph_GetLogicalSizeActiveRegs**
- `0x0041CEF0` **FontGlyph_GetLogicalSizeForStyleRegs**
- `0x0041D370` **FontGlyph_DrawBottomAligned**
- `0x0041D400` **FontGlyph_DrawVerticallyCentered**
- `0x0041CA50` **TextResourcePage_Load** — cross: `Package_LoadEntry` → `assets/package/runtime`, `Resource_Release` → `assets/resource/runtime`
- `0x0041CCF0` **TextResourceOverride_Register**
- `0x0041CDE0` **TextResource_Resolve**
