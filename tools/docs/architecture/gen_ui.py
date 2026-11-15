"""Draws docs/architecture/ui/ui.svg, the block diagram of the ui area.

Run from the repository root: python tools/docs/architecture/gen_ui.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

W, H = 1280, 940
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
mod(290, 150, 340, 170, 'core', ['Frame loop, action queue, frame lock; key bindings',
                                 '([keys] in thandor.ini) and the shared bindings panel.'],
    ['frame_loop', 'runtime', 'key_bindings', 'key_bindings_ini', 'key_bindings_panel', 'key_binding_tables',
     'chat_history', 'model_tint', 'pcx_preview'], '9 files · 3.3k lines')
mod(650, 150, 160, 170, 'dialogs', ['Modal roots:', 'display mode,', 'fatal error.'],
    ['display_settings', 'fatal_error'], '2 files')
mod(830, 150, 160, 170, 'text', ['Fonts, rich text;', 'HD font for the', 'GPU renderer.'],
    ['font', 'richtext_render', 'hd_font'], '3 files')
mod(290, 360, 700, 130, 'controls', ['Control framework: node vtables, root stack, pointer capture and keyboard focus, '
                                     'input dispatch, widgets.'],
    ['root_stack', 'input', 'container', 'window', 'panels', 'image', 'buttons', 'text_buttons', 'text',
     'text_edit', 'lists', 'scrollable', 'selectable', 'slider', 'gauge', 'tooltip', 'focus_proxy', 'minimap'],
    '18 files · 11k lines')
mod(290, 540, 340, 270, 'frontend', ['Main-menu state machine and pages (campaign map,', 'Eigene Karten tab, options); the 3D world view.'],
    ['main_loop', 'lifecycle', 'state', 'menu_room', 'menu_room_scene', 'session', 'network', 'player',
     'scenario_selection', 'campaign_tab', 'campaign_map_control', 'campaign_graph', 'campaign_level_info',
     'task_assignment', 'mission_briefing', 'results', 'display_settings', 'language_choice',
     'key_bindings_page', 'end_movie', 'ui_template'], '29 files · 17k lines')
mod(650, 540, 340, 270, 'ingame', ['HUD, pages and panels of a running session, world', 'input, hotkeys, objective overlay, editor tools.'],
    ['root_frame', 'world_input', 'commands', 'hotkeys', 'key_commands', 'hud', 'layout', 'pages',
     'build_catalog', 'army_stock', 'selection_detail', 'selection_overlay', 'minimap_texture',
     'savegame_page', 'technology', 'objective_overlay', 'key_bindings_overlay', 'editor_tools',
     'ui_template'], '34 files · 20k lines')

# ---- other areas ------------------------------------------------------------
area(40, 150, 160, 340, 'platform', ['SDL3 event pump,', 'keyboard queue,', 'timers, bootstrap'])
area(90, 565, 110, 40, 'audio')
area(90, 640, 110, 40, 'movie')
area(1060, 360, 140, 130, 'graphics', ['framebuffer, 2D blits,', 'cursor event ring,', 'render passes'])
area(1060, 565, 140, 50, 'gameplay')
area(1060, 635, 140, 40, 'network')
area(1060, 700, 140, 40, 'world')
d.ext(290, 870, 340, 40, 'assets · texts, ROM records, catalogs (used by all)', href='../assets/README.md',
      color='assets')
d.ext(650, 870, 340, 40, 'core · settings (thandor.ini), memory (used by all)', href='../core/README.md',
      color='core')

# ---- arrows -----------------------------------------------------------------
# platform -> ui
d.line([(200, 200), (290, 200)], 'pump · ticks', 207, 193)
d.line([(200, 425), (290, 425)], 'key events', 207, 418)
d.line([(60, 490), (60, 780), (290, 780)], 'Frontend_MainLoop', 54, 635, anchor='middle', rotate=True)
# core <-> controls, core -> dialogs/text are drawn by the root walk
d.line([(460, 320), (460, 360)], 'dispatch input · draw roots', 467, 345)
d.line([(560, 360), (560, 320)], 'queued actions', 567, 335)
# controls <-> graphics, text -> graphics
d.line([(990, 400), (1060, 400)], 'blits', 997, 393)
d.line([(1060, 450), (990, 450)], 'pointer ev.', 997, 443)
d.line([(990, 215), (1130, 215), (1130, 360)], 'glyph blits', 997, 208)
# frontend world view -> graphics (through the gap between controls and the session row)
d.line([(560, 540), (560, 515), (1100, 515), (1100, 490)], 'world view render passes', 660, 509)
# frontend -> assets: custom maps list, campaign level info from the packages
d.line([(320, 810), (320, 870)], 'custom maps · level info', 313, 850, anchor='end')
# frontend -> audio, movie
d.line([(290, 585), (200, 585)], 'menu music', 207, 578)
d.line([(290, 660), (200, 660)], 'end movie', 207, 653)
# ingame <-> right column
d.line([(990, 580), (1060, 580)], 'cmds · goals', 995, 573)
d.line([(1060, 600), (990, 600)], 'builds root', 995, 618)
d.line([(990, 655), (1060, 655)], 'cmd queue', 995, 648)
d.line([(990, 720), (1060, 720)], 'camera', 995, 713)
# core <-> gameplay over the top: the session loop presents frames, the frame lock release runs the sim tick
d.line([(380, 150), (380, 125), (1220, 125), (1220, 575), (1200, 575)],
       'session loop: UiFrame_ProcessAndPresent  ·  frame-lock release hook: simulation step', 392, 119, both=True)
# frontend -> gameplay, frontend <-> network around the bottom
d.line([(450, 810), (450, 845), (1260, 845), (1260, 605), (1200, 605)],
       'InGameRuntime_RunSessionUntilExit  ·  campaign progress', 660, 859)
d.line([(520, 810), (520, 828), (1240, 828), (1240, 665), (1200, 665)], 'lobby · transfers', 660, 823, both=True)

d.save('docs/architecture/ui/ui.svg')
if failed:
    print('boxes too small:', failed)

if len(sys.argv) > 1:
    A.render_png('docs/architecture/ui/ui.svg', sys.argv[1], W, H)
