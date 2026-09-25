"""Check function-pointer slots in the original image against the functions stored in them.

Slots in the mapped data image (see platform/bootstrap/image.h) are filled with recovered C
functions at runtime, bypassing the compiler's type checks. On x86 MSVC a mismatch in how a
result is returned breaks the call silently:

    scalar/bool   -> EAX (AL)
    struct <= 8   -> EDX:EAX
    struct > 8    -> hidden result pointer as extra first stack argument

This compares, for every global whose type is `Proc *` (or an array of them), the return
class of the slot type with the return class of the function the original image stores there.

Usage: python tools/check_slots.py path\\to\\thandor_original.exe
"""
import json
import pathlib
import re
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent


def struct_sizes(types_h):
    """Very small layout model: sizes of structs made of scalars/pointers/arrays/nested structs."""
    scalar = {"byte": 1, "char": 1, "bool": 1, "undefined": 1, "undefined1": 1, "sbyte": 1,
              "word": 2, "short": 2, "ushort": 2, "undefined2": 2, "wchar_t": 2,
              "dword": 4, "int": 4, "uint": 4, "sdword": 4, "undefined4": 4, "float": 4, "long": 4, "ulong": 4,
              "qword": 8, "longlong": 8, "ulonglong": 8, "undefined8": 8, "double": 8}
    typedefs = dict(re.findall(r"^typedef\s+(?:enum\s+)?(\w+)\s+(\w+);", types_h, re.M))
    bodies = dict(re.findall(r"^struct (\w+) \{\n(.*?)^\};", types_h, re.S | re.M))
    cache = {}

    def size(t, depth=0):
        t = t.strip()
        if t.endswith("*"):
            return 4
        t = re.sub(r"^(struct|union|enum)\s+", "", t)
        if t in scalar:
            return scalar[t]
        if t in cache:
            return cache[t]
        if depth > 20:
            return None
        if t in typedefs:
            return size(typedefs[t], depth + 1)
        if re.search(r"^typedef enum \w* ?\{[^}]*\} " + t + ";", types_h, re.M | re.S):
            return 4
        if t in bodies:
            total = 0
            for field in re.sub(r"//[^\n]*", "", bodies[t]).split(";"):
                field = field.strip()
                if not field:
                    continue
                if "(" in field:
                    total += 4
                    continue
                m = re.match(r"(.*?)(\w+)((?:\[\w+\])*)$", field)
                if not m:
                    return None
                s = size(m.group(1), depth + 1)
                if s is None:
                    return None
                for d in re.findall(r"\[(\w+)\]", m.group(3)):
                    s *= int(d, 0)
                total += s
            cache[t] = total
            return total
        return None

    return size


def return_class(ret, size):
    ret = ret.strip()
    if ret.startswith("void") and "*" not in ret:
        return "void"
    s = size(ret)
    if s is None:
        return "?" + ret
    base = re.sub(r"^(struct|union)\s+", "", ret)
    is_struct = re.search(r"\b(struct|union)\b", ret) or (s and not ret.endswith("*") and s not in (1, 2, 4, 8)) \
        or re.search(r"(Cf\d+|Regs\w*|Eax\w*|Pair\w*|Lanes)$", base)
    if is_struct:
        return "struct>8" if s > 8 else "struct<=8"
    return "scalar"


def main(original):
    types_h = (ROOT / "include/thandor/generated/types.h").read_text(encoding="utf-8")
    globals_h = (ROOT / "include/thandor/generated/globals.h").read_text(encoding="utf-8")
    size = struct_sizes(types_h)
    proto_ret = {}
    for text in (types_h, globals_h):
        for ret, name in re.findall(r"^typedef\s+(.+?)\s+(?:__\w+\s+)?(\w+)\s*\([^;]*\)\s*;", text, re.M):
            proto_ret[name] = ret
    src = "\n".join(re.sub(r"/\*.*?\*/", " ", p.read_text(encoding="utf-8"), flags=re.S)
                    for p in (ROOT / "src").rglob("*.c") if p.parent.name != "generated")
    func_ret = {}
    for ret, name in re.findall(r"^([A-Za-z_][\w \*]*?)\s+(?:__\w+\s+)?\n?(\w+)\s*\([^;{}]*\)\s*\{", src, re.M):
        func_ret.setdefault(name, ret)
    by_addr = {int(f["address"], 16): f["name"] for f in
               (json.loads(l) for l in (ROOT / "ghidra/export/functions.jsonl").read_text(encoding="utf-8").splitlines())}
    aliases = {}
    fm = (ROOT / "tools/gen_function_map.py").read_text(encoding="utf-8")
    for a, b in re.findall(r'"(\w+)":\s*"(\w+)"', fm):
        aliases[a] = b

    data = open(original, "rb").read()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    opt = struct.unpack_from("<H", data, pe + 20)[0]
    sec = pe + 24 + opt
    va, raw = struct.unpack_from("<I", data, sec + 12)[0], struct.unpack_from("<I", data, sec + 20)[0]

    def dword(addr):
        return struct.unpack_from("<I", data, addr - 0x400000 - va + raw)[0]

    problems = 0
    for name, ptype, count, addr in re.findall(
            r"^#define (\w+) \(\*\((\w+) \* (?:\*|\(\*\)\[(\d+)\])\)0x([0-9a-f]+)\)", globals_h, re.M):
        if ptype not in proto_ret:
            continue
        want = return_class(proto_ret[ptype], size)
        for i in range(int(count or 1)):
            target = dword(int(addr, 16) + 4 * i)
            fn = by_addr.get(target)
            if not fn:
                continue
            fn = aliases.get(fn, fn)
            have = return_class(func_ret.get(fn, "?"), size)
            if want != have:
                problems += 1
                print(f"{name}[{i}] ({ptype}: {want}) <- {fn} ({have}: {func_ret.get(fn)})")
    print(f"{problems} mismatching slot(s)")


if __name__ == "__main__":
    main(sys.argv[1])
