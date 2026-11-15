"""Draws docs/architecture/gameplay/gameplay.svg, the block diagram of the gameplay area.

Run from the repository root: python tools/docs/architecture/gen_gameplay.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

W, H = 1280, 800
OUT = 'docs/architecture/gameplay/gameplay.svg'
SRC = '../../../src/gameplay/'

d = A.Diagram(W, H, 'gameplay — session, simulation tick, armies, AI, factions',
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
mod('session', 80, 160, 800, 130,
    ['Session lifecycle: start a new level or a saved game, run frames until exit, save, campaign carry-over.',
     'tick.cpp: one simulation step = lockstep, per-entity update, level script every 20 steps, 8 world jobs.'],
    ['startup', 'new_session', 'loaded_session', 'level_new', 'level_saved', 'level', 'level_script', 'tick',
     'savegame', 'savegame_load', 'scenario_load', 'campaign_carryover', 'loading_movie'],
    '14 files · 5.6k lines')

# middle row
mod('ai', 80, 340, 300, 190,
    ['Computer factions: per-faction planning', 'every 64 steps (unit or economy phase),',
     'per-army target choice.'],
    ['planning', 'perception', 'workspaces', 'units', 'combat', 'construction', 'purchasing', 'placement',
     'technology'],
    '9 files · 5.1k')
mod('army', 450, 340, 380, 190,
    ['Units and buildings: creation and pool, per-class', 'update tables, movement, weapons and damage,',
     'placement, factories.'],
    ['pool', 'factory', 'class_dispatch', 'class_updates', 'move_orders', 'drive_*', 'walker', 'aircraft',
     'weapons', 'turrets', 'combat', 'damage', 'collision', 'placement*', 'preview', 'model_*', 'audio'],
    '22 files · 11.6k')
mod('selection', 920, 340, 300, 190,
    ['Per-player selection sets, selection', 'queries for the HUD, and the in-game', 'command handlers (move, stop, target).'],
    ['selection_set', 'queries', 'commands'],
    '3 files · 1.4k')

# lower row
mod('faction', 80, 580, 440, 100,
    ['Faction records: economy (mining, energy), relations,', 'army stock and production queues, score metrics.'],
    ['economy', 'relations', 'army_stock', 'runtime'],
    '4 files · 2.0k')
mod('technology', 640, 580, 260, 100,
    ['Research unlocks per faction,', 'derived limits, availability tests.'],
    ['runtime'],
    '1 file')
area('audio', 940, 600, 280, 40, 'audio — positioned unit sounds')
area('world', 80, 730, 1140, 40, 'world — owner list, terrain and field grid, pathing, models, shots, effects, camera')

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
    d.line([(880, y), (920, y)])
# tick -> modules
d.line([(230, 290), (230, 340)], 'planning (jobs 2, 6)', 237, 320)
d.line([(640, 290), (640, 340)], 'per-entity update', 647, 320)
d.line([(80, 250), (56, 250), (56, 630), (80, 630)], 'economy (job 0)', 50, 440, anchor='middle', rotate=True)
d.line([(80, 200), (28, 200), (28, 750), (80, 750)], 'level setup, world jobs', 22, 475, anchor='middle', rotate=True)
# between modules
d.line([(380, 400), (450, 400)], 'create, route', 415, 393, anchor='middle')
d.line([(450, 470), (380, 470)], 'target choice', 415, 463, anchor='middle')
d.line([(920, 435), (830, 435)], 'orders', 875, 428, anchor='middle')
d.line([(200, 530), (200, 580)], 'purchases, relations', 207, 560)
d.line([(480, 530), (480, 580)], 'factory queue, Xenite', 473, 565, anchor='end')
d.line([(700, 530), (700, 580)], 'research, unlocks', 707, 568)
d.line([(810, 530), (810, 545), (1080, 545), (1080, 600)], 'sounds', 1087, 578)
# army <-> world: pathing, shots, models; shots hit back into army damage
d.line([(580, 530), (580, 730)], 'pathing, shots, models · hits', 588, 712, both=True)

d.save(OUT)

if __name__ == '__main__':
    for k, (fy, bottom) in free.items():
        if fy > bottom:
            print(f'box {k}: content ends at {fy}, box ends at {bottom}')
    if len(sys.argv) > 1:
        A.render_png(OUT, sys.argv[1], W, H)
