"""One-time conversion (step 4c of the readability plan): the generated image objects become ordinary C
variables, and the address macros of globals.h / recovered.h (`#define g_X (*(T *)THANDOR_IMAGE(0x...))`)
go away.

  - An object that is exactly one global (a single member whose type matches the global's declared type)
    becomes `T g_X[...] = ...;` in src/generated/image_data.c and `extern T g_X[...];` in
    include/thandor/generated/image_data.h; its macro line is removed.
  - Every other global (a member of a group the code uses as a whole, a field inside another object, a
    different declared type) keeps a macro, now on the variable that holds it instead of an original
    address: `#define g_X (*(T *)&g_ImageObject_<start>.at_g_X)`.
  - The THANDOR_IMAGE_0x<address> table disappears.

After this run gen_image_data.py can no longer regenerate the data (it reads the types from the macros);
the generated files become the source.
Run from the repository root: python tools/data/dissolve_image_data.py"""
import re
import sys

HEADER_PATH = 'include/thandor/generated/image_data.h'
SOURCE_PATH = 'src/generated/image_data.c'
MACRO_PATHS = ['include/thandor/generated/globals.h', 'include/thandor/data/recovered.h']


def read(path):
    return open(path, encoding='utf-8').read()


def write(path, text):
    open(path, 'w', encoding='utf-8', newline='\n').write(text)


norm = lambda t: re.sub(r'\s+', '', t)

header = read(HEADER_PATH)
source = read(SOURCE_PATH)

# ---- address table: THANDOR_IMAGE_0x<addr> -> expression
image_expr = {}
for m in re.finditer(r'^#define THANDOR_IMAGE_(0x[0-9a-f]{8}) \((.*)\)\n', header, re.M):
    image_expr[m.group(1)] = m.group(2)

# ---- object structs in the header
struct_re = re.compile(r'typedef struct (ImageObject_([0-9A-F]{8})) \{\n(.*?)\n\} \1;\nextern \1 (g_ImageObject_\2);\n',
                       re.S)
member_re = re.compile(r'^    (\S.*?)\s+(at_\w+)((?:\[[^\]]*\])*);(?: (/\*.*\*/))?$')
objects = {}   # start -> dict(type, var, members[list of (type, name, dims, comment)], text)
for m in struct_re.finditer(header):
    lines = m.group(3).split('\n')
    members = []
    plain = True
    for line in lines:
        mm = member_re.match(line)
        if mm is None:
            plain = False
            continue
        members.append((mm.group(1), mm.group(2), mm.group(3), mm.group(4) or ''))
    objects[m.group(2)] = {'type': m.group(1), 'var': m.group(4), 'members': members, 'plain': plain,
                           'text': m.group(0)}

# ---- object definitions in the source
def_re = re.compile(r'\n__declspec\(align\((\d+)\)\) (ImageObject_([0-9A-F]{8})) (g_ImageObject_\3) = \{\n(.*?)\n\};\n',
                    re.S)
definitions = {m.group(3): m for m in def_re.finditer(source)}

# ---- the macros
macro_re = re.compile(r'^#define (\w+) \(\*\((.*)\)THANDOR_IMAGE\((0x[0-9a-fA-F]{8})\)\)[ \t]*(/\*.*\*/)?[ \t]*$', re.M)
convert = {}   # object start -> (global name, member)
macro_of_start = {}
for path in MACRO_PATHS:
    for m in macro_re.finditer(read(path)):
        name, ptype, addr = m.group(1), m.group(2).strip(), m.group(3).lower()
        start = addr[2:].upper()
        obj = objects.get(start)
        if obj is None or len(obj['members']) != 1 or not obj['plain']:
            continue
        mtype, member, dims, _ = obj['members'][0]
        expected = '%s (*)%s' % (mtype, dims) if dims else '%s *' % mtype
        if norm(expected) != norm(ptype) or start not in definitions:
            continue
        if start in convert:   # two globals on one object: keep the first, the second stays a macro
            continue
        convert[start] = (name, member)

# ---- rewrite the header: converted structs -> extern declarations
def declaration(mtype, name, dims):
    space = '' if mtype.endswith('*') else ' '
    return '%s%s%s%s' % (mtype, space, name, dims)

for start, (name, member) in convert.items():
    obj = objects[start]
    mtype, _, dims, comment = obj['members'][0]
    header = header.replace(obj['text'], 'extern %s;%s\n' % (declaration(mtype, name, dims),
                                                             (' ' + comment) if comment else ''))
# the address table goes away
header = re.sub(r'\n/\* Original address -> generated storage\. \*/\n(?:#define THANDOR_IMAGE_0x[^\n]*\n)+', '\n', header)

# ---- rewrite the source: converted definitions
def single_initializer(body):
    """The initializer expression of a one-member object body and its comment."""
    lines = body.split('\n')
    comment = ''
    if lines and re.match(r'^    /\* [0-9A-F]{8} .*\*/$', lines[0]):
        comment = lines[0].strip()
        lines = lines[1:]
    text = '\n'.join(lines)
    m = re.match(r'^(.*), (/\* [0-9A-F]{8} .*\*/)$', text, re.S)
    if m:
        text, comment = m.group(1), m.group(2)
    else:
        text = text.rstrip()
        assert text.endswith(','), text[-80:]
        text = text[:-1]
    return text.strip(), comment

for start, (name, member) in convert.items():
    m = definitions[start]
    mtype, _, dims, _ = objects[start]['members'][0]
    init, comment = single_initializer(m.group(5))
    init = init.replace('\n    ', '\n')
    replacement = '\n%s__declspec(align(%s)) %s = %s;\n' % (comment + '\n' if comment else '', m.group(1),
                                                            declaration(mtype, name, dims), init)
    source = source.replace(m.group(0), replacement)

# ---- references to converted objects (initializers, the block table, the remaining macros)
renames = {}
for start, (name, member) in convert.items():
    renames['g_ImageObject_%s.%s' % (start, member)] = name
    renames['g_ImageObject_%s' % start] = name
ref_re = re.compile(r'\bg_ImageObject_([0-9A-F]{8})(\.at_\w+)?\b')

def rename(m):
    whole = m.group(0)
    if whole in renames:
        return renames[whole]
    if m.group(2) is None and ('g_ImageObject_%s' % m.group(1)) in renames:
        return renames['g_ImageObject_%s' % m.group(1)]
    return whole

header = ref_re.sub(rename, header)
source = ref_re.sub(rename, source)
for k in list(image_expr):
    image_expr[k] = ref_re.sub(rename, image_expr[k])

# ---- the macro headers: drop converted macros, point the others at their variable
converted_names = set(n for n, _ in convert.values())
for path in MACRO_PATHS:
    text = read(path)

    def macro(m):
        name, ptype, addr = m.group(1), m.group(2).strip(), m.group(3).lower()
        if name in converted_names:
            return '\x00'
        expr = image_expr.get(addr)
        if expr is None:
            sys.exit('no storage for %s at %s' % (name, addr))
        comment = (' ' + m.group(4)) if m.group(4) else ''
        plain = re.match(r'^\(uintptr_t\)(&[\w.]+)$', expr)
        offset = re.match(r'^\(uintptr_t\)&([\w.]+) \+ (0x[0-9A-Fa-f]+)$', expr)
        if plain:
            expr = plain.group(1)
        elif offset:
            expr = '((uint8_t *)&%s + %s)' % (offset.group(1), offset.group(2))
        return '#define %s (*(%s)%s)%s' % (name, ptype, expr, comment)

    text = macro_re.sub(macro, text)
    text = re.sub(r'^\x00\n', '', text, flags=re.M)
    assert 'THANDOR_IMAGE(' not in text, path
    write(path, text)

write(HEADER_PATH, header)
write(SOURCE_PATH, source)
print('%d objects became plain variables; %d objects stay structs' % (len(convert), len(objects) - len(convert)))
