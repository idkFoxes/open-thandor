"""Builds the high-resolution game font ("v8") from the game's own font.gfx / fontk.gfx and writes it as the
generated source src/assets/text/hd_font_data.cpp (the optional "Hochaufgeloeste Schrift" of the GPU renderers).

usage:
  python tools/data/font_hd/make_hd_font.py --game <game dir>            (engine\\font.gfx, engine\\fontk.gfx from
                                                                            <game dir>\\ENGINE.PCK)
  python tools/data/font_hd/make_hd_font.py --pck <ENGINE.PCK>
  python tools/data/font_hd/make_hd_font.py --font <font.gfx> --fontk <fontk.gfx>
options:
  --out <file.cpp>   the generated source (default src/assets/text/hd_font_data.cpp of this checkout); "-" = none
  --npz <file.npz>   also the glyphs as numpy arrays, keys font_<N>_<code hex> / fontk_<N>_<code hex> (uint8 alpha)
  --compare <npz>    compare the glyphs with such a file (e.g. one written from the original work_v8 scripts); exit
                     code 1 when any glyph differs
  --png <dir>        also one overview PNG per font and scale (needs Pillow): every glyph white on dark, 1x above

Method (the v8 pipeline of the font work, unchanged): every glyph of both fonts is rebuilt at 2x and 3x by
upscale3.render on the font model of dumpcur.model - straight centre-line strokes of one stroke width per font and
scale (upscale4), contour vectorisation with 45-degree facets and hinting for the rest, and about 30 *_parts.py
modules with hand constructions of single glyphs (accents, dots, digits, symbols). Every glyph is exactly
(width * N) x (height * N) target pixels; advances, line height and baseline stay the original ones times N.
The alpha (0..1) becomes round(alpha * 255). Some glyphs of one font are built from the other one (fontk's Yen and
TM from font.gfx), so both files are always needed.

Output format (decoded by src/ui/text/hd_font.cpp): the zlib stream (miniz on the game side) of
  'OTHF', u32 version 1, u32 set count, then per set: u8 font index (0 font.gfx, 1 fontk.gfx), u8 scale N,
  u16 glyph count, per glyph u32 source hash, u16 source width, u16 source height; after all set headers the
  alpha bytes of every set's glyphs in order, row by row ((width * N) * (height * N) bytes each).
The source hash is FNV-1a 32 over the u16 width, u16 height (little endian) and the glyph's 1x alpha bytes; the game
uses a glyph's high-resolution form only when the hash of its own font.gfx / fontk.gfx glyph matches, so another
version of the font files falls back to the plain scaling glyph by glyph.

Runs about 2-3 minutes (numpy, single core)."""
import argparse
import os
import struct
import sys
import time
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..'))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
DEFAULT_OUT = os.path.join(REPO, 'src', 'assets', 'text', 'hd_font_data.cpp')
FONTS = ('font.gfx', 'fontk.gfx')
SCALES = (2, 3)
VERSION = 1
# a white texel: the font glyphs' palettes hold white with the coverage in alpha (checked below), so the game builds
# the high-resolution texels as 0x00FFFFFF | alpha << 24
WHITE_RGB = (255, 255, 255)


def fnv1a(data, h=0x811C9DC5):
    for b in data:
        h = ((h ^ b) * 0x01000193) & 0xFFFFFFFF
    return h


def read_fonts(args):
    if args.font or args.fontk:
        if not (args.font and args.fontk):
            sys.exit('--font and --fontk go together')
        return {'font.gfx': open(args.font, 'rb').read(), 'fontk.gfx': open(args.fontk, 'rb').read()}
    pck_path = args.pck or (os.path.join(args.game, 'ENGINE.PCK') if args.game else None)
    if not pck_path:
        sys.exit('give --game, --pck or --font/--fontk')
    import pck
    data, entries = pck.entries(pck_path)
    found = {}
    for entry in entries:
        name = entry[0].lower()
        for font in FONTS:
            if name == 'engine\\' + font:
                found[font] = pck.read_entry(data, entry)
    missing = [f for f in FONTS if f not in found]
    if missing:
        sys.exit('%s: no engine\\%s' % (pck_path, ', engine\\'.join(missing)))
    return found


def source_glyphs(font_bytes):
    """(width, height, alpha bytes, rgb ok) of every glyph of one .gfx (the alpha the upscaler reads)."""
    import gfx
    import numpy as np
    import upscale as U
    d, imgs = gfx.parse(font_bytes)
    out = []
    for im in imgs:
        a = U.alpha_of(d, im)
        w, h = im.size
        if im.origin != (0, 0) or im.logical != im.size:
            sys.exit('glyph %d: origin %s / logical size %s differ from the plain glyph box' % (im.index, im.origin,
                                                                                             im.logical))
        ok = True
        if im.palette != -1 and w * h:
            base = gfx.HEADER + im.palette * gfx.BANK
            lut = np.frombuffer(d, np.uint8, 256 * 8, base).reshape(256, 8)
            idx = np.frombuffer(d, np.uint8, w * h, im.offset)
            used = np.unique(idx)
            for v in used:
                b, g, r, al = lut[v, :4]
                if al and (r, g, b) != WHITE_RGB:
                    ok = False
        elif w * h:
            raw = np.frombuffer(d, np.uint8, w * h * 4, im.offset).reshape(-1, 4)
            vis = raw[raw[:, 3] > 0]
            ok = bool((vis[:, :3] == 255).all())
        out.append((w, h, (np.round(a * 255)).astype(np.uint8).tobytes(), ok))
    return out


def build(fonts):
    import numpy as np
    import upscale as U
    U.FONT_DATA.update(fonts)
    import dumpcur as DC
    import upscale3 as S
    glyphs = {}
    for font in FONTS:
        for N in SCALES:
            t0 = time.perf_counter()
            M = DC.model(font, N)
            for c in range(256):
                r, _info = S.render(M, c, True)
                glyphs[(font, N, c)] = (np.clip(r, 0, 1) * 255 + 0.5).astype(np.uint8)
            print('%s %dx: %.1f s' % (font, N, time.perf_counter() - t0), flush=True)
    return glyphs


def encode(fonts, glyphs):
    sets = []
    for fi, font in enumerate(FONTS):
        src = source_glyphs(fonts[font])
        bad = [c for c, g in enumerate(src) if not g[3]]
        if bad:
            sys.exit('%s: glyphs with coloured texels (the game assumes white): %s' % (font, bad))
        for N in SCALES:
            sets.append((fi, font, N, src))
    head = bytearray(b'OTHF') + struct.pack('<II', VERSION, len(sets))
    body = bytearray()
    for fi, font, N, src in sets:
        head += struct.pack('<BBH', fi, N, len(src))
        for c, (w, h, alpha, _ok) in enumerate(src):
            g = glyphs[(font, N, c)]
            if g.shape != (h * N, w * N):
                sys.exit('%s %dx glyph 0x%02X: %s, not %dx%d' % (font, N, c, g.shape, h * N, w * N))
            head += struct.pack('<IHH', fnv1a(struct.pack('<HH', w, h) + alpha), w, h)
            body += g.tobytes()
    raw = bytes(head + body)
    return raw, zlib.compress(raw, 9)


def write_cpp(path, raw, packed):
    lines = []
    for i in range(0, len(packed), 24):
        lines.append('    ' + ','.join('0x%02X' % b for b in packed[i:i + 24]) + ',')
    text = '''/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/hd_font_data.cpp
 * Project code (not in the original game)
 */

/* GENERATED by tools/data/font_hd/make_hd_font.py from the game's engine\\font.gfx and engine\\fontk.gfx - do not
   edit. The high-resolution glyphs ("v8") of both fonts at UI scale 2 and 3 for the option "Hochaufgeloeste
   Schrift" (ui/text/hd_font.h): a zlib stream of %d bytes that unpacks to %d (format: make_hd_font.py). */

#include <cstddef>
#include <cstdint>

extern const uint8_t g_HdFontPacked[];
extern const size_t g_HdFontPackedSize;
extern const size_t g_HdFontUnpackedSize;

const size_t g_HdFontPackedSize = %d;
const size_t g_HdFontUnpackedSize = %d;
const uint8_t g_HdFontPacked[] = {
%s
};
''' % (len(packed), len(raw), len(packed), len(raw), '\n'.join(lines))
    with open(path, 'w', encoding='ascii', newline='\n') as f:
        f.write(text)


def save_npz(path, glyphs):
    import numpy as np
    np.savez_compressed(path, **{'%s_%d_%02X' % (f.split('.')[0], N, c): g for (f, N, c), g in glyphs.items()})


def compare(path, glyphs):
    import numpy as np
    ref = np.load(path)
    keys = {'%s_%d_%02X' % (f.split('.')[0], N, c): g for (f, N, c), g in glyphs.items()}
    diff = [k for k in sorted(keys) if k not in ref.files or ref[k].shape != keys[k].shape or
            not np.array_equal(ref[k], keys[k])]
    extra = [k for k in ref.files if k not in keys]
    print('compare %s: %d glyphs, %d differ, %d only in the reference' % (path, len(keys), len(diff), len(extra)))
    for k in diff[:20]:
        print('  differs:', k)
    return not diff and not extra


def save_png(out_dir, fonts, glyphs):
    import numpy as np
    from PIL import Image
    import upscale as U
    os.makedirs(out_dir, exist_ok=True)
    for font in FONTS:
        one = U.load(font)
        for N in SCALES:
            cells = [glyphs[(font, N, c)] for c in range(256)]
            cw = max(g.shape[1] for g in cells) + 2 * N
            ch = max(g.shape[0] for g in cells)
            cell_h = ch + max(a.shape[0] for a in one) + 3 * N
            sheet = np.full((16 * cell_h, 16 * cw), 24, np.uint8)
            for c, g in enumerate(cells):
                y, x = (c // 16) * cell_h, (c % 16) * cw
                a = (np.clip(one[c], 0, 1) * 255 + 0.5).astype(np.uint8)
                sheet[y + N:y + N + a.shape[0], x + N:x + N + a.shape[1]] = np.maximum(
                    24, a)[:, :]
                yy = y + 2 * N + max(b.shape[0] for b in one)
                sheet[yy:yy + g.shape[0], x + N:x + N + g.shape[1]] = np.maximum(24, g)
            fn = os.path.join(out_dir, '%s_%dx.png' % (font.split('.')[0], N))
            Image.fromarray(sheet, 'L').save(fn)
            print(fn)


def main():
    # glyph names and the work scripts' notes are not ASCII; never fail on a cp1252 console
    for stream in (sys.stdout, sys.stderr):
        try:
            stream.reconfigure(errors='backslashreplace')
        except (AttributeError, ValueError):
            pass
    p = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    p.add_argument('--game')
    p.add_argument('--pck')
    p.add_argument('--font')
    p.add_argument('--fontk')
    p.add_argument('--out', default=DEFAULT_OUT)
    p.add_argument('--npz')
    p.add_argument('--compare')
    p.add_argument('--png')
    args = p.parse_args()
    fonts = read_fonts(args)
    glyphs = build(fonts)
    raw, packed = encode(fonts, glyphs)
    if args.out != '-':
        write_cpp(args.out, raw, packed)
        print('%s: %d bytes packed, %d unpacked' % (args.out, len(packed), len(raw)))
    if args.npz:
        save_npz(args.npz, glyphs)
        print(args.npz)
    if args.png:
        save_png(args.png, fonts, glyphs)
    if args.compare and not compare(args.compare, glyphs):
        sys.exit(1)


if __name__ == '__main__':
    main()
