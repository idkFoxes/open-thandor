"""Data layout of the original image. Every non-code byte of 0x401000-0x58C000 is assigned to one
object. Object starts: typed globals (globalmap.txt), then Ghidra labels outside them. A global
inside a larger declared global is a field of it (members.tsv). Each object extends to the next
start or code byte.

Writes layout.tsv (start, extent, declared size, name, kind, nonzero bytes) and members.tsv, and
prints: bytes per kind, globals whose type is smaller than their extent, and the largest runs of
bytes that belong to no object (usually code outside Ghidra functions)."""
import os

import common

args = common.parse_arguments(__doc__)
START, END = common.TEXT_START, common.TEXT_END
text = common.load_text(args.original)
code = common.code_mask(text, common.instruction_starts(args.asm), args.work)
recovered = set(n for n in common.address_macros()
                if n in open(os.path.join(common.REPO, 'include', 'thandor', 'data', 'recovered.h')).read())

objects = {}
members = []
container_end = 0
for addr, size, name in sorted(((a, s, n) for n, (a, s) in common.global_sizes(args.work).items()),
                               key=lambda x: (x[0], -x[1])):
    if addr < container_end:
        members.append((addr, size, name))
        continue
    objects[addr] = (name, size, 'recovered' if name in recovered else 'global')
    container_end = max(container_end, addr + size)
inside = bytearray(END - START)
for addr, (_, size, _) in objects.items():
    for b in range(max(addr + 1, START), min(addr + size, END)):
        inside[b - START] = 1
for addr, name in common.ghidra_labels():
    if START <= addr < END and addr not in objects and not code[addr - START] and not inside[addr - START]:
        objects[addr] = (name, 0, 'label')

rows = []
keys = sorted(k for k in objects if START <= k < END and not code[k - START])
for i, addr in enumerate(keys):
    limit = keys[i + 1] if i + 1 < len(keys) else END
    end = addr
    while end < limit and not code[end - START]:
        end += 1
    name, declared, kind = objects[addr]
    rows.append((addr, end - addr, declared, name, kind, sum(1 for x in text[addr - START:end - START] if x)))

with open(os.path.join(args.work, 'layout.tsv'), 'w', encoding='utf-8') as f:
    for r in rows:
        f.write('%08x\t%d\t%d\t%s\t%s\t%d\n' % r)
with open(os.path.join(args.work, 'members.tsv'), 'w', encoding='utf-8') as f:
    for addr, size, name in members:
        f.write('%08x\t%d\t%s\n' % (addr, size, name))

kinds = {}
for addr, size, declared, name, kind, nonzero in rows:
    k = kinds.setdefault(kind, [0, 0, 0])
    k[0], k[1], k[2] = k[0] + 1, k[1] + size, k[2] + nonzero
print('objects by kind (count, bytes, nonzero bytes):')
for kind, v in sorted(kinds.items()):
    print('  %-10s %6d %8d %8d' % (kind, *v))
print('globals that are fields of a larger global: %d (members.tsv)' % len(members))
small = [r for r in rows if r[4] == 'global' and r[1] > r[2] and any(
    text[b - START] not in (0, 0x90) for b in range(r[0] + r[2], r[0] + r[1]))]
print('globals whose declared type is smaller than their extent: %d' % len(small))
for r in sorted(small, key=lambda r: -(r[1] - r[2]))[:15]:
    print('  %08x %-50s declared %6d extent %6d' % (r[0], r[3], r[2], r[1]))

covered = bytearray(END - START)
for addr, size, *_ in rows:
    for b in range(addr, addr + size):
        covered[b - START] = 1
runs = []
i = 0
while i < END - START:
    if not code[i] and not covered[i] and text[i] not in (0, 0x90):
        j = i
        while j < END - START and not code[j] and not covered[j]:
            j += 1
        runs.append((sum(1 for x in text[i:j] if x not in (0, 0x90)), START + i, j - i))
        i = j
    else:
        i += 1
print('bytes in no object: %d nonzero; largest runs:' % sum(r[0] for r in runs))
for nonzero, addr, length in sorted(runs, reverse=True)[:8]:
    print('  %08x %6d bytes (%d nonzero) %s' % (addr, length, nonzero, text[addr - START:addr - START + 12].hex()))
