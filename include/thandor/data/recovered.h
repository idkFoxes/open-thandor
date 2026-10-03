/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/data/recovered.h
 */

#ifndef THANDOR_DATA_RECOVERED_H
#define THANDOR_DATA_RECOVERED_H

/*
Data the recovered code reached through raw image addresses. Named here so no C source uses a
bare original address; like generated/globals.h these still resolve into the mapped image until
the data moves into C definitions. Array sizes are the extent up to the next known object.
*/

/* ---- network/protocol/transfer: 64-bit block cipher, eight 16x16 nibble substitution tables
   (table n maps round-key nibble n and data nibble to a 4-bit output, stored as dwords). */

/* ---- platform/system/time_locale: function-pointer slot statically holding
   Locale_MapTelephoneCountryCodeToRegionTagPacked (no code reference to the slot found). */
#define g_LocaleMapTelephoneCountryCodeToRegionTagPacked (*(LocaleRegionTagPacked (**)(LocaleTelephoneCountryCode))&g_ImageObject_004027B0.at_g_LocaleMapTelephoneCountryCodeToRegionTagPacked)

/* ---- core/error: fatal error texts; their addresses double as the error codes. */
#define g_ErrorTextHeapAllocationFailed (*(uint16_t (*)[71])&g_ImageObject_00407D84.at_g_ErrorTextHeapAllocationFailed)

/* ---- ui/controls/lists: drive-letter buffer and the "*.*" search pattern. */

/* ---- audio/backend: decoded coefficient block of the current SAM frame. */
#define g_SoundSampleCoefficientBlock (*(short (*)[256])&g_ImageObject_00417364.at_g_SoundSampleCoefficientBlock)

/* ---- assets/text: empty string returned for missing text resources. */

/* ---- graphics/render/shading: MMX intensity scale per 8-bit level (four word lanes each). */

/* ---- software scaling weights. With g_SoftwareBilinearForwardFactors (0x0041FF20) and
   g_SoftwareBilinearInverseFactors (0x00420F20) these are four 256-entry tables; the UI scaler in
   ui/controls/input uses the pair below (first-pixel weight 0x4000 at index 0). */

/* ---- ui/controls/misc: display-settings dialog tree, copied into a fresh root. */
#define g_UiDisplaySettingsRootTemplate (*(uint32_t (*)[0x2f5])&g_ImageObject_004229B4.at_g_UiDisplaySettingsRootTemplate)

/* ---- graphics/resources/palette: palette bank slots and the remap byte table. */
#define g_GraphicsPaletteBankSlots (*(uint32_t (*)[0x200])&g_ImageObject_004AD930.at_g_GraphicsPaletteBankSlots)
#define g_GraphicsPaletteRemapBytes (*(uint8_t (*)[0x100])&g_ImageObject_004AE130.at_g_GraphicsPaletteRemapBytes)

/* ---- graphics/core: scratch for direction-to-angles in an object's local frame. */

/* ---- graphics/render/model: entries -136..-1 of g_ModelDistanceAttenuationMmx. ModelRender_ComputeVertexIntensity-
   DefaultPath indexes that table with the signed light-facing dot >> 21, so negative dots read these rows: a ramp
   rising by 0x80 per step from 0x007F (index -1) to 0x3F7F, then 0x3FFF (-128..-136); alpha lane 0x4000. The
   8 bytes 0x004cad58..0x004cad5f before it are 0x90 filler. */

/* ---- graphics/render/model: MMX distance attenuation per (distance >> 21), up to the lighting
   multiplier table that follows it. */
#define g_ModelDistanceAttenuationMmx (*(SoftwareBgraWordLanes (*)[0x222])&g_ImageObject_004CB1A0.at_g_ModelDistanceAttenuationMmx)

/* ---- audio/spatial: listener rotation basis and the combined world-to-listener transform. */

/* ---- ui/ingame: directory scratch for resource registration. */
#define g_ResourceRegistrationDirectoryUtf16 (*(uint16_t (*)[0x100])&g_ImageObject_0050DCC4.at_g_ResourceRegistrationDirectoryUtf16)

/* ---- network/protocol/transfer: version string shown to joining players ("1.5.45"). */
#define g_GameVersionUtf16 (*(uint16_t (*)[7])&g_ImageObject_0050F07C.at_g_GameVersionUtf16)

/* ---- ui/ingame: HUD number and countdown ("mm:ss") text scratch. */

/* ---- ui/ingame/technology: per-slot offsets of the technology panel's row controls. */

/* ---- gameplay/selection/overlay: transient effect markers on command targets. */

/* ---- ui/ingame: the chat phrase that unlocks the developer toggles. */

/* ---- gameplay/input/world: per pointer mode, the click handler, command id and preview army. */

/* ---- ui/ingame: keyboard dispatch records {key code, modifier mask, handler} ending in 0. */
#define g_InGameKeyboardDispatchRecordsTerminator (*(uint32_t *)&g_ImageObject_0056E5C0.at_g_InGameKeyboardDispatchRecordsTerminator)

/* ---- graphics/backend/direct3d: enumerated and selected texture pixel formats. */

/* ---- graphics/backend/glide: refresh rates offered for Glide modes. */

/* ---- graphics/backend/glide: pointers to the three GrVertex records g_GlideVertices[0..2]; no code reads the list (Glide3_DrawPrimitiveQueue pushes the vertex addresses
   directly), the name is historical. */
/* ---- graphics/backend/glide: a GrTexInfo {smallLodLog2 8, largeLodLog2 8 (256), aspect 0 (1:1),
   format 0xC (GR_TEXFMT_ARGB_4444), data NULL} after the vertex records; no code references it by
   address. */

/* ---- platform/filesystem/win32: "\\.\X:" device path of the unreachable IOCTL_STORAGE_CHECK_VERIFY
   probe in Win32Drive_CheckMediaReady (drive letter at index 4); 9 bytes of NOP fill follow up to
   FileSystem_Init. */
#define g_Win32DriveDevicePathA (*(char (*)[7])&g_ImageObject_00575CA0.at_g_Win32DriveDevicePathA)

/* ---- network/backend/fallback_udp: nonzero option value (0xFFFFFFFF) for setsockopt/ioctlsocket,
   "IP=" option. */
#define s_CommandLineOptionIp (*(char (*)[4])&g_ImageObject_00584078.at_s_CommandLineOptionIp)

/* Byte-offset access into a named object, for code that indexes records by computed offsets. */
#define THANDOR_BYTE_AT(object, offset) ((uint8_t *)&(object) + (int)(offset))
/* The address of a named object plus a byte offset as an integer, where recovered code does
   address arithmetic (table base + scaled index, pre-decremented cursors, saved offsets). */
#define THANDOR_ADDR(object, offset) ((uintptr_t)&(object) + (int)(offset))

/* ---- ui/core: the path of the window class image, after the root-stack action page. */

/* ---- gameplay/selection: transient effect markers over the owned entities (count:
   g_InGameOwnedEntityTransientEffectMarkerCount). */

/* ---- platform/input: the DirectInput mouse data format's object list and axis GUIDs. */

/* ---- ui/frontend: vtable of the 3D model pointer context (unaligned; slot 10 is
   g_FrontendModelPointerContextUpdateCallback). */
#define g_FrontendModelPointerContextVtable (*(UiNodeVtable *)&g_ImageObject_0050BB37.at_g_FrontendModelPointerContextVtable)

/* ---- graphics/backend/glide: the import table walked by Glide3_InitAndEnumerate. */

/* ---- ui/ingame: offsets of the catalog grid cells in the in-game UI image, per column count
   (the pointer tables g_UiCatalogGroup48OffsetTables etc. share the default for 0..4 columns). */

/* ---- platform/bootstrap: the main message buffer and the main window's class, which overlap in
   the original (see Win32MainMessageStorage). */
#define g_MainMessage (g_MainMessageStorage.message)
#define g_MainWindowClass (g_MainMessageStorage.overlay.windowClass)

/* ---- unreferenced original data (no code reference found; kept as data). */
/* value after g_TerrainAuxHeightMinimum (0x2000, Q12 2.0), followed by 0x90 fill up to 0x00503b10 */
#define g_TerrainUnreferencedValue00503B04 (*(int32_t *)&g_ImageObject_00503B04.at_g_TerrainUnreferencedValue00503B04)
/* file patterns after u_save___sve_0050d9c8: L"level\\*.lev" and L"level\\*.cgn" */
/* L"army0000.gfx" and "ARMY" after g_AiCommandGenerationRetainedTarget, followed by 0x90 fill up to
   0x0051b3c0 */
#define g_UnreferencedArmyTag (*(char (*)[5])&g_ImageObject_0051B3AE.at_g_UnreferencedArmyTag)

/* ---- graphics/backend/software, graphics/resources/texture: function-pointer slots of the software
   backend's hook table (statically holding the software implementations). */

/* ---- graphics: L"Software" behind the hook table (followed by two NOP padding bytes before the code at
   0x004A8F80); no code or data reference to it found. */
#define g_UnreferencedSoftwareTextUtf16 (*(uint16_t (*)[9])&g_ImageObject_004A8F6C.at_g_UnreferencedSoftwareTextUtf16)

#endif /* THANDOR_DATA_RECOVERED_H */
