"""Draws docs/architecture/gameplay/gameplay.svg, the block diagram of the gameplay area.

Run from the repository root: python tools/docs/architecture/gen_gameplay.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

W, H = 1280, 820
OUT = 'docs/architecture/gameplay/gameplay.svg'
SRC = '../../../src/gameplay/'

d = A.Diagram(W, H, 'gameplay — session, simulation tick, armies, AI, factions, recorder',
              'Modules under src/gameplay/ (headers in include/thandor/gameplay/). Arrows are the main calls and data '
              'flows read from the code; dashed boxes are other areas. Click a box for its source folder or page.')

free = {}


def mod(name, x, y, w, h, desc, chips, meta):
    free[name] = (d.box(x, y, w, h, name, desc, chips, href=f'{SRC}{name}/', meta=meta, color='gameplay'), y + h)


def area(key, x, y, w, h, text):
    d.ext(x, y, w, h, text, href=f'../{key}/README.md', color=key)


# other areas: top row and the column right of session
area('ui', 80, 76, 480, 44, 'ui — frontend main loop, in-game HUD, world input')
area('network', 720, 76, 500, 44, 'network — command queue and lockstep exchange')
area('assets', 920, 160, 300, 28, 'assets — PCK level, army, model, tec files')
area('movie', 920, 194, 300, 28, 'movie — loading and end movies')
area('graphics', 920, 228, 300, 28, 'graphics — terrain textures, shading')
area('platform', 920, 262, 300, 28, 'platform — 80 Hz timer, game data, debug hooks')

# session
mod('session', 80, 160, 820, 150,
    ['Session lifecycle: start a new level or a saved game, run frames until exit, save, campaign carry-over.',
     'tick.cpp: one simulation step = lockstep, per-entity update, level script every 20 steps, 8 world jobs.',
     'Project additions: autosave, campaign progress (campaign map), mission objective for the HUD overlay.'],
    ['startup', 'new_session', 'loaded_session', 'level_new', 'level_saved', 'level', 'level_script', 'tick',
     'savegame', 'savegame_load', 'scenario_load', 'campaign_carryover', 'loading_movie', 'autosave',
     'campaign_progress*', 'campaign_start', 'mission_objective'],
    '19 files · 8.8k lines')

# middle row
mod('ai', 80, 360, 300, 190,
    ['Computer factions: per-faction planning', 'every 64 steps (unit or economy phase),',
     'per-army target choice.'],
    ['planning', 'perception', 'workspaces', 'units', 'combat', 'construction', 'purchasing', 'placement',
     'technology'],
    '9 files · 5.2k')
mod('army', 450, 360, 380, 190,
    ['Units and buildings: creation and pool, per-class', 'update tables, movement (original or improved),',
     'weapons and damage, placement, factories.'],
    ['pool', 'factory', 'class_dispatch', 'class_updates', 'move_orders', 'drive_*', 'walker', 'aircraft',
     'weapons', 'turrets', 'combat', 'damage', 'collision', 'placement*', 'preview', 'model_*', 'audio'],
    '23 files · 11.9k')
mod('selection', 920, 360, 300, 190,
    ['Per-player selection sets, selection', 'queries for the HUD, and the in-game', 'command handlers (move, stop, target).'],
    ['selection_set', 'queries', 'commands'],
    '3 files · 1.5k')

# lower row
mod('faction', 80, 600, 420, 110,
    ['Faction records: economy (mining, energy), relations,', 'army stock and production queues, score metrics.'],
    ['economy', 'relations', 'army_stock', 'runtime'],
    '4 files · 2.0k')
mod('technology', 560, 600, 190, 110,
    ['Research unlocks,', 'derived limits,', 'availability tests.'],
    ['runtime'],
    '1 file')
area('audio', 770, 620, 120, 40, 'audio — sounds')
mod('replay', 920, 600, 300, 110,
    ['Game recorder (.otr, project code): records', 'commands and steps, plays back from a checkpoint.'],
    ['runtime', 'stream', 'files', 'state_hash'],
    '4 files · 2.0k')
area('world', 80, 750, 1140, 40, 'world — owner list, terrain and field grid, pathing, models, shots, effects, camera')

# ui / network <-> session
d.line([(160, 120), (160, 160)], 'start: RunSessionUntilExit', 167, 144)
d.line([(400, 120), (400, 160)], 'tick (sync hook)', 407, 144)
d.line([(520, 160), (520, 120)], 'frames, UI root', 527, 144)
d.line([(560, 98), (720, 98)], 'queue command', 640, 92, anchor='middle')
d.line([(800, 160), (800, 120)], 'submit / batch', 807, 144, both=True)
# network executes the batched commands in selection (and faction) handlers
d.line([(1220, 98), (1252, 98), (1252, 420), (1220, 420)], 'execute command batch', 1246, 260,
       anchor='middle', rotate=True)
# session -> loaded areas (column on the right)
for y in (174, 208, 242, 276):
    d.line([(900, y), (920, y)])
# tick -> modules
d.line([(230, 310), (230, 360)], 'planning (jobs 2, 6)', 237, 340)
d.line([(640, 310), (640, 360)], 'per-entity update', 647, 340)
d.line([(80, 250), (56, 250), (56, 650), (80, 650)], 'economy (job 0)', 50, 450, anchor='middle', rotate=True)
d.line([(80, 200), (28, 200), (28, 770), (80, 770)], 'level setup, world jobs', 22, 485, anchor='middle', rotate=True)
# tick.cpp and startup.cpp call the recorder before and after every step and at session start and end
d.line([(890, 310), (890, 575), (1070, 575), (1070, 600)], 'before / after each step', 980, 568, anchor='middle')
# between modules
d.line([(380, 420), (450, 420)], 'create, route', 415, 413, anchor='middle')
d.line([(450, 490), (380, 490)], 'target choice', 415, 483, anchor='middle')
d.line([(920, 455), (830, 455)], 'orders', 858, 448, anchor='middle')
# network's command dispatch asks the recorder to record each local command
d.line([(1252, 420), (1252, 655), (1220, 655)], 'record commands', 1246, 540, anchor='middle', rotate=True)
d.line([(200, 550), (200, 600)], 'purchases, relations', 207, 580)
d.line([(470, 550), (470, 600)], 'factory queue, Xenite', 463, 585, anchor='end')
d.line([(700, 550), (700, 600)], 'research', 707, 588)
d.line([(820, 550), (820, 620)], 'sounds', 827, 590)
# army <-> world: pathing, shots, models; shots hit back into army damage
d.line([(530, 550), (530, 750)], 'pathing, shots, models · hits', 538, 732, both=True)

d.save(OUT)

if __name__ == '__main__':
    for k, (fy, bottom) in free.items():
        if fy > bottom:
            print(f'box {k}: content ends at {fy}, box ends at {bottom}')
    if len(sys.argv) > 1:
        A.render_png(OUT, sys.argv[1], W, H)
