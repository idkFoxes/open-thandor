"""Shared loading helpers for the data-layout tools.

Inputs (all given on the command line, see add_common_arguments):
  --original   the original thandor.exe (image base 0x400000)
  --asm        per-function disassembly written by tools/ghidra/DumpDisassembly.java
  --work       output/work directory (default: build-data)
"""
import argparse
import glob
import json
import os
import re
import shutil
import struct
import subprocess

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
IMAGE_BASE = 0x400000
TEXT_START = 0x401000
TEXT_END = 0x58C000


def add_common_arguments(parser):
    parser.add_argument('--original', default=os.path.join(REPO, 'thandor_original.exe'),
                        help='original thandor.exe (default: thandor_original.exe in the repository root)')
    parser.add_argument('--asm', default=os.path.join(REPO, 'build-data', 'asm'),
                        help='directory of per-function .asm files from DumpDisassembly.java')
    parser.add_argument('--work', default=os.path.join(REPO, 'build-data'), help='work/output directory')
    return parser


def parse_arguments(description, extra=None):
    parser = add_common_arguments(argparse.ArgumentParser(description=description))
    if extra:
        extra(parser)
    args = parser.parse_args()
    os.makedirs(args.work, exist_ok=True)
    return args


def load_text(original):
    """Raw bytes of the original .text section (0x401000-0x58C000)."""
    exe = open(original, 'rb').read()
    pe = struct.unpack_from('<I', exe, 0x3c)[0]
    opt = struct.unpack_from('<H', exe, pe + 20)[0]
    raw = struct.unpack_from('<I', exe, pe + 24 + opt + 20)[0]
    return exe[raw:raw + (TEXT_END - TEXT_START)]


def instruction_starts(asm_dir):
    starts = set()
    for path in glob.glob(os.path.join(asm_dir, '*.asm')):
        for m in re.finditer(r'^\s+([0-9a-f]{8})\s+\S', open(path, errors='replace').read(), re.M):
            starts.add(int(m.group(1), 16))
    if not starts:
        raise SystemExit('no disassembly in %s (run tools/ghidra/DumpDisassembly.java first)' % asm_dir)
    return sorted(starts)


def instruction_lengths(text, work):
    """Instruction lengths from a linear objdump sweep (optional; cached in the work directory)."""
    cache = os.path.join(work, 'text_objdump.txt')
    if not os.path.exists(cache):
        objdump = shutil.which('objdump')
        if objdump is None:
            return {}
        binpath = os.path.join(work, 'text.bin')
        open(binpath, 'wb').write(text)
        out = subprocess.run([objdump, '-D', '-b', 'binary', '-mi386', '--adjust-vma=0x%x' % TEXT_START, binpath],
                             capture_output=True, text=True).stdout
        open(cache, 'w').write(out)
    lengths = {}
    previous = None
    for line in open(cache):
        m = re.match(r'\s+([0-9a-f]+):\t((?:[0-9a-f]{2} )+)\s*(\S?)', line)
        if not m:
            continue
        count = len(m.group(2).split())
        if m.group(3) == '' and previous is not None:
            lengths[previous] += count  # objdump wraps long instructions onto a second line
        else:
            previous = int(m.group(1), 16)
            lengths[previous] = count
    return lengths


def code_mask(text, starts, work):
    """bytearray over .text: 1 where an original instruction byte is."""
    lengths = instruction_lengths(text, work)
    code = bytearray(TEXT_END - TEXT_START)
    for i, a in enumerate(starts):
        if not TEXT_START <= a < TEXT_END:
            continue
        nxt = starts[i + 1] if i + 1 < len(starts) else TEXT_END
        n = lengths.get(a)
        if n is None or a + n > nxt:
            n = max(1, nxt - a) if nxt - a <= 15 else 1
        for b in range(a, min(a + n, TEXT_END)):
            code[b - TEXT_START] = 1
    return code


def function_map():
    """original entry address -> C function name, from src/generated/function_map.c"""
    funcs = {}
    for line in open(os.path.join(REPO, 'src', 'generated', 'function_map.c'), encoding='utf-8'):
        m = re.search(r'\{0x([0-9A-Fa-f]+)u, \(void \*\)&(\w+)\}', line)
        if m:
            funcs[int(m.group(1), 16)] = m.group(2)
    return funcs


def address_macros():
    """name -> (type text, address) for every address-defined object in the headers."""
    out = {}
    for header in (os.path.join(REPO, 'include', 'thandor', 'generated', 'globals.h'),
                   os.path.join(REPO, 'include', 'thandor', 'data', 'recovered.h')):
        text = open(header, encoding='utf-8', errors='replace').read()
        for m in re.finditer(r'^#define (\w+) \(\*\((.+)\)(0x[0-9a-fA-F]+)\)\s*$', text, re.M):
            out[m.group(1)] = (m.group(2), int(m.group(3), 16))
    return out


def global_sizes(work):
    """name -> (address, sizeof) from globalmap.txt written by globalmap.py."""
    path = os.path.join(work, 'globalmap.txt')
    if not os.path.exists(path):
        raise SystemExit('%s missing: run tools/data/globalmap.py first' % path)
    sizes = {}
    for line in open(path):
        name, addr, size = line.split()
        sizes[name] = (int(addr, 16), int(size))
    return sizes


def ghidra_labels():
    path = os.path.join(REPO, 'ghidra', 'export', 'labels.jsonl')
    return [(int(j['address'], 16), j['name']) for j in map(json.loads, open(path))]


def strip_comments(source):
    """Replaces comments and string literals with spaces, keeping offsets and line breaks."""
    out = list(source)
    i, n = 0, len(source)
    while i < n:
        if source.startswith('/*', i):
            j = source.find('*/', i + 2)
            j = n if j < 0 else j + 2
        elif source.startswith('//', i):
            j = source.find('\n', i)
            j = n if j < 0 else j
        elif source[i] == '"':
            j = i + 1
            while j < n and source[j] != '"':
                j += 2 if source[j] == '\\' else 1
            j = min(j + 1, n)
        else:
            i += 1
            continue
        for k in range(i, j):
            if out[k] != '\n':
                out[k] = ' '
        i = j
    return ''.join(out)


def c_sources():
    return glob.glob(os.path.join(REPO, 'src', '**', '*.c'), recursive=True)
