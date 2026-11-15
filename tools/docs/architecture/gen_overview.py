"""Draws docs/architecture/overview.svg, the top-level block diagram.

Run from the repository root: python tools/docs/architecture/gen_overview.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

d = A.Diagram(1280, 760, 'open-thandor — architecture overview',
              'Top-level areas under src/ (public headers in include/thandor/). Arrows show the main runtime flows; '
              'every area also uses core and platform services. Click an area for its page.')

for y, t in ((84, 'PRESENTATION'), (224, 'GAME LOGIC'), (374, 'OUTPUT · COMMUNICATION'),
             (544, 'PLATFORM'), (690, 'OUTSIDE')):
    d.band(966, y, t, anchor='end')
d.band(1240, 84, 'FOUNDATION · USED BY ALL', anchor='end')


def area(x, y, w, h, key, desc, mods, meta):
    d.box(x, y, w, h, key, desc, mods, href=f'{key}/README.md', meta=meta, color=key)


area(60, 92, 940, 100, 'ui', ['Front-end menus, in-game HUD and world input, dialogs and the control framework.',
                              'Campaign map, custom maps tab, options (language, key bindings). Runs the per-frame loop: dispatch input, update, draw.'],
     ['frontend', 'ingame', 'dialogs', 'controls', 'core', 'text'], '95 files · 53k lines')
area(60, 232, 450, 118, 'gameplay', ['Session lifecycle (new, load, save, campaign progress) and the',
                                     'simulation tick: armies, AI, factions, research; game recorder.'],
     ['session', 'army', 'ai', 'faction', 'selection', 'technology', 'replay'], '63 files · 32k lines')
area(550, 232, 410, 118, 'world', ['Entities, camera, terrain, pathfinding', '(original or improved), 3D models, effects, shots.'],
     ['runtime', 'camera', 'terrain', 'pathing', 'model', 'effects', 'shots'], '36 files · 15k lines')
area(60, 382, 210, 118, 'network', ['Multiplayer transport,', 'session and transfer protocol.'],
     ['protocol', 'backend'], '10 files')
area(290, 382, 180, 118, 'audio', ['Positioned sound,', 'sample codec, mixer slots.'],
     ['spatial', 'codec', 'backend'], '5 files')
area(490, 382, 150, 118, 'movie', ['FLM playback', 'and encoding.'], ['runtime'], '3 files')
area(660, 382, 340, 118, 'graphics', ['Render pipeline, terrain renderer, textures,', 'palettes; software renderer.'],
     ['render', 'terrain', 'resources', 'core', 'backend'], '31 files · 14k lines')
area(1030, 92, 210, 230, 'assets', ['Game data: PCK packages,', 'level catalogs for armies,', 'models, shots, effects,',
                                    'sprites; menu room, scenarios', 'and custom maps, texts.'],
     ['package', 'rom', 'army', 'model', 'effect', 'shot', 'sprite', 'scenario', 'text'], '20 files')
area(1030, 342, 210, 158, 'core', ['Fixed-point maths, memory,', 'settings (thandor.ini) and the', 'Original Thandor switch,', 'errors, strings.'],
     ['math', 'memory', 'settings', 'error', 'text'], '14 files')
area(60, 552, 1180, 100, 'platform', ['Start-up and event pump; SDL3 layer: window, SDL GPU renderer (Vulkan / D3D12), '
                                      'audio device, input, timer;', 'file system, system services, debug tools (state hash, path trace, profiler) and self-tests.'],
     ['bootstrap', 'sdl3', 'input', 'filesystem', 'system', 'debug', 'selftest'], '69 files · 29k lines')

for x, w, t in ((60, 200, 'Keyboard · mouse (Steam Input)'), (300, 170, 'Display'), (500, 170, 'Sound device'),
                (700, 170, 'LAN peers'), (1000, 240, 'Game files: PCK · ini · saves · maps')):
    d.ext(x, 700, w, 40, t)

# outside <-> platform
d.line([(160, 700), (160, 652)], 'raw events', 167, 680)
d.line([(385, 652), (385, 700)], 'frames', 392, 680)
d.line([(585, 652), (585, 700)], 'audio', 592, 680)
d.line([(785, 700), (785, 652)], 'UDP', 792, 680, both=True)
d.line([(1120, 700), (1120, 652)], 'read / write', 1127, 680, both=True)
# platform -> ui (left gutter)
d.line([(60, 600), (36, 600), (36, 142), (60, 142)], 'input events', 30, 380, anchor='middle', rotate=True)
d.line([(280, 192), (280, 232)], 'player commands', 287, 216)
d.line([(982, 192), (982, 382)], 'menus · HUD', 976, 290, anchor='middle', rotate=True)
d.line([(510, 291), (550, 291)], 'tick', 530, 284, anchor='middle')
d.line([(165, 350), (165, 382)], 'lockstep commands', 172, 370, both=True)
d.line([(380, 350), (380, 382)], 'sounds', 387, 370)
d.line([(700, 350), (700, 382)], 'scene', 707, 370)
# output -> platform
d.line([(165, 500), (165, 552)], 'packets', 172, 530, both=True)
d.line([(380, 500), (380, 552)], 'mix', 387, 530)
d.line([(565, 500), (565, 552)], 'frames', 572, 530)
d.line([(830, 500), (830, 552)], 'draw calls', 837, 530)
d.line([(1135, 552), (1135, 500)], 'file access', 1142, 530)

d.save('docs/architecture/overview.svg')
