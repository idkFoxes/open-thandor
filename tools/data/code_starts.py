"""Writes code_starts.bin: every original instruction start as a little-endian dword.
Copy it next to thandor.exe and start with OPEN_THANDOR_POISON=1 to overwrite the original code
with INT3 (see Thandor_MapOriginalImage); any jump into original code then shows in crash.log."""
import os
import struct

import common

args = common.parse_arguments(__doc__)
starts = [a for a in common.instruction_starts(args.asm) if common.TEXT_START <= a < common.TEXT_END]
out = os.path.join(args.work, 'code_starts.bin')
with open(out, 'wb') as f:
    for a in starts:
        f.write(struct.pack('<I', a))
print('%d instruction starts -> %s' % (len(starts), out))
