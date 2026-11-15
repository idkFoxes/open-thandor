"""Dumps the game's campaign files (level\\<name>.cgn) and the campaign catalog (level\\campagne.dat).

usage:
  python tools/data/cgn.py dump <file.cgn | level dir> ... [--triggers] [--all]
  python tools/data/cgn.py catalog <campagne.dat>

Extract the files first: python tools/data/pck.py extract LEVEL.PCK <out dir> level\\ ; then pass <out dir>/level.
A directory dumps its campagne.dat and every .cgn in it.

Layout (CampaignAsset / CampaignLevelRecord, include/thandor/assets/scenario/catalog.h): a 0x200-byte header
("cgn\\0", allocation size, format/converter version, producer and source name like a .lev, firstLevelId at
+0xB4, levelRecordCount at +0xB8, currentLevelId at +0xC4, and at +0x100 the campaign's 0x100-byte catalog record),
then levelRecordCount records of 0x180 bytes: successorLevelIds[8] and two end movie number tables (one entry per
end selection, g_EndMovieSelectionIndex; the "variant" table is used when g_EndMovieVariantIndex != 0), per-faction
exit zones (centre, radius, destination, Q12 world units, indexed by the army's
factionIndex; RebuildScenarioReplayTables in src/gameplay/session/campaign_carryover.cpp carries a unit whose distance
to the centre is <= radius and moves it by destination - centre; radius <= 0 = none), levelId, carryOverMask and skipMask (one bit per end
selection) and the UTF-16 level file name (level\\<name>.lev).

A campagne.dat record (0x100 bytes, ScenarioCatalogRecord) holds the UTF-16 file name at +0x00, the minimum and
maximum player count as UTF-16 digit text at +0x40 / +0x48 and the campaign title index at +0x50
(TEXT_ID_CAMPAIGN_TITLE_BASE / TEXT_ID_CAMPAIGN_DESCRIPTION_BASE + index). The embedded record at +0x100 of a .cgn
has the same layout.

--triggers also parses every level of a campaign with tools/data/lev.py and lists its end triggers as
(faction, endMovieSelection, movieVariant); it flags every trigger where selection 0 and variant 0 do not go
together (selection 0 with a success movie, or a failure movie on a selection other than 0).
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lev  # noqa: E402

CGN_MAGIC = b"cgn\0"
HEADER = 0x200
RECORD = 0x180
SELECTIONS = 8
FACTIONS = 8
CATALOG_RECORD = 0x100


def utf16z(data, offset, units):
    return lev.utf16z(data, offset, units)[0]


def parse_catalog_record(data, offset=0):
    return {'name': utf16z(data, offset, 32), 'minPlayers': utf16z(data, offset + 0x40, 4),
            'maxPlayers': utf16z(data, offset + 0x48, 4), 'titleIndex': struct.unpack_from('<i', data, offset + 0x50)[0]}


def parse(data):
    """Parses a CGN image into a dict; raises ValueError when it is not a campaign file."""
    if len(data) < HEADER or data[:4] != CGN_MAGIC:
        raise ValueError('not a cgn file')
    alloc, fmt, conv = struct.unpack_from('<3I', data, 4)
    first, count = struct.unpack_from('<ii', data, 0xB4)
    current = struct.unpack_from('<i', data, 0xC4)[0]
    cgn = {'allocationSize': alloc, 'formatVersion': fmt, 'converterVersion': conv,
           'producerName': utf16z(data, 0x30, 32), 'sourceName': utf16z(data, 0x70, 32),
           'firstLevelId': first, 'levelRecordCount': count, 'currentLevelId': current,
           'catalogRecord': parse_catalog_record(data, 0x100), 'levels': []}
    if HEADER + count * RECORD > len(data):
        raise ValueError('%d records do not fit into %d bytes' % (count, len(data)))
    for i in range(count):
        base = HEADER + i * RECORD

        def table(offset):
            return list(struct.unpack_from('<8i', data, base + offset))
        level_id, carry, skip = struct.unpack_from('<iII', data, base + 0x100)
        cgn['levels'].append({
            'successorLevelIds': table(0x000), 'endMovieNumbersVariant': table(0x020),
            'endMovieNumbers': table(0x040),
            'exitZones': [{'index': f, 'x': cx, 'y': cy, 'radius': r, 'destX': dx, 'destY': dy}
                          for f, (cx, cy, r, dx, dy) in enumerate(zip(table(0x060), table(0x080), table(0x0A0),
                                                                      table(0x0C0), table(0x0E0)))],
            'levelId': level_id, 'carryOverMask': carry, 'skipMask': skip,
            'levelFileName': utf16z(data, base + 0x10C, 58)})
    return cgn


def used_selections(record):
    """Number of end selections shown: up to the last nonzero successor or mask bit, at least 2 (loss, win)."""
    used = 2
    for i, successor in enumerate(record['successorLevelIds']):
        if successor or (record['carryOverMask'] | record['skipMask']) >> i & 1:
            used = max(used, i + 1)
    return used


def level_triggers(level_dir, file_name):
    """(faction, endMovieSelection, movieVariant) of every active end trigger, or None when the file is missing."""
    path = os.path.join(level_dir, *file_name.split('\\')) + '.lev'
    if not os.path.exists(path):
        return None
    level, _ = lev.load(path)
    return [(t['faction'], t['endMovieSelection'], t['movieVariant']) for t in level['triggers'] if t['stateFlags']]


def dump(path, show_triggers=False, show_all=False):
    data = open(path, 'rb').read()
    cgn = parse(data)
    record = cgn['catalogRecord']
    print('%s: %d bytes, alloc %d, format %d, converter 0x%08X, producer %r, source %r' % (
        path, len(data), cgn['allocationSize'], cgn['formatVersion'], cgn['converterVersion'], cgn['producerName'],
        cgn['sourceName']))
    print('  catalog record: name %r, players %s-%s, title index %d' % (
        record['name'], record['minPlayers'], record['maxPlayers'], record['titleIndex']))
    print('  firstLevelId %d, records %d, currentLevelId %d' % (
        cgn['firstLevelId'], cgn['levelRecordCount'], cgn['currentLevelId']))
    level_dir = os.path.dirname(path)
    mismatches = 0
    for r in cgn['levels']:
        used = SELECTIONS if show_all else used_selections(r)
        print('  id %2d %-28s succ %s carry 0x%X skip 0x%X' % (
            r['levelId'], r['levelFileName'], r['successorLevelIds'][:used], r['carryOverMask'], r['skipMask']))
        print('        movies fail %s success %s' % (r['endMovieNumbers'][:used], r['endMovieNumbersVariant'][:used]))
        for z in r['exitZones']:
            if z['radius'] > 0 or show_all:
                print('        exit zone [%d]: centre (%s, %s) radius %s (0x%X) destination (%s, %s)' % (
                    z['index'], lev.q12(z['x']), lev.q12(z['y']), lev.q12(z['radius']), z['radius'] & 0xFFFFFFFF,
                    lev.q12(z['destX']),
                    lev.q12(z['destY'])))
        if show_triggers:
            triggers = level_triggers(level_dir, r['levelFileName'])
            if triggers is None:
                print('        triggers: level file missing')
                continue
            odd = [t for t in triggers if (t[1] == 0) != (t[2] == 0)]
            mismatches += len(odd)
            print('        triggers (faction, selection, variant): %s%s' % (
                ' '.join('(%d,%d,%d)' % t for t in sorted(set(triggers))),
                '  SELECTION 0 != VARIANT 0: ' + ' '.join('(%d,%d,%d)' % t for t in odd) if odd else ''))
    if show_triggers:
        print('  selection 0 == variant 0 on every trigger' if not mismatches else
              '  %d triggers where selection 0 and variant 0 differ' % mismatches)


def catalog(path):
    data = open(path, 'rb').read()
    print('%s: %d records' % (path, len(data) // CATALOG_RECORD))
    for offset in range(0, len(data) - CATALOG_RECORD + 1, CATALOG_RECORD):
        record = parse_catalog_record(data, offset)
        if record['name']:
            print('  %-12s players %s-%s title index %d' % (
                record['name'], record['minPlayers'], record['maxPlayers'], record['titleIndex']))


def main():
    if len(sys.argv) < 3 or sys.argv[1] not in ('dump', 'catalog'):
        print(__doc__)
        sys.exit(2)
    paths = [a for a in sys.argv[2:] if not a.startswith('--')]
    if sys.argv[1] == 'catalog':
        for path in paths:
            catalog(path)
        return
    for path in paths:
        if os.path.isdir(path):
            names = sorted(os.listdir(path))
            if 'campagne.dat' in names:
                catalog(os.path.join(path, 'campagne.dat'))
            files = [os.path.join(path, n) for n in names if n.lower().endswith('.cgn')]
        else:
            files = [path]
        for name in files:
            dump(name, '--triggers' in sys.argv, '--all' in sys.argv)


if __name__ == '__main__':
    main()
