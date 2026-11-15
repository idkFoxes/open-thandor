"""Draws docs/architecture/graphics/graphics.svg, the block diagram of the graphics area.

Run from the repository root: python tools/docs/architecture/gen_graphics.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

W, H = 1280, 850
d = A.Diagram(W, H, 'graphics — render pipeline, terrain, textures, software renderer',
              'Modules under src/graphics/ (headers in include/thandor/graphics/). Solid boxes are modules of this area, '
              'dashed boxes other areas. Arrows show calls and data; slot = function pointer the backend can swap.')

SRC = '../../../src/graphics/'


def mod(x, y, w, h, key, desc, chips, meta):
    free = d.box(x, y, w, h, key, desc, chips, href=SRC + key + '/', meta=meta, color='graphics')
    assert free <= y + h, f'{key}: content ends at {free}, box ends at {y + h}'


# ---- callers (row 1)
d.ext(40, 76, 240, 44, 'gameplay (level, army, effects)', href='../gameplay/README.md', color='gameplay')
d.ext(310, 76, 930, 44, 'ui — world view (FrontendModelPointerContext_RenderWorldViewQueuesClipped), HUD, menus',
      href='../ui/README.md', color='ui')

# ---- graphics modules (row 2)
R2, H2 = 196, 216
mod(40, R2, 240, H2, 'resources',
    ['Loads and owns gfx texture sources,', 'texture sets and pal palettes; PCX',
     'read/write; tiled blits; framebuffer', 'globals and access hooks.'],
    ['texture_set', 'texture_source', 'palette', 'pcx', 'tiled_blit', 'framebuffer', 'palette_optimizer',
     'texture_decompose'], '9 files')
mod(310, R2, 210, H2, 'terrain',
    ['Terrain pass: clips the field grid', 'rows to the frustum, projects and',
     'shades vertices, queues two', 'triangles per grid quad. Loads the', 'material texture sets and palettes.'],
    ['terrain_render', 'terrain_resources'], '2 files')
mod(550, R2, 350, H2, 'render',
    ['Projection and frustum, model hierarchies (cull,', 'transform, light, submit), dynamic light records,',
     'the primitive queue with its radix sort,', 'generated shadow textures, off-screen model',
     'rendering (army preview).'],
    ['projection', 'model_draw', 'model_submit', 'model_lighting', 'light_records', 'light_transitions',
     'primitives', 'shadow_texture', 'offscreen'], '9 files')
mod(930, R2, 310, H2, 'core',
    ['2D draw list and its backend switch', '(software / GPU record / compare): installs',
     'the blit and fill slots; graphics tables,', 'display framebuffer publish, shutdown;',
     'cursor animation and input event ring.'],
    ['draw2d', 'device', 'cursor'], '3 files')

# ---- row 3: assets, world (data), software backend
B = R2 + H2           # bottom of row 2
R3 = B + 64
d.ext(40, R3, 240, 44, 'assets (Package_LoadEntry)', href='../assets/README.md', color='assets')
d.ext(310, R3 + 40, 290, 44, 'world (field grid, model nodes, lights)', href='../world/README.md', color='world')
mod(650, R3, 500, 156, 'backend',
    ['Software renderer: 40 triangle handlers over the sorted queue into the',
     'framebuffer and depth buffer; 32-bit blits and fills; bilinear scaling,',
     'minimap; depth buffer on mode switch; display-mode table checks.'],
    ['software_rasterizer', 'software_blit', 'software_texture_scale', 'software_display_mode',
     'display_modes', 'software'], '6 files')

# ---- row 4/5: platform and display
R4 = R3 + 204
R5 = R4 + 98
d.ext(650, R4, 270, 48, 'platform/sdl3 video.cpp — SdlVideo_Present', href='../platform/README.md', color='platform')
d.ext(960, R4, 190, 48, 'Display (window)')
d.ext(310, R5, 930, 48, 'platform/sdl3 SDL GPU renderer (Vulkan / D3D12): gpu_renderer.cpp, gpu_ui2d.cpp, '
      'gpu_ui_textures.cpp', href='../platform/README.md', color='platform')

# notes in the free lower-left corner
for i, s in enumerate(['Renderer selection (video.cpp): the saved renderer',
                       'or OPEN_THANDOR_GPU. StartGpuDevice swaps the 3D slots',
                       'g_GraphicsSetViewportAndClearDepth, ...DrawPrimitiveQueue',
                       'and ...EndScene and sets the draw list to GPU record;',
                       'StopGpuDevice puts the software ones back.']):
    d.text(40, R3 + 132 + i * 16, s, cls='f')

# ---- arrows: callers into graphics
d.line([(160, 120), (160, R2)], 'gfx, palette loads', 167, 160)
d.line([(415, 120), (415, R2)], 'terrain pass', 422, 150)
d.line([(700, 120), (700, R2)], 'projection, model passes,', 707, 146)
d.text(707, 160, 'sort + draw the queue', cls='l')
d.line([(1085, 120), (1085, R2)], 'blits, fills, minimap', 1092, 160)

# inside graphics (labels below the boxes: the gaps are narrow)
d.line([(310, B - 12), (280, B - 12)])
d.text(295, B + 12, 'loads', cls='l', anchor='middle')  # label under the gap
d.line([(520, B - 12), (550, B - 12)])
d.text(535, B + 12, 'queue', cls='l', anchor='middle')
d.line([(160, B), (160, R3)], 'Package_LoadEntry', 167, B + 36)

# world data
d.line([(415, R3 + 40), (415, B)], 'field grid', 422, B + 30)
d.line([(580, R3 + 40), (580, B)], 'model nodes, light records', 573, B + 52, anchor='end')

# to the backends
d.line([(800, B), (800, R3)], 'slot: DrawPrimitiveQueue', 807, B + 36)
d.line([(1000, B), (1000, R3)], 'slots: software blitters', 1007, B + 36)
d.line([(620, B), (620, R5)], 'slot: GpuRenderer_DrawPrimitiveQueue', 612, (B + R5) // 2 + 60, anchor='middle', rotate=True)
d.line([(1210, B), (1210, R5)], 'Draw2D_FrameItems (GPU record)', 1202, (B + R5) // 2, anchor='middle', rotate=True)

# presentation
d.line([(800, R3 + 156), (800, R4)], 'framebuffer pixels', 807, R3 + 182)
d.line([(920, R4 + 24), (960, R4 + 24)])
d.text(940, R4 - 8, 'SDL_Renderer', cls='l', anchor='middle')
d.line([(760, R4 + 48), (760, R5)], 'PresentGpuFrame', 767, R4 + 78)
d.line([(1055, R5), (1055, R4 + 48)], 'swapchain', 1062, R4 + 78)

d.save('docs/architecture/graphics/graphics.svg')

