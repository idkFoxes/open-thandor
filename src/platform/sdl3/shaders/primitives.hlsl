/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/shaders/primitives.hlsl
 * Project code (not in the original game)
 */

/* Shaders of the SDL_GPU primitive renderer (src/platform/sdl3/gpu_renderer.cpp, CMake option
   THANDOR_RENDERER_SDL_GPU). They rasterize the software renderer's primitive packets: the CPU has already lit,
   projected, clipped and sorted them, so the shaders only interpolate and sample.

   Vertex input (one 32-byte vertex per packet corner, attribute n = TEXCOORDn as SDL_GPU binds them on D3D12):
     0 position  float3  x/y in normalized device coordinates of the triangle the software rasterizer would draw
                         (rebuilt on the CPU in its sample space, see gpu_renderer.cpp), z = depth / 2^32
     1 texel     float2  U/V in texels of the packet's texture (Q12 / 4096), not wrapped
     2 colour    unorm4  the packet's ARGB diffuse colour as stored in memory (B, G, R, A)
     3 atlas     uint4   the texture's origin in the atlas and its wrap masks (width - 1, height - 1); a width mask
                         of 0xFFFF marks an untextured packet
   w is 1 everywhere, so every attribute is interpolated affinely in screen space like the software rasterizer.

   Compiled twice: with fxc to DXBC shader model 5.1 (SDL_GPU_SHADERFORMAT_DXBC, the Direct3D 12 backend) and with
   dxc -spirv to SPIR-V (SDL_GPU_SHADERFORMAT_SPIRV, the Vulkan backend). SDL_GPU's resource conventions: the
   fragment stage's sampled textures are register space 2 on D3D12 and descriptor set 2 on Vulkan, where SDL binds
   each texture + sampler pair as one combined image sampler at binding n; dxc turns the register space into the
   set, and [[vk::combinedImageSampler]] merges texture and sampler into that one binding. The stage inputs and
   outputs get their Vulkan locations in declaration order (attribute n = location n). */

struct VertexInput {
    float3 position : TEXCOORD0;
    float2 texel : TEXCOORD1;
    float4 colorBgra : TEXCOORD2;
    uint4 atlas : TEXCOORD3;
};

struct VertexOutput {
    float4 position : SV_Position;
    float2 texel : TEXCOORD0;
    float4 color : TEXCOORD1;
    nointerpolation uint4 atlas : TEXCOORD2;
};

#ifdef __spirv__
[[vk::combinedImageSampler]] [[vk::binding(0, 2)]] Texture2D<float4> g_Atlas;
[[vk::combinedImageSampler]] [[vk::binding(0, 2)]] SamplerState g_AtlasSampler;
#else
Texture2D<float4> g_Atlas : register(t0, space2);
SamplerState g_AtlasSampler : register(s0, space2);
#endif

VertexOutput VertexMain(VertexInput input)
{
    VertexOutput output;
    output.position = float4(input.position, 1.0);
    output.texel = input.texel;
    output.color = input.colorBgra.zyxw;
    output.atlas = input.atlas;
    return output;
}

/* The shaded colour times the nearest texel (wrapped by the texture size), as the textured modes do; the
   untextured modes use the colour alone. */
float4 ShadeFragment(VertexOutput input)
{
    float4 color = input.color;
    if (input.atlas.z != 0xFFFF) {
        int2 texel = int2(floor(input.texel));
        uint x = input.atlas.x + ((uint)texel.x & input.atlas.z);
        uint y = input.atlas.y + ((uint)texel.y & input.atlas.w);
#ifdef __spirv__
        /* the combined image sampler has to be sampled (a Load would leave the sampler unused): the nearest texel
           at its centre is the same texel */
        color *= g_Atlas.SampleLevel(g_AtlasSampler, (float2(x, y) + 0.5) / 4096.0, 0);
#else
        color *= g_Atlas.Load(int3(x, y, 0));
#endif
    }
    return color;
}

float4 FragmentMain(VertexOutput input) : SV_Target0
{
    return ShadeFragment(input);
}

/* Depth pass of the alpha-blended modes that write depth (4/6/12/14, 20/22/28/30): the depth is written where
   the modulated alpha is at least 128 (Raster_AlphaWritesDepth); the colour write mask is 0. */
float4 FragmentAlphaTestMain(VertexOutput input) : SV_Target0
{
    float4 color = ShadeFragment(input);
    if (color.a * 255.0 < 127.5) {
        discard;
    }
    return color;
}
