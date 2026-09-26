"""Symbolizes the last entry of crash_raw.log (written by the crash handler without the CRT) with
the linker map of the build that crashed (link with /MAP, e.g. set LINK=/MAP before building).

usage: symbolize.py crash_raw.log build-rel/thandor.map"""
import bisect
import re
import sys

log, mapfile = sys.argv[1], sys.argv[2]
symbols = []
for line in open(mapfile, errors='replace'):
    m = re.match(r'\s*\d{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})', line)
    if m:
        symbols.append((int(m.group(2), 16), m.group(1)))
symbols.sort()
addresses = [s[0] for s in symbols]

def name(value):
    i = bisect.bisect_right(addresses, value) - 1
    if i < 0 or not 0x10000000 <= value < 0x10400000:
        return None
    return '%s+0x%X' % (symbols[i][1], value - symbols[i][0])

entry = open(log, errors='replace').read().split('==== ')[-1].splitlines()
for line in entry[:3]:
    print(line)
m = re.search(r'eip=([0-9A-F]{8})', '\n'.join(entry))
if m:
    print('eip', name(int(m.group(1), 16)) or m.group(1))
for line in entry:
    m = re.match(r'\s*\+([0-9A-F]{3}):((?: [0-9A-F]{8})+)', line)
    if not m:
        continue
    base = int(m.group(1), 16)
    for k, word in enumerate(m.group(2).split()):
        symbol = name(int(word, 16))
        if symbol:
            print('  esp+%03X %s %s' % (base + 4 * k, word, symbol))
