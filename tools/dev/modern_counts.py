#!/usr/bin/env python3
"""Step 13 progress counts: the modern-C++ metrics per area.

Counts the occurrences (like ``grep -o``, not matching lines) of the step 13 metrics
(docs/plans/step13_modern_cpp.md, section 9 regexes, one group per item 1-9) in every file
below ``src/`` and ``include/``, split by area: the top-level directory below ``src/`` or
``include/thandor/`` (``include/thandor/<area>.h`` counts for <area>, ``thandor.h`` and
``version.h.in`` for ``root``).

Usage (from the repository root, or pass --root):

    python tools/dev/modern_counts.py                      # table: metric x area
    python tools/dev/modern_counts.py --rev 3de753df       # count a commit instead of the work tree
    python tools/dev/modern_counts.py --json out.json      # also write the counts as a baseline file
    python tools/dev/modern_counts.py --check tools/dev/modern_counts_baseline.json

``--check`` exits with status 1 and names metric and area for every count of a checked metric
that is higher than in the baseline (per area and in total). Metrics marked "info" (all
``#define`` lines, include guards, ``constexpr``, ``enum class``, the ``while`` tails) are
expected to move either way and are not checked. After a wave the coordinator writes a new
baseline with ``--json``.
"""

import argparse
import json
import os
import re
import subprocess
import sys

# (key, item, checked, description, regex) - regexes exactly as in plan section 9.
METRICS = [
    ("cstyle_cast", 1, True, "C-style pointer cast",
     r"(^|[^A-Za-z0-9_])\((const |volatile )?[A-Za-z_][A-Za-z0-9_:<>]*( const)? ?\*+ ?\) ?[A-Za-z_(&*]"),
    ("ingame_ui", 2, True, "INGAME_UI(", r"INGAME_UI\("),
    ("ui_at_sibling", 2, True, "THANDOR_UI_AT/SIBLING(", r"THANDOR_UI_(AT|SIBLING)\("),
    ("frontend_ui", 2, True, "FRONTEND_UI(_FIELD)(", r"FRONTEND_UI(_FIELD)?\("),
    ("define_numeric", 3, True, "object-like numeric #define",
     r"#\s*define\s+\w+\s+\(?-?(0x[0-9A-Fa-f]+|[0-9]+)[uUlL]*\)?\s*(/[*/].*)?$"),
    ("define_all", 3, False, "#define lines (info)", r"^\s*#\s*define"),
    ("include_guard", 3, False, "include guards (info)", r"#\s*define\s+THANDOR_[A-Z0-9_]+_H\s*$"),
    ("constexpr", 3, False, "constexpr (info)", r"\bconstexpr\b"),
    ("typedef_struct", 4, True, "typedef struct", r"typedef struct"),
    ("enum_class", 4, False, "enum class (info, rises)", r"\benum class\b"),
    ("libc_alloc", 5, True, "libc malloc/calloc/realloc/free",
     r"(^|[^.>A-Za-z0-9_])(std::)?(malloc|calloc|realloc|free)\("),
    ("arena", 5, True, "g_MemoryApi.* (arena)", r"g_MemoryApi\.\w+"),
    ("mem_funcs", 5, True, "memset/memcpy/memmove", r"\b(memset|memcpy|memmove)\("),
    ("bool8", 6, True, "Bool8", r"\bBool8\b"),
    ("addr_5xxxxx", 7, True, "0x5xxxxx addresses", r"0x5[0-9A-Fa-f]{5}\b"),
    ("fill_90", 8, True, "0x90909090", r"0x90909090"),
    ("do_while", 9, True, "do {", r"\bdo ?\{"),
    ("while_tail", 9, False, "} while (...); (info)", r"\} ?while ?\(.*\);"),
]

COMPILED = [(key, re.compile(rx)) for key, _, _, _, rx in METRICS]
TOP_DIRS = ("src/", "include/")


def area_of(path):
    """Area of a repository path (forward slashes)."""
    parts = path.split("/")
    if parts[0] == "src" and len(parts) > 2:
        return parts[1]
    if parts[0] == "include" and len(parts) > 2 and parts[1] == "thandor":
        if len(parts) > 3:
            return parts[2]
        stem = parts[2].split(".")[0]
        return "root" if stem in ("thandor", "version") else stem
    return "root"


def worktree_files(root):
    for top in TOP_DIRS:
        base = os.path.join(root, top)
        for dirpath, _, names in os.walk(base):
            for name in names:
                full = os.path.join(dirpath, name)
                rel = os.path.relpath(full, root).replace(os.sep, "/")
                with open(full, "rb") as f:
                    yield rel, f.read()


def rev_files(root, rev):
    names = subprocess.run(["git", "-C", root, "ls-tree", "-r", "--name-only", rev, "--", "src", "include"],
                           check=True, capture_output=True).stdout.decode().split("\n")
    names = [n for n in names if n]
    proc = subprocess.Popen(["git", "-C", root, "cat-file", "--batch"], stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE)
    for name in names:
        proc.stdin.write(f"{rev}:{name}\n".encode())
        proc.stdin.flush()
        header = proc.stdout.readline().split()
        size = int(header[2])
        data = proc.stdout.read(size)
        proc.stdout.read(1)
        yield name, data
    proc.stdin.close()
    proc.wait()


def count(files):
    """Returns {metric: {area: n}} and the number of files."""
    result = {key: {} for key, _ in COMPILED}
    nfiles = 0
    for path, data in files:
        nfiles += 1
        # Line by line like grep (so `\s` never crosses a line end); CRLF and CR end lines too.
        lines = data.decode("utf-8", errors="replace").splitlines()
        area = area_of(path)
        for key, rx in COMPILED:
            n = sum(1 for line in lines for _ in rx.finditer(line))
            if n:
                result[key][area] = result[key].get(area, 0) + n
    return result, nfiles


def print_table(counts):
    areas = sorted({a for per in counts.values() for a in per})
    head = ["metric", "item"] + areas + ["total"]
    rows = []
    for key, item, checked, _, _ in METRICS:
        per = counts[key]
        rows.append([key + ("" if checked else " *"), str(item)] + [str(per.get(a, 0)) for a in areas]
                    + [str(sum(per.values()))])
    widths = [max(len(r[i]) for r in rows + [head]) for i in range(len(head))]
    fmt = lambda r: "  ".join(c.ljust(w) if i == 0 else c.rjust(w) for i, (c, w) in enumerate(zip(r, widths)))
    print(fmt(head))
    for r in rows:
        print(fmt(r))
    print("(* = info only, not checked)")


def check(counts, baseline_path):
    with open(baseline_path, encoding="utf-8") as f:
        base = json.load(f)["counts"]
    failures = []
    for key, _, checked, _, _ in METRICS:
        if not checked:
            continue
        now = counts[key]
        old = base.get(key, {})
        for area in sorted(set(now) | set(old)):
            if now.get(area, 0) > old.get(area, 0):
                failures.append(f"{key} in {area}: {old.get(area, 0)} -> {now.get(area, 0)}")
        if sum(now.values()) > sum(old.values()):
            failures.append(f"{key} total: {sum(old.values())} -> {sum(now.values())}")
    for line in failures:
        print("ROSE: " + line)
    if not failures:
        print(f"check: no checked count rose against {baseline_path}")
    return 1 if failures else 0


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--root", default=os.path.normpath(os.path.join(os.path.dirname(__file__), "..", "..")),
                    help="repository root (default: this script's repository)")
    ap.add_argument("--rev", help="count this git revision instead of the work tree")
    ap.add_argument("--json", metavar="FILE", help="write the counts to FILE (baseline format)")
    ap.add_argument("--check", metavar="BASELINE", help="exit 1 if a checked count rose against BASELINE")
    args = ap.parse_args()

    files = rev_files(args.root, args.rev) if args.rev else worktree_files(args.root)
    counts, nfiles = count(files)
    print(f"{nfiles} files below src/ and include/" + (f" at {args.rev}" if args.rev else ""))
    print_table(counts)
    if args.json:
        rev = args.rev or subprocess.run(["git", "-C", args.root, "rev-parse", "--short", "HEAD"],
                                         capture_output=True, text=True).stdout.strip()
        data = {"revision": rev, "worktree": not args.rev, "files": nfiles,
                "metrics": {k: {"item": i, "checked": c, "description": d, "regex": r}
                            for k, i, c, d, r in METRICS},
                "counts": {k: dict(sorted(v.items())) for k, v in counts.items()}}
        with open(args.json, "w", encoding="utf-8", newline="\n") as f:
            json.dump(data, f, indent=1)
            f.write("\n")
    if args.check:
        return check(counts, args.check)
    return 0


if __name__ == "__main__":
    sys.exit(main())
