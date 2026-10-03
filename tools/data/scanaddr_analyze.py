"""Evaluates scanaddr.txt from OPEN_THANDOR_SELFTEST=scanaddr: which game files (or saves, via
OPEN_THANDOR_SCANFILES) store dwords equal to an original function entry (the Address: comments in src/) or a
labeled data address (Ghidra labels, ghidra/export/labels.jsonl).
Everything else in the address range is coincidence (texture, sound, text data).

usage: scanaddr_analyze.py path/to/scanaddr.txt"""
from collections import Counter, defaultdict

import common

args = common.parse_arguments(__doc__, lambda p: p.add_argument('scan', help='scanaddr.txt'))
funcs = common.function_map()
globs = {a: n for a, n in common.ghidra_labels() if a not in funcs}
per_file = defaultdict(Counter)
examples = defaultdict(list)
for line in open(args.scan, errors='replace'):
    parts = line.split()
    if len(parts) != 5:
        continue
    package, entry, tag, offset, value = parts
    v = int(value, 16)
    kind = 'function' if v in funcs else 'global' if v in globs else None
    if kind is None:
        continue
    per_file[(package, entry)][kind] += 1
    if len(examples[(package, entry)]) < 4:
        examples[(package, entry)].append('%s@+%s=%s' % (kind, offset, funcs.get(v) or globs.get(v)))
for key, counts in sorted(per_file.items(), key=lambda x: -sum(x[1].values())):
    print('%-12s %-40s functions=%d globals=%d  %s' % (key[0], key[1], counts['function'], counts['global'],
                                                       '; '.join(examples[key])))
print('%d files with exact hits' % len(per_file))
