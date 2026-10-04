# Step 10 (later): patch installer for Open Thandor 1.0.6

Status: planned, not started. Step 10 is an installer that puts our build onto an existing Thandor installation as a
patch, with the version bumped to 1.0.6 and a picture of the game, looking like the original `Thandor-Patch-5.exe`
(a classic Windows installer). Open decisions for the owner are listed in work package 0 (section 7).

This plan is a read-only analysis (`dev` at 08eee1b6; line numbers refer to that commit). The original
`Thandor-Patch-5.exe` was never executed, only read as bytes. Its extracted pictures, strings and a mock-up of our
wizard image are kept locally outside the repository (they are game assets). Items marked **(guess)** are inferred
and not verified.

---

## 1. The original patch installer `Thandor-Patch-5.exe`

### Framework

- **Inno Setup 5.1.2 (ANSI build).** Evidence:
  - The setup header starts with the signature `Inno Setup Setup Data (5.1.2)` (file offset 814459).
  - The message block identifies itself as `Inno Setup Messages (5.1.0)`.
  - The PE stub is a Delphi binary (sections `CODE`/`DATA`/`BSS`, 32-bit, timestamp 1992). Its RCDATA #11111 holds
    the `rDlPtS` loader offset table, and its manifest names `JR.Inno.Setup`.
  - The version resource comment reads "This installation was built with Inno Setup: http://www.innosetup.com".
- File size 1,082,903 bytes, with no Authenticode signature. The parts are:
  - the loader stub (52,224 bytes),
  - setup-1 data starting at 52224: one solid chunk `zlb\x1a` + LZMA, 762,231 bytes compressed, 2,726,132 unpacked,
  - the setup-0 header at 814459: two LZMA blocks of 123,592 and 207 bytes unpacked,
  - the embedded `Setup.exe` at 855016 (663,040 bytes unpacked; it was not extracted and must not be run).
- Version resource: CompanyName "Innonics & Thandor-World.de", FileDescription "Thandor Patch 5 Setup". FileVersion
  is empty.

### Setup header (decoded strings)

| Field | Value |
|---|---|
| AppName | `Thandor Patch 5` |
| AppVerName | `Version 1.05` |
| AppId | `Thandor Patch 5` (gives the uninstall key `...\Uninstall\Thandor Patch 5_is1`) |
| AppPublisher | `Innonics & Thandor-World.de` |
| Publisher, Support and Updates URL | `http://www.thandor-world.de` |
| AppVersion | (empty) |
| DefaultDirName | `{reg:HKLM\Software\Thandor,GamePath\|{pf}\Thandor}` |
| DefaultGroupName | `Thandor` |
| next string (probably OutputBaseFilename) | `Thandor-Patch#5` |
| License / InfoBefore / InfoAfter | none (no licence or readme page, no patch notes inside the installer) |
| UninstallFilesDir | `{app}` (the uninstaller goes to `{app}\unins000.exe/.dat`) |
| DefaultUserInfoName/Org | `{sysuserinfoname}` / `{sysuserinfoorg}` (Inno defaults; no user-info page) |
| Language | one entry, `german`. Dialog font Arial / Verdana. These are the standard German messages "German translation maintained by Michael Reitz" |
| Custom messages | the 9 Inno standard ones (`NameAndVersion`, `CreateDesktopIcon`, ...) |
| [Code] | none (CompiledCodeText is empty) |

### Files, icons, registry

| Entry | Detail |
|---|---|
| `{app}\PATCH00.PCK` | 1,083,584 bytes, MD5 `da4c0a48...1a5f`, timestamp 2000-06-25. The installed copy is identical (MD5 checked) |
| `{app}\Thandor-World.url` | 52 bytes, `[InternetShortcut] URL=http://www.thandor-world.de/`, dated 2009-03-26 |
| `{app}\thandor.exe` | 1,642,496 bytes, MD5 `bd565d4c8ac0207f187a553900bf8873`, dated 2009-03-26 (stored with Inno's x86 call filter). **The installed `C:\Program Files (x86)\Thandor\thandor.exe` has exactly this MD5**, so the installed game is Patch 5 / 1.05 |
| Uninstaller file entry | file type `ftUninstExe`, so Uninstallable=yes **(guess from the entry; the option bits were not fully decoded)** |
| [Icons] | `{group}\Deutsche Thandor-Homepage. Inklusive Forum!`, which points to `{app}\Thandor-World.url` |
| [Registry], [INI], [Run], [InstallDelete] | none (all counts are 0) |

How it finds the installation: only through the default dir `HKLM\Software\Thandor\GamePath`. This key **does not
exist** on this PC, and the original 1999 setup never writes it. The original setup writes
`HKLM\SOFTWARE\WOW6432Node\Planet4\Thandor` with `Pfad` (here the stale `E:\Thandor`) and `CD` (`G:`). So Patch 5
falls back to `{pf}\Thandor` (32-bit Inno: `C:\Program Files (x86)\Thandor`) and the user confirms or browses on
the directory page. There is no check that the chosen folder really contains Thandor.

Uninstall: the standard Inno uninstaller deletes the three files it installed. It does **not** restore the
previous `thandor.exe` or `PATCH00.PCK`, because Inno has no backup of overwritten files. After uninstalling,
the game has no exe at all. This is the weak point we must do better on.

### Wizard pages (German) and look

The installer uses the classic Inno 5 wizard (Windows 2000/XP style): a grey dialog of about 497x360 px.
The pages:

1. **Welcome**: the large 164x314 picture on the left on white. Title "Willkommen zum Thandor Patch 5
   Setup-Assistenten", text "Dieser Assistent wird jetzt Version 1.05 auf Ihrem Computer installieren. Sie sollten
   alle anderen Anwendungen beenden, bevor Sie mit dem Setup fortfahren." Then "Weiter" zum Fortfahren, "Abbrechen"
   zum Verlassen.
2. **Ziel-Ordner wählen**: "Wohin soll Thandor Patch 5 installiert werden?" with "Durchsuchen ..." and the
   required space ("Mindestens [mb] MB freier Speicherplatz ist erforderlich.").
3. **Startmenü-Ordner auswählen**: group "Thandor" **(guess: shown, because an [Icons] entry exists and the
   DisableProgramGroupPage bit could not be confirmed)**.
4. **Bereit zur Installation**: "Das Setup ist jetzt bereit, Thandor Patch 5 auf Ihrem Computer zu installieren."
   The memo shows Ziel-Ordner and Startmenü-Ordner.
5. **Installiere ...**: "Warten Sie bitte während Thandor Patch 5 auf Ihrem Computer installiert wird."
6. **Finished**: the large picture again, "Beenden des Thandor Patch 5 Setup-Assistenten" / "Setup hat die
   Installation von Thandor Patch 5 auf Ihrem Computer abgeschlossen. ..." and the "Fertigstellen" button.

Inner pages have a white header strip with bold title and subtitle, and the **55x55 small image top right**: a
golden Thandor emblem (a four-pointed ring with a skull or helmet) on black.

The large image (see `original_wizard_image_164x314.png`), from top to bottom:

- the silver winged "THANDOR – DIE INVASION" logo,
- "PATCH 5" in metallic letters,
- in-game artwork: two hover tanks and an excavator unit on green grass,
- "HTTP://WWW.THANDOR-WORLD.DE" at the bottom.

Both images are 16-bit BMPs.

The complete German message set is in `original_patch5_strings.txt`. Inno ships the same texts (in a newer
revision) as `Languages\German.isl`, so we get the identical wording for free.

---

## 2. Versions

### What the original reports

| Source | Value |
|---|---|
| Patch 5 installer | AppVerName **"Version 1.05"**, AppName "Thandor Patch 5", files dated 2009-03-26 (the patch is by Thandor-World.de together with Innonics) |
| `thandor.exe` (installed) | **No VERSIONINFO resource at all.** It has only an icon group and five icons. MD5 matches the exe in Patch 5 |
| Strings in `thandor.exe` | UTF-16 `"1.5.45"` at file offset 1110140. This is the "game version" placed into the network session title (see below). There is no "1.05" anywhere in the exe |
| `liesmich.txt` (1999-12-26) | header "THANDOR - DIE INVASION (c) Innonics GmbH Ver. 1.0" (retail readme, older than any patch) |
| `UNINST00.LOG` and the registry | original 1999 setup ("Setup Specialist"): product "Thandor", version **1.04.92**, company Planet4; uninstall key `HKLM\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Thandor 1.04.92` |
| Registry `HKLM\SOFTWARE\WOW6432Node\Planet4\Thandor` | `Pfad=E:\Thandor` (stale), `CD=G:`. There is no `Thandor_is1` or `Thandor Patch 5_is1` key, so Patch 5 was not installed through its installer on this PC. The exe is byte-identical anyway (its file date 2002-04-27 suggests it came from a re-release or copy) |
| Data packs | `PATCH00.PCK` = Patch 5's file (1,083,584 bytes: fonts and the `texte\*.str` text pages, no version text). **`PATCH01.PCK` (2010-10-15) is not a PCK but a RAR archive that contains a `PATCH00.PCK`.** It is junk from some download. The game probes `PATCH00..PATCH99` (`src/platform/bootstrap/runtime.cpp:674-682`). The installer must leave it alone, but this is worth knowing |

**Conclusion:** the last official version is **1.05 = Patch 5** (its own naming is "Version 1.05"; there is no
"1.0.5" spelling). A web search finds no later patch. Community pages also call 1.05 the last version
(e.g. the phobetor.de forum thread "Thandor Patch auf 1.05"). The 1999 retail build reported 1.04.92 in its setup.

The bump "1.0.6" therefore matches the series, with two caveats:

- To match the original spelling, the wizard could say "Version 1.06". **Suggestion:** use the numeric version
  1.0.6 (resource `1,0,6,0`) and display "1.06" in the installer, so the name reads like the Patch 5 installer. The
  owner decides.
- Our exe has no version resource yet, so 1.0.6 is the first version our build reports.

### Version places in our code

| # | Place | What | May change to 1.0.6? |
|---|---|---|---|
| 1 | `CMakeLists.txt:2` `project(thandor_curated_reference C CXX)` | no version | **Yes: make this the single source**, `project(... VERSION 1.0.6 ...)` |
| 2 | (missing) exe VERSIONINFO resource | does not exist. There is no `.rc` in the repo; the window icon is loaded at runtime from `thandor.ico` (`src/platform/sdl3/window_icon.cpp:42`) | **Yes: new**, generated from #1 (FileVersion/ProductVersion 1.0.6.0, ProductName "Thandor", FileDescription "Thandor – Open Thandor 1.0.6") |
| 3 | `thandor.log` | logs the SDL version only (`src/platform/sdl3/platform.cpp:111`) | **Yes: add** one line "Open Thandor 1.0.6" at start (useful for crash reports) |
| 4 | `src/network/protocol/lobby.cpp:15-16` `g_GameVersionUtf16 = L"1.5.45"`, used at `lobby.cpp:294-297` | the host puts it into text 0x211A (`include/thandor/network/protocol/lobby.h:45`, "selector 0 = game version") and sends the result as `sessionTitleUtf16[20]` in packet 0x50001 (`include/thandor/network/protocol/types.h:156`) to every original or Open Thandor client that browses sessions | **Do NOT change.** It is the original exe's build string and goes over the wire. Nothing compares it (only display), but players of the original see it in the session list, and keeping "1.5.45" keeps us identical to a Patch 5 host. A changed value would also change the network-test traffic |
| 5 | `include/thandor/network/protocol/mailbox.h:45` `FRONTEND_PROTOCOL_MAGIC 0x2931`; `include/thandor/network/protocol/lobby.h:23` `FRONTEND_SEQUENCE_TOKEN_HIGH_WORD 0x12340000` | discovery/handshake checks (`lobby.cpp:235`, `lobby.cpp:288-290`) | **Must NOT change**: compatibility with the original 1.05 in LAN games |
| 6 | Savegame package header: `src/gameplay/session/savegame.cpp:252-275` (`InGameSaveGame_CreatePackage`: "pck\0", size 0x200, version 1, format 0x10000) | `.sve` files in `save\` | **Must NOT change**: saves of the original and of ours must stay interchangeable |
| 7 | PCK converter versions (`PCK_CONVERTER_*`, e.g. `src/gameplay/session/level.cpp:48`, `src/assets/sprite/catalog.cpp:60`), FLM `MOVIE_FLM_CONVERTER_VERSION` | data format checks of the original packs | **Must NOT change**: data format |
| 8 | `src/platform/bootstrap/runtime.cpp:62` window title `L"Thandor"` | taskbar and window title | Keep "Thandor" **(suggestion)**. A version in the title is possible but not like the original |
| 9 | Main menu | the original shows no version text **(guess: no version string exists in exe or `PATCH00.PCK` texts)** | Optional, later, owner's decision. Pixel checks (`tools/test`) compare the menu, so a menu text would need new references |

---

## 3. What our installer installs, and what it leaves alone

**Installs (into the existing game folder):**

| File | Source | Note |
|---|---|---|
| `thandor.exe` | the GCC build, preset **`mingw-release`** (`THANDOR_DEV_TOOLS=OFF`, `build-mingw-release\thandor.exe`) | x64 PE32+, image base 0x10000000, not ASLR. It imports only Windows DLLs, the UCRT (`api-ms-win-crt-*`), `dbghelp` and `SDL3.dll`. **Never ship the `mingw-test` build** (dev tools, `OPEN_THANDOR_*` variables). The test build is 35.7 MB with debug info; `strip --strip-debug` (keeps the symbol table, addresses unchanged) should shrink it a lot. **(guess: verify that `thandor.sym` still matches, `src/platform/bootstrap/image.cpp:235`)** |
| `SDL3.dll` | copied by the build (`CMakeLists.txt:361`), vcpkg `x64-mingw-dynamic` | imports only system DLLs and the UCRT (checked with `objdump -p`), so no `libgcc`/`libwinpthread` is needed. It must be built with Vulkan (`sdl3[vulkan]`), otherwise only DX12 and Software are offered |
| `thandor.sym` | build output (`CMakeLists.txt:326-331`) | about 380 KB. Gives function names in `crash.log`/`hang.log`. **Recommended to ship** (small, and helps with bug reports) |
| docs (optional) | e.g. `OpenThandor-liesmich.txt` (German: what changed, renderer choice, `thandor.ini`, how to uninstall) plus the licence notice for SDL3 (zlib licence, `LICENSE-SDL3.txt`) | do not overwrite the original `liesmich.txt` |

**Leaves alone (never overwritten or deleted, also not on uninstall):**

- `*.PCK` (including `PATCH00.PCK` and the junk `PATCH01.PCK`), `flm\`, `save\` and every `*.sve`
- `thandor.dat` (the game only reads it, see `docs/BUILDING.md:156-158`)
- `thandor.ini`, `thandor.log`, `crash.log`, `hang.log`: runtime files. Uninstall leaves them; optionally ask
  "Einstellungen und Protokolle löschen?" (default: keep)
- `thandor.ico`, `liesmich.txt`, `thandor.url`, the old DirectX wrapper DLLs (`D3D8.dll`, `DDraw.dll`, ...; our exe
  does not load them), `Setup\`, `UNINST00.LOG`, and the `Planet4` registry key

**Backup of the original exe (our version of uninstall):**

- Before copying, in `[Code]` `CurStepChanged(ssInstall)`: if `{app}\thandor.exe` is not ours, copy it to
  `{app}\thandor-1.05.exe` (or `{app}\OpenThandor\backup\thandor.exe`) once. Don't overwrite an existing backup.
  Clear the read-only attribute first; the installed original is `-r-x`.
- Detect "ours" by our VERSIONINFO, or by MD5 ≠ `bd565d4c8ac0207f187a553900bf8873` and no Open Thandor resource.
  If the old exe is not the Patch 5 exe (unknown MD5), show "Es wurde nicht Version 1.05 gefunden. Trotzdem
  fortfahren?" The backup is still made.
- Uninstall (`CurUninstallStepChanged(usPostUninstall)`): delete our `thandor.exe`, `SDL3.dll` and `thandor.sym`,
  then rename the backup back to `thandor.exe` and set read-only again. Result: the game folder is back on 1.05.
- `[Files]` flags for `thandor.exe`: `ignoreversion overwritereadonly uninsneveruninstall`, because the
  uninstall is handled by our code. Otherwise Inno deletes the exe before we restore it. (The alternative is the
  default delete plus restore afterwards; both work, decide in WP3.)

**Prerequisites and checks:**

- **64-bit Windows**: `ArchitecturesAllowed=x64compatible` (Inno shows its own German message). On a 32-bit
  Windows the patch refuses to install.
- **Windows 10 or newer** recommended (`MinVersion=10.0`). The UCRT is part of Windows 10/11. On 7/8.1 it would need
  KB2999226 and SDL3 targets Windows 7+, but the GPU path needs Vulkan/DX12. **Suggestion: require 10.**
- **GPU**: no prerequisite. The game falls back to the software renderer (`docs/BUILDING.md:281-283`). Nothing to
  install, and no VC++ redistributable is needed (GCC runtimes are static).
- **Existing installation**: the target must contain `DATEN.PCK` and `thandor.exe`. `PATCH00.PCK` should be there
  (Patch 5 data). Without it, warn "Bitte zuerst den offiziellen Patch 5 installieren"; whether to block is a
  decision. **(guess: our exe needs the Patch 5 texts and fonts from `PATCH00.PCK`)**
- **Write access (important)**: our exe writes `thandor.ini`, `thandor.log`, `crash.log` and `save\*.sve` next to
  itself (`src/platform/bootstrap/image.cpp:41,443`, `docs/BUILDING.md:149`).
  - The 32-bit original was redirected by UAC file virtualization to `%LOCALAPPDATA%\VirtualStore`. **A 64-bit
    process gets no virtualization**, so in `C:\Program Files (x86)\Thandor` (ACL: Users = read/execute only,
    checked with `icacls`) a normal user cannot save settings or games.
  - Options:
    - **(a)** The installer grants `users-modify` on `{app}` (Inno `[Dirs] Name: "{app}"; Permissions:
      users-modify` and the same for `{app}\save`). This is a classic approach for old games, and is the
      recommended one (no code change).
    - **(b)** A code change: settings, logs and saves go to `%APPDATA%\Thandor`. This is larger, changes behaviour
      and the test tools, and leaves old saves behind.
  - Uninstall does not revert the ACL. Mention this in the readme.
  - Also: saves from an earlier VirtualStore (`%LOCALAPPDATA%\VirtualStore\Program Files (x86)\Thandor\save`)
    would be invisible to the x64 exe. There is none on this PC. A `[Code]` step could offer to copy them
    **(optional)**.
- **Game running**: `AppMutex` is not possible (the original has no mutex we know). Use Inno 6
  `CloseApplications=yes` (Restart Manager) or a check for a running `thandor.exe` instead.

**Side finding (not installer scope):** `src/platform/bootstrap/runtime.cpp:602-613` opens
`HKLM\Software\Planet4\Thandor` with `KEY_READ` only. In the 64-bit process this reads the 64-bit view, and the key
lives in `WOW6432Node`. So the `CD` value (movie lookup on the CD) is never found by the x64 build. The fix would be
`KEY_READ | KEY_WOW64_32KEY`. Pre-existing; flag separately.

---

## 4. Tool choice

| | Inno Setup 6 | NSIS 3 (MUI2) | WiX (MSI) |
|---|---|---|---|
| Look like the original | **Same tool as Patch 5.** `WizardStyle=classic` gives the same page layout, with the 164x314 left image (`WizardImageFile`) and the small image (`WizardSmallImageFile`) | MUI2 is similar (welcome bitmap 164x314, header 150x57) but looks different: other fonts, header layout, buttons | MSI UI (WixUI) looks completely different (493x312 banner and dialog bitmaps) |
| German | `Languages\German.isl` (same translation lineage as Patch 5) | `German.nlf` / MUI German | WixUI de-de |
| Uninstaller | built in, `unins000.exe` in `{app}` like Patch 5, Pascal `[Code]` hooks for backup and restore | built in, script-written | MSI. Restoring an overwritten file needs custom actions, which is awkward; MSI also dislikes patching files it does not own |
| Version info and registry | `VersionInfoVersion`, `AppVersion`, `[Registry]`; reading `HKLM32\...\Planet4\Thandor` with `{reg:...}` is easy | yes | yes |
| 64-bit | `ArchitecturesAllowed/InstallIn64BitMode=x64compatible` | `x64.nsh` | native |
| Command-line compiler | `ISCC.exe script.iss /DAppVersion=1.0.6` | `makensis /DVERSION=...` | `wix build` |
| Licence | free. **Note (verify before use):** since Inno Setup 6.3 (2024) the licence says commercial use needs a paid licence; non-commercial and open-source hobby use stays free | zlib/libpng, free | MS-RL; WiX v5+ adds the "Open Source Maintenance Fee" for commercial users **(verify)** |

**Recommendation: Inno Setup 6 (latest 6.x), `WizardStyle=classic`, German only.** It is the same framework as
the original, so the "quasi normal Windows installer" look comes for free. Backup and restore is a few lines of
Pascal. None of the three tools is installed on this PC (checked Program Files and PATH); **installing Inno Setup
needs the owner's permission.** It is a single installer from jrsoftware.org, and a portable `ISCC` in
`tools/installer/` is possible too.

Inno 6 also accepts several image files for high DPI. With `WizardImageFile=wizard-164.bmp,wizard-328.bmp`, Inno
picks the best one. Recommended sizes per Inno docs: 164x314 (100 %) up to 410x797 (250 %) for the large image,
55x55/55x58 up to 138x140 for the small one **(exact list to verify in the Inno 6 docs)**. Use 24-bit BMPs.

Sketch of the script (`tools/installer/thandor-patch.iss`):

```ini
#define AppVersion "1.0.6"          ; passed in by CMake: /DAppVersion=...
[Setup]
AppId={{<new GUID>}             ; new id, not "Thandor Patch 5"
AppName=Thandor Patch 6 (Open Thandor)   ; owner decides; Patch 5 used "Thandor Patch 5"
AppVerName=Version 1.06
AppVersion={#AppVersion}
AppPublisher=Open Thandor
VersionInfoVersion={#AppVersion}.0
DefaultDirName={code:FindThandorDir}   ; Planet4\Pfad (32-bit view) -> Software\Thandor\GamePath -> {commonpf32}\Thandor, each only if DATEN.PCK exists
DirExistsWarning=no
DisableProgramGroupPage=yes          ; or keep the group page like Patch 5
CreateUninstallRegKey=yes
UninstallFilesDir={app}\OpenThandor  ; or {app} like Patch 5 (would collide with unins000 of Patch 5)
UninstallDisplayName=Thandor 1.06 (Open Thandor)
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
WizardStyle=classic
WizardImageFile=wizard-164.bmp,wizard-328.bmp
WizardSmallImageFile=small-55.bmp,small-110.bmp
SetupIconFile=...                    ; see the copyright note in section 5
OutputBaseFilename=Thandor-Patch-6
Compression=lzma2/max
SolidCompression=yes
PrivilegesRequired=admin
[Languages]
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
[Dirs]
Name: "{app}"; Permissions: users-modify
Name: "{app}\save"; Permissions: users-modify
[Files]
Source: "stage\thandor.exe"; DestDir: "{app}"; Flags: ignoreversion overwritereadonly uninsneveruninstall
Source: "stage\SDL3.dll";    DestDir: "{app}"; Flags: ignoreversion
Source: "stage\thandor.sym"; DestDir: "{app}"; Flags: ignoreversion
Source: "stage\OpenThandor-liesmich.txt"; DestDir: "{app}"; Flags: isreadme
[Icons]   ; optional, like Patch 5
Name: "{group}\Thandor"; Filename: "{app}\thandor.exe"; WorkingDir: "{app}"
[Code]   ; FindThandorDir, NextButtonClick(wpSelectDir) check for DATEN.PCK/thandor.exe,
         ; CurStepChanged(ssInstall) backup, CurUninstallStepChanged(usPostUninstall) restore
```

---

## 5. The picture

**Source:** the game's own capture. A `mingw-test` build (dev tools) with
`OPEN_THANDOR_WINDOWED=1 OPEN_THANDOR_SCRIPT=<script>` uses the script command `shot` (writes
`shots\script_NNNN.bmp`), or `OPEN_THANDOR_AUTOSHOT=<ms>` (`docs/BUILDING.md:347-348`). The shot comes from the
framebuffer at the selected resolution. Use a high resolution (1920x1080 or more) and the GPU smooth renderer so the
downscaled crop stays sharp.

**Suggested scenes** (the original shows battle units on terrain under the logo):

1. **Recommended:** a skirmish with a strong base and moving units, e.g. Niflheim (snow) or a green map, from the
   default camera with the side panel cropped off. Script: start with `-NOINTRO -KARTE="<map>"`, build or select
   units, take `shot` after a few seconds. The mock-up `mockup_wizard_image_164x314.png` shows the crop from an
   existing soak shot (Niflheim base).
2. A campaign intro moment with explosions (more drama, harder to time).
3. The main menu hangar with the mechs (static and easy to reproduce, but less "game").

**Composition** like Patch 5: a portrait crop (164:314, roughly 0.52 aspect) from the playfield (exclude the right
command panel), then a darker band at the top with the Thandor logo and "1.06" (or "Open Thandor 1.06"), and the
project URL at the bottom.

- The logo can be cut from the game's own menu graphics, or the top of `original_wizard_image_164x314.png` can be
  reused **(copyright: the logo and artwork belong to Innonics/Thandor-World. Using them in a fan patch is what
  Patch 5 did, but committing them to a public repo is the owner's decision)**.
- Small image (55x55 / 55x58): reuse the gold emblem style, e.g. the faction emblem from the game GUI, or a unit
  close-up.
- Make the images with a small Python/Pillow script (`tools/installer/make_wizard_images.py`: crop, scale with
  Lanczos, write 24-bit BMP in all DPI sizes), so it can be repeated after a new screenshot.

**Copyright note:** a screenshot shows original game assets. Options:

- (a) commit only the script and keep the BMPs out of git (generated from the user's own installation at build
  time, like the game data);
- (b) commit the finished BMPs.

The same applies to the installer icon (`SetupIconFile`): the original `thandor.ico` comes from the installation.
At build time it could be taken from the game folder given to CMake.

---

## 6. Build integration

**Single version source:** `CMakeLists.txt:2` becomes `project(thandor_curated_reference VERSION 1.0.6 LANGUAGES C CXX)`.
From there:

- `configure_file(include/thandor/version.h.in ${CMAKE_BINARY_DIR}/generated/thandor/version.h)` with
  `THANDOR_VERSION_STRING "1.0.6"` and `THANDOR_VERSION_MAJOR/MINOR/PATCH`. It is used by the log line
  (`platform.cpp`) and, if wanted, by `SDL_SetAppMetadata("Thandor", "1.0.6", ...)`.
- A new `src/platform/bootstrap/thandor.rc.in` (VERSIONINFO, and optionally an icon if the owner accepts an icon in
  the repo), added to `add_executable(thandor ...)` at `CMakeLists.txt:304-307`.
  - MinGW: CMake's RC language uses `windres`. **Risk:** `docs/BUILDING.md:67` notes that vcpkg's windres step fails
    with spaces in the path, and the repo path contains a space ("Gerrit Bluemel"). CMake's own windres call is
    normally quoted correctly **(verify)**.
  - MSVC: `rc.exe` (already set up in `cmake/msvc-x64.cmake:43`).
- Optionally a top-level `VERSION` file read by CMake (`file(READ ...)`). It is easier for scripts, but
  `project(VERSION)` is enough.

**Installer target**, only if ISCC is found (like the optional fxc/dxc):

```cmake
find_program(THANDOR_ISCC ISCC HINTS "$ENV{ProgramFiles\(x86\)}/Inno Setup 6" "$ENV{ProgramFiles}/Inno Setup 6")
if(THANDOR_ISCC AND NOT THANDOR_DEV_TOOLS)
  add_custom_target(installer
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/installer/stage
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:thandor> $<TARGET_FILE_DIR:thandor>/SDL3.dll
            $<TARGET_FILE_DIR:thandor>/thandor.sym ${CMAKE_BINARY_DIR}/installer/stage
    COMMAND ${CMAKE_STRIP} --strip-debug ${CMAKE_BINARY_DIR}/installer/stage/thandor.exe   # optional, see 3
    COMMAND ${THANDOR_ISCC} /Q /DAppVersion=${PROJECT_VERSION} /DStage=${CMAKE_BINARY_DIR}/installer/stage
            /O${CMAKE_BINARY_DIR}/installer ${CMAKE_SOURCE_DIR}/tools/installer/thandor-patch.iss
    DEPENDS thandor VERBATIM)
endif()
```

- New files: `tools/installer/thandor-patch.iss`, `tools/installer/OpenThandor-liesmich.txt` (German),
  `tools/installer/make_wizard_images.py`, `tools/installer/README.md` (or a section in `docs/BUILDING.md`).
- The `installer` target refuses a dev-tools build (the `NOT THANDOR_DEV_TOOLS` condition above). Use it from the
  `mingw-release` preset: `cmake --build --preset mingw-release --target installer`, which produces
  `build-mingw-release/installer/Thandor-Patch-6.exe`.
- An alternative to the CMake target is `tools/installer/build_installer.py <build dir>` (collect, strip, call ISCC).
  The CMake target is preferred, because it has the version and the paths.
- Not signed (Patch 5 was not either). Expect SmartScreen and antivirus false positives; mention them in the readme.

---

## 7. Step plan

| WP | Content | Size | Verification |
|---|---|---|---|
| 0 | Owner decisions: (1) installing Inno Setup 6, (2) name and display ("Thandor Patch 6"/"Version 1.06" vs "1.0.6"), (3) images and icon committed or generated, (4) write permission option (a) ACL vs (b) %APPDATA%, (5) require Patch 5 / `PATCH00.PCK` or only warn, (6) Start menu entry yes/no | – | – |
| 1 | Version source: `project(VERSION 1.0.6)`, `version.h.in`, log line "Open Thandor 1.0.6" | S | Build `mingw-test` and `mingw-release`; `thandor.log` shows the line; all `tools/test/run_checks.py` checks unchanged (no behaviour change; `lobby.cpp` "1.5.45" untouched) |
| 2 | VERSIONINFO resource (`thandor.rc.in`), GCC windres and MSVC rc | S | `objdump -p`/PowerShell `(Get-Item thandor.exe).VersionInfo` shows 1.0.6.0; Explorer properties tab; both compilers build; `pe_not_large_address_aware` and the fixed base still OK; `thandor.sym` still accepted |
| 3 | `tools/installer/thandor-patch.iss` (classic wizard, German, dir detection with DATEN.PCK check, x64/Win10 check, `[Dirs]` ACL, backup and restore `[Code]`, running-game check) and the German readme | M | ISCC compiles without warnings. **Test only on copies**: copy `C:\Program Files (x86)\Thandor` to e.g. `C:\ThandorTest\Thandor` and `D:\...\Thandor` with spaces, select it on the directory page. Never install into `C:\Program Files (x86)\Thandor` |
| 4 | Pictures: autoshot script in `tools/installer/` (or `tools/test`), `make_wizard_images.py`, all DPI sizes, small image and icon | S | Look at the wizard at 100 %, 150 % and 200 % DPI; compare side by side with `original_wizard_image_164x314.png` |
| 5 | CMake `installer` target (stage, strip, ISCC with version) | S | `cmake --build --preset mingw-release --target installer` produces `Thandor-Patch-6.exe`; its version resource is 1.0.6 |
| 6 | Install and uninstall tests on copies of the game folder (record the MD5s before and after) | M | (1) Install into the copy: `thandor.exe` = ours, `SDL3.dll` and `thandor.sym` present, backup `thandor-1.05.exe` has MD5 `bd565d4c...8873`, PCK/dat/ini/save MD5s unchanged; the game starts as a **non-admin user** and saves settings and a game (proves the ACL). (2) Reinstall over itself: the backup is not overwritten by our exe. (3) Uninstall: `thandor.exe` MD5 is back to `bd565d4c...`, read-only attribute restored, `SDL3.dll`/`thandor.sym`/uninstaller gone, saves and `thandor.ini` still there, the Apps & Features entry is gone. (4) The wrong folder (no DATEN.PCK) is refused. (5) Silent mode `/SILENT /DIR=...` for an automated test with `tools/test`. (6) Optional: a 32-bit Windows VM refuses to install |
| 7 | Docs: `README.md:93-95` (playing: installer as the normal path), `docs/BUILDING.md` (installer target), CHANGELOG | S | Review |

Sizes: S = less than half a day, M = about a day. Total roughly 3–4 days including the owner decisions and image work.

**Test safety rule for every WP:** all installs run against copies of the game folder (the test tools already
create `<game dir>_chk_*` copies). The real installation and its registry keys are only read. The original
`Thandor-Patch-5.exe` is never run, and for comparison its pictures and texts in this folder are enough.
