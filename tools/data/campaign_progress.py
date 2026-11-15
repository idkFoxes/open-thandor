"""Dumps the campaign progress file save\\kampagne.dat (src/gameplay/session/campaign_progress.cpp).

usage:
  python tools/data/campaign_progress.py <kampagne.dat> [--units]

The file is a PCK package (tools/data/pck.py lists and extracts it): progress.bin holds the sections, one
carry_<name>_<target>.hex per carry-over snapshot holds a small header and the oldunit.hex layout. Every payload
starts with tag, version, data size and the CRC32 of the data behind that 16-byte header; the dump checks them.
Layouts: include/thandor/gameplay/session/campaign_progress.h. --units lists the units of every snapshot.

As a module (the campaign map test, tools/test/run_campaign_map.py): read_progress(path) returns the sections as
dicts, encode_progress(sections) builds a file the game reads back (write_payload also makes broken payloads),
fingerprint(cgn bytes) is the campaign's fingerprint (CampaignAsset_Fingerprint)."""
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
PROGRESS_TAG = b"kpg\0"
CARRY_TAG = b"kco\0"
CGN_HEADER = 0x200
CGN_RECORD = 0x180


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


def write_payload(tag, data, version=1, crc=None):
    """A payload: tag, version, size, CRC32 of data (or the given crc), data, the zero pad of a size that is a
    multiple of 16."""
    size = HEADER + len(data)
    crc = zlib.crc32(data) if crc is None else crc
    return struct.pack("<4sIII", tag, version, size, crc) + data + (b"\0" * 4 if size % 16 == 0 else b"")


def read_progress(path):
    """The sections of a progress file: a list of dicts name, fingerprint ((crc32, records) or None), wins
    ({level: (selection mask, last selection)}), unlocked ([levels]), carries ({target: dict source, selection,
    units [(asset, faction, x, y, rotation)], tech [64 words], payload (the entry's bytes)})."""
    data, items = pck.entries(path)
    payloads = {entry[0]: pck.read_entry(data, entry) for entry in items}
    main_data = check_payload("progress.bin", payloads["progress.bin"], PROGRESS_TAG)
    offset = HEADER
    (section_count,) = struct.unpack_from("<I", main_data, offset)
    offset += 4
    sections = []
    for _ in range(section_count):
        name = main_data[offset:offset + NAME_BYTES].split(b"\0")[0].decode("ascii", "replace")
        offset += NAME_BYTES
        flags, crc, records, win_count = struct.unpack_from("<IIiI", main_data, offset)
        offset += 16
        section = {"name": name, "fingerprint": (crc, records) if flags & 1 else None, "wins": {},
                   "unlocked": [], "carries": {}}
        for _ in range(win_count):
            level, mask, last = struct.unpack_from("<iBB", main_data, offset)
            offset += 8
            section["wins"][level] = (mask, last)
        (unlocked_count,) = struct.unpack_from("<I", main_data, offset)
        section["unlocked"] = list(struct.unpack_from("<%di" % unlocked_count, main_data, offset + 4))
        offset += 4 + 4 * unlocked_count
        (carry_count,) = struct.unpack_from("<I", main_data, offset)
        targets = struct.unpack_from("<%di" % carry_count, main_data, offset + 4)
        offset += 4 + 4 * carry_count
        for target in targets:
            entry = "carry_%s_%d.hex" % (name.lower(), target)
            carry = check_payload(entry, payloads[entry], CARRY_TAG)
            stored_target, source, selection, _, count = struct.unpack_from("<iiiII", carry, HEADER)
            records_at = CARRY_HEADER + 4
            units = [struct.unpack_from("<iiiiI", carry, records_at + index * 32) for index in range(count)]
            tech = list(struct.unpack_from("<%dI" % TECH_WORDS, carry,
                                           records_at + UNIT_CAPACITY * RECORD_DWORDS * 4))
            section["carries"][target] = {"target": stored_target, "source": source, "selection": selection,
                                          "units": units, "tech": tech, "payload": payloads[entry]}
        sections.append(section)
    if offset != len(main_data):
        raise ValueError("%d bytes behind the last section" % (len(main_data) - offset))
    return sections


def encode_carry(target, carry):
    """The payload of a carry_<name>_<target>.hex entry."""
    data = struct.pack("<iiii", target, carry["source"], carry["selection"], 0)
    data += struct.pack("<I", len(carry["units"]))
    for index in range(UNIT_CAPACITY):
        unit = carry["units"][index] if index < len(carry["units"]) else (0, 0, 0, 0, 0)
        data += struct.pack("<iiiiI", *unit) + b"\0" * 12
    data += struct.pack("<%dI" % TECH_WORDS, *carry.get("tech", [0] * TECH_WORDS))
    return write_payload(CARRY_TAG, data)


def encode_sections(sections):
    """progress.bin's data (behind its header) for the sections, in the given order."""
    data = struct.pack("<I", len(sections))
    for section in sections:
        name = section["name"].encode("ascii")
        fingerprint = section.get("fingerprint")
        data += name + b"\0" * (NAME_BYTES - len(name))
        data += struct.pack("<IIiI", 1 if fingerprint else 0, fingerprint[0] if fingerprint else 0,
                            fingerprint[1] if fingerprint else 0, len(section.get("wins", {})))
        for level, (mask, last) in sorted(section.get("wins", {}).items()):
            data += struct.pack("<iBBxx", level, mask, last)
        unlocked = sorted(section.get("unlocked", []))
        data += struct.pack("<I%di" % len(unlocked), len(unlocked), *unlocked)
        targets = sorted(section.get("carries", {}))
        data += struct.pack("<I%di" % len(targets), len(targets), *targets)
    return data


def encode_progress(sections):
    """A progress file (bytes) the game reads: sections sorted by name, as the game writes them."""
    sections = sorted(sections, key=lambda section: section["name"])
    items = [("progress.bin", write_payload(PROGRESS_TAG, encode_sections(sections)))]
    for section in sections:
        for target, carry in sorted(section.get("carries", {}).items()):
            items.append(("carry_%s_%d.hex" % (section["name"].lower(), target), encode_carry(target, carry)))
    return pck.write_package(items)


def fingerprint(cgn):
    """(crc32, record count) of a .cgn image as CampaignAsset_Fingerprint computes it: per level record its id,
    successors, carry-over and skip masks and the UTF-16 level file name up to its terminator."""
    count = max(struct.unpack_from("<i", cgn, 0xB8)[0], 0)
    stream = bytearray()
    for index in range(count):
        base = CGN_HEADER + index * CGN_RECORD
        level_id, carry, skip = struct.unpack_from("<iII", cgn, base + 0x100)
        stream += struct.pack("<i8iII", level_id, *struct.unpack_from("<8i", cgn, base), carry, skip)
        name = cgn[base + 0x10C:base + CGN_RECORD]
        for unit in range(0, len(name) - 1, 2):
            if name[unit:unit + 2] == b"\0\0":
                break
            stream += name[unit:unit + 2]
        stream += b"\0\0"
    return zlib.crc32(stream), count


def main():
    path = sys.argv[1]
    show_units = "--units" in sys.argv[2:]
    _, items = pck.entries(path)
    sections = read_progress(path)
    print("%d sections, %d entries" % (len(sections), len(items)))
    for section in sections:
        print("[campaign %s]" % section["name"])
        if section["fingerprint"]:
            print("  fingerprint %08x %d" % section["fingerprint"])
        for level, (mask, last) in section["wins"].items():
            selections = [s for s in range(8) if mask >> s & 1]
            print("  done %d selections %s last %d" % (level, selections, last))
        if section["unlocked"]:
            print("  unlocked %s" % " ".join(str(level) for level in section["unlocked"]))
        for carry in section["carries"].values():
            factions = [f for f in range(8) if any(carry["tech"][f * 8:f * 8 + 8])]
            print("  carry %d from %d sel %d units %d (tech words set for factions %s)"
                  % (carry["target"], carry["source"], carry["selection"], len(carry["units"]), factions))
            if show_units:
                for asset, faction, x, y, rotation in carry["units"]:
                    print("    unit 0x%08X %d %d %d %u" % (asset & 0xFFFFFFFF, faction, x, y, rotation))


if __name__ == "__main__":
    main()
