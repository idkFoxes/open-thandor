"""font.gfx free-standing marks (work_gmarks2), 3x and 2x only; fontk.gfx is not touched.
The acute, grave, dieresis and cedilla are cut out of the FINISHED accented letters at run time (pixels and row),
so they stay identical to them. In the originals the free marks use the same source rows as the marks on the
lower-case letters, so they take over the finished letters' rows. Columns: centred on the original's ink.
B4 acute    = part of e-acute (E9)
60 grave    = B4's part mirrored (same as the accent of e-grave)
A8 dieresis = both dots of a-dieresis (E4)
B8 cedilla  = part of c-cedilla (E7)
AF macron   = rectangle, thickness M.W, length and position from the original's coverage"""
import math
import numpy as np
import accents_parts as A

CODES = (0x60, 0xA8, 0xAF, 0xB4, 0xB8)
SRC = {0x60: 0xE9, 0xB4: 0xE9, 0xA8: 0xE4, 0xB8: 0xE7}


def finished_mark(M, letter):
    """(raster, top row) of the mark on a finished accented letter = letter minus its placed base."""
    cache = M.__dict__.setdefault('_gmarks', {})
    if letter not in cache:
        import upscale3 as S
        r = S.render(M, letter)
        d = np.where(np.abs(r - A.placed_base(M, letter)) > 1e-9, r, 0.0)
        rows = np.nonzero(d.max(axis=1) > 0)[0]
        cols = np.nonzero(d.max(axis=0) > 0)[0]
        cache[letter] = d[rows[0]:rows[-1] + 1, cols[0]:cols[-1] + 1], int(rows[0])
    return cache[letter]


def _centre_x(a):
    """ink centre of the original in source px."""
    s = a.sum(axis=0)
    return float((s * (np.arange(len(s)) + 0.5)).sum() / s.sum())


def _macron(M):
    N, W, a = M.N, M.W, M.g[0xAF]
    h, w = a.shape[0] * N, a.shape[1] * N
    col = a.sum(axis=0)
    cov = np.clip(col / np.median(col[col > 0.5 * col.max()]), 0, 1)
    full = np.nonzero(cov >= 0.999)[0]
    left = full[0] - cov[:full[0]].sum()
    right = full[-1] + 1 + cov[full[-1] + 1:].sum()
    L = int(round((right - left) * N))
    x0 = int(math.floor((left + right) / 2.0 * N - L / 2.0 + 0.5))
    s = a.sum(axis=1)
    cy = float((s * (np.arange(len(s)) + 0.5)).sum() / s.sum())
    y0 = int(math.floor(cy * N - W / 2.0 + 0.5))
    r = np.zeros((h, w))
    r[y0:y0 + W, x0:x0 + L] = 1.0
    return r


def render(M, code):
    if code not in CODES or M.N not in (2, 3) or M.g[ord('A')].shape[0] != 15:
        return None
    if code == 0xAF:
        return _macron(M)
    N, a = M.N, M.g[code]
    part, y0 = finished_mark(M, SRC[code])
    if code == 0x60:
        part = part[:, ::-1]
    ph, pw = part.shape
    h, w = a.shape[0] * N, a.shape[1] * N
    x0 = max(0, min(w - pw, int(math.floor(_centre_x(a) * N - pw / 2.0 + 0.5))))
    r = np.zeros((h, w))
    r[y0:y0 + ph, x0:x0 + pw] = part
    return r
