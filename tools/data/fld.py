"""Reads and writes field grids (.fld, the terrain of a level).

usage:
  python tools/data/fld.py dump <file.fld>
  python tools/data/fld.py flat <out.fld> <width> <height> [--like <stock.fld>] [--material N] [--height Q12]
                                [--water Q12]

A .fld as the game holds it in memory (FieldGridAsset, include/thandor/generated/types.h) is a 0x200-byte header
followed by width * height FieldGridCells of 0x80 bytes. In LEVEL.PCK it is stored with compression method 2
(PckCodec_DecodeFieldGrid; tools/data/pck.py extracts it expanded); loose next to the executable
(Package_LoadEntry) it is read as is, uncompressed, which is also how the map editor saves it
(FieldGrid_SaveAssetImageFromRuntimeState). `flat` writes that loose, expanded form.
Format notes: ot-scratch/fld_format.md."""
import argparse
import collections
import struct
import sys

HEADER = 0x200
CELL = 0x80
MAGIC = b"fld\0"
CONVERTER_VERSION = 0x00060006      # PCK_CONVERTER_FLD_SHT_00060006, checked by TerrainVisualResources_LoadPrimary
FORMAT_VERSION = 1
COLUMN_STEP_X = 0x901               # FIELD_GRID_WORLD_COLUMN_STEP_X
ROW_STEP_X = 0x480                  # FIELD_GRID_WORLD_ROW_STEP_X
ROW_STEP_Y = -1999                  # FIELD_GRID_WORLD_ROW_STEP_Y
STRAIGHT_UP_NORMAL = 0x4000 << 16   # triangle normal angles: elevation quarter turn, azimuth 0

# flagsAndMaterial (+0x50)
MATERIAL_MASK = 0xFF
VARIANT_MASK = 0x700                # random at load
XENITE = 0x800
TRITIUM = 0x1000
EDGE_MASK = 0x88006000              # rebuilt at load
DEBUG_MARK = 0x8000                 # cleared at load
RECEIVER_EXCLUDED = 0x20000000
SOURCE_EXCLUDED = 0x40000000


def s32(value):
    return value - 0x100000000 if value & 0x80000000 else value


def read(path):
    data = open(path, "rb").read()
    if data[:4] != MAGIC:
        sys.exit("%s: no 'fld' magic (a packed PCK entry? extract it with tools/data/pck.py)" % path)
    return data


def header_fields(data):
    magic, size, version, converter = struct.unpack_from("<4sIII", data, 0)
    field_flags, runtime_flags, width, height = struct.unpack_from("<4I", data, 0xB0)
    return dict(size=size, version=version, converter=converter, field_flags=field_flags,
                runtime_flags=runtime_flags, width=width, height=height)


def utf16(data, start, end):
    return data[start:end].decode("utf-16-le", "replace").split("\0")[0]


def cells(data, width, height):
    """(row, column, worldX, worldY, terrainHeight, waterSurfaceDelta, flagsAndMaterial, persistedAux54)"""
    for index in range(width * height):
        x, y, terrain, water, flags, aux = struct.unpack_from("<6I", data, HEADER + index * CELL + 0x40)
        yield index // width, index % width, s32(x), s32(y), s32(terrain), s32(water), flags, aux


def histogram(counter, limit=12):
    total = sum(counter.values())
    parts = ["%s:%d" % (key, count) for key, count in counter.most_common(limit)]
    if len(counter) > limit:
        parts.append("... (%d distinct)" % len(counter))
    return ", ".join(parts) + "  [n=%d]" % total


def dump(path):
    data = read(path)
    h = header_fields(data)
    width, height = h["width"], h["height"]
    print("file               %s (%d bytes)" % (path, len(data)))
    print("allocationSize     %d%s" % (h["size"], "" if h["size"] == len(data) else "  (!= file size)"))
    print("formatVersion      %d   converterVersion 0x%08X%s" % (h["version"], h["converter"],
          "" if h["converter"] == CONVERTER_VERSION else "  (game expects 0x%08X)" % CONVERTER_VERSION))
    print("timestamps         %s" % data[0x10:0x28].hex(" "))
    print("producer names     %r / %r" % (utf16(data, 0x30, 0x70), utf16(data, 0x70, 0xB0)))
    print("fieldFlags         0x%08X  materials %s" % (h["field_flags"],
          [i for i in range(26) if h["field_flags"] >> i & 1]))
    print("runtimeStateFlags  0x%08X" % h["runtime_flags"])
    print("grid               %d x %d (%d cells; expected size 0x200 + cells * 0x80 = %d)" % (
          width, height, width * height, HEADER + width * height * CELL))
    print("0xC0..0xFF         %s" % ("zero" if not any(data[0xC0:0x100]) else data[0xC0:0x100].hex()))
    print("sourcePath         %r" % utf16(data, 0x100, 0x200))
    if len(data) < HEADER + width * height * CELL:
        sys.exit("file too short for the grid")

    materials, flag_bits, heights, waters, aux = (collections.Counter() for _ in range(5))
    world_mismatch = 0
    dry = wet = 0
    min_h = max_h = None
    for row, column, x, y, terrain, water, flags, aux54 in cells(data, width, height):
        materials[flags & MATERIAL_MASK] += 1
        for bit in range(8, 32):
            if flags >> bit & 1:
                flag_bits["0x%08X" % (1 << bit)] += 1
        heights[terrain] += 1
        waters[water] += 1
        aux[aux54] += 1
        if (x, y) != (column * COLUMN_STEP_X + row * ROW_STEP_X, row * ROW_STEP_Y):
            world_mismatch += 1
        if water > 0:
            wet += 1
        else:
            dry += 1
        min_h = terrain if min_h is None else min(min_h, terrain)
        max_h = terrain if max_h is None else max(max_h, terrain)
    other = collections.Counter()
    for index in range(width * height):
        cell = data[HEADER + index * CELL:HEADER + (index + 1) * CELL]
        for start, end, name in ((0x00, 0x40, "0x00-0x3F"), (0x58, 0x80, "0x58-0x7F")):
            if any(cell[start:end]):
                other[name] += 1
    print()
    print("material (low byte) %s" % histogram(materials))
    print("flag bits           %s" % (histogram(flag_bits, 24) if flag_bits else "none"))
    print("terrainHeight       min %d max %d; %s" % (min_h, max_h, histogram(heights, 6)))
    print("waterSurfaceDelta   under water (> 0): %d, dry: %d; %s" % (wet, dry, histogram(waters, 6)))
    print("persistedAux54      %s" % histogram(aux, 4))
    print("worldX/worldY       %s" % ("regular lattice" if not world_mismatch else
                                      "%d cells off the lattice (or zero)" % world_mismatch))
    print("runtime-only bytes  %s" % (", ".join("%s nonzero in %d cells" % item for item in other.items())
                                       if other else "all zero"))


def build_flat(width, height, like, material, terrain, water):
    if like is not None:
        header = bytearray(read(like)[:HEADER])
    else:
        header = bytearray(HEADER)
        header[0:4] = MAGIC
        struct.pack_into("<II", header, 8, FORMAT_VERSION, CONVERTER_VERSION)
    size = HEADER + width * height * CELL
    struct.pack_into("<I", header, 4, size)
    # fieldFlags: the materials whose texture set must exist (the editor save writes the set of used ones)
    struct.pack_into("<4I", header, 0xB0, 1 << material, 0, width, height)
    header[0xC0:0x100] = bytes(0x40)
    header[0x100:0x200] = bytes(0x100)
    header[0x100:0x100 + 2 * len("flat.gfx")] = "flat.gfx".encode("utf-16-le")
    out = bytearray(size)
    out[:HEADER] = header
    for row in range(height):
        for column in range(width):
            cell = HEADER + (row * width + column) * CELL
            # the map-edge bits are rebuilt at load, but the stock files carry them too
            flags = material & MATERIAL_MASK
            flags |= 0x4000 if row == 0 else 0
            flags |= 0x80000000 if row == height - 1 else 0
            flags |= 0x2000 if column == 0 else 0
            flags |= 0x08000000 if column == width - 1 else 0
            struct.pack_into("<I", out, cell + 0x08, STRAIGHT_UP_NORMAL)
            struct.pack_into("<6I", out, cell + 0x40,
                             (column * COLUMN_STEP_X + row * ROW_STEP_X) & 0xFFFFFFFF,
                             (row * ROW_STEP_Y) & 0xFFFFFFFF,
                             terrain & 0xFFFFFFFF, water & 0xFFFFFFFF, flags, 0)
            struct.pack_into("<I", out, cell + 0x7C, 0)
            struct.pack_into("<I", out, cell + 0x78, STRAIGHT_UP_NORMAL)
    return bytes(out)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)
    p_dump = sub.add_parser("dump")
    p_dump.add_argument("file")
    p_flat = sub.add_parser("flat")
    p_flat.add_argument("out")
    p_flat.add_argument("width", type=int)
    p_flat.add_argument("height", type=int)
    p_flat.add_argument("--like", help="stock .fld (extracted) whose header prefix (version, timestamps) is copied")
    p_flat.add_argument("--material", type=int, default=1,
                        help="ground material index 0..25 = texture <ground base>a..z.gfx (default 1, the flat "
                             "ground of the tutorial maps)")
    p_flat.add_argument("--height", type=int, default=0x1000, dest="terrain",
                        help="terrainHeight in Q12 (default 0x1000)")
    p_flat.add_argument("--water", type=int, default=None,
                        help="waterSurfaceDelta in Q12, <= 0 is dry (default -height: water level 0 under the "
                             "ground, as in t00_tut/t01_tut)")
    args = parser.parse_args()
    if args.command == "dump":
        dump(args.file)
        return
    if not 0 <= args.material < 26:
        sys.exit("material must be 0..25 (TERRAIN_MATERIAL_COUNT)")
    if args.width < 4 or args.height < 4:
        sys.exit("width and height must be at least 4 (the outer ring is border)")
    if args.water is None:
        args.water = -args.terrain
    if args.water > 0:
        print("warning: waterSurfaceDelta > 0 puts every cell under water", file=sys.stderr)
    if args.width % 4 != 3 or args.height % 4 != 3:
        sys.exit("width and height must be 4k+3: the editor grid-vertex overlay (SelectionOverlay_DrawGridVertexMarkers, "
                 "0x0052F680) visits cells 1, 5, ..., 4*(n>>2)+1, which is past the last interior cell otherwise")
    if args.width % 8 != 3 or args.height % 8 != 3:
        print("warning: every stock map is 8k+3 cells on each side (67, 75, ..., 139); %dx%d is untested"
              % (args.width, args.height), file=sys.stderr)
    data = build_flat(args.width, args.height, args.like, args.material, args.terrain, args.water)
    open(args.out, "wb").write(data)
    print("wrote %s: %dx%d, %d bytes, material %d, height %d, water delta %d" % (
          args.out, args.width, args.height, len(data), args.material, args.terrain, args.water))


if __name__ == "__main__":
    main()
