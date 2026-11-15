"""fontk.gfx G / g and S / s (work_kbdpq), rebuilt from the finished O and 0 like k4_parts builds the 2. Same fault as the
fontk 4 / 2: the contour's iso line filled the small counters. G had a solid wedge under its bar and a square top right,
S had a square top left, a wedge at the bottom left and its upper counter closed on the right (read as 8).
Lower case cells are copies of the capitals in the fontk original, so they stay identical to the capitals.
Body = source rows 2..8, bar / spine in source row 5 / 4. Stroke M.W, all arcs are pixel copies of the O / the 0.
G: the O moved to the left cell edge, right stem cut flat under the top arc (source row 4) and again above the bar;
   bar in source row 5 from column 3 to column 6 with the stem running on below it (spur right of the stem as the original).
S: left half of the 0 / right half of the 0, one column wider below (the original's lower bowl is 0.3 .. 0.5 px wider).
   Upper bowl = top left corner + bottom left corner of the 0, spine = its bottom left + top right corner, lower bowl
   = its right corners, left terminal = bottom left corner of the 0 cut flat at source row 6.
   The upper terminal is cut short (TERM), the upper counter is only one source row high and has to stay open."""
import numpy as np
import upscale3 as S

CODES = (0x47, 0x67, 0x53, 0x73)
Y0, Y1 = 2, 8                # glyph top / baseline (source px)
TERM = {3: 4, 2: 3}          # S: rows of the top right arc kept, from the glyph top (output px)


def applies(M, code):
    return code in CODES and M.g[ord('A')].shape[0] == 9 and M.g[code].shape[0] == 9 and M.N in (2, 3)


def g_G(M, shape):
    N = M.N
    h, wd = shape[0] * N, shape[1] * N
    O = S.render(M, ord('O'))
    x0 = int(np.nonzero(O.max(axis=0) > 0.02)[0][0])
    r = np.zeros((h, wd))
    w = min(wd, O.shape[1] - x0)
    r[:, :w] = O[:, x0:x0 + w]
    mid = int(np.nonzero(r.max(axis=0) > 0.02)[0][-1] + 1) // 2
    r[4 * N:5 * N, mid:] = 0.0                            # gap between the top terminal and the bar
    r[5 * N:6 * N, 3 * N:6 * N] = 1.0                     # bar
    return r


def g_S(M, shape):
    N = M.N
    h, wd = shape[0] * N, shape[1] * N
    O = S.render(M, ord('0'))[Y0 * N:Y1 * N]              # the 0: 6N rows, ink in columns 0 .. 4N
    H = 6 * N
    L, R = O[:, :2 * N], O[:, 2 * N:4 * N]
    R1 = np.hstack([R[:, :1], R])                         # lower bowl: one column wider
    A = np.zeros((H, 2 * N))
    B = np.zeros((H, 2 * N + 1))
    a = (3 * N + 1) // 2                                  # upper bowl: top corner rows, the rest from the bottom corner
    A[:a] = L[:a]
    A[a:3 * N] = L[H - (3 * N - a):]
    A[4 * N:] = L[4 * N:]                                 # left terminal + bottom arc
    t = TERM[N]
    B[:t, :2 * N] = R[:t]                                 # top arc + short terminal
    B[2 * N:] = np.vstack([R1[:2 * N], R1[H - 2 * N:]])   # spine + lower bowl
    r = np.zeros((h, wd))
    r[Y0 * N:Y1 * N, :4 * N + 1] = np.hstack([A, B])
    return r


def render(M, code):
    if not applies(M, code):
        return None
    return (g_G if code in (0x47, 0x67) else g_S)(M, M.g[code].shape)
