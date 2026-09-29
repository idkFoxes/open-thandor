"""Inventory of the compiled-in original data (src/generated/image_data.c).

usage: python tools/data/image_data_report.py [--work DIR] [--top N] [--kind KIND]

Reads <work>/image_objects.tsv, written by gen_image_data.py (one line per member: start, end, name, kind,
nonzero bytes, pointers inside, declared type). Kinds:
  typed          object written through a real struct / array type with named fields
  typed-generic  written through its declared type, but that is only uint32_t[n] / uint8_t[n] and the like
  ui-template    UI node template, one member per node
  string         text literal
  computed       table the program builds at startup (storage only)
  rest           bytes behind an object's declared type (dword grid)
  raw            named object without a usable type (dword grid)
  gap            bytes between objects (padding, unnamed)
  padding        alignment padding behind a string or template
  jump-table     switch table of the original code, not used by the C code

Prints the share of bytes shown typed and the largest raw objects with how often the C code names them.
Zero-only ranges (storage the program fills at runtime) are counted apart: they need no content."""
import argparse
import collections
import os
import pathlib
import re

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
parser.add_argument('--work', default=str(ROOT / 'build-data'))
parser.add_argument('--top', type=int, default=40)
parser.add_argument('--kind', help='list every object of this kind')
args = parser.parse_args()

rows = []
for line in open(os.path.join(args.work, 'image_objects.tsv'), encoding='utf-8'):
    start, end, name, kind, nonzero, pointers, declared = line.rstrip('\n').split('\t')
    rows.append(dict(start=int(start, 16), end=int(end, 16), name=name, kind=kind, nonzero=int(nonzero),
                     pointers=int(pointers), declared=declared))

# how often the C code names each object (by name or by its THANDOR_IMAGE address)
code = []
for path in (ROOT / 'src').rglob('*.c'):
    if 'generated' not in path.parts:
        code.append(re.sub(r'/\*.*?\*/', ' ', path.read_text(encoding='utf-8', errors='replace'), flags=re.S))
words = collections.Counter(w for text in code for w in re.findall(r'\b\w+\b', text))
for r in rows:
    r['uses'] = words.get(r['name'], 0) + words.get('THANDOR_IMAGE_0x%08x' % r['start'], 0)

READABLE = ('typed', 'ui-template', 'string', 'jump-table', 'padding')
total = sum(r['end'] - r['start'] for r in rows)
zero = sum(r['end'] - r['start'] for r in rows if r['nonzero'] == 0 and r['kind'] != 'computed')
by_kind = collections.Counter()
by_kind_content = collections.Counter()
for r in rows:
    by_kind[r['kind']] += r['end'] - r['start']
    if r['nonzero']:
        by_kind_content[r['kind']] += r['end'] - r['start']
computed = by_kind['computed']
content = total - zero - computed
readable = sum(by_kind_content[k] for k in READABLE)
print('%d bytes in %d members: %d zero-only (storage filled at runtime), %d in tables computed at startup,'
      ' %d with content' % (total, len(rows), zero, computed, content))
print('typed and named: %.1f %% of the bytes with content (%d of %d)' % (100.0 * readable / content, readable, content))
print()
print('%-14s %10s %10s' % ('kind', 'bytes', 'content'))
for kind, size in by_kind.most_common():
    print('%-14s %10d %10d' % (kind, size, by_kind_content[kind]))

raw = [r for r in rows if r['kind'] in ('raw', 'rest', 'typed-generic', 'gap') and r['nonzero']]
raw.sort(key=lambda r: (-(r['end'] - r['start']), -r['uses']))
print()
print('largest objects with content that are not typed (size, uses in the C code):')
for r in raw[:args.top]:
    print('  %08X %7d  %-13s %4d uses  %-50s %s' % (r['start'], r['end'] - r['start'], r['kind'], r['uses'],
                                                  r['name'] or '(gap)', r['declared']))
if args.kind:
    print()
    for r in rows:
        if r['kind'] == args.kind:
            print('  %08X %7d %4d uses  %s  %s' % (r['start'], r['end'] - r['start'], r['uses'], r['name'],
                                                 r['declared']))
