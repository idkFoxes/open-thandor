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

NUMERIC = re.compile(r'^(?:const )?(?:byte|word|dword|qword|short|ushort|int|uint|sdword|char|undefined[1248]?|'
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
for line in open(os.path.join(args.work, 'layout.tsv'), encoding='utf-8'):
    a, size, declared, name, kind, nonzero = line.rstrip('\n').split('\t')
    objects.append((int(a, 16), int(size), name))
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
    if t and not re.match(r'^(?:word|char|byte|undefined[12]?)\s*(?:\*|\(\*\)\[)', t.strip()):
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
        return '&%s' % owner[2] if offset == 0 else '(byte *)&%s + 0x%X' % (owner[2], offset)
    if value in member_at:
        k, member = member_at[value]
        return '&%s.%s' % (block_name(k), member)
    k = block_of(value)
    return '(byte *)&%s + 0x%X' % (block_name(k), value - blocks[k][0])

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
    if len(encoded) > end - start or (end - start) % unit:
        return None
    if bytes(byte_at(x) for x in range(start, start + len(encoded))) != encoded:
        return None
    if any(byte_at(x) for x in range(start + len(encoded), end)):
        return None
    literal = c_string_literal(value, wide)
    if literal is None:
        return None
    return ('word' if wide else 'char', (end - start) // unit, literal)

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
        return '(dword)%s' % funcs[value]
    # a UI vtable address is never a number: UI node images typed as plain words (the fatal error
    # dialog, g_UiDisplaySettingsRootTemplate) need it as a pointer
    if (not owner_numeric and (value in anchors or inner_pointer(value))) or value in ui_vtables:
        pointers.append((field_addr, value, 'data', ''))
        target = address_expression(value)
        return '(dword)(%s)' % target if '+' in target else '(dword)%s' % target
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
        if len(line) == 8 or v.startswith('(dword)'):
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
for m in re.finditer(r'^typedef\s+(.+?)\s+(\w+)\s*;',
                     open(os.path.join(common.REPO, 'include', 'thandor', 'generated', 'types.h'),
                          encoding='utf-8').read(), re.M):
    typedef_text[m.group(2)] = m.group(1).strip()
INT_SIZES = {'byte': 1, 'char': 1, 'uchar': 1, 'undefined': 1, 'undefined1': 1, 'bool': 1,
             'unsigned char': 1, 'word': 2, 'short': 2, 'ushort': 2, 'undefined2': 2, 'unsigned short': 2,
             'dword': 4, 'int': 4, 'uint': 4, 'sdword': 4, 'undefined4': 4, 'long': 4, 'ulong': 4,
             'unsigned int': 4, 'unsigned long': 4, 'qword': 8, 'longlong': 8, 'ulonglong': 8,
             'undefined8': 8, 'unsigned long long': 8, 'long long': 8}

def resolve_type(t):
    t = t.strip()
    seen = set()
    while t in typedef_text and t not in seen:
        seen.add(t)
        t = typedef_text[t]
    return t

class Unrepresentable(Exception):
    pass

def scalar_init(t, addr, typed_pointers):
    """Initializer for one scalar of type t at addr; pointers go through typed_pointers."""
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
    value = int.from_bytes(bytes(byte_at(addr + i) for i in range(size)), 'little')
    if value == 0:
        return '0'
    if value in funcs or value in anchors:
        raise Unrepresentable('address-like value in an integer field')
    return ('0x%X' % value) + ('ull' if size == 8 else '')

def type_size(t):
    r = resolve_type(t)
    if '*' in r or '(' in r or r.startswith('enum '):
        return 4
    if r in INT_SIZES:
        return INT_SIZES[r]
    if r in ('float', 'double'):
        return 4 if r == 'float' else 8
    if r.startswith('struct ') and r.split()[1] in type_layouts:
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

def value_init(t, addr, typed_pointers, depth=0):
    r = resolve_type(t)
    if '*' in r or '(' in r:
        return scalar_init(t, addr, typed_pointers)
    struct_name = r.split()[1] if r.startswith('struct ') else (r if r in type_layouts else None)
    if struct_name is not None:
        layout = type_layouts.get(struct_name)
        if layout is None or layout['size'] is None:
            raise Unrepresentable(t)
        covered = bytearray(layout['size'])
        parts = []
        for field in layout['fields']:
            kind = field['kind']
            if 'offset' not in field or kind in ('union', 'array-of-union', 'bitfield', 'unparsed', 'opaque'):
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
                items = [value_init(element, addr + field['offset'] + i * width, typed_pointers, depth + 1)
                         for i in range(count)]
                while items and items[-1] in ('0', '{0}'):
                    items.pop()
                if items:
                    parts.append('.%s = {%s}' % (field['name'], ', '.join(items)))
            else:
                item = value_init(element, addr + field['offset'], typed_pointers, depth + 1)
                if item not in ('0', '{0}'):
                    parts.append('.%s = %s' % (field['name'], item))
        if any(byte_at(addr + x) for x in range(layout['size']) if not covered[x]):
            raise Unrepresentable(struct_name + ' padding')
        if not parts:
            return '{0}'
        text = ', '.join(parts)
        if depth == 0 and len(text) > 100 and '\n' not in text:
            # one field per line for long top-level structs (vtables, callback tables)
            return '{\n        %s}' % ',\n        '.join(parts)
        return '{%s}' % text
    if r.startswith('union '):
        if any(byte_at(addr + x) for x in range(type_size(t))):
            raise Unrepresentable(t)
        return '{0}'
    return scalar_init(t, addr, typed_pointers)

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
        items = [value_init(element, start + i * width, typed_pointers) for i in range(count)]
    except (Unrepresentable, RecursionError):
        return None
    # every pointer the dword grid would convert must also be a pointer field here; otherwise
    # (e.g. UI nodes inside a template's opaque byte gaps) keep the grid
    owner = containing_object(start)
    if not (owner is not None and NUMERIC.match(types.get(owner[2], 'struct').strip())):
        typed_locations = set(p[0] for p in typed_pointers)
        for offset in range(0, width * count - 3, 4):
            value = dword_at(start + offset)
            if (value in funcs or value in anchors) and start + offset not in typed_locations:
                return None
    decl = '    %s %s%s;' % (element, member, ''.join('[%d]' % d for d in dims))
    if len(dims) > 1:
        init = nested_init(items, dims, 1)
    elif array:
        while items and items[-1] in ('0', '{0}'):
            items.pop()
        init = '{%s}' % ', '.join(items) if items else '{0}'
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
UI_TEMPLATES = ('g_FrontendRootInitializationTemplate', 'g_InGameRuntimeDefaultImageTemplate',
                'g_UiFourValueDialogTemplateImage', 'g_FatalErrorUiRootTemplateImage',
                'g_UiDisplaySettingsRootTemplate')
ui_vtables = set(a for a, _, n in objects if 'vtable' in n.lower())
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
        elif field['type'] == 'sdword':
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
    type_name = 'UiTemplate_%08X' % start
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
            fields.append('    UiNodeBase node%04X; /* %s */' % (piece_start, vtable_name))
            inits.append('        %s,' % node_base_init(addr, typed_pointers, '+%04X %s' % (piece_start, vtable_name)))
            rest_start = piece_start + NODE_BASE['size']
        count = (piece_end - rest_start) // 4
        if count:
            label = 'node%04X_fields' % piece_start if is_node else 'header'
            fields.append('    dword %s[%d];' % (label, count))
            before = len(pointers)
            inits.append('        %s,' % dword_list(start + rest_start, count, False).replace('\n        ',
                                                                                             '\n            '))
            typed_pointers.extend(pointers[before:])
            del pointers[before:]
    extra_types.setdefault(k, []).append('/* %s: %d UI nodes */\ntypedef struct %s {\n%s\n} %s;\n' % (
        name, len(nodes), type_name, '\n'.join(fields), type_name))
    return '    %s %s;' % (type_name, member), '{\n%s\n    }' % '\n'.join(inits), typed_pointers

layout_lines = []   # struct member declarations per block
init_lines = []     # initializer per block
typed_count = 0
for k, (a, b) in enumerate(blocks):
    decls = []
    inits = []
    for start, end, member, name in members[k]:
        comment = '/* %08X %s */' % (start, name if name else 'gap')
        owner = containing_object(start)
        numeric = owner is not None and bool(NUMERIC.match(types.get(owner[2], 'struct').strip()))
        s = string_member(start, end) if name else None
        if s is not None:
            ctype, count, literal = s
            decls.append('    %s %s[%d]; %s' % (ctype, member, count, comment))
            inits.append('    %s, %s' % (literal, comment))
            continue
        if name in COMPUTED:
            count, tail = divmod(end - start, 4)
            assert not tail
            decls.append('    dword %s[%d]; %s' % (member, count, comment))
            inits.append('    {0}, /* %08X %s: filled at startup by %s */' % (start, name, COMPUTED[name]))
            continue
        template = template_member(k, start, end, name, member) if name else None
        if template is not None:
            decl, init, typed_pointers = template
            pointers.extend(typed_pointers)
            decls.append('%s %s' % (decl, comment))
            inits.append('    %s, %s' % (init, comment))
            continue
        typed = typed_member(start, end, name, member) if name else None
        if typed is not None:
            decl, init, typed_pointers, typed_end = typed
            pointers.extend(typed_pointers)
            typed_count += 1
            decls.append('%s %s' % (decl, comment))
            inits.append('    %s, %s' % (init, comment))
            if typed_end < end:
                rest_count, rest_tail = divmod(end - typed_end, 4)
                if rest_count:
                    decls.append('    dword %s_rest[%d]; /* beyond the declared type */' % (member, rest_count))
                    inits.append('    %s,' % dword_list(typed_end, rest_count, numeric))
                if rest_tail:
                    decls.append('    byte %s_rest_tail[%d];' % (member, rest_tail))
                    inits.append('    %s,' % byte_list(typed_end + 4 * rest_count, end))
            continue
        count, tail = divmod(end - start, 4)
        if name is None or count == 0:
            decls.append('    byte %s[%d]; %s' % (member, end - start, comment))
            inits.append('    %s, %s' % (byte_list(start, end), comment))
            continue
        decls.append('    dword %s[%d]; %s' % (member, count, comment))
        inits.append('    %s, %s' % (dword_list(start, count, numeric), comment))
        if tail:
            decls.append('    byte %s_tail[%d];' % (member, tail))
            inits.append('    %s,' % byte_list(start + 4 * count, end))
    layout_lines.append(decls)
    init_lines.append(inits)

# ---- header
hdr = [HEADER % 'include/thandor/generated/image_data.h',
       '/* Generated by tools/data/gen_image_data.py from the original thandor.exe. Do not edit. */\n\n'
       '#ifndef THANDOR_GENERATED_IMAGE_DATA_H\n#define THANDOR_GENERATED_IMAGE_DATA_H\n\n'
       '#include <thandor/generated/types.h>\n\n'
       '/* UI template node links: offsets from the template start, made into pointers when the\n'
       '   template is copied and linked. */\n'
       '#define UI_TEMPLATE_LINK(offset) ((UiNodeBase *)(offset))\n'
       '#define UI_TEMPLATE_NO_LINK ((UiNodeBase *)-1)\n\n#pragma pack(push, 1)\n']
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
           'typedef struct ThandorImageBlock { dword start; dword end; const byte *data; } ThandorImageBlock;\n'
           'typedef struct ThandorImagePointer { dword location; dword originalValue; } ThandorImagePointer;\n'
           'extern const ThandorImageBlock g_ThandorImageBlocks[%d];\n'
           'extern const ThandorImagePointer g_ThandorImagePointers[%d];\n\n#endif\n' % (len(blocks), len(pointers)))
open(os.path.join(common.REPO, 'include', 'thandor', 'generated', 'image_data.h'), 'w',
     encoding='utf-8').write(''.join(hdr))

# ---- source
src = [HEADER % 'src/generated/image_data.c',
       '/* Generated by tools/data/gen_image_data.py from the original thandor.exe: the data of\n'
       '   0x401000-0x58C000 in original order, one packed struct per run between code. Do not edit;\n'
       '   regenerate, or move objects out into hand-written definitions. */\n\n'
       '#include <thandor/thandor.h>\n#include <thandor/generated/image_data.h>\n\n'
       '#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */\n']
for k, (a, b) in enumerate(blocks):
    # a multi-line initializer gets its object comment as a heading instead of at its end
    lines = [re.sub(r'^    (\{\n.*), (/\* [0-9A-F]{8} [^\n]* \*/)$', r'    \2\n    \1,', line, flags=re.S)
             for line in init_lines[k]]
    src.append('\nImageData_%08X %s = {\n%s\n};\n' % (a, block_name(k), '\n'.join(lines)))
src.append('\nconst ThandorImageBlock g_ThandorImageBlocks[%d] = {\n' % len(blocks))
for k, (a, b) in enumerate(blocks):
    src.append('    {0x%08X, 0x%08X, (const byte *)&%s},\n' % (a, b, block_name(k)))
src.append('};\n\nconst ThandorImagePointer g_ThandorImagePointers[%d] = {\n' % len(pointers))
for location, value, kind, name in pointers:
    src.append('    {0x%08X, 0x%08X},\n' % (location, value))
src.append('};\n')
open(os.path.join(common.REPO, 'src', 'generated', 'image_data.c'), 'w', encoding='utf-8').write(''.join(src))

with open(os.path.join(args.work, 'image_pointers.tsv'), 'w', encoding='utf-8') as f:
    for p in pointers:
        f.write('%08x\t%08x\t%s\t%s\n' % p)
string_count = sum(1 for lines in layout_lines for l in lines if ' = ' not in l and ('word ' in l or 'char ' in l)
                   and not l.strip().startswith('dword'))
print('%d blocks, %d bytes, %d members (%d typed), %d function pointers, %d data pointers' % (
    len(blocks), sum(b - a for a, b in blocks), sum(len(m) for m in members), typed_count,
    sum(1 for p in pointers if p[2] == 'function'), sum(1 for p in pointers if p[2] == 'data')))
if missing:
    print('address macros outside every block:', ', '.join('%08x' % a for a in missing))
