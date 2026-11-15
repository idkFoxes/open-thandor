"""Builds and checks custom maps (map editor projects maps\\<name>.thmap, single player) and writes broken ones
for robustness tests.

usage:
  python tools/data/custom_map.py list GAME_DIR
  python tools/data/custom_map.py build GAME_DIR ORIGINAL NAME OUTDIR [--legacy] [--title T] [--description D]
                                  [--description-en D] [--author A] [--start-xenite N] [--start-tritium N]
                                  [--custom-terrain]
  python tools/data/custom_map.py check FILE.thmap ...
  python tools/data/custom_map.py corrupt SRC.thmap OUT_DIR KIND

A custom map is the file maps\\NAME.thmap next to the executable (docs/custom_maps.md), in one of the two forms the
map editor writes:
  ZIP     map.ini at the root ("[map]", format = 1, level_entry, field_entry, string_entry, display_name, author)
  legacy  stored PCK with editor/project.txt ("THANDOR_MAP_EDITOR_PROJECT_V3", key=value, values %-encoded)
with level/<x>.lev (field grid path +0xB0 "level\\<x>"), level/<x>.fld (expanded field grid) and optionally
level/<x>.str (level text) and gfx/boden/* terrain files.

build takes ORIGINAL (a base name such as asgard or s01_insel, or a path such as level\\hansolo\\s01_insel) from
the game's PCK packages (or a loose level\\*.lev) in GAME_DIR, which is only read, and writes OUTDIR\\maps\\NAME.thmap
(ZIP, or with --legacy a stored PCK V3). The .lev keeps every byte of the original except the identifier
("NAME."), the field grid path (level\\NAME, in its own 0x100-byte slot appended to the prefix so the loader's
".fld" rewrite has room) and, with --start-xenite / --start-tritium, the start resources of the active factions.
--custom-terrain copies the level's ground textures (+0xB4 path: .dat, .pal, a..z.gfx) into the project as
gfx/boden/edit_NAME* and points +0xB4 at the copy (in a second appended slot), so the map only loads while the
game serves the project's files.

check verifies .thmap files as far as the file formats go (the LEV identifier +0x100 is not checked: the game
replaces it by "maps\NAME." with NAME the file stem) (the game's own checks are stricter: CustomMap_Validate
and the field grid check); corrupt copies a .thmap to OUT_DIR\\bad_<kind>.thmap broken in one way: KIND is one of
CORRUPT_KINDS, or "all".
"""
import argparse
import io
import os
import re
import struct
import sys
import zipfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fld  # noqa: E402
import lev  # noqa: E402
import pck  # noqa: E402

NAME_PATTERN = re.compile(r'^[A-Za-z0-9_-]{1,30}$')  # identifier "NAME." must fit 32 UTF-16 units with its NUL
SLOT_SIZE = 0x100  # 128 code units for an appended path string
FIELD_EXTENSION_UNITS = 5  # ".fld" + NUL written after the path by the loader
MAX_FILE_BYTES = 32 * 1024 * 1024  # kMapProjectMaxArchiveBytes
MAX_ENTRY_BYTES = 16 * 1024 * 1024
MAX_GRID_SIDE = 1024
ZIP_MANIFEST = 'map.ini'
LEGACY_MANIFEST = 'editor/project.txt'
LEGACY_MAGICS = ('THANDOR_MAP_EDITOR_PROJECT_V1', 'THANDOR_MAP_EDITOR_PROJECT_V2', 'THANDOR_MAP_EDITOR_PROJECT_V3')
STR_STRINGS = 16
COUNTRY_GERMAN, COUNTRY_ENGLISH = 49, 44
CORRUPT_KINDS = {
    'truncated': 'the file cut in half (ZIP: no central directory; PCK: entry walk past the end)',
    'traversal': 'an extra entry ../../evil.gfx',
    'bad_lev': 'LEV magic "lev\\0" replaced by "xxx\\0"',
    'bad_fld': 'FLD header width and height 55 (side not 4k+3 in 59..139)',
    'field_mismatch': 'manifest field_entry renamed to level/other.fld (entry renamed too)',
    'no_manifest': 'manifest entry removed',
    'oversized': 'padded to 32 MiB + 1 byte',
}


def field_path(name):
    return 'level\\%s' % name


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
    """{entry path: loader()} of every .lev in the packages and loose in game_dir\\level (packages win)."""
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


def load_entries(game_dir, rel_paths):
    """{lowercase path: bytes} of the assets rel_paths (game paths such as level\\asgard.fld): packages first, then
    loose files. Missing ones are left out."""
    wanted = {p.lower() for p in rel_paths}
    found = {}
    for _, data, items in _packages(game_dir):
        for entry in items:
            key = entry[0].lower()
            if key in wanted and key not in found:
                found[key] = pck.read_entry(data, entry)
    for key in wanted - set(found):
        loose = os.path.join(game_dir, *key.split('\\'))
        if os.path.isfile(loose):
            found[key] = open(loose, 'rb').read()
    return found


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


def retarget_level(data, name, start_xenite=None, start_tritium=None, ground=None):
    """The original LEV image with identifier "NAME." and field grid path level\\NAME (and with ground the ground
    texture path +0xB4) in new 0x100-byte slots appended to the prefix (the placement table moves up; size, prefix
    size and the path offsets follow). Every other byte stays. Optional start resources (whole units) for every
    active faction."""
    level = lev.parse(data)
    problems = lev.check(level, len(data))
    if problems:
        raise SystemExit('original level inconsistent: ' + '; '.join(problems))
    prefix = level['prefixSize']
    slots = [(0xB0, field_path(name))] + ([(0xB4, ground)] if ground else [])
    out = bytearray(data[:prefix]) + bytearray(SLOT_SIZE * len(slots)) + bytearray(data[prefix:])
    for index, (pointer, text) in enumerate(slots):
        _put_utf16_slot(out, prefix + index * SLOT_SIZE, text, SLOT_SIZE // 2)
        struct.pack_into('<I', out, pointer, prefix + index * SLOT_SIZE)
    struct.pack_into('<I', out, 0x04, len(out))
    struct.pack_into('<I', out, 0xDC, prefix + SLOT_SIZE * len(slots))
    _put_utf16_slot(out, 0x100, name + '.', 32)
    for faction in range(level['world']['activeFactionCount']):
        slot = 0x200 + faction * 0x20
        if start_xenite is not None:
            struct.pack_into('<I', out, slot + 0x14, start_xenite * 16)
        if start_tritium is not None:
            struct.pack_into('<I', out, slot + 0x18, start_tritium * 16)
    return bytes(out)


# ---------------------------------------------------------------- STR (level text)

def _str_string(text):
    raw = text.encode('utf-16le') + b'\0\0'
    return raw + bytes(-len(raw) % 4)


def build_str(blocks):
    """A STR text resource: blocks is [(country code, [strings])]; 16 strings per block (missing ones empty)."""
    out = bytearray(0x200)
    out[0:4] = b'str\0'
    struct.pack_into('<II', out, 0x08, 1, 0)
    struct.pack_into('<I', out, 0xB0, len(blocks))
    for country, strings in blocks:
        strings = (list(strings) + [''] * STR_STRINGS)[:STR_STRINGS]
        start = len(out)
        out += bytes(0x10 + 4 * len(strings))
        struct.pack_into('<III', out, start + 4, len(strings), country, 0)
        for index, text in enumerate(strings):
            struct.pack_into('<I', out, start + 0x10 + 4 * index, len(out) - start)
            out += _str_string(text)
        struct.pack_into('<I', out, start, len(out) - start)
    struct.pack_into('<I', out, 0x04, len(out))
    return bytes(out)


def parse_str(data):
    """{country: [strings]}; raises ValueError for an inconsistent resource."""
    if len(data) < 0x200 or data[:4] != b'str\0':
        raise ValueError('bad STR magic')
    if struct.unpack_from('<I', data, 4)[0] != len(data):
        raise ValueError('STR allocation size mismatch')
    count = struct.unpack_from('<I', data, 0xB0)[0]
    if count == 0 or count > (len(data) - 0x200) // 0x10:
        raise ValueError('STR locale count %d' % count)
    cursor, result = 0x200, {}
    for _ in range(count):
        size, strings, country = struct.unpack_from('<III', data, cursor)
        if size < 0x10 + 4 * strings or cursor + size > len(data) or size & 3:
            raise ValueError('STR locale block size')
        offsets = list(struct.unpack_from('<%dI' % strings, data, cursor + 0x10)) + [size]
        texts = []
        for index in range(strings):
            begin, end = offsets[index], offsets[index + 1]
            if begin < 0x10 + 4 * strings or begin >= size or begin & 1 or end < begin:
                raise ValueError('STR string offset')
            texts.append(data[cursor + begin:cursor + end].decode('utf-16le', 'replace').split('\0')[0])
        result[country] = texts
        cursor += size
    if cursor != len(data):
        raise ValueError('STR block walk does not end at the allocation size')
    return result


# ---------------------------------------------------------------- archives

def _escape(value):
    return ''.join(c if c.isalnum() or c in ' _-./' else ''.join('%%%02X' % b for b in c.encode('utf-8'))
                   for c in value)


def _unescape(value):
    raw, i = bytearray(), 0
    while i < len(value):
        if value[i] == '%' and i + 2 < len(value) and re.match(r'[0-9A-Fa-f]{2}', value[i + 1:i + 3]):
            raw.append(int(value[i + 1:i + 3], 16))
            i += 3
        else:
            raw += value[i].encode('utf-8')
            i += 1
    return raw.decode('utf-8', 'replace')


def zip_manifest(values):
    lines = ['[map]', 'format = 1'] + ['%s = %s' % (k, ' '.join(v.split())) for k, v in values.items() if v]
    return ('\r\n'.join(lines) + '\r\n').encode('utf-8')


def legacy_manifest(values):
    lines = [LEGACY_MAGICS[-1]] + ['%s=%s' % (k, _escape(v)) for k, v in values.items() if v is not None]
    return ('\n'.join(lines) + '\n').encode('utf-8')


def write_zip(entries):
    buffer = io.BytesIO()
    with zipfile.ZipFile(buffer, 'w', zipfile.ZIP_DEFLATED) as archive:
        for path, data in entries:
            archive.writestr(path, data)
    return buffer.getvalue()


def write_stored_pck(entries):
    """A stored (method 1) PCK as the map editor writes legacy projects: entry names with '\\', payloads padded to
    four bytes (stored size), unpacked size the real length."""
    body = bytearray()
    for path, data in entries:
        name = path.replace('/', '\\').encode('utf-16le')
        if len(name) >= 0x1EC:
            raise SystemExit('entry path too long: ' + path)
        padded = bytes(data) + bytes(-len(data) % 4)
        header = bytearray(0x200)
        header[0:len(name)] = name
        struct.pack_into('<II4sII', header, 0x1EC, 0, len(data), bytes(data[:4]).ljust(4, b'\0'), len(padded), 1)
        body += header + padded
    out = bytearray(0x200)
    out[0:4] = b'pck\0'
    struct.pack_into('<III', out, 4, 0x200 + len(body), 1, 0x10000)
    label = 'THANDOR MAP EDITOR'.encode('utf-16le')
    out[0x30:0x30 + len(label)] = label
    out[0x70:0x70 + len(label)] = label
    struct.pack_into('<I', out, 0xB0, len(entries))
    return bytes(out + body)


def read_project(data):
    """(zip?, [(normalized path, bytes)] in archive order, {manifest key: value}); raises ValueError."""
    if len(data) > MAX_FILE_BYTES:
        raise ValueError('file too large (%d bytes, at most %d)' % (len(data), MAX_FILE_BYTES))
    is_zip = data[:4] in (b'PK\x03\x04', b'PK\x05\x06')
    entries = []
    if is_zip:
        try:
            with zipfile.ZipFile(io.BytesIO(data)) as archive:
                for info in archive.infolist():
                    if info.is_dir():
                        continue
                    if info.file_size > MAX_ENTRY_BYTES:
                        raise ValueError('entry %s too large' % info.filename)
                    entries.append((info.filename, archive.read(info)))
        except (zipfile.BadZipFile, zipfile.LargeZipFile, EOFError) as error:
            raise ValueError('invalid ZIP: %s' % error)
    else:
        if len(data) < 0x200 or data[:4] != b'pck\0':
            raise ValueError('neither a ZIP nor a PCK')
        if struct.unpack_from('<I', data, 4)[0] != len(data):
            raise ValueError('PCK size mismatch')
        count = struct.unpack_from('<I', data, 0xB0)[0]
        if count > (len(data) - 0x200) // 0x200:
            raise ValueError('PCK entry count %d' % count)
        offset = 0x200
        for _ in range(count):
            name = data[offset:offset + 0x1EC].decode('utf-16le', 'replace').split('\0')[0]
            _, unpacked, _, stored, method = struct.unpack_from('<II4sII', data, offset + 0x1EC)
            start = offset + 0x200
            if start + stored > len(data):
                raise ValueError('PCK entry %s past the end' % name)
            if unpacked > MAX_ENTRY_BYTES:
                raise ValueError('entry %s too large' % name)
            entry = (name, '', unpacked, stored, method, start)
            entries.append((name, pck.read_entry(data, entry)))
            offset = start + stored
        if offset != len(data):
            raise ValueError('PCK entry walk does not end at the file size')
    normalized = []
    seen = set()
    for name, payload in entries:
        path = name.replace('\\', '/').lower()
        if not path or path.startswith('/') or ':' in path or any(p in ('', '.', '..') for p in path.split('/')):
            raise ValueError('invalid entry path %r' % name)
        if path in seen:
            raise ValueError('duplicate entry %s' % path)
        seen.add(path)
        normalized.append((path, payload))
    files = dict(normalized)
    manifest_name = ZIP_MANIFEST if is_zip else LEGACY_MANIFEST
    if manifest_name not in files:
        raise ValueError('missing ' + manifest_name)
    text = files[manifest_name]
    if is_zip and b'\0' in text:
        raise ValueError('NUL in map.ini')
    lines = text.split(b'\0')[0].decode('utf-8-sig', 'replace').split('\n')
    first = lines[0].strip()
    if (is_zip and first != '[map]') or (not is_zip and first not in LEGACY_MAGICS):
        raise ValueError('unsupported manifest %r' % first[:40])
    values = {}
    for line in lines[1:]:
        line = line.rstrip('\r')
        stripped = line.strip()
        if not stripped or stripped[0] in ';#':
            continue
        if '=' not in stripped:
            if is_zip:
                raise ValueError('map.ini line without =')
            continue
        key, value = line.split('=', 1)
        values[key.strip()] = _unescape(value.strip() if is_zip else value)
    if is_zip and values.get('format') != '1':
        raise ValueError('map.ini format is not 1')
    return is_zip, normalized, values


# ---------------------------------------------------------------- commands

def cmd_list(args):
    for key in original_levels(args.game_dir):
        print(key)


def cmd_build(args):
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
    grid = load_entries(args.game_dir, [fld_rel]).get(fld_rel.lower())
    if grid is None or grid[:4] != fld.MAGIC:
        raise SystemExit('%s: missing or not an expanded field grid' % fld_rel)
    entries = []
    ground = None
    if args.custom_terrain:
        base = source_level['paths']['groundTexture']
        ground = 'gfx\\boden\\edit_' + args.name.lower()
        suffixes = ['.dat', '.pal'] + [chr(ord('a') + i) + '.gfx' for i in range(26)]
        found = load_entries(args.game_dir, [base + s for s in suffixes])
        for suffix in suffixes:
            data = found.get((base + suffix).lower())
            if data is not None:
                entries.append(((ground + suffix).replace('\\', '/'), data))
        if not entries:
            raise SystemExit('no ground texture files %s* in %s' % (base, args.game_dir))
    data = retarget_level(source, args.name, args.start_xenite, args.start_tritium, ground)
    title = args.title or args.name
    description = args.description if args.description is not None else 'Abgeleitet von %s' % key
    blocks = [(COUNTRY_GERMAN, [' ' + title, description])]
    if args.description_en is not None:
        blocks.append((COUNTRY_ENGLISH, [' ' + title, args.description_en]))
    stem = 'level/' + args.name.lower()
    entries = [(stem + '.lev', data), (stem + '.fld', grid), (stem + '.str', build_str(blocks))] + entries
    values = {'level_entry': stem + '.lev', 'field_entry': stem + '.fld', 'string_entry': stem + '.str',
              'display_name': args.title or '', 'author': args.author or ''}
    if args.legacy:
        values = dict([('map_name', args.name)] + list(values.items()))
        blob = write_stored_pck([(LEGACY_MANIFEST, legacy_manifest(values))] + entries)
    else:
        blob = write_zip([(ZIP_MANIFEST, zip_manifest(values))] + entries)
    target_dir = os.path.join(args.outdir, 'maps')
    os.makedirs(target_dir, exist_ok=True)
    target = os.path.join(target_dir, args.name + '.thmap')
    with open(target, 'wb') as handle:
        handle.write(blob)
    print('built %s -> %s (%s, %d entries, %d bytes)' % (key, target, 'legacy PCK V3' if args.legacy else 'ZIP',
                                                         len(entries) + 1, len(blob)))
    problems = check_project(target)
    if problems:
        raise SystemExit('built map fails check: ' + '; '.join(problems))


def check_project(path):
    """Problems of a .thmap file (empty list = loadable as far as the file formats go)."""
    name = os.path.splitext(os.path.basename(path))[0]
    problems = []
    if not NAME_PATTERN.match(name):
        problems.append('file name %r is not 1..30 characters A-Z a-z 0-9 _ -' % name)
    try:
        data = open(path, 'rb').read()
        is_zip, entries, values = read_project(data)
    except (OSError, ValueError, struct.error) as error:
        return problems + [str(error)]
    files = dict(entries)
    level_entry = values.get('level_entry', '').replace('\\', '/').lower()
    field_entry = values.get('field_entry', '').replace('\\', '/').lower()
    string_entry = values.get('string_entry', '').replace('\\', '/').lower()
    if not (level_entry.startswith('level/') and level_entry.endswith('.lev') and
            field_entry.startswith('level/') and field_entry.endswith('.fld') and
            (not string_entry or string_entry.endswith('.str'))):
        return problems + ['manifest entries %r %r %r not level/*.lev, level/*.fld, *.str'
                           % (level_entry, field_entry, string_entry)]
    for entry in (level_entry, field_entry) + ((string_entry,) if string_entry else ()):
        if entry not in files:
            problems.append('missing entry ' + entry)
    if problems:
        return problems
    lev_data = files[level_entry]
    try:
        level = lev.parse(lev_data)
        problems += ['lev: ' + p for p in lev.check(level, len(lev_data))]
        offset = level['pathOffsets']['field']
        path_text, terminated = lev.utf16z(lev_data, offset) if offset < len(lev_data) else ('', False)
        if not terminated:
            problems.append('lev field grid path at 0x%X not terminated' % offset)
        elif (path_text + '.fld').replace('\\', '/').lower() != field_entry:
            problems.append('lev field grid path %r does not name %s' % (path_text, field_entry))
        else:
            end = offset + (len(path_text) + FIELD_EXTENSION_UNITS) * 2
            limit = min([level['prefixSize']] +
                        [off for n, off in level['pathOffsets'].items() if n != 'field' and off > offset] +
                        [t['offset'] for t in level['tables'].values() if t['count'] and t['offset'] > offset])
            if end > limit:
                problems.append('lev field grid path slot 0x%X..0x%X has no room for ".fld"' % (offset, end))
    except (ValueError, struct.error, KeyError) as error:
        problems.append('lev: %s' % error)
    grid = files[field_entry]
    if len(grid) < fld.HEADER or grid[:4] != fld.MAGIC:
        problems.append('fld: not an expanded field grid')
    else:
        h = fld.header_fields(grid)
        if h['converter'] not in (fld.CONVERTER_VERSION, 0x00070001):
            problems.append('fld: converter 0x%08X' % h['converter'])
        if not (4 <= h['width'] <= MAX_GRID_SIDE and 4 <= h['height'] <= MAX_GRID_SIDE):
            problems.append('fld: grid %d x %d out of range' % (h['width'], h['height']))
        elif len(grid) != fld.HEADER + h['width'] * h['height'] * fld.CELL or h['size'] != len(grid):
            problems.append('fld: %d bytes do not match the %d x %d grid' % (len(grid), h['width'], h['height']))
    if string_entry:
        try:
            parse_str(files[string_entry])
        except (ValueError, struct.error) as error:
            problems.append('str: %s' % error)
    for path_name, payload in entries:
        if path_name.startswith('gfx/boden/') and path_name.endswith(('.gfx', '.pal')):
            magic = b'gfx\0' if path_name.endswith('.gfx') else b'pal\0'
            if len(payload) < 8 or payload[:4] != magic or struct.unpack_from('<I', payload, 4)[0] != len(payload):
                problems.append('%s: bad magic or size (not served)' % path_name)
    return problems


def cmd_check(args):
    bad = 0
    for path in args.files:
        problems = check_project(path)
        bad += bool(problems)
        print('%-40s %s' % (path, 'ok' if not problems else '; '.join(problems)))
    sys.exit(1 if bad else 0)


def corrupt(src, out_dir, kind):
    if kind not in CORRUPT_KINDS:
        raise SystemExit('unknown KIND %r (one of %s, all)' % (kind, ', '.join(CORRUPT_KINDS)))
    data = open(src, 'rb').read()
    is_zip, entries, values = read_project(data)
    name = 'bad_' + kind
    manifest_name = ZIP_MANIFEST if is_zip else LEGACY_MANIFEST
    level_entry, field_entry = values['level_entry'], values['field_entry']
    entries = [e for e in entries if e[0] != manifest_name]

    def edit(entry, func):
        return [(p, func(bytearray(d)) if p == entry else d) for p, d in entries]

    blob = None
    if kind == 'truncated':
        blob = data[:len(data) // 2]
    elif kind == 'traversal':
        entries.append(('../../evil.gfx', b'gfx\0' + struct.pack('<I', 8)))
    elif kind == 'bad_lev':
        entries = edit(level_entry, lambda d: b'xxx\0' + d[4:])
    elif kind == 'bad_fld':
        def side(d):
            struct.pack_into('<II', d, 0xB8, 55, 55)
            return bytes(d)
        entries = edit(field_entry, side)
    elif kind == 'field_mismatch':
        entries = [('level/other.fld' if p == field_entry else p, d) for p, d in entries]
        values['field_entry'] = 'level/other.fld'
    if blob is None:
        manifest = zip_manifest({k: v for k, v in values.items() if k != 'format'}) if is_zip \
            else legacy_manifest(values)
        if kind != 'no_manifest':
            entries.insert(0, (manifest_name, manifest))
        if is_zip:
            buffer = io.BytesIO()
            with zipfile.ZipFile(buffer, 'w', zipfile.ZIP_DEFLATED) as archive:
                for path, payload in entries:
                    archive.writestr(zipfile.ZipInfo(path), payload)  # keeps "../" (writestr(str) strips it)
            blob = buffer.getvalue()
        else:
            blob = write_stored_pck(entries)
        if kind == 'oversized':
            blob += bytes(MAX_FILE_BYTES + 1 - len(blob))  # the game rejects by file size before reading
    os.makedirs(out_dir, exist_ok=True)
    target = os.path.join(out_dir, name + '.thmap')
    with open(target, 'wb') as handle:
        handle.write(blob)
    problems = check_project(target)
    print('%-16s %s -> %s' % (kind, target, '; '.join(problems) if problems else 'check still ok'))


def cmd_corrupt(args):
    for kind in (CORRUPT_KINDS if args.kind == 'all' else [args.kind]):
        corrupt(args.src, args.out_dir, kind)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest='command', required=True)
    p = sub.add_parser('list', help='original levels of a game directory')
    p.add_argument('game_dir')
    p.set_defaults(func=cmd_list)
    p = sub.add_parser('build', help='.thmap custom map from an original level')
    p.add_argument('game_dir')
    p.add_argument('original')
    p.add_argument('name')
    p.add_argument('outdir')
    p.add_argument('--legacy', action='store_true', help='legacy stored PCK (V3) instead of a ZIP')
    p.add_argument('--title', help='display_name (else the STR title, which is the name)')
    p.add_argument('--description', help='German description (STR string 1)')
    p.add_argument('--description-en', help='English description (an extra STR block, country 44)')
    p.add_argument('--author')
    p.add_argument('--start-xenite', type=int, help='start Xenite of every active faction (whole units)')
    p.add_argument('--start-tritium', type=int, help='start Tritium of every active faction (whole units)')
    p.add_argument('--custom-terrain', action='store_true',
                   help='ship the ground textures in the project under gfx/boden/edit_NAME*')
    p.set_defaults(func=cmd_build)
    p = sub.add_parser('check', help='verify .thmap files')
    p.add_argument('files', nargs='+')
    p.set_defaults(func=cmd_check)
    p = sub.add_parser('corrupt', help='broken copy of a .thmap; kinds: ' + ', '.join(CORRUPT_KINDS) + ', all')
    p.add_argument('src')
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
