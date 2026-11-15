"""(R) 0xAE and (C) 0xA9, built from parts (work_regcopy): the finished O ring + a reduced R / C centred in its counter.
font.gfx: ring = font.gfx O, pixel-identical at the O's own position (same cell size).
   The fontk caps (18 rows at 3x, 12 at 2x) are taller than the O's counter (17x17 at 3x, 10x12 at 2x), so:
   3x: fontk R / C at scale 3 (stroke 3, real pixels), rows / flat columns removed to 11 x 13;
   2x: stroke 2 leaves no gap (R's counter closes), so a small R / C of stroke 1 from rectangles + small arcs.
fontk.gfx: the cell keeps the original symbol's size (15 rows x 10 columns, much larger than the fontk O), so
   ring = fontk O with flat rows / columns inserted (same arcs and stroke), inner letter = fontk R / C at scale N."""
import numpy as np

REG, COPY = 0xAE, 0xA9
LETTER = {REG: 'R', COPY: 'C'}
SS = 8
# fontk caps -> reduced form in font.gfx at 3x: rows / columns of the ink-cropped glyph to remove
FONT3_CUT = {'R': ((3, 4, 10, 11, 17), (5, 6, 7)),          # counter 4 -> 2 rows, leg: upper rows and tail tip;
             'C': ((7, 8, 9, 10, 11), (5, 6, 7, 8))}        # flat bowl rows, flat bar columns
FONTK_RING = {3: (26, 28, 8, 2), 2: (18, 20, 5, 1)}          # ring width, height, top row, left column
FONTK_C_CUT = {3: 5, 2: None}                               # 3x C is 15 wide: one flat column out (even, as the counter)
_kcaps = {}


def _ink(r):
    rows = np.nonzero(r.max(axis=1) > 0.02)[0]
    cols = np.nonzero(r.max(axis=0) > 0.02)[0]
    return rows[0], rows[-1] + 1, cols[0], cols[-1] + 1


def _caps(N):
    """ink-cropped finished R / C of fontk.gfx at scale N."""
    if N not in _kcaps:
        import dumpcur as D, upscale3 as S
        K = D.model('fontk.gfx', N)
        out = {}
        for ch in 'RC':
            r = S.render(K, ord(ch))
            y0, y1, x0, x1 = _ink(r)
            out[ch] = r[y0:y1, x0:x1]
        _kcaps[N] = out
    return _kcaps[N]


def _small(ch):
    """stroke 1 letter, 6 x 8 px: rectangles + small arcs (radius 2 outside, 1 inside), 22.5 deg leg."""
    h, w = 8, 6
    yy, xx = np.mgrid[0:h * SS, 0:w * SS]
    X, Y = (xx + 0.5) / SS, (yy + 0.5) / SS

    def rr(x0, y0, x1, y1, tl, tr, br, bl):
        m = (X >= x0) & (X <= x1) & (Y >= y0) & (Y <= y1)
        for cx, cy, sx, sy, rad in ((x0, y0, 1, 1, tl), (x1, y0, -1, 1, tr), (x1, y1, -1, -1, br), (x0, y1, 1, -1, bl)):
            if rad > 0:
                ax, ay = cx + sx * rad, cy + sy * rad
                m &= ~(((X - ax) * sx < 0) & ((Y - ay) * sy < 0)) | ((X - ax) ** 2 + (Y - ay) ** 2 <= rad * rad)
        return m
    if ch == 'C':
        m = rr(0, 0, 6, 8, 2, 2, 2, 2) & ~rr(1, 1, 5, 7, 1, 1, 1, 1)
        m &= ~((X > 4) & (Y > 2) & (Y < 6))                 # opening: terminals 2 rows, as the fontk C
    else:
        m = rr(0, 0, 6, 5, 0, 2, 2, 0) & ~rr(1, 1, 5, 4, 0, 1, 1, 0)   # bowl
        m |= (X <= 1)                                       # stem
        xl = 2.75 + (Y - 5) * 0.75
        m |= (Y >= 4.5) & (X >= xl) & (X <= xl + 1.25)      # leg
    return m.reshape(h * SS, w * SS).reshape(h, SS, w, SS).mean(axis=(1, 3))


def _stretch(O, w, h):
    """O ink box, flat middle columns / rows repeated up to w x h (arcs and stroke unchanged)."""
    y0, y1, x0, x1 = _ink(O)
    a = O[y0:y1, x0:x1]
    my, mx = a.shape[0] // 2, a.shape[1] // 2
    if np.abs(a[my - 1] - a[my]).max() > 1e-6 or np.abs(a[:, mx - 1] - a[:, mx]).max() > 1e-6:
        return None
    a = np.insert(a, [my] * (h - a.shape[0]), a[my], axis=0)
    return np.insert(a, [mx] * (w - a.shape[1]), a[:, mx:mx + 1], axis=1)


def _place(out, a):
    """centre the letter in the ring's ink box; the sizes have the same parity, so exactly centred."""
    y0, y1, x0, x1 = _ink(out)
    y, x = y0 + (y1 - y0 - a.shape[0]) // 2, x0 + (x1 - x0 - a.shape[1]) // 2
    out[y:y + a.shape[0], x:x + a.shape[1]] = np.maximum(out[y:y + a.shape[0], x:x + a.shape[1]], a)
    return out


def render(M, code):
    """returns the raster or None (other codes, other scales)."""
    if code not in LETTER or M.N not in (2, 3) or M.g[code].max() <= 0:
        return None
    import upscale3 as S
    N, ch = M.N, LETTER[code]
    h, w = M.g[code].shape[0] * N, M.g[code].shape[1] * N
    O = S.render(M, ord('O'))
    if O.shape == (h, w):                                   # font.gfx: same cell as the O, ring pixel-identical
        if N == 3:
            rows, cols = FONT3_CUT[ch]
            a = np.delete(np.delete(_caps(3)[ch], rows, 0), cols, 1)
        else:
            a = _small(ch)
        return _place(O.copy(), a)
    rw, rh, top, left = FONTK_RING[N]                       # fontk.gfx: O arcs stretched to the symbol's size
    ring = _stretch(O, rw, rh)
    if ring is None:
        return None
    out = np.zeros((h, w))
    out[top:top + rh, left:left + rw] = ring
    a = _caps(N)[ch]
    if ch == 'C' and FONTK_C_CUT[N] is not None:
        a = np.delete(a, FONTK_C_CUT[N], 1)
    return _place(out, a)
