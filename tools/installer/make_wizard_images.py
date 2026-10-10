"""Makes the installer's wizard images from an in-game screenshot and the game's own Thandor logo.

usage: make_wizard_images.py GAME_DIR SHOT.bmp [--crop X,Y,W,H] [--out DIR] [--preview DIR]
                             [--original PATCH5_IMAGE] [--version 1.0.7] [--bottom "OPEN THANDOR"]

Large image (WizardImageFile, left side of the welcome and finish pages), in the style of Patch 5's picture:
a portrait 164:314 crop of the playfield of SHOT (take_wizard_shots.py; --crop in shot pixels, W:H is corrected to
164:314 around the crop's centre; default: the full shot height at the playfield's centre), a dark band at the
top with the Thandor logo, the version in metallic letters below it, and a dark strip at the bottom with the
project name. Small image (WizardSmallImageFile, top right of the inner pages): the logo's globe on black.
Each size is composed at its own resolution (the scene and logo scaled down with Lanczos from the full-size
sources, the text drawn at that size), so every DPI variant is sharp.

The logo comes from the game: gfx\\panel\\credits.gfx in GAME_DIR\\GRAPHIK.PCK, image 12 (the last credits page:
logo, "DIE INVASION", www.thandor.de, drawn over the faded logo every credits page has). That faded background is
the per-pixel minimum of the other credits pages (their texts sit in different places); the logo's mask is where
page 12 differs from it, holes closed, the URL line cut off.

Output (default: <repo>/build-installer-images, ignored by git; the committed set is tools/installer/images,
made with --out tools/installer/images, see README.md): 24-bit BMPs
wizard-image-<W>x<H>.bmp and wizard-small-image-<N>.bmp in every size of the Inno Setup 7 tables (the image
area at 100 ... 250 % DPI, current and pre-6.6 layouts), plus wizard-files.txt with the WizardImageFile /
WizardSmallImageFile lines. --preview DIR also writes PNG previews and compare.png (Patch 5's image, given with
--original, beside ours at 100 %, and both enlarged 2x).

The images contain the original game's art (logo, screenshot); the owner decided to commit the finished set."""
import argparse
import os
import sys

import numpy as np
from PIL import Image, ImageChops, ImageDraw, ImageEnhance, ImageFilter, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(REPO, 'tools', 'data'))
import gfx  # noqa: E402
import pck  # noqa: E402

ASPECT = 164 / 314
# Inno Setup 7 help, WizardImageFile / WizardSmallImageFile: image area at 100, 125, ... 250 % DPI with the
# default settings since 6.6.0, then with the settings before 6.6.0 (the classic Patch 5 size 164x314 is 100 %)
LARGE_SIZES = [(202, 386), (269, 515), (336, 643), (403, 772), (430, 824), (498, 953), (534, 1022),
               (164, 314), (240, 459), (290, 556), (315, 604), (366, 700), (416, 797)]
SMALL_SIZES = [58, 77, 97, 116, 124, 143, 159, 55, 71, 85, 103, 112, 129, 147]
CREDITS = 'gfx\\panel\\credits.gfx'
LOGO_PAGE = 12
LOGO_BOTTOM = 296  # credits page 12: the www.thandor.de line starts below this row
FONT_CANDIDATES = {
    'version': ['bahnschrift.ttf', 'arialbd.ttf', 'DejaVuSans-Bold.ttf'],
    'bottom': ['bahnschrift.ttf', 'verdanab.ttf', 'arialbd.ttf', 'DejaVuSans-Bold.ttf'],
}


# ------------------------------------------------------------------------------------------------ sources

def credits_pages(game_dir):
    data, items = pck.entries(os.path.join(game_dir, 'GRAPHIK.PCK'))
    entry = next((e for e in items if e[0].lower() == CREDITS), None)
    if entry is None:
        sys.exit('%s not found in GRAPHIK.PCK' % CREDITS)
    asset, images = gfx.parse(pck.read_entry(data, entry), CREDITS)
    return [gfx.to_pil(asset, image).convert('RGB') for image in images]


def fill_holes(mask):
    """mask: bool array; everything not reachable from the border through False pixels becomes True."""
    outside = Image.fromarray(np.where(mask, 0, 255).astype(np.uint8))
    padded = Image.new('L', (outside.width + 2, outside.height + 2), 255)
    padded.paste(outside, (1, 1))
    ImageDraw.floodfill(padded, (0, 0), 128)
    reached = np.asarray(padded)[1:-1, 1:-1] == 128
    return ~reached


def brighten(image):
    """The credits page is drawn dim; lift it to the bright, slightly cool silver of Patch 5's logo."""
    a = np.asarray(image).astype(float) / 255.0
    a = np.clip(a * 1.35, 0, 1) ** 0.85
    a = a * np.array([0.97, 0.99, 1.04])
    return Image.fromarray((np.clip(a, 0, 1) * 255 + 0.5).astype(np.uint8))


def logo_and_globe(game_dir):
    """(logo RGBA cut to its bounding box, globe RGB square on black) from the credits pages."""
    pages = credits_pages(game_dir)
    page = np.asarray(pages[LOGO_PAGE]).astype(int)
    background = np.min(np.stack([np.asarray(p).astype(int) for i, p in enumerate(pages) if i != LOGO_PAGE]),
                        axis=0)
    mask = np.abs(page - background).max(axis=2) > 8
    mask[LOGO_BOTTOM:, :] = False
    mask = fill_holes(mask)
    alpha = Image.fromarray((mask * 255).astype(np.uint8)).filter(ImageFilter.GaussianBlur(0.6))
    logo = brighten(Image.fromarray(page.astype(np.uint8))).convert('RGBA')
    logo.putalpha(alpha)
    ys, xs = np.nonzero(mask)
    logo = logo.crop((xs.min() - 2, ys.min() - 2, xs.max() + 3, ys.max() + 3))

    # the globe: the ring around the centre of the wings, cut out round
    rows = np.nonzero(mask[:, (xs.min() + xs.max()) // 2])[0]
    top = rows.min()
    cx = (xs.min() + xs.max()) / 2.0
    radius = 88
    cy = top + radius - 2
    box = (int(cx - radius - 4), int(cy - radius - 4), int(cx + radius + 4), int(cy + radius + 4))
    globe = brighten(Image.fromarray(page.astype(np.uint8))).crop(box)
    size = globe.width * 4
    round_mask = Image.new('L', (size, size), 0)
    ImageDraw.Draw(round_mask).ellipse((16, 16, size - 16, size - 16), fill=255)
    round_mask = round_mask.resize(globe.size, Image.LANCZOS)
    globe = Image.composite(globe, Image.new('RGB', globe.size), round_mask)
    return logo, globe


def scene_crop(shot, crop):
    """The portrait crop of the playfield (aspect 164:314)."""
    if crop:
        x, y, w, h = crop
        cx, cy = x + w / 2.0, y + h / 2.0
        h = min(h, w / ASPECT)
        w = h * ASPECT
    else:
        playfield = playfield_width(shot)
        h = shot.height
        w = h * ASPECT
        cx, cy = playfield / 2.0, h / 2.0
    box = (int(round(cx - w / 2)), int(round(cy - h / 2)), int(round(cx + w / 2)), int(round(cy + h / 2)))
    if box[0] < 0 or box[1] < 0 or box[2] > shot.width or box[3] > shot.height:
        sys.exit('crop %s is outside the shot (%dx%d)' % (box, shot.width, shot.height))
    return shot.crop(box)


def playfield_width(shot):
    """Width of the playfield left of the side panel: the panel (gfx\\panel\\panel2.gfx at high resolutions) is
    about 140 px wide, its top bar with the resource gauges reaches 290 px from the right edge."""
    return shot.width - 290


# -------------------------------------------------------------------------------------------- composition

def font(kind, size):
    for name in FONT_CANDIDATES[kind]:
        for folder in (os.path.join(os.environ.get('WINDIR', 'C:\\Windows'), 'Fonts'), ''):
            try:
                f = ImageFont.truetype(os.path.join(folder, name) if folder else name, size)
                if name == 'bahnschrift.ttf':
                    try:
                        f.set_variation_by_name('Bold' if kind == 'version' else 'SemiBold')
                    except Exception:
                        pass
                return f
            except OSError:
                continue
    return ImageFont.load_default(size)


def scaled(image, size):
    """Lanczos resize with premultiplied alpha (no dark fringes)."""
    if image.mode == 'RGBA':
        return image.convert('RGBa').resize(size, Image.LANCZOS).convert('RGBA')
    return image.resize(size, Image.LANCZOS)


def vertical_gradient(width, height, stops):
    """L image: stops = [(y fraction, value)], linear in between."""
    ys = np.linspace(0, 1, height)
    fractions, values = zip(*stops)
    column = np.interp(ys, fractions, values)
    return Image.fromarray(np.repeat(column[:, None], width, axis=1).astype(np.uint8))


def metallic_text(text, height, kind='version', tracking=0.0):
    """RGBA text with a brushed-metal vertical gradient, dark outline and drop shadow, cap height ~height."""
    f = font(kind, int(round(height * 1.38)))
    left, top, right, bottom = f.getbbox(text, anchor='ls')
    advance = [f.getlength(c) for c in text]
    extra = tracking * height
    width = int(sum(advance) + extra * (len(text) - 1)) + 8
    stroke = max(1, int(round(height / 12.0)))
    canvas_h = (bottom - top) + 4 * stroke + 6
    mask = Image.new('L', (width + 4 * stroke, canvas_h), 0)
    draw = ImageDraw.Draw(mask)
    x = 2 * stroke + 2
    base = -top + 2 * stroke + 2
    outline = Image.new('L', mask.size, 0)
    outline_draw = ImageDraw.Draw(outline)
    for c, a in zip(text, advance):
        draw.text((x, base), c, font=f, fill=255, anchor='ls')
        outline_draw.text((x, base), c, font=f, fill=255, anchor='ls', stroke_width=stroke, stroke_fill=255)
        x += a + extra
    ink = np.asarray(mask)
    rows = np.nonzero(ink.max(axis=1))[0]
    y0, y1 = rows.min(), rows.max() + 1
    # metal: bright top, dark band just above the middle, bright again, darker bottom (like the THANDOR letters)
    gradient = vertical_gradient(mask.width, mask.height, [
        (0, 150), (y0 / mask.height, 245), ((y0 + 0.42 * (y1 - y0)) / mask.height, 120),
        ((y0 + 0.55 * (y1 - y0)) / mask.height, 225), (y1 / mask.height, 150), (1, 120)])
    metal = Image.merge('RGB', (gradient, gradient, ImageChops.multiply(gradient, Image.new('L', mask.size, 248))))
    result = Image.new('RGBA', mask.size, (0, 0, 0, 0))
    shadow = outline.filter(ImageFilter.GaussianBlur(stroke))
    shadow_layer = Image.new('RGBA', mask.size, (0, 0, 0, 255))
    shadow_layer.putalpha(ImageChops.multiply(shadow, Image.new('L', mask.size, 200)))
    result.alpha_composite(shadow_layer, (0, 0))
    edge = Image.new('RGBA', mask.size, (20, 22, 26, 255))
    edge.putalpha(outline)
    result.alpha_composite(edge)
    face = metal.convert('RGBA')
    face.putalpha(mask)
    result.alpha_composite(face)
    return result.crop(result.getbbox())


def plain_text(text, height, color, tracking=0.0):
    f = font('bottom', int(round(height * 1.38)))
    advance = [f.getlength(c) for c in text]
    extra = tracking * height
    width = int(sum(advance) + extra * (len(text) - 1)) + 8
    _, top, _, bottom = f.getbbox(text, anchor='ls')
    canvas = Image.new('RGBA', (width + 6, bottom - top + 8), (0, 0, 0, 0))
    shadow = Image.new('L', canvas.size, 0)
    sd = ImageDraw.Draw(shadow)
    draw = ImageDraw.Draw(canvas)
    x = 3
    for c, a in zip(text, advance):
        sd.text((x + 1, -top + 4 + 1), c, font=f, fill=255, anchor='ls')
        x += a + extra
    shadow_layer = Image.new('RGBA', canvas.size, (0, 0, 0, 255))
    shadow_layer.putalpha(shadow.filter(ImageFilter.GaussianBlur(0.6)))
    canvas.alpha_composite(shadow_layer)
    x = 3
    for c, a in zip(text, advance):
        draw.text((x, -top + 4), c, font=f, fill=color, anchor='ls')
        x += a + extra
    return canvas.crop(canvas.getbbox())


def paste_centred(base, layer, centre_x, top):
    base.alpha_composite(layer, (int(round(centre_x - layer.width / 2.0)), int(round(top))))


def large_image(scene, logo, size, version, bottom):
    w, h = size
    u = w / 164.0
    image = scaled(scene, size)
    image = ImageEnhance.Contrast(image).enhance(1.08)
    image = ImageEnhance.Color(image).enhance(1.12)
    image = image.filter(ImageFilter.UnsharpMask(radius=max(0.6, 0.8 * u), percent=60, threshold=1))
    image = image.convert('RGBA')
    # dark band behind the logo, fading into the scene; dark strip at the bottom
    shade = vertical_gradient(w, h, [(0, 200), (0.25, 170), (0.38, 60), (0.47, 0), (0.88, 0), (0.94, 170),
                                     (1, 215)])
    dark = Image.new('RGBA', size, (14, 20, 32, 255))
    dark.putalpha(shade)
    image.alpha_composite(dark)
    logo_w = int(round(150 * u))
    logo_h = int(round(logo.height * logo_w / float(logo.width)))
    paste_centred(image, scaled(logo, (logo_w, logo_h)), w / 2.0, 7 * u)
    if version:
        text = metallic_text(version, 20 * u, 'version', tracking=0.08)
        paste_centred(image, text, w / 2.0, 7 * u + logo_h + 5 * u)
    if bottom:
        text = plain_text(bottom, 6.6 * u, (225, 228, 232, 255), tracking=0.22)
        paste_centred(image, text, w / 2.0, h - 5 * u - text.height)
    return image.convert('RGB')


def small_image(globe, n):
    image = scaled(globe, (n, n))
    return image.filter(ImageFilter.UnsharpMask(radius=0.7, percent=50, threshold=1))


# --------------------------------------------------------------------------------------------------- main

def compare_sheet(original, ours, original_small, ours_small):
    """Patch 5 beside ours at 100 % (left), then both 2x enlarged (nearest), small images below."""
    gap = 12
    big = [original, ours]
    width = 2 * 164 + gap * 3 + 2 * (328 + gap)
    height = 628 + 2 * gap
    sheet = Image.new('RGB', (width, height), (240, 240, 240))
    x = gap
    for image in big:
        if image is not None:
            sheet.paste(image, (x, gap))
        x += 164 + gap
    for image in big:
        if image is not None:
            sheet.paste(image.resize((328, 628), Image.NEAREST), (x, gap))
        x += 328 + gap
    y = 314 + 2 * gap
    x = gap
    for image in (original_small, ours_small):
        if image is not None:
            sheet.paste(image, (x, y))
            sheet.paste(image.resize((image.width * 2, image.height * 2), Image.NEAREST), (x, y + 60 + gap))
        x += 164 + gap
    return sheet


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('game_dir', help='Thandor installation (GRAPHIK.PCK)')
    parser.add_argument('shot', help='in-game screenshot (take_wizard_shots.py)')
    parser.add_argument('--crop', help='X,Y,W,H in shot pixels (default: full height at the playfield centre)')
    parser.add_argument('--out', default=os.path.join(REPO, 'build-installer-images'))
    parser.add_argument('--preview', help='also write PNG previews and compare.png here')
    parser.add_argument('--original', help="Patch 5's 164x314 image for compare.png")
    parser.add_argument('--original-small', help="Patch 5's 55x55 image for compare.png")
    parser.add_argument('--version', default='1.0.7')
    parser.add_argument('--bottom', default='OPEN THANDOR')
    args = parser.parse_args()

    crop = tuple(int(v) for v in args.crop.split(',')) if args.crop else None
    shot = Image.open(args.shot).convert('RGB')
    scene = scene_crop(shot, crop)
    logo, globe = logo_and_globe(args.game_dir)
    os.makedirs(args.out, exist_ok=True)
    large_names, small_names = [], []
    made = {}
    for size in sorted(LARGE_SIZES):
        image = large_image(scene, logo, size, args.version, args.bottom)
        name = 'wizard-image-%dx%d.bmp' % size
        image.save(os.path.join(args.out, name))  # RGB: 24-bit BMP
        large_names.append(name)
        made[size] = image
    for n in sorted(SMALL_SIZES):
        image = small_image(globe, n)
        name = 'wizard-small-image-%d.bmp' % n
        image.save(os.path.join(args.out, name))
        small_names.append(name)
        made[n] = image
    with open(os.path.join(args.out, 'wizard-files.txt'), 'w') as f:
        f.write('WizardImageFile=%s\n' % ','.join(large_names))
        f.write('WizardSmallImageFile=%s\n' % ','.join(small_names))
    print('%d large and %d small images in %s (scene %dx%d from %s)' % (
        len(large_names), len(small_names), args.out, scene.width, scene.height, os.path.basename(args.shot)))
    if args.preview:
        os.makedirs(args.preview, exist_ok=True)
        for key in ((164, 314), (328, 628) if (328, 628) in made else (336, 643), (534, 1022), 55, 58, 159):
            name = ('wizard-image-%dx%d.png' % key) if isinstance(key, tuple) else ('wizard-small-image-%d.png' % key)
            made[key].save(os.path.join(args.preview, name))
        original = Image.open(args.original).convert('RGB') if args.original else None
        original_small = Image.open(args.original_small).convert('RGB') if args.original_small else None
        compare_sheet(original, made[(164, 314)], original_small, made[55]).save(
            os.path.join(args.preview, 'compare.png'))
        print('previews and compare.png in %s' % args.preview)


if __name__ == '__main__':
    main()
