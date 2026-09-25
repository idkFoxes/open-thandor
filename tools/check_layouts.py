"""Compare C struct layouts with the Ghidra program database.

Generates a probe program that prints sizeof()/offsetof() for every struct and union in
ghidra/export/layouts.jsonl, builds it with the same compiler as the project (32-bit MSVC),
runs it and reports every type whose size or field offsets differ from Ghidra's.

Usage: python tools/check_layouts.py <vcvars32.bat>
"""
import json
import pathlib
import re
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parent.parent


def main(vcvars):
    layouts = [json.loads(l) for l in (ROOT / "ghidra/export/layouts.jsonl").read_text(encoding="utf-8").splitlines()]
    types_h = (ROOT / "include/thandor/generated/types.h").read_text(encoding="utf-8")
    declared = {n: k for k, n in re.findall(r"^(struct|union) (\w+) \{", types_h, re.M)}
    probe = ["#include <stdio.h>", "#include <stddef.h>", "#include <thandor/thandor.h>", "int main(void) {"]
    checked = []
    for d in layouts:
        if d["kind"] == "enum" or d["name"] not in declared:
            continue
        kw = declared[d["name"]]  # the C keyword; Ghidra may disagree (reported below)
        probe.append(f'  printf("S {d["name"]} %u\\n", (unsigned)sizeof({kw} {d["name"]}));')
        m = re.search(r"^" + kw + r" " + d["name"] + r" \{\n(.*?)^\}", types_h, re.S | re.M)
        body = m.group(1) if m else ""
        c_fields = set(re.findall(r"(\w+)(?:\[[^\]]*\])*\s*(?::\s*\d+)?\s*;", re.sub(r"//[^\n]*", "", body)))
        for f in d["fields"]:
            if f["bitfield"] or not f["name"] or f["name"] not in c_fields:
                continue
            probe.append(f'  printf("F {d["name"]} {f["name"]} %u\\n", (unsigned)offsetof({kw} {d["name"]}, {f["name"]}));')
        checked.append(d)
    probe += ["  return 0;", "}", ""]
    work = pathlib.Path(tempfile.mkdtemp())
    (work / "probe.c").write_text("\n".join(probe), encoding="utf-8")
    (work / "build.bat").write_text(
        f'@echo off\r\ncall "{vcvars}" >nul\r\n'
        f'cl /nologo /std:c11 /w /I"{ROOT / "include"}" "{work / "probe.c"}" /Fe"{work / "probe.exe"}" '
        f'/Fo"{work / "probe.obj"}" /link /SUBSYSTEM:CONSOLE > "{work / "cl.log"}"\r\n', encoding="ascii")
    subprocess.run(["cmd", "/c", str(work / "build.bat")], check=False)
    exe = work / "probe.exe"
    if not exe.exists():
        print((work / "cl.log").read_text(errors="replace")[:4000])
        return
    out = subprocess.run([str(exe)], capture_output=True, text=True).stdout.splitlines()
    size = {}
    offset = {}
    for line in out:
        parts = line.split()
        if parts[0] == "S":
            size[parts[1]] = int(parts[2])
        else:
            offset[(parts[1], parts[2])] = int(parts[3])
    bad = 0
    for d in checked:
        problems = []
        if size.get(d["name"]) != d["length"]:
            problems.append(f"size C={size.get(d['name'])} Ghidra={d['length']}")
        for f in d["fields"]:
            k = (d["name"], f["name"])
            if k in offset and offset[k] != f["offset"]:
                problems.append(f"{f['name']} C=+0x{offset[k]:X} Ghidra=+0x{f['offset']:X}")
                break  # later fields shift with the first difference
        if problems:
            bad += 1
            print(f"{d['kind']} {d['name']}: " + "; ".join(problems))
    print(f"{bad} of {len(checked)} layouts differ")


if __name__ == "__main__":
    main(sys.argv[1])
