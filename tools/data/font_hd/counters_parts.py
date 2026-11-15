"""Counters (closed inner areas, 'Punzen'): automatic check + the R leg clip.

check:  python counters_parts.py [font.gfx|fontk.gfx] [N ...]   -> list of glyphs with ink inside a counter

Three tests per glyph (result raster r of upscale3.render):
  orig   counter of the ORIGINAL glyph (enclosed free area, alpha < 0.15), upscaled and eroded by max(1, N//2)
         output px; any result ink > EPS there is reported (strokes / AA fringes reaching into the counter).
  stray  counter of the RESULT (enclosed area with r < 0.5); ink > EPS there that is not 8-adjacent to a
         solid pixel (r >= 0.5) is a detached residue (e.g. the top of a clipped leg).
  part   glyphs built from a part (R = P, Q = O, accented = base): inside the source's counter including its
         AA ring the result must equal the source pixel-exactly (nothing added there by leg / tail / accent).
The R leg itself: clip_leg() removes every leg pixel above the bottom edge of the middle bar and every leg pixel
inside P's counter (+ its AA ring), so R's counter is P's counter pixel-identically.
"""
import sys
from collections import deque
import numpy as np

EPS = 0.02


def holes(free):
    """enclosed components of a boolean 'free' mask (4-connected, not touching the border)."""
    h, w = free.shape
    seen = np.zeros_like(free)
    q = deque([(y, x) for y in range(h) for x in range(w) if (y in (0, h - 1) or x in (0, w - 1)) and free[y, x]])
    for p in q:
        seen[p] = True
    while q:
        y, x = q.popleft()
        for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            yy, xx = y + dy, x + dx
            if 0 <= yy < h and 0 <= xx < w and free[yy, xx] and not seen[yy, xx]:
                seen[yy, xx] = True
                q.append((yy, xx))
    return free & ~seen


def dil(m, r=1):
    out = m.copy()
    h, w = m.shape
    for dy in range(-r, r + 1):
        for dx in range(-r, r + 1):
            sh = np.zeros_like(m)
            sh[max(0, dy):h + min(0, dy), max(0, dx):w + min(0, dx)] = \
                m[max(0, -dy):h + min(0, -dy), max(0, -dx):w + min(0, -dx)]
            out |= sh
    return out


def ero(m, r=1):
    return ~dil(~m, r)


def counter_orig(M, code):
    """original counter, output resolution, eroded by max(1, N//2) (keeps away from the original's AA edge)."""
    a = M.g[code]
    hm = holes(a < 0.15)
    if not hm.any():
        return np.zeros((a.shape[0] * M.N, a.shape[1] * M.N), bool)
    big = np.kron(hm, np.ones((M.N, M.N))) > 0.5
    return ero(big, max(1, M.N // 2))


def counter_result(r):
    """counter of the result (r < 0.5 enclosed) and its AA ring (counter pixels next to solid ink)."""
    c = holes(r < 0.5)
    return c


# ------------------------------------------------------------------ R leg


def largest(m):
    """largest 4-connected component of a boolean mask."""
    h, w = m.shape
    lab = np.zeros(m.shape, int)
    best, n = (0, 0), 0
    for y0, x0 in zip(*np.nonzero(m)):
        if lab[y0, x0]:
            continue
        n += 1
        lab[y0, x0] = n
        q, cnt = deque([(y0, x0)]), 0
        while q:
            y, x = q.popleft()
            cnt += 1
            for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                yy, xx = y + dy, x + dx
                if 0 <= yy < h and 0 <= xx < w and m[yy, xx] and not lab[yy, xx]:
                    lab[yy, xx] = n
                    q.append((yy, xx))
        if cnt > best[0]:
            best = (cnt, n)
    return lab == best[1] if n else m


def clip_leg(P, leg, N):
    """the leg begins only below P's middle bar: all rows above the bar's bottom edge are cleared, and nothing of
    the leg may lie in P's counter or its AA ring (non-solid pixels next to the counter, enclosed by P's ink).
    bar bottom = first row below P's main counter whose mean coverage over the counter columns drops below 0.5
    (a per-column run would follow the stem where P's stem is narrower in the bowl, e.g. 3x W=5)."""
    cnt = holes(P < 0.5)
    if not cnt.any():
        return leg
    big = largest(cnt)
    ys, xs = np.nonzero(big)
    yb = ys.max()                                    # lowest counter row (just above the middle bar)
    span = slice(xs.min(), xs.max() + 1)             # counter columns (bar between stem and bowl)
    y = yb + 1
    while y < P.shape[0] and P[y, span].mean() >= 0.5:
        y += 1
    leg = leg.copy()
    leg[:y] = 0
    ring = holes(P < 0.999) & dil(cnt, 1)
    return np.where(cnt | ring, 0.0, leg)


# ------------------------------------------------------------------ check


def part_source(S, code):
    if code == ord('R'):
        return ord('P'), None
    if code == ord('Q'):
        return ord('O'), None
    import accents_parts as AP
    dec = AP.decomp(code) if code in AP.COMPOSE_OK else None
    if dec is not None:
        return dec[0], 'compose'
    return None, None


def check_glyph(M, S, code, r=None):
    """-> dict test -> list of (y, x, value) of offending pixels."""
    if r is None:
        r = S.render(M, code)
    out = {}
    co = counter_orig(M, code)
    if co.any() and co.shape == r.shape:
        ys, xs = np.nonzero(co & (r > EPS))
        out['orig'] = [(int(y), int(x), float(r[y, x])) for y, x in zip(ys, xs)]
    cr = counter_result(r)
    if cr.any():
        solid = dil(r >= 0.5, 1)
        ys, xs = np.nonzero(cr & (r > EPS) & ~solid)
        out['stray'] = [(int(y), int(x), float(r[y, x])) for y, x in zip(ys, xs)]
    src, how = part_source(S, code)
    if src is not None and M.g[src].max() > 0:
        Sr = S.render(M, src)
        if Sr.shape[0] >= r.shape[0]:
            Sr = Sr[:r.shape[0]]
            cnt = holes(Sr < 0.5)
            if cnt.any():
                zone = holes(Sr < 0.999) & dil(cnt, 1) | cnt
                if how == 'compose' or Sr.shape != r.shape:
                    # place the source where it sits in the result
                    pad = np.zeros_like(r)
                    pad[:, :min(r.shape[1], Sr.shape[1])] = Sr[:, :r.shape[1]]
                    zp = np.zeros(r.shape, bool)
                    zp[:, :min(r.shape[1], Sr.shape[1])] = zone[:, :r.shape[1]]
                    best = None
                    for dx in range(-r.shape[1], r.shape[1] + 1):
                        cv = np.roll(pad, dx, 1)
                        zz = np.roll(zp, dx, 1)
                        e = np.abs(cv - r)[zz].sum() if zz.any() else 1e9
                        if best is None or e < best[0]:
                            best = (e, cv, zz)
                    _, Sr, zone = best
                d = zone & (np.abs(r - Sr) > EPS)
                ys, xs = np.nonzero(d)
                out['part'] = [(int(y), int(x), float(r[y, x])) for y, x in zip(ys, xs)]
    return out


def check_all(M, S, codes=None):
    res = {}
    for c in (codes if codes is not None else range(0x21, 0x100)):
        if M.g[c].size == 0 or M.g[c].max() <= 0:
            continue
        try:
            r = S.render(M, c)
            has = holes(M.g[c] < 0.15).any() or part_source(S, c)[0] is not None or counter_result(r).any()
            if not has:
                continue
            res[c] = check_glyph(M, S, c, r)
        except Exception as e:                      # e.g. fontk compose (pre-existing)
            res[c] = {'error': [(0, 0, 0.0)]}
            res[c]['msg'] = '%s: %s' % (type(e).__name__, e)
    return res


def ch(c):
    return bytes([c]).decode('cp1252', 'replace')


if __name__ == '__main__':
    sys.stdout.reconfigure(errors='backslashreplace')  # glyph names are not ASCII; never fail on a cp1252 console
    import upscale as U
    import upscale3 as S
    fname = sys.argv[1] if len(sys.argv) > 1 else 'font.gfx'
    Ns = [int(v) for v in sys.argv[2:]] or [2, 3]
    F = U.load(fname)
    WD = {'font.gfx': {2: 3, 3: 4}, 'fontk.gfx': {2: 2, 3: 3}}[fname]
    kw = dict(r_big_src=3.17) if fname == 'font.gfx' else dict(level=0.42, sym_thr=0.03, r_big_src=1.5)
    for N in Ns:
        M = S.FontModel(F, N, W=WD[N], **kw)
        res = check_all(M, S)
        checked = ''.join(ch(c) for c in res)
        bad = {c: v for c, v in res.items() if any(v.values())}
        print('%s %dx: %d Glyphen mit Punze/Bauteil geprueft: %s' % (fname, N, len(res), checked))
        for c, v in bad.items():
            print('  %s (0x%02X): ' % (ch(c), c) + '  '.join('%s %d px (max %.2f) %s' % (
                k, len(p), max(q[2] for q in p), ' '.join('(%d,%d)' % (q[0], q[1]) for q in p[:8]))
                for k, p in v.items() if p and k != 'msg') + ('  ' + v['msg'] if 'msg' in v else ''))
        if not bad:
            print('  keine Tinte in Punzen')
