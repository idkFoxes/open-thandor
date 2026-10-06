"""Takes the in-game screenshots for the installer's wizard image (see make_wizard_images.py).

usage: take_wizard_shots.py GAME_DIR [--exe TEST_EXE] [--maps hansolo:12,niflheim,...] [--out DIR]
                            [--width 1920] [--height 1080] [--api vulkan|d3d12] [--timeout S]

For every map: makes a linked copy GAME_DIR_chk_wizshot (tools/test/run_checks.make_copy) of the test build
(default build-mingw-test/thandor.exe, else build-test/thandor.exe; the developer tools are needed for the input
script), writes its thandor.ini with a WIDTH x HEIGHT window, the GPU renderer (smooth rasterization, UI scale 1,
high texture quality, sound off) and runs tools/installer/wizard_showcase.txt (default; wizard_shot.txt for stock maps) with -NOINTRO -KARTE="<map>" (a
skirmish map) or, for <campaign>:<level>, -KARTE="-" with OPEN_THANDOR_CAMPAIGN / _LEVEL (a campaign level), or for "showcase" the
level of make_showcase_level.py (hansolo level 4 plus heavy units; --showcase-args are passed on) in a
visible window (OPEN_THANDOR_TEST_VISIBLE=1, the GPU renderer does not draw a minimized window the same way).
The script starts the skirmish, reveals the map and takes shots (shots\\script_NNNN.bmp); they are copied to
OUT/<map>_NN.bmp (default OUT: GAME_DIR_wizshots) together with a contact sheet OUT/sheet.png, and the copy is
removed. Pick a shot and pass it to make_wizard_images.py --shot.

The shots show the original game's art: keep them (and the images made from them) out of the repository."""
import argparse
import glob
import os
import shlex
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(REPO, 'tools', 'test'))
import run_checks  # noqa: E402
from gpu_compare import remove_copy  # noqa: E402

DEFAULT_MAPS = 'showcase'
PORT = 995

INI = """[display]
width = {width}
height = {height}
bits_per_pixel = 32
renderer = {api}
display_mode = window

[graphics]
texture_quality = high
gpu_rasterization = smooth
ui_scale = 1
vsync = on
frame_limit = 0

[sound]
effects = false
music = false
"""


def main():
    default_exe = os.path.join(REPO, 'build-mingw-test', 'thandor.exe')
    if not os.path.exists(default_exe):
        default_exe = os.path.join(REPO, 'build-test', 'thandor.exe')
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('game_dir')
    parser.add_argument('--exe', default=default_exe, help='test build (default: %(default)s)')
    parser.add_argument('--maps', default=DEFAULT_MAPS, help='comma-separated (default: %(default)s)')
    parser.add_argument('--script', default=os.path.join(HERE, 'wizard_showcase.txt'),
                        help='input script (default: %(default)s; wizard_shot.txt for stock maps)')
    parser.add_argument('--width', type=int, default=1920)
    parser.add_argument('--height', type=int, default=1080)
    parser.add_argument('--api', choices=('vulkan', 'd3d12'), default='vulkan')
    parser.add_argument('--timeout', type=int, default=150, help='seconds per map (default %(default)s)')
    parser.add_argument('--showcase-args', default='',
                        help='extra arguments for make_showcase_level.py (map name "showcase")')
    parser.add_argument('--tag', default='', help='suffix of the shot names')
    parser.add_argument('--out')
    args = parser.parse_args()

    run_checks.game = os.path.abspath(args.game_dir)
    out = os.path.abspath(args.out or run_checks.game.rstrip('\\/') + '_wizshots')
    os.makedirs(out, exist_ok=True)
    # visible window for this run only (game_env reads it from the environment)
    os.environ['OPEN_THANDOR_TEST_VISIBLE'] = '1'
    env = {'OPEN_THANDOR_GPU': args.api, 'OPEN_THANDOR_UI_SCALE': '1', 'OPEN_THANDOR_GPU_RASTER': 'smooth',
           'OPEN_THANDOR_SCRIPT': os.path.abspath(args.script)}
    taken = []
    for name in [m.strip() for m in args.maps.split(',') if m.strip()]:
        folder = run_checks.make_copy('wizshot', os.path.abspath(args.exe))
        try:
            for entry in ('thandor.dat', 'thandor.ini'):
                if os.path.exists(os.path.join(folder, entry)):
                    os.remove(os.path.join(folder, entry))
            with open(os.path.join(folder, 'thandor.ini'), 'w') as f:
                f.write(INI.format(width=args.width, height=args.height, api=args.api))
            shutil.rmtree(os.path.join(folder, 'shots'), ignore_errors=True)
            run_env = dict(env)
            if name == 'showcase':  # stock level plus a group of heavy units (make_showcase_level.py)
                level_dir = os.path.join(folder, 'level')
                if os.path.isjunction(level_dir):  # private copy: the junction's target is the shared game folder
                    subprocess.run('cmd /c rmdir "%s"' % level_dir, shell=True, capture_output=True)
                    shutil.copytree(os.path.join(run_checks.game, 'level'), level_dir)
                subprocess.run([sys.executable, os.path.join(HERE, 'make_showcase_level.py'), folder] +
                               shlex.split(args.showcase_args), check=True)
                arguments = '-NOINTRO -KARTE="wizshow."'
            elif ':' in name:  # a campaign level: -KARTE="-" opens "Choose game", the test aid starts the campaign
                campaign, level = name.split(':')
                run_env.update(OPEN_THANDOR_CAMPAIGN=campaign, OPEN_THANDOR_CAMPAIGN_LEVEL=level)
                arguments = '-NOINTRO -KARTE="-"'
            else:
                arguments = '-NOINTRO -KARTE="%s"' % name
            exited, crashed, log = run_checks.run_game(folder, arguments, run_env, args.timeout, PORT)
            modes = [l.strip() for l in log.splitlines() if 'display mode' in l]
            shots = sorted(glob.glob(os.path.join(folder, 'shots', 'script_*.bmp')))
            for k, shot in enumerate(shots):
                target = os.path.join(out, '%s%s_%02d.bmp' % (name.replace(':', ''), args.tag, k))
                shutil.copy(shot, target)
                taken.append(target)
            print('%s: %d shots, exited by itself: %s, crash/hang log: %s; %s' % (
                name, len(shots), exited, crashed, modes[-1] if modes else 'no display mode line'))
        finally:
            remove_copy(folder)
    if taken:
        from PIL import Image, ImageDraw
        columns, tw, th = 4, 480, 270
        sheet = Image.new('RGB', (columns * tw, th * ((len(taken) + columns - 1) // columns)))
        draw = ImageDraw.Draw(sheet)
        for k, path in enumerate(taken):
            x, y = (k % columns) * tw, (k // columns) * th
            sheet.paste(Image.open(path).convert('RGB').resize((tw, th), Image.LANCZOS), (x, y))
            draw.text((x + 6, y + 6), os.path.basename(path), fill=(255, 255, 0))
        sheet.save(os.path.join(out, 'sheet.png'))
    print('%d shots in %s' % (len(taken), out))
    return 0 if taken else 1


if __name__ == '__main__':
    sys.exit(main())
