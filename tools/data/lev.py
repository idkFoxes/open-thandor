"""Parser and dumper for Thandor level files (.lev, converter version 0x70001).

usage: python tools/data/lev.py dump <file.lev> [--all]
       python tools/data/lev.py check <file.lev | directory> ...

The layout follows the loaders in src/gameplay/session/level.c
(InGameLevelRuntime_LoadResourcesAfterDefaultReset, InGameLevelRuntime_SaveLevelAssetImageFromWorldState) and the
types LevelAssetHeader / LevelPlayerSlotRecord / LevelWorldSettings / InGameConditionSchedule /
LevelInitialArmyPlacementRecord20 in include/thandor/gameplay/session/types.h. Format notes: ot-scratch/lev_format.md.
parse(data) returns a dict; check(level, size) returns a list of problems (empty = consistent); build(level)
serializes such a dict back (stock layout; every stock level rebuilds byte-identically, which `check` verifies).
"""
import os
import re
import struct
import sys

LEV_MAGIC = 0x0076656C  # "lev\0"
LEV_CONVERTER = 0x00070001
PATH_RECORD_SIZE = 0x40  # 32 UTF-16 code units
PLACEMENT_SIZE = 0x20
SCHEDULE_OFFSET = 0x380  # InGameLevelConditionStorage.schedule, copied from the file with the prefix
CONDITION_COUNT = 64
TRIGGER_COUNT = 16
PREFIX_MIN = 0x800  # header + slots + world settings + reserved + schedule

PATH_FIELDS = ('field', 'groundTexture', 'surfaceTexture', 'skyTexture', 'armyTexture', 'shotTexture',
               'effectTexture', 'movie', 'sound', 'technology')
TABLES = ('arm', 'mdl', 'eff', 'sht')  # order of (count, offset) pairs at +0xE0

CONDITION_KINDS = {
    0: 'none', 2: 'faction_has_no_army', 4: 'faction_has_no_group_a_army', 6: 'faction_has_no_army_of_asset',
    8: 'faction_inactive_or_allied', 10: 'xenite_at_least', 12: 'tritium_at_least',
    14: 'tritium_rate_at_least', 16: 'army_of_asset_count_at_least', 18: 'occupancy_percent_at_least',
    20: 'countdown_elapsed', 22: 'xenite_storage_limit_at_most_250', 24: 'no_army_of_class_outside_group_a',
    26: 'postfix_expression',
}

WORLD_FIELDS = (
    'packedFieldRegionOrigin', 'terrainRampStepColor', 'terrainBaseColor', 'terrainLightingCycleTicks',
    'packedFieldRegionSize', 'terrainLightingColor128', 'terrainSecondaryColor', 'reserved1C',
    'terrainLightingColor130', 'terrainLightingColor134', 'terrainLightingColor138', 'terrainLightingColor13C',
    'assignableFactionCount', 'activeFactionCount', 'relationUiFlags', 'alliedGroupMasks', 'friendlyGroupMasks',
    'introNotificationMovieId', 'altPackedFieldRegionOrigin', 'altPackedFieldRegionSize', 'altTerrainRampStepColor',
    'altTerrainBaseColor', 'altTerrainLightingColor128', 'altTerrainSecondaryColor', 'altTerrainLightingColor130',
    'altTerrainLightingColor134', 'altTerrainLightingColor138', 'altTerrainLightingColor13C',
)


def u32(data, offset):
    return struct.unpack_from('<I', data, offset)[0]


def s32(data, offset):
    return struct.unpack_from('<i', data, offset)[0]


def utf16z(data, offset, units=None):
    end = len(data) if units is None else min(len(data), offset + units * 2)
    chars = []
    for pos in range(offset, end - 1, 2):
        unit = struct.unpack_from('<H', data, pos)[0]
        if unit == 0:
            return ''.join(chars), True
        chars.append(chr(unit))
    return ''.join(chars), False


def parse(data):
    """Parses a LEV image (bytes) into a dict; raises ValueError when it is not a LEV file."""
    if len(data) < PREFIX_MIN:
        raise ValueError('file shorter than the 0x800-byte fixed prefix')
    magic, alloc, fmt, conv = struct.unpack_from('<4I', data, 0)
    if magic != LEV_MAGIC:
        raise ValueError('magic 0x%08X is not "lev"' % magic)
    lev = {'magic': magic, 'allocationSize': alloc, 'formatVersion': fmt, 'converterVersion': conv}
    lev['timestamps'] = list(struct.unpack_from('<6I', data, 0x10))
    lev['anchor28'] = data[0x28:0x30]
    lev['producerName'] = utf16z(data, 0x30, 32)[0]
    lev['sourceName'] = utf16z(data, 0x70, 32)[0]
    lev['pathOffsets'] = dict(zip(PATH_FIELDS, struct.unpack_from('<10I', data, 0xB0)))
    lev['paths'] = {name: utf16z(data, off, 128)[0] if 0 < off < len(data) else None
                    for name, off in lev['pathOffsets'].items()}
    lev['placementCount'] = u32(data, 0xD8)
    lev['prefixSize'] = u32(data, 0xDC)  # also the offset of the placement table
    tables = {}
    for index, name in enumerate(TABLES):
        count, offset = struct.unpack_from('<2I', data, 0xE0 + index * 8)
        entries = [utf16z(data, offset + i * PATH_RECORD_SIZE, 32)[0]
                   for i in range(count) if offset + (i + 1) * PATH_RECORD_SIZE <= len(data)]
        tables[name] = {'count': count, 'offset': offset, 'entries': entries}
    lev['tables'] = tables
    # +0x100..+0x1FF: the level's scenario catalog record (identical to its record in level\level.dat)
    lev['catalogName'] = utf16z(data, 0x100, 32)[0]
    lev['catalogDwords'] = list(struct.unpack_from('<32I', data, 0x140))
    lev['catalogRecord'] = data[0x100:0x200]
    lev['titleTextIndex'] = u32(data, 0x170)
    lev['campaignAssociationIndex'] = s32(data, 0x190)
    lev['catalogTimestamp'] = utf16z(data, 0x1C0, 32)[0]
    slots = []
    for i in range(7):
        x, y, z, dist, hp, xen, tri, ai = struct.unpack_from('<iiiIIIIi', data, 0x200 + i * 0x20)
        slots.append({'faction': i + 1, 'cameraX': x, 'cameraY': y, 'cameraZ': z, 'cameraDistance': dist,
                      'heading': hp & 0xFFFF, 'pitch': hp >> 16, 'startXeniteQ4': xen, 'startTritiumQ4': tri,
                      'aiClassOrMode': ai})
    lev['playerSlots'] = slots
    world = dict(zip(WORLD_FIELDS, struct.unpack_from('<28I', data, 0x2E0)))
    world['musicSamples'] = list(struct.unpack_from('<4I', data, 0x350))
    world['effectSamples'] = list(struct.unpack_from('<4I', data, 0x360))
    lev['world'] = world
    lev['reserved370'] = data[0x370:0x380]
    conditions = []
    for i in range(CONDITION_COUNT):
        raw = data[SCHEDULE_OFFSET + i * 16:SCHEDULE_OFFSET + i * 16 + 16]
        kind = raw[0] & 0xFE
        entry = {'index': i, 'kind': kind, 'kindName': CONDITION_KINDS.get(kind, 'unknown_%d' % kind),
                 'satisfied': raw[0] & 1, 'raw': raw}
        if kind == 26:
            entry['expression'] = list(raw[1:])
        else:
            entry['operands'] = list(struct.unpack_from('<3i', raw, 4))
        conditions.append(entry)
    lev['conditions'] = conditions
    triggers = []
    base = SCHEDULE_OFFSET + CONDITION_COUNT * 16
    for i in range(TRIGGER_COUNT):
        t = data[base + i * 8:base + i * 8 + 8]
        triggers.append({'index': i, 'stateFlags': t[0], 'movieVariant': t[1], 'skipArmyDisable': t[2],
                         'reserved3': t[3], 'faction': t[4], 'endMovieSelection': t[5], 'condition': t[6],
                         'reserved7': t[7]})
    lev['triggers'] = triggers
    placements = []
    offset = lev['prefixSize']
    for i in range(lev['placementCount']):
        pos = offset + i * PLACEMENT_SIZE
        if pos + PLACEMENT_SIZE > len(data):
            break
        asset, faction, a08, a0c, rot = struct.unpack_from('<IIiiI', data, pos)
        placements.append({'armyAssetId': asset, 'faction': faction,
                           # +0x08 = world X of the node (editor saver), passed as the worldYQ12 parameter;
                           # +0x0C = world Y of the node, passed as worldXQ12
                           'x08': a08, 'y0C': a0c, 'rotation': rot, 'padding': data[pos + 0x14:pos + 0x20]})
    lev['placements'] = placements
    return lev


def check(lev, size):
    """Consistency problems of a parsed level (empty list = everything fits)."""
    problems = []
    if lev['converterVersion'] != LEV_CONVERTER:
        problems.append('converter version 0x%08X' % lev['converterVersion'])
    if lev['allocationSize'] != size:
        problems.append('allocation size %d != file size %d' % (lev['allocationSize'], size))
    prefix = lev['prefixSize']
    if prefix < PREFIX_MIN or prefix % 4:
        problems.append('prefix size 0x%X' % prefix)
    if prefix + lev['placementCount'] * PLACEMENT_SIZE != size:
        problems.append('placements 0x%X + %d * 0x20 != size 0x%X' % (prefix, lev['placementCount'], size))
    if len(lev['placements']) != lev['placementCount']:
        problems.append('placement table truncated')
    spans = [('schedule', SCHEDULE_OFFSET, PREFIX_MIN)]
    for name, table in lev['tables'].items():
        end = table['offset'] + table['count'] * PATH_RECORD_SIZE
        if table['count'] and (table['offset'] < PREFIX_MIN or end > prefix):
            problems.append('%s table 0x%X..0x%X outside 0x800..prefix' % (name, table['offset'], end))
        if table['count']:
            spans.append((name, table['offset'], end))
    for name, off in lev['pathOffsets'].items():
        if name == 'field' and off == 0:
            continue
        if not PREFIX_MIN <= off < prefix:
            problems.append('path %s offset 0x%X outside 0x800..prefix' % (name, off))
            continue
        spans.append(('path ' + name, off, off + PATH_RECORD_SIZE))
    spans.sort(key=lambda s: s[1])
    for (na, sa, ea), (nb, sb, eb) in zip(spans, spans[1:]):
        if sb < ea:
            problems.append('%s 0x%X..0x%X overlaps %s 0x%X' % (na, sa, ea, nb, sb))
    world = lev['world']
    if not 1 <= world['assignableFactionCount'] <= world['activeFactionCount'] <= 7:
        problems.append('faction counts assignable %d active %d' % (world['assignableFactionCount'],
                                                                    world['activeFactionCount']))
    for p in lev['placements']:
        if not 0 <= p['faction'] <= 7:
            problems.append('placement faction %d' % p['faction'])
    for t in lev['triggers']:
        if t['stateFlags'] and t['condition'] >= CONDITION_COUNT:
            problems.append('trigger %d condition %d' % (t['index'], t['condition']))
    return problems


def _put_utf16(buf, offset, text, units):
    raw = text.encode('utf-16le')
    if len(raw) > (units - 1) * 2:
        raise ValueError('string %r longer than %d code units' % (text, units - 1))
    buf[offset:offset + len(raw)] = raw


# Order in which the stock files (and build) lay out the variable part after the 0x800-byte fixed prefix.
STOCK_TABLE_ORDER = ('mdl', 'arm', 'sht', 'eff')
STOCK_PATH_ORDER = ('field', 'groundTexture', 'surfaceTexture', 'skyTexture', 'armyTexture', 'shotTexture',
                    'effectTexture', 'sound', 'technology', 'movie')


def build(lev):
    """Serializes a dict shaped like parse()'s result into a LEV image, with the stock layout: fixed prefix
    0x000..0x7FF, MDL/ARM/SHT/EFF path tables from 0x800, ten 0x40-byte path strings, then the placements.
    Offsets, counts and sizes are recomputed; unknown bytes come from lev['catalogRecord'] etc. when present."""
    tables = {name: list(lev['tables'][name]['entries']) for name in TABLES}
    prefix = PREFIX_MIN + sum(len(entries) for entries in tables.values()) * PATH_RECORD_SIZE \
        + len(STOCK_PATH_ORDER) * PATH_RECORD_SIZE
    size = prefix + len(lev['placements']) * PLACEMENT_SIZE
    buf = bytearray(size)
    struct.pack_into('<4I', buf, 0, LEV_MAGIC, size, lev.get('formatVersion', 1), LEV_CONVERTER)
    struct.pack_into('<6I', buf, 0x10, *lev.get('timestamps', [0] * 6))
    _put_utf16(buf, 0x30, lev.get('producerName', ''), 32)
    _put_utf16(buf, 0x70, lev.get('sourceName', ''), 32)
    cursor = PREFIX_MIN
    table_offsets = {}
    for name in STOCK_TABLE_ORDER:
        table_offsets[name] = cursor
        for entry in tables[name]:
            _put_utf16(buf, cursor, entry, 32)
            cursor += PATH_RECORD_SIZE
    path_offsets = {}
    for name in STOCK_PATH_ORDER:
        path_offsets[name] = cursor
        _put_utf16(buf, cursor, lev['paths'][name], 32)
        cursor += PATH_RECORD_SIZE
    struct.pack_into('<10I', buf, 0xB0, *(path_offsets[name] for name in PATH_FIELDS))
    struct.pack_into('<2I', buf, 0xD8, len(lev['placements']), prefix)
    for index, name in enumerate(TABLES):
        struct.pack_into('<2I', buf, 0xE0 + index * 8, len(tables[name]), table_offsets[name])
    record = bytearray(lev.get('catalogRecord', bytes(0x100)))
    buf[0x100:0x200] = record
    buf[0x100:0x140] = bytes(0x40)
    _put_utf16(buf, 0x100, lev['catalogName'], 32)
    struct.pack_into('<I', buf, 0x170, lev['titleTextIndex'])
    struct.pack_into('<i', buf, 0x190, lev.get('campaignAssociationIndex', 0))
    for i, s in enumerate(lev['playerSlots']):
        struct.pack_into('<iiiIIIIi', buf, 0x200 + i * 0x20, s['cameraX'], s['cameraY'], s['cameraZ'],
                         s['cameraDistance'], (s['heading'] & 0xFFFF) | (s['pitch'] & 0xFFFF) << 16,
                         s['startXeniteQ4'], s['startTritiumQ4'], s['aiClassOrMode'])
    world = lev['world']
    struct.pack_into('<28I', buf, 0x2E0, *(world[name] for name in WORLD_FIELDS))
    struct.pack_into('<4I', buf, 0x350, *world['musicSamples'])
    struct.pack_into('<4I', buf, 0x360, *world['effectSamples'])
    for c in lev['conditions']:
        pos = SCHEDULE_OFFSET + c['index'] * 16
        if 'raw' in c:
            buf[pos:pos + 16] = c['raw']
        elif c['kind'] == 26:
            expression = bytes(c['expression'])
            buf[pos] = 26
            buf[pos + 1:pos + 1 + len(expression)] = expression
        else:
            buf[pos] = c['kind']
            struct.pack_into('<3i', buf, pos + 4, *c['operands'])
    base = SCHEDULE_OFFSET + CONDITION_COUNT * 16
    for t in lev['triggers']:
        struct.pack_into('<8B', buf, base + t['index'] * 8, t['stateFlags'], t.get('movieVariant', 0),
                         t.get('skipArmyDisable', 0), 0, t['faction'], t.get('endMovieSelection', 0), t['condition'], 0)
    for i, p in enumerate(lev['placements']):
        struct.pack_into('<IIiiI', buf, prefix + i * PLACEMENT_SIZE, p['armyAssetId'], p['faction'], p['x08'],
                         p['y0C'], p['rotation'] & 0xFFFFFFFF)
    return bytes(buf)


def load(path):
    with open(path, 'rb') as handle:
        data = handle.read()
    return parse(data), len(data)


_ARMY_NAMES = None


def army_name(asset_id):
    """Name of an army asset id from enum PckArmyAssetIdCatalog (include/thandor/core/types.h)."""
    global _ARMY_NAMES
    if _ARMY_NAMES is None:
        _ARMY_NAMES = {}
        types_h = os.path.join(os.path.dirname(__file__), '..', '..', 'include', 'thandor', 'core', 'types.h')
        try:
            text = open(types_h, encoding='utf-8', errors='replace').read()
            body = re.search(r'enum\s*\{([^{}]*)\};\s*typedef int PckArmyAssetIdCatalog;', text, re.S)
            if body:
                for name, value in re.findall(r'(\w+)=(\d+)', body.group(1)):
                    _ARMY_NAMES[int(value)] = name
        except OSError:
            pass
    return _ARMY_NAMES.get(asset_id, '?')


def q12(value):
    return '%.3f' % (value / 4096.0)


def dump(path, show_all=False):
    lev, size = load(path)
    print('%s: %d bytes' % (path, size))
    print('  magic lev, alloc %d, format %d, converter 0x%08X, producer %r, source %r' % (
        lev['allocationSize'], lev['formatVersion'], lev['converterVersion'], lev['producerName'],
        lev['sourceName']))
    print('  timestamps ' + ' '.join('%08X' % t for t in lev['timestamps']))
    print('  paths:')
    for name in PATH_FIELDS:
        print('    %-15s +0x%04X  %r' % (name, lev['pathOffsets'][name], lev['paths'][name]))
    print('  prefix/placement offset 0x%X, placements %d' % (lev['prefixSize'], lev['placementCount']))
    for name in TABLES:
        table = lev['tables'][name]
        print('  %s table: %d at 0x%X: %s' % (name.upper(), table['count'], table['offset'],
                                            ', '.join(table['entries'])))
    print('  catalog record: name %r, dwords +0x40.. %s, title index %d, campaign index %d, timestamp %r' % (
        lev['catalogName'], ' '.join('%X' % d for d in lev['catalogDwords'][:20]), lev['titleTextIndex'],
        lev['campaignAssociationIndex'], lev['catalogTimestamp']))
    print('  player slots:')
    for s in lev['playerSlots']:
        if not show_all and not any((s['cameraX'], s['cameraY'], s['startXeniteQ4'], s['startTritiumQ4'],
                                     s['aiClassOrMode'])):
            continue
        print('    faction %d: camera (%s, %s, %s) dist %s heading 0x%04X pitch 0x%04X, xenite %d tritium %d '
              '(Q4 %d/%d), aiClassOrMode %d' % (
                  s['faction'], q12(s['cameraX']), q12(s['cameraY']), q12(s['cameraZ']), q12(s['cameraDistance']),
                  s['heading'], s['pitch'], s['startXeniteQ4'] >> 4, s['startTritiumQ4'] >> 4, s['startXeniteQ4'],
                  s['startTritiumQ4'], s['aiClassOrMode']))
    w = lev['world']
    print('  world settings:')
    for name in WORLD_FIELDS:
        print('    %-28s 0x%08X' % (name, w[name]))
    print('    music %s, effects %s' % (w['musicSamples'], w['effectSamples']))
    print('  conditions:')
    for c in lev['conditions']:
        if c['kind'] == 0 and not show_all:
            continue
        if 'expression' in c:
            tokens = []
            for b in c['expression']:
                if b == 0xFC:
                    break
                tokens.append({0xFD: 'NOT', 0xFE: 'AND', 0xFF: 'OR'}.get(b, 'c%d' % b))
            detail = ' '.join(tokens)
        else:
            detail = 'operands %s' % c['operands']
        print('    %2d %-32s %s' % (c['index'], c['kindName'], detail))
    print('  end triggers:')
    for t in lev['triggers']:
        if t['stateFlags'] == 0 and not show_all:
            continue
        print('    %2d flags %d faction %d condition %d movieVariant %d endMovie %d skipDisable %d' % (
            t['index'], t['stateFlags'], t['faction'], t['condition'], t['movieVariant'], t['endMovieSelection'],
            t['skipArmyDisable']))
    print('  placements:')
    for i, p in enumerate(lev['placements']):
        print('    %3d asset %4d %-28s faction %d  +08 %9s  +0C %9s  rot 0x%08X%s' % (
            i, p['armyAssetId'], army_name(p['armyAssetId']), p['faction'], q12(p['x08']), q12(p['y0C']),
            p['rotation'], '' if p['padding'] == bytes(12) else ' pad ' + p['padding'].hex()))
    problems = check(lev, size)
    print('  check: ' + ('ok' if not problems else '; '.join(problems)))


def check_paths(paths):
    files = []
    for path in paths:
        if os.path.isdir(path):
            for root, _, names in os.walk(path):
                files.extend(os.path.join(root, n) for n in sorted(names) if n.lower().endswith('.lev'))
        else:
            files.append(path)
    bad = 0
    for path in files:
        try:
            lev, size = load(path)
            problems = check(lev, size)
            with open(path, 'rb') as handle:
                data = handle.read()
            rebuilt = build(lev)
            if rebuilt != data:
                first = next((i for i, (a, b) in enumerate(zip(rebuilt, data)) if a != b), min(len(rebuilt), len(data)))
                problems.append('rebuild differs at 0x%X (sizes %d/%d)' % (first, len(rebuilt), len(data)))
        except (ValueError, struct.error) as error:
            problems = [str(error)]
        if problems:
            bad += 1
        print('%-50s %s' % (os.path.basename(path), 'ok' if not problems else '; '.join(problems)))
    print('%d files, %d with problems' % (len(files), bad))
    return bad == 0


def main():
    if len(sys.argv) < 3 or sys.argv[1] not in ('dump', 'check'):
        print(__doc__)
        sys.exit(2)
    if sys.argv[1] == 'dump':
        dump(sys.argv[2], '--all' in sys.argv[3:])
    else:
        sys.exit(0 if check_paths(sys.argv[2:]) else 1)


if __name__ == '__main__':
    main()
