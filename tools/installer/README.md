# Installer pictures (step 10, WP4)

The wizard images of the patch installer (Inno Setup 7, `WizardStyle=classic`) are in `images/` (committed by the
owner's decision, 2026-10-06; they show the original game's art: the Thandor logo from `gfx\panel\credits.gfx`
and an in-game screenshot). All files are 24-bit BMPs in every size of the Inno Setup 7 DPI tables (image area at
100 ... 250 % DPI, current layout and the one before Inno 6.6.0); Setup picks the best fitting one.

| Directive | Files (`images/`) |
|---|---|
| `WizardImageFile` (164:314, welcome and finish pages) | `wizard-image-164x314.bmp`, `-202x386`, `-240x459`, `-269x515`, `-290x556`, `-315x604`, `-336x643`, `-366x700`, `-403x772`, `-416x797`, `-430x824`, `-498x953`, `-534x1022` |
| `WizardSmallImageFile` (square, top right of the inner pages) | `wizard-small-image-55.bmp`, `-58`, `-71`, `-77`, `-85`, `-97`, `-103`, `-112`, `-116`, `-124`, `-129`, `-143`, `-147`, `-159` |

`images/wizard-files.txt` holds the two lines ready for the `[Setup]` section of `thandor-patch.iss` (file names
relative to `images/`).

## Making them again

1. `make_showcase_level.py` (called by step 2): campaign `hansolo` level 4 (green base) as a single game without
   its mission script, player 1's stock units replaced by a group of armed heavy units (81 rocket-pod tank, 84
   rocket-rack tank, 82 multi launcher, 101 heavy twin-cannon tank, 223 rocket walker) in front of the base. Unit
   weapons are separate army ids: the last id of a chassis group (89, 105, 141, 224, ...) is the bare chassis.
2. `take_wizard_shots.py GAME_DIR` (test build with the developer tools, `build-mingw-test`): in a linked copy
   `GAME_DIR_chk_wizshot` (private `level` folder, removed afterwards) starts that level in a visible 1920x1080
   Vulkan window (UI scale 1, smooth rasterization) with `wizard_showcase.txt`: zoom in with the mouse wheel,
   clear the selection (right click on empty ground), park the pointer on empty ground and then on the side panel
   (no hover frame), then the shots (`GAME_DIR_wizshots/showcase_NN.bmp`, plus `sheet.png`). `--maps` also takes
   stock maps or `campaign:level` with `--script wizard_shot.txt`.
3. `make_wizard_images.py GAME_DIR SHOT.bmp --crop X,Y,W,H --out tools/installer/images`: crops the playfield to
   164:314, adds the logo, the version (`--version`, default "1.0.7") and "OPEN THANDOR", writes all sizes and `wizard-files.txt`.
   The small image is the logo's globe on black. `--preview DIR --original PATCH5.png` adds PNG previews and
   `compare.png` (Patch 5's picture beside ours).

The committed set (2026-10-10, version "1.0.7"): a 2560x1440 GPU-native battle frame of the README screenshots
(`ot-scratch/readme-shots/raw/f_s2_gpunative_0006.bmp`, the frame behind `battle-1`) with
`--crop 1340,60,580,1110 --version 1.0.7`; the small images came out byte-identical to the earlier set. (The set of
2026-10-06, version "1.0.6", used `showcase_02.bmp` (5 wheel notches) with `--crop 840,200,460,880`; that shot no
longer exists.) Shots are not bit-reproducible (timing), so a new run gives a slightly different picture.
