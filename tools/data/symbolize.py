"""Symbolizes the last entry of crash_raw.log (written by the crash handler without the CRT) with the build that
crashed: its linker map (MSVC: link with /MAP, e.g. set LINK=/MAP before building; GCC: the build writes
thandor.map, which lists only global symbols) or, better for a GCC build, the executable itself through addr2line
(MinGW's binutils on the PATH; it reads the DWARF line info and knows static functions too).

usage: symbolize.py crash_raw.log build-rel/thandor.map
       symbolize.py crash_raw.log build-mingw-test/thandor.exe"""
import bisect
import re
import subprocess
import sys

IMAGE_START, IMAGE_END = 0x10000000, 0x10400000  # fixed base of the rebuilt executable, upper bound of its image

log, symbolsource = sys.argv[1], sys.argv[2]
entry = open(log, errors='replace').read().split('==== ')[-1].splitlines()

pc = None
m = re.search(r'rip=([0-9A-F]{16})', '\n'.join(entry))
if m:
    pc = int(m.group(1), 16)
words = []  # (stack offset, word)
for line in entry:
    m = re.match(r'\s*\+([0-9A-F]{3}):((?: [0-9A-F]{8})+)', line)
    if m:
        base = int(m.group(1), 16)
        words += [(base + 4 * k, int(w, 16)) for k, w in enumerate(m.group(2).split())]
code = sorted({w for _, w in words if IMAGE_START <= w < IMAGE_END} | ({pc} if pc else set()))

if symbolsource.lower().endswith('.exe'):
    out = subprocess.run(['addr2line', '-f', '-C', '-e', symbolsource] + ['0x%X' % a for a in code],
                         capture_output=True, text=True, check=True).stdout.splitlines()
    names = {}
    for k, a in enumerate(code):
        function, place = out[2 * k], out[2 * k + 1]
        names[a] = None if function == '??' else '%s (%s)' % (function, place)
    name = names.get
else:
    symbols = []
    for line in open(symbolsource, errors='replace'):
        m = re.match(r'\s*\d{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8,16})', line)  # MSVC /MAP
        if not m:
            m2 = re.match(r'\s+0x([0-9a-f]{16})\s+([A-Za-z_][^\s=]*)\s*$', line)  # GNU ld -Map
            if m2:
                symbols.append((int(m2.group(1), 16), m2.group(2)))
            continue
        symbols.append((int(m.group(2), 16), m.group(1)))
    symbols.sort()
    addresses = [s[0] for s in symbols]

    def name(value):
        i = bisect.bisect_right(addresses, value) - 1
        if i < 0 or not IMAGE_START <= value < IMAGE_END:
            return None
        return '%s+0x%X' % (symbols[i][1], value - symbols[i][0])

for line in entry[:3]:
    print(line)
if pc is not None:
    print('rip', name(pc) or '%X' % pc)
for offset, word in words:
    symbol = name(word) if IMAGE_START <= word < IMAGE_END else None
    if symbol:
        print('  rsp+%03X %08X %s' % (offset, word, symbol))
