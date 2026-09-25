# Building

The tree builds as a 32-bit static library (`thandor_curated.lib`) with MSVC.

```bat
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat"
cmake -S . -B build-x86 -G Ninja -DCMAKE_C_COMPILER=cl
cmake --build build-x86
```

There is no executable target yet; linking one needs the TODO items below.

## Generated files and tools

| File | Produced by | Notes |
|---|---|---|
| `include/thandor/generated/types.h` | Ghidra export, ordered by `tools/sort_types.py` | Re-run the script after pasting a new export. |
| `include/thandor/generated/globals.h`, `src/generated/globals.c` | `python tools/gen_globals.py ghidra/thandor.exeV537.c` | Globals, recovered function-pointer signatures (`tools/infer_signatures.py`), Ghidra-invented symbols. |
| `ghidra/export/*.jsonl` | `tools/ghidra/ExportBuildData.java` (headless, see below) | Function-signature types, string values and data symbols that Ghidra's C export omits. Committed, so regenerating does not need Ghidra. |
| `include/thandor/core/ghidra.h` | hand-written | `CONCATxy`, `SUBxy`, `CARRYx`, partial access, `THANDOR_BITCAST`, `THANDOR_CONTAINER_OF`. |

One-shot rewriters used on `src/` (safe to re-run on a fresh decompiler export):

- `tools/fix_partial_access.py` — `x._off_size_` → `THANDOR_PART` / `THANDOR_READ_PART` / `THANDOR_WRITE_PART`
- `tools/fix_abi_casts.py <msvc.log>` — register-image struct casts flagged by C2440 → `THANDOR_BITCAST`
- `tools/fix_stack_refs.py` — leftover `stack0x...` slots → per-function `thandor_stack_frame`

## Refreshing the Ghidra export

Needs Ghidra 12 and JDK 21+ (`JAVA_HOME`):

```bat
"%GHIDRA_HOME%\support\analyzeHeadless.bat" %TEMP%\ghproj thandor -import ghidra\thandor.exeV537.gzf -noanalysis ^
    -scriptPath tools\ghidra -postScript ExportBuildData.java ghidra\export -deleteProject
python tools\gen_globals.py ghidra\thandor.exeV537.c
```

## Known TODOs (compiles, but will not run correctly yet)

Search for `TODO` in the tree. The main groups:

- **Function-pointer signatures** come from the Ghidra export. `__stdcall` is kept only for DLL imports (Glide,
  WinSock); the game's own functions are plain C here. A few call sites disagree with the recovered signature
  (extra stale-register arguments, or consuming EAX of a `void` callee) and are commented where fixed.
- **Five string literals** (`TODO: verify text` in `src/generated/globals.c`) have no string data in the program
  database and are still reconstructed from their labels.
- **Code-relative dispatch** (`THANDOR_CODE_AT`, absolute table `0x563748`): needs a real handler table.
- **Stack frames** (`thandor_stack_frame`): `richtext.c`, `lists.c`, `scenario/catalog.c` have unrecovered stack locals.
- **Variadic UI group helpers** (`UiSelectableGroup_*`): callers do not pass the control list yet.
