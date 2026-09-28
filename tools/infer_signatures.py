"""Recover function-signature types that Ghidra leaves out of its C export.

Ghidra's "Export C" writes `FileSystemOpenProc *g_FileSystemOpen;` but never
the FunctionDefinition `FileSystemOpenProc` itself. Two sources recover it:

  1. assignment: `g_FileSystemOpen = Win32File_Open;` -> copy the full
     prototype of Win32File_Open from the public headers;
  2. call site:  `FVar10 = (*g_FatalErrorPrimaryDispatchCf)(...)` -> the return
     type is the declared type of FVar10; parameters stay unprototyped.

Anything else stays an unprototyped `dword Name()` placeholder.
Imported by tools/gen_globals.py.
"""
import json
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


GHIDRA_EXPORT = ROOT / "ghidra/export/function_definitions.jsonl"


def ghidra_definitions():
    """FunctionDefinition types exported by tools/ghidra/ExportBuildData.java, if present."""
    if not GHIDRA_EXPORT.exists():
        return {}
    defs = {}
    for line in GHIDRA_EXPORT.read_text(encoding="utf-8").splitlines():
        d = json.loads(line)
        defs.setdefault(d["name"], d)
    return defs


def infer(ptypes, globals_decls):
    ghidra = ghidra_definitions()
    if ghidra:
        # the program database is authoritative; heuristics only for names it lacks
        types_h = read(ROOT / "include/thandor/generated/types.h")
        defined = lambda n: re.search(r"typedef[^;{]*\b" + n + r"\b\s*[(;\[]|\}\s*" + n + r"\b", types_h)
        ordered, seen = [], set()

        def visit(name):
            # dependencies (other FunctionDefinitions used in the prototype) first
            if name in seen or name not in ghidra or defined(name):
                return
            seen.add(name)
            d = ghidra[name]
            for t in [d["return"]] + [p["type"] for p in d["params"]]:
                visit(t.replace("*", "").strip())
            ordered.append(name)

        for p in ptypes:
            visit(p)
        exact = {p: ("ghidra", ghidra[p]) for p in ordered}
        rest = [p for p in ptypes if p not in ghidra]
        return {**exact, **(infer_heuristic(rest, globals_decls) if rest else {})}
    return infer_heuristic(ptypes, globals_decls)


def is_import(d):
    """DLL entry points (Glide, WinSock) really are __stdcall."""
    return ("Import" in d["category"] or "/Network" in d["category"]
            or d["name"].startswith(("WinSock_", "WSA", "Gr")))


def infer_heuristic(ptypes, globals_decls):
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
        if sig and sig[0] == "full" and names:
            sig = validate(sig, names, sources.values())
        result[ptype] = sig
    return result


def call_sites(names, texts):
    """(argument count, result used) for every `(*holder)(...)` call."""
    alt = "|".join(map(re.escape, sorted(names)))
    pat = re.compile(r"(=\s*)?\(\s*\*\s*(?:[\w\.\->\[\]]*?)\b(?:" + alt + r")\b(?:\s*\[[^\]]*\])?\s*\)\s*\(")
    for text in texts:
        for m in pat.finditer(text):
            i, depth, args, nonempty = m.end(), 1, 0, False
            while depth and i < len(text):
                c = text[i]
                if c in "([":
                    depth += 1
                elif c in ")]":
                    depth -= 1
                elif c == "," and depth == 1:
                    args += 1
                elif not c.isspace() and depth == 1:
                    nonempty = True
                i += 1
            yield (args + 1 if nonempty or args else 0), bool(m.group(1))


def validate(sig, names, texts):
    """Keep a recovered prototype only if every call site agrees with it."""
    _, fn, ret, params = sig
    count = 0 if params.strip() in ("", "void") else params.count(",") + 1
    sites = list(call_sites(names, texts))
    if any(n != count for n, _ in sites):
        return ("ret", ret if ret.split()[0] != "void" else "dword")
    if ret.split()[0] == "void" and any(used for _, used in sites):
        return ("ret", "dword")
    return sig


def render(ptype, sig):
    if sig and sig[0] == "ghidra":
        proto = sig[1]["prototype"].replace(" unknown ", " ", 1)
        if not is_import(sig[1]):
            # Ghidra's default x86 convention; the game's own functions are plain C (cdecl) here
            proto = proto.replace(" __stdcall ", " ", 1)
        # the original register conventions (__thandor_*) do not matter to the C build
        proto = re.sub(r" __thandor_\w+ ", " ", proto, count=1)
        return f"typedef {proto}; /* Ghidra FunctionDefinition {sig[1]['category']} */"
    if sig is None:
        return f"typedef dword {ptype}(); /* TODO: unrecovered signature */"
    if sig[0] == "full":
        _, fn, ret, params = sig
        return f"typedef {ret} {ptype}({params}); /* recovered from {fn} */"
    return f"typedef {sig[1]} {ptype}(); /* return type from call site; parameters TODO */"
