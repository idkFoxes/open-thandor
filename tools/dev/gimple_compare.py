#!/usr/bin/env python3
"""Compare the optimised GCC middle-end output of one source file between a git revision and the working tree.

For refactorings whose machine code is not byte-identical only because the register allocator and the
scheduler break ties by SSA version numbers (e.g. a cast `*(uint64_t *)p` replaced by an inline memcpy
helper Thandor_LoadU64(p): same loads, same arithmetic, but the extra inlining shifts every SSA number).

Both versions are compiled with the file's own command from compile_commands.json plus
-fdump-tree-optimized; the dumps are normalised and diffed:
  - declarations and `# DEBUG` bind statements (-g) dropped, SSA names renumbered in definition order (so only the statements count),
  - the two spellings of a load, `MEM[(T *)p]` and `MEM <T> [(char * {ref-all})p]` (memcpy), unified
    to `LOAD<T>(p)` (typedefs mapped with --alias, e.g. MmxPackedValue64="long long unsigned int"),
  - operands of commutative binary operators (* + & | ^ == !=) sorted.
Equal normalised dumps mean the same operations in the same order on the same values; a real change
(another offset, operator, width, order) shows as a difference.

usage:
  python tools/dev/gimple_compare.py -p build-mingw-release [--rev HEAD] [--alias T=U ...] src/a.cpp [src/b.cpp ...]
Exit 0 when every file is equal.
"""
import argparse
import difflib
import glob
import json
import os
import re
import shlex
import subprocess
import sys
import tempfile

SSA = re.compile(r"(?<![A-Za-z0-9_.])((?:[A-Za-z_][A-Za-z0-9_.]*)?_\d+)(?![A-Za-z0-9_])")
LOAD_CAST = re.compile(r"MEM\[\(([A-Za-z_][A-Za-z0-9_ ]*?) \*\)([^\]]+)\]")
LOAD_REFALL = re.compile(r"MEM <([^>]+)> \[\(char \* \{ref-all\}\)([^\]]+)\]")
COMM = re.compile(r"^(\s*\S+ = )(\S+) ([*+&|^]|==|!=) (\S+);$")
DEF = re.compile(r"^\s*(?:# )?(\S+) = ")
DECL = re.compile(r"^\s+[^=]*;$")

C_TYPES = {"unsigned long long": "long long unsigned int", "uint64_t": "long long unsigned int",
           "unsigned int": "unsigned int", "uint32_t": "unsigned int",
           "unsigned short": "short unsigned int", "uint16_t": "short unsigned int"}


def normalise(text, aliases):
    types = dict(C_TYPES)
    types.update(aliases)

    def load(m):
        t = m.group(1).strip()
        return "LOAD<%s>(%s)" % (types.get(t, t), m.group(2))

    funcs, cur = [], []
    for line in text.splitlines():
        if line.startswith(";; Function") and cur:
            funcs.append(cur)
            cur = []
        cur.append(line)
    funcs.append(cur)
    out = []
    for f in funcs:
        stmts, in_body = [], False
        for line in f:
            if "<bb " in line:
                in_body = True
            if (not in_body and DECL.match(line)) or line.lstrip().startswith("# DEBUG"):
                continue
            line = LOAD_CAST.sub(load, line)
            line = LOAD_REFALL.sub(load, line)
            stmts.append(line)
        names = {}
        for line in stmts:
            m = DEF.match(line)
            if m and SSA.fullmatch(m.group(1)) and m.group(1) not in names:
                names[m.group(1)] = "v%d" % len(names)

        def ren(m):
            n = m.group(1)
            return names.get(n) or re.sub(r"_\d+$", "", n) + "_D"

        for line in stmts:
            line = SSA.sub(ren, line)
            m = COMM.match(line)
            if m:
                a, b = sorted((m.group(2), m.group(4)))
                line = "%s%s %s %s;" % (m.group(1), a, m.group(3), b)
            out.append(line)
    return out


def compile_dump(cmd, directory, src_text, src_name, tmp, tag):
    src = os.path.join(tmp, "%s_%s" % (tag, os.path.basename(src_name)))
    with open(src, "w", encoding="utf-8", newline="") as fh:
        fh.write(src_text)
    args = shlex.split(cmd.replace("\\", "/"), posix=True)
    new, skip = [], False
    for a in args:
        if skip:
            skip = False
            continue
        if a == "-o":
            skip = True
            continue
        if a == "-c" or a.replace("\\", "/").endswith(src_name.replace("\\", "/")):
            continue
        new.append(a)
    obj = os.path.join(tmp, tag + ".o")
    subprocess.run(new + ["-fdump-tree-optimized", "-dumpdir", tmp + "/", "-c", src, "-o", obj],
                   cwd=directory, check=True)
    dumps = glob.glob(os.path.join(tmp, "*%s_*.optimized" % tag)) + glob.glob(os.path.join(tmp, "%s*.optimized" % tag))
    if not dumps:
        sys.exit("gimple_compare: no dump written for " + src_name)
    with open(sorted(set(dumps))[0], encoding="utf-8", errors="replace") as fh:
        return fh.read()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-p", required=True, help="build directory with compile_commands.json")
    ap.add_argument("--rev", default="HEAD", help="git revision of the reference (default HEAD)")
    ap.add_argument("--alias", action="append", default=[], help='TYPE="gimple type", e.g. MmxPackedValue64="long long unsigned int"')
    ap.add_argument("files", nargs="+")
    a = ap.parse_args()
    aliases = dict(x.split("=", 1) for x in a.alias)
    root = subprocess.run(["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True, check=True).stdout.strip()
    db = json.load(open(os.path.join(a.p, "compile_commands.json")))
    rc = 0
    for f in a.files:
        rel = os.path.relpath(os.path.abspath(f), root).replace("\\", "/")
        entry = next((e for e in db if e["file"].replace("\\", "/").endswith(rel)), None)
        if entry is None:
            sys.exit("gimple_compare: %s not in compile_commands.json" % rel)
        cmd = entry.get("command") or " ".join(shlex.quote(x) for x in entry["arguments"])
        ref_text = subprocess.run(["git", "show", "%s:%s" % (a.rev, rel)], capture_output=True, text=True,
                                  encoding="utf-8", check=True, cwd=root).stdout
        new_text = open(os.path.join(root, rel), encoding="utf-8", newline="").read()
        with tempfile.TemporaryDirectory() as tmp:
            ref = normalise(compile_dump(cmd, entry["directory"], ref_text, rel, tmp, "ref"), aliases)
            new = normalise(compile_dump(cmd, entry["directory"], new_text, rel, tmp, "new"), aliases)
        diff = list(difflib.unified_diff(ref, new, "ref", "new", lineterm="", n=1))
        if diff:
            rc = 1
            print("%s: DIFFERENT (%d normalised lines, %d changed)" % (rel, len(new), sum(1 for d in diff if d[:1] in "+-") - 2))
            print("\n".join(diff[:40]))
        else:
            print("%s: equal (%d normalised GIMPLE lines)" % (rel, len(new)))
    return rc


if __name__ == "__main__":
    sys.exit(main())
