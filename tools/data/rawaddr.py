"""Check: lists original-image address literals left in the C sources (comments and strings
ignored). Every data access should go through a named object (generated/globals.h,
data/recovered.h, THANDOR_ADDR). Remaining hits are listed with the object they fall into.
Known non-addresses (switch keys of continuation dispatchers, values that only look like
addresses) are filtered by context or listed in NOT_ADDRESSES."""
import bisect
import os
import re

import common

NOT_ADDRESSES = {
    0x440043,  # UTF-16 "CD" in a player record, not an address
    0x536c13, 0x576502,  # dead return-address pushes kept by the decompiler
    0x55f130, 0x55fb90, 0x55fc30,  # multiplayer command ids: offsets from handler code (TODO: handler table)
}

args = common.parse_arguments(__doc__)
sizes = common.global_sizes(args.work)
objects = sorted((a, s, n) for n, (a, s) in sizes.items())
starts = [o[0] for o in objects]
funcs = common.function_map()
rows = []
for path in common.c_sources():
    rel = os.path.relpath(path, common.REPO)
    if rel.endswith('function_map.c') or os.sep + 'bootstrap' + os.sep in rel or rel.endswith('richtext.c'):
        continue
    text = open(path, encoding='utf-8', errors='replace').read()
    code = common.strip_comments(text)
    lines = text.split('\n')
    for m in re.finditer(r'(?<![\w.])0x(?:00)?([45][0-9a-fA-F]{5})u?\b', code):
        a = int(m.group(1), 16)
        if not common.TEXT_START <= a < common.TEXT_END or a in NOT_ADDRESSES:
            continue
        line_no = code.count('\n', 0, m.start()) + 1
        ctx = lines[line_no - 1].strip()
        if re.search(r'case\s+0x|target\s*==|continuationEntryAddress', ctx):
            continue
        i = bisect.bisect_right(starts, a) - 1
        where = 'FUNCTION ENTRY' if a in funcs else (
            '%s+%x' % (objects[i][2], a - objects[i][0]) if i >= 0 else '-')
        rows.append('%08x %s:%d [%s] %s' % (a, rel, line_no, where, ctx[:110]))
for r in sorted(rows):
    print(r)
print('%d raw address literals' % len(rows))
