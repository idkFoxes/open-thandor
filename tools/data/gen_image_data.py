"""Generates the data of the original image as C, so the executable no longer needs it.

Every run of data bytes between original code in 0x401000-0x58C000 becomes one packed struct
(g_ImageData_<start>) with one named member per object, in original order: code that walks from
one object into its neighbour keeps working. Members:
  - strings from the Ghidra export as literals (word[] = L"...", char[] = "..."),
  - everything else as dword arrays counted from the object's own start, with pointers written as
    symbols: an original function entry -> (dword)CFunction, a known data anchor -> the address of
    the member (or byte offset) that now holds it; other values stay numbers,
  - all-zero objects as {0}; gaps (padding, code inside a block) as zero byte arrays.

Outputs:
  include/thandor/generated/image_data.h   member layouts, THANDOR_IMAGE_0x<address> for every address
                                           used by the address macros, tables for the self-test
  src/generated/image_data.c               the initialized blocks
  <work>/image_pointers.tsv                every converted pointer, for review
Needs globalmap.txt, layout.tsv and members.tsv in the work directory (globalmap.py, layout.py)."""
import bisect
import json
import os
import re
import struct

import common

NUMERIC = re.compile(r'^(?:const )?(?:byte|word|dword|qword|short|ushort|int|uint|sdword|char|undefined[1248]?|u?int(?:8|16|32|64)_t|'
                     r'Q\d+|float|double|long|ulong|longlong|ulonglong|SoftwareBgraWordLanes|PackedArgb32)\s*'
                     r'(?:\(\*\)\[[^\]]*\](?:\[[^\]]*\])*)?\s*\*?$')
HEADER = ('/*\n * Open Thandor\n * Project: https://github.com/idkFoxes/open-thandor/tree/main\n'
          ' * File: https://github.com/idkFoxes/open-thandor/blob/main/%s\n */\n\n')

args = common.parse_arguments(__doc__)
START, END = common.TEXT_START, common.TEXT_END
text = common.load_text(args.original)
code = common.code_mask(text, common.instruction_starts(args.asm), args.work)
funcs = common.function_map()
macros = common.address_macros()
sizes = common.global_sizes(args.work)
types = {n: t for n, (t, _) in macros.items()}

def byte_at(addr):
    return 0 if not START <= addr < END or code[addr - START] else text[addr - START]

def dword_at(addr):
    return byte_at(addr) | byte_at(addr + 1) << 8 | byte_at(addr + 2) << 16 | byte_at(addr + 3) << 24

# ---- blocks: runs of non-code bytes, dword aligned, merged across gaps shorter than 8 bytes, and
# only those holding something the code can name
runs = []
i = 0
while i < END - START:
    if code[i]:
        i += 1
        continue
    j = i
    while j < END - START and not code[j]:
        j += 1
    runs.append([START + i, START + j])
    i = j
blocks = []
for a, b in runs:
    a, b = a & ~3, (b + 3) & ~3
    if blocks and a - blocks[-1][1] < 8:
        blocks[-1][1] = max(blocks[-1][1], b)
    else:
        blocks.append([a, b])
wanted = set(a for _, (a, _) in sizes.items())
for line in open(os.path.join(args.work, 'members.tsv'), encoding='utf-8'):
    wanted.add(int(line.split('\t')[0], 16))
wanted.update(a for a, _ in common.ghidra_labels())
wanted = sorted(a for a in wanted if START <= a < END)

def holds_wanted(block):
    k = bisect.bisect_left(wanted, block[0])
    return k < len(wanted) and wanted[k] < block[1]

blocks = [blk for blk in blocks if holds_wanted(blk)]
block_starts = [b[0] for b in blocks]

def block_of(addr):
    k = bisect.bisect_right(block_starts, addr) - 1
    return k if k >= 0 and blocks[k][0] <= addr < blocks[k][1] else None

def block_name(k):
    return 'g_ImageData_%08X' % blocks[k][0]

# ---- objects (layout.tsv) and the names a pointer target may carry
objects = []
declared_sizes = {}  # start -> size of the declared type (the extent may run on into alignment padding)
for line in open(os.path.join(args.work, 'layout.tsv'), encoding='utf-8'):
    a, size, declared, name, kind, nonzero = line.rstrip('\n').split('\t')
    objects.append((int(a, 16), int(size), name))
    declared_sizes[int(a, 16)] = int(declared)
object_starts = [o[0] for o in objects]

def containing_object(addr):
    k = bisect.bisect_right(object_starts, addr) - 1
    return objects[k] if k >= 0 and objects[k][0] <= addr < objects[k][0] + objects[k][1] else None

anchors = set(a for _, (a, _) in sizes.items())
for line in open(os.path.join(args.work, 'members.tsv'), encoding='utf-8'):
    anchors.add(int(line.split('\t')[0], 16))
anchors.update(a for a, _ in common.ghidra_labels())
strings = {}
for name in ('symbols.jsonl', 'strings.jsonl'):
    for j in map(json.loads, open(os.path.join(common.REPO, 'ghidra', 'export', name))):
        anchors.add(int(j['address'], 16))
        if name == 'strings.jsonl':
            strings[int(j['address'], 16)] = (j['type'], j['value'])
anchors = set(a for a in anchors if block_of(a) is not None and not code[a - START])

# strings that start inside an object (typically the next string of a string list) become their
# own object, unless the object has a struct type
label_names = {}
for a, n in common.ghidra_labels():
    label_names.setdefault(a, n)
split = []
for a in sorted(strings):
    owner = containing_object(a)
    if owner is None or owner[0] == a:
        continue
    t = types.get(owner[2], '')
    if t and not re.match(r'^(?:word|char|byte|uint16_t|uint8_t|int8_t|undefined[12]?)\s*(?:\*|\(\*\)\[)', t.strip()):
        continue
    # the next string of a list follows a terminator; otherwise Ghidra only broke the string at a
    # character it did not read as text (the 0xF6 of "erlöse" in g_DeveloperChatPhraseUtf16)
    unit = 2 if 'unicode' in strings[a][0].lower() or 'utf16' in strings[a][0].lower() else 1
    if any(byte_at(a - 1 - i) for i in range(unit)):
        continue
    split.append(a)
if split:
    new_objects = []
    for start, size, name in objects:
        cuts = [a for a in split if start < a < start + size]
        bounds = [start] + cuts + [start + size]
        for i in range(len(bounds) - 1):
            piece_name = name if i == 0 else label_names.get(bounds[i], 'str_%08X' % bounds[i])
            new_objects.append((bounds[i], bounds[i + 1] - bounds[i], piece_name))
    objects = new_objects
    object_starts = [o[0] for o in objects]

# unnamed DAT_ objects a named table points to take the table's name and the first index that
# points to them (g_Table_5); a DAT_ label nothing points to, directly after such an object, is a
# part of it (Ghidra labelled an inner address the code indexes from)
all_dwords = set()
for a, b in blocks:
    for x in range(a, b - 3, 4):
        all_dwords.add(dword_at(x))
index_of = {o[0]: i for i, o in enumerate(objects)}
pointed = {}
for start, size, name in objects:
    if name.startswith('DAT_') or name not in macros:
        continue
    for i in range(size // 4):
        target = index_of.get(dword_at(start + 4 * i))
        if target is not None and objects[target][2].startswith('DAT_'):
            pointed.setdefault(target, '%s_%d' % (name, i))
renamed = []
for k, (start, size, name) in enumerate(objects):
    if k in pointed:
        renamed.append((start, size, pointed[k]))
    elif (name.startswith('DAT_') and start not in all_dwords and renamed and
          renamed[-1][0] + renamed[-1][1] == start and renamed[-1][2] in pointed.values()):
        previous = renamed.pop()
        renamed.append((previous[0], previous[1] + size, previous[2]))
    else:
        renamed.append((start, size, name))
objects = renamed
object_starts = [o[0] for o in objects]

def is_jump_table(name, start, end):
    """A label-only object holding only addresses inside original functions (switch targets) and
    0x90 padding: a jump table (or the tail of one) of the original code."""
    if name in macros or end - start < 8:
        return False
    targets = 0
    for x in range(start, end - 3, 4):
        v = dword_at(x)
        if v == 0x90909090:
            continue
        if not (START <= v < END and code[v - START] and v not in funcs):
            return False
        targets += 1
    return targets > 0

# ---- member layout per block
def identifier(name, used):
    # leading underscore: the object names are also macros (globals.h), which would expand here
    base = 'at_' + re.sub(r'\W', '_', name)
    candidate = base
    n = 2
    while candidate in used:
        candidate = '%s_%d' % (base, n)
        n += 1
    used.add(candidate)
    return candidate

members = []        # per block: list of (start, end, member name, object name or None)
member_at = {}      # address -> (block index, member name)
for k, (a, b) in enumerate(blocks):
    used = set()
    fields = []
    cursor = a
    first = bisect.bisect_left(object_starts, a)
    for start, size, name in objects[first:]:
        if start >= b:
            break
        end = min(start + size, b)
        if start > cursor:
            fields.append((cursor, start, identifier('gap_%08X' % cursor, used), None))
        fields.append((start, end, identifier(name, used), name))
        member_at[start] = (k, fields[-1][2])
        cursor = end
    if cursor < b:
        fields.append((cursor, b, identifier('gap_%08X' % cursor, used), None))
    members.append(fields)

# objects known only by a Ghidra label (vtables, handler tables) get an alias in image_data.h so the
# data can name them
label_aliases = {}
for k, fields in enumerate(members):
    for start, end, member, name in fields:
        if name and re.match(r'^[gk]_\w+$', name) and name not in macros and name not in label_aliases:
            label_aliases[name] = (start, '%s.%s' % (block_name(k), member))

def address_expression(value):
    """C constant expression for the generated address of original data address value: through the
    object's own name where globals.h / recovered.h define one (&g_Name, (byte *)&g_Name + 0x10),
    else through the block member."""
    owner = containing_object(value)
    if owner is not None and ((owner[2] in macros and macros[owner[2]][1] == owner[0]) or
                              label_aliases.get(owner[2], (None,))[0] == owner[0]):
        offset = value - owner[0]
        return '&%s' % owner[2] if offset == 0 else '(uint8_t *)&%s + 0x%X' % (owner[2], offset)
    if value in member_at:
        k, member = member_at[value]
        return '&%s.%s' % (block_name(k), member)
    k = block_of(value)
    return '(uint8_t *)&%s + 0x%X' % (block_name(k), value - blocks[k][0])

# ---- initializers
def c_string_literal(value, wide):
    out = []
    for ch in value:
        o = ord(ch)
        if ch == '\\':
            out.append('\\\\')
        elif ch == '"':
            out.append('\\"')
        elif 0x20 <= o < 0x7f:
            out.append(ch)
        elif o < 0x100:
            out.append('\\%03o' % o)
        elif wide:
            out.append('\\u%04x' % o)
        else:
            return None
    return ('L"%s"' if wide else '"%s"') % ''.join(out)

def string_member(start, end):
    entry = strings.get(start)
    if entry is None:
        return None
    kind, value = entry
    wide = kind == 'unicode'
    unit = 2 if wide else 1
    try:
        encoded = (value + '\0').encode('utf-16le' if wide else 'latin-1')
    except UnicodeEncodeError:
        return None
    if len(encoded) > end - start:
        return None
    if bytes(byte_at(x) for x in range(start, start + len(encoded))) != encoded:
        return None
    literal = c_string_literal(value, wide)
    if literal is None:
        return None
    rest = [byte_at(x) for x in range(start + len(encoded), end)]
    if not any(rest) and (end - start) % unit == 0:
        return ('uint16_t' if wide else 'char', (end - start) // unit, literal, end - start)
    if all(b in (0, 0x90) for b in rest):
        # the string, then alignment padding (0x90) up to the next object
        return ('uint16_t' if wide else 'char', len(encoded) // unit, literal, len(encoded))
    return None

pointers = []

def inner_pointer(value):
    """A dword-aligned address inside a known data object (an element of a table, such as the
    fifth player-name buffer); unaligned values of that range are text or numbers."""
    return value & 3 == 0 and block_of(value) is not None and not code[value - START] and \
        containing_object(value) is not None

def value_expression(field_addr, owner_numeric):
    value = dword_at(field_addr)
    if not owner_numeric and value in funcs:
        pointers.append((field_addr, value, 'function', funcs[value]))
        return '(uint32_t)%s' % funcs[value]
    # a UI vtable address is never a number: UI node images typed as plain words (the fatal error
    # dialog, g_UiDisplaySettingsRootTemplate) need it as a pointer
    if (not owner_numeric and (value in anchors or inner_pointer(value))) or value in ui_vtables:
        pointers.append((field_addr, value, 'data', ''))
        target = address_expression(value)
        return '(uint32_t)(%s)' % target if '+' in target else '(uint32_t)%s' % target
    return '0x%08X' % value

def dword_list(start, count, numeric):
    values = [value_expression(start + 4 * i, numeric) for i in range(count)]
    while values and values[-1] == '0x00000000':
        values.pop()
    if not values:
        return '{0}'
    lines = []
    line = []
    for v in values:
        line.append(v)
        if len(line) == 8 or v.startswith('(uint32_t)'):
            lines.append(', '.join(line))
            line = []
    if line:
        lines.append(', '.join(line))
    return '{\n        ' + ',\n        '.join(lines) + '}'

def byte_list(start, end):
    values = [byte_at(x) for x in range(start, end)]
    while values and values[-1] == 0:
        values.pop()
    if not values:
        return '{0}'
    return '{' + ', '.join('0x%02X' % v for v in values) + '}'

# ---- typed members: objects whose declared type is known and representable as a C initializer
layout_path = os.path.join(args.work, 'typelayout.json')
type_layouts = json.load(open(layout_path)) if os.path.exists(layout_path) else {}
typedef_text = {}
_types_text = open(os.path.join(common.REPO, 'include', 'thandor', 'generated', 'types.h'), encoding='utf-8').read()
for m in re.finditer(r'^typedef\s+(.+?)\s*(\**)\s*\b(\w+)\s*;', _types_text, re.M):
    typedef_text[m.group(3)] = (m.group(1) + ' ' + m.group(2)).strip()
for m in re.finditer(r'^typedef enum (\w*)\s*\{.*?^\}\s*(\w+);', _types_text, re.M | re.S):
    typedef_text[m.group(2)] = 'enum ' + (m.group(1) or m.group(2))
INT_SIZES = {'byte': 1, 'char': 1, 'uchar': 1, 'undefined': 1, 'undefined1': 1, 'bool': 1,
             'unsigned char': 1, 'word': 2, 'short': 2, 'ushort': 2, 'undefined2': 2, 'unsigned short': 2,
             'dword': 4, 'int': 4, 'uint': 4, 'sdword': 4, 'undefined4': 4, 'long': 4, 'ulong': 4,
             'unsigned int': 4, 'unsigned long': 4, 'qword': 8, 'longlong': 8, 'ulonglong': 8,
             'undefined8': 8, 'unsigned long long': 8, 'long long': 8,
             'uint8_t': 1, 'int8_t': 1, 'uint16_t': 2, 'int16_t': 2, 'uint32_t': 4, 'int32_t': 4,
             'uint64_t': 8, 'int64_t': 8, 'uintptr_t': 4}

def resolve_type(t):
    t = t.strip()
    seen = set()
    while t in typedef_text and t not in seen:
        seen.add(t)
        t = typedef_text[t]
    return t

class Unrepresentable(Exception):
    pass

numeric_ranges = []   # (address, size) of integer and text fields written by scalar_init

def scalar_init(t, addr, typed_pointers, name=None):
    """Initializer for one scalar of type t (field name) at addr; pointers go through typed_pointers."""
    r = resolve_type(t)
    if '*' in r or '(' in r:
        value = dword_at(addr)
        if value == 0:
            return '0'
        if value in funcs:
            typed_pointers.append((addr, value, 'function', funcs[value]))
            return '(void *)%s' % funcs[value]
        if value in anchors:
            typed_pointers.append((addr, value, 'data', ''))
            target = address_expression(value)
            return '(void *)(%s)' % target if '+' in target else '(void *)%s' % target
        return '(void *)0x%08X' % value
    if r.startswith('enum '):
        size = 4
    elif r in INT_SIZES:
        size = INT_SIZES[r]
    elif r in ('float', 'double'):
        size = 4 if r == 'float' else 8
        raw = bytes(byte_at(addr + i) for i in range(size))
        value = struct.unpack('<f' if size == 4 else '<d', raw)[0]
        literal = repr(value)
        if value != value or literal in ('inf', '-inf') or \
                struct.pack('<f' if size == 4 else '<d', float(literal)) != raw:
            raise Unrepresentable(t)
        return literal + ('f' if size == 4 else '')
    else:
        raise Unrepresentable(t)
    numeric_ranges.append((addr, size))
    value = int.from_bytes(bytes(byte_at(addr + i) for i in range(size)), 'little')
    if value == 0:
        return '0'
    if value in funcs or value in anchors:
        raise Unrepresentable('address-like value in an integer field')
    return number_literal(value, size, t, r, name)

# ---- number style: the form that shows what a value means
SIGNED = {'char', 'short', 'int', 'sdword', 'long', 'longlong', 'long long', 'int8_t', 'int16_t', 'int32_t',
          'int64_t'}
# field names whose unsigned values are counts, sizes, coordinates or percentages (decimal) or bit patterns (hex)
DECIMAL_NAME = re.compile(r'(?i)(^num[A-Z0-9_]|count|size|width|height|length|len$|index|number|percent|ticks|delay|time|speed|'
                          r'rate|radius|distance|range|level|score|weight|cost|limit|capacity|amount|step|columns?|'
                          r'rows?|^[xyz]$|[a-z][XYZ]$|^[xyz][A-Z]|left|right|top|bottom)')
HEX_NAME = re.compile(r'(?i)(flag|mask|colou?r|argb|rgb|bits|magic|key|code|hash|seed|pattern|state|lane)')
ENUM_MEMBERS = {}     # enum tag -> {value: member name}
for m in re.finditer(r'^typedef enum (\w*)\s*\{(.*?)^\}\s*(\w+);', _types_text, re.M | re.S):
    tag = m.group(1) or m.group(3)
    enum_members, next_value = {}, 0
    for item in re.sub(r'/\*.*?\*/|//[^\n]*', '', m.group(2), flags=re.S).split(','):
        item = item.strip()
        if not item:
            continue
        name_value = item.split('=')
        try:
            next_value = int(name_value[1].strip(), 0) if len(name_value) > 1 else next_value
        except ValueError:
            break
        enum_members.setdefault(next_value & 0xFFFFFFFF, name_value[0].strip())
        next_value += 1
    ENUM_MEMBERS[tag] = enum_members
Q_TYPE = re.compile(r'^Q(\d+)$')

def number_literal(value, size, declared, resolved, name):
    """value (the unsigned bytes) as a C literal of the field's type: enum member names, signed and counted
    values in decimal, flags, masks and colours in hex, fixed-point values with their real value."""
    bits = 8 * size
    if resolved.startswith('enum '):
        member = ENUM_MEMBERS.get(resolved.split()[1], {}).get(value)
        if member is not None:
            return member
    q = Q_TYPE.match(declared.strip())
    signed = resolved in SIGNED or resolved.startswith('enum ') or bool(q)
    if signed and value >> (bits - 1):
        value -= 1 << bits
    field = name or ''
    if resolved == 'char' and 0x20 <= value < 0x7F:
        return "'\\''" if value == 0x27 else "'\\\\'" if value == 0x5C else "'%c'" % value
    if q:
        return '%d /* %s */' % (value, ('%.6f' % (value / float(1 << int(q.group(1))))).rstrip('0').rstrip('.'))
    if size == 8:
        return ('%dll' % value) if signed else ('0x%Xull' % value)
    if signed:
        if HEX_NAME.search(field) and not DECIMAL_NAME.search(field):
            return '-0x%X' % -value if value < 0 else '0x%X' % value
        return '%d' % value
    if HEX_NAME.search(field) and not DECIMAL_NAME.search(field):
        return '0x%X' % value
    if DECIMAL_NAME.search(field) or size < 4 or value < 0x10000:
        return '%d%s' % (value, 'u' if value > 0x7FFFFFFF else '')
    return '0x%X' % value

def type_size(t):
    r = resolve_type(t)
    if '*' in r or '(' in r or r.startswith('enum '):
        return 4
    if r in INT_SIZES:
        return INT_SIZES[r]
    if r in ('float', 'double'):
        return 4 if r == 'float' else 8
    if (r.startswith('struct ') or r.startswith('union ')) and r.split()[1] in type_layouts \
            and type_layouts[r.split()[1]]['size']:
        return type_layouts[r.split()[1]]['size']
    if r in type_layouts:
        return type_layouts[r]['size']
    raise Unrepresentable(t)

def array_string(element, addr, count, width):
    """A char/word array holding NUL-terminated printable text (and zeros after it) as a literal."""
    r = resolve_type(element)
    if width not in (1, 2) or r not in INT_SIZES or '*' in r:
        return None
    units = [int.from_bytes(bytes(byte_at(addr + i * width + b) for b in range(width)), 'little')
             for i in range(count)]
    if 0 not in units:
        return None
    end = units.index(0)
    if end < 2 or any(units[end:]):
        return None
    chars = ''.join(chr(u) for u in units[:end])
    if not all(ch.isprintable() or ch in '\t\r\n' for ch in chars):
        return None
    return c_string_literal(chars, width == 2)

def value_init(t, addr, typed_pointers, depth=0, name=None):
    r = resolve_type(t)
    if '*' in r or '(' in r:
        return scalar_init(t, addr, typed_pointers, name)
    struct_name = r.split()[1] if r.startswith('struct ') else (r if r in type_layouts else None)
    if struct_name is not None:
        layout = type_layouts.get(struct_name)
        if layout is None or layout['size'] is None:
            raise Unrepresentable(t)
        covered = bytearray(layout['size'])
        parts = []
        for field in layout['fields']:
            kind = field['kind']
            if 'offset' not in field or kind in ('array-of-union', 'bitfield', 'unparsed', 'opaque'):
                if any(byte_at(addr + x) for x in range(field.get('offset', 0),
                                                         field.get('offset', 0) + field.get('size', 0))) \
                        or 'offset' not in field:
                    raise Unrepresentable(struct_name + '.' + str(field['name']))
                for x in range(field['offset'], field['offset'] + field['size']):
                    covered[x] = 1
                continue
            for x in range(field['offset'], field['offset'] + field['size']):
                covered[x] = 1
            if kind == 'pointer' and '(' in field['type']:
                element = 'void *'
            else:
                element = re.sub(r'\s*\[.*$', '', field['type'])
            if kind.startswith('array-of-'):
                count = field['count']
                width = field['size'] // count
                text_literal = array_string(element, addr + field['offset'], count, width)
                if text_literal is not None:
                    parts.append('.%s = %s' % (field['name'], text_literal))
                    continue
                items = [value_init(element, addr + field['offset'] + i * width, typed_pointers, depth + 1,
                                    field['name']) for i in range(count)]
                while items and items[-1] in ('0', '{0}'):
                    items.pop()
                if items:
                    text = ', '.join(items)
                    if len(text) > 100:
                        # long arrays: pointer and struct tables one entry per line, numbers in rows, with the
                        # index of the first entry of each line
                        text = '\n' + table_rows(items, '            ') + '\n        '
                    parts.append('.%s = {%s}' % (field['name'], text))
            else:
                item = value_init(element, addr + field['offset'], typed_pointers, depth + 1, field['name'])
                if item not in ('0', '{0}'):
                    parts.append('.%s = %s' % (field['name'], item))
        if any(byte_at(addr + x) for x in range(layout['size']) if not covered[x]):
            raise Unrepresentable(struct_name + ' padding')
        if not parts:
            return '{0}'
        text = ', '.join(parts)
        if depth == 0 and len(text) > 100:
            # one field per line for long top-level structs (vtables, callback tables)
            return '{\n        %s}' % ',\n        '.join(parts)
        return '{%s}' % text
    if r.startswith('union '):
        size = type_size(t)
        if not any(byte_at(addr + x) for x in range(size)):
            return '{0}'
        # a union is initialized through its first member, if that covers all of it
        layout = type_layouts.get(r.split()[1])
        first = layout['fields'][0] if layout and layout['fields'] else None
        if first and first.get('offset') == 0 and first.get('size') == size and \
                first['kind'] not in ('union', 'array-of-union', 'bitfield', 'unparsed', 'opaque'):
            type_layouts['__union_view'] = {'size': size, 'fields': [first]}
            return value_init('__union_view', addr, typed_pointers, depth)
        raise Unrepresentable(t)
    return scalar_init(t, addr, typed_pointers, name)

def table_rows(items, indent):
    """Array items one per line (pointers, structs, commented values) or in rows of numbers, each line
    starting with the index of its first item."""
    single = any('(void *)' in i or '{' in i or '/*' in i or '&' in i for i in items)
    per_line = 1 if single else (16 if max(len(i) for i in items) <= 4 else 8)
    width = len(str(len(items) - 1))
    lines = []
    for k in range(0, len(items), per_line):
        lines.append('%s/* %*d */ %s' % (indent, width, k, ', '.join(items[k:k + per_line])))
    return ',\n'.join(lines)

def nested_init(items, dims, depth):
    """Braced initializer of a multi-dimensional array, innermost rows on one line."""
    if len(dims) == 1:
        return '{%s}' % ', '.join(items)
    step = len(items) // dims[0]
    rows = [nested_init(items[i * step:(i + 1) * step], dims[1:], depth + 1) for i in range(dims[0])]
    indent = '\n' + '    ' * (depth + 1)
    return '{' + indent + (',' + indent).join(rows) + '}'

# label-only UI vtables (no declaration in the headers): typed as UiNodeVtable when every method slot
# holds a function or 0; methods a subclass adds follow as a dword rest
label_types = {}
if 'UiNodeVtable' in type_layouts:
    slots = type_layouts['UiNodeVtable']['size'] // 4
    for start, size, name in objects:
        if name in macros or 'vtable' not in name.lower() or size < slots * 4:
            continue
        values = [dword_at(start + 4 * i) for i in range(slots)]
        if all(v == 0 or v in funcs for v in values) and any(values):
            label_types[name] = ('UiNodeVtable *', start)

def typed_member(start, end, name, member):
    """(declarations, initializers, pointers) for an object with a known type, or None."""
    macro = macros.get(name) or label_types.get(name)
    if macro is None or macro[1] != start:
        return None
    t = macro[0].strip()
    array = re.match(r'(.+?)\s*\(\*\)((?:\[\w+\])+)$', t)
    dims = []
    if array:
        element = array.group(1).strip()
        dims = [int(d, 0) for d in re.findall(r'\[(\w+)\]', array.group(2))]
        count = 1
        for d in dims:
            count *= d
    elif t.endswith('*'):
        element, count = t[:-1].strip(), 1
    else:
        return None
    if resolve_type(element) in ('undefined', 'undefined1', 'undefined2', 'undefined4', 'undefined8'):
        return None
    try:
        width = type_size(element)
        if width * count > end - start:
            return None
        typed_pointers = []
        del numeric_ranges[:]
        items = [value_init(element, start + i * width, typed_pointers, 0, name) for i in range(count)]
    except (Unrepresentable, RecursionError):
        return None
    # every pointer the dword grid would convert must also be a pointer field here; otherwise
    # (e.g. UI nodes inside a template's opaque byte gaps) keep the grid
    owner = containing_object(start)
    if not (owner is not None and NUMERIC.match(types.get(owner[2], 'struct').strip())):
        typed_locations = set(p[0] for p in typed_pointers)
        numeric_bytes = set(a + i for a, size in numeric_ranges for i in range(size))
        for offset in range(0, width * count - 3, 4):
            value = dword_at(start + offset)
            if all(start + offset + i in numeric_bytes for i in range(4)):
                continue    # text or numbers that happen to look like an address
            if (value in funcs or value in anchors) and start + offset not in typed_locations:
                return None
    decl = '    %s %s%s;' % (element, member, ''.join('[%d]' % d for d in dims))
    if len(dims) > 1:
        init = nested_init(items, dims, 1)
    elif array:
        while items and items[-1] in ('0', '{0}'):
            items.pop()
        init = '{%s}' % ', '.join(items) if items else '{0}'
        if len(init) > 100:
            # long tables: pointer and struct entries one per line, numbers in rows, each line with its index
            init = '{\n%s}' % table_rows(items, '        ')
        text_literal = array_string(element, start, count, width)
        if text_literal is not None and not typed_pointers:
            init = text_literal
    else:
        init = items[0]
    return decl, init, typed_pointers, start + width * count

# objects the program computes at startup instead (see WinMain): storage only, no initializer
COMPUTED = {'g_FixedSinBeforeZeroQ28': 'FixedMath_BuildSinCosTables',
            'g_FixedSinQ28': 'FixedMath_BuildSinCosTables', 'g_FixedCosQ28': 'FixedMath_BuildSinCosTables',
            'g_MovieChromaLumaToArgb': 'Movie_BuildChromaLumaTable',
            'g_PackedLightingLookupTable': 'GraphicsLighting_BuildPackedLookupTable'}

# UI templates: images of UI node sets the code copies whole (e.g. FrontendRuntime_Initialize) and
# then links up. Every node starts with a UiNodeBase whose vtable points to a UI vtable; the node
# links are offsets from the template start (-1: none). Written as one member per node.
UI_TEMPLATES = {'g_InGameRuntimeDefaultImageTemplate': ('InGameUiImage', 'INGAME_UI'),
                'g_FrontendRootInitializationTemplate': ('FrontendUiImage', 'FRONTEND_UI'),
                'g_UiDisplaySettingsRootTemplate': ('DisplaySettingsUiImage', 'DISPLAY_SETTINGS_UI'),
                'g_UiFourValueDialogTemplateImage': ('FourValueDialogUiImage', 'FOUR_VALUE_DIALOG_UI'),
                'g_FatalErrorUiRootTemplateImage': ('FatalErrorUiImage', 'FATAL_ERROR_UI')}
# node names: tools/data/ui_node_names.json, {image type: {"0xOFFSET": {"name": ..., "note": ...,
# optional "prefix": a UI_NODE_PREFIX_TYPES type}}}
# short descriptions of data objects for their comment in image_data.c: tools/data/object_notes.json,
# {object name: description}
_notes_path = os.path.join(common.REPO, 'tools', 'data', 'object_notes.json')
OBJECT_NOTES = json.load(open(_notes_path, encoding='utf-8')) if os.path.exists(_notes_path) else {}
_names_path = os.path.join(common.REPO, 'tools', 'data', 'ui_node_names.json')
UI_NODE_NAMES = json.load(open(_names_path, encoding='utf-8')) if os.path.exists(_names_path) else {}
ui_template_types = []   # typedefs for include/thandor/generated/ui_templates.h

def node_member(type_name, offset):
    entry = UI_NODE_NAMES.get(type_name, {}).get('0x%04X' % offset)
    if not entry:
        return 'node%04X' % offset, ''
    name = entry['name']
    return name[0].lower() + name[1:], entry.get('note', '')

# Values some nodes keep in the dwords in front of them (read back through the node pointer, e.g. an
# option button's mode value): a node with "prefix": type in ui_node_names.json gets a member
# <node>_prefix of that type directly before it, instead of the tail of the previous node's _fields.
# type -> (comment, [(C type, field, comment)]); all fields are dwords.
UI_NODE_PREFIX_TYPES = {
    'UiDisplayModeOptionPrefix': (
        'The dwords in front of a display settings option button (DisplaySettingsUiImage <button>_prefix):\n'
        '   its mode value(s), then (as in front of every template node) the tooltip text id. Read back by the\n'
        '   option actions (UiDisplayModeAction_Update*Selection).',
        [('int32_t', 'resolutionHeight', '-0xC: resolution buttons only'),
         ('int32_t', 'modeValue', '-0x8: bits per pixel, resolution width or adapter index'),
         ('uint32_t', 'tooltipTextResourceId', '-0x4')]),
    'UiTechnologyAreaTabPrefix': (
        'The dwords in front of a technology area tab (InGameUiImage technologyAreaTabN_prefix): the name text\n'
        '   id of the tab\'s technology and the tab\'s tooltip text (the expanded label), both set at runtime.',
        [('int32_t', 'nameTextResourceId', '-8: TECHNOLOGY_TEXT_ID_BASE + 2 * technology id'),
         ('uint16_t *', 'tooltipText', '-4')]),
}

def node_prefix(type_name, offset):
    entry = UI_NODE_NAMES.get(type_name, {}).get('0x%04X' % offset)
    prefix_type = entry.get('prefix') if entry else None
    if prefix_type is None:
        return None, 0
    return prefix_type, 4 * len(UI_NODE_PREFIX_TYPES[prefix_type][1])

def prefix_init(prefix_type, addr, typed_pointers):
    parts = []
    for i, (ctype, field, _) in enumerate(UI_NODE_PREFIX_TYPES[prefix_type][1]):
        value = dword_at(addr + 4 * i)
        if '*' in ctype:
            text = scalar_init(ctype, addr + 4 * i, typed_pointers)
        elif ctype == 'int32_t':
            text = signed_literal(value)
        else:
            text = '0x%X' % value if value else '0'
        if text != '0':
            parts.append('.%s = %s' % (field, text))
    return '{%s}' % ', '.join(parts) if parts else '{0}'
ui_vtables = set(a for a, s, n in objects if 'vtable' in n.lower() and not is_jump_table(n, a, a + s))
NODE_BASE = type_layouts.get('UiNodeBase')
NODE_LINKS = ('nextSibling', 'firstChild', 'parent')
extra_types = {}    # block index -> typedefs the block struct needs

def signed_literal(value):
    value = value - (1 << 32) if value & 0x80000000 else value
    if -0x10000 < value < 0x10000:
        return '%d' % value
    return '-0x%X' % -value if value < 0 else '0x%X' % value

def node_starts(start, size):
    nodes = []
    for offset in range(0, size - NODE_BASE['size'] + 1, 4):
        if dword_at(start + offset + 12) not in ui_vtables:
            continue
        links = [dword_at(start + offset + 4 * i) for i in range(3)]
        if any(v != 0xFFFFFFFF and v >= size for v in links):
            continue
        if nodes and offset < nodes[-1] + NODE_BASE['size']:
            continue
        nodes.append(offset)
    return nodes

NODE_LINE_BREAKS = ('vtable', 'left', 'leftOffset', 'leftAnchorQ31', 'layoutWidth')

def node_base_init(addr, typed_pointers, label):
    lines = []
    parts = []
    for field in NODE_BASE['fields']:
        if field['name'] in NODE_LINE_BREAKS and parts:
            lines.append(', '.join(parts))
            parts = []
        value = dword_at(addr + field['offset'])
        if field['name'] in NODE_LINKS:
            text = 'UI_TEMPLATE_NO_LINK' if value == 0xFFFFFFFF else 'UI_TEMPLATE_LINK(0x%X)' % value
        elif field['kind'] == 'pointer':
            text = scalar_init(field['type'], addr + field['offset'], typed_pointers)
        elif field['type'] in ('sdword', 'int32_t'):
            text = signed_literal(value)
        else:
            text = '0x%X' % value if value else '0'
        if text != '0':
            parts.append('.%s = %s' % (field['name'], text))
    if parts:
        lines.append(', '.join(parts))
    return '{ /* %s */\n            %s}' % (label, ',\n            '.join(lines))

def template_member(k, start, end, name, member):
    if name not in UI_TEMPLATES or NODE_BASE is None:
        return None
    size = end - start
    nodes = node_starts(start, size)
    if not nodes or size % 4:
        return None
    type_name = UI_TEMPLATES[name][0]
    fields, inits = [], []
    typed_pointers = []
    pieces = ([(0, nodes[0], False)] if nodes[0] else []) + \
        [(o, nodes[i + 1] if i + 1 < len(nodes) else size, True) for i, o in enumerate(nodes)]
    for piece_start, piece_end, is_node in pieces:
        addr = start + piece_start
        rest_start = piece_start
        if is_node:
            target = containing_object(dword_at(addr + 12))
            vtable_name = target[2] if target else ''
            node_name, note = node_member(type_name, piece_start)
            prefix_type, prefix_size = node_prefix(type_name, piece_start)
            if prefix_type:
                fields.append('    %s %s_prefix; /* +%04X */' % (prefix_type, node_name, piece_start - prefix_size))
                inits.append('        %s,' % prefix_init(prefix_type, addr - prefix_size, typed_pointers))
            fields.append('    UiNodeBase %s; /* +%04X %s%s */' % (node_name, piece_start, vtable_name,
                                                               ': ' + note if note else ''))
            inits.append('        %s,' % node_base_init(addr, typed_pointers, '+%04X %s %s' % (
                piece_start, node_name, vtable_name)))
            rest_start = piece_start + NODE_BASE['size']
        # the prefix dwords of the next node are its own member
        next_prefix_size = node_prefix(type_name, piece_end)[1] if piece_end < size else 0
        count = (piece_end - next_prefix_size - rest_start) // 4
        assert count >= 0, (name, piece_end)
        if count:
            label = node_member(type_name, piece_start)[0] + '_fields' if is_node else 'header'
            fields.append('    uint32_t %s[%d];' % (label, count))
            before = len(pointers)
            inits.append('        %s,' % dword_list(start + rest_start, count, False).replace('\n        ',
                                                                                             '\n            '))
            typed_pointers.extend(pointers[before:])
            del pointers[before:]
    prefix = UI_TEMPLATES[name][1]
    ui_template_types.append(
        '/* %s: %d UI nodes. %s(root, node) is the node in a copy of it (or a node\'s <node>_prefix),\n'
        '   %s_FIELD(root, node, offset, type) a class field behind the UiNodeBase of the node. */\n'
        'typedef struct %s {\n%s\n} %s;\n'
        '#define %s(root, node) (&((%s *)(uintptr_t)(root))->node)\n'
        '#define %s_FIELD(root, node, offset, type) (*(type *)((uint8_t *)%s(root, node) + (offset)))\n' % (
            name, len(nodes), prefix, prefix, type_name, '\n'.join(fields), type_name,
            prefix, type_name, prefix, prefix))
    return '    %s %s;' % (type_name, member), '{\n%s\n    }' % '\n'.join(inits), typed_pointers

layout_lines = []   # struct member declarations per block
init_lines = []     # initializer per block
typed_count = 0
inventory = []      # (start, end, name, kind) of every member, for tools/data/image_data_report.py

def note(start, end, name, kind):
    # untyped bytes that are only code alignment filler (0x90 NOP, 0xCC INT3) are padding, not data
    if kind in ('rest', 'gap', 'raw') and all(byte_at(x) in (0, 0x90, 0xCC) for x in range(start, end)) and \
            any(byte_at(x) for x in range(start, end)):
        kind = 'padding'
    inventory.append((start, end, name or '', kind))
for k, (a, b) in enumerate(blocks):
    decls = []
    inits = []
    for start, end, member, name in members[k]:
        comment = '/* %08X %s */' % (start, name if name else 'gap')
        if name in OBJECT_NOTES:
            comment = '/* %08X %s: %s */' % (start, name, OBJECT_NOTES[name])
        if name and ('_SwitchTable_' in name or name.startswith('switchdata') or is_jump_table(name, start, end)):
            comment = '/* %08X %s: jump table of the original code, not used by the C code */' % (start, name)
        owner = containing_object(start)
        numeric = owner is not None and bool(NUMERIC.match(types.get(owner[2], 'struct').strip()))
        if name and ('_SwitchTable_' in name or name.startswith('switchdata') or is_jump_table(name, start, end)):
            note(start, end, name, 'jump-table')
        s = string_member(start, end) if name else None
        if s is not None:
            ctype, count, literal, used = s
            decls.append('    %s %s[%d]; %s' % (ctype, member, count, comment))
            inits.append('    %s, %s' % (literal, comment))
            note(start, start + used, name, 'string')
            if start + used < end:
                decls.append('    uint8_t %s_padding[%d];' % (member, end - start - used))
                inits.append('    %s,' % byte_list(start + used, end))
                note(start + used, end, name, 'padding')
            continue
        if name in COMPUTED:
            count, tail = divmod(end - start, 4)
            assert not tail
            decls.append('    uint32_t %s[%d]; %s' % (member, count, comment))
            inits.append('    {0}, /* %08X %s: filled at startup by %s */' % (start, name, COMPUTED[name]))
            note(start, end, name, 'computed')
            continue
        # a template ends at its declared size; the object's extent runs on to the next code or
        # object, which after the last node is only the 0x90 alignment padding of the next function
        template_end = end
        if name in UI_TEMPLATES and 0 < declared_sizes.get(start, 0) < end - start:
            template_end = start + declared_sizes[start]
        template = template_member(k, start, template_end, name, member) if name else None
        if template is not None:
            decl, init, typed_pointers = template
            pointers.extend(typed_pointers)
            decls.append('%s %s' % (decl, comment))
            inits.append('    %s, %s' % (init, comment))
            note(start, template_end, name, 'ui-template')
            if template_end < end:
                decls.append('    uint8_t %s_padding[%d]; /* alignment padding after the template */' % (
                    member, end - template_end))
                inits.append('    %s,' % byte_list(template_end, end))
                note(template_end, end, name, 'padding')
            continue
        typed = typed_member(start, end, name, member) if name else None
        if typed is not None:
            decl, init, typed_pointers, typed_end = typed
            pointers.extend(typed_pointers)
            typed_count += 1
            decls.append('%s %s' % (decl, comment))
            inits.append('    %s, %s' % (init, comment))
            element_type = macros.get(name, label_types.get(name, ('',)))[0].strip()
            note(start, typed_end, name,
                 'typed-generic' if re.match(r'^(?:dword|uint|undefined\d?)\s*[\*(]',
                                             element_type) else 'typed')
            if typed_end < end:
                rest_count, rest_tail = divmod(end - typed_end, 4)
                note(typed_end, end, name, 'rest')
                if rest_count:
                    decls.append('    uint32_t %s_rest[%d]; /* beyond the declared type */' % (member, rest_count))
                    inits.append('    %s,' % dword_list(typed_end, rest_count, numeric))
                if rest_tail:
                    decls.append('    uint8_t %s_rest_tail[%d];' % (member, rest_tail))
                    inits.append('    %s,' % byte_list(typed_end + 4 * rest_count, end))
            continue
        count, tail = divmod(end - start, 4)
        if name is None or count == 0:
            decls.append('    uint8_t %s[%d]; %s' % (member, end - start, comment))
            inits.append('    %s, %s' % (byte_list(start, end), comment))
            if not (inventory and inventory[-1][0] == start and inventory[-1][3] == 'jump-table'):
                note(start, end, name, 'gap' if name is None else 'raw')
            continue
        decls.append('    uint32_t %s[%d]; %s' % (member, count, comment))
        inits.append('    %s, %s' % (dword_list(start, count, numeric), comment))
        if not (inventory and inventory[-1][0] == start and inventory[-1][3] == 'jump-table'):
            note(start, end, name, 'raw')
        if tail:
            decls.append('    uint8_t %s_tail[%d];' % (member, tail))
            inits.append('    %s,' % byte_list(start + 4 * count, end))
    layout_lines.append(decls)
    init_lines.append(inits)

# ---- header
hdr = [HEADER % 'include/thandor/generated/image_data.h',
       '/* Generated by tools/data/gen_image_data.py from the original thandor.exe. Do not edit. */\n\n'
       '#ifndef THANDOR_GENERATED_IMAGE_DATA_H\n#define THANDOR_GENERATED_IMAGE_DATA_H\n\n'
       '#include <thandor/generated/types.h>\n#include <thandor/generated/ui_templates.h>\n\n'
       '#pragma pack(push, 1)\n']
for k, (a, b) in enumerate(blocks):
    if k in extra_types:
        hdr.append('\n' + '\n'.join(extra_types[k]))
    hdr.append('\n/* original 0x%08X-0x%08X */\ntypedef struct ImageData_%08X {\n%s\n} ImageData_%08X;\n'
               'extern ImageData_%08X %s;\n' % (a, b, a, '\n'.join(layout_lines[k]), a, a, block_name(k)))
hdr.append('\n#pragma pack(pop)\n\n/* Objects the headers do not declare, by their Ghidra label. */\n')
for name, (start, target) in sorted(label_aliases.items(), key=lambda x: x[1][0]):
    hdr.append('#define %s (%s)\n' % (name, target))
hdr.append('\n/* Original address -> generated storage. */\n')
missing = []
for addr in sorted(set(a for _, (_, a) in macros.items())):
    if addr in member_at:
        k, member = member_at[addr]
        hdr.append('#define THANDOR_IMAGE_0x%08x ((uintptr_t)&%s.%s)\n' % (addr, block_name(k), member))
    elif block_of(addr) is not None:
        k = block_of(addr)
        hdr.append('#define THANDOR_IMAGE_0x%08x ((uintptr_t)&%s + 0x%X)\n' % (addr, block_name(k), addr - blocks[k][0]))
    elif not START <= addr < END:
        # numbers Ghidra typed as addresses (low error codes, the image base in a comparison)
        hdr.append('#define THANDOR_IMAGE_0x%08x ((uintptr_t)0x%08xu)\n' % (addr, addr))
    else:
        missing.append(addr)
hdr.append('\n/* For OPEN_THANDOR_SELFTEST=imagecmp: each block with its original range, and every converted\n'
           '   pointer with its original location and value. */\n'
           'typedef struct ThandorImageBlock { uint32_t start; uint32_t end; const uint8_t *data; } ThandorImageBlock;\n'
           'typedef struct ThandorImagePointer { uint32_t location; uint32_t originalValue; } ThandorImagePointer;\n'
           'extern const ThandorImageBlock g_ThandorImageBlocks[%d];\n'
           'extern const ThandorImagePointer g_ThandorImagePointers[%d];\n\n#endif\n' % (len(blocks), len(pointers)))
open(os.path.join(common.REPO, 'include', 'thandor', 'generated', 'image_data.h'), 'w',
     encoding='utf-8').write(''.join(hdr))

# ---- UI template types
open(os.path.join(common.REPO, 'include', 'thandor', 'generated', 'ui_templates.h'), 'w', encoding='utf-8').write(
    HEADER % 'include/thandor/generated/ui_templates.h' +
    '/* Generated by tools/data/gen_image_data.py from the original thandor.exe and\n'
    '   tools/data/ui_node_names.json. Do not edit. */\n\n'
    '#ifndef THANDOR_GENERATED_UI_TEMPLATES_H\n#define THANDOR_GENERATED_UI_TEMPLATES_H\n\n'
    '#include <thandor/generated/types.h>\n\n'
    '/* UI template node links: offsets from the template start, made into pointers when the\n'
    '   template is copied and linked. */\n'
    '#define UI_TEMPLATE_LINK(offset) ((UiNodeBase *)(offset))\n'
    '#define UI_TEMPLATE_NO_LINK ((UiNodeBase *)-1)\n\n' +
    ''.join('/* %s */\ntypedef struct %s {\n%s\n} %s;\n' % (
        comment, prefix_type,
        '\n'.join('    %s%s%s; /* %s */' % (ctype, '' if ctype.endswith('*') else ' ', field, note)
                  for ctype, field, note in fields),
        prefix_type) for prefix_type, (comment, fields) in UI_NODE_PREFIX_TYPES.items()) +
    '/* The <node>_prefix of the given type in front of a node the code only has as a pointer (the\n'
    '   node of an action callback, a node chosen at runtime); with the node\'s name known,\n'
    '   <TEMPLATE>_UI(root, <node>_prefix) names it directly. */\n'
    '#define UI_TEMPLATE_NODE_PREFIX(type, node) (((type *)(uintptr_t)(node))[-1])\n\n'
    '#pragma pack(push, 1)\n\n' + '\n'.join(ui_template_types) + '\n#pragma pack(pop)\n\n#endif\n')

# ---- source
src = [HEADER % 'src/generated/image_data.c',
       '/* Generated by tools/data/gen_image_data.py from the original thandor.exe: the data of\n'
       '   0x401000-0x58C000 in original order, one packed struct per run between code. Do not edit;\n'
       '   regenerate, or move objects out into hand-written definitions. */\n\n'
       '#include <thandor/thandor.h>\n#include <thandor/generated/image_data.h>\n\n'
       '#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */\n']
for k, (a, b) in enumerate(blocks):
    # a multi-line initializer gets its object comment as a heading instead of at its end
    lines = [re.sub(r'^    (\{[^\n]*\n.*), (/\* [0-9A-F]{8} [^\n]* \*/)$', r'    \2\n    \1,', line, flags=re.S)
             for line in init_lines[k]]
    src.append('\nImageData_%08X %s = {\n%s\n};\n' % (a, block_name(k), '\n'.join(lines)))
src.append('\nconst ThandorImageBlock g_ThandorImageBlocks[%d] = {\n' % len(blocks))
for k, (a, b) in enumerate(blocks):
    src.append('    {0x%08X, 0x%08X, (const uint8_t *)&%s},\n' % (a, b, block_name(k)))
src.append('};\n\nconst ThandorImagePointer g_ThandorImagePointers[%d] = {\n' % len(pointers))
for location, value, kind, name in pointers:
    src.append('    {0x%08X, 0x%08X},\n' % (location, value))
src.append('};\n')
open(os.path.join(common.REPO, 'src', 'generated', 'image_data.c'), 'w', encoding='utf-8').write(''.join(src))

with open(os.path.join(args.work, 'image_pointers.tsv'), 'w', encoding='utf-8') as f:
    for p in pointers:
        f.write('%08x\t%08x\t%s\t%s\n' % p)
with open(os.path.join(args.work, 'image_objects.tsv'), 'w', encoding='utf-8') as f:
    # start, end, name, kind, nonzero bytes, pointers inside, declared type
    pointer_locations = sorted(p[0] for p in pointers)
    for start, end, name, kind in inventory:
        nonzero = sum(1 for x in range(start, end) if byte_at(x))
        inside = bisect.bisect_left(pointer_locations, end) - bisect.bisect_left(pointer_locations, start)
        declared = macros.get(name, label_types.get(name, ('',)))[0].strip() if name else ''
        f.write('%08x\t%08x\t%s\t%s\t%d\t%d\t%s\n' % (start, end, name, kind, nonzero, inside, declared))
string_count = sum(1 for lines in layout_lines for l in lines if ' = ' not in l and ('uint16_t ' in l or 'char ' in l)
                   and not l.strip().startswith('uint32_t'))
print('%d blocks, %d bytes, %d members (%d typed), %d function pointers, %d data pointers' % (
    len(blocks), sum(b - a for a, b in blocks), sum(len(m) for m in members), typed_count,
    sum(1 for p in pointers if p[2] == 'function'), sum(1 for p in pointers if p[2] == 'data')))
if missing:
    print('address macros outside every block:', ', '.join('%08x' % a for a in missing))
