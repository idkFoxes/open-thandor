"""fontk.gfx symbols with a 15-row original cell, rebuilt from fontk's own finished letters (work_ksym).
Oslash / oslash = fontk O / o (pixel-identical, the two originals are the same glyph) + one straight 45 deg stroke,
Eth = fontk D + short rectangular bar through the stem, eth = fontk o bowl + ascender + the same bar,
degree = small circular ring, @ = ring (O arcs stretched), open at the lower right, its right wall curving into the foot
of the inner a shape (O arcs, straight stem), as the original @; a closed ring around the a reads as nested boxes.
Letter glyphs of fontk have 9-row cells. Rich text (FontGlyph_DrawVerticallyCentered) centres each cell in the line,
so a 9-row cell sits exactly 3 source rows below the top of a 15-row cell: ROW_9 = 3 source rows puts the letters
built here on the rows of the real fontk letters. Horizontal: the (narrower) ink is centred in the unchanged cell.
@ and degree keep the ink rows of their originals. Stroke width M.W everywhere (3x 3 px, 2x 2 px)."""
import numpy as np

SLASH = (0xD8, 0xF8)
ETH_U, ETH_L, DEG, AT = 0xD0, 0xF0, 0xB0, 0x40
CODES = SLASH + (ETH_U, ETH_L, DEG, AT)
ROW_9 = 3                                   # source rows from the top of a 15-row cell to the top of a 9-row cell
# @: output ink rows / columns of the ring, inner a size (w, h)
AT_GEO = {3: dict(top=9, left=1, w=34, h=33, aw=18, ah=17), 2: dict(top=6, left=1, w=22, h=22, aw=12, ah=12)}
DEG_GEO = {3: (9, 12), 2: (6, 8)}           # top row, outer diameter (3x stroke 3: counter 6, 2x stroke 2: counter 4)
SS = 16


def applies(M, code):
    return (code in CODES and M.N in (2, 3) and M.W == M.N and M.g[ord('A')].shape[0] == 9
            and M.g[code].shape[0] == 15)


def _ink(r):
    rows = np.nonzero(r.max(axis=1) > 0.02)[0]
    cols = np.nonzero(r.max(axis=0) > 0.02)[0]
    return rows[0], rows[-1] + 1, cols[0], cols[-1] + 1


def _letter(M, ch):
    import upscale3 as S
    r = S.render(M, ord(ch))
    y0, y1, x0, x1 = _ink(r)
    return r[y0:y1, x0:x1], y0


def _resize(a, w, h):
    """flat middle rows / columns repeated or removed (arcs and stroke unchanged)."""
    my, mx = a.shape[0] // 2, a.shape[1] // 2
    assert np.abs(a[my - 1] - a[my]).max() < 1e-6 and np.abs(a[:, mx - 1] - a[:, mx]).max() < 1e-6
    dh, dw = h - a.shape[0], w - a.shape[1]
    a = np.insert(a, [my] * dh, a[my], axis=0) if dh >= 0 else np.delete(a, range(my, my - dh), axis=0)
    a = np.insert(a, [mx] * dw, a[:, mx:mx + 1], axis=1) if dw >= 0 else np.delete(a, range(mx, mx - dw), axis=1)
    return a


def _diag(M, shape, yc, xc, y0, y1, t):
    """straight stroke through output point (yc, xc), rising to the right, x changes by t per row, flat cut at the
    output rows y0 / y1."""
    import upscale4 as U4
    N = M.N
    e = (y1 - y0) + 2 * M.W
    pts = [((yc + e) / N, (xc - e * t) / N), ((yc - e) / N, (xc + e * t) / N)]
    return U4.lo(U4.stroke_raster(M, shape, pts, clip=(y0 / N, y1 / N)))


def g_slashed(M, code):
    N = M.N
    h, w = M.g[code].shape[0] * N, M.g[code].shape[1] * N
    O, oy = _letter(M, 'O')
    y0, x0 = oy + ROW_9 * N, (w - O.shape[1]) // 2
    out = np.zeros((h, w))
    out[y0:y0 + O.shape[0], x0:x0 + O.shape[1]] = O
    st = _diag(M, M.g[code].shape, y0 + O.shape[0] / 2.0, x0 + O.shape[1] / 2.0, y0, y0 + O.shape[0], 1.0)
    return np.maximum(out, st)


def _mid_bar_rows(M, y0):
    """the rows of the middle bar of fontk's E, so the Eth bar sits on the same line."""
    E, _ = _letter(M, 'E')
    rows = np.nonzero(E[:, 2 * M.W] > 0.5)[0]
    gaps = np.nonzero(np.diff(rows) > 1)[0]
    mid = rows[gaps[0] + 1]
    return y0 + mid, y0 + mid + M.W


def g_eth_u(M, code):
    N, W = M.N, M.W
    h, w = M.g[code].shape[0] * N, M.g[code].shape[1] * N
    D, oy = _letter(M, 'D')
    y0 = oy + ROW_9 * N
    total = W + D.shape[1]                    # bar: W left of the stem, stem, W into the counter
    x0 = (w - total) // 2 + W                 # column of the stem
    out = np.zeros((h, w))
    out[y0:y0 + D.shape[0], x0:x0 + D.shape[1]] = D
    b0, b1 = _mid_bar_rows(M, y0)
    out[b0:b1, x0 - W:x0 + 2 * W] = 1.0
    return out


def g_eth_l(M, code):
    """bowl = fontk o on the letter rows, ascender 3 source rows above it leaning left out of the right wall
    (22.5 deg, flat cut), horizontal rectangular bar through the ascender (as the original eth, without the blob)."""
    import upscale4 as U4
    N, W = M.N, M.W
    h, w = M.g[code].shape[0] * N, M.g[code].shape[1] * N
    O, oy = _letter(M, 'O')
    y0, x0 = oy + ROW_9 * N, (w - O.shape[1]) // 2
    out = np.zeros((h, w))
    out[y0:y0 + O.shape[0], x0:x0 + O.shape[1]] = O
    top = y0 - ROW_9 * N
    if N == 2:                                # work_kacc: pixel stair (1 px step every 2 rows), ends in the top band of the bowl
        for i in range(3):
            out[top + 2 * i:top + 2 * i + 2, x0 + 6 + i:x0 + 8 + i] = 1.0
        out[y0, x0 + 8:x0 + 10] = 1.0
        out[top + W:top + 2 * W, x0 + 5:x0 + 11] = 1.0
        return out
    xr = x0 + O.shape[1] - W / 2.0            # centre of the right wall
    t = U4.T225
    y_join = y0 + W                           # the stroke ends in the top band of the bowl (no wedge in the counter)
    e = 2.0 * W
    pts = [((y_join + e) / N, (xr + e * t) / N), ((top - e) / N, (xr - (y_join - top + e) * t) / N)]
    st = U4.lo(U4.stroke_raster(M, M.g[code].shape, pts, clip=(top / N, y_join / N)))
    st[:, x0 + O.shape[1]:] = 0.0             # nothing right of the wall
    out = np.maximum(out, st)
    b0 = top + W                              # one stroke of ascender tip above the bar
    xc = xr - (y_join - (b0 + W / 2.0)) * t   # ascender centre at the bar
    bl = 3 * W
    bx0 = int(round(xc - bl / 2.0))
    out[b0:b0 + W, bx0:min(bx0 + bl, w)] = 1.0
    return out


def g_degree(M, code):
    N, W = M.N, M.W
    h, w = M.g[code].shape[0] * N, M.g[code].shape[1] * N
    top, d = DEG_GEO[N]
    x0 = (w - d) // 2
    yy, xx = np.mgrid[0:d * SS, 0:d * SS]
    rr = np.hypot((yy + 0.5) / SS - d / 2.0, (xx + 0.5) / SS - d / 2.0)
    m = ((rr <= d / 2.0) & (rr >= d / 2.0 - W)).astype(float).reshape(d, SS, d, SS).mean(axis=(1, 3))
    m[m < 0.04] = 0.0
    out = np.zeros((h, w))
    out[top:top + d, x0:x0 + d] = m
    return out


def g_at(M, code):
    N, W = M.N, M.W
    h, w = M.g[code].shape[0] * N, M.g[code].shape[1] * N
    G = AT_GEO[N]
    O, _ = _letter(M, 'O')
    top, left, rw, rh, aw, ah = G['top'], G['left'], G['w'], G['h'], G['aw'], G['ah']
    ay, ax = top + (rh - ah) // 2, left + (rw - aw) // 2
    # inner a: O arcs, right wall = straight stem with a square upper corner
    a = _resize(O, aw, ah).copy()
    k = W + 3
    a[:W, -k:] = 1.0
    a[-W:, -k:] = 1.0
    a[:, -W:] = 1.0
    a[W:-W, -k:-W] = 0.0
    out = np.zeros((h, w))
    # lower ring: left wall and bottom band, square cut before the lower right arc
    ring = _resize(O, rw, rh)
    xcut = rw - (k + W)
    yfoot = ay + ah - top                     # ring rows below the foot of the a
    keep = ring.copy()
    keep[yfoot:, xcut:] = 0.0
    keep[:yfoot, rw // 2:] = 0.0
    out[top:top + rh, left:left + rw] = keep
    # upper ring: top band, right wall, lower right arc = foot running into the a (same arcs, ring only as high as the foot)
    up = _resize(O, rw, yfoot)
    up[:, :rw // 2] = 0.0
    up[W:yfoot - W, :rw - W - 3] = 0.0
    up[yfoot - W:, :ax + aw - W - left] = 0.0
    out[top:top + yfoot, left:left + rw] = np.maximum(out[top:top + yfoot, left:left + rw], up)
    out[ay:ay + ah, ax:ax + aw] = np.maximum(out[ay:ay + ah, ax:ax + aw], a)
    return out


def render(M, code):
    if not applies(M, code):
        return None
    if code in SLASH:
        return g_slashed(M, code)
    return {ETH_U: g_eth_u, ETH_L: g_eth_l, DEG: g_degree, AT: g_at}[code](M, code)
