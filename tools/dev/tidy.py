"""Run clang-tidy over a part of the source tree, optionally applying the fixes (developer tool).

    python tools/dev/tidy.py --checks=modernize-use-nullptr [--fix] [-j 6] [-p build-mingw-test] src/core [src/ui/controls ...]
    python tools/dev/tidy.py --report [--output build/tidy-report.txt] [-p build-mingw-test] [src/ui/ingame ...]

The translation units come from compile_commands.json of a GCC build (the mingw-* presets export it; run
`cmake --preset mingw-test` first). Only the .cpp files below the given path prefixes are checked; diagnostics
and fixes in headers are kept for the headers of the same modules: src/<area>/<module>... also takes
include/thandor/<area>/<module>... (and a prefix below include/ is taken as is); a single src/.../x.cpp takes its
own x.h (next to it or below include/thandor).

clang-tidy is run with --target=x86_64-w64-windows-gnu: without it clang assumes the MSVC target on Windows and
parses the GCC command lines against the MSVC headers. With the MinGW target clang finds the GCC 15 headers
itself through the compiler path in compile_commands.json (C:/mingw64/bin/g++.exe).

With --fix every file's fixes are exported to a temporary directory and applied at the end with
clang-apply-replacements, which merges the identical header fixes coming from several translation units.

With --report the checks of the repository's .clang-tidy (the step 13 report set) run over all of src/ (or the
given prefixes); the distinct diagnostics are counted per check, per area (src/<area> and include/thandor/<area>)
and per file and written to build/tidy-report.txt (--output), the totals are printed. Without path prefixes the
headers are filtered by the .clang-tidy HeaderFilterRegex (all of ours), with prefixes only the headers of those
modules count, as above. --checks overrides the configured check list. A report never applies fixes.

The tools are found on the PATH, next to each other, or in the pip package `clang-tidy`
(pip install --user clang-tidy); --clang-tidy overrides.
"""
import argparse
import concurrent.futures
import importlib.util
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
TARGET = "x86_64-w64-windows-gnu"


def find_tool(name, explicit=None, near=None):
    if explicit:
        return explicit
    exe = name + (".exe" if os.name == "nt" else "")
    if near and os.path.isfile(os.path.join(os.path.dirname(near), exe)):
        return os.path.join(os.path.dirname(near), exe)
    found = shutil.which(name)
    if found:
        return found
    spec = importlib.util.find_spec("clang_tidy")
    if spec and spec.origin:
        candidate = os.path.join(os.path.dirname(spec.origin), "data", "bin", exe)
        if os.path.isfile(candidate):
            return candidate
    sys.exit(f"tidy.py: {name} not found (PATH or pip package clang-tidy)")


def norm(path):
    return os.path.normcase(os.path.normpath(path))


def header_prefixes(prefixes):
    """The source prefixes plus the include/thandor mirror of each src/ prefix."""
    result = []
    for prefix in prefixes:
        result.append(prefix)
        rel = os.path.relpath(prefix, ROOT).replace("\\", "/")
        if rel == "src":
            result.append(os.path.join(ROOT, "include"))
        elif rel.startswith("src/"):
            if rel.endswith(".cpp"):
                # a single source file: its own header next to it or under include/thandor
                rel = rel[:-4]
                if os.path.isfile(os.path.join(ROOT, rel + ".h")):
                    result.append(os.path.join(ROOT, rel + ".h"))
            mirror = os.path.join(ROOT, "include", "thandor", rel[4:])
            result.append(mirror)
            if os.path.isfile(mirror + ".h"):
                result.append(mirror + ".h")
    return result


def header_regex(prefixes):
    """A --header-filter regex matching files below the prefixes (either slash, any case).

    clang-tidy's regex is POSIX ERE (llvm::Regex): an inline flag like (?i) makes it match no header at all, so
    the case insensitivity is spelled out as [xX] classes."""
    def nocase(piece):
        return "".join(f"[{c.lower()}{c.upper()}]" if c.isalpha() else re.escape(c) for c in piece)

    parts = []
    for prefix in prefixes:
        rel = os.path.relpath(prefix, ROOT).replace("\\", "/")
        pieces = [nocase(p) for p in rel.split("/")]
        parts.append(r"[\\/]".join(pieces) + (r"$" if rel.endswith(".h") else r"([\\/]|$)"))
    return r".*[\\/](" + "|".join(parts) + ")"


def area_of(path):
    """The area a file belongs to: src/<area>/... and include/thandor/<area>/... give <area>."""
    parts = os.path.relpath(path, ROOT).replace("\\", "/").split("/")
    if len(parts) > 2 and parts[0] == "src":
        return parts[1]
    if len(parts) > 3 and parts[0] == "include" and parts[1] == "thandor":
        return parts[2]
    return "other"


def write_report(path, args, files, warnings, errors, failed):
    """The --report summary: totals, per check, per area (with the checks), per file."""
    by_check = {}
    by_area = {}
    by_file = {}
    for key, names in warnings.items():
        area = area_of(key[0])
        rel = os.path.relpath(key[0], ROOT).replace("\\", "/")
        for check in names:
            by_check[check] = by_check.get(check, 0) + 1
            per_area = by_area.setdefault(area, {})
            per_area[check] = per_area.get(check, 0) + 1
            per_file = by_file.setdefault(rel, {})
            per_file[check] = per_file.get(check, 0) + 1

    lines = ["clang-tidy report (tools/dev/tidy.py --report)",
             f"checks: {args.checks or 'from .clang-tidy'}",
             f"paths: {' '.join(args.paths) if args.paths else 'src'}",
             f"{len(files)} translation units, {len(warnings)} distinct diagnostics in {len(by_file)} files, "
             f"{len(errors)} errors, {len(failed)} failed runs",
             "", "per check:"]
    for check, count in sorted(by_check.items(), key=lambda kv: (-kv[1], kv[0])):
        lines.append(f"  {count:7d}  {check}")
    lines += ["", "per area (total, then per check):"]
    for area in sorted(by_area):
        lines.append(f"  {area:10s} {sum(by_area[area].values()):7d}")
        for check, count in sorted(by_area[area].items(), key=lambda kv: (-kv[1], kv[0])):
            lines.append(f"      {count:7d}  {check}")
    lines += ["", "per file (total: per check):"]
    for rel in sorted(by_file):
        counts = by_file[rel]
        detail = ", ".join(f"{c} {n}" for c, n in sorted(counts.items()))
        lines.append(f"  {sum(counts.values()):6d}  {rel}: {detail}")
    if errors:
        lines += ["", "errors (parse problems):"] + ["  " + e for e in errors]
    for p, tail in failed:
        lines.append(f"clang-tidy failed on {os.path.relpath(p, ROOT)}: {tail[0]}")
    os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
    with open(path, "w", encoding="utf-8", newline="\n") as handle:
        handle.write("\n".join(lines) + "\n")


def run_one(args, tool, path, index, fixes_dir, hfilter):
    cmd = [tool, "-p", args.build, f"--extra-arg=--target={TARGET}", "--quiet"]
    if args.checks:
        cmd.append(f"--checks=-*,{args.checks}")
    if hfilter:
        cmd.append(f"--header-filter={hfilter}")
    if args.report:
        cmd.append(f"--config-file={os.path.join(ROOT, '.clang-tidy')}")
    if fixes_dir:
        cmd.append(f"--export-fixes={os.path.join(fixes_dir, f'{index:04d}.yaml')}")
    cmd.append(path)
    proc = subprocess.run(cmd, capture_output=True, text=True, errors="replace")
    return path, proc.returncode, proc.stdout, proc.stderr


def main():
    parser = argparse.ArgumentParser(description="Run clang-tidy on part of the tree (see the module docstring).")
    parser.add_argument("paths", nargs="*", help="path prefixes, e.g. src/core src/ui/controls (--report: default src)")
    parser.add_argument("--checks", help="clang-tidy check list, e.g. modernize-use-nullptr (--report: default the "
                                         "checks of .clang-tidy)")
    parser.add_argument("--fix", action="store_true", help="apply the fixes with clang-apply-replacements")
    parser.add_argument("--report", action="store_true",
                        help="count the .clang-tidy checks per check, area and file and write --output")
    parser.add_argument("--output", default=os.path.join(ROOT, "build", "tidy-report.txt"),
                        help="report file (default build/tidy-report.txt)")
    parser.add_argument("-j", type=int, default=6, help="parallel clang-tidy runs (default 6)")
    parser.add_argument("-p", "--build", default=os.path.join(ROOT, "build-mingw-test"),
                        help="build directory with compile_commands.json (default build-mingw-test)")
    parser.add_argument("--clang-tidy", help="clang-tidy executable")
    parser.add_argument("--verbose", action="store_true", help="print every diagnostic, not only the summary")
    args = parser.parse_args()
    if args.report and args.fix:
        parser.error("--report never applies fixes; run --fix with --checks separately")
    if not args.report and (not args.checks or not args.paths):
        parser.error("--checks and at least one path prefix are required (or use --report)")

    db_path = os.path.join(args.build, "compile_commands.json")
    if not os.path.isfile(db_path):
        sys.exit(f"tidy.py: {db_path} missing; configure a mingw-* preset first")
    with open(db_path, encoding="utf-8") as handle:
        db = json.load(handle)

    prefixes = [os.path.abspath(os.path.join(ROOT, p)) if not os.path.isabs(p) else p for p in (args.paths or ["src"])]
    norm_prefixes = [norm(p) for p in prefixes]
    files = sorted({e["file"] for e in db
                    if any(norm(e["file"]) == p or norm(e["file"]).startswith(p + os.sep) for p in norm_prefixes)})
    if not files:
        sys.exit("tidy.py: no translation units below " + " ".join(args.paths or ["src"]))

    tidy = find_tool("clang-tidy", args.clang_tidy)
    apply_tool = find_tool("clang-apply-replacements", near=tidy) if args.fix else None
    # A full report filters the headers by .clang-tidy (all of ours); otherwise only the given modules' headers.
    hfilter = None if args.report and not args.paths else header_regex(header_prefixes(prefixes))
    fixes_dir = tempfile.mkdtemp(prefix="tidy-fixes-") if args.fix else None

    warnings = {}
    errors = []
    failed = []
    diag = re.compile(r"^(.*?):(\d+):(\d+): (warning|error): (.*?)(?: \[([\w.,-]+)\])?$")
    try:
        with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, args.j)) as pool:
            futures = [pool.submit(run_one, args, tidy, f, i, fixes_dir, hfilter) for i, f in enumerate(files)]
            for done, future in enumerate(concurrent.futures.as_completed(futures), 1):
                path, code, out, err = future.result()
                print(f"[{done}/{len(files)}] {os.path.relpath(path, ROOT)}", flush=True)
                errors_before = len(errors)
                for line in out.splitlines():
                    m = diag.match(line)
                    if not m:
                        continue
                    if args.verbose:
                        print("  " + line)
                    if m.group(4) == "error":
                        errors.append(line)
                    else:
                        key = (norm(m.group(1)), m.group(2), m.group(3), m.group(6) or "")
                        warnings[key] = (m.group(6) or "clang").split(",")
                if code != 0 and len(errors) == errors_before:
                    failed.append((path, (err or out).strip().splitlines()[-1:] or ["?"]))

        by_check = {}
        for names in warnings.values():
            for check in names:
                by_check[check] = by_check.get(check, 0) + 1
        by_file = {}
        for key in warnings:
            by_file[key[0]] = by_file.get(key[0], 0) + 1

        print()
        print(f"{len(files)} translation units, {len(warnings)} distinct diagnostics in {len(by_file)} files")
        for check, count in sorted(by_check.items(), key=lambda kv: -kv[1]):
            print(f"  {count:6d}  {check}")
        if errors:
            print(f"{len(errors)} errors (parse problems):")
            for line in errors[:20]:
                print("  " + line)
        for path, tail in failed:
            print(f"clang-tidy failed on {os.path.relpath(path, ROOT)}: {tail[0]}")
        if args.report:
            write_report(args.output, args, files, warnings, errors, failed)
            print(f"report written to {args.output}")

        if args.fix:
            if errors:
                print("not applying fixes: there were errors")
                return 1
            proc = subprocess.run([apply_tool, "--remove-change-desc-files", fixes_dir], capture_output=True, text=True)
            if proc.returncode != 0:
                print(proc.stdout + proc.stderr)
                return 1
            print("fixes applied")
    finally:
        if fixes_dir:
            shutil.rmtree(fixes_dir, ignore_errors=True)
    return 1 if errors or failed else 0


if __name__ == "__main__":
    sys.exit(main())
