"""Environment for every game process the test tools start.

The checks compare pixels and state hashes with references made by the software rasterizer, so the game runs
the software renderer (OPEN_THANDOR_GPU=0) unless the caller's environment already names a renderer
(OPEN_THANDOR_GPU=vulkan|d3d12|1|compare|0, see docs/BUILDING.md). The developer tools' window
(OPEN_THANDOR_WINDOWED=1, set by the scripts) stays a window whatever the saved display mode kind is."""
import os


def game_env(**variables):
    """os.environ plus OPEN_THANDOR_GPU=0 (when not set) plus the given variables."""
    env = dict(os.environ)
    env.setdefault('OPEN_THANDOR_GPU', '0')
    env.update(variables)
    return env
