"""Give unrecovered Ghidra stack references (`stack0xffffffd0`) a home.

Ghidra prints stack slots it could not turn into locals as `stack0x<offset>` relative to
the entry ESP. Each function that uses them gets one byte array standing in for its
frame, and every reference maps to the same relative position inside it:

    &stack0xffffffd0   ->   &thandor_stack_frame[0x80 - 0x30]

This keeps the relative layout between those references (arrays of records indexed by
`+ i * 8` keep working), but not their overlap with named locals, so every such function
is marked TODO for manual recovery.

Usage: python tools/fix_stack_refs.py [files...]   (default: every src/**/*.c)
"""
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
REF = re.compile(r"\bstack0x([0-9a-f]{8})\b")
BIAS = 0x80
DECL = ("  byte thandor_stack_frame[0x100]; "
        "/* TODO: unrecovered Ghidra stack slots (stack0x...), entry ESP at index 0x80 */\n")


def index(hexoff):
    off = int(hexoff, 16)
    if off >= 0x80000000:
        off -= 0x100000000
    idx = BIAS + off
    assert 0 <= idx < 0x100, hexoff
    return f"0x{BIAS:x} - 0x{-off:x}" if off < 0 else f"0x{BIAS:x} + 0x{off:x}"


def main(files):
    paths = [pathlib.Path(f) for f in files] or sorted((ROOT / "src").rglob("*.c"))
    for p in paths:
        text = p.read_text(encoding="utf-8")
        if not REF.search(text):
            continue
        out, pos, n = [], 0, 0
        # function bodies: "{" at column 0 through "}" at column 0
        for m in re.finditer(r"(?m)^\{\n(.*?)^\}", text, re.S):
            body = m.group(1)
            # only code: comment spans are copied through untouched
            parts = re.split(r"(/\*.*?\*/|//[^\n]*)", body, flags=re.S)
            if not any(REF.search(s) for s in parts[::2]):
                continue
            n += sum(len(REF.findall(s)) for s in parts[::2])
            new = "".join(s if i % 2 else REF.sub(lambda r: f"thandor_stack_frame[{index(r.group(1))}]", s)
                          for i, s in enumerate(parts))
            out.append(text[pos:m.start(1)])
            out.append(DECL + new)
            pos = m.end(1)
        out.append(text[pos:])
        p.write_text("".join(out), encoding="utf-8", newline="\n")
        print(f"{p.relative_to(ROOT)}: {n} references")


if __name__ == "__main__":
    main(sys.argv[1:])
