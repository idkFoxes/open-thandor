"""Rewrite Ghidra register-image casts that C rejects, driven by MSVC C2440 errors.

Ghidra models multi-register values as structs ({eax, carry}, {eax, ecx, carry}, ...)
and freely casts them to/from integers such as uint5 or to other layout-compatible
structs. C forbids casts involving struct types, so each flagged site is rewritten to

    THANDOR_BITCAST(From, To, expr)

(defined in core/contracts.h), which reinterprets the bytes through a union.

Handled error shapes (German or English MSVC messages):
    "Typumwandlung"/"type cast": "From" -> "To"   ->  (To)expr      => BITCAST(From, To, expr)
    "=":                         "From" -> "To"   ->  lhs = expr;   => lhs = BITCAST(From, To, expr);

A site is only rewritten when exactly one candidate matches inside the statement,
so ambiguous lines are reported and left for manual work.

Usage: python tools/fix_abi_casts.py msvc.log
"""
import collections
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
ERR = re.compile(r'^(?P<file>[^(]+)\((?P<line>\d+)\): error C2440: "(?P<op>[^"]+)": "(?P<from>[^"]+)" '
                 r'(?:kann nicht in|cannot convert from) "(?P<to>[^"]+)"')
CAST_OPS = {"Typumwandlung", "type cast"}
FORTY_BIT = {"uint5", "undefined5"}


def statement_span(text, line_start):
    """From the start of the error line to the terminating ';' at paren depth 0."""
    depth, i = 0, line_start
    while i < len(text):
        c = text[i]
        if c in "([":
            depth += 1
        elif c in ")]":
            depth -= 1
        elif c in ";{}" and depth <= 0:
            return line_start, i
        i += 1
    return line_start, len(text)


def operand_end(text, i):
    """End of the unary expression starting at i (after a cast)."""
    while text[i] in " \t\r\n":
        i += 1
    while text[i] in "*&-!~":
        i += 1
    if text[i] == "(":
        i = match_paren(text, i)
        # a nested cast "(T)x" continues with its operand
        if re.match(r"\s*[A-Za-z_(*&]", text[i:]) and re.fullmatch(r"\([\w\s\*]+\)", text[text.rfind("(", 0, i):i] or ""):
            return operand_end(text, i)
    else:
        m = re.match(r"[A-Za-z_]\w*|0x[0-9a-fA-F]+|\d+", text[i:])
        if not m:
            return None
        i += m.end()
    while True:
        m = re.match(r"\s*(\.|->)\s*[A-Za-z_]\w*", text[i:])
        if m:
            i += m.end()
            continue
        m = re.match(r"\s*[\[(]", text[i:])
        if m:
            i = match_paren(text, i + m.end() - 1)
            continue
        return i


def match_paren(text, i):
    pairs = {"(": ")", "[": "]"}
    stack = [pairs[text[i]]]
    i += 1
    while stack:
        c = text[i]
        if c in pairs:
            stack.append(pairs[c])
        elif c == stack[-1]:
            stack.pop()
        i += 1
    return i


def wrap(frm, to, expr):
    expr = expr.strip()
    if to in FORTY_BIT:
        return f"(THANDOR_BITCAST({frm}, qword, {expr}) & 0xFFFFFFFFFFull)"
    return f"THANDOR_BITCAST({frm}, {to}, {expr})"


def main(log):
    errors = collections.defaultdict(list)
    for line in open(log, encoding="mbcs" if sys.platform == "win32" else "utf-8", errors="replace"):
        m = ERR.match(line.strip())
        if m and (m["op"] in CAST_OPS or m["op"] == "="):
            errors[m["file"]].append((int(m["line"]), m["op"], m["from"], m["to"]))
    fixed = skipped = 0
    for rel, errs in errors.items():
        path = ROOT / rel
        text = path.read_text(encoding="utf-8")
        line_starts = [0] + [m.end() for m in re.finditer("\n", text)]
        edits = {}
        for lineno, op, frm, to in sorted(set(errs)):
            start, end = statement_span(text, line_starts[lineno - 1])
            stmt = text[start:end]
            if op == "=":
                cands = [m for m in re.finditer(r"(?<![=!<>+\-*/%&|^])=(?!=)", stmt)]
                if len(cands) != 1:
                    skipped += 1
                    print(f"skip {rel}:{lineno} ({len(cands)} assignments)")
                    continue
                a = start + cands[0].end()
                edits[(a, end)] = " " + wrap(frm, to, text[a:end])
            else:
                cands = [m for m in re.finditer(r"\(\s*" + re.escape(to).replace(r"\ ", r"\s*") + r"\s*\)", stmt)]
                if len(cands) != 1:
                    skipped += 1
                    print(f"skip {rel}:{lineno} ({len(cands)} casts to {to})")
                    continue
                a = start + cands[0].start()
                b = operand_end(text, start + cands[0].end())
                if b is None:
                    skipped += 1
                    print(f"skip {rel}:{lineno} (operand)")
                    continue
                edits[(a, b)] = wrap(frm, to, text[start + cands[0].end():b])
        # apply back to front; drop overlapping edits
        last = len(text) + 1
        for (a, b), new in sorted(edits.items(), reverse=True):
            if b > last:
                skipped += 1
                continue
            text = text[:a] + new + text[b:]
            last = a
            fixed += 1
        path.write_text(text, encoding="utf-8", newline="\n")
    print(f"rewrote {fixed} sites, skipped {skipped}")


if __name__ == "__main__":
    main(sys.argv[1])
