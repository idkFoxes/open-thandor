import sys, numpy as np, upscale as U, upscale3 as S
FONT_KW = {'fontk.gfx': dict(level=0.42, sym_thr=0.03, r_big_src=1.5), 'font.gfx': dict(r_big_src=3.17)}
WDEF = {'font.gfx': {2: 3, 3: 4}, 'fontk.gfx': {2: 2, 3: 3}}
def model(fn, N):
    return S.FontModel(U.load(fn), N, W=WDEF[fn][N], **FONT_KW[fn])
if __name__ == '__main__':
    M = model(sys.argv[1], int(sys.argv[2]))
    print('W', M.W, 'D', M.D, 'baseline', M.baseline)
    for ch in sys.argv[3]:
        c = ch.encode('cp1252')[0]
        r, info = S.render(M, c, True)
        print(repr(ch), r.shape, info['conf'])
        for y, row in enumerate(r):
            if row.max() > 0.02:
                print('%2d ' % y + ''.join(' .:-=+*#%@'[min(9, int(v * 9.99))] for v in row))
