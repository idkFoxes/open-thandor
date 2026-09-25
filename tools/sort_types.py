"""Reorder a Ghidra "Export C header" types file into dependency order.

Ghidra emits type declarations in an order unrelated to their dependencies,
so a C compiler sees many types before they are declared. This script:

  1. keeps the preamble (everything before the first type declaration),
  2. hoists every `typedef struct|union X X, *PX;` forward declaration,
  3. topologically sorts all remaining top-level declarations, keeping the
     original order wherever dependencies allow (stable Kahn sort).

A struct/union used only through a pointer does not create an edge, because
the hoisted forward typedef already satisfies it.

Usage: python tools/sort_types.py include/thandor/generated/types.h
"""
import heapq
import re
import sys

IDENT = re.compile(r"[A-Za-z_]\w*")
FORWARD = re.compile(r"^typedef\s+(struct|union)\s+(\w+)\s+(\w+)\s*,\s*\*\s*(\w+)\s*;\s*$")


def strip_comments(text):
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    text = re.sub(r"//[^\n]*", " ", text)
    return text


def split_top_level(text):
    """Split into top-level chunks: each chunk ends at a ';' at brace depth 0
    or is a run of preprocessor lines. Comments/blank lines stick to the next chunk."""
    chunks, cur, depth = [], [], 0
    lines = text.splitlines(keepends=True)
    i = 0
    while i < len(lines):
        line = lines[i]
        code = strip_comments(line).strip()
        if depth == 0 and code.startswith("#"):
            # preprocessor line (with continuations) is its own chunk
            block = [line]
            while block[-1].rstrip().endswith("\\") and i + 1 < len(lines):
                i += 1
                block.append(lines[i])
            chunks.append(("pp", "".join(cur + block)))
            cur = []
            i += 1
            continue
        cur.append(line)
        depth += code.count("{") - code.count("}")
        if depth == 0 and code.endswith(";"):
            chunks.append(("decl", "".join(cur)))
            cur = []
        i += 1
    if cur:
        chunks.append(("tail", "".join(cur)))
    return chunks


def defined_names(code):
    """Names a declaration defines, and whether it is a struct/union body."""
    c = " ".join(code.split())
    m = re.match(r"^(struct|union)\s+(\w+)\s*\{", c)
    if m:
        return {m.group(2)}, True
    if c.startswith("typedef"):
        # typedef enum {..} Name; / typedef <type> Name; / typedef ret (*Name)(..); / typedef ret Name(..);
        body = re.sub(r"\{.*\}", " ", c)
        names = set()
        m = re.search(r"\(\s*[\w\s]*\*\s*(\w+)\s*\)\s*\(", body)
        if m:
            names.add(m.group(1))
        else:
            m = re.search(r"(\w+)\s*\(", body)
            if m and "(" in body:
                names.add(m.group(1))
            else:
                m = re.search(r"(\w+)\s*(\[[^\]]*\]\s*)*;\s*$", body)
                if m:
                    names.add(m.group(1))
        # enum constants
        for e in re.findall(r"\{(.*)\}", c):
            for part in e.split(","):
                n = IDENT.match(part.strip())
                if n:
                    names.add(n.group(0))
        return names, False
    return set(), False


def main(path):
    with open(path, encoding="utf-8", newline="") as f:
        text = f.read()
    chunks = split_top_level(text)

    # preamble: everything up to the first decl
    first = next(i for i, (k, _) in enumerate(chunks) if k == "decl")
    # the guard's #endif at the end must stay last
    last_pp = max(i for i, (k, _) in enumerate(chunks) if k == "pp")
    preamble = [t for _, t in chunks[:first]]
    body = chunks[first:last_pp]
    epilogue = [t for _, t in chunks[last_pp:]]

    forwards, rest = [], []
    for kind, t in body:
        if kind == "decl" and FORWARD.match(strip_comments(t).strip()):
            forwards.append(t)
        else:
            rest.append((kind, t))

    tagged = set()
    for t in forwards:
        m = FORWARD.match(strip_comments(t).strip())
        tagged.update({m.group(2), m.group(3), m.group(4)})

    owner = {}
    info = []
    for idx, (kind, t) in enumerate(rest):
        code = strip_comments(t)
        names, is_body = defined_names(code) if kind == "decl" else (set(), False)
        for n in names:
            owner.setdefault(n, idx)
        info.append((code, names, is_body))

    deps = [set() for _ in rest]
    for idx, (code, names, _) in enumerate(info):
        toks = [(m.group(0), m.end()) for m in IDENT.finditer(code)]
        for name, end in toks:
            if name in names or name not in owner:
                continue
            tgt = owner[name]
            if tgt == idx:
                continue
            # pointer use of a forward-declared struct/union needs no ordering
            after = code[end:end + 40].lstrip()
            after = re.sub(r"^(const|volatile)\b\s*", "", after)
            if name in tagged and info[tgt][2] and after.startswith("*"):
                continue
            deps[idx].add(tgt)

    # stable Kahn sort; cycles are broken by emitting the lowest remaining index
    indeg = [len(d) for d in deps]
    users = [[] for _ in rest]
    for i, d in enumerate(deps):
        for j in d:
            users[j].append(i)
    heap = [i for i, n in enumerate(indeg) if n == 0]
    heapq.heapify(heap)
    done, order = [False] * len(rest), []
    while len(order) < len(rest):
        if not heap:
            i = min(i for i in range(len(rest)) if not done[i])
            print(f"warning: dependency cycle, forcing chunk {i}: "
                  f"{sorted(info[i][1])[:3]}", file=sys.stderr)
            heapq.heappush(heap, i)
            indeg[i] = 0
        i = heapq.heappop(heap)
        if done[i]:
            continue
        done[i] = True
        order.append(i)
        for u in users[i]:
            indeg[u] -= 1
            if indeg[u] == 0 and not done[u]:
                heapq.heappush(heap, u)

    out = "".join(preamble)
    out += "\n/* Forward declarations (hoisted by tools/sort_types.py). */\n"
    out += "".join(t.lstrip("\n") for t in forwards)
    out += "".join(rest[i][1] for i in order)
    out += "".join(epilogue)
    with open(path, "w", encoding="utf-8", newline="") as f:
        f.write(out)
    moved = sum(1 for pos, i in enumerate(order) if pos != i)
    print(f"{len(forwards)} forward typedefs hoisted, {len(rest)} decls, {moved} moved")


if __name__ == "__main__":
    main(sys.argv[1])
