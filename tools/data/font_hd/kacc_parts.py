"""fontk.gfx accented letters, second pass (work_kacc). Rule: accented letter = finished base letter + accent part.
  a            = fontk A (same source pixels) with one more straight leg row, so top and bar rise one row onto the
                 line of the other fontk letters; the accented a follow via accents_parts (run-time base letter).
  y-acute, y-diaeresis, s-caron: 15-row cells copied from font.gfx -> fontk y / s + the normal accent part, on the rows
                 of the 9-row letters (ROW_9 = 3 source rows, see ksym_parts), ink centred in the unchanged cell.
  S-caron, Y-diaeresis: body as v7 (large, as the original); the flat bars become the caron / two-dot parts of
                 accents_parts (originals: row 1 of S-caron is a V, row 1 of Y-diaeresis has two peaks).
  2x only      : lower-case tilde 3 rows high (1 clear row above the body, band is only 3 rows),
                 ring 4 x 3 with a 2-wide hole (a 3 x 3 ring has a 1-px grey hole), upper and lower case.
Active for fontk only (9-row A cell), 2x / 3x with M.W == M.N."""
import numpy as np
import accents_parts as A

A_LOWER = 0x61
BASED = (0x9A, 0xFD, 0xFF)                  # s-caron, y-acute, y-diaeresis
MARKS = (0x8A, 0x9F)                        # S-caron, Y-diaeresis
CODES = (A_LOWER,) + BASED + MARKS
ROW_9 = 3

TILDE_2X = np.array([[.25, 1, 1, 1, .4, 0, 0],
                     [1, 1, 1, 1, 1, 1, 1],
                     [0, 0, .4, 1, 1, 1, .25]])
RING_2X = np.array([[.4, 1, 1, .4],
                    [1, 0, 0, 1],
                    [.4, 1, 1, .4]])
TILDE, RING = '̃', '̊'


def active(M):
    return M.N in (2, 3) and M.W == M.N and M.g[ord('A')].shape[0] == 9


def _seed_2x_parts(M):
    if M.N != 2 or M.__dict__.get('_kacc_seeded'):
        return
    M.__dict__['_kacc_seeded'] = True
    cache = M.__dict__.setdefault('_accparts', {})
    cache[(TILDE, False)] = (TILDE_2X, TILDE_2X.shape[1] / 2.0)
    for upper in (True, False):
        cache[(RING, upper)] = (RING_2X, RING_2X.shape[1] / 2.0)


def _box(r):
    rows = np.nonzero(r.max(axis=1) > 0.5)[0]
    cols = np.nonzero(r.max(axis=0) > 0.5)[0]
    return int(rows[0]), int(rows[-1]) + 1, int(cols[0]), int(cols[-1]) + 1


def _stamp(out, part, y0, cx):
    h, w = out.shape
    ph, pw = part.shape
    x0 = max(0, min(w - pw, int(np.floor(cx - pw / 2.0 + 0.5))))
    out[y0:y0 + ph, x0:x0 + pw] = np.maximum(out[y0:y0 + ph, x0:x0 + pw], part)


def g_a(M):
    """A with the first all-straight leg row doubled: everything above it moves up one row."""
    import upscale3 as S
    r = S.render(M, ord('A')).copy()
    y0, y1, _, _ = _box(r)
    y = y1 - 2                              # a leg row (flat down to the baseline)
    out = np.zeros_like(r)
    out[y0 - 1:y] = r[y0:y + 1]
    out[y:] = r[y:]
    return out


def g_based(M, code):
    import upscale3 as S
    N = M.N
    h, w = M.g[code].shape[0] * N, M.g[code].shape[1] * N
    b, acc = A.decomp(code)
    base = S.render(M, b)
    _, _, c0, c1 = _box(base)
    yo = ROW_9 * N
    out = np.zeros((h, w))
    x0 = (w - (c1 - c0)) // 2
    out[yo:yo + base.shape[0], x0:x0 + c1 - c0] = base[:, c0:c1]
    part, _ = A.accent_part(M, acc, False)
    _stamp(out, part, yo + A.params(M)['base'][False] - part.shape[0], x0 + (c1 - c0) / 2.0)
    return out


def g_marks(M, code):
    import upscale3 as S
    M.__dict__['_kacc_skip'] = code
    try:
        r = S.render(M, code).copy()
    finally:
        M.__dict__['_kacc_skip'] = None
    ink = r.max(axis=1) > 0.02
    y = int(np.argmax(ink))
    while ink[y]:
        y += 1
    y += int(np.argmax(ink[y:]))            # first row of the body, below the old mark
    r[:y] = 0.0
    _, acc = A.decomp(code)
    part, _ = A.accent_part(M, acc, True)
    _, _, c0, c1 = _box(r)
    _stamp(r, part, y - A.params(M)['gap'] - part.shape[0], (c0 + c1) / 2.0)
    return r


def render(M, code):
    if not active(M):
        return None
    _seed_2x_parts(M)
    if code not in CODES or M.__dict__.get('_kacc_skip') == code:
        return None
    if code == A_LOWER:
        return g_a(M)
    return g_based(M, code) if code in BASED else g_marks(M, code)
