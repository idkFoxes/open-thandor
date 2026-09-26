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

# ---- member layout per block
def identifier(name, used):
    # leading underscore: the object names are also macros (globals.h), which would expand here
    base = '_' + re.sub(r'\W', '_', name)
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

def address_expression(value):
    """C constant expression for the generated address of original data address value."""
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

def value_expression(field_addr, owner_numeric):
    value = dword_at(field_addr)
    if not owner_numeric and value in funcs:
        pointers.append((field_addr, value, 'function', funcs[value]))
        return '(dword)%s' % funcs[value]
    if not owner_numeric and value in anchors:
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

layout_lines = []   # struct member declarations per block
init_lines = []     # initializer per block
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
       '#include <thandor/generated/types.h>\n\n#pragma pack(push, 1)\n']
for k, (a, b) in enumerate(blocks):
    hdr.append('\n/* original 0x%08X-0x%08X */\ntypedef struct ImageData_%08X {\n%s\n} ImageData_%08X;\n'
               'extern ImageData_%08X %s;\n' % (a, b, a, '\n'.join(layout_lines[k]), a, a, block_name(k)))
hdr.append('\n#pragma pack(pop)\n\n/* Original address -> generated storage. */\n')
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
       '#include <thandor/thandor.h>\n#include <thandor/generated/image_data.h>\n']
for k, (a, b) in enumerate(blocks):
    src.append('\nImageData_%08X %s = {\n%s\n};\n' % (a, block_name(k), '\n'.join(init_lines[k])))
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
print('%d blocks, %d bytes, %d members, %d function pointers, %d data pointers' % (
    len(blocks), sum(b - a for a, b in blocks), sum(len(m) for m in members),
    sum(1 for p in pointers if p[2] == 'function'), sum(1 for p in pointers if p[2] == 'data')))
if missing:
    print('address macros outside every block:', ', '.join('%08x' % a for a in missing))
