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
| `include/thandor/core/ghidra.h` | hand-written | `CONCATxy`, `SUBxy`, `CARRYx`, partial access, `THANDOR_BITCAST`, `THANDOR_CONTAINER_OF`. |

One-shot rewriters used on `src/` (safe to re-run on a fresh decompiler export):

- `tools/fix_partial_access.py` — `x._off_size_` → `THANDOR_PART` / `THANDOR_READ_PART` / `THANDOR_WRITE_PART`
- `tools/fix_abi_casts.py <msvc.log>` — register-image struct casts flagged by C2440 → `THANDOR_BITCAST`
- `tools/fix_stack_refs.py` — leftover `stack0x...` slots → per-function `thandor_stack_frame`

## Known TODOs (compiles, but will not run correctly yet)

Search for `TODO` in the tree. The main groups:

- **Unrecovered function-pointer signatures** (`globals.h`, `TODO: unrecovered signature`): Ghidra does not export
  FunctionDefinition types; these are unprototyped `dword Name()` placeholders.
- **String literals** (`src/generated/globals.c`): text reconstructed from Ghidra labels; verify against `thandor.exe`.
- **Code-relative dispatch** (`THANDOR_CODE_AT`, absolute table `0x563748`): needs a real handler table.
- **Stack frames** (`thandor_stack_frame`): `richtext.c`, `lists.c`, `scenario/catalog.c` have unrecovered stack locals.
- **Variadic UI group helpers** (`UiSelectableGroup_*`): callers do not pass the control list yet.
