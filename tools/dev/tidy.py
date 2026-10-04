"""Run clang-tidy over a part of the source tree, optionally applying the fixes (developer tool).

    python tools/dev/tidy.py --checks=modernize-use-nullptr [--fix] [-j 6] [-p build-mingw-test] src/core [src/ui/controls ...]

The translation units come from compile_commands.json of a GCC build (the mingw-* presets export it; run
`cmake --preset mingw-test` first). Only the .cpp files below the given path prefixes are checked; diagnostics
and fixes in headers are kept for the headers of the same modules: src/<area>/<module>... also takes
include/thandor/<area>/<module>... (and a prefix below include/ is taken as is).

clang-tidy is run with --target=x86_64-w64-windows-gnu: without it clang assumes the MSVC target on Windows and
parses the GCC command lines against the MSVC headers. With the MinGW target clang finds the GCC 15 headers
itself through the compiler path in compile_commands.json (C:/mingw64/bin/g++.exe).

With --fix every file's fixes are exported to a temporary directory and applied at the end with
clang-apply-replacements, which merges the identical header fixes coming from several translation units.

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


def run_one(args, tool, path, index, fixes_dir, hfilter):
    cmd = [tool, "-p", args.build, f"--checks=-*,{args.checks}", f"--header-filter={hfilter}",
           f"--extra-arg=--target={TARGET}", "--quiet"]
    if fixes_dir:
        cmd.append(f"--export-fixes={os.path.join(fixes_dir, f'{index:04d}.yaml')}")
    cmd.append(path)
    proc = subprocess.run(cmd, capture_output=True, text=True, errors="replace")
    return path, proc.returncode, proc.stdout, proc.stderr


def main():
    parser = argparse.ArgumentParser(description="Run clang-tidy on part of the tree (see the module docstring).")
    parser.add_argument("paths", nargs="+", help="path prefixes, e.g. src/core src/ui/controls")
    parser.add_argument("--checks", required=True, help="clang-tidy check list, e.g. modernize-use-nullptr")
    parser.add_argument("--fix", action="store_true", help="apply the fixes with clang-apply-replacements")
    parser.add_argument("-j", type=int, default=6, help="parallel clang-tidy runs (default 6)")
    parser.add_argument("-p", "--build", default=os.path.join(ROOT, "build-mingw-test"),
                        help="build directory with compile_commands.json (default build-mingw-test)")
    parser.add_argument("--clang-tidy", help="clang-tidy executable")
    parser.add_argument("--verbose", action="store_true", help="print every diagnostic, not only the summary")
    args = parser.parse_args()

    db_path = os.path.join(args.build, "compile_commands.json")
    if not os.path.isfile(db_path):
        sys.exit(f"tidy.py: {db_path} missing; configure a mingw-* preset first")
    with open(db_path, encoding="utf-8") as handle:
        db = json.load(handle)

    prefixes = [os.path.abspath(os.path.join(ROOT, p)) if not os.path.isabs(p) else p for p in args.paths]
    norm_prefixes = [norm(p) for p in prefixes]
    files = sorted({e["file"] for e in db
                    if any(norm(e["file"]) == p or norm(e["file"]).startswith(p + os.sep) for p in norm_prefixes)})
    if not files:
        sys.exit("tidy.py: no translation units below " + " ".join(args.paths))

    tidy = find_tool("clang-tidy", args.clang_tidy)
    apply_tool = find_tool("clang-apply-replacements", near=tidy) if args.fix else None
    hfilter = header_regex(header_prefixes(prefixes))
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
                        warnings[key] = m.group(6) or "clang"
                if code != 0 and len(errors) == errors_before:
                    failed.append((path, (err or out).strip().splitlines()[-1:] or ["?"]))

        by_check = {}
        for check in warnings.values():
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
