/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/text/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/core/text/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(4)) WideNumberFormatState g_WideNumberFormatState = {
    .decimalSeparatorLength = 1,
    .groupSeparatorLength = 1,
    .positiveSignLength = 1,
    .negativeSignLength = 1,
    .hexPrefixLength = 2,
    .decimalSeparator = {44},
    .groupSeparator = {46},
    .positiveSign = {43},
    .negativeSign = {45},
    .hexPrefix = L"0x",
    .spacePadding = {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
    .zeroPadding = {48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48},
    .digitAlphabet = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70}};
