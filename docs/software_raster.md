# Software triangle rasterizer

`src/graphics/backend/software.c` holds 60 triangle handlers,
`SoftwareRaster{16,Non16,Aux}_ModeNN`, decompiled from hand-written MMX. This page
explains how they are built, so they can be rewritten into readable C one group at a
time. The `rastercmp` self-test checked each rewrite against the original machine code; it needed the
mapped original image and was retired with it (see below).

Already rewritten (the reference examples): `SoftwareRaster16_Mode00/01/02/08/16/24`,
`SoftwareRasterNon16_Mode00/02` and `SoftwareRasterAux_Mode00`. The shared helpers are in
`src/graphics/backend/software_raster.h`.

## Dispatch

`SoftwareRenderer_DrawQueue16Bit`, `SoftwareRenderer_DrawQueueNon16Bit` and
`SoftwareRenderer_DrawQueueAuxiliary` call `SoftwareRenderer_PrepareTrianglePacket` for each
packet. That call sorts the vertices by Y, snaps X and Y to whole pixels (Q12), adds the depth
epoch and scales U/V to the texture size. Each packet then goes to
`handlers[(renderFlags & 0x3f000) >> 12]` of the family's 64-entry table
(`g_SoftwareRasterHandlers16Bit`, `...Non16Bit`, `...Auxiliary`). Handler ABI: five stack
arguments `(clipMaxY, clipMaxX, clipMinY, clipMinX, packet)`, `ret 0x14`, no register
arguments. The C functions are plain cdecl, because the tables are only called from C.

| family | target | pixel | colour row stride | depth row stride |
|---|---|---|---|---|
| `Raster16` | `g_FramebufferAccess->pixels` | 16 bit, 565/555 via `g_SoftwarePixelMmxConstants` | `g_FramebufferRowStrideBytes` | `g_SoftwareDepthRowStrideBytes` |
| `RasterNon16` | `g_FramebufferAccess->pixels` | 32 bit ARGB | `g_FramebufferRowStrideBytes` | `g_SoftwareDepthRowStrideBytes` |
| `RasterAux` | `g_SoftwareAuxiliaryTargetBase` | 32 bit ARGB | `clipMaxX * 4` | `clipMaxX * 4` (!) |

The Aux draw queue passes `clipMinY = clipMinX = 0`. The Aux handlers still clip against the
arguments.

Mode index bits: 1 = `0x1000`, 2 = `0x2000`, 4 = `0x4000`, 8 = `0x8000` (flat: all three
vertex colours are equal), 16 = `0x10000` (textured). Indices 32..63 reuse the handlers 1, 2, 9,
10, 17, 18, 25 and 26. Only 20 indices per family have their own handler: 0, 1, 2, 4, 6, 8, 9,
10, 12, 14, 16, 17, 18, 20, 22, 24, 25, 26, 28, 30.

These pairs are byte-identical in the original, so implement each pair once and have both
handlers call it:

- all families: 04 = 06, 12 = 14, 20 = 22, 28 = 30
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
- **D**: destination unpacked to Q4. Use `Raster_Unpack16` for the 16-bit family and
  `Raster_Unpack32` for the 32-bit families.
- **sat(x)**: `Raster_LanesToBytes(x, 4)`, then `Raster_Pack16` or `Raster_Pack32`.
- For opaque untextured modes, `sat(colour >> 6)` equals `sat(S >> 4)`.

| modes | 16 and Non16 | Aux |
|---|---|---|
| 0, 8, 16, 24 | write sat(S), write depth | same |
| 1, 9, 17, 25 | write sat(`Raster_BlendAlpha(S, D)`), no depth write | write sat(S). D is read but unused. No depth write |
| 2, 10, 18, 26 | write sat(S + D) (additive), no depth write | same as Aux 1 |
| 4/6, 12/14 | like 1, and write depth if **stale MM2** (see below) | write sat(S), and write depth if stale MM2 |
| 20/22, 28/30 | like 1, and write depth if `(word)S.alpha >= 0x800`, i.e. the modulated alpha is >= 128 (a negative lane also passes) | write sat(S), and write depth if the span's U prestep delta `(unsigned)((prestep * uStepX) >> 12) >= 0x800` (leftover MM2) |

Stale MM2: the untextured modes 4/6/12/14 test `MM2 >= 0x800` but never load MM2, so the
original sees whatever the previous triangle left in it. The current C deliberately uses the
pixel's blend index instead: write depth when the alpha lane `>> 4 > 0x7f`. rastercmp runs the
original with MM2 = 0 and with MM2 = ~0. Colour must then match exactly, and every depth dword
must match one of the two runs. Keep the C behaviour; read it from the current function.

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
- `g_SoftwarePixelMmxConstants` (0x41F6E0..0x41F6FF: pack weights, quantize masks, unpack
  scales, pixel masks for 565/555), used by the 16-bit family
- `g_SoftwareBlendAlphaFactors` (0x421720) and `g_SoftwareBlendInverseAlphaFactors` (0x421F20)
- the packet (vertices at +0x00/+0x20/+0x40: X, Y, depth +0x10, U +0x14, V +0x18, colour +0x1C)
  and `packet->textureEntry` (`widthLog2`, `heightLog2`, `sourceAsset`, `sourceEntry` with
  `paletteIndex` and `dataOffset`)

The original also writes `g_SoftwareRasterScanState` (0x4D11C0..0x4D1233), but only as scratch.
No handler reads anything it did not write in the same call; rastercmp checks this and found
none. A rewrite keeps these values in locals (`RasterEdges`, `RasterGradients`, `RasterSpan`).
rastercmp therefore reports it as "state-only", which is not an error.

## Helpers (software_raster.h)

| helper | purpose |
|---|---|
| `RasterColor` | four 16-bit lanes (b, g, r, a) with wrapping add, `RasterColor_Negate` and `RasterColor_ShiftRight` (PSRAW) |
| `Raster_MulShift`, `Raster_Diff`, `Raster_Channel`, `Raster_SaturateByte`, `Raster_MulHigh` | exact 64-bit product shift, wrapping difference, byte channel, PACKUSWB clamp, PMULHW |
| `Raster_Unpack16`, `Raster_Pack16`, `Raster_ShadeToPixel16` | 565/555 pixels through `g_SoftwarePixelMmxConstants` |
| `Raster_Unpack32`, `Raster_Pack32` | 32-bit ARGB pixels |
| `Raster_LanesToBytes` | `>> shift` + saturate |
| `Raster_BlendAlpha` | alpha-table blend of two Q4 colours |
| `Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD/FLAT, textured, &edges, &gradients)` | step 1 |
| `Raster_SetupTexture`, `Raster_FetchTexel`, `Raster_TexelLanes`, `Raster_Modulate` | textures |
| `Raster_FramebufferTarget(pixelBytes, ...)`, `Raster_AuxiliaryTarget(...)` | family targets |
| `Raster_WalkTriangle(&target, packet, &edges, &gradients, texture or NULL, spanProc)` | steps 2 and 3 |
| `RasterSpan_Next` | step 4 advance |

A handler then comes down to a span function and six lines of setup (see
`SoftwareRaster16_Mode16`):

```c
static void Raster16_SpanTexturedOpaque(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor texel = Raster_TexelLanes(Raster_FetchTexel(span->texture, span->u, span->v));
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(Raster_Modulate(span->color, texel), 4, channel);
            *(word *)span->pixel = Raster_Pack16(channel);
            *span->depth = span->depthValue;
        }
        RasterSpan_Next(span);
    }
}
```

Span functions can be shared across modes: flat and Gouraud use the same one. Across families,
only the pixel store differs, so an agent can add `Raster32_*` spans once and reuse them for
Non16 and Aux. The existing spans are `Raster16_SpanShadedOpaque`,
`Raster16_SpanShadedAlphaBlend`, `Raster16_SpanShadedAdd`, `Raster16_SpanTexturedOpaque`,
`Raster32_SpanShadedOpaque` and `Raster32_SpanShadedAdd`.

## rastercmp (retired)

The self-test ran each C handler and a copy of the original code (0x4D1710..0x4FE620, read
from `thandor_original.exe`) on the same random input (sizes, clip rectangles and strides, 565
and 555 layouts, framebuffer and depth contents, partly off-screen triangles, paletted and direct
textures from 1x1 to 256x256, scan state) and compared colour, depth, guard bytes and scan state.
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

## Work packages

The remaining 51 handlers split into six packages. Each package touches only its own handler
bodies in `software.c`: the families occupy separate, contiguous regions, so git merges cleanly.
Put new helpers as `static` functions right above the first handler that uses them. Only add a
helper to `software_raster.h` if another package needs it, and then append it at the end in a
section named after your package.

| WP | handlers | new pixel ops |
|---|---|---|
| 1 | `Raster16_Mode04/06, 09, 10, 12/14` (6) | alpha blend with the C depth rule. 09/10/12/14 are flat versions of 01/02/04 |
| 2 | `Raster16_Mode17, 18, 20/22, 25, 26, 28/30` (8) | textured blend, additive and alpha-tested depth. Reuse the WP1 depth rule |
| 3 | `RasterNon16_Mode01, 04/06, 08, 09, 10, 12/14` (8) | 32-bit blend spans (`Raster32_SpanShadedAlphaBlend`, ...) |
| 4 | `RasterNon16_Mode16, 17, 18, 20/22, 24, 25, 26, 28/30` (10) | 32-bit textured spans |
| 5 | `RasterAux_Mode01/02, 04/06, 08, 09/10, 12/14` (9) | Aux "blend" = plain write without depth; stale-MM2 depth rule |
| 6 | `RasterAux_Mode16, 17/18, 20/22, 24, 25/26, 28/30` (10) | Aux textured, and the U-prestep depth rule for 20/22/28/30 |

WP1 and WP2 share the 16-bit alpha-tested depth rule, and WP3/WP5 share the 32-bit rule. The
first package to need a rule should add it as a helper and tell the others its name. For the
alpha-tested depth rule of untextured modes, reproduce the current C condition. The
`SoftwareTextureSource_*` blits in the same file are not triangle handlers and are out of scope
here; see [Blits](#blits) below.

# Blits

`software.c` also holds the 2D paths that draw a texture-source subresource or a coloured
rectangle straight into the framebuffer. They are installed per framebuffer depth by
`directdraw.c` (`g_GraphicsTextureSourceBlit*`, `g_GraphicsFramebufferFillRectArgb`). The software renderer
is the only renderer: the original's Glide and Direct3D backends were removed. Like the rasterizer they were
hand-written MMX, and the
decompiled C was full of `CONCAT`/`pmulhw` emulation.

The `blitcmp` self-test compared each of them with the original machine code (retired with the mapped
build, see below):

| function | original | C status |
|---|---|---|
| `SoftwareTextureSource_BlitSourceAlpha16/32` | 0x4A93C0, 0x4A9710 | rewritten (reference) |
| `SoftwareFramebuffer_FillRectArgb16/32` | 0x4AD110, 0x4AD2A0 | rewritten (reference) |
| `SoftwareTextureSource_BlitHalfSourceRgb16/32` | 0x4A9B20, 0x4A9E10 | rewritten |
| `SoftwareTextureSource_BlitSourceAlphaPaletteBank16/32` | 0x4AADE0, 0x4AB150 | rewritten |
| `SoftwareTextureSource_BlitModulatedSourceAlpha16/32` | 0x4AC040, 0x4AC4C0 | rewritten |
| `SoftwareTextureSource_BlitSaturatedAddRgb16/32` | 0x4AB4A0, 0x4AB750 | rewritten |
| `SoftwareTextureSource_BlitHalfRgbSaturatedAdd16/32` | 0x4ABA70, 0x4ABD20 | rewritten |
| `SoftwareTextureSource_BlitIntegerScaledSourceAlpha16/32` | 0x4AA630, 0x4AAA40 | rewritten |
| `SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31` | 0x519270 | rewritten |

"Identical" means blitcmp found no difference in 3000 runs on seeds 1 and 7: the decompiled C
of all 17 functions already matched the original.

## ABI

All of them are `__stdcall` in the original (`ret 0x24` / `0x28`, `ret 4` for the mask step),
preserve every register and clear CF on return. The C versions are plain cdecl and return
`false` where the header declares `bool`. Argument order:

- plain blits: `(clipMaxY, clipMaxX, clipMinY, clipMinX, drawY, drawX, subresourceIndex, asset, framebuffer)`
- `IntegerScaled`, `PaletteBank`, `Modulated`: the extra argument (scale, bank, ARGB) comes before `subresourceIndex`
- fills: `(clipMaxY, clipMaxX, clipMinY, clipMinX, rectMaxY, rectMaxX, rectMinY, rectMinX, argb, framebuffer)`

The framebuffer (`SoftwareFramebufferAccess`) is `width, height, bytesPerPixel, pixels`, and its row
stride is `width * bytesPerPixel`. Every function first checks `bytesPerPixel` (2 or 4) and
returns if it does not match.

## Asset layout

A texture source asset has the magic `ASSET_MAGIC_GFX` and, at +0xB0, `subresourceCount`,
`paletteBankCount` and `subresourceTableOffset`. The subresource table holds 32-byte
`GraphicsTextureSourceEntry` records: `paletteIndex` +8, `dataOffset` +0xC, `originX/Y`
+0x10/+0x14, `pixelWidth/Height` +0x18/+0x1C. Palette banks are 256 entries of 8 bytes each, at
`asset + 0x200 + bank * 0x800`:

- +0: the ARGB colour.
- +4: the colour converted to 16-bit pixel format, with the alpha in the top byte.

`paletteIndex == -1` means one ARGB dword per texel. Any other value must be below
`paletteBankCount` (compared unsigned), or nothing is drawn.

Palette quirk, kept as it is: the 16-bit blits test the alpha of +4, write its low word when opaque,
and blend +0 using +0's own alpha. The 32-bit blits use +4 for everything: the alpha test, the
blend colour, and the opaque write. The opaque write converts +4 through
`g_SoftwarePixelPackTables` a second time.

## Common structure

Every clipped blit (all except `IntegerScaled`) and both fills follow the same steps:

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
operation. See `SoftwareTextureSource_BlitSourceAlpha16`:

```c
if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 2, drawX, drawY, clipMaxY, clipMaxX,
                           clipMinY, clipMinX, &region)) {
  return false;
}
for (y = 0; y < region.height; y++) {
  const byte *texel = region.texels + y * region.texelStride;
  word *pixel = (word *)(region.pixels + y * region.pixelStride);
  for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
    ... Blit_IsTransparent / Blit_IsOpaque / Blit_BlendArgb16 ...
  }
}
```

## Pixel operations and helpers

The blit helpers are in `software_raster.h`, in the section "Texture-source blits and rectangle
fills". They reuse `RasterColor` and `Raster_MulHigh`.

| helper | MMX it reproduces |
|---|---|
| `Blit_IsTransparent`, `Blit_IsOpaque` | `CMP EAX,0x1000000 / JC` and `CMP EAX,0xFF000000 / JC` on the whole dword |
| `Blit_PaletteColor`, `Blit_PalettePixel` | the palette entry's +0 and +4 dwords |
| `Blit_ConvertArgb` | the `g_SoftwarePixelPackTables` lookup (blue + alpha + green + red, dword adds) |
| `Blit_ArgbLanes(argb, shift)` | `PUNPCKLBW x,x` + `PSRLW shift`: lanes `(c * 0x101) >> shift` |
| `Blit_Unpack16` | broadcast, `PAND` with the pixel masks, `PMULLW` with the unpack scales, `PSRLW 2` |
| `Blit_BlendLanes(src, dst, alpha)` | `PMULHW` with `g_SoftwareBlendAlphaFactors[alpha]` and `...InverseAlphaFactors[alpha]`, then `PADDW` |
| `Blit_PackLanes16` | `PAND` with the quantize masks, `PMADDWD` with the pack weights, then `PSRLQ 8/40` + `PADDW` |
| `Blit_PackLanes32` | `PSRLW 4` (logical) + `PACKUSWB` (the alpha lane is written too) |
| `Blit_BlendArgb16/32` | the source-alpha blend of one ARGB colour over one pixel |

Pixel operations of the other blits, read from their asm (the rewritten code follows them):

- **HalfSourceRgb**: there is no opaque shortcut, so alpha 0xFF also blends (factor index 0xFF).
  The source lanes are `(c * 0x101) >> 3` (half), except in the 32-bit direct-colour path, which
  uses `>> 2`. The destination is `Blit_Unpack16` / `Blit_ArgbLanes(d, 2)`.
- **PaletteBank**: the same as SourceAlpha, but the palette bank argument replaces the entry's bank
  after clipping. It is checked with `bank < paletteBankCount`. The entry's own `paletteIndex`
  must still be valid (or -1).
- **Modulated**: before the alpha tests, each source channel is multiplied by the matching
  modulation channel. Blue is `(b * mb) >> 8`; green, red and alpha are `((c * mc) & 0xFF00)`
  shifted into place. The rest is the same as SourceAlpha.
- **SaturatedAddRgb**: skip the texel if `argb & 0xFFFFFF == 0`.
  - 16-bit: `PADDUSW` of the source lanes `c * 0x101` and the unmasked scaled destination
    `(p & mask) * scale`, then `PSRLW 4`, then `Blit_PackLanes16`'s mask and pack.
  - 32-bit: `PADDUSW` of `c * 0x101` and `d * 0x101`, then `PSRLW 8` + `PACKUSWB`.
- **HalfRgbSaturatedAdd**: the same as SaturatedAddRgb, with the source lanes `>> 1` first.
- **IntegerScaled**: no source clipping. The clip rectangle is clamped to `[0, framebuffer)`.
  Every texel is replicated `scale x scale` times, and each written pixel is tested against the
  clip rectangle. The destination pointer walks the unclipped image, starting at
  `(drawY + originY * scale) * width + drawX + originX * scale`. Scale 0, or an image size of 0,
  makes the original loop 2^32 times, so blitcmp does not generate them.
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

## Remaining work

None: all raster handlers and blits are rewritten and were verified by rastercmp, blitcmp and
blendscalecmp before these self-tests were retired. The per-function pixel operations above describe the rewritten code.
