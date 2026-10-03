"""Shared helpers for the check tools in tools/data (param_ret_scan.py, scanaddr_analyze.py,
unresolved_registers.py).

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

REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))


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


ADDRESS = re.compile(r'/\*\s*Address: 0x([0-9A-Fa-f]{8})\b')
DEFINITION = re.compile(r'(?<![\w.>])([A-Za-z_]\w*)\s*\(')
CALLING_CONVENTIONS = ('__declspec', '__cdecl', '__stdcall', '__fastcall', '__thiscall')


def function_map():
    """original entry address -> C function name, from docs/original_addresses.txt (written in step 5c, when
    the `/* Address: 0x... */` comments were removed from src/)"""
    listing = os.path.join(REPO, 'docs', 'original_addresses.txt')
    if os.path.exists(listing):
        funcs = {}
        for line in open(listing, encoding='utf-8'):
            parts = line.split()
            if len(parts) >= 3 and parts[1] == 'function':
                funcs[int(parts[0], 16)] = parts[2]
        return funcs
    return function_map_from_comments()


def function_map_from_comments():
    """original entry address -> C function name, from the `/* Address: 0x... */` comment directly above every
    recovered function in src/ (only blank and preprocessor lines may stand between them)"""
    funcs = {}
    for path in c_sources():
        text = open(path, encoding='utf-8', errors='replace').read()
        for m in ADDRESS.finditer(text):
            end = text.find('*/', m.end())
            if end < 0:
                continue
            lines = text[end + 2:].split('\n')
            k = 0 if lines[0].strip() else 1
            while k < len(lines) and (not lines[k].strip() or lines[k].lstrip().startswith('#')
                                      or (k > 0 and lines[k - 1].rstrip().endswith('\\'))):
                k += 1
            head = '\n'.join(lines[k:k + 12])
            body = head.find('{')
            semicolon = head.find(';')
            if body < 0 or 0 <= semicolon < body:
                continue
            names = [n for n in DEFINITION.findall(head[:body]) if n not in CALLING_CONVENTIONS]
            if names:
                funcs[int(m.group(1), 16)] = names[0]
    return funcs


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
