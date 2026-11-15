"""Draws docs/architecture/platform/platform.svg, the block diagram of the platform area.

Run from the repository root: python tools/docs/architecture/gen_platform.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

W, H = 1280, 740
d = A.Diagram(W, H, 'platform — start-up, SDL3 layer and services',
              'Modules under src/platform/ (headers in include/thandor/platform/). Arrows are calls or data handed over; '
              'most cross-area calls go through the original function slots (g_PlatformPumpEvents, g_Sound*, ...).')

SRC = '../../../src/platform/'
checks = []


def mod(x, y, w, h, name, desc, chips, meta=''):
    free = d.box(x, y, w, h, name, desc, chips, href=f'{SRC}{name}/', meta=meta, color='platform')
    checks.append((name, free, y + h))


def area(x, y, w, h, key, text=None):
    d.ext(x, y, w, h, text or key, href=f'../{key}/README.md', color=key)


# --- other areas (top row) -------------------------------------------------------------------------------------
d.band(40, 72, 'CALLED BY BOOTSTRAP')
d.band(640, 72, 'CALLERS OF THE SDL3 LAYER')
d.band(1110, 72, 'OS · DEVICES')
for x, key in ((40, 'core'), (185, 'assets'), (330, 'network'), (475, 'movie')):
    area(x, 80, 125, 40, key)
for x, key in ((640, 'ui'), (790, 'graphics'), (940, 'audio')):
    area(x, 80, 130, 40, key)

# --- platform modules -------------------------------------------------------------------------------------------
mod(40, 180, 560, 220, 'bootstrap',
    ['WinMain: crash and hang reports, start-up tables, then a self-test',
     'or ProcessEntry: priority, command line, one instance, main window.',
     'ProcessEntry_RunGame: heap, files, locale, SDL video / input / audio,',
     'network, settings, first display mode 640x480, UiRuntime_Initialize.',
     'Game_Run: Game_LoadCoreAssets, Game_PlayIntroMovies, saved mode,',
     'Frontend_MainLoop; then Runtime_Shutdown, SdlPlatform_Quit.'],
    ['main.cpp', 'runtime.cpp', 'image.cpp', 'low_memory.cpp', 'thandor.rc.in'], '4 files · 3k lines')
mod(640, 180, 430, 220, 'sdl3',
    ['Window and event pump, periodic timers, audio stream;',
     'video: renderer choice, display modes, present, pacing;',
     'SDL GPU renderer (Vulkan / Direct3D 12): 3D scenes and the',
     '2D draw list; software frames through an SDL_Renderer.'],
    ['platform.cpp', 'video.cpp', 'gpu_renderer.cpp', 'gpu_ui2d.cpp', 'gpu_ui_textures.cpp',
     'input.cpp', 'timer.cpp', 'audio.cpp', 'window_icon.cpp', 'gpu_diagnostics.cpp', 'shaders/'],
    '10 files · 7k lines')

mod(40, 440, 170, 160, 'filesystem',
    ['Win32 file slots', '(g_FileSystemOpen, ...),', 'executable directory,',
     'thandor.cfg, engine.pck.'],
    ['win32.cpp'])
mod(225, 440, 150, 160, 'system',
    ['Locale date / time', 'formatting; the pump', 'and timer slots.'],
    ['time_locale.cpp', 'win32.cpp'])
mod(390, 440, 210, 160, 'debug',
    ['Developer-tool hooks', '(THANDOR_DEV_TOOLS): input', 'script, autoshot, state hash,',
     'campaign autowin, movie tools.'],
    ['hooks', 'statehash', 'script', 'autoshot', 'campaign'])
mod(640, 440, 200, 160, 'selftest',
    ['Chosen by the variable', 'OPEN_THANDOR_SELFTEST:', 'codec, raster, hexscan,',
     'inifixtures, uiatlas, ...'],
    ['selftests.cpp', '6 more'])
mod(860, 440, 210, 160, 'input',
    ['Keyboard ring (64 events),', 'modifier and lock-key mask,', 'held special keys; cursor',
     'asset loading and buffers.'],
    ['devices.cpp'])

# --- devices, files and the remaining callers (right column, bottom row) -----------------------------------------
d.ext(1110, 190, 130, 40, 'Keyboard · mouse')
d.ext(1110, 270, 130, 40, 'Display · GPU')
d.ext(1110, 350, 130, 40, 'Sound device')
d.ext(40, 654, 170, 40, 'Files: PCK · cfg · saves')
d.ext(225, 654, 150, 40, 'Win32 time · locale')
area(390, 654, 210, 40, 'gameplay', 'gameplay (session hooks)')
area(860, 654, 210, 40, 'ui', 'ui (keyboard dispatch)')

# --- arrows -----------------------------------------------------------------------------------------------------
# bootstrap -> areas it initialises (up)
d.line([(102, 180), (102, 120)], 'heap, settings', 108, 154)
d.line([(247, 180), (247, 120)], 'Package_Mount', 253, 154)
d.line([(392, 180), (392, 120)], 'Network_Init', 398, 154)
d.line([(537, 180), (537, 120)], 'intro movies', 531, 168, anchor='end')
# bootstrap -> ui (Frontend_MainLoop) through the gap between bootstrap and sdl3
d.line([(600, 205), (620, 205), (620, 100), (640, 100)], 'main loop', 614, 152, anchor='middle', rotate=True)
# bootstrap -> sdl3
d.line([(600, 300), (640, 300)], 'init', 620, 293, anchor='middle')
# areas -> sdl3
d.line([(675, 120), (675, 180)], 'pump · present', 681, 146)
d.text(681, 160, '· timers', 'l')
d.line([(820, 120), (820, 180)], 'draw list, queues', 826, 146)
d.line([(900, 180), (900, 120)], 'mouse events', 906, 164)
d.line([(1005, 120), (1005, 180)], 'g_Sound* voices', 1011, 146)
# sdl3 <-> devices
d.line([(1110, 210), (1070, 210)], 'events', 1090, 203, anchor='middle')
d.line([(1070, 290), (1110, 290)], 'frames', 1090, 283, anchor='middle')
d.line([(1070, 370), (1110, 370)], 'mix', 1090, 363, anchor='middle')
# bootstrap -> filesystem, system, debug
d.line([(125, 400), (125, 440)], 'FileSystem_Init', 131, 425)
d.line([(300, 400), (300, 440)], 'Locale_Init', 306, 425)
d.line([(450, 400), (450, 440)], 'RunSelfTest', 456, 425)
# sdl3 -> debug (message pump hook), sdl3 -> input
d.line([(670, 400), (670, 420), (575, 420), (575, 440)])
d.text(678, 416, 'DebugHook_MessagePump', 'l')
d.line([(965, 400), (965, 440)], 'Keyboard_OnKeyDown / OnChar', 971, 425)
# debug -> selftest
d.line([(600, 520), (640, 520)])
d.text(620, 512, 'run', 'l', 'middle')
# bottom row
d.line([(125, 600), (125, 654)], 'Win32 files', 131, 630, both=True)
d.line([(300, 600), (300, 654)], 'GetLocalTime', 306, 630)
d.line([(495, 654), (495, 600)], 'step / session hooks', 501, 630)
d.line([(965, 600), (965, 654)], 'g_KeyboardReadEvent', 971, 630)

d.text(40, 722, 'Not drawn: core Runtime_Shutdown calls the Sdl*_Shutdown functions; assets, core and gameplay read and '
       'write files through the filesystem slots; ui and gameplay register timers.', 'f')

out = 'docs/architecture/platform/platform.svg'
d.save(out)
for name, free, bottom in checks:
    if free > bottom:
        print(f'WARNING: box {name} too short: content to y={free}, box ends at {bottom}')
if len(sys.argv) > 1:
    A.render_png(out, sys.argv[1], W, H)
    print('png:', sys.argv[1])
