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

/* ---- core/error: fatal error texts; their addresses double as the error codes. */

/* ---- ui/controls/lists: drive-letter buffer and the "*.*" search pattern. */

/* ---- audio/backend: decoded coefficient block of the current SAM frame. */

/* ---- assets/text: empty string returned for missing text resources. */

/* ---- graphics/render/shading: MMX intensity scale per 8-bit level (four word lanes each). */

/* ---- software scaling weights. With g_SoftwareBilinearForwardFactors (0x0041FF20) and
   g_SoftwareBilinearInverseFactors (0x00420F20) these are four 256-entry tables; the UI scaler in
   ui/controls/input uses the pair below (first-pixel weight 0x4000 at index 0). */

/* ---- graphics/core: scratch for direction-to-angles in an object's local frame. */

/* ---- audio/spatial: listener rotation basis and the combined world-to-listener transform. */


/* ---- ui/ingame: HUD number and countdown ("mm:ss") text scratch. */

/* ---- ui/ingame/technology: per-slot offsets of the technology panel's row controls. */

/* ---- gameplay/selection/overlay: transient effect markers on command targets. */

/* ---- ui/ingame: the chat phrase that unlocks the developer toggles. */

/* ---- gameplay/input/world: per pointer mode, the click handler, command id and preview army. */

/* ---- ui/ingame: keyboard dispatch records {key code, modifier mask, handler} ending in 0. */

/* ---- graphics/backend/direct3d: enumerated and selected texture pixel formats. */

/* ---- graphics/backend/glide: refresh rates offered for Glide modes. */

/* ---- graphics/backend/glide: pointers to the three GrVertex records g_GlideVertices[0..2]; no code reads the list (Glide3_DrawPrimitiveQueue pushes the vertex addresses
   directly), the name is historical. */
/* ---- graphics/backend/glide: a GrTexInfo {smallLodLog2 8, largeLodLog2 8 (256), aspect 0 (1:1),
   format 0xC (GR_TEXFMT_ARGB_4444), data NULL} after the vertex records; no code references it by
   address. */

/* ---- network/backend/fallback_udp: nonzero option value (0xFFFFFFFF) for setsockopt/ioctlsocket,
   "IP=" option. */

/* Byte-offset access into a named object, for code that indexes records by computed offsets. */
#define THANDOR_BYTE_AT(object, offset) ((uint8_t *)&(object) + (int)(offset))
/* The address of a named object plus a byte offset as an integer, where recovered code does
   address arithmetic (table base + scaled index, pre-decremented cursors, saved offsets). */
#define THANDOR_ADDR(object, offset) ((uintptr_t)&(object) + (int)(offset))

/* ---- ui/core: the path of the window class image, after the root-stack action page. */

/* ---- gameplay/selection: transient effect markers over the owned entities (count:
   g_InGameOwnedEntityTransientEffectMarkerCount). */

/* ---- platform/input: the DirectInput mouse data format's object list and axis GUIDs. */


/* ---- graphics/backend/glide: the import table walked by Glide3_InitAndEnumerate. */

/* ---- ui/ingame: offsets of the catalog grid cells in the in-game UI image, per column count
   (the pointer tables g_UiCatalogGroup48OffsetTables etc. share the default for 0..4 columns). */

/* ---- platform/bootstrap: the main message buffer and the main window's class, which overlap in
   the original (see Win32MainMessageStorage). */
#define g_MainMessage (g_MainMessageStorage.message)
#define g_MainWindowClass (g_MainMessageStorage.overlay.windowClass)

/* ---- unreferenced original data (no code reference found; kept as data). */
/* value after g_TerrainAuxHeightMinimum (0x2000, Q12 2.0), followed by 0x90 fill up to 0x00503b10 */
/* file patterns after u_save___sve_0050d9c8: L"level\\*.lev" and L"level\\*.cgn" */
/* L"army0000.gfx" and "ARMY" after g_AiCommandGenerationRetainedTarget, followed by 0x90 fill up to
   0x0051b3c0 */

/* ---- graphics/backend/software, graphics/resources/texture: function-pointer slots of the software
   backend's hook table (statically holding the software implementations). */


#endif /* THANDOR_DATA_RECOVERED_H */
