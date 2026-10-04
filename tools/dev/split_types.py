"""Split include/thandor/generated/{types,proc_types,ui_templates}.h into per-module type headers (step 5d).

One-shot migration tool. Run it on a tree that still has the generated headers:

    python tools/dev/split_types.py [--report] [--write]

--report prints where the items go, --write does the split.

What it does:
1. Parses the three headers into items: a struct/union (with its forward typedefs and layout checks), an
   anonymous enum with the typedef that follows it, a typedef, a #define, an #ifndef...#endif block. Comments and
   blank lines in front of an item belong to it; the #pragma pack(push)...#pragma pack(pop) region of an item is
   remembered. The text of an item is kept exactly.
2. Builds the dependency graph between items: a "full" dependency needs the complete definition (a member by
   value, an array, a scalar typedef name, an enumerator), a "decl" dependency only a forward declaration (a
   pointer or Ptr32<> member, a parameter of a function type).
3. Finds the users of each item: the modules (src/<area>/<module>, include/thandor/<area>/<module>[.h]) whose
   files name it (a header mention counts three times). An item no module names inherits the users of the items
   that name it; an item without users is deleted, with its layout checks in src/core/layout_checks.cpp.
4. Assigns each item a module type header include/thandor/<area>/<module>/types.h:
   - one user module: that module;
   - several: the user module whose functions are named like the item (WorldRuntimeContext -> the module with
     WorldRuntime_* functions), else the user module with the most uses;
   - include/thandor/core/types.h (the common header) for Bool8/Q12/UQ12/UQ8, what the core helper headers
     (contracts.h, x86_emulation.h) need, scalars used in 4+ areas and small records used in 4+ areas with no
     owning module or area;
   - parts go with their whole: an item that only items of one header embed goes to that header, an item only
     other items name goes with them;
   - whatever the common header needs complete is common too.
   The module type headers must not include each other in a cycle: they are ordered (a feedback-arc-set
   heuristic on the "needs complete" graph) and an item a lower header needs complete moves down into the lowest
   header that needs it.
5. Emits the type headers (the type headers they need complete, forward declarations for the rest, the items in
   their original order under a "Types" comment), replaces the generated includes in every other header by the
   type headers it names, adds the module's types.h to the module umbrella header (core/types.h to core.h) and
   deletes the generated headers (generated/imports.h stays).
"""
import collections
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
INC = os.path.join(ROOT, 'include')
SRC = os.path.join(ROOT, 'src')
GEN = 'thandor/generated/'
SOURCES = ['thandor/generated/types.h', 'thandor/generated/proc_types.h', 'thandor/generated/ui_templates.h']
COMMON = 'core'  # include/thandor/core/types.h
CORE_HDR = 'core/_helpers'  # contracts.h, x86_emulation.h: what they need is common
IDENT = re.compile(r'[A-Za-z_][A-Za-z0-9_]*')
KEYWORDS = set('''typedef struct union enum int unsigned signed char short long void const volatile
    float double static inline define ifndef endif ifdef if pragma pack push pop sizeof uint8_t uint16_t uint32_t
    int8_t int16_t int32_t uint64_t int64_t uintptr_t intptr_t size_t __stdcall __cdecl __fastcall Ptr32 UPtr32
    _MSC_VER defined'''.split())
# Common vocabulary named by the 5a review: one-byte booleans and fixed-point scalars, wherever they are used.
COMMON_NAMES = {'Bool8', 'Q12', 'UQ12', 'UQ8', '__fastcall', '_WCHAR_T_DEFINED'}


def rd(path):
    with open(path, encoding='utf-8', newline='') as f:
        return f.read().replace('\r\n', '\n')


def wr(path, text):
    """Writes with the line ends of a Windows checkout (git stores LF)."""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w', encoding='utf-8', newline='\r\n') as f:
        f.write(text)


def strip_comments(text):
    """Comments and string literals replaced by spaces (newlines kept), so that offsets line up."""
    out = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if text.startswith('//', i):
            j = text.find('\n', i)
            j = n if j < 0 else j
            out.append(' ' * (j - i))
            i = j
        elif text.startswith('/*', i):
            j = text.find('*/', i + 2)
            j = n if j < 0 else j + 2
            out.append(re.sub(r'[^\n]', ' ', text[i:j]))
            i = j
        elif c in '"\'':
            j = i + 1
            while j < n and text[j] != c:
                j += 2 if text[j] == '\\' else 1
            j += 1
            out.append(' ' * (j - i))
            i = j
        else:
            out.append(c)
            i += 1
    return ''.join(out)


# ---------------------------------------------------------------------------------------------------------------
# 1. Items


class Stmt:
    def __init__(self, text, code, kind, src, line):
        self.text, self.code, self.kind, self.src, self.line = text, code, kind, src, line


def statements(text, src):
    """Top-level statements of a header body: preprocessor lines and declarations ending in ';'. Leading comments
    and blank lines belong to the following statement; a comment after the ';' on the same line to this one."""
    clean = strip_comments(text)
    out = []
    n = len(text)
    pos = 0
    i = 0
    depth = 0
    start = None
    while i < n:
        if start is None and depth == 0:
            j = i
            while j < n and clean[j] in ' \t\n':
                j += 1
            if j >= n:
                break
            if clean[j] == '#':
                e = text.find('\n', j)
                e = n if e < 0 else e
                while text[e - 1] == '\\':
                    e = text.find('\n', e + 1)
                end = min(e + 1, n)
                out.append(Stmt(text[pos:end], clean[pos:end], 'pp', src, text.count('\n', 0, j) + 1))
                pos = i = end
                continue
            start = j
            i = j
        c = clean[i]
        if c in '({[':
            depth += 1
        elif c in ')}]':
            depth -= 1
        elif c == ';' and depth == 0:
            end = text.find('\n', i)
            end = n if end < 0 else end + 1
            rest = text[i + 1:end].strip()
            if rest and not rest.startswith('/'):
                raise SystemExit(f'{src}:{text.count(chr(10), 0, i) + 1}: code after ;')
            if rest.startswith('/*') and '*/' not in rest:
                end = text.find('\n', text.find('*/', i)) + 1
            out.append(Stmt(text[pos:end], clean[pos:end], 'decl', src, text.count('\n', 0, start) + 1))
            pos = i = end
            start = None
            continue
        i += 1
    tail = text[pos:]
    return out, tail


class Item:
    def __init__(self, stmts):
        self.stmts = stmts
        self.text = ''.join(s.text for s in stmts)
        self.code = ''.join(s.code for s in stmts)
        self.src = stmts[0].src
        self.line = stmts[0].line
        self.fwd = []         # hoisted forward-declaration statements
        self.names = set()    # names this item defines
        self.aggs = {}        # struct/union tag -> 'struct' | 'union' defined here
        self.kind = 'scalar'  # agg | alias | func | define | scalar | block
        self.alias_target = None
        self.pack = None      # (push line text, region id)
        self.refs = {}        # name -> 'full' | 'decl' | 'soft'
        self.users = collections.Counter()
        self.target = None
        self.order = 0

    def __repr__(self):
        return f'{self.src}:{self.line}:{sorted(self.names)[:2]}'


def body_of(path):
    text = rd(os.path.join(INC, path))
    lines = text.split('\n')
    # body: after the last top #include line, up to the closing #endif of the guard
    last_inc = max(i for i, l in enumerate(lines) if l.startswith('#include'))
    end = max(i for i, l in enumerate(lines) if l.startswith('#endif'))
    body = '\n'.join(lines[last_inc + 1:end]) + '\n'
    return body, last_inc + 1


def parse_items():
    items = []
    region = 0
    for path in SOURCES:
        body, base = body_of(path)
        stmts, tail = statements(body, path)
        for s in stmts:
            s.line += base
        if tail.strip():
            raise SystemExit(f'{path}: text after the last item')
        i = 0
        hoisting = True
        pack = None
        group = []
        while i < len(stmts):
            s = stmts[i]
            code = s.code.strip()
            if s.kind == 'pp':
                if code.startswith('#pragma pack(push'):
                    region += 1
                    # the comment in front of the pragma goes with the first item of the region
                    k = s.text.rfind('#pragma')
                    pack = (s.text[k:], region)
                    if s.text[:k].strip():
                        group.append(Stmt(s.text[:k], s.code[:k], 'lead', path, s.line))
                    i += 1
                    continue
                if code.startswith('#pragma pack(pop'):
                    k = s.text.rfind('#pragma')
                    if s.text[:k].strip():
                        raise SystemExit(f'{path}:{s.line}: comment before pack(pop)')
                    pack = None
                    i += 1
                    continue
                if code.startswith('#ifndef') or code.startswith('#ifdef') or code.startswith('#if '):
                    depth = 0
                    j = i
                    while True:
                        c2 = stmts[j].code.strip()
                        if re.match(r'#\s*if', c2):
                            depth += 1
                        elif c2.startswith('#endif'):
                            depth -= 1
                        if depth == 0:
                            break
                        j += 1
                    it = Item(group + stmts[i:j + 1])
                    group = []
                    it.kind = 'block'
                    it.pack = pack
                    items.append(it)
                    i = j + 1
                    hoisting = False
                    continue
                if code.startswith('#define'):
                    it = Item(group + [s])
                    group = []
                    it.kind = 'define'
                    it.pack = pack
                    items.append(it)
                    i += 1
                    continue
                if not code:  # pure comment / blank lines before something: keep with the next statement
                    group.append(s)
                    i += 1
                    continue
                raise SystemExit(f'{path}:{s.line}: unexpected directive {code[:40]}')
            m = re.match(r'typedef\s+(struct|union)\s+(\w+)\s+(\w+)\s*(,\s*\*\s*\w+\s*)?;$', re.sub(r'\s+', ' ', code))
            if m and m.group(2) == m.group(3):
                it = Item(group + [s])
                group = []
                it.kind = 'fwd'
                it.fwd_name = m.group(2)
                it.fwd_kw = m.group(1)
                it.hoisted = hoisting
                items.append(it)
                i += 1
                continue
            hoisting = False
            if code.startswith('THANDOR_STATIC_ASSERT'):
                # a layout check goes with the struct before it
                prev = items[-1]
                checked = re.search(r'offsetof\((\w+)', code).group(1)
                if not re.search(r'\b' + checked + r'\b', prev.code):
                    raise SystemExit(f'{path}:{s.line}: layout check apart from its struct')
                for g in group + [s]:
                    prev.stmts.append(g)
                    prev.text += g.text
                    prev.code += g.code
                group = []
                i += 1
                continue
            if re.match(r'enum\b[^{]*\{', code) and not re.match(r'enum\s+\w+\s*\{', code):
                # anonymous enum: the typedef after it names it
                nxt = stmts[i + 1] if i + 1 < len(stmts) else None
                lead = nxt.text[:len(nxt.text) - len(nxt.text.lstrip())] if nxt else ''
                if nxt and nxt.kind == 'decl' and nxt.code.strip().startswith('typedef') and lead.count('\n') <= 1 \
                        and not nxt.text.lstrip().startswith('/'):
                    it = Item(group + [s, nxt])
                    i += 2
                else:
                    it = Item(group + [s])
                    i += 1
                group = []
                it.pack = pack
                items.append(it)
                continue
            it = Item(group + [s])
            group = []
            it.pack = pack
            items.append(it)
            i += 1
        if group:
            raise SystemExit(f'{path}: comment after the last item')
    return items


def analyse_item(it):
    code = it.code
    for m in re.finditer(r'\b(struct|union)\s+(\w+)\s*\{', code):
        it.aggs[m.group(2)] = m.group(1)
        it.names.add(m.group(2))
    if it.kind == 'define':
        m = re.search(r'#\s*define\s+(\w+)', code)
        it.names.add(m.group(1))
        return
    if it.kind == 'block':
        for m in re.finditer(r'#\s*define\s+(\w+)', code):
            it.names.add(m.group(1))
        for m in re.finditer(r'typedef[^;]*?\b(\w+)\s*;', code):
            it.names.add(m.group(1))
        return
    if it.kind == 'fwd':
        return
    # enumerators
    for m in re.finditer(r'\benum\b[^{]*\{([^}]*)\}', code):
        for e in m.group(1).split(','):
            mm = re.match(r'\s*(\w+)', e)
            if mm:
                it.names.add(mm.group(1))
    if it.aggs:
        it.kind = 'agg'
    # typedef names: every top-level typedef statement of the item
    for st in it.stmts:
        c = re.sub(r'\s+', ' ', st.code).strip()
        if not c.startswith('typedef'):
            continue
        # strip a struct/union/enum body
        c2 = c
        while '{' in c2:
            a = c2.find('{')
            d = 0
            for k in range(a, len(c2)):
                if c2[k] == '{':
                    d += 1
                elif c2[k] == '}':
                    d -= 1
                    if d == 0:
                        break
            c2 = c2[:a] + c2[k + 1:]
        fm = re.search(r'\(\s*(?:__\w+\s+)?\*\s*(\w+)\s*\)\s*\(', c2)
        if fm:
            it.names.add(fm.group(1))
            if not it.aggs:
                it.kind = 'func'
            continue
        fm = re.match(r'typedef\s+(.*?)\b(\w+)\s*\(', c2)
        if fm and not re.search(r'\[', c2[:fm.end()]):
            it.names.add(fm.group(2))
            if not it.aggs:
                it.kind = 'func'
            continue
        for decl in c2[len('typedef'):].rstrip(';').split(','):
            ids = IDENT.findall(decl.split('[')[0])
            if ids:
                it.names.add(ids[-1])
        am = re.match(r'typedef (struct|union) (\w+) (\w+);$', c2)
        if am and not it.aggs:
            it.kind = 'alias'
            it.alias_target = am.group(2)


def resolve_refs(it, by_name):
    """name -> 'full' | 'decl' | 'soft' for the names of other items this item mentions."""
    code = it.code
    if it.kind == 'fwd':
        return
    for m in IDENT.finditer(code):
        w = m.group(0)
        if w in KEYWORDS or w in it.names or w not in by_name:
            continue
        other = by_name[w]
        if other is it:
            continue
        if it.kind in ('define', 'block'):
            kind = 'soft'
        elif other.kind != 'agg' or w not in other.aggs:
            kind = 'full'
        elif it.kind == 'func' or it.kind == 'alias':
            kind = 'decl'
        else:
            after = code[m.end():].lstrip()
            before = code[:m.start()]
            # inside Ptr32< ... > ?
            k = before.rfind('Ptr32<')
            inside = k >= 0 and before[k:].count('<') > before[k:].count('>')
            kind = 'decl' if after.startswith('*') or inside else 'full'
        if it.refs.get(w) != 'full':
            if not (it.refs.get(w) == 'decl' and kind == 'soft'):
                it.refs[w] = kind


# ---------------------------------------------------------------------------------------------------------------
# 3. Users


def module_of(rel):
    """Module key of a file (path relative to the repo root, '/' separated) or None."""
    p = rel.split('/')
    if p[0] == 'src':
        if len(p) >= 4:
            return p[1] + '/' + p[2]
        return None  # src/core/layout_checks.cpp
    if p[0] == 'include' and p[1] == 'thandor':
        if p[2] == 'generated':
            return 'platform/system' if p[3] == 'imports.h' else None
        if p[2] == 'core' and len(p) == 4 and p[3] in ('contracts.h', 'x86_emulation.h', 'ptr32.h', 'types.h'):
            return CORE_HDR
        if p[2] == 'platform' and p[3] == 'win32_constants.h':
            return 'platform/system'
        if len(p) >= 5:
            return p[2] + '/' + p[3]
        if len(p) == 4 and p[3].endswith('.h'):
            return p[2] + '/' + p[3][:-2]
    return None


UIROOT = re.compile(r'\n/\* The top-level UI node: follow parent links[^\n]*\n'
                    r'static __inline UiNodeBase \*Thandor_UiRoot\(.*?\n\}\n', re.S)


def project_text(rel, full):
    """A project file as the split sees it: contracts.h without the unused Thandor_UiRoot, the only code that
    made it need the UI node layout."""
    text = rd(full)
    if rel == 'include/thandor/core/contracts.h':
        text, n = UIROOT.subn('', text)
        assert n == 1
    return text


def project_files():
    for top in ('src', 'include'):
        for d, _, fs in os.walk(os.path.join(ROOT, top)):
            for f in fs:
                if f.endswith(('.cpp', '.h')):
                    full = os.path.join(d, f)
                    rel = os.path.relpath(full, ROOT).replace('\\', '/')
                    yield rel, full


# ---------------------------------------------------------------------------------------------------------------


def area(mod):
    return mod.split('/')[0]


def header_of(target):
    return f'thandor/{target}/types.h' if target != COMMON else 'thandor/core/types.h'


def build():
    items = parse_items()
    for k, it in enumerate(items):
        it.order = k
        analyse_item(it)
    by_name = {}
    for it in items:
        for n in it.names:
            if n in by_name and by_name[n] is not it:
                prev = by_name[n]
                if prev.kind == 'agg' and n in prev.aggs:
                    continue  # struct tag wins over a same-named typedef elsewhere
            by_name[n] = it
    # forward declarations join their struct
    fwds = [it for it in items if it.kind == 'fwd']
    real = [it for it in items if it.kind != 'fwd']
    for f in fwds:
        by_name.setdefault(f.fwd_name, f)
    for f in fwds:
        owner = by_name.get(f.fwd_name)
        if owner is None or owner is f:
            # a struct that stays incomplete: the forward declaration is the item
            f.kind = 'agg'
            f.aggs = {f.fwd_name: f.fwd_kw}
            f.names = {f.fwd_name}
            by_name[f.fwd_name] = f
            real.append(f)
            continue
        if owner.kind != 'agg':
            raise SystemExit(f'forward declaration of a non-struct: {f.fwd_name}')
        if f.hoisted:
            owner.fwd.append(f)
        else:
            owner.stmts = f.stmts + owner.stmts
            owner.text = f.text + owner.text
            owner.code = f.code + owner.code
    items = sorted(real, key=lambda x: x.order)
    for it in items:
        resolve_refs(it, by_name)

    # users
    for rel, full in project_files():
        if rel.startswith('include/' + GEN) and not rel.endswith('imports.h'):
            continue
        mod = module_of(rel)
        if mod is None:
            continue
        hdr = rel.endswith('.h')
        text = strip_comments(project_text(rel, full))
        if mod == CORE_HDR:
            # the core helper macros expand where they are used; they need nothing themselves
            text = re.sub(r'(?m)^#\s*define(.*\\\n)*.*$', '', text)
        words = collections.Counter(IDENT.findall(text))
        for w, c in words.items():
            it = by_name.get(w)
            if it is not None:
                it.users[mod] += c * (3 if hdr else 1)
    # items nobody names inherit the users of the items that reference them (fixpoint)
    referrers = collections.defaultdict(set)
    for it in items:
        for w in it.refs:
            referrers[id(by_name[w])].add(it)
    changed = True
    while changed:
        changed = False
        for it in items:
            if it.users:
                continue
            inh = collections.Counter()
            for r in referrers[id(it)]:
                for m in r.users:
                    inh[m] += 1
            if inh:
                it.users = inh
                it.inherited = True
                changed = True
    dead = [it for it in items if not it.users]
    live = [it for it in items if it.users]

    def full_deps(it):
        """Items that must be complete before `it` (aliases forward the need to their struct)."""
        out = set()
        for w, k in it.refs.items():
            if k != 'full':
                continue
            o = by_name[w]
            out.add(o)
            while o.kind == 'alias' and o.alias_target in by_name and by_name[o.alias_target].kind == 'agg':
                o = by_name[o.alias_target]
                out.add(o)
        return out

    for it in live:
        it.fdeps = full_deps(it)
        it.all_refs = {by_name[w] for w in it.refs}
    return items, live, dead, by_name


def function_prefixes():
    """module -> Counter of the prefixes (the part before the first '_') of the functions its headers declare."""
    out = collections.defaultdict(collections.Counter)
    pat = re.compile(r'\b([A-Z][A-Za-z0-9]*)_[A-Za-z0-9_]*\s*\(')
    for rel, full in project_files():
        if not rel.startswith('include/') or not rel.endswith('.h'):
            continue
        mod = module_of(rel)
        if mod is None or mod == CORE_HDR or rel.endswith('/types.h'):
            continue
        for m in pat.finditer(strip_comments(rd(full))):
            out[mod][m.group(1)] += 1
    return out


def camel_words(p):
    return len(re.findall(r'[A-Z][a-z0-9]*|[a-z0-9]+', p))


def main_names(it):
    """The type names of an item (not its enumerators or macros)."""
    names = [n for n in sorted(it.names) if not re.fullmatch(r'[A-Z0-9_]+', n) or n in it.aggs]
    return names or sorted(it.names)


def family_owner(it, prefixes, users, min_words=1):
    """The module whose functions are named like the item: the longest function prefix the item's name starts
    with (two CamelCase words or more, or one word for a user module), ties broken by the module path appearing in
    the name and by the number of functions. None when there is no such module."""
    best = None
    for name in main_names(it):
        for mod, pc in prefixes.items():
            for p, c in pc.items():
                if len(p) < 3 or not name.startswith(p):
                    continue
                if len(name) > len(p) and not (name[len(p)].isupper() or name[len(p)].isdigit()):
                    continue
                if mod not in users or camel_words(p) < min_words:
                    continue
                in_name = sum(1 for part in mod.split('/') if part.lower() in name.lower())
                key = (len(p), in_name, mod in users, c)
                if best is None or key > best[0]:
                    best = (key, mod)
    return best[1] if best else None


def dominant_area(mods):
    """The area with at least half of the uses, or None."""
    per = collections.Counter()
    for m, c in mods.items():
        per[area(m)] += c
    a, c = per.most_common(1)[0]
    return a if 2 * c >= sum(per.values()) else None


def small_closure(it):
    """True for a small record whose complete-type needs are a few small records and scalars: it can go to the
    common header without pulling a cluster of module types with it."""
    seen = set()
    stack = [it]
    while stack:
        x = stack.pop()
        if id(x) in seen:
            continue
        seen.add(id(x))
        if x.kind == 'agg' and x.code.count(';') > 8:
            return False
        if len(seen) > 12:
            return False
        stack.extend(x.fdeps)
    return True


def place(live, break_cycles=True):
    live_ids = {id(it) for it in live}
    for it in live:
        it.embedders = []
        it.referrers = []
    for it in live:
        for d in it.fdeps:
            if id(d) in live_ids:
                d.embedders.append(it)
    for it in live:
        for d in {it2 for it2 in it.all_refs if id(it2) in live_ids}:
            d.referrers.append(it)
    prefixes = function_prefixes()
    # 1. by direct use: the only user module; else the user module whose functions are named like the item (the
    #    module that works on it), else the one that uses it most
    for it in live:
        mods = it.users
        if getattr(it, 'inherited', False):
            it.target = None
        elif CORE_HDR in mods or it.names & COMMON_NAMES:
            it.target = COMMON
        elif it.kind in ('scalar', 'block') and len({area(m) for m in mods}) >= 4:
            it.target = COMMON
        elif it.kind == 'agg' and len({area(m) for m in mods}) >= 4 and small_closure(it) \
                and not family_owner(it, prefixes, mods, min_words=2) and not dominant_area(mods):
            it.target = COMMON  # small records of the common vocabulary (vectors, angles, pixel lanes)
        else:
            fam = family_owner(it, prefixes, mods) if len(mods) > 1 else None
            if fam:
                it.target = fam
                it.family = True
            else:
                it.target = max(sorted(mods), key=lambda m: mods[m])
    # 2. parts stay with their whole: an item embedded only in items of one header goes there, and an item only
    #    other items name goes with them. Embedders come later in the source order, so walk backwards.
    for rnd in range(3):
        for it in sorted(live, key=lambda x: -x.order):
            if getattr(it, 'inherited', False):
                pool = it.embedders or it.referrers
                ts = collections.Counter(r.target for r in pool if r.target)
                if ts:
                    it.target = max(sorted(ts), key=lambda t: ts[t])
            elif it.embedders and it.target != COMMON and not getattr(it, 'family', False):
                ts = {r.target for r in it.embedders}
                if len({area(t) for t in ts if t}) >= 4 and small_closure(it):
                    it.target = COMMON  # a small record embedded all over the program
                elif len(ts) == 1 and None not in ts:
                    t = next(iter(ts))
                    it.target = t
    for it in live:
        if it.target is None:
            raise SystemExit(f'no target: {it}')
    common_closure(live)
    if break_cycles:
        order_and_break(live)


def common_closure(live):
    """The common header is the bottom: whatever it needs complete is common too."""
    stack = [it for it in live if it.target == COMMON]
    while stack:
        it = stack.pop()
        for d in it.fdeps:
            if d.target != COMMON:
                d.target = COMMON
                d.moved = 'common'
                stack.append(d)


def module_graph(live):
    w = collections.defaultdict(set)  # (a, b) -> items of b that a needs complete
    for it in live:
        for d in it.fdeps:
            if d.target != it.target:
                w[(it.target, d.target)].add(d)
    return w


def order_and_break(live):
    """Order the module type headers (Eades-Lin-Smyth heuristic for a small feedback arc set, edges weighted by
    the number of items behind them) and move every item a lower header needs complete from a higher header
    down into the lowest header that needs it, together with what it needs complete from its old header."""
    w = module_graph(live)
    nodes = {it.target for it in live}
    out_w = collections.defaultdict(collections.Counter)
    for (a, b), ds in w.items():
        out_w[a][b] = len(ds)
    rest = set(nodes)
    top, bottom = [], []   # top: needs most, bottom: needed most

    def wdeg(n, direction):
        if direction == 'out':
            return sum(c for b, c in out_w[n].items() if b in rest and b != n)
        return sum(out_w[a][n] for a in rest if a != n)
    while rest:
        changed = True
        while changed:
            changed = False
            for n in sorted(rest):
                if wdeg(n, 'out') == 0:      # needs nothing: bottom
                    bottom.append(n)
                    rest.discard(n)
                    changed = True
                elif wdeg(n, 'in') == 0:     # needed by nobody: top
                    top.append(n)
                    rest.discard(n)
                    changed = True
        if rest:
            n = max(sorted(rest), key=lambda x: wdeg(x, 'in') - wdeg(x, 'out'))
            bottom.append(n)  # needed most: low
            rest.discard(n)
    order = bottom + list(reversed(top))    # index 0 = lowest
    if COMMON in order:
        order.remove(COMMON)
    order.insert(0, COMMON)
    rank = {n: i for i, n in enumerate(order)}
    for rnd in range(100):
        moved = 0
        by_needer = collections.defaultdict(set)
        for it in live:
            for d in it.fdeps:
                if rank[d.target] > rank[it.target]:
                    by_needer[d].add(it.target)
        if not by_needer:
            break
        for d in sorted(by_needer, key=lambda x: x.order):
            low = min(by_needer[d], key=lambda t: rank[t])
            if rank[low] < rank[d.target]:
                d.moved = (d.target, low)
                d.target = low
                moved += 1
        if not moved:
            break
    else:
        raise SystemExit('cycles left')
    w = module_graph(live)
    for (a, b) in w:
        if rank[b] > rank[a]:
            raise SystemExit(f'back edge left {a} -> {b}')
    return order


def main():
    report = '--report' in sys.argv
    write = '--write' in sys.argv
    items, live, dead, by_name = build()
    place(live)
    if report:
        do_report(live, dead, by_name)
    if write:
        emit(live, dead, by_name)


def do_report(live, dead, by_name):
    per = collections.Counter(it.target for it in live)
    per_area = collections.Counter(area(it.target) if it.target != COMMON else 'common' for it in live)
    print('items:', len(live) + len(dead), 'live:', len(live), 'dead:', len(dead))
    print('per area:', dict(sorted(per_area.items())))
    for t, c in sorted(per.items()):
        print(f'  {t}: {c}')
    print('common:')
    for it in live:
        if it.target == COMMON:
            print('   ', it.kind, ' '.join(sorted(it.names))[:150])
    print('moved by cycle breaking:')
    for it in live:
        if hasattr(it, 'moved'):
            print('   ', ' '.join(sorted(it.names))[:80], it.moved)
    print('dead:')
    for it in dead:
        print('   ', it.kind, ' '.join(sorted(it.names))[:150])


# ---------------------------------------------------------------------------------------------------------------
# 5. Emit

BANNER = '''/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/{path}
 * Reverse engineering by idkFoxes 2026
 */
'''


def guard(path):
    return re.sub(r'[^A-Za-z0-9]', '_', path).upper()


def emit(live, dead, by_name):
    targets = sorted({it.target for it in live})
    header = {t: header_of(t) for t in targets}
    defined_in = {}
    for it in live:
        for n in it.names:
            defined_in[n] = it.target

    # 5a. the type headers
    for t in targets:
        mine = sorted((it for it in live if it.target == t), key=lambda x: x.order)
        incs = set()
        for it in mine:
            for d in it.fdeps:
                if d.target != t:
                    incs.add(header[d.target])
            for w, k in it.refs.items():
                if k == 'full' and by_name[w].target != t:
                    incs.add(header[by_name[w].target])
        own_fwd = [f for it in mine for f in it.fwd]
        own_fwd_names = {f.fwd_name for f in own_fwd}
        fwd = {}
        for it in mine:
            for w, k in it.refs.items():
                o = by_name[w]
                if k != 'decl' or w in own_fwd_names:
                    continue
                if o.target != t and header[o.target] in incs:
                    continue
                fwd[w] = o.aggs[w]
        lines = [BANNER.format(path=header[t]), f'#ifndef {guard(header[t])}', f'#define {guard(header[t])}', '']
        lines.append('#include <stdint.h>')
        if any('offsetof' in it.code for it in mine):
            lines.append('#include <stddef.h> /* offsetof */')
        if any('THANDOR_STATIC_ASSERT' in it.code for it in mine):
            lines.append('#include <thandor/core/contracts.h> /* THANDOR_STATIC_ASSERT */')
        lines.append('#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */')
        for h in sorted(incs):
            lines.append(f'#include <{h}>')
        lines.append('')
        body = ['/* Types (split from generated/types.h by tools/dev/split_types.py). */\n\n']
        if own_fwd or fwd:
            for f in own_fwd:
                body.append(f.text)
            for w in sorted(fwd):
                body.append(f'typedef {fwd[w]} {w} {w};\n')
            body.append('\n')
        region = None
        for it in mine:
            r = it.pack[1] if it.pack else None
            if r != region:
                if region is not None:
                    body.append('#pragma pack(pop)\n')
                if r is not None:
                    body.append(it.pack[0])
                region = r
            body.append(it.text)
        if region is not None:
            body.append('#pragma pack(pop)\n')
        text = '\n'.join(lines) + '\n' + ''.join(body)
        text = text.rstrip('\n') + '\n\n' + f'#endif /* {guard(header[t])} */\n'
        wr(os.path.join(INC, header[t]), text)

    # 5b. the other headers (and the .cpp files outside thandor.h): the type headers they name replace the
    #     generated includes
    pat = re.compile(r'^#include <thandor/generated/(types|proc_types|ui_templates)\.h>[^\n]*\n', re.M)
    for rel, full in project_files():
        if rel.startswith('include/') and rel[len('include/'):] in header.values():
            continue
        if rel.startswith('include/' + GEN) and not rel.endswith('imports.h'):
            continue
        orig = rd(full)
        text = project_text(rel, full)
        m = pat.search(text)
        if rel.endswith('.cpp') and not m:
            continue
        own_defs = set()
        if rel.endswith('.h'):
            # names a header defines itself (contracts.h has its own Q12) do not count
            own_defs = set(re.findall(r'typedef[^;{]*\b(\w+)\s*;', strip_comments(text)))
        # a macro needs nothing where it is defined, only where it is used
        code = re.sub(r'(?m)^#\s*define(.*\\\n)*.*$', '', strip_comments(pat.sub('', text)))
        words = set(IDENT.findall(code)) - own_defs
        need = sorted({header[defined_in[w]] for w in words if w in defined_in})
        if not m and not need:
            if text != orig:
                wr(full, text)
            continue
        have = set(re.findall(r'^#include <([^>]+)>', text, re.M))
        new_inc = ''.join(f'#include <{h}>\n' for h in need if h not in have)
        if m:
            text = text[:m.start()] + new_inc + pat.sub('', text[m.start():])
        else:
            k = text.find('#include')
            if k >= 0:
                text = text[:k] + new_inc + text[k:]
            else:
                g = re.search(r'^#define THANDOR_\w+_H\n', text, re.M)
                if not g:
                    raise SystemExit(f'{rel}: no place for the type headers')
                text = text[:g.end()] + '\n' + new_inc + text[g.end():]
        if text != orig:
            wr(full, text)

    # 5c. module umbrella headers include their type header
    for t in targets:
        umb = 'thandor/core.h' if t == COMMON else f'thandor/{t}.h'
        if not os.path.exists(os.path.join(INC, umb)):
            umb = f'thandor/{area(t)}.h'
        p = os.path.join(INC, umb)
        text = rd(p)
        k = text.find('#include')
        text = text[:k] + f'#include <{header[t]}>\n' + text[k:]
        wr(p, text)

    # 5d. the layout checks of deleted types go with them
    dead_names = {n for it in dead for n in it.names}
    p = os.path.join(SRC, 'core', 'layout_checks.cpp')
    text = rd(p)
    out = []
    for chunk in re.split(r'(?<=;\n)', text):
        if chunk.lstrip().startswith('static_assert') and set(IDENT.findall(chunk)) & dead_names:
            continue
        out.append(chunk)
    wr(p, ''.join(out))

    # 5e. the generated headers go
    for s in SOURCES:
        os.remove(os.path.join(INC, s))
    os.remove(os.path.join(INC, 'thandor', 'generated.h'))
    p = os.path.join(INC, 'thandor', 'thandor.h')
    wr(p, rd(p).replace('#include <thandor/generated.h>\n', ''))
    print('deleted (unused):')
    for it in dead:
        print('   ', ', '.join(main_names(it)))


if __name__ == '__main__':
    main()
