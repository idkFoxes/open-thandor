# Step 13 (later): modern C++ - typed UI, named casts, constants, enums, RAII, bool, loops

Status: done (2026-10-07; final check W7: all eight run_checks pass (maps only the known stromschnelle
WARN), gpu_compare Vulkan and D3D12 23/23, GCC and MSVC without new warnings, all self-tests identical, a
pre-step-13 savegame loads). Step 13 takes the idiomatic-C++ backlog that step 8 left for "a later step of its own"
([step 8 owner decisions](step8_idiomatic_cpp.md#owner-decisions-2026-10-04)): the decompiler's C-style casts, the
UI reached through byte offsets into template images, `#define` constants, `typedef struct`, raw allocation,
`Bool8`, the original key-dispatch addresses, the 0x90 fill leftovers and the decompiled `do { } while` counting
loops. Nothing in this step changes behaviour: every package is a refactoring whose result the existing checks can
compare.

This plan is a read-only analysis of `dev` at 3de753df (683 files, 165,760 lines in `src/` and `include/`). Counts
were taken with `grep` (the regexes are in section 9); line numbers refer to that commit and will move. Items marked
**(guess)** are estimates, not measured. The owner accepted all recommendations of section 2 (D1-D9) on 2026-10-06.

## 1. Ground rules

The ground rules of [step 8](step8_idiomatic_cpp.md#ground-rules) apply unchanged. In short:

- **Formats** stay byte-identical: PCK/LEV/FLD/MDL/ROM and the other data files, the savegame (`.sve`), the network
  protocol with the original game, and every layout asserted in `src/core/layout_checks.cpp`.
- **Simulation** stays bit-identical: no `Random_*` call moved, no iteration or tie order changed, no arithmetic
  changed (also not its width or signedness: `int` vs `unsigned` promotions of a former `#define` are arithmetic).
- **Arena order and sizes** stay: `g_MemoryApi.alloc/free` keep their order and sizes on every valid path (block
  addresses are sort keys for texture sets and opaque packets).
- **Original quirks** stay and stay marked "Original quirk".
- **Not adopted** (unchanged since step 8): namespaces, virtual functions for UI and model node types (their vtables
  are 32-bit slots inside data images), `std::sort` instead of the original sorts.

New for step 13:

- **One kind of change per package, one file group per package**, about 15 minutes of agent work (S) and never more
  than about 30 (M); bigger items are split until they fit ([agent sizing](#7-work-packages)). Within a wave no two
  packages touch the same file, except the token-only replacements named in the wave.
- **Layout structs change only in layout-neutral ways** (section 3.3): a member may get a different *type* of the
  same size and alignment (a typed control instead of `UiNodeBase` + `uint32_t fields[N]`, an `enum class` with the
  same fixed underlying type, a typed `Ptr32<T>` instead of an untyped one); nothing is reordered, resized or
  removed, and `layout_checks.cpp` is extended, never relaxed.
- **Work mode** (as in steps 8 and 9): many small agent packages run in parallel, each in its own worktree on its
  own branch; a coordinator merges them wave by wave into the step branch and runs the gates. Agents never push and
  never commit in the main checkout; builds are staggered (CPU limit about 80 %). A running agent gets no extra
  tasks - a new small package is started instead.
- **No game tests during the step** (owner rule since step 8): between merges only the GCC build, the MSVC build
  (0 new warnings), the self-tests against the reference build and `golden_cmp.ps1`. The full
  `tools/test/run_checks.py` runs once at the end of the step, with the game windows minimized.

## 2. Owner decisions

Decided 2026-10-06: every recommendation below (D1-D9) is accepted as written.

| # | Question | Options | Recommendation |
|---|---|---|---|
| D1 | **How far the UI object model goes** (item 2) | (a) typed members in the five `*UiImage` template structs, so `INGAME_UI(rt, node)` returns the control's real type and the casts disappear; same bytes, same template copy and relocation. (b) (a) plus C++ inheritance between the control structs (`UiSpriteButtonControl : UiSelectableControl : UiNodeBase`) instead of the embedded `selectable.base.` chains; layout-neutral under the same packing, but every `.selectable.base.x` access changes. (c) real objects: constructors instead of the dword copy of a template image, virtual functions instead of the 32-bit vtable slots. | **(a) in step 13**, (b) as an optional later wave, (c) not at all while the 32-bit vtable slots and template images exist (it contradicts the step 8 "not adopted" rule and would touch every control; L-XL, high pixel risk). |
| D2 | **Touch layout structs at all?** | (a) only layout-neutral member *type* changes guarded by `layout_checks.cpp` (rule above). (b) no changes to any asserted struct (then items 2, 4-enum-fields and the typed `Ptr32` casts stay half done). | **(a)**. |
| D3 | **`Bool8` in layout structs and file data** | keep `Bool8` (`uint8_t`) for the two struct fields and every byte that comes from a file, savegame or packet; or use `bool` there too. A `bool` byte holding 2..255 (a corrupted or original savegame) is undefined behaviour. | **Keep `Bool8` only there**, `bool` everywhere else; `Bool8` stays as the documented "byte that is a flag in a format" type. |
| D4 | **clang-tidy** | clang-tidy 22.1.8 is installed as the pip package (`clang_tidy`, with `clang-apply-replacements`), and VS 18 ships clang-tidy 22.1.3 in `VC/Tools/Llvm/x64/bin`; there is **no clang compiler** (no `clang`, `clang++`, `clang-cl`). `tools/dev/tidy.py` already runs it on the GCC `compile_commands.json`. Options: (a) use it per package as a finder and fixer (`modernize-macro-to-enum`, `cppcoreguidelines-pro-type-cstyle-cast`, `google-readability-casting`, `readability-implicit-bool-conversion`, `modernize-loop-convert`) without a repository config; (b) also commit a `.clang-tidy` with the step 13 checks as a report gate at the step's end; (c) add clang-cl as a third compiler (needs the VS "C++ Clang tools" component). | **(a) + (b)**; (c) is not needed for this step (step 5 named clang, but two compilers already catch most portability issues). |
| D5 | **Constant spelling** (item 3) | keep the `SCREAMING_CASE` names as `inline constexpr` (diff = the `#define` lines only), or rename to `kCamelCase` (touches every use). | **Keep the names** in step 13. |
| D6 | **Arena allocations and RAII** (item 5) | (a) RAII only for native allocations (libc, `new`, SDL) and for arena blocks whose owner frees them at the end of the same scope today; (b) an owning `ArenaBlock<T>` type for every arena allocation. | **(a)**: global arena-owned buffers stay explicit; the free point is a determinism rule, not a style choice. |
| D7 | **Key-dispatch actions** (item 7) | (a) `enum class` actions numbered 1..N, the original addresses kept only in `docs/original_addresses.txt` (kind `continuation`), like step 5c did for functions; (b) keep the addresses as enumerator values. | **(a)**. |
| D8 | **Counting loops whose count can be 0** (item 9) | (a) convert only loops whose count is provably > 0, leave the rest as `do { } while` with a one-line comment why; (b) convert all and keep the "runs once with count 0" behaviour explicitly where it matters. | **(a)**. A loop that wraps on count 0 (runs 2^32 times) is a crash case for step 11, not a refactoring. |
| D9 | **enum class scope** (item 4) | (a) value sets and flag sets that are used in code with names already (the `#define` families and the `using XFlags = uint32_t` aliases); (b) also give every raw number in the template and table initialisers an enumerator. | **(a)**; template initialisers keep numbers where no enumerator exists. |

## 3. What is constrained

### 3.1 Binary layouts that must not change

| What | Where | What it binds |
|---|---|---|
| 32-bit struct layouts | `src/core/layout_checks.cpp`: 513 `static_assert`s; 471 of the 539 distinct struct names declared with `typedef struct` are asserted (the other 68 are runtime-only, e.g. in `graphics/backend`, `platform/system`, `core/memory`) | sizes and pointer-field offsets of every record that is copied, saved, sent or loaded |
| Pointer fields | `Ptr32<T>` (705 uses, `include/thandor/core/ptr32.h`); process kept below 2 GB (`CMakeLists.txt:307` `/LARGEADDRESSAWARE:NO`, fixed base 0x10000000) | 4-byte pointer fields; any native pointer stored there must come from below 2 GB (true for heap and `std::vector` too) |
| Savegame `.sve` | `src/gameplay/session/savegame.cpp:74`, `:83`, `:518`, `:664`: whole `ArmyRuntimeSlot`, `ModelRuntimeSlot`, `EffectRuntimeSlot` pools, `GameFactionRuntimeImage`, shading records written as raw bytes | every field type inside these structs (incl. enum underlying types, `Bool8` bytes) |
| Network protocol | `include/thandor/network/protocol/types.h` (23 asserted packet structs); command codes are handler offsets from `INGAME_COMMAND_CODE_BASE 0x0055F130` (`include/thandor/network/protocol/commands.h:23`) | packet layouts and the command codes on the wire. **These 0x55F130-relative codes are not item 7 and must stay.** |
| UI template images | `InGameUiImage` 0xC3E4 bytes (`layout_checks.cpp:1600`), `FrontendUiImage` 0x78E8 (`:1585`), `DisplaySettingsUiImage`, `FourValueDialogUiImage`, `FatalErrorUiImage`; initialised in `src/ui/ingame/ui_template.cpp` (3,835 lines), `src/ui/frontend/ui_template.cpp` (2,286), `src/ui/dialogs/*.cpp`; copied dword by dword into an arena block (`src/ui/frontend/lifecycle.cpp:324-334`) and relocated (`UI_TEMPLATE_LINK(offset)`, `include/thandor/ui/ingame/types.h:730`) | node offsets, sizes, the 32-bit vtable slots (`UI_SLOT`) |
| Module data | 182 "Module data" sections in `src/` (the original's data as initialisers) | values only; their types may become typed members as long as the bytes stay |
| Determinism state hash | `src/platform/debug/statehash.cpp:273-306` hashes named fields and pool indices computed from pointer distances (`:50`) | field values and pool strides |
| `thandor.sym` / crash symbols | written from the linked exe (`CMakeLists.txt:314-319`) | nothing to keep; function renames are fine |
| Self-test hashes | e.g. `src/platform/selftest/keymatch_selftest.cpp:142` (own table with a 0x90909090 terminator), raster/tables/sam hashes | the self-test inputs; a package that changes a self-test's input must say so and update the reference in the same package |

### 3.2 How to tell a constrained use from a free one

1. **Struct field or local?** A struct named in `layout_checks.cpp` is constrained in its *bytes*; a local variable,
   parameter, return value or a struct in the 68 unasserted names is free.
2. **Does the value cross a format?** Search the struct name in `savegame*.cpp`, `network/protocol/*`,
   `assets/package/*`, the `*UiImage` templates and `statehash.cpp`. Any hit makes the byte representation binding.
3. **Is the address used as a value?** Original addresses that reach the wire (network command codes) are bound;
   addresses only compared locally (key dispatch, item 7) are free.
4. **Does the free point or the allocation order matter?** Every `g_MemoryApi.alloc/free` is order-bound; native
   allocations (libc, `std::vector`, SDL) are not.
5. **When unsure: add the `static_assert` first** (size and the offsets touched), then change the type.

### 3.3 Layout-neutral changes (allowed on constrained structs under D2 (a))

- `typedef struct X {...} X;` to `struct X {...};` and `typedef struct X X, *PX;` to nothing (pure syntax).
- A member `UiNodeBase node; uint32_t node_fields[N];` to `UiSpriteButtonControl node;` when
  `sizeof(UiSpriteButtonControl) == sizeof(UiNodeBase) + 4 * N` (asserted).
- A `uint32_t`/`uint8_t` member to an `enum class E : uint32_t`/`: uint8_t` member.
- `Ptr32<void>` or a union of pointer views to `Ptr32<T>` (same 4 bytes).

## 4. The items

Counts: `src/` + `include/`, per area (assets, audio, core, gameplay, graphics, movie, network, platform, ui, world).

### Item 1 - C-style pointer casts

- **Where:** 5,172 casts of the form `(Type *)expr` (the looser regex that also matches unnamed prototype parameters
  gives 5,582). Per area: ui 1,968, audio 985, gameplay 818, graphics 343, world 308, platform 242, assets 216,
  network 197, core 54, movie 41. Top files: `audio/codec/sam.cpp` 651, `ui/ingame/layout.cpp` 469,
  `audio/codec/sam_encoder.cpp` 331, `ui/frontend/scenario_selection.cpp` 75, `platform/bootstrap/runtime.cpp` 75,
  `ui/frontend/network.cpp` 74.
- **By target type:** UI control types 1,410 (`UiSpriteButtonControl` 346+, `UiNodeBase` 217, `UiFramedTextButtonControl`,
  `UiImagePanelControl`, ...), byte/scalar pointers 1,387 (`uint32_t *` 373, `uint16_t *` 310, `uint8_t *` 282,
  `void **` 70), `MmxPackedValue64 *` 962 (only the SAM decoder and encoder), world/gameplay records 829
  (`ModelRuntimeSlot *` 168, `ModelDefinition *`, `ModelRuntimeNode *`, `FieldGridCell *`, `WorldRuntimeContext *`),
  graphics 176, other 408. Named casts exist 422 times, 417 of them in `platform/` (the SDL layer is new code).
- **Constrained?** No cast is itself a format, but many casts *are* the access to a constrained layout (byte offsets
  into records). They are removed by better types (typed members, typed `Ptr32<T>`, accessors), not by
  `reinterpret_cast` spelling alone.
- **Mechanical part:** the MMX casts (`*(MmxPackedValue64 *)(p + off)`, e.g. `sam.cpp:405-408`) become one
  `Thandor_LoadU64(p + off)` helper (memcpy-based, same code) - script, 2 files. Casts that disappear with item 2
  (about 1,100 UI casts) come for free after the UI wave. `cppcoreguidelines-pro-type-cstyle-cast` lists the rest;
  `google-readability-casting` fixes a part of them automatically (guess: the `static_cast`-able half).
- **Manual part:** world/gameplay record casts through untyped fields (`(ModelRuntimeSlot *)worldNode->runtimePayload`,
  `gameplay/ai/perception.cpp:50`; `runtimePayload` is a union `ModelRuntimePayloadReference4`,
  `include/thandor/world/model/types.h:53`) need typed accessors on the union; byte-pointer arithmetic
  (`(uint8_t *)p + n`) needs a judgement per site: named member, `std::span`, or an explicit `reinterpret_cast`
  with a comment.
- **Risks:** `static_cast` between unrelated struct pointers does not compile (good, it finds the punning sites);
  `reinterpret_cast` keeps today's semantics. Aliasing: GCC builds with `-fno-strict-aliasing` (`CMakeLists.txt:46`)
  and MSVC does no type-based alias analysis, so punning stays defined in practice; do not remove that flag in this
  step. A cast that also drops `const` must become `const_cast` visibly or the `const` is fixed at the source.
- **Target:** C-style pointer casts below 300, each remaining one commented (guess), the rest named casts or gone.

### Item 2 - UI access by offsets

- **Where:** `INGAME_UI(rt, node)` 1,080 uses in 20 files (`ui/ingame/layout.cpp` 709, `editor_keyboard.cpp` 51,
  `technology.cpp` 45, `hotkeys.cpp` 36, `editor_tool_selection.cpp` 28, `chat.cpp` 26, ... and 10 in
  `gameplay/`); `FRONTEND_UI` 312; `DISPLAY_SETTINGS_UI` 51; `FOUR_VALUE_DIALOG_UI` 3; `THANDOR_UI_AT` /
  `THANDOR_UI_SIBLING` 254 (`editor_tool_selection.cpp` 79, `ui/frontend/task_assignment.cpp` 57,
  `ui/ingame/settings.cpp` 33, ...); `THANDOR_UI_FIELD` 7, `INGAME_UI_FIELD` 3.
- **How it works today:** the macros already name the node (`INGAME_UI` is
  `&((InGameUiImage *)(uintptr_t)(root))->node`, `include/thandor/ui/ingame/types.h:1655`). The problem is the
  member type: `InGameUiImage` (`types.h:742-1654`) declares every one of its 452 nodes as
  `UiNodeBase node; uint32_t node_fields[N];`, so every user casts (654 of the 1,080 uses are directly cast, 343 to
  `UiSpriteButtonControl *`). `FrontendUiImage` has 252 such nodes. `THANDOR_UI_AT(base, offset)`
  (`include/thandor/core/contracts.h:68`) adds byte offsets from tables like `g_UiMappedCommandControlOffsets`;
  `THANDOR_UI_SIBLING` (`:76`) reaches a neighbour node through `offsetof` differences.
- **Proposal (D1 (a)):** retype the template members to the control structs. The sizes already match
  (`UiNodeBase` 0x4C + 11 dwords = `UiSpriteButtonControl` 0x78; asserted in `layout_checks.cpp`). Then
  `INGAME_UI(rt, x)` yields `UiSpriteButtonControl *` and the cast goes; the selectable view becomes
  `&INGAME_UI(rt, x)->selectable`. Offset tables become arrays of member pointers (`&InGameUiImage::x`) or of node
  pointers resolved once per copy; `THANDOR_UI_SIBLING` becomes `container_of`-style access through the image
  struct (or a stored root pointer).
- **Prerequisites:** control structs are missing for six vtables used in the template:
  `UiFocusProxyControl` (58 nodes), `UiLayoutContainerControl` (24, probably `UiPageStackControl`, guess),
  `UiListOffsetControl` (10), `UiCommandSpriteButtonWithDetails` (30), `UiCommandVisibilitySingleLineText` (3),
  `UiCommandVisibilityWrappedText` (1). They are defined first from their methods' accesses and the `_fields[N]`
  sizes.
- **Constrained:** completely - the template bytes, node offsets, links and vtable slots. The template initialisers
  (`{0x00000002, 0xFFFFFFFF, 0x000000B0}` blocks, `ui_template.cpp:30`) must become designated initialisers of the
  typed struct with exactly the same bytes.
- **Mechanical part:** a generator (`tools/dev/ui_image_retype.py`, new) that reads the image struct, the vtable per
  node (it is in the member comment) and the control struct definitions, and rewrites the member declarations and
  the initialiser blocks (dwords to named fields; unnamed dwords to the control's `reserved` members). Then the casts
  at the use sites go with a token-level script (`((T *)INGAME_UI(r, x))` -> `INGAME_UI(r, x)` when `T` is the new
  member type; to `&...->selectable` / `->selectable.base` when `T` is a base view).
- **Manual part:** nodes used as two unrelated types (a node cast to a type that is neither its control nor an
  embedded base) are real puns; each needs a look (guess: a few dozen). The offset tables and `THANDOR_UI_SIBLING`
  users are manual.
- **Risks:** a wrong field in a rewritten initialiser changes the UI silently. The safety net is a new self-test that
  hashes the bytes of the five template images (package W0.4) and must give the same hash before and after; plus
  `pixels` at the end of the step.

### Item 3 - `#define` constants

- **Where:** 2,405 `#define` lines, `constexpr` 88. Of the defines: 389 include guards, 4 other empty defines, 114
  function-like macros, **1,787 object-like numeric literals** and 111 object-like expressions. 1,718 of the numeric
  ones are in headers, 76 in `.cpp` files. Per area (object-like): ui 675, platform 236, gameplay 191, core 162,
  graphics 153, network 139, world 118, assets 99, movie 30, audio 15. Families: Win32 copies (`VK_*`, `LANG_*`,
  `LOCALE_*`, ...) 91, `TEXT_ID_*` 84, names with `_FLAG(S)/_MASK/_BIT(S)` 190, sizes/counts/limits 255,
  network/command/PCK/save constants 78.
- **Constrained?** The *values* are (formats, protocol, command codes); the spelling is free. `inline constexpr`
  keeps the value.
- **Mechanical part:** `#define NAME literal` -> `inline constexpr <type> NAME = literal;` where `<type>` is the type
  the literal has today (`0x80000000` is `unsigned int`, `-1` is `int`, `0xFFFFFFFFu` is `unsigned int`): using
  `auto` with the unchanged literal keeps every promotion exactly. `modernize-macro-to-enum` can turn related
  families into unscoped enums (only where item 4 does not want an `enum class` anyway).
- **Must stay macros:** include guards; the 114 function-like macros unless they become `constexpr` functions or
  templates (separate, optional); names used in `#if`, in token pasting or stringising, in `static_assert` messages
  built by the preprocessor, in the generated imports header (`include/thandor/generated/imports.h`), and the
  Win32 copies if a real Windows header can define them too (the SDL layer includes `<windows.h>` in places; check
  per file).
- **Risks:** a define that was used in a context needing a constant expression of a specific type (bit-field width,
  array size, `case`) works the same with `inline constexpr`; a define that was redefined later or `#undef`ined
  breaks the build (found by the compiler). ODR-use (passing by `const &`, e.g. to `std::min`) emits a COMDAT
  object; harmless.
- **Expected binary effect:** the stripped release executable should stay byte-identical for most packages (guess);
  a difference must be explained.

### Item 4 - `typedef struct` and `enum class`

- **Where:** 620 `typedef struct` lines: 544 forward aliases `typedef struct X X, *PX;` (451 with a `*PX` pointer
  alias; the `PX` names are used 296 times), 64 definitions `typedef struct X {...} X;`, plus 39 `typedef union`.
  Most definitions are already plain `struct X {` (536). Per header: `gameplay/army/types.h` 89,
  `ui/frontend/types.h` 65, `ui/controls/types.h` 47, `ui/ingame/types.h` 46, `gameplay/session/types.h` 29,
  `graphics/render/types.h` 27, `network/protocol/types.h` 23, `graphics/resources/types.h` 20.
  `enum class`: 3 (`src/platform/sdl3/input.cpp:35`, `network/protocol/lockstep.h:34`, `ui/core/key_dispatch.h:23`);
  `enum` in all forms 114; `typedef enum` 1. Scalar aliases `using X = uint32_t` and the like: 579, of which 80 are
  flag/mask aliases and 154 id/kind/state/mode/index aliases - the natural `enum class` candidates.
- **`typedef struct`:** pure syntax, layout-neutral, fully mechanical (script per header; `PX` -> `X *`). The only
  risk is a name clash between a struct tag and another entity, which the compiler reports.
- **`enum class`:** new header `include/thandor/core/flags.h` with `THANDOR_FLAG_ENUM(E)` (bitwise operators,
  `Any(e)`, `ToBits(e)`, `FromBits<E>(u)`), then family by family: the `#define` family or the `using XFlags`
  alias becomes `enum class XFlags : uint32_t`, the struct fields of that alias change type (layout-neutral, same
  underlying type), uses get the enumerators. Values not named by an enumerator stay valid (fixed underlying type),
  so original data with unknown bits still loads.
- **Constrained:** the underlying type of every field in an asserted struct; flags that are hashed or sent keep
  their bit values. A flag field that is also accessed as raw bytes elsewhere (byte offsets) must be fixed together.
- **Manual part:** choosing families, naming, and the call sites that mix flags with arithmetic (`flags + 1`,
  shifts, use as index). Plan per area; one family per package for the big ones (`UiSelectableStateFlags`,
  `UiNodeFlags`, army movement/command mode flags, field cell flags).
- **Risks:** an implicit conversion lost in a comparison (`flags != 0` must become `Any(flags)`), arithmetic on
  flags turned into bit operations by mistake. Determinism covers the gameplay families.

### Item 5 - malloc/free, memset/memcpy

- **Where:** the ~150 "malloc/free" of the first count are mostly the arena: `g_MemoryApi.alloc` 131,
  `g_MemoryApi.free` 143, `shrinkInPlace` 10, `allocLargestFreeBlock` 8, `queryFreeBytes` 4 (296 uses). Real libc
  `malloc/calloc/realloc/free` are only 30: `platform/selftest/selftests.cpp` 14, `uiatlas_selftest.cpp` 7,
  `hexscan_selftest.cpp` 6, `core/settings/persistent.cpp` 3. `memset/memcpy/memmove`: 103 (platform 68 -
  `selftests.cpp` 28, `sdl3/gpu_ui_textures.cpp` 10, `sdl3/gpu_renderer.cpp` 10, `bootstrap/image.cpp` 6;
  `core/settings/persistent.cpp` 9; `gameplay/session/new_session.cpp` 6; others 1-4). Plus 23 hand-written dword
  copy/clear loops (`for (n = sizeof(X) / 4; n != 0; n--)`, e.g. `ui/frontend/lifecycle.cpp:334`). Already in use:
  `std::vector` 91, `std::unique_ptr` 8, `std::span` 13.
- **Constrained:** every arena call (order and size, rule above); copies of whole template images and records
  (exact byte counts; the dword loops copy exactly `sizeof/4` dwords).
- **Free:** the libc uses (self-tests, the ini writer) -> `std::vector`/`std::string`/`std::unique_ptr`; the SDL
  layer's `memset/memcpy` of plain structs -> value initialisation (`T x{}`) and assignment; dword copy loops over
  trivially copyable structs -> struct assignment or `std::memcpy` (same bytes, clearer).
- **Arena (D6 (a)):** a small `ArenaScoped` guard only where an arena block is allocated and freed in the same
  function on every path today (e.g. the received-dwords buffer in `network/protocol/scenario_transfer.cpp:53`);
  the free must happen at exactly the same point (guard declared so that its scope ends there, or explicit
  `release()`). Global arena-owned buffers stay explicit.
- **Risks:** a `memset` that cleared padding the code later reads as data (value initialisation also zeroes
  padding for aggregates without constructors - keep `memset` where in doubt); `std::vector` growth policy
  irrelevant (not order-bound).

### Item 6 - `Bool8`

- **Where:** 1,144 uses (`using Bool8 = uint8_t;`, `include/thandor/core/types.h:25`). gameplay 354, ui 297,
  world 126, graphics 101, assets 92, network 72, platform 57, core 22, movie 12, audio 11. By role: function
  return types and parameters 364, local variables 320, 2 globals, 3 pointers (`Bool8 *wrapped`, `outConfirmed`),
  18 `(Bool8)` casts, about 37 in `types.h` files: callback signatures (`Ptr32<Bool8 (...)>` slots such as
  `keyboardFallback`, `vetoClose`, the terrain samplers and placement tests, `using ...Proc = Bool8 (...)`) and only
  **two struct fields**: `rejected` (`include/thandor/gameplay/army/types.h:1754`) and `noFrame`
  (`include/thandor/movie/runtime/types.h:104`).
- **Constrained:** the two fields (D3: they stay `Bool8`) and any `Bool8` byte read from a file or packet.
  Callback signatures are not layout: `Ptr32<bool (...)>` is still 4 bytes; but all handlers of one slot must
  change in the same package (typed `UI_SLOT` refuses a mismatch - good).
- **Risks (manual check per site):** truncation semantics differ: `Bool8 b = flags & 0x100;` is 0 today and `true`
  as `bool` - an original quirk that a blind replacement would "fix"; such sites keep `!= 0` explicitly *as today's
  result* (i.e. `(uint8_t)(flags & 0x100) != 0`) and get an "Original quirk" note. Functions declared `Bool8` that
  return other values than 0/1 (counts, codes), `== 1` comparisons, `Bool8` used in arithmetic or as an index, and
  `Bool8` values pushed into the command queue (`InGameCommand_Issue` arguments are dwords; 0/1 stays 0/1).
  `readability-implicit-bool-conversion` finds the conversions.
- **Order:** after the UI wave (many `Bool8` signatures are UI slots) and after the flag enums (some "booleans"
  turn out to be flag bytes).

### Item 7 - original addresses 0x5xxxxx

- **Where:** 225 occurrences, 84 distinct values, **all in `src/ui/`**: `ui/ingame/key_commands.cpp` 81,
  `editor_keyboard.cpp` 67, `hotkeys.cpp` 36, `camera_commands.cpp` 32, `ui/frontend/state.cpp` 5,
  `ui/frontend/end_movie_commands.cpp` 4. They are the original continuation addresses in the
  `continuationEntryAddress` field of `UiCommandDispatchRecord` / `InGameCameraCommandDispatchRecord`
  (`include/thandor/ui/ingame/types.h:495`, `:355`; type `ContinuationEntryAddress32 = uint32_t`, `:277`), used
  only as switch keys (`hotkeys.cpp:57-58`, `case 0x5671e0:`). `key_commands.cpp:88-107` already names them in an
  unscoped `enum InGameKeyCommandContinuation`, with one non-original value `INGAME_KEY_INFO_TEXT_NEXT = 0x1`
  (record 63, `:81`).
- **Constrained?** No: the tables are module data in our code, nothing reads them from a file, sends them or hashes
  them (the `keymatch` self-test has its own table). The record structs are asserted (0xC bytes,
  `layout_checks.cpp:494`, `:944`): an `enum class ... : uint32_t` field keeps that.
- **Not this item:** the network command codes relative to `INGAME_COMMAND_CODE_BASE 0x0055F130`
  (`network/protocol/commands.h:23`) are on the wire and stay.
- **Change:** per table an `enum class` of actions (1..N), the table holds the action, the switch uses it; the
  original addresses move to `docs/original_addresses.txt` as kind `continuation` (D7). Mechanical and small; one
  package per file.

### Item 8 - 0x90909090 fill leftovers

28 matches, of which **10 are fill leftovers**; the rest are not fill:

| Place | Read? | Action |
|---|---|---|
| `graphics/terrain/terrain_render.cpp:49`, entry 259 of `g_TerrainProjectedRowSpans` (260 entries, `:17`); loops start at entry 0 and are bounded by `TERRAIN_PROJECTED_GRID_MAX_ROWS 257` + emptied rows | not read on valid grids (guess: entry 259 is only reachable for gridHeight 258, which `:84` rejects) | zero it, keep the 260 size; verify the bound in the package |
| `graphics/render/shadow_texture.cpp:61`, `reserved14`, `reserved1A4` of `g_GeneratedTextureScratchRuntime` | check | drop the initialisers if never read (the fields stay, layout) |
| key table terminators: `ui/ingame/hotkeys.cpp:34` (record 15), `key_commands.cpp:82`, `editor_keyboard.cpp:53`, `ui/frontend/state.cpp:38` | the dispatcher stops at commandCode 0 (`include/thandor/ui/core/key_dispatch.h:64`) | set to 0 with item 7 (same files, same packages) |
| `ui/frontend/end_movie_commands.cpp:26` terminator `modifierClassFlags = 0x90909090` | **read** (original quirk, comment `:19-22`) | keep |
| `platform/selftest/keymatch_selftest.cpp:142` | self-test input reproducing that quirk | keep |
| `graphics/backend/software_raster.h:236-251` | a dump of original bytes (`g_SoftwareBlendOverreadOriginalDwords`, data the blitters' over-read sees) | keep (it is data) |
| `gameplay/army/preview.cpp:64` | a lookup value `0x90909090909ull`, not fill | not this item |

### Item 9 - `do { } while` counting loops

- **Where:** 422 `do {`; 415 matched tails. Shapes: `while (n != 0)` 274 (decrement-and-test counters),
  `while (i < n)`/`<=` 67, `while (--n != 0)` 3, `while (!x)` 10, pointer/`next` walks 6, others. Per area: ui 136,
  gameplay 63, world 63, graphics 60, assets 28, platform 20, core 19, network 15, audio 12, movie 6. Top files:
  `ui/frontend/player.cpp` 18, `graphics/terrain/terrain_render.cpp` 12, `world/pathing/scratch_grid.cpp` 11,
  `world/terrain/water_relaxation.cpp` 10, `ui/frontend/credits_mask.cpp` 10, `graphics/resources/tiled_blit.cpp` 10,
  `graphics/resources/palette_optimizer.cpp` 10, `assets/package/codec.cpp` 10.
- **Count-0 analysis:** a `do { ...; n--; } while (n != 0);` with `n == 0` runs 2^32 times (or until a `return`),
  `for` runs 0 times. Only about 40 loops have a constant or `sizeof` count assigned in the three lines before
  the `do` (heuristic); the other ~380 need a reason per loop (a guard before it, a caller contract, a table that is
  never empty). Example: `ui/frontend/player.cpp:366` walks the player blocks with `remainingBlocks` - safe only if
  the block count is never 0.
- **Change:** convert where the count is provably >= 1 (comment the proof if not obvious), keeping the iteration
  order and the counter's final value if it is used after the loop; a search loop with `return` inside becomes a
  range-for or `std::find_if` only if the order and the "first match" semantics stay. Where count 0 is possible and
  wraps, leave the `do`/`while`, add "Original quirk: runs 2^32 times for n == 0" and list it for step 11 (D8 (a)).
- **Mechanical part:** none really; `modernize-loop-convert` does not touch `do`/`while`. The agent work is reading
  each loop. (guess: 60 % convertible.)
- **Risks:** the decompiled loops often hoist pointer increments; an off-by-one changes pixels or the simulation.
  Gameplay/world/pathing loops are covered by determinism and AI hash at the end; graphics loops by golden and
  raster self-tests.

## 5. Order: what unlocks what

```
W0 tools + helpers + uitemplate self-test
 |
W1 syntax: typedef struct (item 4a) | key dispatch enums + their 0x90 terminators (7, 8) | other 0x90 fill (8)
 |
W2 #define -> inline constexpr (3), per area
 |
W3 typed UI images (2): missing control structs -> retype templates (generator) -> casts at INGAME_UI / FRONTEND_UI users -> offset tables
 |
W4 casts (1): MMX helper, remaining UI downcasts, typed Ptr32/union accessors in world/gameplay, byte casts per area
 |
W5 enum class flag/value families (4b), then Bool8 (6): slot signatures first, then per area
 |
W6 RAII / memset / dword loops (5) | do-while loops (9), per area
 |
W7 final: full run_checks, gpu_compare, both compilers, .clang-tidy report, counts
```

Why this order:

- **typedef struct first**: it touches every type header once, mechanically, with zero risk; doing it later would
  collide with every other wave's header edits.
- **constexpr before enum class**: the `#define` families become `inline constexpr` first (mechanical), then the
  enum wave turns selected families into `enum class` with the names already in C++.
- **typed UI before the cast wave**: about 1,100 of the 5,172 casts disappear with the typed template members; a
  cast wave before it would rewrite them twice.
- **enum class before Bool8**: some `Bool8` parameters and returns carry flag bytes; once the flag types exist they
  get the right type instead of `bool`. The UI slot signatures (`Ptr32<Bool8 (...)>`) are settled after W3.
- **loops and RAII last**: manual, file-local, low conflict; they profit from typed code (fewer casts to read).
- Items 7 and 8 are independent and go early (small, they remove noise from the hotkey files before W3 touches them).

## 6. Safety net

Per package (agent, in its worktree):

- GCC build (`mingw-test` preset) and MSVC build (`test` preset), **0 new warnings** in both.
- `layout_checks.cpp` compiles; a package that changes a member type in an asserted struct adds the asserts of
  the offsets it touches first.
- Self-tests against the reference build: `raster`, `tables`, `sam`, `keymatch`, `codec`, `fixedmath`, `hexscan`,
  `pcx`, `uiatlas`, `settings`, `numberformat`, `trianglesetup`, `keymap`, `icon`, `movieenc`, and the new
  `uitemplate` (W0.4); `golden_cmp.ps1` (ot-scratch).
- For the purely syntactic waves (W1 typedef, W2 constexpr): compare the stripped `mingw-release` executable with
  the one before the package; identical bytes are the expected result, a difference is explained in the package
  report (guess: identical for W1 always, for W2 in most packages).
- No game tests (determinism, pixels, multiplayer, maps, campaign) during the step.

Per wave (coordinator, after merging): both compilers, self-tests, `golden_cmp.ps1`, the counts script
(W0.1) showing that no count went up.

At the end (W7): `tools/test/run_checks.py` with all eight checks (`determinism`, `aihash`, `pixels`, `saveload`,
`textedit`, `multiplayer`, `campaign`, `maps`), `tools/test/gpu_compare.py` (Vulkan and D3D12), both compilers,
the `.clang-tidy` report, and a savegame of the step's start loaded by the step's end build (saveload covers
our own save/load; an old save checks the frozen format).

## 7. Work packages

Sizes: **S** about 15 minutes of agent work, **M** about 25-30 minutes. Each package runs in its own worktree on
a branch from the wave's base, is merged by the coordinator, and lists the files it touched. "Verify" names the
checks beyond the per-package standard of section 6.

### W0 - tools and helpers (before everything; 5 packages, can run in parallel)

| Id | Title | Files | Size | Verify |
|---|---|---|---|---|
| W0.1 | Counts script: the nine metrics per area (the regexes of section 9), `--check <baseline>` fails when a count rises | `tools/dev/modern_counts.py` (new) | S | run on dev, numbers of this plan |
| W0.2 | `flags.h`: `THANDOR_FLAG_ENUM` operators, `Any`, `ToBits`, `FromBits`; compile-time tests | `include/thandor/core/flags.h` (new) | S | static_asserts |
| W0.3 | Load/store helpers `Thandor_LoadU64/U32/U16`, `Thandor_Store*` (memcpy), next to `x86_emulation.h` | `include/thandor/core/x86_emulation.h` | S | `sam` self-test unchanged |
| W0.4 | `uitemplate` self-test: hashes the bytes of the five `*UiImage` templates and of a relocated in-game copy | `src/platform/selftest/` (new file), CMake | S | reference hash recorded |
| W0.5 | `.clang-tidy` with the step 13 checks (report only), `tidy.py --report` mode; agent rules file for step 13 in ot-scratch | `.clang-tidy` (new), `tools/dev/tidy.py` | S | report on dev |

### W1 - syntax and small items (16 packages)

| Id | Title | Files | Size | Verify |
|---|---|---|---|---|
| T1 | `typedef struct`/`union` to `struct`/`union`, `PX` to `X *` | `include/thandor/gameplay/army/types.h` + `PArmy*` users (token-only) | M | stripped exe identical |
| T2 | same | `include/thandor/gameplay/{session,ai,faction,selection,technology}/` | M | identical |
| T3 | same | `include/thandor/ui/frontend/` | M | identical |
| T4 | same | `include/thandor/ui/{controls,core,dialogs}/` | M | identical |
| T5 | same | `include/thandor/ui/ingame/` | S | identical |
| T6 | same | `include/thandor/graphics/`, `src/graphics/` | M | identical |
| T7 | same | `include/thandor/network/`, `include/thandor/world/` | S | identical |
| T8 | same | `include/thandor/{assets,audio,movie}/` | S | identical |
| T9 | same | `include/thandor/{core,platform}/`, the 16 `.cpp` files with a local `typedef struct X {` | S | identical |
| K0 | `docs/original_addresses.txt`: the 84 continuation addresses as kind `continuation` (one place, before K1-K5) | docs | S | - |
| K1 | Key actions `enum class` + terminator 0 | `ui/ingame/hotkeys.cpp` | S | `keymatch` |
| K2 | same (replaces `enum InGameKeyCommandContinuation`) | `ui/ingame/key_commands.cpp` | S | `keymatch` |
| K3 | same | `ui/ingame/editor_keyboard.cpp` (+ `editor_keyboard.h:30`) | M | `keymatch` |
| K4 | same | `ui/ingame/camera_commands.cpp` | S | - |
| K5 | same; end-movie terminator stays (quirk) | `ui/frontend/state.cpp`, `end_movie_commands.cpp` | S | `keymatch` |
| N1 | 0x90 fill: row span 259, scratch runtime reserved fields (check reads first) | `graphics/terrain/terrain_render.cpp`, `graphics/render/shadow_texture.cpp` | S | golden |

T1-T9 replace `PX` tokens in `.cpp` files of other areas too; these are single-token edits and merge cleanly. Run
them as one wave, nothing else touching headers at the same time.

### W2 - `#define` to `inline constexpr` (11 packages)

One package per header group, numeric object-like defines only; families that W5 will turn into `enum class` are
still converted here (the enum wave then replaces the constants).

| Id | Files | Defines (approx.) | Size |
|---|---|---|---|
| C1 | `include/thandor/ui/frontend/`, `src/ui/frontend/` | 250 (guess) | M |
| C2 | `include/thandor/ui/ingame/`, `src/ui/ingame/` | 250 (guess) | M |
| C3 | `include/thandor/ui/{controls,core,dialogs}/`, `src/ui/{controls,core,dialogs}/` | 175 (guess) | M |
| C4 | `gameplay/` | 191 | M |
| C5 | `platform/` (Win32 copies: keep as macros where `<windows.h>` may also define them) | 236 | M |
| C6 | `core/` | 162 | M |
| C7 | `graphics/` | 153 | M |
| C8 | `network/` (protocol values; values unchanged) | 139 | M |
| C9 | `world/` | 118 | M |
| C10 | `assets/`, `audio/`, `movie/` | 144 | M |
| C11 | function-like macros that are simple expressions -> `constexpr` functions (optional, owner) | 114 total | M |

Verify: stripped exe identical or explained.

### W3 - typed UI images (16 packages)

| Id | Title | Files | Size | Verify |
|---|---|---|---|---|
| U1 | Define the six missing control structs from their methods and `_fields[N]` sizes, with asserts | `include/thandor/ui/controls/types.h`, `ui/ingame/types.h`, `layout_checks.cpp` | M | asserts |
| U2 | Generator `ui_image_retype.py`; pilot on `FatalErrorUiImage`, `FourValueDialogUiImage`, `DisplaySettingsUiImage` | `tools/dev/`, `src/ui/dialogs/`, `include/thandor/ui/dialogs/types.h` | M | `uitemplate` identical |
| U3 | Retype `InGameUiImage` members and initialisers (generated) | `include/thandor/ui/ingame/types.h`, `src/ui/ingame/ui_template.cpp` | M | `uitemplate` identical |
| U4 | Retype `FrontendUiImage` (generated) | `include/thandor/ui/frontend/types.h`, `src/ui/frontend/ui_template.cpp` | M | `uitemplate` identical |
| U5a-d | Drop the casts at `INGAME_UI` users in `layout.cpp` (709 uses), split by function ranges, **sequential** (same file) | `src/ui/ingame/layout.cpp` | 4 x S | golden |
| U6 | same | `ui/ingame/editor_keyboard.cpp`, `editor_tools.cpp` | S | - |
| U7 | same | `ui/ingame/technology.cpp`, `army_stock.cpp`, `selection_detail.cpp` | S | - |
| U8 | same | `ui/ingame/hotkeys.cpp`, `chat.cpp`, `pages.cpp`, `hud.cpp`, remaining `ui/ingame` users, the 10 in `gameplay/` | M | - |
| U9 | same for `FRONTEND_UI` (312), dialogs (`DISPLAY_SETTINGS_UI` 51, `FOUR_VALUE_DIALOG_UI` 3) | `src/ui/frontend/`, `src/ui/dialogs/` | M | - |
| U10 | Offset tables -> member pointers / node pointers; `THANDOR_UI_AT` users | `ui/ingame/editor_tool_selection.cpp` (79) | M | `uitemplate`, golden |
| U11 | same | `ui/frontend/task_assignment.cpp` (57) | M | - |
| U12 | `THANDOR_UI_SIBLING` users (`GRAPHICS_UI`, `settings.cpp:247`), remaining `THANDOR_UI_AT`/`_FIELD` | `ui/ingame/settings.cpp`, `savegame_page.cpp`, `pages.cpp`, `ui/frontend/scenario_selection.cpp`, `faction_setup.cpp` | M | - |
| U13 | Remove `THANDOR_UI_AT`/`_SIBLING`/`_FIELD`, `INGAME_UI_FIELD`, `FRONTEND_UI_FIELD` when unused | `include/thandor/core/contracts.h`, the type headers | S | - |

U6-U9 run in parallel after U3/U4; U5a-d in sequence alongside them. U10-U12 after U5-U9. Optional later
(D1 (b)): inheritance between control structs (a wave of its own, about 8 M packages, guess).

### W4 - remaining casts (19 packages)

| Id | Title | Files | Size |
|---|---|---|---|
| X1 | MMX loads via W0.3 helpers (script) | `audio/codec/sam.cpp` | S |
| X2 | same | `audio/codec/sam_encoder.cpp` | S |
| X3 | Typed accessors on the record unions (`ModelRuntimePayloadReference4`, ownership references) | `include/thandor/world/model/types.h`, `include/thandor/gameplay/army/types.h` | M |
| X4-X7 | World/gameplay casts through the X3 accessors and typed `Ptr32<T>` | `gameplay/ai`, `gameplay/army`, `gameplay/{session,selection,faction,technology}`, `world/*` (one package each) | 4 x M |
| X8-X10 | UI node downcasts left after W3 (`UiNodeBase *` 217 etc.) | `ui/controls`, `ui/frontend`, `ui/ingame` | 3 x M |
| X11-X17 | Byte/scalar pointer casts: named cast with a reason, `std::span`, or a member | `assets`, `graphics/{backend,resources}`, `graphics/{render,terrain,core}`, `network`, `platform/bootstrap+system`, `platform/{sdl3,debug,selftest}`, `core`+`movie` | 7 x M |
| X18 | Leftovers by the clang-tidy report | per report | M |
| X19 | Gate: `cppcoreguidelines-pro-type-cstyle-cast` count in `.clang-tidy` report; counts baseline | - | S |

### W5 - enum class and Bool8 (20 packages)

| Id | Title | Files | Size |
|---|---|---|---|
| E1 | UI selectable/node/root flags (`UiSelectableStateFlags`, `UiNodeFlags`, root flags) | `include/thandor/ui/{core,controls}/`, users | M |
| E2 | UI frontend/in-game state families (player state, page ids) | `ui/frontend`, `ui/ingame` | M |
| E3 | Army movement / command mode flags | `include/thandor/gameplay/army/types.h`, `gameplay/army` | M |
| E4 | Other gameplay families (session role, AI states) | `gameplay/{session,ai}` | M |
| E5 | Field cell / terrain flags | `world/terrain`, `graphics/terrain` | M |
| E6 | World model/effects/shots families | `world/{model,effects,shots,pathing}` | M |
| E7 | Graphics flags (render context, texture source, packet flags) | `graphics/` | M |
| E8 | Network/session flags (values on the wire unchanged) | `network/` | M |
| E9 | Assets/audio/movie/platform families | rest | M |
| E10 | Value sets that are plain unscoped `enum`s today (114) where scoping helps | per area | M |
| B1 | Callback slot signatures `Ptr32<Bool8 (...)>` / `...Proc = Bool8 (...)` and all their handlers (alone in its sub-wave) | the ~35 `types.h` lines, handlers | M |
| B2-B9 | `Bool8` -> `bool` for returns, parameters, locals; truncation and non-0/1 sites checked and marked | gameplay (2 packages), ui (2), world, graphics, assets+network, platform+core+audio+movie | 8 x M |
| B10 | `Bool8` stays only for the two fields and file bytes; comment at `core/types.h:25` | `include/thandor/core/types.h` | S |

### W6 - RAII and loops (16 packages)

| Id | Title | Files | Size |
|---|---|---|---|
| R1 | libc in self-tests -> `std::vector`/`std::unique_ptr` | `platform/selftest/selftests.cpp`, `uiatlas_selftest.cpp`, `hexscan_selftest.cpp` | S |
| R2 | ini writer -> `std::string`/`std::vector`; its `memcpy`/`memset` | `core/settings/persistent.cpp` | S |
| R3 | SDL layer `memset/memcpy` -> value init / assignment / `std::copy` | `platform/sdl3/` | M |
| R4 | Dword copy/clear loops -> struct assignment / `std::memcpy` (exact byte counts) | the 23 sites (`ui/frontend/lifecycle.cpp`, `gameplay/session/*`, ...) | M |
| R5 | `ArenaScoped` for function-local arena blocks freed at scope end today (owner D6) | `network/protocol/scenario_transfer.cpp`, `world/pathing/scratch_grid.cpp` and similar | M |
| L1-L11 | `do { } while` per area with count-0 analysis | ui/frontend (2), ui/ingame+controls (2), gameplay (2), world (2), graphics (2), assets+audio+core+movie+network+platform (1) | 11 x M |

### W7 - end of step (1 package, coordinator)

Full `run_checks.py` (all eight), `gpu_compare.py`, GCC + MSVC 0 new warnings, `.clang-tidy` report, the counts
table of section 8 filled in, `tools/docs/gen_docs.py` regenerated, README/CHANGELOG.

**Total: 104 packages** (5 + 16 + 11 + 16 + 19 + 20 + 16 + 1; U5a-d counted as four), 28 S and 76 M.

## 8. Estimate and targets

| Item | Now | Target after step 13 |
|---|---|---|
| C-style pointer casts | 5,172 | < 300, each commented (guess) |
| `INGAME_UI` casts / `THANDOR_UI_AT`+`SIBLING` | 654 cast uses / 254 | 0 / 0 (macros removed) |
| object-like numeric `#define` | 1,787 | the ones that must stay macros (guess: < 150) |
| `typedef struct` | 620 | 0 |
| `enum class` | 3 | about 60-100 families (guess) |
| libc malloc/free | 30 | 0 |
| arena alloc/free | 296 | unchanged (by rule), some in `ArenaScoped` |
| `Bool8` | 1,144 | 2 fields + file bytes (guess: < 30) |
| original 0x5xxxxx addresses | 225 | 0 in `src/` (kept in `docs/original_addresses.txt`) |
| 0x90909090 fill | 10 leftovers | 2 kept on purpose (quirk + its self-test) plus the raster data dump |
| `do { } while` | 422 | about 170 left, each with a reason (guess: 60 % converted) |

**Effort (guess):** 28 S x 15 min + 76 M x 27 min = about 41 agent-hours; with 5-6 agents in parallel and the
coordinator's merges and gates per wave about 8 waves x 1.5 h, that is roughly 4-5 working sessions of the
coordinator, plus about 2 hours for the final checks. W3 (typed UI) and W5 (Bool8 truncation sites) carry the most
risk; W1 and W2 are the safest and fastest.


**Result after step 13 (W7, 2026-10-07):**

| Item | Start | End |
|---|---|---|
| C-style pointer casts (modern_counts) | 5,174 | 4 |
| `INGAME_UI` / `FRONTEND_UI` / `THANDOR_UI_AT`+`SIBLING` | 1,080 / 320 / 254 | 0 / 0 / 0 (macros removed) |
| object-like numeric `#define` | 1,790 | 121 (Win32/platform copies and two override points) |
| `constexpr` | 90 | 1,608 |
| `typedef struct` | 620 | 0 |
| `enum class` | 3 | 96 |
| libc malloc/free | 30 | 0 |
| arena alloc/free | 297 | 290 (order unchanged; `ArenaScoped` in four functions) |
| `Bool8` | 1,145 | 5 (type, two D3 fields, two comments) |
| original 0x5xxxxx addresses | 225 | 0 (in `docs/original_addresses.txt`) |
| 0x90909090 fill | 28 | 18 (read by the original's over-read emulation, quirk terminators, self-test inputs) |
| `do { } while` | 421 | 297 (about 150 marked "D8: kept for step 11", the rest are not counting loops) |
| clang-tidy diagnostics (step 13 checks) | 15,926 | 889 |

## 9. How the counts were taken

`grep -rE` over `src include` at 3de753df:

- C-style pointer cast: `(^|[^A-Za-z0-9_])\((const |volatile )?[A-Za-z_][A-Za-z0-9_:<>]*( const)? ?\*+ ?\) ?[A-Za-z_(&*]`
- `INGAME_UI\(`, `THANDOR_UI_(AT|SIBLING)\(`, `FRONTEND_UI(_FIELD)?\(`
- defines: `^\s*#\s*define`; numeric object-like `#\s*define\s+\w+\s+\(?-?(0x[0-9A-Fa-f]+|[0-9]+)[uUlL]*\)?\s*(/[*/].*)?$`;
  guards `#\s*define\s+THANDOR_[A-Z0-9_]+_H\s*$`
- `typedef struct`, `\benum class\b`, `\bconstexpr\b`
- libc: `(^|[^.>A-Za-z0-9_])(std::)?(malloc|calloc|realloc|free)\(`; arena: `g_MemoryApi\.\w+`;
  `\b(memset|memcpy|memmove)\(`
- `\bBool8\b`; `0x5[0-9A-Fa-f]{5}\b`; `0x90909090`; `\bdo ?\{` and `\} ?while ?\(.*\);`
- asserted structs: the names in `static_assert(sizeof(X)` of `src/core/layout_checks.cpp` against the names in
  `typedef struct X`.
