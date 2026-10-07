#!/usr/bin/env python3
"""Compare the optimised GCC middle-end output of one source file between a git revision and the working tree.

For refactorings whose machine code is not byte-identical only because the register allocator and the
scheduler break ties by SSA version numbers (e.g. a cast `*(uint64_t *)p` replaced by an inline memcpy
helper Thandor_LoadU64(p): same loads, same arithmetic, but the extra inlining shifts every SSA number).

Both versions are compiled with the file's own command from compile_commands.json plus
-fdump-tree-optimized: the reference from a `git archive` of --rev (src/ and include/ unpacked into a temp
dir, the repository's include directories pointed into it), so a package that changes header signatures or
uses relative includes compiles against the reference's own headers; the new one from the working tree.
The dumps are normalised and diffed:
  - declarations and `# DEBUG` bind statements (-g) dropped, SSA names renumbered in definition order (so only the statements count),
  - the two spellings of a load, `MEM[(T *)p]` and `MEM <T> [(char * {ref-all})p]` (memcpy), unified
    to `LOAD<T>(p)` (typedefs mapped with --alias, e.g. MmxPackedValue64="long long unsigned int"),
  - operands of commutative binary operators (* + & | ^ == !=) sorted,
  - the numbers of scalar-replacement names (`ISRA.92`, `point$x_1.88`) and temporaries (`D.122377`) masked,
  - a member read through a cast load of the struct, `LOAD<struct T>(p).f`, written as `p->f`,
  - the `;; Function` header lines reduced to the name (funcdef_no, decl_uid, cgraph_uid and symbol_order
    shift when the file gains or loses an inline helper or template instance),
  - every --alias typedef name also mapped where it names a type in a cast or MEM (`(Name *)`, `<Name>`).
Equal normalised dumps mean the same operations in the same order on the same values; a real change
(another offset, operator, width, order) shows as a difference.

--bool (for Bool8 -> bool packages) canonicalises the representation-only differences of bool vs uint8_t
before the normalisation above: the type names Bool8/bool, the 'h'/'b' parameter codes of mangled names in the
`;; Function` headers, `x = (Bool8|bool|unsigned char) c;` when c is a comparison result (x replaced by c),
`if (x == 0) goto A; else goto B;` written as `if (x != 0) goto B; else goto A;`, the operands of `if (a == b)` /
`if (a != b)` sorted, basic blocks renumbered in order of definition (also in PHI edges), profile counts and
probabilities and the `Removing basic block` lines dropped. What remains is a real difference or a bool
value-range result (a PHI argument known to be 0/1 on an edge, jump threading) that the report must explain.

--calls-stores (GIMPLE mode) additionally compares, per function, the multiset of called functions and of
memory stores (targets with `->`, MEM, `*p` or a g_ global; SSA numbers masked, indirect calls counted under
one name): a cheap check that a remaining GIMPLE difference only reorders or threads code.

--objdump compares the machine code instead (objects built with -g0, `objdump -d -r`, addresses removed,
branch targets as function offsets, relocations kept). Function labels carry the mangled name, so a changed
signature shows as a changed label; an equal listing means identical instructions.

usage:
  python tools/dev/gimple_compare.py -p build-mingw-release [--rev HEAD] [--alias T=U ...] [--objdump | [--bool] [--calls-stores]] src/a.cpp [src/b.cpp ...]
Exit 0 when every file is equal.
"""
import argparse
import collections
import difflib
import glob
import io
import json
import os
import re
import shlex
import subprocess
import tarfile
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


# The per-compilation numbers of scalar-replacement names (`ISRA.92`, `point$x_1.88`) and of compiler
# temporaries (`D.122377`), which shift like the SSA numbers when the file gains or loses an inline helper.
# A member read through a cast load of the whole struct, `LOAD<const struct T>(p).f`: the same access as
# `p->f` once p has the type T * (a parameter retyped from int * to T *).
STRUCT_LOAD = re.compile(r"LOAD<(?:const )?struct [A-Za-z_][A-Za-z0-9_]*>\(([A-Za-z0-9_.()]+?)\)\.")

SRA_NUMBER = re.compile(r"\bISRA\.\d+|(?<=[A-Za-z0-9_$]_\d)\.\d+(?=_D\b)|(?<=_\d\d)\.\d+(?=_D\b)|(?<=_\d\d\d)\.\d+(?=_D\b)|\bD\.\d+\b")

# The per-compilation numbers in a header line `;; Function F (mangled, funcdef_no=N, decl_uid=N, ...)`.
FUNC_NUMBERS = re.compile(r", funcdef_no=[^)]*\)")


def normalise(text, aliases):
    types = dict(C_TYPES)
    types.update(aliases)

    def load(m):
        t = m.group(1).strip()
        return "LOAD<%s>(%s)" % (types.get(t, t), m.group(2))

    alias_rx = [(re.compile(r"(?<=[(<])%s(?= ?[*>])" % re.escape(k)), v) for k, v in aliases.items()]

    funcs, cur = [], []
    for line in text.splitlines():
        if line.startswith(";; Function"):
            line = FUNC_NUMBERS.sub(")", line)
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
            for rx, v in alias_rx:
                line = rx.sub(v, line)
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
            line = STRUCT_LOAD.sub(r"\1->", line)
            line = SRA_NUMBER.sub(lambda m: m.group(0).split(".")[0] + ".N", line)
            m = COMM.match(line)
            if m:
                a, b = sorted((m.group(2), m.group(4)))
                line = "%s%s %s %s;" % (m.group(1), a, m.group(3), b)
            out.append(line)
    return out


def tree_args(cmd, rel, tree, root):
    """The file's compile command for the copy of the source tree at `tree`: include directories inside the
    repository (root) that exist in `tree` are redirected there, so the reference is compiled against its own
    headers; build directories (generated headers) are kept. -o, -c and the source are dropped."""
    args = shlex.split(cmd.replace("\\", "/"), posix=True)
    rootn = os.path.normcase(os.path.abspath(root)).replace("\\", "/").rstrip("/")
    out, skip = [], False
    for a in args:
        if skip:
            skip = False
            continue
        if a == "-o":
            skip = True
            continue
        if a == "-c" or a.endswith(rel):
            continue
        for flag in ("-I", "-isystem"):
            if a.startswith(flag) and len(a) > len(flag):
                path = a[len(flag):]
                pn = os.path.normcase(os.path.abspath(path)).replace("\\", "/")
                if pn.startswith(rootn + "/"):
                    moved = os.path.join(tree, pn[len(rootn) + 1:])
                    if os.path.isdir(moved):
                        a = flag + moved.replace("\\", "/")
                break
        out.append(a)
    return out


def compile_obj(cmd, directory, rel, tree, root, out_dir, extra):
    """Compiles tree/rel (the file at its own place in the tree, so relative includes resolve there) into
    out_dir/x.o with the extra flags."""
    os.makedirs(out_dir)
    obj = os.path.join(out_dir, "x.o")
    subprocess.run(tree_args(cmd, rel, tree, root) + extra + ["-c", os.path.join(tree, rel), "-o", obj],
                   cwd=directory, check=True)
    return obj


def gimple_dump(cmd, directory, rel, tree, root, out_dir):
    compile_obj(cmd, directory, rel, tree, root, out_dir, ["-fdump-tree-optimized", "-dumpdir", out_dir + "/"])
    dumps = glob.glob(os.path.join(out_dir, "*.optimized"))
    if not dumps:
        sys.exit("gimple_compare: no dump written for " + rel)
    with open(dumps[0], encoding="utf-8", errors="replace") as fh:
        return fh.read()


INSN = re.compile(r"^\s*[0-9a-f]+:\s+(.*)$")
LABEL = re.compile(r"^[0-9a-f]+ (<.*>:)$")
TARGET = re.compile(r"\b[0-9a-f]+ <([^>+]+)(\+0x[0-9a-f]+)?>")


def objdump_listing(cmd, directory, rel, tree, root, out_dir):
    """Machine code of the object without -g, address-normalised: instruction text without addresses and
    comments, branch targets inside a function as <+offset>, relocations kept, function labels by mangled
    name (a changed signature shows as a changed label)."""
    obj = compile_obj(cmd, directory, rel, tree, root, out_dir, ["-g0"])
    text = subprocess.run(["objdump", "-d", "-r", "--no-show-raw-insn", obj], capture_output=True, text=True,
                          check=True).stdout
    out = []
    for line in text.splitlines():
        m = LABEL.match(line)
        if m:
            out.append(m.group(1))
            continue
        if "R_X86_64" in line or "IMAGE_REL" in line:
            out.append("  RELOC " + " ".join(line.split()[1:]))
            continue
        m = INSN.match(line)
        if m:
            ins = re.sub(r"\s+#.*$", "", m.group(1)).rstrip()
            ins = TARGET.sub(lambda t: "<%s>" % (t.group(2) or t.group(1)), ins)
            out.append("  " + ins)
    return out


# --bool: canonicalisation of Bool8 (uint8_t) vs bool, applied to the raw dump before normalise().
BOOL_CMP_DEF = re.compile(r"^\s*(\S+) = .* (==|!=|<|>|<=|>=) .*;$")
BOOL_CAST = re.compile(r"^\s*(\S+) = \((?:Bool8|bool|unsigned char)\) (\S+);$")
BOOL_IF_EQ0 = re.compile(r"^(\s*)if \((.*) == 0\)$")
BOOL_IF_CMP = re.compile(r"^(\s*)if \((\S+) (==|!=) (\S+)\)$")
BOOL_PROFILE = re.compile(r" \[(?:local count: \d+|\d+\.\d+%|count: \d+)[^\]]*\]")


def bool_canon_function(lines):
    compares, subst, out = set(), {}, []
    for line in lines:
        for k, v in subst.items():
            line = re.sub(r"(?<![\w.])%s(?![\w.])" % re.escape(k), v, line)
        m = BOOL_CMP_DEF.match(line)
        if m:
            compares.add(m.group(1))
        m = BOOL_CAST.match(line)
        if m and m.group(2) in compares:
            subst[m.group(1)] = m.group(2)
            continue
        m = BOOL_IF_CMP.match(line)
        if m and m.group(4) != "0":
            a, b = sorted((m.group(2), m.group(4)))
            line = "%sif (%s %s %s)" % (m.group(1), a, m.group(3), b)
        out.append(line)
    res, i = [], 0
    while i < len(out):
        m = BOOL_IF_EQ0.match(out[i])
        if (m and i + 3 < len(out) and out[i + 1].strip().startswith("goto") and out[i + 2].strip() == "else"
                and out[i + 3].strip().startswith("goto")):
            res += ["%sif (%s != 0)" % (m.group(1), m.group(2)), out[i + 3], out[i + 2], out[i + 1]]
            i += 4
            continue
        res.append(out[i])
        i += 1
    nums = {}
    for line in res:
        m = re.match(r"^\s*<bb (\d+)>:", line)
        if m and m.group(1) not in nums:
            nums[m.group(1)] = str(len(nums) + 2)
    res = [re.sub(r"<bb (\d+)>", lambda m: "<bb %s>" % nums.get(m.group(1), "?" + m.group(1)), l) for l in res]
    return [re.sub(r"\((\d+)\)(?=[,>])", lambda m: "(%s)" % nums.get(m.group(1), "?" + m.group(1)), l)
            if "PHI" in l else l for l in res]


def bool_canon(text):
    text = re.sub(r"\bBool8\b", "bool", text)
    funcs, cur = [], []
    for line in text.splitlines():
        if line.startswith(";; Function"):
            line = re.sub(r"\(_Z\S+", "(", line)
            if cur:
                funcs.append(cur)
                cur = []
        if line.startswith("Removing basic block"):
            continue
        cur.append(BOOL_PROFILE.sub("", line))
    funcs.append(cur)
    return "\n".join(l for f in funcs for l in bool_canon_function(f))


CALL = re.compile(r"\b([A-Za-z_]\w*) \(")
NOT_CALLS = {"if", "PHI", "MEM", "BIT_FIELD_REF", "VIEW_CONVERT_EXPR", "LOAD", "sizeof", "REALPART_EXPR",
             "IMAGPART_EXPR"}


def calls_stores(text):
    """Per function: Counter of 'call F' and 'store TARGET' (SSA numbers masked, indirect calls as one name)."""
    funcs, cur = {}, None
    for line in text.splitlines():
        m = re.match(r";; Function (\S+)", line)
        if m:
            cur = funcs.setdefault(m.group(1), collections.Counter())
            continue
        if cur is None:
            continue
        for c in CALL.findall(line):
            if c not in NOT_CALLS:
                cur["call " + ("<indirect>" if re.fullmatch(r"_\d+", c) else c)] += 1
        m = re.match(r"^\s*(\S.*?) = ", line)
        if m and "CLOBBER" not in line and ("->" in m.group(1) or "MEM" in m.group(1) or
                                            m.group(1).startswith("*") or m.group(1).startswith("g_")):
            cur["store " + re.sub(r"_\d+", "", m.group(1))] += 1
    return funcs


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-p", required=True, help="build directory with compile_commands.json")
    ap.add_argument("--rev", default="HEAD", help="git revision of the reference (default HEAD)")
    ap.add_argument("--alias", action="append", default=[], help='TYPE="gimple type", e.g. MmxPackedValue64="long long unsigned int"')
    ap.add_argument("--objdump", action="store_true",
                    help="compare the machine code of the objects (objdump -d -r, address-normalised) instead of GIMPLE")
    ap.add_argument("--bool", action="store_true", dest="bool_canon",
                    help="canonicalise Bool8/bool representation differences before comparing (GIMPLE mode)")
    ap.add_argument("--calls-stores", action="store_true",
                    help="also compare the per-function multisets of calls and memory stores (GIMPLE mode)")
    ap.add_argument("files", nargs="+")
    a = ap.parse_args()
    aliases = dict(x.split("=", 1) for x in a.alias)
    root = subprocess.run(["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True, check=True).stdout.strip()
    db = json.load(open(os.path.join(a.p, "compile_commands.json")))
    rc = 0
    with tempfile.TemporaryDirectory() as tmp:
        # the reference: src/ and include/ of --rev (its own headers, relative includes next to the file)
        ref_tree = os.path.join(tmp, "ref_tree")
        os.makedirs(ref_tree)
        archive = subprocess.run(["git", "archive", "--format=tar", a.rev, "src", "include"], cwd=root,
                                 capture_output=True, check=True).stdout
        with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
            if hasattr(tarfile, "data_filter"):
                tar.extractall(ref_tree, filter="data")
            else:
                tar.extractall(ref_tree)
        for n, f in enumerate(a.files):
            rel = os.path.relpath(os.path.abspath(f), root).replace("\\", "/")
            entry = next((e for e in db if e["file"].replace("\\", "/").endswith(rel)), None)
            if entry is None:
                sys.exit("gimple_compare: %s not in compile_commands.json" % rel)
            if not os.path.isfile(os.path.join(ref_tree, rel)):
                print("%s: not in %s" % (rel, a.rev))
                rc = 1
                continue
            cmd = entry.get("command") or " ".join(shlex.quote(x) for x in entry["arguments"])
            work = os.path.join(tmp, "f%d" % n)
            if a.objdump:
                ref = objdump_listing(cmd, entry["directory"], rel, ref_tree, root, os.path.join(work, "ref"))
                new = objdump_listing(cmd, entry["directory"], rel, root, root, os.path.join(work, "new"))
                what = "instructions"
            else:
                ref_text = gimple_dump(cmd, entry["directory"], rel, ref_tree, root, os.path.join(work, "ref"))
                new_text = gimple_dump(cmd, entry["directory"], rel, root, root, os.path.join(work, "new"))
                if a.calls_stores:
                    rcs, ncs = calls_stores(ref_text), calls_stores(new_text)
                    bad = sorted(k for k in set(rcs) | set(ncs) if rcs.get(k) != ncs.get(k))
                    if bad:
                        rc = 1
                        print("%s: calls/stores DIFFERENT in %d function(s)" % (rel, len(bad)))
                        for k in bad:
                            r, w = rcs.get(k, collections.Counter()), ncs.get(k, collections.Counter())
                            print("  %s: only ref %s, only new %s" % (k, dict(r - w), dict(w - r)))
                    else:
                        print("%s: calls/stores equal (%d functions)" % (rel, len(ncs)))
                if a.bool_canon:
                    ref_text, new_text = bool_canon(ref_text), bool_canon(new_text)
                ref = normalise(ref_text, aliases)
                new = normalise(new_text, aliases)
                what = "normalised GIMPLE lines" + (" (--bool)" if a.bool_canon else "")
            diff = list(difflib.unified_diff(ref, new, "ref", "new", lineterm="", n=1))
            if diff:
                rc = 1
                print("%s: DIFFERENT (%d %s, %d changed)" % (rel, len(new), what, sum(1 for d in diff if d[:1] in "+-") - 2))
                print("\n".join(diff[:40]))
            else:
                print("%s: equal (%d %s)" % (rel, len(new), what))
    return rc


if __name__ == "__main__":
    sys.exit(main())
