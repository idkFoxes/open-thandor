"""Draws docs/architecture/ui/ui.svg, the block diagram of the ui area.

Run from the repository root: python tools/docs/architecture/gen_ui.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

W, H = 1280, 870
d = A.Diagram(W, H, 'ui — modules and flows',
              'Modules under src/ui/ (headers in include/thandor/ui/). Solid boxes are ui modules, dashed boxes other '
              'areas. Arrows show the main calls and data, not every include. Click a box.')

SRC = '../../../src/ui/'
failed = []


def mod(x, y, w, h, name, desc, chips, meta):
    free = d.box(x, y, w, h, name, desc, chips, href=f'{SRC}{name}/', meta=meta, color='ui')
    if free > y + h:
        failed.append((name, free, y + h))


def area(x, y, w, h, key, lines=()):
    d.ext(x, y, w, h, key if not lines else '', href=f'../{key}/README.md', color=key)
    if lines:
        top = y + h / 2 - (len(lines) * 15) / 2 - 4
        d.text(x + w / 2, top, key, 't', 'middle')
        for i, s in enumerate(lines):
            d.text(x + w / 2, top + 20 + i * 15, s, 'f', 'middle')


# ---- ui modules -------------------------------------------------------------
mod(290, 150, 340, 135, 'core', ['Frame loop (events, ticks, actions, draw, present),',
                                 'action queue and handler pages, frame lock.'],
    ['frame_loop', 'runtime', 'chat_history', 'model_tint', 'pcx_preview'], '5 files')
mod(650, 150, 160, 135, 'dialogs', ['Modal roots:', 'display mode,', 'fatal error.'],
    ['display_settings', 'fatal_error'], '2 files')
mod(830, 150, 160, 135, 'text', ['Fonts, rich-text', 'command streams:', 'measure, wrap, draw.'],
    ['font', 'richtext_render'], '2 files')
mod(290, 340, 700, 130, 'controls', ['Control framework: node vtables, root stack, pointer capture and keyboard focus, '
                                     'input dispatch, widgets.'],
    ['root_stack', 'input', 'container', 'window', 'panels', 'image', 'buttons', 'text_buttons', 'text',
     'text_edit', 'lists', 'scrollable', 'selectable', 'slider', 'gauge', 'tooltip', 'focus_proxy', 'minimap'],
    '18 files · 11k lines')
mod(290, 530, 340, 218, 'frontend', ['Main-menu state machine and pages; the 3D world', 'view control (menu room, also in-game).'],
    ['main_loop', 'lifecycle', 'state', 'menu_room', 'menu_room_scene', 'session', 'network', 'player',
     'scenario_selection', 'task_assignment', 'mission_briefing', 'results', 'settings', 'end_movie',
     'credits', 'ui_template'], '23 files · 14k lines')
mod(650, 530, 340, 218, 'ingame', ['HUD, pages and panels of a running session, world', 'input, hotkeys, map editor tools.'],
    ['root_frame', 'world_input', 'commands', 'hotkeys', 'key_commands', 'hud', 'layout', 'pages',
     'build_catalog', 'army_stock', 'selection_detail', 'selection_overlay', 'minimap_texture',
     'savegame_page', 'technology', 'editor_tools', 'ui_template'], '32 files · 19k lines')

# ---- other areas ------------------------------------------------------------
area(40, 150, 160, 300, 'platform', ['SDL3 event pump,', 'keyboard queue,', 'timers, bootstrap'])
area(90, 545, 110, 40, 'audio')
area(90, 620, 110, 40, 'movie')
area(1060, 340, 140, 130, 'graphics', ['framebuffer, 2D blits,', 'cursor event ring,', 'render passes'])
area(1060, 545, 140, 50, 'gameplay')
area(1060, 615, 140, 40, 'network')
area(1060, 680, 140, 40, 'world')
d.ext(290, 800, 340, 40, 'assets · texts, ROM records, catalogs (used by all)', href='../assets/README.md',
      color='assets')
d.ext(650, 800, 340, 40, 'core · settings (thandor.ini), memory (used by all)', href='../core/README.md',
      color='core')

# ---- arrows -----------------------------------------------------------------
# platform -> ui
d.line([(200, 200), (290, 200)], 'pump · ticks', 207, 193)
d.line([(200, 405), (290, 405)], 'key events', 207, 398)
d.line([(60, 450), (60, 715), (290, 715)], 'Frontend_MainLoop', 54, 590, anchor='middle', rotate=True)
# core <-> controls, core -> dialogs/text are drawn by the root walk
d.line([(460, 285), (460, 340)], 'dispatch input · draw roots', 467, 318)
d.line([(560, 340), (560, 285)], 'queued actions', 567, 300)
# controls <-> graphics, text -> graphics
d.line([(990, 380), (1060, 380)], 'blits', 997, 373)
d.line([(1060, 430), (990, 430)], 'pointer ev.', 997, 423)
d.line([(990, 215), (1130, 215), (1130, 340)], 'glyph blits', 997, 208)
# frontend world view -> graphics (through the gap between controls and the session row)
d.line([(560, 530), (560, 500), (1100, 500), (1100, 470)], 'world view render passes', 660, 494)
# frontend -> audio, movie
d.line([(290, 565), (200, 565)], 'menu music', 207, 558)
d.line([(290, 640), (200, 640)], 'end movie', 207, 633)
# ingame <-> right column
d.line([(990, 560), (1060, 560)], 'commands', 995, 553)
d.line([(1060, 580), (990, 580)], 'builds root', 995, 598)
d.line([(990, 635), (1060, 635)], 'cmd queue', 995, 628)
d.line([(990, 700), (1060, 700)], 'camera', 995, 693)
# core <-> gameplay over the top: the session loop presents frames, the frame lock release runs the sim tick
d.line([(380, 150), (380, 125), (1220, 125), (1220, 555), (1200, 555)],
       'session loop: UiFrame_ProcessAndPresent  ·  frame-lock release hook: simulation step', 392, 119, both=True)
# frontend -> gameplay, frontend <-> network around the bottom
d.line([(450, 748), (450, 780), (1260, 780), (1260, 585), (1200, 585)],
       'InGameRuntime_RunSessionUntilExit', 660, 794)
d.line([(520, 748), (520, 765), (1240, 765), (1240, 645), (1200, 645)], 'lobby · transfers', 660, 760, both=True)

d.save('docs/architecture/ui/ui.svg')
if failed:
    print('boxes too small:', failed)

if len(sys.argv) > 1:
    A.render_png('docs/architecture/ui/ui.svg', sys.argv[1], W, H)
