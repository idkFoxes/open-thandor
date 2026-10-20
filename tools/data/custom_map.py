"""Creates and checks custom maps (maps\\<name>\\, single player) and writes broken ones for robustness tests.

usage:
  python tools/data/custom_map.py list GAME_DIR
  python tools/data/custom_map.py derive GAME_DIR ORIGINAL NAME OUTDIR [--title T] [--description D]
                                  [--title-en T] [--description-en D] [--start-xenite N] [--start-tritium N]
  python tools/data/custom_map.py check DIR ...
  python tools/data/custom_map.py corrupt SRC_DIR OUT_DIR KIND

A custom map is the folder maps\\NAME\\ next to the executable with
  NAME.lev   level (docs/level_format.md); catalog identifier (+0x100) "NAME." and field grid path (+0xB0)
             "maps\\NAME\\NAME" (relative to the game directory, without extension, like the stock "level\\asgard")
  NAME.fld   field grid in the loose, expanded form (docs/field_grid_format.md: 0x200 header + w*h*0x80 cells)
  map.ini    UTF-8, section [map] with title, description and optional title.<lang>, description.<lang>

derive takes ORIGINAL (a base name such as asgard or s01_insel, or a path such as level\\hansolo\\s01_insel) from
the game's PCK packages (or a loose level\\*.lev) in GAME_DIR, which is only read, and writes OUTDIR\\maps\\NAME\\.
The .lev keeps every byte of the original except the identifier, the field grid path (moved into its own 0x100-byte
slot appended to the prefix, so the loader's ".fld" rewrite has room for any NAME) and, with --start-xenite /
--start-tritium, the start resources of the active factions. The .fld is the original grid, expanded.

check verifies a maps\\NAME folder; corrupt copies a map folder to OUT_DIR (renamed to OUT_DIR's base name) and
breaks it in one way: KIND is one of CORRUPT_KINDS, or "all" (OUT_DIR is then a parent folder that receives one
map folder bad_<kind> per kind).
"""
import argparse
import os
import re
import shutil
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fld  # noqa: E402
import lev  # noqa: E402
import pck  # noqa: E402

NAME_PATTERN = re.compile(r'^[A-Za-z0-9_-]{1,30}$')  # identifier "NAME." must fit 32 UTF-16 units with its NUL
FIELD_SLOT_SIZE = 0x100  # 128 code units: "maps\" + 30 + "\" + 30 + ".fld" + NUL fits with room to spare
FIELD_EXTENSION_UNITS = 5  # scenario_load.cpp LEVEL_PATH_EXTENSION_UNITS: ".fld" + NUL written after the path
MAX_GRID_SIDE = 1024  # sanity bound for check; stock grids are 67..139 cells per side
INI_NAME = 'map.ini'
CORRUPT_KINDS = {
    'truncated': '.lev cut to 0x400 bytes (shorter than the 0x800-byte fixed prefix)',
    'truncated_fld': '.fld cut in the middle of the cell array',
    'bad_magic': '.lev magic "lev\\0" replaced by "xxx\\0"',
    'bad_magic_fld': '.fld magic "fld\\0" replaced by "xxx\\0"',
    'huge_dims': '.fld header width and height 0x10000 each (size no longer matches)',
    'bad_path': '.lev field grid path offset (+0xB0) points past the end of the file',
    'unterminated_path': '.lev field grid path points at 0x40 non-NUL bytes appended to the file (no terminator)',
    'traversal_path': '.lev field grid path "..\\..\\..\\NAME", outside the game directory',
    'missing_fld': 'the .fld is deleted',
    'bad_ini': 'map.ini is not UTF-8 and has no [map] section',
    'missing_ini': 'map.ini is deleted',
}


def field_path(name):
    return 'maps\\%s\\%s' % (name, name)


# ---------------------------------------------------------------- game data

def _packages(game_dir):
    """(path, data, entries) of every readable PCK in game_dir (PATCH01.PCK of some installs is not a package)."""
    result = []
    for file_name in sorted(os.listdir(game_dir)):
        if not file_name.lower().endswith('.pck'):
            continue
        path = os.path.join(game_dir, file_name)
        try:
            data, items = pck.entries(path)
        except (struct.error, ValueError, UnicodeDecodeError):
            continue
        result.append((path, data, items))
    return result


def original_levels(game_dir):
    """{entry path: loader()} of every .lev in the packages and loose in game_dir\\level (packages win, as in
    Package_LoadEntry)."""
    levels = {}
    for _, data, items in _packages(game_dir):
        for entry in items:
            if entry[0].lower().endswith('.lev') and entry[0].lower() not in {k.lower() for k in levels}:
                levels[entry[0]] = (lambda d=data, e=entry: pck.read_entry(d, e))
    loose = os.path.join(game_dir, 'level')
    if os.path.isdir(loose):
        for root, _, names in os.walk(loose):
            for file_name in sorted(names):
                if file_name.lower().endswith('.lev'):
                    full = os.path.join(root, file_name)
                    rel = os.path.relpath(full, game_dir).replace('/', '\\')
                    if rel.lower() not in {k.lower() for k in levels}:
                        levels[rel] = (lambda p=full: open(p, 'rb').read())
    return levels


def load_entry(game_dir, rel_path):
    """An asset by its game path (e.g. level\\asgard.fld): packages first, then the loose file."""
    for _, data, items in _packages(game_dir):
        for entry in items:
            if entry[0].lower() == rel_path.lower():
                return pck.read_entry(data, entry)
    loose = os.path.join(game_dir, *rel_path.split('\\'))
    if os.path.isfile(loose):
        return open(loose, 'rb').read()
    raise SystemExit('%s: not found in the packages or loose in %s' % (rel_path, game_dir))


def find_original(levels, original):
    wanted = original.replace('/', '\\').lower()
    if wanted.endswith('.lev'):
        wanted = wanted[:-4]
    matches = [key for key in levels
               if key.lower()[:-4] == wanted or key.lower()[:-4].rsplit('\\', 1)[-1] == wanted]
    if not matches:
        raise SystemExit('no original level %r (see the list command)' % original)
    if len(matches) > 1:
        raise SystemExit('%r is ambiguous: %s' % (original, ', '.join(matches)))
    return matches[0]


# ---------------------------------------------------------------- .lev editing

def _put_utf16_slot(buf, offset, text, units):
    raw = text.encode('utf-16le')
    if len(raw) > (units - 1) * 2:
        raise ValueError('%r longer than %d code units' % (text, units - 1))
    buf[offset:offset + units * 2] = raw + bytes(units * 2 - len(raw))


def retarget_level(data, name, start_xenite=None, start_tritium=None):
    """The original LEV image with identifier "NAME." and field grid path maps\\NAME\\NAME in a new 0x100-byte slot
    appended to the prefix (the placement table moves up by 0x100; size, prefix size and +0xB0 follow). Every other
    byte stays. Optional start resources (whole units) for every active faction."""
    level = lev.parse(data)
    problems = lev.check(level, len(data))
    if problems:
        raise SystemExit('original level inconsistent: ' + '; '.join(problems))
    prefix = level['prefixSize']
    out = bytearray(data[:prefix]) + bytearray(FIELD_SLOT_SIZE) + bytearray(data[prefix:])
    _put_utf16_slot(out, prefix, field_path(name), FIELD_SLOT_SIZE // 2)
    new_prefix = prefix + FIELD_SLOT_SIZE
    struct.pack_into('<I', out, 0x04, len(out))
    struct.pack_into('<I', out, 0xB0, prefix)
    struct.pack_into('<I', out, 0xDC, new_prefix)
    _put_utf16_slot(out, 0x100, name + '.', 32)
    active = level['world']['activeFactionCount']
    for faction in range(active):
        slot = 0x200 + faction * 0x20
        if start_xenite is not None:
            struct.pack_into('<I', out, slot + 0x14, start_xenite * 16)
        if start_tritium is not None:
            struct.pack_into('<I', out, slot + 0x18, start_tritium * 16)
    return bytes(out)


# ---------------------------------------------------------------- map.ini

def write_ini(path, values):
    lines = ['; custom map description (UTF-8)', '[map]']
    for key, value in values.items():
        if value is not None:
            lines.append('%s = %s' % (key, ' '.join(str(value).split())))
    with open(path, 'w', encoding='utf-8', newline='\r\n') as handle:
        handle.write('\n'.join(lines) + '\n')


def read_ini(path):
    """{key: value} of section [map]; raises ValueError for a file that is not UTF-8 or has no [map] section."""
    with open(path, 'rb') as handle:
        text = handle.read().decode('utf-8-sig')  # UnicodeDecodeError is a ValueError
    section = None
    values = {}
    for number, line in enumerate(text.splitlines(), 1):
        line = line.strip()
        if not line or line[0] in ';#':
            continue
        if line.startswith('[') and line.endswith(']'):
            section = line[1:-1].strip().lower()
            continue
        if '=' not in line:
            raise ValueError('line %d: no key = value' % number)
        key, value = line.split('=', 1)
        if section == 'map':
            values[key.strip().lower()] = value.strip()
    if section is None and not values:
        raise ValueError('no [map] section')
    if 'title' not in values:
        raise ValueError('[map] has no title')
    return values


# ---------------------------------------------------------------- commands

def cmd_list(args):
    for key in original_levels(args.game_dir):
        print(key)


def cmd_derive(args):
    if not NAME_PATTERN.match(args.name):
        raise SystemExit('NAME must be 1..30 characters A-Z a-z 0-9 _ -')
    levels = original_levels(args.game_dir)
    originals = {key.lower()[:-4].rsplit('\\', 1)[-1] for key in levels}
    if args.name.lower() in originals:
        raise SystemExit('NAME %r collides with an original level' % args.name)
    key = find_original(levels, args.original)
    source = levels[key]()
    source_level = lev.parse(source)
    fld_rel = source_level['paths']['field'] + '.fld'
    grid = load_entry(args.game_dir, fld_rel)
    if grid[:4] != fld.MAGIC:
        raise SystemExit('%s: not an expanded field grid' % fld_rel)
    data = retarget_level(source, args.name, args.start_xenite, args.start_tritium)
    target = os.path.join(args.outdir, 'maps', args.name)
    os.makedirs(target, exist_ok=True)
    with open(os.path.join(target, args.name + '.lev'), 'wb') as handle:
        handle.write(data)
    with open(os.path.join(target, args.name + '.fld'), 'wb') as handle:
        handle.write(grid)
    write_ini(os.path.join(target, INI_NAME), {
        'title': args.title or args.name,
        'description': args.description if args.description is not None else 'Derived from %s' % key,
        'title.en': args.title_en,
        'description.en': args.description_en,
        'source': key})
    print('derived %s -> %s (%s, %d bytes lev, %d bytes fld)' % (key, target, field_path(args.name), len(data),
                                                                   len(grid)))
    problems = check_map(target)
    if problems:
        raise SystemExit('derived map fails check: ' + '; '.join(problems))


def check_map(folder):
    """Problems of a maps\\NAME folder (empty list = loadable as far as the file formats go)."""
    folder = os.path.normpath(folder)
    name = os.path.basename(folder)
    problems = []
    if not NAME_PATTERN.match(name):
        problems.append('folder name %r is not 1..30 characters A-Z a-z 0-9 _ -' % name)
    lev_path = os.path.join(folder, name + '.lev')
    fld_path = os.path.join(folder, name + '.fld')
    ini_path = os.path.join(folder, INI_NAME)
    for path in (lev_path, fld_path, ini_path):
        if not os.path.isfile(path):
            problems.append('missing ' + os.path.basename(path))
    if os.path.isfile(lev_path):
        data = open(lev_path, 'rb').read()
        try:
            level = lev.parse(data)
            problems += ['lev: ' + p for p in lev.check(level, len(data))]
            identifier = level['catalogName']
            if identifier.rstrip('.').lower() != name.lower():
                problems.append('lev identifier %r is not %r' % (identifier, name + '.'))
            path_offset = level['pathOffsets']['field']
            path, terminated = lev.utf16z(data, path_offset) if path_offset < len(data) else ('', False)
            if not terminated:
                problems.append('lev field grid path at 0x%X not terminated' % path_offset)
            elif path.lower() != field_path(name).lower():
                problems.append('lev field grid path %r is not %r' % (path, field_path(name)))
            else:
                # room for the ".fld" rewrite before the next string/table or the prefix end
                end = path_offset + (len(path) + FIELD_EXTENSION_UNITS) * 2
                limit = min([level['prefixSize']] +
                            [off for n, off in level['pathOffsets'].items() if n != 'field' and off > path_offset] +
                            [t['offset'] for t in level['tables'].values() if t['count'] and t['offset'] > path_offset])
                if end > limit:
                    problems.append('lev field grid path slot 0x%X..0x%X has no room for ".fld" (next 0x%X)'
                                    % (path_offset, end, limit))
        except (ValueError, struct.error) as error:
            problems.append('lev: ' + str(error))
    if os.path.isfile(fld_path):
        data = open(fld_path, 'rb').read()
        if len(data) < fld.HEADER:
            problems.append('fld: %d bytes, shorter than the 0x200-byte header' % len(data))
        elif data[:4] != fld.MAGIC:
            problems.append('fld: magic %r is not "fld"' % data[:4])
        else:
            h = fld.header_fields(data)
            if h['converter'] != fld.CONVERTER_VERSION:
                problems.append('fld: converter 0x%08X' % h['converter'])
            if not (4 <= h['width'] <= MAX_GRID_SIDE and 4 <= h['height'] <= MAX_GRID_SIDE):
                problems.append('fld: grid %d x %d out of range' % (h['width'], h['height']))
            else:
                expected = fld.HEADER + h['width'] * h['height'] * fld.CELL
                if len(data) != expected:
                    problems.append('fld: %d bytes, %d x %d grid needs %d' % (len(data), h['width'], h['height'],
                                                                              expected))
                if h['size'] != len(data):
                    problems.append('fld: allocation size %d != file size %d' % (h['size'], len(data)))
    if os.path.isfile(ini_path):
        try:
            values = read_ini(ini_path)
            if not values.get('title'):
                problems.append('map.ini: empty title')
        except ValueError as error:
            problems.append('map.ini: %s' % error)
    return problems


def cmd_check(args):
    bad = 0
    for folder in args.dirs:
        problems = check_map(folder)
        bad += bool(problems)
        print('%-40s %s' % (folder, 'ok' if not problems else '; '.join(problems)))
    sys.exit(1 if bad else 0)


def copy_renamed(src, dst):
    """Copies map folder src to dst; when the base names differ the files, identifier and field path are renamed."""
    src = os.path.normpath(src)
    dst = os.path.normpath(dst)
    old, new = os.path.basename(src), os.path.basename(dst)
    if not NAME_PATTERN.match(new):
        raise SystemExit('OUT_DIR base name %r is not a valid map name' % new)
    if os.path.exists(dst):
        # only a previous map folder is replaced, never an arbitrary directory
        foreign = [n for n in os.listdir(dst) if n != INI_NAME and not n.lower().endswith(('.lev', '.fld'))]
        if foreign or not os.path.isdir(dst):
            raise SystemExit('%s exists and is not a map folder (%s)' % (dst, ', '.join(foreign[:3])))
        shutil.rmtree(dst)
    os.makedirs(dst)
    for file_name in os.listdir(src):
        stem, ext = os.path.splitext(file_name)
        target = new + ext if stem.lower() == old.lower() and ext.lower() in ('.lev', '.fld') else file_name
        shutil.copyfile(os.path.join(src, file_name), os.path.join(dst, target))
    lev_path = os.path.join(dst, new + '.lev')
    data = bytearray(open(lev_path, 'rb').read())
    level = lev.parse(bytes(data))
    _put_utf16_slot(data, 0x100, new + '.', 32)
    off = level['pathOffsets']['field']
    slot_units = FIELD_SLOT_SIZE // 2 if off + FIELD_SLOT_SIZE <= level['prefixSize'] else 32
    _put_utf16_slot(data, off, field_path(new), slot_units)
    open(lev_path, 'wb').write(bytes(data))
    return new


def corrupt(src, dst, kind):
    if kind not in CORRUPT_KINDS:
        raise SystemExit('unknown KIND %r (one of %s, all)' % (kind, ', '.join(CORRUPT_KINDS)))
    name = copy_renamed(src, dst)
    lev_path = os.path.join(dst, name + '.lev')
    fld_path = os.path.join(dst, name + '.fld')
    ini_path = os.path.join(dst, INI_NAME)

    def patch(path, func):
        data = bytearray(open(path, 'rb').read())
        data = func(data)
        open(path, 'wb').write(bytes(data))

    if kind == 'truncated':
        patch(lev_path, lambda d: d[:0x400])
    elif kind == 'truncated_fld':
        patch(fld_path, lambda d: d[:fld.HEADER + (len(d) - fld.HEADER) // 2 + 3])
    elif kind == 'bad_magic':
        patch(lev_path, lambda d: b'xxx\0' + d[4:])
    elif kind == 'bad_magic_fld':
        patch(fld_path, lambda d: b'xxx\0' + d[4:])
    elif kind == 'huge_dims':
        def huge(d):
            struct.pack_into('<II', d, 0xB8, 0x10000, 0x10000)
            return d
        patch(fld_path, huge)
    elif kind == 'bad_path':
        def past_end(d):
            struct.pack_into('<I', d, 0xB0, len(d) + 0x100)
            return d
        patch(lev_path, past_end)
    elif kind == 'unterminated_path':
        def unterminated(d):
            # 0x40 non-NUL bytes appended after the placements (allocation size follows, placements stay
            # intact) and the field grid path pointed at them: no terminator before the end of the image
            start = len(d)
            d += 'A'.encode('utf-16le') * 0x20
            struct.pack_into('<I', d, 0x04, len(d))
            struct.pack_into('<I', d, 0xB0, start)
            return d
        patch(lev_path, unterminated)
    elif kind == 'traversal_path':
        def traversal(d):
            off = struct.unpack_from('<I', d, 0xB0)[0]
            text = '..\\..\\..\\' + name
            raw = text.encode('utf-16le')
            d[off:off + len(raw) + 2] = raw + b'\0\0'
            return d
        patch(lev_path, traversal)
    elif kind == 'missing_fld':
        os.remove(fld_path)
    elif kind == 'bad_ini':
        open(ini_path, 'wb').write(b'title = \xff\xfe broken \xc3\x28\nno section, no equals sign\n')
    elif kind == 'missing_ini':
        os.remove(ini_path)
    else:
        raise SystemExit('unknown KIND %r (one of %s, all)' % (kind, ', '.join(CORRUPT_KINDS)))
    problems = check_map(dst)
    print('%-18s %s -> %s' % (kind, dst, '; '.join(problems) if problems else 'check still ok'))


def cmd_corrupt(args):
    if args.kind == 'all':
        for kind in CORRUPT_KINDS:
            corrupt(args.src_dir, os.path.join(args.out_dir, 'bad_' + kind), kind)
    else:
        corrupt(args.src_dir, args.out_dir, args.kind)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest='command', required=True)
    p = sub.add_parser('list', help='original levels of a game directory')
    p.add_argument('game_dir')
    p.set_defaults(func=cmd_list)
    p = sub.add_parser('derive', help='custom map from an original level')
    p.add_argument('game_dir')
    p.add_argument('original')
    p.add_argument('name')
    p.add_argument('outdir')
    p.add_argument('--title')
    p.add_argument('--description')
    p.add_argument('--title-en')
    p.add_argument('--description-en')
    p.add_argument('--start-xenite', type=int, help='start Xenite of every active faction (whole units)')
    p.add_argument('--start-tritium', type=int, help='start Tritium of every active faction (whole units)')
    p.set_defaults(func=cmd_derive)
    p = sub.add_parser('check', help='verify maps\\NAME folders')
    p.add_argument('dirs', nargs='+')
    p.set_defaults(func=cmd_check)
    p = sub.add_parser('corrupt', help='broken copy of a map folder; kinds: ' + ', '.join(CORRUPT_KINDS) + ', all')
    p.add_argument('src_dir')
    p.add_argument('out_dir')
    p.add_argument('kind')
    p.set_defaults(func=cmd_corrupt)
    args = parser.parse_args()
    for value in (getattr(args, 'start_xenite', None), getattr(args, 'start_tritium', None)):
        if value is not None and not 0 <= value <= 0x0FFFFFFF:
            raise SystemExit('start resources must be 0..268435455')
    args.func(args)


if __name__ == '__main__':
    main()
