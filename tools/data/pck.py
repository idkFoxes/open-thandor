"""Reads the game's PCK packages (LEVEL.PCK, GRAPHIK.PCK, ...).

usage:
  python tools/data/pck.py list <package.pck>
  python tools/data/pck.py extract <package.pck> <out dir> [substring]

Layout (include/thandor/generated/types.h): a 0x200-byte PckArchiveHeader (entry count at +0xB0), then the
entries one after another: a 0x200-byte PckEntryHeader (UTF-16 path, runtime payload offset, unpacked size,
type tag, stored size, compression method) followed by the stored bytes. Compression method 0 = the Huffman/RLE
codec of PckCodec_DecodeHuffmanRle (256-byte frequency table, then the bit stream; entries whose stored size
equals the unpacked size are raw), 1 = stored, 2 = field grid (PckCodec_DecodeFieldGrid, the .fld entries)."""
import os
import struct
import sys

ARCHIVE_HEADER = 0x200
ENTRY_HEADER = 0x200


def decode_huffman_rle(src, outsize):
    """Port of PckCodec_DecodeHuffmanRle: the code tree is rebuilt from the 256 byte weights exactly as the
    original does (always merging the two lowest non-zero weights, first found wins); each token is a 0 bit and
    a literal code, or a 1 bit, a 4-bit run length - 3 and the code of the repeated byte."""
    freq = list(src[:256])
    nodes = 512
    weight = [0] * nodes
    zero = [None] * nodes
    one = [None] * nodes
    for i in range(256):
        weight[i] = freq[i]
    next_node = 256
    while True:
        low = sec = 0xFFFFFFFF
        low_node = sec_node = None
        for i in range(nodes):
            if weight[i] != 0:
                if weight[i] < low:
                    if low < sec:
                        sec, sec_node = low, low_node
                    low, low_node = weight[i], i
                elif weight[i] < sec:
                    sec, sec_node = weight[i], i
        if sec >= 0x80000000:
            break
        weight[next_node] = low + sec
        zero[next_node], one[next_node] = low_node, sec_node
        weight[low_node] = weight[sec_node] = 0
        next_node += 1
    root = next_node - 1
    data = src[256:] + b"\0" * 8
    pos = bit = 0
    out = bytearray()
    while len(out) < outsize:
        window = struct.unpack_from("<I", data, pos)[0] >> bit
        if window & 1 == 0:
            code, node, bit = window >> 1, root, bit + 1
            while zero[node] is not None:
                node = one[node] if code & 1 else zero[node]
                code >>= 1
                bit += 1
            out.append(node)
        else:
            code, node, bit = window >> 5, root, bit + 5
            while zero[node] is not None:
                node = one[node] if code & 1 else zero[node]
                code >>= 1
                bit += 1
            out.extend([node] * min(((window >> 1) & 0xF) + 3, outsize - len(out)))
        pos += bit >> 3
        bit &= 7
    return bytes(out)


FIELD_GRID_PREFIX = 0x10        # PCK_FIELD_GRID_PREFIX_BYTES: dword 0 = compact image size, rest unused
FIELD_GRID_HEADER = 0x200       # FieldGridAsset up to cells[]
FIELD_GRID_CELL = 0x80          # sizeof(FieldGridCell)


def decode_field_grid(src, outsize):
    """Port of PckCodec_DecodeFieldGrid (compression method 2): a 0x10-byte prefix whose first dword is the size
    of the compact image, then that image packed with method 0. The compact image is the 0x200-byte header
    followed by one 0x10-byte record per cell (persistedAux54, terrainHeight, waterSurfaceDelta,
    flagsAndMaterial); every record is expanded into a zeroed 0x80-byte FieldGridCell (offsets 0x54, 0x48, 0x4C,
    0x50) and worldX/worldY (+0x40/+0x44) are regenerated: column * 0x901 + row * 0x480, row * -1999."""
    compact_size = struct.unpack_from("<I", src, 0)[0]
    compact = decode_huffman_rle(src[FIELD_GRID_PREFIX:], compact_size)
    width, height = struct.unpack_from("<II", compact, 0xB8)
    out = bytearray(FIELD_GRID_HEADER + width * height * FIELD_GRID_CELL)
    out[:FIELD_GRID_HEADER] = compact[:FIELD_GRID_HEADER]
    for row in range(height):
        for column in range(width):
            index = row * width + column
            aux, terrain, water, flags = struct.unpack_from("<4I", compact, FIELD_GRID_HEADER + index * 0x10)
            cell = FIELD_GRID_HEADER + index * FIELD_GRID_CELL
            world_x = (column * 0x901 + row * 0x480) & 0xFFFFFFFF
            world_y = (row * -1999) & 0xFFFFFFFF
            struct.pack_into("<6I", out, cell + 0x40, world_x, world_y, terrain, water, flags, aux)
    if len(out) != outsize:
        raise ValueError("field grid unpacks to %d bytes, entry says %d" % (len(out), outsize))
    return bytes(out)


def entries(path):
    """(name, type tag, unpacked size, stored size, compression, data offset) for every entry."""
    data = open(path, "rb").read()
    count = struct.unpack_from("<I", data, 0xB0)[0]
    offset = ARCHIVE_HEADER
    result = []
    for _ in range(count):
        # garbage may follow the terminator, so decode only up to it
        name = data[offset:offset + 492].decode("utf-16-le", "replace").split("\0")[0]
        payload, unpacked, tag, stored, method = struct.unpack_from("<I I 4s I I", data, offset + 492)
        result.append((name, tag.rstrip(b"\0").decode("latin-1"), unpacked, stored, method, offset + ENTRY_HEADER))
        offset += ENTRY_HEADER + stored
    return data, result


def read_entry(data, entry):
    name, tag, unpacked, stored, method, start = entry
    raw = data[start:start + stored]
    # g_PckDecoderTable: 0 Huffman/RLE, 1 stored, 2 field grid. The stock packages use 0 (or stored-size ==
    # unpacked-size for tiny entries) and 2 for the .fld entries.
    if method == 2:
        return decode_field_grid(raw, unpacked)
    if method == 1 or stored == unpacked:
        return raw[:unpacked]
    return decode_huffman_rle(raw, unpacked)


def main():
    command, package = sys.argv[1], sys.argv[2]
    data, items = entries(package)
    if command == "list":
        for name, tag, unpacked, stored, method, _ in items:
            print("%-40s %-4s %8d %8d %d" % (name, tag, unpacked, stored, method))
        print(len(items), "entries")
    elif command == "extract":
        out_dir = sys.argv[3]
        pattern = sys.argv[4].lower() if len(sys.argv) > 4 else ""
        for entry in items:
            if pattern in entry[0].lower():
                target = os.path.join(out_dir, *entry[0].split("\\"))
                os.makedirs(os.path.dirname(target), exist_ok=True)
                open(target, "wb").write(read_entry(data, entry))
                print("extracted", entry[0])


if __name__ == "__main__":
    main()
