/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/shaders/ui2d.hlsl
 * Project code (not in the original game)
 */

/* Shaders of the GPU 2D (UI) renderer (src/platform/sdl3/gpu_ui2d.cpp, step 9). They draw the 2D draw list's quads
   (sprites, fills, streamed image regions) into the frame target with the software blits' rules
   (src/graphics/backend/software_blit.cpp); one fragment entry point per blend mode, one pipeline each.

   Vertices come from a vertex buffer (GpuUi2D_Upload copies the frame's vertices into it before the render pass;
   step 9 WP4 replaced the first design, which pushed them as vertex uniform data in 4 KiB chunks: on Direct3D 12
   that drew quads with other quads' corners and dropped runs of glyphs). One vertex is struct GpuUiVertex in
   gpu_ui2d.h, attribute n = TEXCOORDn as SDL_GPU binds them on D3D12 (SPIR-V: location n):
     0 x, y     float2  position in target pixels (top-left origin; pixel centres at +0.5)
     1 u, v     float2  normalized texture coordinates in the page texture
     2 tint     uint    ARGB colour (Modulated: the modulation; Fill: the colour)
     3 flags    uint    GPU_UI_VERTEX_FLAG_* (bit 0: the source is paletted)

   Compiled like primitives.hlsl: fxc to DXBC shader model 5.1 (Direct3D 12) and dxc -spirv (Vulkan). SDL_GPU's
   resource conventions: vertex uniform buffers are (b[n], space1) / descriptor set 1, fragment textures and
   samplers (t0/s0, space2) / set 2 as one combined image sampler at binding 0. */

#define UI2D_FLAG_PALETTED 1u

#ifdef __spirv__
[[vk::binding(0, 1)]] cbuffer Ui2dTarget : register(b0, space1)
#else
cbuffer Ui2dTarget : register(b0, space1)
#endif
{
    float4 g_Target; /* 2 / width, 2 / height, 0, 0 */
};

#ifdef __spirv__
[[vk::combinedImageSampler]] [[vk::binding(0, 2)]] Texture2D<float4> g_Page;
[[vk::combinedImageSampler]] [[vk::binding(0, 2)]] SamplerState g_PageSampler;
#else
Texture2D<float4> g_Page : register(t0, space2);
SamplerState g_PageSampler : register(s0, space2);
#endif

struct Ui2dVertexInput {
    float2 position : TEXCOORD0;
    float2 uv : TEXCOORD1;
    uint tint : TEXCOORD2;
    uint flags : TEXCOORD3;
};

struct Ui2dVaryings {
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    nointerpolation uint tint : TEXCOORD1;
    nointerpolation uint flags : TEXCOORD2;
};

Ui2dVaryings Ui2dVertexMain(Ui2dVertexInput input)
{
    Ui2dVaryings output;
    output.position = float4(input.position.x * g_Target.x - 1.0, 1.0 - input.position.y * g_Target.y, 0.0, 1.0);
    output.uv = input.uv;
    output.tint = input.tint;
    output.flags = input.flags;
    return output;
}

/* ARGB dword -> channels 0..255 in r, g, b, a order. */
uint4 UnpackArgb(uint argb)
{
    return uint4((argb >> 16) & 0xFF, (argb >> 8) & 0xFF, argb & 0xFF, argb >> 24);
}

/* The nearest texel of the page as channels 0..255. */
uint4 PageTexel(float2 uv)
{
    return (uint4)round(g_Page.Sample(g_PageSampler, uv) * 255.0);
}

/* SRC_ALPHA_SKIP0 (BlitSourceAlpha): alpha 0 is skipped, alpha 0xFF writes the colour (the source-alpha blend with
   alpha 1 does), anything else blends. */
float4 Ui2dSkip0Main(Ui2dVaryings input) : SV_Target0
{
    const uint4 texel = PageTexel(input.uv);
    if (texel.a == 0) {
        discard;
    }
    return texel / 255.0;
}

/* HALF_RGB (BlitHalfSourceRgb): alpha 0 is skipped, everything else blends (0xFF included); a paletted source
   enters the blend at half strength ((c * 0x101) >> 3 against >> 2), a direct-colour source at full strength. */
float4 Ui2dHalfRgbMain(Ui2dVaryings input) : SV_Target0
{
    const uint4 texel = PageTexel(input.uv);
    if (texel.a == 0) {
        discard;
    }
    float4 color = texel / 255.0;
    if ((input.flags & UI2D_FLAG_PALETTED) != 0) {
        color.rgb *= 0.5;
    }
    return color;
}

/* MODULATED (BlitModulatedSourceAlpha): every channel, alpha included, becomes (c * m) >> 8 (Blit_Modulate), so
   the alpha is at most 0xFE and every visible texel blends; a modulated alpha of 0 is skipped. */
float4 Ui2dModulatedMain(Ui2dVaryings input) : SV_Target0
{
    const uint4 modulated = (PageTexel(input.uv) * UnpackArgb(input.tint)) >> 8;
    if (modulated.a == 0) {
        discard;
    }
    return modulated / 255.0;
}

/* OPAQUE: the texel's colour, written without a blend (streamed CPU image regions). */
float4 Ui2dOpaqueMain(Ui2dVaryings input) : SV_Target0
{
    const uint4 texel = PageTexel(input.uv);
    return float4(texel.rgb / 255.0, 1.0);
}

/* FILL (FillRectArgb): the tint, no texture; alpha 0 draws nothing, 0xFF writes, anything else blends. */
float4 Ui2dFillMain(Ui2dVaryings input) : SV_Target0
{
    const uint4 color = UnpackArgb(input.tint);
    if (color.a == 0) {
        discard;
    }
    return color / 255.0;
}
