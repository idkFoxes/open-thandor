"""Draws docs/architecture/audio/audio.svg, the block diagram of the audio area.

Run from the repository root: python tools/docs/architecture/gen_audio.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

W, H = 1280, 680
d = A.Diagram(W, H, 'audio — positioned sound, sample codec, backend slots',
              'Modules under src/audio/ (headers in include/thandor/audio/). The SDL mixer itself lives in '
              'platform/sdl3/audio.cpp. Arrows are real call paths; click a box for its sources or page.')

d.band(1240, 76, 'CALLERS', anchor='end')
d.band(1240, 192, 'AUDIO AREA', anchor='end')
d.band(1240, 600, 'PLATFORM · DEVICE', anchor='end')

# callers (other areas)
d.ext(40, 84, 230, 44, 'gameplay: army sounds, level samples', href='../gameplay/README.md', color='gameplay')
d.ext(300, 84, 200, 44, 'world: effects, shots', href='../world/README.md', color='world')
d.ext(520, 84, 270, 44, 'ui: clicks, music, settings, camera', href='../ui/README.md', color='ui')
d.ext(810, 84, 180, 44, 'movie: soundtrack', href='../movie/README.md', color='movie')
d.ext(1020, 84, 220, 44, 'platform: start-up, saved gains', href='../platform/README.md', color='platform')

src = '../../../src/audio/'
y = d.box(40, 200, 520, 200, 'spatial',
          ['Listener at the camera, distance fade and azimuth pan',
           'for world sounds; 256 slots for looping positioned',
           'sounds, refreshed every 8th frame.'],
          ['runtime.cpp', 'listener transform', 'PlayPositionedOneShot', 'looping slots',
           'Clear/ApplyDesiredGains', 'effects gain', 'reverse stereo'],
          href=src + 'spatial/', meta='1 file', color='audio')
assert y <= 400, y
y = d.box(620, 200, 330, 200, 'backend',
          ['The eight g_Sound* slots every caller', 'plays through; silent stubs until',
           'SdlAudio_Init installs the SDL3 backend.'],
          ['runtime.cpp', 'null_backend.cpp', 'g_SoundPlayOneShot', 'g_SoundPlayLooping', 'attenuation table'],
          href=src + 'backend/', meta='2 files', color='audio')
assert y <= 400, y
y = d.box(1000, 200, 240, 200, 'codec',
          ['.sam sample codec: packed', 'coefficient blocks, inverse', 'cosine transform to PCM.'],
          ['sam.cpp', 'sam_encoder.cpp', 'cosine tables'],
          href=src + 'codec/', meta='2 files', color='audio')
assert y <= 400, y

d.ext(560, 480, 450, 56, 'platform/sdl3/audio.cpp: SDL mixer, voice sets of 8 voices',
      href='../platform/README.md', color='platform')
d.ext(620, 600, 330, 44, 'Sound device')

# callers -> audio
d.line([(150, 128), (150, 200)], 'positioned one-shots, slots', 157, 168)
d.line([(400, 128), (400, 200)], 'one-shots, shot loops', 407, 168)
d.line([(540, 128), (540, 200)], 'listener pose,', 547, 160)
d.text(547, 174, 'gain pass', 'l')
d.line([(700, 128), (700, 200)], 'clicks, music', 707, 168)
d.line([(900, 128), (900, 200)], 'soundtrack', 907, 168)
# platform bootstrap -> SdlAudio_Init (right gutter)
d.line([(1220, 128), (1220, 160), (1258, 160), (1258, 512), (1010, 512)], 'SdlAudio_Init', 1250, 425, anchor='end')
# spatial -> backend
d.line([(560, 300), (620, 300)], 'play', 590, 292, anchor='middle')
d.text(590, 318, 'gains', 'l', anchor='middle')
# backend -> SDL mixer
d.line([(785, 400), (785, 480)], 'slots call SdlAudio_*', 792, 445)
# SDL mixer -> codec
d.line([(1010, 495), (1120, 495), (1120, 400)], 'decode .sam', 1128, 450)
# SDL mixer -> device
d.line([(785, 536), (785, 600)], 'S16 stereo, 22050 Hz', 792, 572)

d.save('docs/architecture/audio/audio.svg')
