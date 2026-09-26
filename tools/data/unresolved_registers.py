"""Check: functions that still read register values the decompiler could not resolve
(in_EAX, in_ZF, unaff_EBX, extraout_ST1, ...). Each such read takes whatever the C compiler left in
that place, so it is either dead code or a real bug; the list is the work queue for fixing them
against the original assembly.

usage: unresolved_registers.py [--details]"""
import os
import re
import sys
from collections import Counter

import common

NAME = r'(?:in_[A-Z][A-Za-z0-9_]*|unaff_[A-Za-z0-9_]+|extraout_[A-Za-z0-9_]+)'
PLACEHOLDER = re.compile(r'\b(' + NAME + r')\b')
found = {}
for path in common.c_sources():
    rel = os.path.relpath(path, common.REPO)
    if os.sep + 'generated' + os.sep in path:
        continue
    code = common.strip_comments(open(path, encoding='utf-8', errors='replace').read())
    for m in re.finditer(r'\n(?:[\w \*]+?)\b(\w+)\s*\n?\s*\([^;{]*?\)\s*\n\{', code):
        start = m.end()
        end = code.find('\n}\n', start)
        body = code[start:end if end > 0 else len(code)]
        declared = set(re.findall(r'^\s+[\w ]+\*?\s*\b(' + NAME + r')\s*(?:=[^;]*)?;', body, re.M))
        # declared with an initializer: resolved on purpose (the declaration says why)
        initialized = set(re.findall(r'^\s+[\w ]+\*?\s*\b(' + NAME + r')\s*=[^;]*;', body, re.M))
        uses = PLACEHOLDER.findall(body)
        reads = [u for u in uses if u not in initialized and not (u in declared and uses.count(u) == 1)]
        if reads:
            found[(rel, m.group(1))] = sorted(set(reads))
kinds = Counter(re.sub(r'_\d+$', '', r) for regs in found.values() for r in regs)
print('%d functions read unresolved registers' % len(found))
print('by register: ' + ', '.join('%s %d' % kv for kv in kinds.most_common()))
if '--details' in sys.argv:
    for (path, name), regs in sorted(found.items()):
        print('%-40s %-60s %s' % (path, name, ', '.join(regs)))
