"""Which data objects does the C code need? Roots are the objects whose names appear in the C
sources or hand-written headers; from there every dword that points into another data object is
followed (vtables, UI templates, pointer tables), except out of plain number tables.
Reads layout.tsv, writes reach.tsv (start, extent, name, kind, reached-from) and a summary."""
import bisect
import glob
import os
import re
import struct

import common

NUMERIC = re.compile(r'^(?:const )?(?:byte|word|dword|qword|short|ushort|int|uint|sdword|char|undefined[1248]?|u?int(?:8|16|32|64)_t|'
                     r'Q\d+|float|double|long|ulong|longlong|ulonglong|SoftwareBgraWordLanes|PackedArgb32)\s*'
                     r'(?:\(\*\)\[[^\]]*\](?:\[[^\]]*\])*)?\s*\*?$')

args = common.parse_arguments(__doc__)
START, END = common.TEXT_START, common.TEXT_END
text = common.load_text(args.original)
rows = []
for line in open(os.path.join(args.work, 'layout.tsv'), encoding='utf-8'):
    a, size, declared, name, kind, nonzero = line.rstrip('\n').split('\t')
    rows.append((int(a, 16), int(size), name, kind, int(nonzero)))
starts = [r[0] for r in rows]
types = {n: t for n, (t, _) in common.address_macros().items()}

source = ''.join(open(p, encoding='utf-8', errors='replace').read() for p in common.c_sources())
for path in glob.glob(os.path.join(common.REPO, 'include', '**', '*.h'), recursive=True):
    if not path.endswith('globals.h'):
        source += open(path, encoding='utf-8', errors='replace').read()
used = set(re.findall(r'\b[A-Za-z_]\w*\b', source))

def object_at(addr):
    i = bisect.bisect_right(starts, addr) - 1
    return i if i >= 0 and rows[i][0] <= addr < rows[i][0] + rows[i][1] else None

reached = {i: 'code' for i, r in enumerate(rows) if r[2] in used}
queue = list(reached)
while queue:
    i = queue.pop()
    addr, size, name = rows[i][0], rows[i][1], rows[i][2]
    if NUMERIC.match(types.get(name, 'struct').strip()):
        continue
    for off in range(0, size - 3, 4):
        value = struct.unpack_from('<I', text, addr - START + off)[0]
        j = object_at(value) if START <= value < END else None
        if j is not None and j not in reached:
            reached[j] = name
            queue.append(j)

with open(os.path.join(args.work, 'reach.tsv'), 'w', encoding='utf-8') as f:
    for i, r in enumerate(rows):
        f.write('%08x\t%d\t%s\t%s\t%s\n' % (r[0], r[1], r[2], r[3], reached.get(i, '-')))
summary = {}
for i, r in enumerate(rows):
    s = summary.setdefault((r[3], i in reached), [0, 0, 0])
    s[0], s[1], s[2] = s[0] + 1, s[1] + r[1], s[2] + r[4]
for (kind, hit), (count, size, nonzero) in sorted(summary.items()):
    print('%-10s %-10s %6d objects %8d bytes %8d nonzero' % (kind, 'reached' if hit else 'unreached',
                                                              count, size, nonzero))
print('reached objects known only by a Ghidra label (need a name and type):')
for i in sorted((i for i in reached if rows[i][3] == 'label'), key=lambda i: -rows[i][4])[:20]:
    print('  %08x %6d %-50s via %s' % (rows[i][0], rows[i][1], rows[i][2], reached[i]))
