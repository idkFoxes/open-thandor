# Software triangle rasterizer

`src/graphics/backend/software_rasterizer.cpp` holds 40 triangle handlers,
`SoftwareRaster{32,Aux}_ModeNN`, decompiled from hand-written MMX and rewritten into readable C. This page
explains how they are built. The `rastercmp` self-test checked each rewrite against the original machine code; it
needed the mapped original image and was retired with it (see below).

The original also had a third family for 16-bit (RGB565/555) framebuffers, `SoftwareRaster16_ModeNN` with its own
queue walker and handler table. open-thandor runs in 32-bit colour only, so that family was deleted together with
the other 16-bit paths (blits, pixel constants, display modes). The shared helpers are in
`src/graphics/backend/software_raster.h`.

## Dispatch

`SoftwareRenderer_DrawQueue32Bit` and `SoftwareRenderer_DrawQueueAuxiliary` call
`SoftwareRenderer_PrepareTrianglePacket` for each packet. That call sorts the vertices by Y, snaps X and Y to
whole pixels (Q12), adds the depth epoch and scales U/V to the texture size. Each packet then goes to
`handlers[(renderFlags & 0x3f000) >> 12]` of the family's 64-entry table
(`g_SoftwareRasterHandlers32Bit`, `g_SoftwareRasterHandlersAuxiliary`). Every handler takes
`(clipMaxY, clipMaxX, clipMinY, clipMinX, packet)`.

| family | target | pixel | colour row stride | depth row stride |
|---|---|---|---|---|
| `Raster32` | `g_FramebufferAccess->pixels` | 32 bit ARGB | `g_FramebufferRowStrideBytes` | `g_SoftwareDepthRowStrideBytes` |
| `RasterAux` | `g_SoftwareAuxiliaryTargetBase` | 32 bit ARGB | `clipMaxX * 4` | `clipMaxX * 4` (!) |

The Aux draw queue passes `clipMinY = clipMinX = 0`. The Aux handlers still clip against the
arguments.

Mode index bits: 1 = `0x1000`, 2 = `0x2000`, 4 = `0x4000`, 8 = `0x8000` (flat: all three
vertex colours are equal), 16 = `0x10000` (textured). Indices 32..63 reuse the handlers 1, 2, 9,
10, 17, 18, 25 and 26. Only 20 indices per family have their own handler: 0, 1, 2, 4, 6, 8, 9,
10, 12, 14, 16, 17, 18, 20, 22, 24, 25, 26, 28, 30.

These pairs are byte-identical in the original, so each pair is implemented once and both
handlers call it:

- both families: 04 = 06, 12 = 14, 20 = 22, 28 = 30
- Aux only, additionally: 01 = 02, 09 = 10, 17 = 18, 25 = 26

## Common skeleton

Every handler follows the same four steps:

1. **Triangle setup** (`Raster_SetupTriangle`):
   - Return if the triangle has no height: `height = v2.y - v0.y <= 0`.
   - Compute `invHeight = 0x1000000 / height`.
   - Set up the long edge v0 -> v2: X, depth, colour and U/V, each with a per-scanline step
     `(delta * invHeight) >> shift`.
   - Compute `cross = (x2-x0)(y1-y0) - (x1-x0)(y2-y0)` and `doubleArea = (int)(cross >> 12)`.
     Return if `doubleArea == 0`.
   - Compute `invArea = 0x1000000000 / doubleArea`.
   - Compute the X gradients (`Raster_GradientX`):
     `(int)(((a2-a0)(y1-y0) - (a1-a0)(y2-y0)) >> 12) * invArea >> shift`. The shift is 24 for
     depth and U/V, and 30 for colour (with `a = channel << 12`).
2. **Edge walk** (`Raster_WalkTriangle` / `Raster_WalkRows`): upper part with short edge
   v0 -> v1, then lower part with v1 -> v2. The short-edge step is
   `((0x1000000 / dy) * dx) >> 12`. The scanline counter carries on from the upper to the lower
   part. Only rows with `clipMinY <= y < clipMaxY` are drawn. After each row the long-edge
   attributes, both edge Xs and Y advance.
3. **Scanline** (`Raster_DrawScanline`): `longX = longEdgeX >> 12`, `shortX = shortEdgeX >> 12`.
   The span always starts at the long edge:
   - `shortX > longX`: draw left to right from `max(longX, clipMinX)` to
     `min(shortX, clipMaxX)`. Prestep is `((first + 1) << 12) - longEdgeX`.
   - `shortX < longX`: draw right to left from pixel `min(longX, clipMaxX) - 1` down to
     `max(shortX, clipMinX)`. Prestep is `(first << 12) - longEdgeX`, where `first` is the value
     before the `- 1`.

   From the prestep: depth and U/V get `+ (prestep * stepX) >> 12`. Colour lanes get
   `+ (short)((stepX * (short)(prestep >> 8)) >> 4)`, which is what the PMULHW/PMULLW pair
   computes.
4. **Span** (the mode's `RasterSpanProc`): per pixel, test `depth <= *depthBuffer` (unsigned),
   do the mode's pixel operation, then `RasterSpan_Next`. The deltas are already negated for
   right-to-left spans.

Attribute formats:

- Gouraud colour starts at `c << 6` (Q6) and is interpolated. Flat colour is v0's colour as
  `(c * 0x101) >> 2` and never stepped.
- Textures are nearest-texel and wrap:
  `index = ((u & uMask) >> 12) + (((v & vMask) >> 12) << widthLog2)`. A paletted texture reads
  one byte per texel and looks it up in an 8-byte palette entry at
  `asset + 0x200 + bank * 0x800`. A direct-colour texture (`paletteIndex < 0`) reads one dword
  per texel. See `Raster_SetupTexture` and `Raster_FetchTexel`.
- The textured colour is `Raster_Modulate(colour, Raster_TexelLanes(texel))`, which gives Q4.

## Per-pixel operations

Notation:

- **S**: source colour in Q4. For untextured modes this is `colour >> 2`; for textured modes it
  is the modulated texel.
- **D**: destination unpacked to Q4 (`Raster_Unpack32`).
- **sat(x)**: `Raster_LanesToBytes(x, 4)`, then `Raster_Pack32`.
- For opaque untextured modes, `sat(colour >> 6)` equals `sat(S >> 4)`.

| modes | Raster32 | Aux |
|---|---|---|
| 0, 8, 16, 24 | write sat(S), write depth | same |
| 1, 9, 17, 25 | write sat(`Raster_BlendAlpha(S, D)`), no depth write | write sat(S). D is read but unused. No depth write |
| 2, 10, 18, 26 | write sat(S + D) (additive), no depth write | same as Aux 1 |
| 4/6, 12/14 | like 1, and write depth if **stale MM2** (see below) | write sat(S), and write depth if stale MM2 |
| 20/22, 28/30 | like 1, and write depth if `(word)S.alpha >= 0x800`, i.e. the modulated alpha is >= 128 (a negative lane also passes) | write sat(S), and write depth if the span's U prestep delta `(unsigned)((prestep * uStepX) >> 12) >= 0x800` (leftover MM2) |

Stale MM2: the untextured modes 4/6/12/14 test `MM2 >= 0x800` but never load MM2, so the
original sees whatever the previous triangle left in it. The current C deliberately uses the
pixel's blend index instead: write depth when the alpha lane `>> 4 > 0x7f`. rastercmp ran the
original with MM2 = 0 and with MM2 = ~0. Colour then had to match exactly, and every depth dword
had to match one of the two runs. Keep the C behaviour; read it from the current function.

The blend index is `(word)S.alpha >> 4`. The alpha lane is in -0x2000..0x1FFF, so the index is
0..0x1FF (alpha overshoots 255, e.g. 256/257 from interpolation) or 0xE00..0xFFF (negative alpha).
The original then reads past the 256-entry `g_SoftwareBlendAlphaFactors` and
`g_SoftwareBlendInverseAlphaFactors` tables: the alpha table runs into the inverse table, the
inverse table into 0x422720-0x422F1F (a vtable, scratch strings, code, UI callbacks and template),
and the 0xE00.. indexes land in `g_FixedSineQ28`. `Raster_BlendAlpha` reproduces these reads
through `Raster_OriginalBlendDword`: today's variables for the tables, the scratch strings and the
sine table, and a constant table of the original dwords (pointers, code, UI data) for the rest.
Do not clamp the index.

## Globals

The handlers read these globals:

- `g_FramebufferAccess` (0x4A8E70, `->pixels`) and `g_FramebufferRowStrideBytes` (0x4A8E84)
- `g_SoftwareDepthBuffer` (0x4D1238) and `g_SoftwareDepthRowStrideBytes` (0x4D1234)
- `g_SoftwareAuxiliaryTargetBase` (0x4D123C)
- `g_SoftwareBlendAlphaFactors` (0x421720) and `g_SoftwareBlendInverseAlphaFactors` (0x421F20)
- the packet (vertices at +0x00/+0x20/+0x40: X, Y, depth +0x10, U +0x14, V +0x18, colour +0x1C)
  and `packet->textureEntry` (`widthLog2`, `heightLog2`, `sourceAsset`, `sourceEntry` with
  `paletteIndex` and `dataOffset`)

The original's 16-bit family also read the 565/555 pack and unpack constants
(`g_SoftwarePixelMmxConstants`, 0x41F6E0..0x41F6FF); they went with it.

The original also writes `g_SoftwareRasterScanState` (0x4D11C0..0x4D1233), but only as scratch.
No handler reads anything it did not write in the same call; rastercmp checked this and found
none. The rewrite keeps these values in locals (`RasterEdges`, `RasterGradients`, `RasterSpan`).

## Helpers (software_raster.h)

| helper | purpose |
|---|---|
| `RasterColor` | four 16-bit lanes (b, g, r, a) with wrapping add, `RasterColor_Negate` and `RasterColor_ShiftRight` (PSRAW) |
| `Raster_MulShift`, `Raster_Diff`, `Raster_Channel`, `Raster_SaturateByte`, `Raster_MulHigh` | exact 64-bit product shift, wrapping difference, byte channel, PACKUSWB clamp, PMULHW |
| `Raster_Unpack32`, `Raster_Pack32` | 32-bit ARGB pixels |
| `Raster_LanesToBytes` | `>> shift` + saturate |
| `Raster_BlendAlpha` | alpha-table blend of two Q4 colours |
| `Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD/FLAT, textured, &edges, &gradients)` | step 1 |
| `Raster_SetupTexture`, `Raster_FetchTexel`, `Raster_TexelLanes`, `Raster_Modulate` | textures |
| `Raster_FramebufferTarget(pixelBytes, ...)`, `Raster_AuxiliaryTarget(...)` | family targets |
| `Raster_WalkTriangle(&target, packet, &edges, &gradients, texture or NULL, spanProc)` | steps 2 and 3 |
| `RasterSpan_Next` | step 4 advance |

A handler then comes down to a span function and a few lines of setup (see
`SoftwareRaster32_Mode16`, which goes through `Raster32_DrawTextured`):

```c
static void Raster32_SpanTexturedOpaque(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor texel = Raster_TexelLanes(Raster_FetchTexel(span->texture, span->u, span->v));
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(Raster_Modulate(span->color, texel), 4, channel);
            *(uint32_t *)span->pixel = Raster_Pack32(channel);
            *span->depth = span->depthValue;
        }
        RasterSpan_Next(span);
    }
}
```

Span functions are shared across modes: flat and Gouraud use the same one, and the opaque
untextured span `Raster32_SpanShadedOpaque` also serves Aux modes 0/8.

## rastercmp (retired)

The self-test ran each C handler and a copy of the original code (0x4D1710..0x4FE620, read
from `thandor_original.exe`) on the same random input (sizes, clip rectangles and strides, 565
and 555 layouts for the 16-bit family, framebuffer and depth contents, partly off-screen triangles, paletted and
direct textures from 1x1 to 256x256, scan state) and compared colour, depth, guard bytes and scan state.
It needed the original image mapped at its address and was removed with the mapped build (step 4c),
after every handler had passed it. Changes to the handlers are now covered by the behaviour checks
(`tools/test/run_checks.py`: determinism, pixel compare against the previous build).

Pitfalls, all of which rastercmp caught:

- Keep the 64-bit products and truncation points exactly as the helpers do.
- Colour lanes are 16 bit and wrap.
- The right-to-left prestep does not add one pixel; the left-to-right prestep does.
- Aux rows are `clipMaxX` pixels long.
- `SHLD` results are signed 32 bit.
- If you use MMX intrinsics instead of plain C, end with `_mm_empty()`.

# Blits

`src/graphics/backend/software_blit.cpp` holds the 2D paths that draw a texture-source subresource or a coloured
rectangle straight into the framebuffer. `src/graphics/core/device.cpp` installs them
(`g_GraphicsTextureSourceBlit*`, `g_GraphicsFramebufferFillRectArgb`). The software renderer
is the only renderer: the original's Glide and Direct3D backends were removed. Like the rasterizer they were
hand-written MMX, and the decompiled C was full of `CONCAT`/`pmulhw` emulation. The original had each blit and
the fill twice, for 16-bit and for 32-bit framebuffers; only the 32-bit versions are left.

The `blitcmp` self-test compared each of them with the original machine code (retired with the mapped
build, see below):

| function | original | C status |
|---|---|---|
| `SoftwareTextureSource_BlitSourceAlpha32` | 0x4A9710 | rewritten (reference) |
| `SoftwareFramebuffer_FillRectArgb32` | 0x4AD2A0 | rewritten (reference) |
| `SoftwareTextureSource_BlitHalfSourceRgb32` | 0x4A9E10 | rewritten |
| `SoftwareTextureSource_BlitSourceAlphaPaletteBank32` | 0x4AB150 | removed in step 8 (nothing called it) |
| `SoftwareTextureSource_BlitModulatedSourceAlpha32` | 0x4AC4C0 | rewritten |
| `SoftwareTextureSource_BlitSaturatedAddRgb32` | 0x4AB750 | removed in step 8 (nothing called it) |
| `SoftwareTextureSource_BlitHalfRgbSaturatedAdd32` | 0x4ABD20 | removed in step 8 (nothing called it) |
| `SoftwareTextureSource_BlitIntegerScaledSourceAlpha32` | 0x4AAA40 | removed in step 8 (nothing called it) |
| `SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31` | 0x519270 | rewritten; now in `src/ui/frontend/credits_mask.cpp` (its only caller is the credits screen) |

## Arguments

- plain blits: `(clipMaxY, clipMaxX, clipMinY, clipMinX, drawY, drawX, subresourceIndex, asset, framebuffer)`
- `Modulated`: the extra argument (ARGB) comes before `subresourceIndex`
- fills: `(clipMaxY, clipMaxX, clipMinY, clipMinX, rectMaxY, rectMaxX, rectMinY, rectMinX, argb, framebuffer)`

The framebuffer (`SoftwareFramebufferAccess`) is `width, height, bytesPerPixel, pixels`, and its row
stride is `width * bytesPerPixel`. Every function first checks `bytesPerPixel` (4) and
returns if it does not match.

## Asset layout

A texture source asset has the magic `ASSET_MAGIC_GFX` and, at +0xB0, `subresourceCount`,
`paletteBankCount` and `subresourceTableOffset`. The subresource table holds 32-byte
`GraphicsTextureSourceEntry` records: `paletteIndex` +8, `dataOffset` +0xC, `originX/Y`
+0x10/+0x14, `pixelWidth/Height` +0x18/+0x1C. Palette banks are 256 entries of 8 bytes each, at
`asset + 0x200 + bank * 0x800`:

- +0: the ARGB colour.
- +4: the colour converted to the framebuffer's pixel format (`g_GraphicsTextureSourceConvertPaletteEntries`),
  with the alpha in the top byte.

`paletteIndex == -1` means one ARGB dword per texel. Any other value must be below
`paletteBankCount` (compared unsigned), or nothing is drawn.

Palette quirk, kept as it is: the 32-bit blits use +4 for everything: the alpha test, the blend colour, and the
opaque write, which converts +4 through `g_SoftwarePixelPackTables` a second time. Exceptions are noted below
(Modulated). The original's 16-bit blits tested the alpha of +4, wrote its low word when opaque,
and blended +0 using +0's own alpha.

## Common structure

Every blit and the fill follow the same steps:

1. **Validate** the magic, `subresourceIndex < subresourceCount` (unsigned), `bytesPerPixel`, and
   the palette index.
2. **Place** the image. Its rectangle is `[drawX + originX, + pixelWidth) x [drawY + originY, + pixelHeight)`.
   A fill uses the rect arguments instead.
3. **Clip** (`Blit_ClipRect`). Clamp the rectangle to 0 and to the framebuffer size, then to the
   clip rectangle. All compares are signed. Return if nothing is left.
4. **Walk** the clipped rows. The source starts at
   `data + ((top - drawY - originY) * pixelWidth + (left - drawX - originX)) * texelBytes`.
5. **Per pixel**, apply the function's operation. Transparent texels leave the pixel alone.

`Blit_SetupSubresource` does steps 1 to 3 and fills a `BlitRegion` (texel and pixel pointers,
strides, palette, size). A rewritten blit is then only the loop over the region with its pixel
operation. See `SoftwareTextureSource_BlitSourceAlpha32`:

```c
if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                           clipMinY, clipMinX, &region)) {
  return false;
}
for (y = 0; y < region.height; y++) {
  const uint8_t *texel = region.texels + y * region.texelStride;
  uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
  for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
    ... Blit_IsTransparent / Blit_IsOpaque / Blit_BlendArgb32 ...
  }
}
```

## Pixel operations and helpers

The blit helpers are in `software_blit_helpers.h`, in the section "Texture-source blits and rectangle
fills". They reuse `RasterColor` and `Raster_MulHigh`.

| helper | MMX it reproduces |
|---|---|
| `Blit_IsTransparent`, `Blit_IsOpaque` | `CMP EAX,0x1000000 / JC` and `CMP EAX,0xFF000000 / JC` on the whole dword |
| `Blit_PaletteColor`, `Blit_PalettePixel` | the palette entry's +0 and +4 dwords |
| `Blit_ConvertArgb` | the `g_SoftwarePixelPackTables` lookup (blue + alpha + green + red, dword adds) |
| `Blit_ArgbLanes(argb, shift)` | `PUNPCKLBW x,x` + `PSRLW shift`: lanes `(c * 0x101) >> shift` |
| `Blit_BlendLanes(src, dst, alpha)` | `PMULHW` with `g_SoftwareBlendAlphaFactors[alpha]` and `...InverseAlphaFactors[alpha]`, then `PADDW` |
| `Blit_PackLanes32` | `PSRLW 4` (logical) + `PACKUSWB` (the alpha lane is written too) |
| `Blit_BlendArgb32` | the source-alpha blend of one ARGB colour over one pixel |

Pixel operations of the other blits, read from their asm (the rewritten code follows them). The palette-bank,
saturated-add, half-RGB saturated-add and integer-scaled blits, their tiled forms and the framebuffer region
copies were removed in step 8: nothing in the game called them.

- **HalfSourceRgb**: there is no opaque shortcut, so alpha 0xFF also blends (factor index 0xFF).
  The paletted path halves the source lanes (`(c * 0x101) >> 3`); the direct-colour path uses
  `>> 2`. The destination is `Blit_ArgbLanes(d, 2)`.
- **Modulated**: before the alpha tests, each source channel is multiplied by the matching
  modulation channel. Blue is `(b * mb) >> 8`; green, red and alpha are `((c * mc) & 0xFF00)`
  shifted into place. The rest is the same as SourceAlpha, except that a paletted texel uses +0.
- **Mask step**: every nonzero byte of `maskPixels` gets `+ 0x1F`, saturated at 0xFF
  (`PCMPEQB` / `PAND` / `PXOR` / `PADDUSB`). It works on 32-byte blocks,
  `width * height >> 5` of them, with the size taken from `g_GraphicsTextureSourceGetLogicalSize`.
  With fewer than 32 pixels the count is 0, and the original loops 2^32 times.

Do not clamp or "fix" anything: wrapping lanes, the logical `PSRLW` and the palette +0/+4 mix-up
are all part of the original output.

## blitcmp (retired)

The test copied 0x4A93C0..0x4AD410 and 0x519270..0x519318 from `thandor_original.exe` and ran both
versions on identical copies of random input (assets with several banks and subresources, early
returns, 16- and 32-bit framebuffers, partly outside or inverted clip rectangles, 565/555 pixel
constants, scale, palette bank, modulation and fill arguments, the mask step), comparing the
framebuffer with guards, the mask buffer, the asset and the carry flag. Like rastercmp it needed
the mapped-image build and was removed with it (step 4c), after all blits had passed it.

## Tests

All raster handlers and blits are rewritten; rastercmp, blitcmp and blendscalecmp verified them against the
original machine code before they were retired. Today `OPEN_THANDOR_SELFTEST=raster`
(`src/platform/selftest/raster_selftest.cpp`) is the safety net: it runs the handlers, the blits and the mask step
on synthetic seeded input and logs one golden hash per group, so two builds can be compared without game data.
