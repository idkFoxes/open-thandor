"""Reads the game's .gfx image sets (as extracted by pck.py) and exports their images as PNG.

usage:
  python tools/data/gfx.py list <file.gfx>
  python tools/data/gfx.py export <file.gfx> <out dir> [index ...]

Layout (GraphicsTextureSourceAsset, include/thandor/graphics/resources/types.h, and the reader in
src/platform/sdl3/gpu_ui_textures.cpp): a 0x200-byte header (magic 'gfx\\0', subresource count at +0xB0, palette
bank count at +0xB4, subresource table offset at +0xB8), then the palette banks (256 entries of 8 bytes each, +0
ARGB8888), then the table of 0x20-byte records (logical width, logical height, palette index, data offset, origin
x, origin y, pixel width, pixel height). Palette index -1 = ARGB8888 texels, else 8-bit indices into that bank."""
import os
import struct
import sys

HEADER = 0x200
BANK = 0x800
MAGIC = 7890535  # 'gfx\0'


class Image:
    def __init__(self, index, logical, palette, offset, origin, size):
        self.index, self.logical, self.palette, self.offset = index, logical, palette, offset
        self.origin, self.size = origin, size


def read(path):
    """Returns (data, [Image]) of a .gfx file."""
    return parse(open(path, 'rb').read(), path)


def parse(data, name='gfx'):
    """Returns (data, [Image]) of the bytes of a .gfx asset (e.g. pck.read_entry of a package entry)."""
    magic = struct.unpack_from('<I', data, 0)[0]
    if magic != MAGIC:
        raise ValueError('%s: not a gfx asset' % name)
    count, banks, table = struct.unpack_from('<III', data, 0xB0)
    images = []
    for i in range(count):
        lw, lh, pal, off, ox, oy, pw, ph = struct.unpack_from('<IIiIiiII', data, table + i * 0x20)
        images.append(Image(i, (lw, lh), pal, off, (ox, oy), (pw, ph)))
    return data, images


def to_pil(data, image, bank=None):
    """One image as a Pillow RGBA image (bank: palette bank override for paletted images)."""
    from PIL import Image as PilImage
    w, h = image.size
    if image.palette == -1:
        raw = data[image.offset:image.offset + w * h * 4]
        return PilImage.frombytes('RGBA', (w, h), raw, 'raw', 'BGRA')
    base = HEADER + (image.palette if bank is None else bank) * BANK
    lut = bytearray()
    for i in range(256):
        b, g, r, a = data[base + i * 8:base + i * 8 + 4]
        lut += bytes((r, g, b, a))
    indices = data[image.offset:image.offset + w * h]
    out = bytearray(w * h * 4)
    for i, v in enumerate(indices):
        out[i * 4:i * 4 + 4] = lut[v * 4:v * 4 + 4]
    return PilImage.frombytes('RGBA', (w, h), bytes(out))


def main():
    if len(sys.argv) < 3 or sys.argv[1] not in ('list', 'export'):
        sys.exit(__doc__)
    data, images = read(sys.argv[2])
    if sys.argv[1] == 'list':
        for image in images:
            print('%4d %5dx%-5d palette %3d origin %d,%d' % (image.index, image.size[0], image.size[1],
                                                               image.palette, image.origin[0], image.origin[1]))
        print('%d images' % len(images))
        return
    out = sys.argv[3]
    os.makedirs(out, exist_ok=True)
    wanted = [int(v) for v in sys.argv[4:]] or range(len(images))
    for i in wanted:
        image = images[i]
        if image.size[0] and image.size[1]:
            to_pil(data, image).save(os.path.join(out, '%04d.png' % i))
    print('exported %d images' % len(wanted))


if __name__ == '__main__':
    main()
