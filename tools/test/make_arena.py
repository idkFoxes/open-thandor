"""Builds a test level for determinism runs: a flat map, two players, every unit and building of the army
catalog on both sides, the two unit fronts facing each other so the whole armies go into battle.

usage: python tools/test/make_arena.py <game dir> [--name testarena] [--base asgard] [--size 131]
       [--units N] [--buildings N] [--script]

Writes <game dir>/level/<name>.lev, <name>.fld and level00.dat (the catalog record that puts the level into the
single game list); start it with -KARTE="<name>." (the trailing dot is part of the catalog name). The level's
files are loose and uncompressed; the game only reads them when LEVEL.PCK has no entry of that name. Needs the
stock LEVEL.PCK of the game directory (the base level is copied for textures, lighting and file lists).
Formats: ot-scratch/lev_format.md, ot-scratch/fld_format.md; tools/data/lev.py, fld.py, pck.py."""
import argparse
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'data'))
import fld  # noqa: E402
import lev  # noqa: E402
import pck  # noqa: E402

COLUMN_STEP_X, ROW_STEP_X, ROW_STEP_Y = 0x901, 0x480, -1999   # isometric cell lattice (fld_format.md)
TYPES_H = os.path.join(HERE, '..', '..', 'include', 'thandor', 'generated', 'types.h')


def catalog_ids():
    """(unit ids, building ids) from the PckArmyAssetIdCatalog enum: 1..299 units, 300..399 buildings."""
    text = open(TYPES_H, encoding='utf-8').read()
    body = re.search(r'typedef enum PckArmyAssetIdCatalog\s*\{(.*?)\}', text, re.S).group(1)
    ids = sorted(int(v) for _, v in re.findall(r'(\w+)=(\d+)', body))
    return [i for i in ids if 1 <= i < 300], [i for i in ids if 300 <= i < 400]


def loaded_army_ids(game_dir, arm_paths):
    """Registry ids of every army asset record in the level's ARM files (DATEN.PCK): records from +0x200, each
    starting with its byte size, the registry id at +8 (ArmyAssetRecordPrefix). A placement with an id that no
    loaded file contains stops the level load (fatal error 0x41, "army id not found")."""
    data, entries = pck.entries(os.path.join(game_dir, 'DATEN.PCK'))
    by_name = {e[0].lower(): e for e in entries}
    ids = set()
    for path in arm_paths:
        entry = by_name.get(path.lower() + '.arm')
        if entry is None:
            continue
        image = pck.read_entry(data, entry)
        count = struct.unpack_from('<I', image, 0xB0)[0]
        offset = 0x200
        for _ in range(count):
            size, _variant, registry_id = struct.unpack_from('<III', image, offset)
            ids.add(registry_id)
            offset += size
    return ids


def world(column, row):
    """World X/Y of a cell centre (placement +0x08 / +0x0C, camera X/Y)."""
    return column * COLUMN_STEP_X + row * ROW_STEP_X, row * ROW_STEP_Y


def block(ids, faction, first_row, row_step, columns, first_column, column_step, rotation):
    """Placements of ids in a block of `columns` per row; rows advance by row_step (negative: towards row 0)."""
    placements = []
    for index, asset_id in enumerate(ids):
        row = first_row + (index // columns) * row_step
        column = first_column + (index % columns) * column_step
        x, y = world(column, row)
        placements.append({'armyAssetId': asset_id, 'faction': faction, 'x08': x, 'y0C': y, 'rotation': rotation})
    return placements


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('game_dir')
    parser.add_argument('--name', default='testarena')
    parser.add_argument('--base', default='asgard', help='stock level whose textures/lighting/file lists are used')
    parser.add_argument('--size', type=int, default=131, help='grid width = height (8k+3; stock maps up to 139)')
    parser.add_argument('--units', type=int, default=0, help='limit the unit types per side (0 = all)')
    parser.add_argument('--buildings', type=int, default=0, help='limit the building types per side (0 = all)')
    parser.add_argument('--script', action='store_true', help='keep the base level script (win/lose); '
                        'default: no script, the battle never ends by itself')
    args = parser.parse_args()
    size = args.size

    data, entries = pck.entries(os.path.join(args.game_dir, 'LEVEL.PCK'))
    by_name = {e[0].lower(): e for e in entries}
    base_lev = pck.read_entry(data, by_name['level\\%s.lev' % args.base])
    base_fld = pck.read_entry(data, by_name['level\\%s.fld' % args.base])
    level = lev.parse(base_lev)

    units, buildings = catalog_ids()
    present = loaded_army_ids(args.game_dir, level['tables']['arm']['entries'])
    units = [i for i in units if i in present]
    buildings = [i for i in buildings if i in present]
    if args.units:
        units = units[:args.units]
    if args.buildings:
        buildings = buildings[:args.buildings]

    # layout (rows count from the map edge at row 0): player 1 buildings, player 1 units | gap | player 2 units,
    # player 2 buildings; 8 buildings per row 8 cells apart, 19 units per row 3 cells apart, fronts 4 rows apart
    middle = size // 2
    building_columns, building_step = 8, 8
    unit_columns, unit_step = 19, 3
    building_first_column = (size - (building_columns - 1) * building_step) // 2
    unit_first_column = (size - (unit_columns - 1) * unit_step) // 2
    unit_rows = (len(units) + unit_columns - 1) // unit_columns
    building_rows = (len(buildings) + building_columns - 1) // building_columns
    front1, front2 = middle - 2, middle + 2
    placements = []
    placements += block(units, 1, front1, -2, unit_columns, unit_first_column, unit_step, 0x8000)
    placements += block(buildings, 1, front1 - 2 * unit_rows - 6, -building_step, building_columns,
                        building_first_column, building_step, 0x8000)
    placements += block(units, 2, front2, 2, unit_columns, unit_first_column, unit_step, 0)
    placements += block(buildings, 2, front2 + 2 * unit_rows + 6, building_step, building_columns,
                        building_first_column, building_step, 0)
    low_row = front1 - 2 * unit_rows - 6 - (building_rows - 1) * building_step
    high_row = front2 + 2 * unit_rows + 6 + (building_rows - 1) * building_step
    if low_row < 3 or high_row > size - 4:
        raise SystemExit('layout needs rows %d..%d, map has 0..%d: use a larger --size or fewer types'
                         % (low_row, high_row, size - 1))

    level['placements'] = placements
    level['catalogName'] = args.name + '.'
    level['paths']['field'] = 'level\\' + args.name
    world_settings = level['world']
    world_settings['assignableFactionCount'] = 2
    world_settings['activeFactionCount'] = 2
    world_settings['terrainLightingCycleTicks'] = 0
    world_settings['introNotificationMovieId'] = 0
    if not args.script:
        for condition in level['conditions']:
            condition['raw'] = bytes(16)
        for trigger in level['triggers']:
            trigger.update(stateFlags=0, faction=0, condition=0, movieVariant=0, skipArmyDisable=0,
                           endMovieSelection=0)
    for slot in level['playerSlots']:
        slot.update(cameraX=0, cameraY=0, cameraZ=0, cameraDistance=0, heading=0, pitch=0, startXeniteQ4=0,
                    startTritiumQ4=0, aiClassOrMode=0)
    for faction, row in ((1, front1 - 3), (2, front2 + 3)):
        x, y = world(size // 2, row)
        level['playerSlots'][faction - 1].update(
            cameraX=x, cameraY=y, cameraZ=60000, cameraDistance=4096, heading=0x4000 if faction == 1 else 0xC000,
            pitch=0xE000, startXeniteQ4=4000 << 4, startTritiumQ4=4000 << 4)
    # catalog record (+0x100..+0x1FF, also level00.dat): name with trailing dot, player count digits '2' / '2'
    record = bytearray(level['catalogRecord'])
    record[0:0x40] = bytes(0x40)
    record[0:2 * len(level['catalogName'])] = level['catalogName'].encode('utf-16-le')
    record[0x40:0x44] = '2'.encode('utf-16-le') + b'\0\0'
    record[0x48:0x4C] = '2'.encode('utf-16-le') + b'\0\0'
    level['catalogRecord'] = bytes(record)

    # flat terrain with the base map's most common ground material (its texture set is known to exist)
    base_header = struct.unpack_from('<I', base_fld, 0xB0)[0]
    materials = [m for m in range(32) if base_header & (1 << m)]
    counts = {m: 0 for m in materials}
    width, height = struct.unpack_from('<II', base_fld, 0xB8)
    for index in range(width * height):
        material = base_fld[0x200 + index * 0x80 + 0x50]
        if material in counts:
            counts[material] += 1
    material = max(counts, key=counts.get)
    terrain = 2560
    field_grid = fld.build_flat(size, size, None, material, terrain, -terrain)

    out = os.path.join(args.game_dir, 'level')
    os.makedirs(out, exist_ok=True)
    lev_image = lev.build(level)
    problems = lev.check(lev.parse(lev_image), len(lev_image))
    if problems:
        raise SystemExit('generated level fails the checks: %s' % problems)
    open(os.path.join(out, args.name + '.lev'), 'wb').write(lev_image)
    open(os.path.join(out, args.name + '.fld'), 'wb').write(field_grid)
    open(os.path.join(out, 'level00.dat'), 'wb').write(level['catalogRecord'])
    print('%s: %dx%d flat (material %d), %d placements (%d unit and %d building types per side), script %s'
          % (args.name, size, size, material, len(placements), len(units), len(buildings),
             'kept' if args.script else 'off'))
    print('start with -KARTE="%s."' % args.name)


if __name__ == '__main__':
    main()
