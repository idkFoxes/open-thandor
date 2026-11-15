"""Draws docs/architecture/movie/movie.svg, the block diagram of the movie area.

movie has one module (movie/runtime), so the boxes are its three source files.
Run from the repository root: python tools/docs/architecture/gen_movie.py
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import archsvg as A  # noqa: E402

W, H = 1280, 640
d = A.Diagram(W, H, 'movie — FLM playback and encoding',
              'One module, src/movie/runtime/ (headers in include/thandor/movie/runtime/); the boxes are its '
              'source files. Arrows are real call paths; click a box for its sources or page.')

d.band(1240, 76, 'WHERE MOVIES PLAY', anchor='end')
d.band(1240, 192, 'MOVIE/RUNTIME', anchor='end')
d.band(1240, 600, 'SERVICES', anchor='end')

# callers
d.ext(40, 84, 250, 44, 'platform: intro movies at start-up', href='../platform/README.md', color='platform')
d.ext(320, 84, 240, 44, 'gameplay: level loading movie', href='../gameplay/README.md', color='gameplay')
d.ext(590, 84, 400, 44, 'ui: briefing loop, campaign end movie, notifications', href='../ui/README.md', color='ui')

src = '../../../src/movie/runtime/'
y = d.box(40, 200, 660, 250, 'playback.cpp',
          ['Opens an FLM (loose movie folder, mounted package, exe folder), loads the header and the',
           'video stream, picks one embedded soundtrack at random, keeps g_ActiveMovie as a one-frame',
           'texture; a refill worker thread streams large movies. One movie open at a time.'],
          ['Movie_Open', 'Movie_AdvanceFrame', 'Movie_StreamWorkerThread', 'Movie_Rewind', 'Movie_Close',
           'Movie_GetFrameDimensions', 'Movie_SetAudioGainQ15', 'IntroMovie_TimerTick'],
          href=src, meta='playback', color='movie')
assert y <= 450, y
y = d.box(760, 200, 480, 110, 'flm_decoder.cpp',
          ['4x4 block delta decoder into the ARGB frame;', 'chroma/luma colour table built at start-up.'],
          ['Movie_DecodeFrame4x4Delta', 'Movie_BuildChromaLumaTable'],
          href=src, meta='decode', color='movie')
assert y <= 310, y
y = d.box(760, 325, 480, 125, 'flm_encoder.cpp',
          ['FLM encoder (keyframe and delta frames); not used', 'by the game, only by the self-tests.'],
          ['Movie_EncodeFrame4x4Keyframe', 'Movie_EncodeFrame4x4Delta', 'MovieColor_*'],
          href=src, meta='encode', color='movie')
assert y <= 450, y

# services
d.ext(40, 520, 200, 44, 'assets: PCK package entry', href='../assets/README.md', color='assets')
d.ext(270, 520, 260, 44, 'platform: file system, periodic timer', href='../platform/README.md', color='platform')
d.ext(560, 520, 200, 44, 'audio: soundtrack voice', href='../audio/README.md', color='audio')
d.ext(790, 520, 220, 44, 'graphics: frame drawn as texture', href='../graphics/README.md', color='graphics')
d.ext(1040, 520, 200, 44, 'platform: self-tests', href='../platform/README.md', color='platform')

# callers -> playback
d.line([(165, 128), (165, 200)], 'open, advance, close', 172, 168)
d.line([(440, 128), (440, 200)], 'open, advance per load step', 447, 168)
d.line([(640, 128), (640, 200)], 'open, advance, rewind', 647, 168)
# playback -> decoder
d.line([(700, 255), (760, 255)], 'decode', 730, 247, anchor='middle')
# playback -> services
d.line([(140, 450), (140, 520)], 'find entry', 147, 490)
d.line([(360, 450), (360, 520)], 'open, read, seek', 367, 490)
d.line([(470, 520), (470, 450)], 'frame ticks', 477, 490)
d.line([(660, 450), (660, 520)], 'voice set, play', 653, 490, anchor='end')
d.line([(690, 450), (690, 490), (900, 490), (900, 520)], 'ARGB frame via callers', 800, 483, anchor='middle')
# self-tests -> encoder
d.line([(1140, 520), (1140, 450)], 'round trip', 1147, 490)

d.save('docs/architecture/movie/movie.svg')
