"""Readability metrics per original function (the ones with an "Address:" header comment in src/).

usage: python tools/readability_report.py [--list CATEGORY]

For every function it checks:
  documented   header comment with a description below the "Address:" line
  typed        no raw memory access by byte offset: *(T *)(p + 0x..), *_UI_FIELD(..., 0x.., ...),
               (int)&x + n address arithmetic
  named        no placeholder or offset-suffixed identifiers (fooXX_YY offsets, unknown*, arg0, payloadDword*,
               View<hex>, ...) in the body
  structured   no endless loop form (`while (true)`, `while (1)`, `for (;;)`) and no goto
A function is "clean" when all four hold. Prints the totals as percentages (used for the README), and the number
of hex literals in code that are neither original addresses nor bit masks (0xff, 0xffff0000, ...).
audio/codec/sam.c is left out of the offset and number checks (MMX table positions, see RAW_EXEMPT)."""
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
RAW = re.compile(r"\*\([A-Za-z_][\w ]*\*+\s*\)\s*\([^()]*\+\s*-?0x[0-9a-fA-F]+\)|\b\w+_UI_FIELD\([^)]*0x|\(int\)\s*&")
# "opaque" alone is a real graphics term (opaque pixel / blit); opaque byte ranges such as opaqueGap0000_05DF are
# caught by the offset-range pattern
PLACEHOLDER = re.compile(r"\b(?:\w*[a-z](?:[0-9A-F]{2,4}_[0-9A-F]{2,4})|\w*(?:[Uu]nknown|payloadDword|[Uu]nresolved)\w*|arg\d+|\w+View[0-9A-F]{2,}|\w+Image[0-9A-F]{3,})\b")
# the sample codec's MMX tables (audio/codec/sam.c) are addressed by genuine table positions; retyping them changes
# the generated code, so that file is left out of the offset and number checks
RAW_EXEMPT = ("sam.c",)


def is_mask(digits):
    """0xff, 0xffff, 0x3fffffff, 0xff00, 0xfffffff0, ...: one contiguous run of at least four set bits. Such
    masks read best as hex and are not counted as unnamed numbers; single flags (0x20) and short runs (0x30) are."""
    value = int(digits, 16)
    if value == 0:
        return False
    while value & 1 == 0:
        value >>= 1
    return value & (value + 1) == 0 and value >= 0xf
FLOW = re.compile(r"while\s*\(\s*(?:true|1)\s*\)|for\s*\(\s*;\s*;\s*\)|\bgoto\b")


def functions():
    for path in sorted((ROOT / "src").rglob("*.c")):
        if "generated" in path.parts or "selftest" in path.parts or path.name.startswith("selftest"):
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
        typed = path.endswith(RAW_EXEMPT) or not RAW.search(body)
        # string-literal symbols (u_/s_<text>_<address>) are named after their text, e.g. "unknown character"
        named = not PLACEHOLDER.search(re.sub(r"\b[us]_\w+_[0-9a-f]{8}\b", " ", body))
        structured = not FLOW.search(body)
        rows.append((path, name, documented, typed, named, structured))
    total = len(rows)
    columns = ("documented", "typed", "named", "structured")
    for i, column in enumerate(columns, 2):
        n = sum(1 for r in rows if r[i])
        print("%-11s %5d / %d  %5.1f %%" % (column, n, total, 100.0 * n / total))
    clean = sum(1 for r in rows if all(r[2:]))
    print("%-11s %5d / %d  %5.1f %%" % ("clean", clean, total, 100.0 * clean / total))
    # hex literals in code (not in comments), without original addresses 0x004xxxxx-0x006xxxxx, bit masks
    # (is_mask) and the sample codec's MMX tables (RAW_EXEMPT)
    hex_count = masks = 0
    for path in (ROOT / "src").rglob("*.c"):
        if "generated" in path.parts or "selftest" in path.parts or path.name.startswith("selftest") or path.name in RAW_EXEMPT:
            continue
        code = re.sub(r"/\*.*?\*/|//[^\n]*", " ", path.read_text(encoding="utf-8", errors="replace"), flags=re.S)
        # the value of a #define is where a number gets its name
        code = re.sub(r"^\s*#\s*define\s+\w+[^\n]*", " ", code, flags=re.M)
        for h in re.findall(r"\b0x([0-9a-fA-F]+)", code):
            if len(h) >= 6 and 0x400000 <= int(h, 16) < 0x700000:
                continue
            if is_mask(h):
                masks += 1
            else:
                hex_count += 1
    print("%-11s %5d unnamed hex literals in code (plus %d bit masks such as 0xff, not counted)"
          % ("numbers", hex_count, masks))
    if "--list" in sys.argv:
        column = sys.argv[sys.argv.index("--list") + 1]
        i = 2 + columns.index(column)
        for r in rows:
            if not r[i]:
                print("   ", r[0], r[1])


if __name__ == "__main__":
    main()
