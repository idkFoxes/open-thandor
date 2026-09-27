"""Checks that a source change did not change the generated machine code (renames, comments,
formatting). Compares the .text of two builds function by function, using their linker maps.

usage:
  check_codegen.py save  <build-dir> <baseline-dir>   copy thandor.exe/.map as the baseline
  check_codegen.py check <build-dir> <baseline-dir>   list functions whose code differs
Exit status 1 when code differs."""
import os
import re
import shutil
import struct
import sys


def text_section(path):
    data = open(path, 'rb').read()
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    count = struct.unpack_from('<H', data, pe + 6)[0]
    opt = struct.unpack_from('<H', data, pe + 20)[0]
    base = struct.unpack_from('<I', data, pe + 24 + 28)[0]
    for i in range(count):
        o = pe + 24 + opt + 40 * i
        if data[o:o + 8].rstrip(b'\0') == b'.text':
            size, rva, raw_size, raw = struct.unpack_from('<IIII', data, o + 8)
            return base + rva, data[raw:raw + min(size, raw_size)]
    raise SystemExit('no .text in ' + path)


def functions(mapfile):
    symbols = []
    for line in open(mapfile, errors='replace'):
        m = re.match(r'\s*0001:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})', line)
        if m:
            symbols.append((int(m.group(2), 16), m.group(1)))
    return sorted(set(symbols))


def split(exe, mapfile):
    start, text = text_section(exe)
    syms = functions(mapfile)
    out = {}
    for i, (addr, name) in enumerate(syms):
        end = syms[i + 1][0] if i + 1 < len(syms) else start + len(text)
        out[name] = text[addr - start:end - start]
    return out


mode, build, baseline = sys.argv[1], sys.argv[2], sys.argv[3]
if mode == 'save':
    os.makedirs(baseline, exist_ok=True)
    for name in ('thandor.exe', 'thandor.map', 'thandor.pdb'):
        shutil.copy(os.path.join(build, name), os.path.join(baseline, name))
    print('baseline saved')
    sys.exit(0)
old = split(os.path.join(baseline, 'thandor.exe'), os.path.join(baseline, 'thandor.map'))
new = split(os.path.join(build, 'thandor.exe'), os.path.join(build, 'thandor.map'))
changed = sorted(n for n in set(old) | set(new) if old.get(n) != new.get(n))
# relocated call targets change bytes when any function moves; report only size/content changes
# of functions that exist in both, plus added/removed ones
if not changed:
    print('machine code identical (%d functions)' % len(new))
    sys.exit(0)
for name in changed[:60]:
    print('differs: %s (%s -> %s bytes)' % (name, len(old.get(name, b'')), len(new.get(name, b''))))
print('%d functions differ' % len(changed))
sys.exit(1)
