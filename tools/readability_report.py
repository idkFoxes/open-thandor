"""Readability metrics per original function (the ones with an "Address:" header comment in src/).

usage: python tools/readability_report.py [--list CATEGORY]

For every function it checks:
  documented   header comment with a description below the "Address:" line
  typed        no raw memory access by byte offset: *(T *)(p + 0x..), *_UI_FIELD(..., 0x.., ...),
               (int)&x + n address arithmetic
  named        no placeholder or offset-suffixed identifiers (fooXX_YY offsets, unknown*, arg0, payloadDword*,
               View<hex>, ...) in the body
  structured   no `while( true )` and no goto
A function is "clean" when all four hold. Prints the totals as percentages (used for the README)."""
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
RAW = re.compile(r"\*\([A-Za-z_][\w ]*\*+\s*\)\s*\([^()]*\+\s*-?0x[0-9a-fA-F]+\)|\b\w+_UI_FIELD\([^)]*0x|\(int\)\s*&")
PLACEHOLDER = re.compile(r"\b(?:\w*[a-z](?:[0-9A-F]{2,4}_[0-9A-F]{2,4})|\w*(?:[Uu]nknown|payloadDword|[Uu]nresolved|[Oo]paque)\w*|arg\d+|\w+View[0-9A-F]{2,}|\w+Image[0-9A-F]{3,})\b")
FLOW = re.compile(r"while\( true \)|\bgoto\b")


def functions():
    for path in sorted((ROOT / "src").rglob("*.c")):
        if "generated" in path.parts or path.name.startswith("selftest"):
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        starts = [m.start() for m in re.finditer(r"/\* Address: 0x", text)] + [len(text)]
        for a, b in zip(starts, starts[1:]):
            chunk = text[a:b]
            end = chunk.find("*/")
            header, body = chunk[:end], chunk[end + 2:]
            body = re.sub(r"/\*.*?\*/|//[^\n]*", " ", body, flags=re.S)
            name = re.search(r"(\w+)\s*\n?\s*\(", body)
            yield path.relative_to(ROOT).as_posix(), name.group(1) if name else "?", header, body


def main():
    rows = []
    for path, name, header, body in functions():
        documented = len([l for l in header.splitlines()[1:] if l.strip()]) >= 1
        typed = not RAW.search(body)
        named = not PLACEHOLDER.search(body)
        structured = not FLOW.search(body)
        rows.append((path, name, documented, typed, named, structured))
    total = len(rows)
    columns = ("documented", "typed", "named", "structured")
    for i, column in enumerate(columns, 2):
        n = sum(1 for r in rows if r[i])
        print("%-11s %5d / %d  %5.1f %%" % (column, n, total, 100.0 * n / total))
    clean = sum(1 for r in rows if all(r[2:]))
    print("%-11s %5d / %d  %5.1f %%" % ("clean", clean, total, 100.0 * clean / total))
    if "--list" in sys.argv:
        column = sys.argv[sys.argv.index("--list") + 1]
        i = 2 + columns.index(column)
        for r in rows:
            if not r[i]:
                print("   ", r[0], r[1])


if __name__ == "__main__":
    main()
