"""Environment for every game process the test tools start.

The checks compare pixels and state hashes with references made by the software rasterizer, so the game runs
the software renderer (OPEN_THANDOR_GPU=0) unless the caller's environment already names a renderer
(OPEN_THANDOR_GPU=vulkan|d3d12|1|compare|0, see docs/BUILDING.md). The developer tools' window
(OPEN_THANDOR_WINDOWED=1, set by the scripts) stays a window whatever the saved display mode kind is. The GPU
renderers' UI scale is 1 (OPEN_THANDOR_UI_SCALE=1) unless the caller sets it, so a GPU run draws at the
framebuffer size as the software references do. Frames are not paced (OPEN_THANDOR_VSYNC=0,
OPEN_THANDOR_FRAME_LIMIT=0) unless the caller sets them, so the tests run as fast as before.
Every game also starts minimized and without taking the focus (OPEN_THANDOR_WINDOW_MINIMIZED=1, implies the
developer tools' window; it keeps running and drawing) unless OPEN_THANDOR_TEST_VISIBLE=1 is set, to watch it.
A minimized game is also muted (SDL_AUDIO_DRIVER=dummy: the audio device opens and mixes as usual, nothing is
played) unless OPEN_THANDOR_TEST_SOUND=1 or SDL_AUDIO_DRIVER is already set.
Local sessions run the original pathing (OPEN_THANDOR_PATHING=original, wins over [game] pathing, whose default is
improved) unless the caller sets it, so the determinism references, the AI hash and the other checks keep comparing
the original movement; the scripts' --pathing option (add_pathing_argument / apply_pathing) sets it for every game
they and the tools they start run: original, improved, or setting (no override: thandor.ini decides)."""
import os

PATHING_CHOICES = ('original', 'improved', 'setting')


def add_pathing_argument(parser):
    """Adds --pathing original|improved|setting to an argparse parser (default: OPEN_THANDOR_PATHING of the caller's
    environment, else original)."""
    parser.add_argument('--pathing', choices=PATHING_CHOICES,
                        help='pathing mode of the local test sessions (OPEN_THANDOR_PATHING; default: the '
                             "environment's, else original; setting: [game] pathing of the copy's thandor.ini)")


def apply_pathing(choice):
    """Puts the --pathing choice into os.environ, so game_env and the tools started from here (they inherit the
    environment) pass it on; None keeps the caller's environment (game_env then defaults to original)."""
    if choice:
        os.environ['OPEN_THANDOR_PATHING'] = choice


def pathing_from_argv(argv):
    """For scripts without argparse: removes --pathing=X / --pathing X from argv, applies it and returns argv."""
    rest = []
    index = 0
    while index < len(argv):
        arg = argv[index]
        if arg == '--pathing' and index + 1 < len(argv):
            choice = argv[index + 1]
            index += 2
        elif arg.startswith('--pathing='):
            choice = arg.split('=', 1)[1]
            index += 1
        else:
            rest.append(arg)
            index += 1
            continue
        if choice not in PATHING_CHOICES:
            raise SystemExit('--pathing must be one of ' + ', '.join(PATHING_CHOICES))
        apply_pathing(choice)
    return rest


def game_env(**variables):
    """os.environ plus OPEN_THANDOR_GPU=0, OPEN_THANDOR_UI_SCALE=1, OPEN_THANDOR_VSYNC=0, OPEN_THANDOR_FRAME_LIMIT=0 and OPEN_THANDOR_PATHING=original (when not set), OPEN_THANDOR_WINDOW_MINIMIZED=1 (unless
    OPEN_THANDOR_TEST_VISIBLE=1 or already set) plus the given variables."""
    env = dict(os.environ)
    env.setdefault('OPEN_THANDOR_GPU', '0')
    env.setdefault('OPEN_THANDOR_UI_SCALE', '1')
    env.setdefault('OPEN_THANDOR_VSYNC', '0')
    env.setdefault('OPEN_THANDOR_FRAME_LIMIT', '0')
    env.setdefault('OPEN_THANDOR_PATHING', 'original')
    if env.get('OPEN_THANDOR_TEST_VISIBLE') != '1':
        env.setdefault('OPEN_THANDOR_WINDOW_MINIMIZED', '1')
        if env.get('OPEN_THANDOR_TEST_SOUND') != '1':
            env.setdefault('SDL_AUDIO_DRIVER', 'dummy')
    env.update(variables)
    return env
