# Key rebinding

Status: P1 (binding model), P2 (dispatch) and P3 (ini `[keys]`: `PersistentSettings_RegisterSection` / `_MarkDirty`,
`KeyBindingsIni_Register` called at the start of `ProcessEntry_RunGame`) done on `experimental/keybinding-edit`;
P4..P6 open.

## Goal

One key binding panel, the same code and nodes in two hosts:

- frontend options, page "Tastenbelegung";
- in-game: the F1 help window gets a tab "Tastatur" that opens the same panel as a modal overlay of the same size;
  "Zurück" returns to the help window.

Two columns: LEFT the action description, RIGHT the key or combination. The right cell is editable. A change applies
immediately everywhere and is saved to `thandor.ini` section `[keys]`.

No effect on determinism or multiplayer: lockstep sends commands, not keys; bindings are local settings and are not
stored in savegames.

## Owner decisions

**Capture.** Clicking a key cell starts capture; the cell shows "Taste drücken… (Esc: abbrechen)". The combination is
taken when the keys are RELEASED: every key held together at the peak forms the combination (modifiers Ctrl / Alt /
Shift plus the key), so holding several keys gives combinations. Esc alone cancels. Keys without a key-down code are
rejected.

**Conflicts.** A combination that is already used in the same context (simulated dispatch over all records of that
context with their modifier rules, plus the held camera keys and the fixed keys) or that is a fixed key is NOT
assigned. The status line shows "Taste bereits belegt: <Aktion>" or "Taste nicht erlaubt"; the old binding stays.
Right-click on a key cell clears the binding (so keys can be freed). The "Standard" button resets to the defaults.

**One editable cell per row.** A new capture replaces all chords of the action. Default alternative chords (for
example Alt+Shift+S for Stop) stay only until the action is rebound.

**Group rows** (select / add / store / add-to group 1..8): only the modifier part is editable, the digits 1..8 stay.

**Camera keys** may be letter keys too (WASD etc.), so the held state is needed for every key-down code (P6).

**Fixed** (shown greyed, not editable): Esc, Alt+F4, the general UI rows (Tab / Space / Enter / Esc in dialogs,
list navigation, sliders), intro skip, end movie Alt+Q / Ctrl+P (WM_CHAR table), the hidden Ctrl+F1 of the faction
setup, the cheats, the map editor tables. In-game Enter (chat) and Tab (side panel) are rebindable.

**ini.** Section `[keys]`, English canonical tokens: `pause = P`, `slower = Alt+G`, `group_store = Ctrl+1..8`,
`camera_left = Left`, `none` for unbound; several chords joined with `, `. Written in full once anything differs from
the defaults. Unknown names are ignored; an invalid value gives the default and a log line; conflicts on load unbind
the later action and log. Hooked into `persistent.cpp` through a section hook `PersistentSettings_RegisterSection`,
with MarkDirty and an immediate Flush.

## Model (P1, `include/thandor/ui/core/key_bindings.h`)

- `KeyAction`: every action, in display order (section by section), then the hidden fixed actions (cheats,
  Ctrl+F1). Per action: stable ini name, context (`InGame`, `Menus`, `EndMovie`, `Ui`), section, dispatcher source and
  its table action number, text id (`TEXT_ID_KEYS_*`), range length (8 for the group rows), fixed / hidden flags,
  default chords (equal to the original tables) and an optional permanent chord (Alt+F4 of `menu_leave`, kept by
  every Set / Clear).
- The camera row is split into ten actions: left / right / up / down, tilt minus / plus (Page Up / Page Down),
  rotate minus / plus (Insert / Delete), zoom in / out (Home / End).
- Conflict simulation: each chord's own press (its key with exactly its modifiers) is dispatched under the
  dispatchers' modifier rules and order. Rules: in-game hotkeys and end movie: Ctrl/Alt exact, Shift ignored unless
  the chord has Shift (then Shift is required: the Shift variant P2 adds to `key_dispatch.h`); world view key
  commands and frontend hotkeys: exact with Shift; camera and UI keys: held (no modifiers in the chord = any
  modifiers). Order: the world view's key commands before the in-game hotkeys; held keys always. Two chords conflict
  when a press reaches both or no longer reaches its own chord (Pause = S with Stop movement = Shift+S is fine: the
  world view gets Shift+S first, S still pauses). The context `Menus` is also checked against the `Ui` actions.
  Esc (any modifiers), Alt+F4 and Alt+Tab are never allowed; a hidden fixed action (cheat) answers "not allowed"
  instead of naming it.
- Changes bump a change counter and call the registered listeners (P2 hooks Apply, P3 hooks save).

## Dispatch (P2, `include/thandor/ui/core/key_binding_tables.h`)

- The in-game hotkeys, the world view's key commands and the frontend hotkeys keep their original records as
  constants; the working tables (`std::array`, built at compile time, equal to the originals with the defaults) hold
  them in the original order, then spare records, then the terminator (code 0). `KeyBindingTables_Install` (program
  start, before the self-tests) registers `KeyBindingTables_Apply` as change listener and applies once.
- Apply: each original record takes the current chord with the index of the default chord it stood for (a group
  record keeps its digit and takes the modifiers, so `groupIndex` stays `commandCode - '1'`); a missing chord gives
  `UI_KEY_COMMAND_CODE_UNBOUND` (0xFFFFFFFF, never a key code, not the terminator). Chords beyond the default count
  go into the spare records, in action order. Spare records per table cover `KEY_BINDING_MAX_KEYS` chords on every
  editable action (hotkeys 40, key commands 167, frontend 3); the self-test checks it. Fixed actions' records (Esc,
  Alt+F4, the cheats, Ctrl+F1) stay as they are; the end-movie table, the editor tables and the intro skip are not
  touched.
- The in-game hotkeys use `UiKeyModifierRule::ShiftRequiredByClass` (Ctrl/Alt exact, Shift ignored unless the
  class has Shift, then required): the same as `ExactShiftIgnored` for every default record.
- The camera (`InGameUiRoot_ApplyCameraKeys`) polls `KeyBindings_HeldDown(CameraLeft..CameraZoomOut)`.

## Packages

| | Content |
|---|---|
| P1 | binding model (this file's model section), self-test `keybindings` |
| P2 | dispatch: tables non-const; Apply rewrites `commandCode` / `modifierClassFlags` in the original record order, unused records get an unbound sentinel; Shift variant in `key_dispatch.h`; camera polls `KeyBindings_HeldDown` |
| P3 | ini `[keys]` (tokens, section hook, MarkDirty + Flush) |
| P4 | shared panel: static node tree in module storage, own vtables, capture, status line; camera direction texts |
| P5 | hosts: frontend page and the in-game "Tastatur" tab overlay; scripts |
| P6 | held state for every key-down code (`devices.cpp`, SDL3 input), cleared on focus loss, for letter camera keys |

## Panel (P4, `include/thandor/ui/core/key_bindings_panel.h`)

- `KeyBindingsPanel_Attach(parent, rect, flags, onClose, context)` links the panel subtree (module storage, own vtables
  `g_KeyBindingsPanelVtable` / `g_KeyBindingsPanelListVtable`, button actions on its own handler page 0x30) as the
  parent's last child; flags `KEY_BINDINGS_PANEL_CENTERED` (rect relative to the parent's centre), `_BACKGROUND`
  (window background and frame, for the in-game overlay), `_TITLE`. "Zurueck" (and Esc outside a capture) calls
  `onClose(context)` from the action queue; the host detaches (`KeyBindingsPanel_Detach`). `KeyBindingsPanel_Refresh`
  after a language switch. Minimum size 520 x 240; the frontend page uses 608 x 284.
- While a capture runs (`KeyBindingsPanel_Capturing`) the list holds the keyboard focus and takes every key, so neither
  the world view's key commands nor the hotkeys (root keyboard fallback) see it. The held camera keys are polled, not
  dispatched: the in-game host skips `InGameUiRoot_ApplyCameraKeys` while capturing.
- Release detection uses the navigation keys' held state until P6 (`Keyboard_KeyHeld`) is merged; other keys are
  taken when the modifiers are released.
