# Step 12 (later): multiplayer security

Status: planned, not started; the last step. The open multiplayer and network findings of the step 8 code review
(owner decision 2026-10-04): data received from peers, the lobby/session/command transfer, the WinSock backend and
the transfer thread. Everything else moved to [step 11](step11_security_crash.md), which comes first (P48 here needs
step 11's P36).

Already done in step 8: the snapshot chunk request offset (P01), the session advert strings with the shared peer-text
sanitizer (P02), the command validator at the dispatch and the earlier lobby/chat bounds (step 8 waves 1-3). Package
ids are those of the review's triage list (a working note outside the repository).

## Rules

The ground rules of [step 8](step8_idiomatic_cpp.md#ground-rules) and the rules of
[step 11](step11_security_crash.md#rules) apply. In addition:

- **Wire format unchanged**: only what valid peers never send is rejected (one log line); the original game must
  still play against ours in both directions.
- **Send order and `Random_*` order unchanged**: sanitizing happens after the existing random calls.
- **Tests**: every package runs `tools/test/run_multiplayer.py` (two instances over UDP) plus determinism; packages
  touching the lobby, transfer or backend are also tested by hand against the original game (host and client).

## Packages

"Started" = a branch from step 8 exists but is not merged; check it against current `dev` before reuse.

### Backend and transfer

| Id | Title | Files | Change |
|---|---|---|---|
| P04 | WinSock received length, sockaddr assert, `-IP` buffer | network/backend/fallback_udp.cpp, network/backend/types.h, core/layout_checks.cpp | return the received byte count, drop datagrams shorter than `unitCount*32` before decrypting; `static_assert(sizeof(WinSockAddress) == 16)`; `-IP=` into a local buffer. Started, branch `step8-p04-winsock` |
| P03 | Mailbox count-0 and quirk labels | network/transfer/mailbox.cpp | count-0 check before record 0; quirk comment on the dropped tail bytes; static_assert for the ping text |
| P48 | Transfer thread race | network/transfer/mailbox.cpp, core/memory/allocator.cpp | spin-lock guard around the unit-cursor/ring section; timer-thread allocation deferred to the main thread or locked (re-verify arena order). After P03 and step 11's P36 |

### Lobby, session and scenario transfer

| Id | Title | Files | Change |
|---|---|---|---|
| P46 | Player name/descriptor 0x30005 | network/protocol/command_exchange.cpp | apply the shared peer-text sanitizer to the 20-unit descriptor before the copy, after the random calls. Uncommitted step 8 work, redo |
| P47 | `FrontendRootRuntimeAddress32` is an `int` | network/protocol/types.h, frontend_session.cpp, lobby.cpp | `uintptr_t` (or a typed pointer); pure type change |
| P05 | Scenario transfer pointer stores bypass `Ptr32` | network/protocol/scenario_transfer.cpp, gameplay/session/scenario_load.cpp | `Thandor_PointerToU32` instead of a truncating cast |
| O7 | `EDITOR_SAVE_MAP` from peers (owner) | network/protocol/commands.cpp (dispatch), ui/ingame/editor_tools.cpp | ignore it unless the local editor flag is set (log once) |

Ordering: P03 before P48 (same file); the other packages have disjoint files and can run in one wave.
Owner-decision ids continue those of step 11 (O1-O6).

Total: 7 packages (6 review packages + 1 owner decision).
