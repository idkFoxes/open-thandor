/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/cmake/pe_not_large_address_aware.cpp
 */

/*
Build helper of the MinGW build (CMakeLists.txt, POST_BUILD of thandor): GNU ld always marks a 64-bit executable
IMAGE_FILE_LARGE_ADDRESS_AWARE (its --disable-large-address-aware exists for 32-bit images only). The game keeps
pointers in 32-bit fields (core/ptr32.h) and needs the process below 2 GB, like MSVC's /LARGEADDRESSAWARE:NO: this
clears the flag in the PE file header and checks that the image is not relocatable (no DYNAMIC_BASE or
HIGH_ENTROPY_VA). usage: pe_not_large_address_aware <exe>
*/

#include <cstdint>
#include <cstdio>

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::fprintf(stderr, "usage: pe_not_large_address_aware <exe>\n");
        return 2;
    }
    std::FILE *file = std::fopen(argv[1], "r+b");
    if (file == nullptr) {
        std::fprintf(stderr, "pe_not_large_address_aware: cannot open %s\n", argv[1]);
        return 1;
    }
    auto read = [file](long offset, void *out, size_t size) {
        return std::fseek(file, offset, SEEK_SET) == 0 && std::fread(out, 1, size, file) == size;
    };
    uint32_t peOffset = 0;
    uint8_t signature[4] = {};
    uint16_t characteristics = 0;
    uint16_t magic = 0;
    uint16_t dllCharacteristics = 0;
    /* e_lfanew; "PE\0\0"; IMAGE_FILE_HEADER.Characteristics at +22; the optional header (+24): Magic 0x20B (PE32+),
       DllCharacteristics at +70 */
    if (!read(0x3C, &peOffset, 4) || !read((long)peOffset, signature, 4) || signature[0] != 'P' ||
        signature[1] != 'E' || signature[2] != 0 || signature[3] != 0 ||
        !read((long)peOffset + 22, &characteristics, 2) || !read((long)peOffset + 24, &magic, 2) || magic != 0x20B ||
        !read((long)peOffset + 24 + 70, &dllCharacteristics, 2)) {
        std::fprintf(stderr, "pe_not_large_address_aware: %s is no PE32+ image\n", argv[1]);
        std::fclose(file);
        return 1;
    }
    const uint16_t largeAddressAware = 0x0020;   /* IMAGE_FILE_LARGE_ADDRESS_AWARE */
    const uint16_t relocatable = 0x0040 | 0x0020; /* IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE | HIGH_ENTROPY_VA */
    if ((characteristics & largeAddressAware) != 0) {
        characteristics = (uint16_t)(characteristics & ~largeAddressAware);
        if (std::fseek(file, (long)peOffset + 22, SEEK_SET) != 0 || std::fwrite(&characteristics, 1, 2, file) != 2) {
            std::fprintf(stderr, "pe_not_large_address_aware: cannot write %s\n", argv[1]);
            std::fclose(file);
            return 1;
        }
    }
    std::fclose(file);
    if ((dllCharacteristics & relocatable) != 0) {
        std::fprintf(stderr, "pe_not_large_address_aware: %s is relocatable (DllCharacteristics 0x%04X)\n", argv[1],
                     dllCharacteristics);
        return 1;
    }
    return 0;
}
