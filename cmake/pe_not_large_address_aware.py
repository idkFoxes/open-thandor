#!/usr/bin/env python3
"""Clear IMAGE_FILE_LARGE_ADDRESS_AWARE in a PE32+ image and reject ASLR flags.

Host-side equivalent of pe_not_large_address_aware.cpp for Linux -> Windows cross builds. The recovered game still
stores some pointers in 32-bit fields and therefore deliberately keeps the process below 2 GiB.
"""

from __future__ import annotations

import struct
import sys
from pathlib import Path

IMAGE_FILE_LARGE_ADDRESS_AWARE = 0x0020
IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA = 0x0020
IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE = 0x0040
PE32_PLUS_MAGIC = 0x020B


def fail(message: str) -> "NoReturn":
    print(f"pe_not_large_address_aware: {message}", file=sys.stderr)
    raise SystemExit(1)


def read_exact(file, offset: int, size: int) -> bytes:
    file.seek(offset)
    data = file.read(size)
    if len(data) != size:
        fail("truncated PE image")
    return data


def patch(path: Path) -> None:
    try:
        file = path.open("r+b")
    except OSError as exc:
        fail(f"cannot open {path}: {exc}")

    with file:
        if read_exact(file, 0, 2) != b"MZ":
            fail(f"{path} is no PE image")
        pe_offset = struct.unpack("<I", read_exact(file, 0x3C, 4))[0]
        if read_exact(file, pe_offset, 4) != b"PE\0\0":
            fail(f"{path} is no PE image")

        characteristics_offset = pe_offset + 22
        optional_header_offset = pe_offset + 24
        characteristics = struct.unpack("<H", read_exact(file, characteristics_offset, 2))[0]
        magic = struct.unpack("<H", read_exact(file, optional_header_offset, 2))[0]
        if magic != PE32_PLUS_MAGIC:
            fail(f"{path} is no PE32+ image")
        dll_characteristics = struct.unpack(
            "<H", read_exact(file, optional_header_offset + 70, 2)
        )[0]

        relocatable = IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE | IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA
        if dll_characteristics & relocatable:
            fail(f"{path} is relocatable (DllCharacteristics 0x{dll_characteristics:04X})")

        new_characteristics = characteristics & ~IMAGE_FILE_LARGE_ADDRESS_AWARE
        if new_characteristics != characteristics:
            file.seek(characteristics_offset)
            file.write(struct.pack("<H", new_characteristics))
            file.flush()


def main(argv: list[str]) -> int:
    if len(argv) != 2:
        print("usage: pe_not_large_address_aware.py <exe>", file=sys.stderr)
        return 2
    patch(Path(argv[1]))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
