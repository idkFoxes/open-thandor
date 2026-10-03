"""One-time move (step 4c-3 of the readability plan): the variables of src/generated/image_data.c go to the
modules that use them.

Every definition goes to src/<area>/<module>/data.c of the directory whose .c files use the variable most
(a variable only referenced from another variable's initializer follows that variable); its extern declaration,
with the comments, #defines and enums above it, goes to include/thandor/<area>/<module>/data.h. Definitions keep
their original address order within a file. include/thandor/generated/image_data.h then only includes the
module headers and declares the imagecmp tables, which stay in src/generated/image_data.c until the self-test
goes away. CMakeLists.txt gets the new data.c files.
Run from the repository root: python tools/data/split_image_data.py"""
import os
import re
from collections import Counter

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SOURCE = os.path.join(REPO, 'src', 'generated', 'image_data.c')
HEADER = os.path.join(REPO, 'include', 'thandor', 'generated', 'image_data.h')
BANNER = '/*\n * Open Thandor\n * Project: https://github.com/idkFoxes/open-thandor/tree/main\n' \
         ' * File: https://github.com/idkFoxes/open-thandor/blob/main/%s\n */\n\n'


def read(path):
    with open(path, encoding='utf-8', newline='') as f:
        return f.read().replace('\r\n', '\n')


def write(path, text):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w', encoding='utf-8', newline='') as f:
        f.write(text.replace('\n', '\r\n'))


def code_only(line):
    """The line without string/char literals and comments, for brace counting."""
    line = re.sub(r'L?"(?:\\.|[^"\\])*"', '""', line)
    line = re.sub(r"'(?:\\.|[^'\\])*'", "''", line)
    line = re.sub(r'/\*.*?\*/', '', line)
    return line.split('//')[0]


# ---- source: prologue, definitions (with the comment lines directly above), tail (imagecmp tables)
src_lines = read(SOURCE).split('\n')
tail_at = next(i for i, l in enumerate(src_lines) if l.startswith('const ThandorImageBlock g_ThandorImageBlocks'))
prologue_end = next(i for i, l in enumerate(src_lines) if l.startswith('#pragma warning')) + 1
definitions = []          # (name, text)
pending = []
i = prologue_end
in_comment = False
while i < tail_at:
    line = src_lines[i]
    stripped = line.strip()
    if in_comment:
        # inside a comment block: it ends on the line holding */
        pending.append(line)
        in_comment = '*/' not in line
        i += 1
        continue
    if stripped.startswith('/*'):
        pending.append(line)
        in_comment = '*/' not in line
        i += 1
        continue
    if not stripped:
        i += 1  # blank lines between definitions; comments above stay with the next definition
        continue
    block = [line]
    depth = code_only(line).count('{') - code_only(line).count('}')
    while depth > 0 or not code_only(block[-1]).rstrip().endswith(';'):
        i += 1
        block.append(src_lines[i])
        depth += code_only(src_lines[i]).count('{') - code_only(src_lines[i]).count('}')
    i += 1
    name = re.search(r'\(\s*\*\s*(\w+)\s*\)\s*\(', block[0]) or re.search(r'\b(\w+)\s*(?:\[[^\]]*\])*\s*=', block[0])
    if not name:
        raise SystemExit('cannot name definition: ' + block[0])
    comments = [l for l in pending if l.strip()]
    definitions.append((name.group(1), '\n'.join(comments + block)))
    pending = []
names = [n for n, _ in definitions]
print('%d definitions' % len(definitions))

# ---- header: chunks that end with an extern declaration
hdr_lines = read(HEADER).split('\n')
body_start = next(i for i, l in enumerate(hdr_lines) if l.startswith('#pragma pack(push')) + 1
body_end = next(i for i, l in enumerate(hdr_lines) if l.startswith('#pragma pack(pop)'))
declarations = {}
chunk = []
for line in hdr_lines[body_start:body_end]:
    if re.match(r'^/\* original 0x[0-9A-Fa-f]+-0x[0-9A-Fa-f]+ \*/$', line.strip()):
        continue  # address region markers of the former blocks
    chunk.append(line)
    m = re.match(r'^extern .*?\(\s*\*\s*(\w+)\s*\)\s*\(.*\)\s*;', line) or re.match(r'^extern .*?\b(\w+)\s*(?:\[[^\]]*\])*\s*;', line)
    if m:
        while chunk and not chunk[0].strip():
            chunk.pop(0)
        declarations[m.group(1)] = '\n'.join(chunk)
        chunk = []
missing = [n for n in names if n not in declarations]
if missing:
    raise SystemExit('no declaration for: ' + ', '.join(missing[:10]))
leftover = '\n'.join(l for l in chunk if l.strip())
if leftover:
    raise SystemExit('header lines after the last extern: ' + leftover[:200])

# ---- owners: the directory whose .c files use a variable most
uses = {}
for base in ('src', 'include'):
    for dirpath, _, files in os.walk(os.path.join(REPO, base)):
        for f in files:
            path = os.path.join(dirpath, f)
            rel = os.path.relpath(path, REPO).replace('\\', '/')
            if not f.endswith('.c') or rel.startswith('src/generated/'):
                continue
            uses[rel] = Counter(re.findall(r'\b[A-Za-z_]\w*\b', read(path)))
owner = {}
for n in names:
    by_dir = Counter()
    for rel, counter in uses.items():
        if counter.get(n):
            by_dir[os.path.dirname(rel)] += counter[n]
    if by_dir:
        owner[n] = by_dir.most_common(1)[0][0]
texts = dict(definitions)
for _ in range(10):  # variables only referenced from initializers follow their referrer
    for n in names:
        if n in owner:
            continue
        pattern = re.compile(r'\b%s\b' % re.escape(n))
        referrers = [m for m in names if m != n and m in owner and pattern.search(texts[m])]
        if referrers:
            owner[n] = Counter(owner[m] for m in referrers).most_common(1)[0][0]
for n in names:
    owner.setdefault(n, 'src/platform/bootstrap')
print('%d modules' % len(set(owner.values())))

# ---- write the module files
modules = sorted(set(owner.values()))
for module in modules:
    sub = module[len('src/'):]
    members = [n for n in names if owner[n] == module]
    data_c = module + '/data.c'
    data_h = 'include/thandor/' + sub + '/data.h'
    guard = 'THANDOR_' + re.sub(r'\W', '_', sub).upper() + '_DATA_H'
    write(os.path.join(REPO, data_c),
          BANNER % data_c +
          '/* Data of the original image that this module uses (moved here from the generated image data in\n'
          '   step 4c); declared in <thandor/%s/data.h>. Original addresses in the comments. */\n\n'
          '#include <thandor/thandor.h>\n\n'
          '#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */\n\n' % sub +
          '\n\n'.join(texts[n] for n in members) + '\n')
    write(os.path.join(REPO, data_h),
          BANNER % data_h +
          '#ifndef %s\n#define %s\n\n'
          '#include <thandor/generated/types.h>\n#include <thandor/generated/ui_templates.h>\n\n' % (guard, guard) +
          '\n\n'.join(declarations[n] for n in members) + '\n\n#endif\n')

# ---- image_data.c keeps the imagecmp tables, image_data.h includes the module headers
prologue = '\n'.join(src_lines[:prologue_end])
prologue = re.sub(r'/\* Generated by .*?\*/', '/* The imagecmp tables (OPEN_THANDOR_SELFTEST=imagecmp): every variable of the original image\n'
                  '   with its original range, and every converted pointer. The variables themselves live in the\n'
                  '   modules (src/<area>/<module>/data.c). */', prologue, count=1, flags=re.S)
write(SOURCE, prologue + '\n\n' + '\n'.join(src_lines[tail_at:]))
header_head = '\n'.join(hdr_lines[:body_start - 1])
header_tail = '\n'.join(hdr_lines[body_end + 1:])
write(HEADER, header_head + '\n' + ''.join('#include <thandor/%s/data.h>\n' % m[len('src/'):] for m in modules) +
      header_tail)

# ---- CMakeLists.txt: the data files belong to the library next to their modules
cmake_path = os.path.join(REPO, 'CMakeLists.txt')
cmake = read(cmake_path)
library = re.search(r'add_library\(thandor_curated STATIC\n(.*?)\n\)', cmake, re.S)
entries = set(l.strip() for l in library.group(1).split('\n') if l.strip())
entries.update(m + '/data.c' for m in modules)
cmake = cmake.replace(library.group(0), 'add_library(thandor_curated STATIC\n' +
                      '\n'.join('    ' + e for e in sorted(entries)) + '\n)')
write(cmake_path, cmake)
print('done: %d data.c files' % len(modules))
