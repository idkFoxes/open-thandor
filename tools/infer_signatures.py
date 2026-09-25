"""Recover function-signature types that Ghidra leaves out of its C export.

Ghidra's "Export C" writes `FileSystemOpenCfProc *g_FileSystemOpenCf;` but never
the FunctionDefinition `FileSystemOpenCfProc` itself. Two sources recover it:

  1. assignment: `g_FileSystemOpenCf = Win32File_OpenCf;` -> copy the full
     prototype of Win32File_OpenCf from the public headers;
  2. call site:  `FVar10 = (*g_FatalErrorPrimaryDispatchCf)(...)` -> the return
     type is the declared type of FVar10; parameters stay unprototyped.

Anything else stays an unprototyped `dword Name()` placeholder.
Imported by tools/gen_globals.py.
"""
import pathlib
import re

ROOT = pathlib.Path(__file__).resolve().parent.parent
PROTO = re.compile(r"(?:^|;|\}|\*/)\s*([A-Za-z_][\w\s\*]*?)\s+([A-Za-z_]\w*)\s*\(([^;{}()]*(?:\([^()]*\)[^;{}()]*)*)\)\s*;", re.S)


def strip_comments(text):
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    return re.sub(r"//[^\n]*", " ", text)


def read(p):
    return p.read_text(encoding="utf-8", errors="replace")


def header_prototypes():
    protos = {}
    for p in (ROOT / "include/thandor").rglob("*.h"):
        if p.parent.name == "generated":
            continue
        for ret, name, params in PROTO.findall(strip_comments(read(p))):
            ret = " ".join(ret.split())
            if ret.startswith(("typedef", "return", "extern")):
                continue
            protos[name] = (ret, " ".join(params.split()))
    # file-local functions only have their definition: "Ret conv\nName(params)\n\n{"
    defn = re.compile(r"^([A-Za-z_][\w \*]*?)\s*\n?([A-Za-z_]\w*)\(([^;{}]*)\)\s*\{", re.M)
    for p in (ROOT / "src").rglob("*.c"):
        for ret, name, params in defn.findall(strip_comments(read(p))):
            ret = " ".join(ret.split())
            if name not in protos and ret and not re.match(r"(if|while|for|switch|return|else|do)\b", ret):
                protos[name] = (ret, " ".join(params.split()))
    return protos


def holders(ptype, types_h, globals_decls):
    """Names of globals and struct fields whose type is `ptype *`."""
    names = {n for t, n in globals_decls if t.replace(" ", "") == ptype + "*"}
    names.update(re.findall(r"\b" + ptype + r"\s*\*\s*(\w+)\s*(?:\[\w*\])?\s*;", types_h))
    return names


def infer(ptypes, globals_decls):
    types_h = read(ROOT / "include/thandor/generated/types.h")
    protos = header_prototypes()
    sources = {p: strip_comments(read(p)) for p in (ROOT / "src").rglob("*.c") if p.parent.name != "generated"}
    result = {}
    for ptype in ptypes:
        names = holders(ptype, types_h, globals_decls)
        sig = None
        if names:
            alt = "|".join(map(re.escape, sorted(names)))
            assign = re.compile(r"\b(?:" + alt + r")\s*(?:\[[^\]]*\])?\s*=\s*(?:\(\s*" + ptype + r"\s*\*\s*\))?\s*&?\s*([A-Za-z_]\w*)\s*;")
            call = re.compile(r"\b([A-Za-z_]\w*)\s*=\s*\(\s*\*\s*(?:[\w\.\->\[\]\(\)]*?)\b(?:" + alt + r")\b(?:\s*\[[^\]]*\])?\s*\)\s*\(")
            for text in sources.values():
                for fn in assign.findall(text):
                    if fn in protos:
                        sig = ("full", fn) + protos[fn]
                        break
                if sig:
                    break
            if not sig:
                for text in sources.values():
                    for m in call.finditer(text):
                        var = m.group(1)
                        decl = re.findall(r"^\s+([A-Za-z_][\w ]*?\s*\**)\s*\b" + var + r"\s*;", text[:m.start()], re.M)
                        if decl:
                            sig = ("ret", decl[-1].strip())
                            break
                    if sig:
                        break
        result[ptype] = sig
    return result


def render(ptype, sig):
    if sig is None:
        return f"typedef dword {ptype}(); /* TODO: unrecovered signature */"
    if sig[0] == "full":
        _, fn, ret, params = sig
        return f"typedef {ret} {ptype}({params}); /* recovered from {fn} */"
    return f"typedef {sig[1]} {ptype}(); /* return type from call site; parameters TODO */"
