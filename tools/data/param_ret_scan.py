"""Check: C functions that take more parameters than the original pops (RET n) and that are
reached through code pointers (vtables, tables, callbacks). Callers through such pointers pass only
the original stack arguments, so the extra C parameter (usually a register pseudo-parameter such as
EBX) reads garbage; the unoptimized build often hides this."""
import os
import re
import struct

import common

args = common.parse_arguments(__doc__)
funcs = {name: addr for addr, name in common.function_map().items()}
params = {}
for path in common.c_sources():
    s = open(path, encoding='utf-8', errors='replace').read()
    for m in re.finditer(r'\n(\w+)\s*\n?\s*\(([^()]*(?:\([^()]*\)[^()]*)*)\)\s*\n\s*\{', s):
        if m.group(1) in funcs:
            a = m.group(2).strip()
            params[m.group(1)] = (0 if a in ('', 'void') else a.count(',') + 1, os.path.relpath(path, common.REPO))
popped = {}
for name in funcs:
    path = os.path.join(args.asm, name + '.asm')
    if os.path.exists(path):
        rets = set(int(m.group(1) or '0', 16) for m in re.finditer(r'\bRET(?: 0x([0-9a-f]+))?\s*$',
                                                                   open(path, errors='replace').read(), re.M))
        if len(rets) == 1:
            popped[name] = rets.pop()
exe = open(args.original, 'rb').read()
source = common.strip_comments(''.join(open(p, encoding='utf-8', errors='replace').read() for p in common.c_sources()))
hits = 0
for name, (count, path) in sorted(params.items()):
    if name not in popped or count <= popped[name] // 4:
        continue
    stored = exe.find(struct.pack('<I', funcs[name])) >= 0
    taken = re.search(r'(=|,|\()\s*&?' + name + r'\s*[;,)]', source) is not None
    if stored or taken:
        hits += 1
        print('%s  %s  C=%d original=%d  %s' % (path, name, count, popped[name] // 4,
                                               'stored in image' if stored else 'address taken in C'))
print('%d candidates (check each call site through a pointer)' % hits)
