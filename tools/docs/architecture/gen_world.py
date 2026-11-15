"""Draws docs/architecture/world/world.svg, the block diagram of the world area.

Run from the repository root: python tools/docs/architecture/gen_world.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

W, H = 1280, 830
d = A.Diagram(W, H, 'world — entities, camera, terrain, pathing, models, effects, shots',
              'Modules under src/world/ (headers in include/thandor/world/). Arrows show the main calls; all modules use '
              'core fixed-point maths. Click a box for its source folder or area page.')

SRC = '../../../src/world/'


def mod(x, y, w, h, name, desc, chips, meta):
    free = d.box(x, y, w, h, name, desc, chips, href=SRC + name + '/', meta=meta, color='world')
    assert free <= y + h, f'{name}: content ends at {free}, box ends at {y + h}'


def ext(x, y, w, h, key, label=None):
    d.ext(x, y, w, h, label or key, href=f'../{key}/README.md', color=key)


# other areas
ext(60, 80, 1160, 40, 'gameplay', 'gameplay — session tick, armies, AI, move orders, weapons')
ext(980, 370, 110, 40, 'audio')
ext(1110, 370, 110, 40, 'graphics')
ext(60, 760, 240, 40, 'network', 'network — lockstep commands')
ext(340, 760, 260, 40, 'assets', 'assets — PCK packages')
ext(640, 760, 580, 40, 'ui', 'ui — world input, camera commands, map editor')

# world modules
mod(60, 170, 280, 320, 'pathing', ['Scratch grid derived from the', 'field grid; influence bands,', 'hex path costs, reachability',
                                   'and entity routes for move', 'orders. Session pathing mode:',
                                   'original or improved (yield,', 'side step, arrival, formation).'],
    ['route.cpp', 'path_cost.cpp', 'reachability.cpp', 'influence.cpp', 'scratch_grid.cpp', 'mode.cpp',
     'improved/yield.cpp', 'improved/arrival.cpp'], '8 files')
mod(380, 170, 250, 140, 'model', ['Model runtime pool, node', 'hierarchy, picking, ray hits.'],
    ['pool.cpp', 'hierarchy.cpp', 'picking.cpp', 'mesh_raycast.cpp'], '4 files')
mod(680, 170, 250, 140, 'shots', ['Projectile pool, flight and', 'hits; launch angles.'],
    ['pool.cpp', 'flight.cpp', 'ballistics.cpp'], '3 files')
mod(980, 170, 240, 140, 'effects', ['Effect pool and lifecycle:', 'animation, fades, motion.'],
    ['pool.cpp', 'lifecycle.cpp'], '2 files')
mod(380, 370, 550, 120, 'runtime', ['World context: owner list of models, shots and effects,',
                                    'world object records, attached field grid.'],
    ['core.cpp', 'entity_registry.cpp'], '2 files')
mod(60, 540, 760, 160, 'terrain', ['Field grid: load and save, height sampling, raycasts, lighting, deformation,',
                                   'water relaxation, occupancy, sight, placement tests, map-editor edits.'],
    ['field_lifecycle.cpp', 'field_sampling.cpp', 'field_raycast.cpp', 'field_lighting.cpp', 'field_deformation.cpp',
     'field_edit_commands.cpp', 'editing.cpp', 'water_relaxation.cpp', 'occupancy.cpp', 'sight.cpp',
     'placement_tests.cpp', 'fog_visibility.cpp', 'overlay_marking.cpp', 'hex_scan.cpp'], '14 files')
mod(860, 540, 360, 160, 'camera', ['Camera motion state (position, target,', 'angles), input deltas, snapshots,',
                                   'menu-room motion splines.'],
    ['camera.cpp', 'camera_input.cpp', 'motion_spline.cpp'], '3 files')

# gameplay -> world (top band)
d.line([(200, 120), (200, 170)], 'move orders', 207, 150)
d.line([(505, 120), (505, 170)], 'create · transforms', 512, 150)
d.line([(740, 120), (740, 170)], 'fire · tick', 747, 150)
d.line([(880, 170), (880, 120)], 'hit damage', 873, 150, anchor='end')
d.line([(1100, 120), (1100, 170)], 'spawn · tick', 1107, 150)
d.line([(60, 100), (36, 100), (36, 620), (60, 620)], 'tick jobs: lighting, relaxation, occupancy', 30, 360,
       anchor='middle', rotate=True)

# inside world
d.line([(680, 240), (630, 240)], 'hit rays', 655, 232, anchor='middle')
d.line([(930, 240), (980, 240)], 'impact', 955, 232, both=True, anchor='middle')
d.text(955, 258, 'linked', 'l', anchor='middle')
d.line([(505, 310), (505, 370)], 'alloc records', 512, 344)
d.line([(805, 310), (805, 370)], 'link · unlink', 812, 344)
d.line([(200, 490), (200, 540)], 'classify field cells', 207, 520)
d.line([(860, 620), (820, 620)], 'raycast', 840, 612, anchor='middle')

# world -> output areas
d.line([(1035, 310), (1035, 370)], 'sounds', 1030, 344, anchor='end')
d.line([(1165, 310), (1165, 370)], 'shading', 1172, 344)
d.line([(780, 540), (780, 515), (1165, 515), (1165, 410)], 'field grid (terrain renderer)', 1000, 508, anchor='middle')

# bottom areas
d.line([(180, 760), (180, 700)], 'terrain edit commands', 187, 734)
d.line([(470, 700), (470, 760)], 'load .fld', 477, 734)
d.line([(720, 760), (720, 700)], 'map editor', 727, 734)
d.line([(1040, 760), (1040, 700)], 'pointer · keys · camera commands', 1047, 734)

d.save('docs/architecture/world/world.svg')

if __name__ == '__main__' and '--png' in sys.argv:
    A.render_png('docs/architecture/world/world.svg', sys.argv[sys.argv.index('--png') + 1], W, H)
