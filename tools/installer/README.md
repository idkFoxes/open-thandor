# Installer pictures (step 10, WP4)

The wizard images of the patch installer (Inno Setup 7, `WizardStyle=classic`) are **generated, not committed**:
they show the original game's art (the Thandor logo from `gfx\panel\credits.gfx` and an in-game screenshot), so
they are made from the user's own Thandor installation like the game data. Whether finished images go into the
repository is the owner's decision (plan `docs/plans/step10_installer.md`, section 5).

1. `take_wizard_shots.py GAME_DIR` (test build with the developer tools, `build-mingw-test`): runs
   `wizard_shot.txt` on a few campaign levels and skirmish maps in a visible 1920x1080 Vulkan window (UI scale 1,
   smooth rasterization), reveals the map with the cheat and takes 8 shots per map into `GAME_DIR_wizshots`
   (plus `sheet.png`). Each map runs in a linked copy `GAME_DIR_chk_wizshot` that is removed afterwards.
2. `make_wizard_images.py GAME_DIR SHOT.bmp --crop X,Y,W,H`: crops the playfield to 164:314, puts the logo, the
   version ("1.0.6") and "OPEN THANDOR" on it and writes 24-bit BMPs in every size of the Inno Setup 7 DPI tables
   to `build-installer-images/` (ignored by git), with `wizard-files.txt` holding the `WizardImageFile=` /
   `WizardSmallImageFile=` lines. The small image is the globe of the logo on black. `--preview DIR` adds PNG
   previews and `compare.png` (with `--original`, Patch 5's picture beside ours).

The first images (2026-10-06) use campaign `hansolo` level 4, shot 2 (`hansolo4_02.bmp`, 26 s after the level
started; base and four vehicles on grass) with `--crop 665,60,501,960`. The shot is not bit-reproducible (the
level's AI and timing vary a little), so keep the chosen BMP next to the generated images.
