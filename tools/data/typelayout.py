"""Field layout of every struct in include/thandor/generated/types.h, as the compiler sees it.

Parses the struct definitions (Ghidra's export style: one field per line, nested types written as
`struct X`, `union X`, `enum X`, function pointers as `T (*name)(...)`), then compiles a program
that prints offsetof/sizeof of every field, so offsets are never guessed. Needs cl.exe in PATH.

Writes typelayout.json: {struct: {"size": n, "fields": [{"name", "offset", "size", "kind", "type",
"count"}]}} with kind one of int, pointer, struct, union, enum, float, array-of-<kind>, bitfield."""
import json
import os
import re
import shutil
import subprocess

import common

args = common.parse_arguments(__doc__)
text = open(os.path.join(common.REPO, 'include', 'thandor', 'generated', 'types.h'), encoding='utf-8').read()

typedefs = {}
for m in re.finditer(r'^typedef\s+(.+?)\s+(\w+)\s*;', text, re.M):
    typedefs[m.group(2)] = m.group(1).strip()

def resolve(type_text):
    """Follows typedefs to the underlying type text."""
    seen = set()
    t = type_text.strip()
    while t in typedefs and t not in seen:
        seen.add(t)
        t = typedefs[t].strip()
    return t

def classify(type_text):
    t = resolve(type_text)
    if '*' in t or '(' in t:
        return 'pointer'
    if t.startswith('struct '):
        name = t.split()[1]
        return 'struct' if name in structs_seen else 'opaque'
    if t.startswith('union '):
        return 'union'
    if t.startswith('enum '):
        return 'enum'
    if t in ('float', 'double', 'long double'):
        return 'float'
    return 'int'

structs = {}
structs_seen = set(m.group(1) for m in re.finditer(r'^struct (\w+) \{', text, re.M))
for m in re.finditer(r'^struct (\w+) \{\n(.*?)^\};', text, re.M | re.S):
    name, body = m.group(1), m.group(2)
    fields = []
    for line in body.split('\n'):
        line = re.sub(r'/\*.*?\*/', '', line.split('//')[0]).strip()
        if not line:
            continue
        fp = re.match(r'(.+?)\(\s*(?:__\w+\s+)?\*\s*(\w+)\s*\)\s*\((.*)\)\s*;$', line)
        if fp:
            fields.append({'name': fp.group(2), 'type': line[:-1], 'kind': 'pointer', 'count': 1})
            continue
        # array of function pointers: ret (*name[N])(args)
        fpa = re.match(r'(.+?)\(\s*(?:__\w+\s+)?\*\s*(\w+)\s*((?:\[\s*\w+\s*\])+)\s*\)\s*\((.*)\)\s*;$', line)
        if fpa:
            count = 1
            for d in re.findall(r'\[\s*(\w+)\s*\]', fpa.group(3)):
                count *= int(d, 0)
            fields.append({'name': fpa.group(2), 'type': 'void *', 'kind': 'array-of-pointer',
                           'count': count, 'dims': fpa.group(3)})
            continue
        bf = re.match(r'(.+?)\s+(\w+)\s*:\s*\d+\s*;$', line)
        if bf:
            fields.append({'name': bf.group(2), 'type': bf.group(1), 'kind': 'bitfield', 'count': 1})
            continue
        fm = re.match(r'(.+?)\s*(\**)\s*(\w+)\s*((?:\[\s*\w+\s*\])*)\s*;$', line)
        if not fm:
            fields.append({'name': None, 'type': line, 'kind': 'unparsed', 'count': 1})
            continue
        base, stars, fname, dims = fm.group(1), fm.group(2), fm.group(3), fm.group(4)
        ftype = (base + ' ' + stars).strip()
        count = 1
        for d in re.findall(r'\[\s*(\w+)\s*\]', dims):
            count *= int(d, 0)
        kind = 'pointer' if stars else classify(base)
        fields.append({'name': fname, 'type': ftype, 'kind': kind if not dims else 'array-of-' + kind,
                       'count': count, 'dims': dims})
    structs[name] = fields

# ---- offsets from the compiler
source = os.path.join(args.work, 'typelayout.c')
with open(source, 'w') as f:
    f.write('#include <stdio.h>\n#include <stddef.h>\n#include <thandor/thandor.h>\n\n'
            '#define FIELD(s, m) printf("F %s %s %u %u\\n", #s, #m, (unsigned)offsetof(struct s, m), '
            '(unsigned)sizeof(((struct s *)0)->m))\n\nint main(void)\n{\n')
    for name, fields in structs.items():
        f.write('    printf("S %s %%u\\n", (unsigned)sizeof(struct %s));\n' % (name, name))
        for field in fields:
            if field['name'] and field['kind'] not in ('bitfield', 'unparsed'):
                f.write('    FIELD(%s, %s);\n' % (name, field['name']))
    f.write('    return 0;\n}\n')
cl = shutil.which('cl')
if cl is None:
    raise SystemExit('%s written; run from a vcvars32 prompt to compile it' % source)
exe = os.path.join(args.work, 'typelayout.exe')
build = subprocess.run([cl, '/nologo', '/w', '/I' + os.path.join(common.REPO, 'include'), source, '/Fe' + exe,
                        '/Fo' + os.path.join(args.work, 'typelayout.obj')], capture_output=True, text=True)
if build.returncode != 0:
    print(build.stdout[-3000:])
    raise SystemExit('typelayout.c did not compile')
sizes = {}
offsets = {}
for line in subprocess.run([exe], capture_output=True, text=True, check=True).stdout.splitlines():
    parts = line.split()
    if parts[0] == 'S':
        sizes[parts[1]] = int(parts[2])
    else:
        offsets[(parts[1], parts[2])] = (int(parts[3]), int(parts[4]))
result = {}
unsupported = 0
for name, fields in structs.items():
    out = []
    for field in fields:
        if (name, field['name']) in offsets:
            field['offset'], field['size'] = offsets[(name, field['name'])]
        else:
            unsupported += 1
        out.append(field)
    result[name] = {'size': sizes.get(name), 'fields': out}
json.dump(result, open(os.path.join(args.work, 'typelayout.json'), 'w'), indent=1)
kinds = {}
for fields in structs.values():
    for field in fields:
        kinds[field['kind']] = kinds.get(field['kind'], 0) + 1
print('%d structs, fields by kind: %s; %d fields without compiler offset (bitfield/unparsed)' % (
    len(structs), ', '.join('%s=%d' % kv for kv in sorted(kinds.items())), unsupported))
