"""Builds one contact sheet per mission run of run_all_maps.py.

usage: soak_sheets.py SOAK_DIR

Writes SOAK_DIR/<run>/sheet.png with every screenshot of that run as a 480x300 thumbnail, four per row, in the
order they were taken, and prints the number of screenshots per run.
"""
import glob
import os
import sys

from PIL import Image

soak = sys.argv[1]
for run in sorted(os.listdir(soak)):
    files = sorted(glob.glob(os.path.join(soak, run, 'shot_*.bmp')))
    if not files:
        continue
    thumbs = [Image.open(f).convert('RGB').resize((480, 300)) for f in files]
    sheet = Image.new('RGB', (480 * 4, 300 * ((len(thumbs) + 3) // 4)))
    for i, thumb in enumerate(thumbs):
        sheet.paste(thumb, ((i % 4) * 480, (i // 4) * 300))
    sheet.save(os.path.join(soak, run, 'sheet.png'))
    print('%s: %d screenshots' % (run, len(files)))
