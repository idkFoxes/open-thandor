#!/usr/bin/env python3
"""Rewrite `*(T *)expr` loads/stores of 64/32/16-bit values to the W0.3 helpers (step 13 X1/X2).

    *(MmxPackedValue64 *)p          ->  Thandor_LoadU64(p)
    *(MmxPackedValue64 *)p = v;     ->  Thandor_StoreU64(p, v);

The operand is the unary-expression the cast applies to: an identifier with postfix
`->m`, `.m`, `[i]`, `(args)`, or a parenthesised expression. Anything else (compound
assignment, increment, address-of, a cast that is not dereferenced, a store that is not
a whole statement) is left unchanged and listed, for manual review.

Usage: mmx_casts_to_helpers.py [--type MmxPackedValue64=U64 ...] [--dry-run] FILE...
"""
import argparse
import re
import sys

IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")


def match_balanced(s, i):
    """s[i] is an opening bracket; return the index after its matching close."""
    pairs = {"(": ")", "[": "]"}
    stack = [pairs[s[i]]]
    j = i + 1
    while stack:
        c = s[j]
        if c in pairs:
            stack.append(pairs[c])
        elif c in ")]":
            if c != stack.pop():
                raise ValueError("unbalanced at %d" % j)
        j += 1
    return j


def skip_ws(s, i):
    while i < len(s) and s[i] in " \t":
        i += 1
    return i


def parse_operand(s, i):
    """Return the end index of the unary-expression operand starting at s[i], or None."""
    if s[i] == "(":
        j = match_balanced(s, i)
    else:
        m = IDENT.match(s, i)
        if not m:
            return None
        j = m.end()
    while True:
        k = skip_ws(s, j)
        if s.startswith("->", k):
            m = IDENT.match(s, skip_ws(s, k + 2))
            if not m:
                return None
            j = m.end()
        elif k < len(s) and s[k] == "." and not s[k + 1 : k + 2].isdigit():
            m = IDENT.match(s, skip_ws(s, k + 1))
            if not m:
                return None
            j = m.end()
        elif k < len(s) and s[k] in "([":
            j = match_balanced(s, k)
        else:
            return j


def strip_outer_parens(e):
    e = e.strip()
    if e.startswith("(") and match_balanced(e, 0) == len(e):
        return e[1:-1].strip()
    return e


def rewrite(text, types):
    cast_re = re.compile(r"\*\s*\(\s*(%s)\s*\*\s*\)\s*" % "|".join(map(re.escape, types)))
    out = []
    pos = 0
    skipped = []
    loads = stores = 0
    for m in cast_re.finditer(text):
        if m.start() < pos:
            continue
        line_no = text.count("\n", 0, m.start()) + 1
        # The `*` must be a unary dereference: previous non-blank char is not an operand end.
        p = m.start() - 1
        while p >= 0 and text[p] in " \t":
            p -= 1
        if p >= 0 and (text[p].isalnum() or text[p] in "_)]"):
            skipped.append((line_no, "binary *"))
            continue
        start = m.end()
        end = parse_operand(text, start)
        if end is None:
            skipped.append((line_no, "operand not parsed"))
            continue
        operand = strip_outer_parens(text[start:end])
        width = types[m.group(1)]
        k = skip_ws(text, end)
        nxt = text[k : k + 2]
        if nxt[:1] == "=" and nxt != "==":
            # Store: must be a whole expression statement `*(T *)p = value;` at line start.
            line_start = text.rfind("\n", 0, m.start()) + 1
            if text[line_start : m.start()].strip() != "":
                skipped.append((line_no, "store not a statement"))
                continue
            semi = text.find(";", k)
            value = text[k + 1 : semi].strip()
            if "\n" in value or not value:
                skipped.append((line_no, "store value needs review"))
                continue
            out.append(text[pos : m.start()])
            out.append("Thandor_Store%s(%s, %s)" % (width, operand, value))
            pos = semi
            stores += 1
        elif nxt in ("+=", "-=", "*=", "/=", "|=", "&=", "^=", "%=", "<<", ">>", "++", "--") and not (
            nxt in ("<<", ">>") and text[k + 2 : k + 3] != "="
        ):
            skipped.append((line_no, "read-modify-write"))
            continue
        else:
            out.append(text[pos : m.start()])
            out.append("Thandor_Load%s(%s)" % (width, operand))
            pos = end
            loads += 1
    out.append(text[pos:])
    return "".join(out), loads, stores, skipped


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--type", action="append", default=[], help="TYPE=U64|U32|U16 (default MmxPackedValue64=U64)")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("files", nargs="+")
    a = ap.parse_args()
    types = dict(t.split("=", 1) for t in a.type) or {"MmxPackedValue64": "U64"}
    rc = 0
    for f in a.files:
        with open(f, encoding="utf-8", newline="") as fh:
            text = fh.read()
        new, loads, stores, skipped = rewrite(text, types)
        print("%s: %d loads, %d stores, %d left" % (f, loads, stores, len(skipped)))
        for line, why in skipped:
            print("  %s:%d: %s" % (f, line, why))
            rc = 1
        if not a.dry_run and new != text:
            with open(f, "w", encoding="utf-8", newline="") as fh:
                fh.write(new)
    return rc


if __name__ == "__main__":
    sys.exit(main())
