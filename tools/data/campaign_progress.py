"""Dumps the campaign progress file save\\kampagne.dat (src/gameplay/session/campaign_progress.cpp).

usage:
  python tools/data/campaign_progress.py <kampagne.dat> [--units]

The file is a PCK package (tools/data/pck.py lists and extracts it): progress.bin holds the sections, one
carry_<name>_<target>.hex per carry-over snapshot holds a small header and the oldunit.hex layout. Every payload
starts with tag, version, data size and the CRC32 of the data behind that 16-byte header; the dump checks them.
Layouts: include/thandor/gameplay/session/campaign_progress.h. --units lists the units of every snapshot."""
import os
import struct
import sys
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pck  # noqa: E402

HEADER = 16
NAME_BYTES = 32
UNIT_CAPACITY = 0x200
RECORD_DWORDS = 8
TECH_WORDS = 64
CARRY_HEADER = 0x20


def check_payload(name, payload, tag):
    """The data of a payload behind its header; raises ValueError when the header or the CRC32 is wrong."""
    got_tag, version, size, crc = struct.unpack_from("<4sIII", payload, 0)
    if got_tag != tag:
        raise ValueError("%s: tag %r, expected %r" % (name, got_tag, tag))
    if version != 1:
        raise ValueError("%s: version %d" % (name, version))
    padded = size + (4 if size % 16 == 0 else 0)
    if size < HEADER or len(payload) != padded or any(payload[size:]):
        raise ValueError("%s: size %d does not match %d payload bytes" % (name, size, len(payload)))
    if zlib.crc32(payload[HEADER:size]) != crc:
        raise ValueError("%s: CRC32 mismatch (damaged or edited)" % name)
    return payload[:size]


def main():
    path = sys.argv[1]
    show_units = "--units" in sys.argv[2:]
    data, items = pck.entries(path)
    payloads = {entry[0]: pck.read_entry(data, entry) for entry in items}
    main_data = check_payload("progress.bin", payloads["progress.bin"], b"kpg\0")
    offset = HEADER
    (section_count,) = struct.unpack_from("<I", main_data, offset)
    offset += 4
    print("%d sections, %d entries" % (section_count, len(items)))
    for _ in range(section_count):
        name = main_data[offset:offset + NAME_BYTES].split(b"\0")[0].decode("ascii", "replace")
        offset += NAME_BYTES
        flags, crc, records, win_count = struct.unpack_from("<IIiI", main_data, offset)
        offset += 16
        print("[campaign %s]" % name)
        if flags & 1:
            print("  fingerprint %08x %d" % (crc, records))
        for _ in range(win_count):
            level, mask, last = struct.unpack_from("<iBB", main_data, offset)
            offset += 8
            selections = [s for s in range(8) if mask >> s & 1]
            print("  done %d selections %s last %d" % (level, selections, last))
        (unlocked_count,) = struct.unpack_from("<I", main_data, offset)
        unlocked = struct.unpack_from("<%di" % unlocked_count, main_data, offset + 4)
        offset += 4 + 4 * unlocked_count
        if unlocked:
            print("  unlocked %s" % " ".join(str(level) for level in unlocked))
        (carry_count,) = struct.unpack_from("<I", main_data, offset)
        targets = struct.unpack_from("<%di" % carry_count, main_data, offset + 4)
        offset += 4 + 4 * carry_count
        for target in targets:
            entry = "carry_%s_%d.hex" % (name.lower(), target)
            carry = check_payload(entry, payloads[entry], b"kco\0")
            stored_target, source, selection, _, count = struct.unpack_from("<iiiII", carry, HEADER)
            records_at = CARRY_HEADER + 4
            tech = struct.unpack_from("<%dI" % TECH_WORDS, carry, records_at + UNIT_CAPACITY * RECORD_DWORDS * 4)
            factions = [f for f in range(8) if any(tech[f * 8:f * 8 + 8])]
            print("  carry %d from %d sel %d units %d (tech words set for factions %s)"
                  % (stored_target, source, selection, count, factions))
            if show_units:
                for index in range(count):
                    asset, faction, x, y, rotation = struct.unpack_from("<iiiiI", carry, records_at + index * 32)
                    print("    unit 0x%08X %d %d %d %u" % (asset & 0xFFFFFFFF, faction, x, y, rotation))
    if offset != len(main_data):
        print("warning: %d bytes behind the last section" % (len(main_data) - offset))


if __name__ == "__main__":
    main()
