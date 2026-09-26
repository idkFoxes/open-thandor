"""Generates the data of the original image as C, so the executable no longer needs it.

Every contiguous run of data bytes in 0x401000-0x58C000 (everything that is not an original
instruction) becomes one dword array, in original order, so code that walks from one object into
its neighbour keeps working. Values stay numbers except pointers, which become symbols:
  - a dword equal to an original function entry  -> (dword)CFunction
  - a dword pointing at a known data anchor (object/member start, Ghidra label or symbol) inside a
    block, found in an object that is not a plain number table   -> (dword)&g_ImageData_x[i] (+ bytes)
Everything else stays the original number.

Outputs:
  src/generated/image_data.c               the arrays, with a comment at every named object
  include/thandor/generated/image_data.h   THANDOR_IMAGE_0x<address> for every address used by the
                                           address macros (globals.h, recovered.h), and the block
                                           table for the comparison self-test
  <work>/image_pointers.tsv                every converted pointer, for review
Needs globalmap.txt and layout.tsv/members.tsv in the work directory (globalmap.py, layout.py)."""
import bisect
import json
import os
import re
import struct

import common

NUMERIC = re.compile(r'^(?:const )?(?:byte|word|dword|qword|short|ushort|int|uint|sdword|char|undefined[1248]?|'
                     r'Q\d+|float|double|long|ulong|longlong|ulonglong|SoftwareBgraWordLanes|PackedArgb32)\s*'
                     r'(?:\(\*\)\[[^\]]*\](?:\[[^\]]*\])*)?\s*\*?$')

args = common.parse_arguments(__doc__)
START, END = common.TEXT_START, common.TEXT_END
text = common.load_text(args.original)
code = common.code_mask(text, common.instruction_starts(args.asm), args.work)
funcs = common.function_map()
macros = common.address_macros()
sizes = common.global_sizes(args.work)

# ---- blocks: runs of non-code bytes, dword aligned, merged across gaps shorter than 8 bytes
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
# keep only blocks that hold something the code can name (object, member, label): the rest is
# padding between functions
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

# ---- data anchors: addresses a data pointer may legitimately point at
anchors = set(a for _, (a, _) in sizes.items())
for line in open(os.path.join(args.work, 'members.tsv'), encoding='utf-8'):
    anchors.add(int(line.split('\t')[0], 16))
anchors.update(a for a, _ in common.ghidra_labels())
for name in ('symbols.jsonl', 'strings.jsonl'):
    for j in map(json.loads, open(os.path.join(common.REPO, 'ghidra', 'export', name))):
        anchors.add(int(j['address'], 16))
anchors = set(a for a in anchors if START <= a < END and block_of(a) is not None and not code[a - START])

# ---- which objects are plain number tables (never hold pointers)
objects = []
for line in open(os.path.join(args.work, 'layout.tsv'), encoding='utf-8'):
    a, size, declared, name, kind, nonzero = line.rstrip('\n').split('\t')
    objects.append((int(a, 16), int(size), name))
object_starts = [o[0] for o in objects]
types = {n: t for n, (t, _) in macros.items()}

def containing_object(addr):
    k = bisect.bisect_right(object_starts, addr) - 1
    return objects[k] if k >= 0 and objects[k][0] <= addr < objects[k][0] + objects[k][1] else None

names_at = {}
for n, (a, s) in sizes.items():
    names_at.setdefault(a, []).append(n)
for line in open(os.path.join(args.work, 'members.tsv'), encoding='utf-8'):
    a, s, n = line.rstrip('\n').split('\t')
    names_at.setdefault(int(a, 16), []).append(n)
for a, n in common.ghidra_labels():
    if START <= a < END and not code[a - START] and a not in names_at and \
            not re.match(r'(DAT|LAB|UNK|PTR|switchD|caseD|override|s_|u_|[a-z]Ram)', n):
        names_at[a] = [n]

# ---- emit
pointers = []
out = []
out.append('/*\n * Open Thandor\n * Project: https://github.com/idkFoxes/open-thandor/tree/main\n'
           ' * File: https://github.com/idkFoxes/open-thandor/blob/main/src/generated/image_data.c\n */\n\n'
           '/* Generated by tools/data/gen_image_data.py from the original thandor.exe: the data of\n'
           '   0x401000-0x58C000 in original order, one array per run between code. Do not edit;\n'
           '   regenerate, or move objects out into hand-written definitions. */\n\n'
           '#include <thandor/thandor.h>\n#include <thandor/generated/image_data.h>\n\n')
for k, (a, b) in enumerate(blocks):
    out.append('dword %s[0x%X] = {\n' % (block_name(k), (b - a) // 4))
    line = []
    for addr in range(a, b, 4):
        labels = [(addr + o, n) for o in range(4) for n in sorted(names_at.get(addr + o, []))]
        if labels and line:
            out.append('    %s,\n' % ', '.join(line))
            line = []
        for at, n in labels:
            out.append('    /* %08X %s */\n' % (at, n))
        # code bytes at the block edges (alignment) are not data: zero
        value = struct.unpack('<I', bytes(0 if not START <= x < END or code[x - START] else text[x - START]
                                          for x in range(addr, addr + 4)))[0]
        owner = containing_object(addr)
        numeric = owner is not None and NUMERIC.match(types.get(owner[2], 'struct').strip())
        expr = '0x%08X' % value
        if value in funcs and not numeric:
            expr = '(dword)%s' % funcs[value]
            pointers.append((addr, value, 'function', funcs[value]))
        elif value in anchors and not numeric:
            t = block_of(value)
            index, rest = divmod(value - blocks[t][0], 4)
            expr = '(dword)&%s[0x%X]' % (block_name(t), index) if rest == 0 else \
                   '(dword)((byte *)&%s[0x%X] + %d)' % (block_name(t), index, rest)
            if names_at.get(value):
                expr += ' /* %s */' % sorted(names_at[value])[0]
            pointers.append((addr, value, 'data', owner[2] if owner else '-'))
        line.append(expr)
        if len(line) == 8 or expr.startswith('(dword)'):
            out.append('    %s,\n' % ', '.join(line))
            line = []
    if line:
        out.append('    %s,\n' % ', '.join(line))
    out.append('};\n\n')
open(os.path.join(common.REPO, 'src', 'generated', 'image_data.c'), 'w', encoding='utf-8').write(''.join(out))

# ---- header: address macros used by globals.h / recovered.h
used = set(a for _, (_, a) in macros.items())
hdr = ['/*\n * Open Thandor\n * Project: https://github.com/idkFoxes/open-thandor/tree/main\n'
       ' * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/generated/image_data.h\n */\n\n'
       '/* Generated by tools/data/gen_image_data.py. */\n\n'
       '#ifndef THANDOR_GENERATED_IMAGE_DATA_H\n#define THANDOR_GENERATED_IMAGE_DATA_H\n\n'
       '#include <thandor/generated/types.h>\n\n']
for k, (a, b) in enumerate(blocks):
    hdr.append('extern dword %s[0x%X];\n' % (block_name(k), (b - a) // 4))
hdr.append('\n/* Original address -> generated storage. */\n')
missing = []
for addr in sorted(used):
    k = block_of(addr)
    if k is None:
        if not START <= addr < END:
            # numbers Ghidra typed as addresses (low error codes, the image base in a comparison)
            hdr.append('#define THANDOR_IMAGE_0x%08x ((uintptr_t)0x%08xu)\n' % (addr, addr))
        else:
            missing.append(addr)
        continue
    hdr.append('#define THANDOR_IMAGE_0x%08x ((uintptr_t)%s + 0x%X)\n' % (addr, block_name(k), addr - blocks[k][0]))
hdr.append('\n/* Blocks for the comparison self-test: original start, end, storage. */\n'
           'typedef struct ThandorImageBlock { dword start; dword end; dword *data; } ThandorImageBlock;\n'
           'extern const ThandorImageBlock g_ThandorImageBlocks[%d];\n\n#endif\n' % len(blocks))
open(os.path.join(common.REPO, 'include', 'thandor', 'generated', 'image_data.h'), 'w',
     encoding='utf-8').write(''.join(hdr))
with open(os.path.join(common.REPO, 'src', 'generated', 'image_data.c'), 'a', encoding='utf-8') as f:
    f.write('const ThandorImageBlock g_ThandorImageBlocks[%d] = {\n' % len(blocks))
    for k, (a, b) in enumerate(blocks):
        f.write('    {0x%08X, 0x%08X, %s},\n' % (a, b, block_name(k)))
    f.write('};\n')

with open(os.path.join(args.work, 'image_pointers.tsv'), 'w', encoding='utf-8') as f:
    for p in pointers:
        f.write('%08x\t%08x\t%s\t%s\n' % p)
print('%d blocks, %d bytes, %d function pointers, %d data pointers' % (
    len(blocks), sum(b - a for a, b in blocks), sum(1 for p in pointers if p[2] == 'function'),
    sum(1 for p in pointers if p[2] == 'data')))
if missing:
    print('address macros outside every block:', ', '.join('%08x' % a for a in missing))
