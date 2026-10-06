"""Builds the scene for the installer's wizard image: a stock level with an extra group of heavy units.

usage: python tools/installer/make_showcase_level.py <game dir> [--name wizshow] [--base level\\hansolo\\s03_erobern]
       [--units 1,10,...] [--at X,Y] [--columns 4] [--spacing 2] [--rotation 0x3700]

Copies the base level (terrain, base buildings and units of every faction stay), switches its mission script
off (so nothing ends the level), turns it into a single game (catalog name "<name>.", catalog kind 2 as the
stock skirmish maps), removes player 1's stock units and adds the given unit types for player 1 in a block of --columns per row, --spacing cells
apart, starting at world X,Y (default: in front of player 1's base in hansolo level 4, where the start camera
looks), then a small decoy unit far away: the level starts with the last placed unit selected, and its health
bar would be in the picture. Writes <game dir>/level/<name>.lev and level00.dat (the single-game catalog record; the field grid stays
the base level's, read from LEVEL.PCK). Start with -KARTE="<name>.".

The level folder of the test copies is a junction to the shared game folder: take_wizard_shots.py --showcase
replaces it with a private copy before calling this, so level00.dat of the arena tests is not touched.
Formats: tools/data/lev.py, tools/test/make_arena.py."""
import argparse
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(REPO, 'tools', 'data'))
sys.path.insert(0, os.path.join(REPO, 'tools', 'test'))
import lev  # noqa: E402
import make_arena  # noqa: E402
import pck  # noqa: E402

# heavy units: rocket tanks and the big walkers with cannons
# Weapons are separate unit ids: the last id of each chassis group (e.g. 89, 105, 141, 224) is the bare chassis,
# the ids before it the armed variants (their army records link them). 81: tracked tank with a rocket pod,
# 84: tracked tank with a rocket rack, 82: tracked multi launcher, 101: heavy tank with twin cannon,
# 223: big walker with rocket rails. Grid columns run into the screen (away from the start camera), rows to the left.
DEFAULT_UNITS = '223,81,84,101,82,81'
COLUMN_STEP_X, ROW_STEP_X, ROW_STEP_Y = make_arena.COLUMN_STEP_X, make_arena.ROW_STEP_X, make_arena.ROW_STEP_Y


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('game_dir')
    parser.add_argument('--name', default='wizshow')
    parser.add_argument('--base', default='level\\hansolo\\s03_erobern')
    parser.add_argument('--units', default=DEFAULT_UNITS, help='army asset ids, comma-separated')
    parser.add_argument('--at', default='124000,-136000', help='world X,Y of the first unit')
    parser.add_argument('--columns', type=int, default=3)
    parser.add_argument('--spacing', type=float, default=2.5, help='cells between units')
    parser.add_argument('--rotation', default='0x3700', help='unit heading (0x10000 = full turn)')
    parser.add_argument('--decoy', default='1@95000,-150000',
                        help='ID@X,Y: a unit placed last, away from the group; the level starts with the last '
                        'placed unit selected (its health bar shows), so that one is out of the picture; "" = none')
    parser.add_argument('--keep-units', action='store_true',
                        help="keep player 1's stock units (one of them starts selected and shows its frame)")
    args = parser.parse_args()

    data, entries = pck.entries(os.path.join(args.game_dir, 'LEVEL.PCK'))
    by_name = {e[0].lower(): e for e in entries}
    level = lev.parse(pck.read_entry(data, by_name[args.base.lower() + '.lev']))
    present = make_arena.loaded_army_ids(args.game_dir, level['tables']['arm']['entries'])
    units = [int(v, 0) for v in args.units.split(',') if v.strip()]
    missing = [u for u in units if u not in present]
    if missing:
        raise SystemExit('unit ids %s are not in the level\'s army files' % missing)
    if not args.keep_units:
        level['placements'] = [p for p in level['placements'] if p['faction'] != 1 or p['armyAssetId'] >= 300]
    x0, y0 = (int(v) for v in args.at.split(','))
    rotation = int(args.rotation, 0)
    for index, asset_id in enumerate(units):
        column, row = index % args.columns, index // args.columns
        # along a cell row (x) and back over the rows (-y), as the isometric lattice of make_arena
        x = x0 + int(column * args.spacing * COLUMN_STEP_X + row * args.spacing * ROW_STEP_X)
        y = y0 + int(row * args.spacing * ROW_STEP_Y)
        level['placements'].append({'armyAssetId': asset_id, 'faction': 1, 'x08': x, 'y0C': y,
                                    'rotation': rotation, 'padding': bytes(12)})

    if args.decoy:
        decoy_id, at = args.decoy.split('@')
        x, y = (int(v) for v in at.split(','))
        level['placements'].append({'armyAssetId': int(decoy_id, 0), 'faction': 1, 'x08': x, 'y0C': y,
                                    'rotation': rotation, 'padding': bytes(12)})

    level['catalogName'] = args.name + '.'
    for condition in level['conditions']:
        condition['raw'] = bytes(16)
    for trigger in level['triggers']:
        trigger.update(stateFlags=0, faction=0, condition=0, movieVariant=0, skipArmyDisable=0, endMovieSelection=0)
    level['world']['introNotificationMovieId'] = 0
    record = bytearray(level['catalogRecord'])
    record[0:0x40] = bytes(0x40)
    record[0:2 * len(level['catalogName'])] = level['catalogName'].encode('utf-16-le')
    record[0x40:0x44] = '2'.encode('utf-16-le') + b'\0\0'
    record[0x48:0x4C] = '2'.encode('utf-16-le') + b'\0\0'
    record[0x60:0x64] = (2).to_bytes(4, 'little')  # level kind: 2 single game (the campaign levels have 1)
    level['catalogRecord'] = bytes(record)

    out = os.path.join(args.game_dir, 'level')
    os.makedirs(out, exist_ok=True)
    image = lev.build(level)
    problems = lev.check(lev.parse(image), len(image))
    if problems:
        raise SystemExit('generated level fails the checks: %s' % problems)
    open(os.path.join(out, args.name + '.lev'), 'wb').write(image)
    open(os.path.join(out, 'level00.dat'), 'wb').write(level['catalogRecord'])
    print('%s: %s plus %d units for player 1, start with -KARTE="%s."' % (args.name, args.base, len(units), args.name))


if __name__ == '__main__':
    main()
