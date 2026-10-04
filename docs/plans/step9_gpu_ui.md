# Step 9 (later): UI and 2D overlays on the GPU (SDL_GPU)

Status: planned, not started. Step 9 moves the drawing of the UI and the 2D overlays from the software blitters to
SDL_GPU (the same device as the 3D rasterizer of step 7), so the whole frame is made on the GPU and the UI can be
scaled for modern resolutions (2x at 1440p, 3x at 4K). The software renderer stays the reference: the checks keep
comparing its pixels.

This plan is a read-only analysis of `dev` at f36067c8 (32-bit XRGB8888 framebuffer only). Guesses are marked (GUESS),
open points TODO. Section 6.4 holds the results of the per-call-site inventories and wins where it differs from
sections 1-5. Paths are relative to `src/` unless they say otherwise.

## 1. 2D draw primitives

### 1.1 How every blit works

- **Arguments:** every blit takes `(clipMaxY, clipMaxX, clipMinY, clipMinX, drawY, drawX, [extra], subresourceIndex, asset, framebuffer)`.
- **Placement and clipping:** it draws one subresource of a "gfx" texture source at `draw + entry.origin`, clipped first to the framebuffer and then to the clip rectangle.
  - Setup: `Blit_SetupSubresource` in `graphics/backend/software_blit_helpers.h:205`.
  - Clipping: `Blit_ClipRect` in `software_blit_helpers.h:172`.
- **Slot selection:** the slots for each colour depth are installed by `GraphicsDirectDraw_PublishFramebuffer` (`graphics/core/device.cpp:95-132`).
- **Destination:** the destination is a `SoftwareFramebufferAccess*` argument. It is not always the display framebuffer; for example, the cursor is drawn into a composite buffer (`platform/sdl3/video.cpp:248`).

### 1.2 Live slots

| Slot | 32-bit implementation | Operation | Callers (references per file) |
|---|---|---|---|
| `g_GraphicsTextureSourceBlitSourceAlpha` | `BlitSourceAlpha32`, software_blit.cpp:77 | alpha 0: skip; alpha 0xFF: write through the pack LUT; anything else: blend | about 120 references:<br>controls: window 22, panels 21, scrollable 16, text_buttons 11, buttons 7, slider 6, tree_list 6, text_edit 5, lists 4, gauge 4, tooltip 3, image 1, text 1<br>others: richtext_render 2, menu_room 2, catalog_entry 2, movie_player 1, bootstrap/runtime 1, video.cpp (cursor) 1, tiled_blit 1 |
| `g_GraphicsTextureSourceBlitModulatedSourceAlpha` | :785 | per channel `(c*m)>>8`. Alpha is at most 0xFE, so every pixel is blended. A paletted texel uses the +0 colour. | font.cpp 4 (glyph colour), text_buttons 6, window 4, panels 2, buttons 1, text 1, text_edit 1 |
| `g_GraphicsTextureSourceBlitHalfSourceRgb` | :148 | always blends. A paletted source is drawn at half strength (`>>3`); a direct-colour source is a plain blend. | only through the alias in the selection-panel row below |
| `g_GraphicsTextureSourceBlitTiledSourceAlpha` / `BlitTiledHalfSourceRgb` | resources/tiled_blit.cpp:31 / :101 | lays the subresource out on a grid of its logical size and calls the plain slot for each tile | panels 5, window 3, menu_room 2 |
| `g_GraphicsTextureSourceStretchDirectColorBilinear` | :291 | bilinear stretch of an ARGB image. No clipping, opaque, works in pixel pairs (an odd last column is dropped). | buttons 2 (image control, letterboxed), panels 1, movie_player 1 |
| `g_GraphicsFramebufferFillRectArgb` | :850 | rectangle fill: alpha 0xFF is written through the LUT, anything else is blended | details in section 1.4 |
| `g_SelectionPanelBlitOpaque` / `g_SelectionPanelBlitClipped` (aliases) | SourceAlpha / TiledSourceAlpha, or HalfSourceRgb when the world view node is suppressed (menu_room.cpp:487-493, :537) | selection frames, markers, health/progress/reload bars | selection_overlay 16, selection_panel_cells 27 |
| `g_GraphicsTextureSourceTestOpaquePixel` | texture_source.cpp | hit test only, not a draw. Stays on the CPU, in logical pixels. | image 6, buttons 4, window 4, text_buttons 2, panels 1, tree_list 1 |

### 1.3 Slots with no callers (do not port)

These were only defined and installed; nothing on the live path called them. Step 8 removed them (with their
raster self-test groups):

- `BlitSaturatedAddRgb` and `BlitHalfRgbSaturatedAdd`, including their tiled slots (static in tiled_blit.cpp:17 and :20).
- `BlitIntegerScaledSourceAlpha` and `BlitSourceAlphaPaletteBank` (static in device.cpp:22 and :24).
- `FramebufferCopyRegionToOrigin` and `CopyOriginToRegion` (static in software_blit.cpp:22 and :25).

### 1.4 Fill-rect callers

1. **Tooltip darken (the only translucent full-screen fill):** `UiTooltip_Draw` (`ui/controls/tooltip.cpp:130-137`) fills the screen with 0x80000000 when no UI root is open.
2. **Fill panels:** `UiFillPanelControl_DrawColorOrTiledTextureAndChildren` (`ui/controls/panels.cpp:196-221`). All uses are opaque black: movie letterbox bars, top/bottom bars and the placement preview frame.
3. **Image letterbox bars:** `UiImageActionControl_DrawImageAndChildren` (`ui/controls/buttons.cpp:432-443`).
4. **3D viewport clear:** `SoftwareRenderer_ClearViewport` (`graphics/backend/software_rasterizer.cpp:42`).
5. **Full-screen black clears:**
   - `ui/frontend/lifecycle.cpp:221`
   - `ui/frontend/end_movie.cpp:57`
   - `gameplay/session/startup.cpp:138`
   - `platform/bootstrap/runtime.cpp:1209` and `:1217`
   - `platform/debug/movie_player.cpp:27`

### 1.5 Direct pixel writes that bypass the slots

Each of these must move behind a new slot.

**A1. Minimap control**
- Code: `UiSelectionGeometryControl_DrawClipped`, `ui/controls/minimap.cpp:179-349`.
- What it does: draws the minimap texture rotated (`rotationAngle`) and scaled (`sampleScaleQ12`). Every pixel is an opaque bilinear 2x2 sample (`SampleBilinear` :126, with its own MMX weight tables at :26). Texels outside the texture count as black.
- Writes go to `g_FramebufferAccess->pixels` with `g_FramebufferRowStrideBytes`.
- GPU version: a rotated, textured quad with a bilinear sampler and clamp-to-border black. Expect small differences in the weights.

**A2. Results graph**
- Code: `FrontendResultsGraph_*Column`, `ui/frontend/results.cpp:269`, `:328`, `:384`. Called per one-pixel column from `FrontendResultsTable_DrawColumnSequenceByType` (:214-248).
- What it does: draws stacked faction segments with opaque colours that are pre-packed through the LUT.
- The original always wrote 16-bit words, even on a 32-bit framebuffer; with 16-bit colour dropped this is fixed to proper 32-bit pixels (decided 2026-10-04).
- GPU version: one fill per segment, or the graph as a texture.

**A3. Credits cross-fade**
- Code: `SoftwareTexture_BilinearBlendScaleSubresources`, `graphics/backend/software_texture_scale.cpp:82-173`.
- Only caller: `UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren` (`ui/controls/panels.cpp:672`), which shows credits.gfx.
- What it does: cross-fades two 8-bit subresources through a per-pixel factor mask (credits_mask.cpp), then scales the result bilinearly as greyscale.
- GPU version: keep the CPU cross-fade, upload the blended 8-bit image as a streaming texture each tick, and apply the grey LUT in the shader.

**A4. 3D scene copy-back**
- Code: `WriteSceneToFramebuffer`, `platform/sdl3/gpu_renderer.cpp:1023`. This is the GPU 3D scene being copied back into the CPU framebuffer.
- With GPU 2D drawing it goes away (see section 3).

**A5. Debug font**
- Code: `DebugFont_DrawText`, `platform/debug/font.cpp:322`. Dev-only, used by the movie player.
- GPU version: fall back to software and upload, or use a fill plus glyph quads.

**Minimap texture source (input to A1, not a screen write)**
- Code: `ui/ingame/minimap_texture.cpp`. It is a CPU-built gfx asset with three ARGB planes.
- Update cadence: plane 1 every 32 ticks, plane 0 every 4 ticks (`ui/ingame/root_frame.cpp:405-410`).
- GPU version: a streaming texture, re-uploaded when it is rebuilt (an explicit dirty flag is cheaper than hashing).

**Army preview (input, not a screen write)**
- Code: `graphics/render/offscreen.cpp:28` and `gameplay/army/preview.cpp:189`. It renders on the CPU into a gfx asset that is cached for each army.
- It is drawn with the normal `BlitSourceAlpha`, so to the GPU it is just another texture.

**What was checked and found clean**
- No line drawing, no XOR, and no darkening behind dialogs or the pause screen. A covered menu room is simply not drawn (`menu_room.cpp:353`).
- Clean files: hud.cpp, preview_markers.cpp (placement ghosts are 3D models), terrain/*, debug_overlay.cpp and model_tint.cpp have no 2D framebuffer drawing.

### 1.6 Pixel maths the GPU will not match exactly (software_blit_helpers.h)

- **Blend:**
  - Lanes are `(c*0x101)>>2`.
  - `PMULHW` multiplies by `(a*0x4040)>>8` and the inverse by `((256-a)*0x4040)>>8`.
  - The two products are added, shifted `>>4` and saturated.
  - Code: `Blit_BlendLanes` :109 and `Blit_PackLanes32` :148.
  - Expect ±1 per channel against a fixed-function `SRC_ALPHA / ONE_MINUS_SRC_ALPHA` blend.
- **Brightness/contrast LUT:** opaque writes go through `g_SoftwarePixelPackTables`, which holds the brightness/contrast LUT (`graphics/backend/software_display_mode.cpp:154`, set from the display settings dialog).
  - Blended pixels do not go through it.
  - The palette +4 dword is already converted through the LUT (`graphics/resources/texture_source.cpp:200`), and the 32-bit opaque path converts it a second time.
  - With the defaults (scale 0x10000, bias 0) the LUT is the identity.

## 2. Framebuffer read-backs and how to replace them

| Where | What it does | GPU replacement |
|---|---|---|
| Cursor: `ComposeCursor` / `RestoreCursor` / `CopyCursorRectangle` (video.cpp:185-259) | Saves the rectangle under the cursor, blits the cursor into a composite buffer, writes it back, and restores the background after the present | Draw the cursor as the last quad of the present pass. No save or restore. |
| `g_GraphicsFramebufferCaptureRegion` (video.cpp:801). Users: platform/debug/autoshot.cpp (test shots), bootstrap/runtime.cpp 1, ui/ingame/editor_keyboard.cpp 1 | Copies a framebuffer region into a new gfx asset | Download from the persistent frame target (synchronous, rare). TODO: check what the runtime and editor captures are for; if the capture is only drawn back, keep it as a GPU texture. |
| Every blend: SourceAlpha with alpha < 0xFF, Modulated, HalfSourceRgb, the 0x80000000 tooltip fill | Read-modify-write of the destination pixel | Fixed-function blending |
| `WriteSceneToFramebuffer` (gpu_renderer.cpp:1023), after a fence wait in `RenderScene` (:996-1000) | Brings the 3D scene back to the CPU so the CPU overlays can draw on it | Not needed: the 3D pass stays on the GPU. This is the main saving in CPU time and latency. |
| Compare mode: `ReadFramebufferRegion` (gpu_renderer.cpp:1051) | Dev tools | Extend to the whole frame (work package 7) |
| Minimap planes (minimap_texture.cpp:297, :414) | Read-modify-write inside the texture, not the screen | Stays on the CPU |

The UI is redrawn completely every frame.
- `UiFrame_Draw` (`ui/core/frame_loop.cpp:125`) walks the whole root stack from bottom to top, then draws the tooltip; after that comes `g_GraphicsFramebufferPresent`.
- (GUESS) No control relies on pixels left from the previous frame. A persistent GPU target loaded with `LOADOP_LOAD` keeps the software semantics anyway.

## 3. Frame ordering and where GPU commands would be recorded

### 3.1 How a frame is composed today

1. **Frame loop:** `UiFrame_ProcessAndPresent` (frame_loop.cpp:53) runs input, ticks and actions, then `UiFrame_Draw`, then `Present`.
   - Other present sites, which must flush the draw list too: gameplay/session/loaded_session, new_session, loading_movie and startup; bootstrap/runtime.cpp (3); debug/movie_player.cpp (3); ui/frontend/end_movie.cpp; ui/frontend/lifecycle.cpp (2).
2. **The 3D view is a control.** `FrontendModelPointerContext_RenderWorldViewQueuesClipped` (`ui/frontend/menu_room.cpp:338`) serves both the menu room and the in-game world. In order, it does:
   1. Clear the viewport to black: `g_GraphicsSetViewportAndClearDepth` (:368).
   2. `BeginScene` (:399).
   3. Run up to four queue passes: models drawn before the terrain, the terrain, the shading pass, the remaining models.
   4. `EndScene` (:485). With the GPU renderer this renders, then downloads and writes the result into the framebuffer.
   5. Set the blit aliases (:487).
   6. Draw the 2D world overlays: army metrics and health bars, drag frame, terrain/world/grid/fluid/resource/debug markers (:496-534).
   7. Reset the aliases (:537).
   8. Draw the child controls.
   
   After the 3D view control, the HUD, the minimap, the other roots and the tooltip are drawn.
3. **Present** (`SdlVideo_Present`, video.cpp:737) composes the cursor, presents through `PresentWithGpu` (gpu_renderer.cpp:1448, upload, then a letterboxed blit) or through SDL_Renderer, and then restores the background under the cursor.
4. **Cursor timer:** `GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer` (`graphics/core/cursor.cpp`) only animates; nothing is drawn from the timer thread.

### 3.2 Proposed recording

- **One command buffer per frame.** Acquire it lazily at the first 2D draw of a frame, or at `SetViewportAndClearDepth`. Submit it in `SdlVideo_Present`.
- **Ordering:** the draw list is one ordered stream. A 3D scene is a single "external pass" entry in it, placed where `GpuRenderer_EndScene` is called:
  1. Close the current 2D render pass.
  2. Run the 3D render pass into the same frame target with load-op LOAD and the scissor set to the scene clip, using its own depth target (the black clear stays a 2D fill).
  3. Reopen the 2D pass.
- **Atlas uploads** go in a copy pass at the start of the command buffer. Uploads that come up during the frame (for example, textures first seen after the 3D pass) need either deferred recording (record into CPU vectors and build all passes at flush time; recommended) or extra copy passes.
- **Present:** draw the frame target letterboxed into the swapchain (linear filter, or nearest for integer scales), then the cursor quad.

## 4. Textures

### 4.1 What UI images are

- UI images are "gfx" texture sources (docs/software_raster.md, "Asset layout"):
  - header at +0xB0 (subresource count, palette bank count, table offset);
  - palette banks at +0x200, each 256 x 8 bytes (+0 ARGB, +4 LUT-converted);
  - 32-byte entries holding `paletteIndex` (-1 means ARGB texels), `dataOffset`, origin, pixel size and logical size.
- Sizes are arbitrary, not powers of two.

### 4.2 GPU UI texture cache

- **Key:** asset pointer, subresource and palette bank.
- **Storage:** a rectangle in a UI atlas, BGRA built from the palette's +0 ARGB (straight alpha).
- **Atlas:** a separate atlas, or a set of 2048² pages, with a general shelf packer and 1 px padding.
  - Do not reuse the 3D atlas: it is 4096², packs power-of-two sizes only and is reset every scene once it is three quarters full (gpu_renderer.cpp:80, :297, :1196).
- **Content checks:** reuse its hash-and-validate scheme (`AtlasSlotOfTexture`, :352-400), but validate once per frame instead of once per scene.
- **Mutable sources:**
  - Minimap: dirty flag set when plane 0 is rebuilt.
  - Credits cross-fade: streaming texture.
  - Movie frames (Stretch slot, movie_player.cpp): streaming texture.
  - Captured assets and army previews: immutable after creation, so hash once.
- **Eviction:** hook into the texture-source release callbacks (`g_GraphicsTextureSourceLifecycleCallbacks3`, ReleasePackage and ReleaseClone in texture_source.cpp:225-250) to evict entries. A freed pointer that is reused must not return stale pixels.
- **Fonts:** glyphs are subresources drawn with the Modulated slot (font.cpp), so they share the atlas and the colour goes into the vertex tint.
- TODO: number and size of the UI gfx assets, to size the atlas pages.

## 5. Design: a 2D draw list

```
enum Draw2DOp   { SPRITE, STRETCH_BILINEAR, ROTATED_BILINEAR (minimap), FILL, STREAM_IMAGE, EXTERNAL_3D };
enum Draw2DBlend{ SRC_ALPHA_SKIP0_OPAQUEFF, HALF_RGB, MODULATED, OPAQUE };
struct Draw2D { op, blend; asset, subresource; int dst[4], src[4], clip[4]; uint32_t tintArgb; float rotation, scale; };
```

### 5.1 Front end

- **Where:** a new module `graphics/core/draw2d.{h,cpp}` with the same signatures as the slots.
- **No call-site changes:** the GPU backend replaces the slot pointers, the same pattern as `StartGpuDevice` at gpu_renderer.cpp:1421. The roughly 200 call sites and the selection aliases stay as they are.
- **Aliases:** `g_SelectionPanelBlit*` are assigned from the slots on every frame, so they follow automatically.
- **New slots for today's direct pixel writes:**
  - `g_GraphicsMinimapDraw` (A1)
  - `g_GraphicsFillColumnSegments` (A2)
  - `g_GraphicsGreyScaleImage` (A3)
  
  Their software implementations are exactly the current loops, moved.
- **Other destinations:** if `framebuffer != &g_DisplayFramebufferAccess` (cursor composite, offscreen buffers), the GPU backend calls the software function.

### 5.2 Software backend

The existing functions, executed immediately. Output stays bit-identical and the tests stay valid.

### 5.3 GPU backend

- **Recording:** each call appends quads (position, uv, tint, flags) and a scissor, and checks or uploads its texture.
- **Batching:** batches change only on scissor, texture page or pipeline. The source order is preserved, with no reordering across overlaps.
- **Shader:** a new `platform/sdl3/shaders/ui2d.hlsl`, added to the CMake shader list at CMakeLists.txt:410 and to the precompiled headers.
  - Fragment stage: sample, then
    - Modulated: `c*m` with a truncation emulation (`floor(c*m/256)`);
    - HalfRgb: half strength for paletted sources;
    - alpha 0: discard.
  - Blend state: SRC_ALPHA / ONE_MINUS_SRC_ALPHA.
- **Brightness/contrast:** either a 256-entry LUT texture applied only on opaque texels (closer to the software path), or a final full-frame pass (simpler, but it also changes blended pixels). Prefer the per-draw LUT for opaque texels.

### 5.4 UI scaling

- **Logical resolution:** `g_FramebufferWidth/Height` stay the virtual UI resolution (for example 1280x720). All layout, hit tests and mouse coordinates stay in logical pixels.
  - `WindowToFramebuffer` (video.cpp:524) already maps window coordinates to logical framebuffer coordinates; fullscreen relative mouse mode (input.cpp:307) is unchanged.
- **GPU frame target:** N x the logical size, with N an integer (2 at 1440p, 3 at 4K). 2D destination rectangles and scissors are multiplied by N. Sprites use nearest sampling, so scaled UI stays crisp; bilinear ops keep linear sampling.
- **3D pass at N x:** scale the rebuilt vertex positions in `SoftwareVertexAt` (gpu_renderer.cpp:474) and the scissors by N. The 3D view then renders at native resolution and is no longer comparable pixel for pixel with software at N > 1.
- **Setting:** "UI scale" (auto / 1 / 2 / 3) in thandor.ini, plus `OPEN_THANDOR_UI_SCALE`. The display mode list then offers logical sizes = native / N.
- **Software renderer:** stays at N = 1. Its present is today's linear letterbox.

## 6. Risks, expected differences and work packages

### 6.1 Expected differences, GPU against software

Compare with thresholds, not identity.

- Blend rounding: ±1 per channel.
- Modulated text: ±1.
- Bilinear stretch and minimap weights: ±2 to 3. The minimap's odd-column skip and edge rules are lost.
- LUT handling: only visible when brightness/contrast differ from the defaults.
- Results graph: none expected (the software path writes proper 32-bit pixels since the 16-bit quirk was dropped).

### 6.2 Risks

- Atlas pointer reuse (needs the release hook).
- Captures need synchronous downloads (only test shots and two game uses).
- Any present path that skips the flush loses the frame. This covers movies, the loading screen, the fatal error box (`UiFrame_ProcessAndPresentWithLockTransition`) and the end movie.
- The render spin lock in the world view (menu_room.cpp) must not hold up GPU recording; recording happens on the UI thread (GUESS: all drawing is on one thread; the cursor timer does not draw).
- With N > 1, `Graphics_SetProjectionClipRect` and the hit tests stay in logical pixels; only the GPU vertex transform scales.

### 6.3 Work packages

Roughly in order. Work packages 2, 3, 5 and 6 can run in parallel after 1.

| # | Work | Files | Size | Verification |
|---|---|---|---|---|
| 1 | Draw-list API, slot routing, software passthrough; move A1-A3 behind new slots | new draw2d.cpp; device.cpp; minimap.cpp, results.cpp, panels.cpp, software_texture_scale.cpp | ~400 new lines, ~150 moved | `run_checks.py --old <previous build>`: pixels (pause and choose pages) and determinism must stay identical |
| 2 | UI texture cache and atlas with lifecycle eviction | new gpu_ui_textures.cpp; texture_source.cpp hooks | ~500 lines | Unit self-test: `OPEN_THANDOR_SELFTEST=uiatlas` packs every gfx asset of the package |
| 3 | ui2d.hlsl shader and pipelines | ui2d.hlsl; CMakeLists.txt; compiled/ headers; new gpu_ui2d.cpp | ~400 lines | Draws a test page |
| 4 | Frame integration: persistent frame target, 3D as an external pass (remove download and `WriteSceneToFramebuffer`), present plus cursor quad, flush at every present site | gpu_renderer.cpp `RenderScene`/`EndScene`/`PresentWithGpu`; video.cpp `Present` | ~300 lines | `display_settings.txt` run under Vulkan and D3D12; tools/test/skirmish_pause.txt by eye |
| 5 | GPU versions of minimap (rotated bilinear quad, streaming texture), credits (streaming grey image), movie (streaming stretch), results columns | — | ~300 lines | Screenshots of in-game, credits and results pages |
| 6 | Captures from the GPU target, so autoshot and the script `shot` command work in GPU mode | — | ~150 lines | — |
| 7 | Full-frame compare mode: `OPEN_THANDOR_GPU=compare` also runs the software 2D path (all slots software into the CPU framebuffer, GPU list in parallel) and writes `shots\gpucmp_NNNN_sw/gpu/diff.bmp` with statistics, extending `CompareScene` (gpu_renderer.cpp:1113) | gpu_renderer.cpp | ~200 lines | Threshold suggestion: mean channel difference < 0.5 and pixels > 8 under 0.1% for the UI-only pages |
| 8 | UI scale factor (setting, display-mode list, N x target, 3D vertex scale, letterbox) | — | ~250 lines | 1440p and 4K windows; mouse hit tests on the display settings page (display_settings.txt with `layout`) |

### 6.4 Addendum: results of the two remaining inventories

These findings arrived after sections 1-5 were written. Where they disagree with those sections, they take precedence.

**Exact call-site counts** (ui/controls, ui/text, menu_room, catalog_entry, movie_player, bootstrap runtime, startup, end_movie, lifecycle):

| Call | Count |
|---|---|
| `BlitSourceAlpha` | 113 |
| `ModulatedSourceAlpha` | 19 (15 use the constant shadow 0x7F000000, 4 are in font.cpp) |
| `TiledSourceAlpha` through the slot | 8 |
| `GraphicsTextureSource_BlitTiledSourceAlpha` called directly | 2 (resource gauge, `ui/controls/panels.cpp:501` and `:538`; route them too) |
| `StretchDirectColorBilinear` | 4: panels.cpp:84, buttons.cpp:417 and :444, movie_player.cpp:95 |
| `FillRectArgb` | 12 |
| `CaptureRegion` | 1 |
| `TestOpaquePixel` | 18 |

**Draw targets**
- Every one of these draws targets `g_FramebufferAccess`; there is no render-to-texture in the UI.
- The only exception is the cursor composite buffer in video.cpp.

**Tiled-blit helpers**
- The `UiWindow_BlitTiled*` helpers (window.cpp:1038, :1057, :1074) serve buttons, gauge, lists, scrollable, panels, text_buttons, window, tree_list, slider and text_edit.
- Tile rows become one quad each if the atlas page uses repeat addressing on a padded copy. The simpler option is one quad per tile.

**Tooltip**
- The tooltip background is textured tiles (tooltip.cpp:103-118), not a fill.

**Captures**
- No capture is ever drawn back. The users are:
  - the PCX screenshot (bootstrap/runtime.cpp:790);
  - the editor Ctrl+P raw capture (editor_keyboard.cpp:468);
  - autoshot.
- So a synchronous download from the GPU frame target is enough (work package 6).

**Movies**
- Intro movies are drawn by a centred `SourceAlpha` blit (runtime.cpp:1182).
- The end movie and the briefing images use the image-action control, which stretches or letterboxes them (buttons.cpp:388-447).
- Movie frames are a gfx asset whose pixels are rewritten every frame (movie/runtime/playback.cpp:231). Upload them as a streaming texture, keyed on the asset, with re-upload on every use.

**Palette +4 and brightness**
- +4 is recomputed only for these assets after a mode switch: menu, win, winclass and the fonts (ui/frontend/display_settings.cpp:258-268), and the cursor (platform/input/devices.cpp:249).
- The brightness/contrast sliders only rebuild the tables (ui/dialogs/display_settings.cpp:309, :510), so existing +4 values stay stale (inferred from the code).
- Fonts use +0.
- For the GPU, always build the atlas from +0. Treat the brightness LUT as a separate step, as in section 5.3.

**Asset lifetime**
- `Package_LoadEntry` (assets/package/runtime.cpp:254) allocates fresh on every load and `Resource_Release` frees it. The clone and releaseClone callbacks are never called.
- So the eviction hook belongs in `GraphicsTextureSource_ReleasePackageAsset` / `Resource_Release`.
- Permanent UI assets: font.gfx, fontk.gfx, win.gfx, winclass.gfx, mouse.gfx.
- Per-scene UI assets: menue.gfx, credits.gfx, panel0/diagram0 (3 resolution variants), tech, window, select.gfx, info.gfx, stat.gfx.
- That is about 15 UI gfx files. Their sizes need the game data, which is not in the repo.

**Unused refresh hooks**
- `g_GraphicsRefreshTextureAlpha` is a no-op (texture_set.cpp, `GraphicsTextureSet_RefreshNoOp`); it can serve as a GPU re-upload hook. The rebuild-all slot the texture quality settings called (`g_GraphicsRebuildAllStagingTextures`) was removed in step 8; a GPU renderer that caches downsampled textures needs a new hook there.

**Results graph**
- The faction colours are packed through the LUT (results.cpp:216-227).
- The original wrote 16-bit words even at 32 bpp; dropped together with 16-bit colour (decided 2026-10-04).

**Frame ordering (frame-loop inventory, which finished late)**
- **Who drives frames:** menu frames are driven by `ui/frontend/main_loop.cpp:27-43`, in-game frames by `gameplay/session/startup.cpp:59-103`. Both call `UiRootStack_InvalidateAll` and then `UiFrame_ProcessAndPresent` every frame.
- **Dirty rectangles are dead:** `g_UiDirtyRectEntries` (ui/core/runtime.cpp:418-446) is never read, so the whole UI is redrawn every frame.
- **Draw order in-game:**
  1. Side panel, including the minimap.
  2. `worldViewArea`, which contains `worldView` (3D scene, then the 2D overlays, then its child texts).
  3. Game windows.
  4. Chat history.
  5. Resource and panel mode stacks.
  6. Chat input.
  7. Tooltip.
  8. Cursor (at present).
  
  The side panel and the world view do not overlap (ui/ingame/layout.cpp:556).
- **Menu:** the menu room is the same 3D control, limited to the band from 1/8 to 7/8 of the screen height, with black fill bars above and below.
- **Simulation runs between render passes:** releasing the render spin lock (`g_SpinLockReleaseAndInvoke`, menu_room.cpp:400-539) runs the simulation and network tick on the main thread between the 3D passes of the same frame.
  - Consequence for the draw list: record the data a draw needs (asset pointer plus a content hash, or an upload into the staging buffer) at the time of the call. Never defer reading the asset memory to the flush.
- **Threads:** all drawing and every present run on the main thread. The SDL timer callbacks (UI tick, cursor animation, countdowns, `IntroMovie_TimerTick`) never draw.
- **Screens that rely on pixels from earlier frames:**
  - The intro movie and the debug movie player clear the screen black twice, then blit only the movie rectangle each frame (bootstrap/runtime.cpp:1158-1221).
  - Single black transition frames are presented once and not redrawn (startup.cpp:135-143, lifecycle.cpp:220-226).
  - (GUESS) The loading page `levelMovieView` and menu pages over a suppressed room may leave stale areas.
  
  The persistent GPU frame target (load-op LOAD) from section 3 keeps this behaviour. Clear it only where the software path issues fills.

### 6.5 Fixed pages for verification

These pages exist today:

- choose-game page: `tools/test/choose_game.txt`, with `-KARTE="-"`;
- paused skirmish: `tools/test/skirmish_pause.txt`, with `OPEN_THANDOR_STATEHASH_PAUSE_AT=150`, a fixed seed and `OPEN_THANDOR_AUTOSHOT=2000`;
- display settings page: `tools/test/display_settings.txt` (uses the `shot` command).

Run each once with `OPEN_THANDOR_GPU=0`, which must match the old build bit for bit (`tools/test/run_checks.py` pixels check), and once with `OPEN_THANDOR_GPU=compare`, where the difference statistics must stay under the thresholds. Add scripts for results, credits and a dialog over the world if needed.
