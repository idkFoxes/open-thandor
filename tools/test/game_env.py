"""Environment for every game process the test tools start.

The checks compare pixels and state hashes with references made by the software rasterizer, so the game runs
the software renderer (OPEN_THANDOR_GPU=0) unless the caller's environment already names a renderer
(OPEN_THANDOR_GPU=vulkan|d3d12|1|compare|0, see docs/BUILDING.md). The developer tools' window
(OPEN_THANDOR_WINDOWED=1, set by the scripts) stays a window whatever the saved display mode kind is. The GPU
renderers' UI scale is 1 (OPEN_THANDOR_UI_SCALE=1) unless the caller sets it, so a GPU run draws at the
framebuffer size as the software references do.
Every game also starts minimized and without taking the focus (OPEN_THANDOR_WINDOW_MINIMIZED=1, implies the
developer tools' window; it keeps running and drawing) unless OPEN_THANDOR_TEST_VISIBLE=1 is set, to watch it."""
import os


def game_env(**variables):
    """os.environ plus OPEN_THANDOR_GPU=0 and OPEN_THANDOR_UI_SCALE=1 (when not set), OPEN_THANDOR_WINDOW_MINIMIZED=1 (unless
    OPEN_THANDOR_TEST_VISIBLE=1 or already set) plus the given variables."""
    env = dict(os.environ)
    env.setdefault('OPEN_THANDOR_GPU', '0')
    env.setdefault('OPEN_THANDOR_UI_SCALE', '1')
    if env.get('OPEN_THANDOR_TEST_VISIBLE') != '1':
        env.setdefault('OPEN_THANDOR_WINDOW_MINIMIZED', '1')
    env.update(variables)
    return env
