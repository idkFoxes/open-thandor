"""fontk.gfx free-standing marks (v9), 2x and 3x; font.gfx has its own (gmarks_parts).
The originals (15-row cells) are distorted copies; they are drawn as the accents on the finished fontk capitals:
the mark is cut out of the finished accented capital at run time (letter minus its placed base, see
gmarks_parts.finished_mark), so pixels and band rows stay identical to the letters. The 15-row cells carry the
9-row letters' rows ROW_9 source rows lower (kacc_parts / ksym_parts). Columns: centred on the original's ink.
60 grave      = mark of A-grave (C0)      88 circumflex = mark of A-circumflex (C2)
98 tilde      = mark of N-tilde (D1)      A8 dieresis   = mark of A-dieresis (C4)
B4 acute      = mark of A-acute (C1)      B8 cedilla    = mark of C-cedilla (C7)
AF macron     = bar of the accent stroke width (accents_parts Wa), as wide as the original's coverage, its bottom
                on the capitals' accent band bottom (where the circumflex / dieresis stand)"""
import math
import numpy as np
import accents_parts as A
import gmarks_parts as G
import kacc_parts as K

CODES = (0x60, 0x88, 0x98, 0xA8, 0xAF, 0xB4, 0xB8)
SRC = {0x60: 0xC0, 0x88: 0xC2, 0x98: 0xD1, 0xA8: 0xC4, 0xB4: 0xC1, 0xB8: 0xC7}


def _macron(M):
    N, a = M.N, M.g[0xAF]
    p = A.params(M)
    h, w = a.shape[0] * N, a.shape[1] * N
    col = a.sum(axis=0)
    cov = np.clip(col / np.median(col[col > 0.5 * col.max()]), 0, 1)
    full = np.nonzero(cov >= 0.999)[0]
    left = full[0] - cov[:full[0]].sum()
    right = full[-1] + 1 + cov[full[-1] + 1:].sum()
    L = int(round((right - left) * N))
    x0 = max(0, min(w - L, int(math.floor((left + right) / 2.0 * N - L / 2.0 + 0.5))))
    y1 = K.ROW_9 * N + p['base'][True]
    r = np.zeros((h, w))
    r[y1 - p['Wa']:y1, x0:x0 + L] = 1.0
    return r


def render(M, code):
    if code not in CODES or not K.active(M) or M.g[code].shape[0] != 15:
        return None
    if code == 0xAF:
        return _macron(M)
    N, a = M.N, M.g[code]
    part, y0 = G.finished_mark(M, SRC[code])
    y0 += K.ROW_9 * N
    ph, pw = part.shape
    h, w = a.shape[0] * N, a.shape[1] * N
    x0 = max(0, min(w - pw, int(math.floor(G._centre_x(a) * N - pw / 2.0 + 0.5))))
    r = np.zeros((h, w))
    r[y0:y0 + ph, x0:x0 + pw] = part[:h - y0, :w - x0]
    return r
