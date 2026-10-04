# Step 8: code review and idiomatic C++

Status: in progress (started 2026-10-04). Step 8 combines a full code review of every area with the move to
idiomatic C++ following the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).
The work is done in many small packages (about 15 minutes of agent work each, one file group and one kind of change
per package), several at a time in separate worktrees, merged in waves; after every wave both compilers build and
`tools/test/run_checks.py` must pass.

## Ground rules

Every package keeps these unchanged:

- **Formats:** PCK/LEV/FLD/MDL/ROM and the other data files, the savegame format, the network protocol (multiplayer
  with the original game), and every struct layout asserted in `src/core/layout_checks.cpp` (32-bit pointer fields
  through `Ptr32<T>`, UI template images with their vtable slots).
- **Simulation:** bit-identical results (determinism state hash, AI hash): no reordering of `Random_*` calls, of
  iteration or tie orders, of integer/fixed-point arithmetic; no floating-point reordering.
- **Arena allocation order and sizes** on valid paths: block addresses are used as sort keys (texture sets, opaque
  packets), so moving an allocation or a free changes pixels and possibly the simulation. RAII is only used where it
  frees at exactly the same point as today.
- **Original quirks** stay and are marked "Original quirk" - except where a quirk is memory corruption or a crash;
  those are bounded and the comment says "The original ...; bounded here because ...".
- **Kept on purpose** although unused: the SAM encoder (future sound tool), the developer map editor, the converter
  tools (TXT2STR compiler, palette optimiser).
- **Not adopted:** namespaces (huge diffs for little gain while the code is module-prefixed functions), virtual
  functions for the UI and model node types (their vtables are 32-bit slots inside data images), `std::sort` instead
  of the original sorts (tie order).

## Review

Eight area reviews (assets, audio/movie, core, gameplay, graphics, network/platform, ui, world; the larger ones split
into sub-reviews) read the whole code base read-only. Their reports stay outside the repository (working notes);
the findings are summarised here and turned into the packages below.

Main findings:

1. **Peer input in multiplayer was trusted.** Received packets and command records are used as sizes, indices and
   pointers without checks: lobby and in-game chat writes past their buffers, player names carry rich-text commands
   that make the client follow arbitrary pointers, player ids/faction indices/army tokens index tables or turn into
   pointers, editor commands write outside the field grid, a tick interval of 0 divides by zero, chunk requests read
   out-of-bounds data and send it back, the arena allocator turns huge peer sizes into 0-byte blocks. The original
   game trusted its peers the same way. Fix: validation at the receiving side and one validator at the command
   dispatch (the host stamps the real sender id), rejecting only what valid peers never send - the wire format and
   valid games are unchanged.
2. **File input was trusted.** PCK directories, field grids, Huffman streams, gfx/pal/MDL/ROM headers, LEV scripts
   and savegames drive sizes, loops and indices unchecked.
3. **x64 leftovers.** Arrays of pointers sized with 4 bytes per entry, an 8-byte HKEY written into a 4-byte variable,
   empty-list navigation storing a pointer 16 GiB off (stopped the game through the Ptr32 overflow check), pointer
   values packed into 32-bit integers.
4. **Threads.** SDL timer callbacks and the main thread share counters and spin locks without atomics; the spin-lock
   release is a plain store.
5. **Duplicates and dead code.** The 84 hex-direction walkers (~2,500 lines) are one algorithm; six copies of
   key-command matching; "call locally or queue the command" ~100 times; settings handlers in frontend and in-game;
   unused control classes (tree list, numeric/path text edits), unused vtables and blits.
6. **Decompiler readability leftovers.** Raw byte offsets where named fields exist, magic numbers, names that say the
   opposite of what the code does, stale comments naming `.c` files that no longer exist.

## Waves

| Wave | Content | State |
|---|---|---|
| 1 | Crashes and file/peer input: codec (field grid, Huffman), PCK directory and decoder index, empty-list navigation, savegame page, HUD buffers; registry HKEY on x64 | done, all checks pass |
| 2 | Lobby chat and player bounds, lobby receive validation, rich-text font/palette bounds, session x64 heap overflows, world crash bounds, arena size cap | done, all checks pass |
| 3 | Command validator at the dispatch (network), frontend peer indices, editor commands (terrain), field-grid load validation, UI controls crashes, movie close race and FLM header checks, graphics bounds | done, all checks pass |
| 4 | Threads: atomic counters, spin lock with `std::atomic_ref` and a guard, timer unregister | done, all checks pass |
| 5 | Test aids first: raster/blit golden-hash self-test, sine/fixed-math table hash, SAM decode hash | done (`raster`, `tables`, `sam` self-tests; GCC and MSVC give the same hashes) |
| 6+ | Idiomatic C++ in small mechanical packages: `nullptr`, `const`, `[[nodiscard]]`, `constexpr` tables, C casts to named casts, flag typedefs `int` to fixed-width unsigned, typed vtable slots (`UI_SLOT`) instead of type-erasing casts, `std::span` for pointer+count, RAII guards for spin locks/framebuffer access/focus lending, typed accessors instead of raw offsets, `enum class` where a value is a pure enumeration | started: clang-tidy wrapper `tools/dev/tidy.py`, `nullptr` everywhere in `src` (done); redundant `(void)` and `typedef` to `using` (in work) |
| later | Larger merges: hex walker template (family by family, determinism check per family), key-command matching, settings handlers, lockstep channel, new vs loaded session | queued |

Each package lists its files, the change, why valid data stays identical, and the checks that cover it.

## Owner decisions (2026-10-04)

- Scope: step 8 covers every security and crash finding of the review, the cosmetic clean-up (dead code, stale
  comments), the global mechanical packages, typed slots, the hex walker template and the larger merges (key commands,
  local-or-queue, settings, sessions, lockstep). The remaining idiomatic backlog (accessors, value types, RAII and
  `enum class` everywhere, class-based subsystems) becomes a later step of its own.
- Kept tools: `texture_decompose.cpp` and the FLM encoder join the SAM encoder, map editor, TXT2STR and palette
  optimiser; the `scanaddr` self-test is removed.
- Behaviour changes allowed as fixes: `EDITOR_SAVE_MAP` from a peer is ignored unless the local editor is active;
  high instead of realtime process priority; the archive writer writes a temporary file and replaces the target;
  a locked savegame shows an error instead of exiting; PCK archives are checked for their magic on mount.
- The inert text override table is removed with a same-size placeholder allocation so the arena order stays.
