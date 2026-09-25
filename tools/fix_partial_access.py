"""Rewrite Ghidra partial-access syntax `base._off_size_` into C.

    x._2_2_          -> THANDOR_PART(word, x, 2)          (lvalue, sizes 1/2/4/8)
    x._0_6_          -> THANDOR_READ_PART(x, 0, 6)        (odd sizes, reads)
    x._0_6_ = v;     -> THANDOR_WRITE_PART(x, 0, 6, v);   (odd sizes, plain assignment)

Helpers live in include/thandor/core/ghidra.h. Bases that are not lvalues (function
calls, arithmetic in parentheses) are reported and left unchanged.

Usage: python tools/fix_partial_access.py [files...]   (default: every src/**/*.c)
"""
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
PIECE = re.compile(r"\._(\d+)_(\d+)_\b")
TYPES = {1: "byte", 2: "word", 4: "dword", 8: "qword"}


def match_back(text, i):
    """text[i] is ')' or ']'; return index of the matching opener."""
    close = text[i]
    opener = "(" if close == ")" else "["
    depth = 0
    while i >= 0:
        if text[i] == close:
            depth += 1
        elif text[i] == opener:
            depth -= 1
            if depth == 0:
                return i
        i -= 1
    raise ValueError("unbalanced")


def base_start(text, dot):
    """Start of the postfix expression that ends right before `dot`; None if not an lvalue."""
    i = dot
    while True:
        j = i - 1
        while j >= 0 and text[j] in " \t":
            j -= 1
        if j < 0:
            return None
        c = text[j]
        if c == "]":
            i = match_back(text, j)
            continue  # array base follows
        if c == ")":
            o = match_back(text, j)
            k = o - 1
            while k >= 0 and text[k] in " \t":
                k -= 1
            if k >= 0 and (text[k].isalnum() or text[k] == "_"):
                return None  # function call result: not addressable
            inner = text[o + 1:j]
            if re.search(r"[-+*/%<>=!&|^?,]", re.sub(r"->", "", inner)) and not re.fullmatch(r"\s*\*\s*[\w\s\.\->\[\]()]+", inner):
                return None  # arithmetic in parentheses: not addressable
            start = o
        elif c.isalnum() or c == "_":
            k = j
            while k >= 0 and (text[k].isalnum() or text[k] == "_"):
                k -= 1
            start = k + 1
        else:
            return None
        # continue through "." / "->" member chains
        k = start - 1
        while k >= 0 and text[k] in " \t":
            k -= 1
        if k >= 0 and text[k] == ".":
            i = k
            continue
        if k >= 1 and text[k - 1:k + 1] == "->":
            i = k - 1
            continue
        return start


def rewrite(text):
    out, pos, done, left = [], 0, 0, 0
    for m in PIECE.finditer(text):
        if m.start() < pos:
            continue
        s = base_start(text, m.start())
        if s is None or s < pos:
            left += 1
            continue
        off, size = int(m.group(1)), int(m.group(2))
        base = text[s:m.start()]
        end = m.end()
        if size in TYPES:
            new = f"THANDOR_PART({TYPES[size]}, {base}, {off})"
        else:
            a = re.match(r"\s*=(?!=)([^;]*);", text[end:])
            line_start = text.rfind("\n", 0, s) + 1
            if a and text[line_start:s].strip() == "":
                new = f"THANDOR_WRITE_PART({base}, {off}, {size}, {a.group(1).strip()});"
                end += a.end()
            else:
                new = f"THANDOR_READ_PART({base}, {off}, {size})"
        out.append(text[pos:s])
        out.append(new)
        pos = end
        done += 1
    out.append(text[pos:])
    return "".join(out), done, left


def main(files):
    paths = [pathlib.Path(f) for f in files] or sorted((ROOT / "src").rglob("*.c"))
    total = remaining = 0
    for p in paths:
        text = p.read_text(encoding="utf-8")
        # repeat: nested pieces like a._0_4_._2_2_ resolve inside-out
        for _ in range(4):
            text, done, left = rewrite(text)
            total += done
            if not done:
                break
        remaining += left
        p.write_text(text, encoding="utf-8", newline="\n")
    print(f"rewrote {total} partial accesses, {remaining} left (non-lvalue base)")


if __name__ == "__main__":
    main(sys.argv[1:])
