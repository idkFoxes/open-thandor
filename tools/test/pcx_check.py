"""Writes a test PCX for OPEN_THANDOR_SELFTEST=pcx and prints the log line the decoder must produce.

usage: pcx_check.py GAME_DIR [--size 64] [--odd]
Creates GAME_DIR/pcxtest.pcx (8-bit, 256-colour palette, PIL's RLE encoder) with a pattern that has runs and
single bytes >= 0xC0, then prints "pcx: WxH hash XXXXXXXX" over the palette as 0xFFRRGGBB dwords and the pixels.
Run the game with OPEN_THANDOR_SELFTEST=pcx in GAME_DIR and compare the last line of thandor.log.
"""
import argparse
import os

from PIL import Image


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('game_dir')
    parser.add_argument('--size', type=int, default=64)
    parser.add_argument('--odd', action='store_true', help='odd width (size + 1), padded scan lines')
    args = parser.parse_args()
    width, height = args.size + (1 if args.odd else 0), args.size
    image = Image.new('P', (width, height))
    palette = []
    for i in range(256):
        palette += [i, (i * 7) & 255, 255 - i]
    image.putpalette(palette)
    image.putdata([((x // 5) * 13 + y * 3) & 255 if (x + y) % 3 else 0xC3 for y in range(height) for x in range(width)])
    path = os.path.join(args.game_dir, 'pcxtest.pcx')
    image.save(path)
    value = 2166136261
    for i in range(256):
        r, g, b = palette[3 * i:3 * i + 3]
        for byte in (b, g, r, 0xFF):
            value = ((value ^ byte) * 16777619) & 0xFFFFFFFF
    for pixel in image.tobytes():
        value = ((value ^ pixel) * 16777619) & 0xFFFFFFFF
    print('pcx: %ux%u hash %08X' % (width, height, value))


if __name__ == '__main__':
    main()
