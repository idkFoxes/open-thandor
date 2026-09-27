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
#define g_UiTransferCipherSubstitution (*(uint32_t (*)[8][16][16])THANDOR_IMAGE(0x00405160))

/* ---- core/math/fixed: the quarter turn of the sine table before angle 0 (sin of -16384..-1 in
   Q28), directly followed by g_FixedSinQ28 and g_FixedCosQ28; signed angle lookups reach it. */
#define g_FixedSinBeforeZeroQ28 (*(int32_t (*)[16384])THANDOR_IMAGE(0x004246a0))

/* ---- core/error: fatal error texts; their addresses double as the error codes. */
#define g_ErrorTextIoInitializationFailed (*(uint16_t (*)[34])THANDOR_IMAGE(0x00407d40))
#define g_ErrorTextHeapAllocationFailed (*(uint16_t (*)[71])THANDOR_IMAGE(0x00407d84))

/* ---- ui/controls/lists: drive-letter buffer and the "*.*" search pattern. */
#define g_UiTimedListDriveLetters (*(uint8_t (*)[32])THANDOR_IMAGE(0x0040f530))
#define g_WildcardAllFilesUtf16 (*(uint16_t (*)[4])THANDOR_IMAGE(0x0040ff50))

/* ---- audio/backend: decoded coefficient block of the current SAM frame. */
#define g_SoundSampleCoefficientBlock (*(short (*)[256])THANDOR_IMAGE(0x00417364))

/* ---- assets/text: empty string returned for missing text resources. */
#define g_EmptyTextResourceUtf16 (*(uint16_t (*)[2])THANDOR_IMAGE(0x0041afa4))

/* ---- graphics/render/shading: MMX intensity scale per 8-bit level (four word lanes each). */
#define g_ShadingIntensityScaleMmx (*(SoftwareBgraWordLanes (*)[256])THANDOR_IMAGE(0x0041ee80))

/* ---- software scaling weights. With g_SoftwareBilinearForwardFactors (0x0041FF20) and
   g_SoftwareBilinearInverseFactors (0x00420F20) these are four 256-entry tables; the UI scaler in
   ui/controls/input uses the pair below (first-pixel weight 0x4000 at index 0). */
#define g_UiScalerSecondPixelWeights (*(SoftwareBgraWordLanes (*)[256])THANDOR_IMAGE(0x0041f720))
#define g_UiScalerFirstPixelWeights (*(SoftwareBgraWordLanes (*)[256])THANDOR_IMAGE(0x00420720))

/* ---- ui/controls/misc: display-settings dialog tree, copied into a fresh root. */
#define g_UiDisplaySettingsRootTemplate (*(uint32_t (*)[0x2f5])THANDOR_IMAGE(0x004229b4))

/* ---- graphics/resources/palette: palette bank slots and the remap byte table. */
#define g_GraphicsPaletteBankSlots (*(uint32_t (*)[0x200])THANDOR_IMAGE(0x004ad930))
#define g_GraphicsPaletteRemapBytes (*(uint8_t (*)[0x100])THANDOR_IMAGE(0x004ae130))

/* ---- network/protocol/transfer: outgoing chunk payload. */
#define g_UiTransferChunkPayload (*(uint32_t (*)[0x3a])THANDOR_IMAGE(0x004aea00))

/* ---- graphics/core: scratch for direction-to-angles in an object's local frame. */
#define g_GraphicsDirectionInverseTransform (*(GraphicsFixedMatrix3x4 *)THANDOR_IMAGE(0x004bcf20))
#define g_GraphicsDirectionWorld (*(GraphicsFixedVec3 *)THANDOR_IMAGE(0x004bcf50))
#define g_GraphicsDirectionLocal (*(GraphicsFixedVec3 *)THANDOR_IMAGE(0x004bcf5c))

/* ---- graphics/render/model: MMX distance attenuation per (distance >> 21), up to the lighting
   multiplier table that follows it. */
#define g_ModelDistanceAttenuationMmx (*(SoftwareBgraWordLanes (*)[0x222])THANDOR_IMAGE(0x004cb1a0))

/* ---- audio/spatial: listener rotation basis and the combined world-to-listener transform. */
#define g_SpatialSoundListenerRotation (*(GraphicsFixedMatrix3x4 *)THANDOR_IMAGE(0x0050b550))
#define g_SpatialSoundListenerWorldToLocal (*(GraphicsFixedMatrix3x4 *)THANDOR_IMAGE(0x0050b580))

/* ---- ui/ingame: directory scratch for resource registration. */
#define g_ResourceRegistrationDirectoryUtf16 (*(uint16_t (*)[0x100])THANDOR_IMAGE(0x0050dcc4))

/* ---- network/protocol/transfer: version string shown to joining players ("1.5.45"). */
#define g_GameVersionUtf16 (*(uint16_t (*)[7])THANDOR_IMAGE(0x0050f07c))

/* ---- ui/ingame: HUD number and countdown ("mm:ss") text scratch. */
#define g_InGameHudNumberTextUtf16 (*(uint16_t (*)[16])THANDOR_IMAGE(0x0055056e))
#define g_InGameCountdownTextUtf16 (*(uint16_t (*)[8])THANDOR_IMAGE(0x00550590))

/* ---- ui/ingame/technology: per-slot offsets of the technology panel's row controls. */
#define g_TechnologyPanelRowFlagOffsets (*(int (*)[7])THANDOR_IMAGE(0x00562d68))
#define g_TechnologyPanelRowValueOffsets (*(int (*)[7])THANDOR_IMAGE(0x00562d84))

/* ---- gameplay/selection/overlay: transient effect markers on command targets. */
#define g_InGameCommandTargetTransientEffectMarkers (*(EffectRuntimeSlot *(*)[128])THANDOR_IMAGE(0x00562ecc))

/* ---- ui/ingame: the chat phrase that unlocks the developer toggles. */
#define g_DeveloperChatPhraseUtf16 (*(uint16_t (*)[32])THANDOR_IMAGE(0x005631de))

/* ---- gameplay/input/world: per pointer mode, the click handler, command id and preview army. */
#define g_InGamePointerModeHandlers (*(code *(*)[8])THANDOR_IMAGE(0x00563748))
#define g_InGamePointerModeCommandIds (*(uint32_t (*)[8])THANDOR_IMAGE(0x00563768))
#define g_InGamePointerModePreviewArmyIds (*(uint32_t (*)[8])THANDOR_IMAGE(0x00563788))

/* ---- ui/ingame: keyboard dispatch records {key code, modifier mask, handler} ending in 0. */
#define g_InGameKeyboardDispatchRecords (*(UiCommandDispatchRecord (*)[36])THANDOR_IMAGE(0x0056e410))
#define g_InGameKeyboardDispatchRecordsTerminator (*(uint32_t *)THANDOR_IMAGE(0x0056e5c0))

/* ---- screenshots: "screen00.pcx" with its two-digit counter at code units 6 and 7. */
#define g_ScreenshotFileNameUtf16 (*(uint16_t (*)[13])THANDOR_IMAGE(0x00572e3c))

/* ---- graphics/backend/direct3d: enumerated and selected texture pixel formats. */
#define g_Direct3DOpaqueTextureFormat (*(DDPIXELFORMAT *)THANDOR_IMAGE(0x00577d90))
#define g_Direct3DAlphaTextureFormat (*(DDPIXELFORMAT *)THANDOR_IMAGE(0x00577db0))
#define g_Direct3DSelectedOpaqueTextureFormat (*(DDPIXELFORMAT *)THANDOR_IMAGE(0x00577dd0))
#define g_Direct3DSelectedAlphaTextureFormat (*(DDPIXELFORMAT *)THANDOR_IMAGE(0x00577df0))

/* ---- graphics/backend/glide: refresh rates offered for Glide modes. */
#define g_GlideRefreshRatesHz (*(uint32_t (*)[9])THANDOR_IMAGE(0x0057ecf0))

/* ---- network/backend/fallback_udp: option value 1 for setsockopt/ioctlsocket, "IP=" option. */
#define g_NetworkFallbackSocketOptionOn (*(uint32_t *)THANDOR_IMAGE(0x00583ef6))
#define s_CommandLineOptionIp (*(char (*)[4])THANDOR_IMAGE(0x00584078))

/* Byte-offset access into a named object, for code that indexes records by computed offsets. */
#define THANDOR_BYTE_AT(object, offset) ((uint8_t *)&(object) + (int)(offset))
/* The address of a named object plus a byte offset as an integer, where recovered code does
   address arithmetic (table base + scaled index, pre-decremented cursors, saved offsets). */
#define THANDOR_ADDR(object, offset) ((uintptr_t)&(object) + (int)(offset))

/* ---- ui/core: the path of the window class image, after the root-stack action page. */
#define u_engine_winclass_gfx_004b0eb8 (*(uint16_t (*)[20])THANDOR_IMAGE(0x004b0eb8))

/* ---- gameplay/selection: transient effect markers over the owned entities (count:
   g_InGameOwnedEntityTransientEffectMarkerCount). */
#define g_InGameOwnedEntityTransientEffectMarkers (*(EffectRuntimeSlot * (*)[32])THANDOR_IMAGE(0x00562e48))

/* ---- platform/input: the DirectInput mouse data format's object list and axis GUIDs. */
#define GUID_XAxis_Local (*(TH_LEGACY_GUID *)THANDOR_IMAGE(0x00576b28))
#define GUID_YAxis_Local (*(TH_LEGACY_GUID *)THANDOR_IMAGE(0x00576b38))
#define GUID_ZAxis_Local (*(TH_LEGACY_GUID *)THANDOR_IMAGE(0x00576b48))
#define MouseObjectFormats (*(DIOBJECTDATAFORMAT (*)[7])THANDOR_IMAGE(0x00576b70))

/* ---- ui/frontend: vtable of the 3D model pointer context (unaligned; slot 10 is
   g_FrontendModelPointerContextUpdateCallback). */
#define g_FrontendModelPointerContextVtable (*(UiNodeVtable *)THANDOR_IMAGE(0x0050bb37))

/* ---- graphics/backend/glide: the import table walked by Glide3_InitAndEnumerate. */
#define g_GlideImportBindings (*(GlideImportBinding (*)[89])THANDOR_IMAGE(0x00573fd8))

/* ---- ui/ingame: offsets of the catalog grid cells in the in-game UI image, per column count
   (the pointer tables g_UiCatalogGroup48OffsetTables etc. share the default for 0..4 columns). */
#define g_UiCatalogGroup48OffsetsDefault (*(int32_t (*)[48])THANDOR_IMAGE(0x005626a8))
#define g_UiCatalogGroup48Offsets5Columns (*(int32_t (*)[48])THANDOR_IMAGE(0x00562768))
#define g_UiCatalogGroup48Offsets6Columns (*(int32_t (*)[48])THANDOR_IMAGE(0x00562828))
#define g_UiCatalogGroup48Offsets7Columns (*(int32_t (*)[48])THANDOR_IMAGE(0x005628e8))
#define g_UiCatalogGroup48Offsets8Columns (*(int32_t (*)[48])THANDOR_IMAGE(0x005629a8))
#define g_UiCatalogGroup42OffsetsDefault (*(int32_t (*)[42])THANDOR_IMAGE(0x00562a68))
#define g_UiCatalogGroup42Offsets5Columns (*(int32_t (*)[42])THANDOR_IMAGE(0x00562b10))
#define g_UiCatalogGroup42Offsets6Columns (*(int32_t (*)[42])THANDOR_IMAGE(0x00562bb8))
#define g_UiCommandSpriteVariantAOffsets (*(int32_t (*)[24])THANDOR_IMAGE(0x00562c60))

/* ---- platform/bootstrap: the main message buffer and the main window's class, which overlap in
   the original (see Win32MainMessageStorage). */
#define g_MainMessage (g_MainMessageStorage.message)
#define g_MainWindowClass (g_MainMessageStorage.overlay.windowClass)

#endif /* THANDOR_DATA_RECOVERED_H */
