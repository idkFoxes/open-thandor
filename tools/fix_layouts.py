"""Make generated/types.h match the Ghidra layouts (ghidra/export/layouts.jsonl).

1. Enums Ghidra stores in 1 or 2 bytes become an integer typedef of that size; their
   constants stay available as an anonymous enum. (A C enum is always 4 bytes.)
2. Composites listed with --pack are wrapped in #pragma pack(push, 1): Ghidra lays them out
   without implicit alignment padding.

Usage: python tools/fix_layouts.py [--pack Name ...]
Run tools/check_layouts.py afterwards to verify.
"""
import json
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
T = ROOT / "include/thandor/generated/types.h"


def fix_enums(text, layouts):
    small = {d["name"]: d["length"] for d in layouts if d["kind"] == "enum" and d["length"] in (1, 2)}
    changed = []
    for name, length in small.items():
        m = re.search(r"typedef enum " + name + r" \{(.*?)\} " + name + r";", text, re.S)
        if not m:
            continue
        base = "byte" if length == 1 else "word"
        repl = (f"enum /* {name}, stored in {length} byte(s) */ {{{m.group(1)}}};\n"
                f"typedef {base} {name};")
        text = text[:m.start()] + repl + text[m.end():]
        text = re.sub(r"\benum " + name + r"\b", name, text)
        changed.append(name)
    return text, changed


def pack(text, names):
    done = []
    for name in names:
        m = re.search(r"^((?:struct|union) " + name + r" \{\n.*?^\};)", text, re.S | re.M)
        if not m or text[max(0, m.start() - 40):m.start()].rstrip().endswith("pack(push, 1)"):
            continue
        text = (text[:m.start()] + "#pragma pack(push, 1) /* Ghidra layout: no alignment padding */\n"
                + m.group(1) + "\n#pragma pack(pop)" + text[m.end():])
        done.append(name)
    return text, done


def main(argv):
    layouts = [json.loads(l) for l in (ROOT / "ghidra/export/layouts.jsonl").read_text(encoding="utf-8").splitlines()]
    text = T.read_text(encoding="utf-8")
    text, enums = fix_enums(text, layouts)
    names = argv[argv.index("--pack") + 1:] if "--pack" in argv else []
    text, packed = pack(text, names)
    T.write_text(text, encoding="utf-8", newline="\n")
    print(f"small enums: {enums}")
    print(f"packed: {packed}")


if __name__ == "__main__":
    main(sys.argv[1:])
