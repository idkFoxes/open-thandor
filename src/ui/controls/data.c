/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/ui/controls/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 004027BC g_Utf16StringCompareAsciiCaseInsensitiveFlags */
__declspec(align(4)) pointer g_Utf16StringCompareAsciiCaseInsensitiveFlags = (void *)Utf16String_CompareAsciiCaseInsensitiveFlags;

/* 0040B214 g_FileSystemEnumerateDirectoryOrVolumeEntries */
__declspec(align(4)) FileSystemEnumerateDirectoryOrVolumeEntriesProc *g_FileSystemEnumerateDirectoryOrVolumeEntries = 0;

/* 0040B218 g_FileSystemValidateDos83Path */
__declspec(align(8)) FileSystemValidateDos83Proc *g_FileSystemValidateDos83Path = 0;

/* 0040F530 g_UiTimedListDriveLetters */
__declspec(align(16)) uint8_t g_UiTimedListDriveLetters[32] = {0};

/* 0040F550 g_UiTimedListRecordPathScratch */
__declspec(align(16)) WidePathBuffer256 g_UiTimedListRecordPathScratch = {0};

/* 0040F750 g_UiTimedListCombinedPathScratch */
__declspec(align(16)) WidePathBuffer256 g_UiTimedListCombinedPathScratch = {0};

/* 0040F950 g_UiTimedListSecondaryPathScratch */
__declspec(align(16)) WidePathBuffer256 g_UiTimedListSecondaryPathScratch = {0};

/* 0040FB50 g_UiTimedListHierarchyPathScratch */
__declspec(align(16)) WidePathBuffer256 g_UiTimedListHierarchyPathScratch = {0};

/* 0040FD50 g_UiTimedListHierarchyParentPathScratch */
__declspec(align(16)) WidePathBuffer256 g_UiTimedListHierarchyParentPathScratch = {0};

/* 0040FF50 g_WildcardAllFilesUtf16 */
__declspec(align(16)) uint16_t g_WildcardAllFilesUtf16[4] = L"*.*";

/* 0040FF58 g_UiTimedListDriveWildcardUtf16 */
__declspec(align(8)) uint16_t g_UiTimedListDriveWildcardUtf16[7] = L"?:\\*.*";

/* 00416818 g_CursorUseOverridePosition */
__declspec(align(8)) uint32_t g_CursorUseOverridePosition = 0;

/* 00416830 g_CursorWheelDelta */
__declspec(align(16)) UiPointerWheelDelta g_CursorWheelDelta = 0;

/* 00417348 g_SoundPlayOneShot */
__declspec(align(8)) SoundPlayVoiceProc *g_SoundPlayOneShot = (void *)SoundBackendDisabled_PlayOneShot;

/* 0041F720 g_UiScalerSecondPixelWeights */
__declspec(align(16)) SoftwareBgraWordLanes g_UiScalerSecondPixelWeights[256] = {
    /*   0 */ {0},
    /*   1 */ {0},
    /*   2 */ {0},
    /*   3 */ {0},
    /*   4 */ {0},
    /*   5 */ {0},
    /*   6 */ {0},
    /*   7 */ {0},
    /*   8 */ {0},
    /*   9 */ {0},
    /*  10 */ {0},
    /*  11 */ {0},
    /*  12 */ {0},
    /*  13 */ {0},
    /*  14 */ {0},
    /*  15 */ {0},
    /*  16 */ {0},
    /*  17 */ {0},
    /*  18 */ {0},
    /*  19 */ {0},
    /*  20 */ {0},
    /*  21 */ {0},
    /*  22 */ {0},
    /*  23 */ {0},
    /*  24 */ {0},
    /*  25 */ {0},
    /*  26 */ {0},
    /*  27 */ {0},
    /*  28 */ {0},
    /*  29 */ {0},
    /*  30 */ {0},
    /*  31 */ {0},
    /*  32 */ {0},
    /*  33 */ {0},
    /*  34 */ {0},
    /*  35 */ {0},
    /*  36 */ {0},
    /*  37 */ {0},
    /*  38 */ {0},
    /*  39 */ {0},
    /*  40 */ {0},
    /*  41 */ {0},
    /*  42 */ {0},
    /*  43 */ {0},
    /*  44 */ {0},
    /*  45 */ {0},
    /*  46 */ {0},
    /*  47 */ {0},
    /*  48 */ {0},
    /*  49 */ {0},
    /*  50 */ {0},
    /*  51 */ {0},
    /*  52 */ {0},
    /*  53 */ {0},
    /*  54 */ {0},
    /*  55 */ {0},
    /*  56 */ {0},
    /*  57 */ {0},
    /*  58 */ {0},
    /*  59 */ {0},
    /*  60 */ {0},
    /*  61 */ {0},
    /*  62 */ {0},
    /*  63 */ {0},
    /*  64 */ {0},
    /*  65 */ {.blue = 128, .green = 128, .red = 128, .alpha = 128},
    /*  66 */ {.blue = 257, .green = 257, .red = 257, .alpha = 257},
    /*  67 */ {.blue = 385, .green = 385, .red = 385, .alpha = 385},
    /*  68 */ {.blue = 514, .green = 514, .red = 514, .alpha = 514},
    /*  69 */ {.blue = 642, .green = 642, .red = 642, .alpha = 642},
    /*  70 */ {.blue = 771, .green = 771, .red = 771, .alpha = 771},
    /*  71 */ {.blue = 899, .green = 899, .red = 899, .alpha = 899},
    /*  72 */ {.blue = 1028, .green = 1028, .red = 1028, .alpha = 1028},
    /*  73 */ {.blue = 1156, .green = 1156, .red = 1156, .alpha = 1156},
    /*  74 */ {.blue = 1285, .green = 1285, .red = 1285, .alpha = 1285},
    /*  75 */ {.blue = 1413, .green = 1413, .red = 1413, .alpha = 1413},
    /*  76 */ {.blue = 1542, .green = 1542, .red = 1542, .alpha = 1542},
    /*  77 */ {.blue = 1670, .green = 1670, .red = 1670, .alpha = 1670},
    /*  78 */ {.blue = 1799, .green = 1799, .red = 1799, .alpha = 1799},
    /*  79 */ {.blue = 1927, .green = 1927, .red = 1927, .alpha = 1927},
    /*  80 */ {.blue = 2056, .green = 2056, .red = 2056, .alpha = 2056},
    /*  81 */ {.blue = 2184, .green = 2184, .red = 2184, .alpha = 2184},
    /*  82 */ {.blue = 2313, .green = 2313, .red = 2313, .alpha = 2313},
    /*  83 */ {.blue = 2441, .green = 2441, .red = 2441, .alpha = 2441},
    /*  84 */ {.blue = 2570, .green = 2570, .red = 2570, .alpha = 2570},
    /*  85 */ {.blue = 2698, .green = 2698, .red = 2698, .alpha = 2698},
    /*  86 */ {.blue = 2827, .green = 2827, .red = 2827, .alpha = 2827},
    /*  87 */ {.blue = 2955, .green = 2955, .red = 2955, .alpha = 2955},
    /*  88 */ {.blue = 3084, .green = 3084, .red = 3084, .alpha = 3084},
    /*  89 */ {.blue = 3212, .green = 3212, .red = 3212, .alpha = 3212},
    /*  90 */ {.blue = 3341, .green = 3341, .red = 3341, .alpha = 3341},
    /*  91 */ {.blue = 3469, .green = 3469, .red = 3469, .alpha = 3469},
    /*  92 */ {.blue = 3598, .green = 3598, .red = 3598, .alpha = 3598},
    /*  93 */ {.blue = 3726, .green = 3726, .red = 3726, .alpha = 3726},
    /*  94 */ {.blue = 3855, .green = 3855, .red = 3855, .alpha = 3855},
    /*  95 */ {.blue = 3983, .green = 3983, .red = 3983, .alpha = 3983},
    /*  96 */ {.blue = 4112, .green = 4112, .red = 4112, .alpha = 4112},
    /*  97 */ {.blue = 4240, .green = 4240, .red = 4240, .alpha = 4240},
    /*  98 */ {.blue = 4369, .green = 4369, .red = 4369, .alpha = 4369},
    /*  99 */ {.blue = 4497, .green = 4497, .red = 4497, .alpha = 4497},
    /* 100 */ {.blue = 4626, .green = 4626, .red = 4626, .alpha = 4626},
    /* 101 */ {.blue = 4754, .green = 4754, .red = 4754, .alpha = 4754},
    /* 102 */ {.blue = 4883, .green = 4883, .red = 4883, .alpha = 4883},
    /* 103 */ {.blue = 5011, .green = 5011, .red = 5011, .alpha = 5011},
    /* 104 */ {.blue = 5140, .green = 5140, .red = 5140, .alpha = 5140},
    /* 105 */ {.blue = 5268, .green = 5268, .red = 5268, .alpha = 5268},
    /* 106 */ {.blue = 5397, .green = 5397, .red = 5397, .alpha = 5397},
    /* 107 */ {.blue = 5525, .green = 5525, .red = 5525, .alpha = 5525},
    /* 108 */ {.blue = 5654, .green = 5654, .red = 5654, .alpha = 5654},
    /* 109 */ {.blue = 5782, .green = 5782, .red = 5782, .alpha = 5782},
    /* 110 */ {.blue = 5911, .green = 5911, .red = 5911, .alpha = 5911},
    /* 111 */ {.blue = 6039, .green = 6039, .red = 6039, .alpha = 6039},
    /* 112 */ {.blue = 6168, .green = 6168, .red = 6168, .alpha = 6168},
    /* 113 */ {.blue = 6296, .green = 6296, .red = 6296, .alpha = 6296},
    /* 114 */ {.blue = 6425, .green = 6425, .red = 6425, .alpha = 6425},
    /* 115 */ {.blue = 6553, .green = 6553, .red = 6553, .alpha = 6553},
    /* 116 */ {.blue = 6682, .green = 6682, .red = 6682, .alpha = 6682},
    /* 117 */ {.blue = 6810, .green = 6810, .red = 6810, .alpha = 6810},
    /* 118 */ {.blue = 6939, .green = 6939, .red = 6939, .alpha = 6939},
    /* 119 */ {.blue = 7067, .green = 7067, .red = 7067, .alpha = 7067},
    /* 120 */ {.blue = 7196, .green = 7196, .red = 7196, .alpha = 7196},
    /* 121 */ {.blue = 7324, .green = 7324, .red = 7324, .alpha = 7324},
    /* 122 */ {.blue = 7453, .green = 7453, .red = 7453, .alpha = 7453},
    /* 123 */ {.blue = 7581, .green = 7581, .red = 7581, .alpha = 7581},
    /* 124 */ {.blue = 7710, .green = 7710, .red = 7710, .alpha = 7710},
    /* 125 */ {.blue = 7838, .green = 7838, .red = 7838, .alpha = 7838},
    /* 126 */ {.blue = 7967, .green = 7967, .red = 7967, .alpha = 7967},
    /* 127 */ {.blue = 8095, .green = 8095, .red = 8095, .alpha = 8095},
    /* 128 */ {.blue = 8224, .green = 8224, .red = 8224, .alpha = 8224},
    /* 129 */ {.blue = 8352, .green = 8352, .red = 8352, .alpha = 8352},
    /* 130 */ {.blue = 8481, .green = 8481, .red = 8481, .alpha = 8481},
    /* 131 */ {.blue = 8609, .green = 8609, .red = 8609, .alpha = 8609},
    /* 132 */ {.blue = 8738, .green = 8738, .red = 8738, .alpha = 8738},
    /* 133 */ {.blue = 8866, .green = 8866, .red = 8866, .alpha = 8866},
    /* 134 */ {.blue = 8995, .green = 8995, .red = 8995, .alpha = 8995},
    /* 135 */ {.blue = 9123, .green = 9123, .red = 9123, .alpha = 9123},
    /* 136 */ {.blue = 9252, .green = 9252, .red = 9252, .alpha = 9252},
    /* 137 */ {.blue = 9380, .green = 9380, .red = 9380, .alpha = 9380},
    /* 138 */ {.blue = 9509, .green = 9509, .red = 9509, .alpha = 9509},
    /* 139 */ {.blue = 9637, .green = 9637, .red = 9637, .alpha = 9637},
    /* 140 */ {.blue = 9766, .green = 9766, .red = 9766, .alpha = 9766},
    /* 141 */ {.blue = 9894, .green = 9894, .red = 9894, .alpha = 9894},
    /* 142 */ {.blue = 10023, .green = 10023, .red = 10023, .alpha = 10023},
    /* 143 */ {.blue = 10151, .green = 10151, .red = 10151, .alpha = 10151},
    /* 144 */ {.blue = 10280, .green = 10280, .red = 10280, .alpha = 10280},
    /* 145 */ {.blue = 10408, .green = 10408, .red = 10408, .alpha = 10408},
    /* 146 */ {.blue = 10537, .green = 10537, .red = 10537, .alpha = 10537},
    /* 147 */ {.blue = 10665, .green = 10665, .red = 10665, .alpha = 10665},
    /* 148 */ {.blue = 10794, .green = 10794, .red = 10794, .alpha = 10794},
    /* 149 */ {.blue = 10922, .green = 10922, .red = 10922, .alpha = 10922},
    /* 150 */ {.blue = 11051, .green = 11051, .red = 11051, .alpha = 11051},
    /* 151 */ {.blue = 11179, .green = 11179, .red = 11179, .alpha = 11179},
    /* 152 */ {.blue = 11308, .green = 11308, .red = 11308, .alpha = 11308},
    /* 153 */ {.blue = 11436, .green = 11436, .red = 11436, .alpha = 11436},
    /* 154 */ {.blue = 11565, .green = 11565, .red = 11565, .alpha = 11565},
    /* 155 */ {.blue = 11693, .green = 11693, .red = 11693, .alpha = 11693},
    /* 156 */ {.blue = 11822, .green = 11822, .red = 11822, .alpha = 11822},
    /* 157 */ {.blue = 11950, .green = 11950, .red = 11950, .alpha = 11950},
    /* 158 */ {.blue = 12079, .green = 12079, .red = 12079, .alpha = 12079},
    /* 159 */ {.blue = 12207, .green = 12207, .red = 12207, .alpha = 12207},
    /* 160 */ {.blue = 12336, .green = 12336, .red = 12336, .alpha = 12336},
    /* 161 */ {.blue = 12464, .green = 12464, .red = 12464, .alpha = 12464},
    /* 162 */ {.blue = 12593, .green = 12593, .red = 12593, .alpha = 12593},
    /* 163 */ {.blue = 12721, .green = 12721, .red = 12721, .alpha = 12721},
    /* 164 */ {.blue = 12850, .green = 12850, .red = 12850, .alpha = 12850},
    /* 165 */ {.blue = 12978, .green = 12978, .red = 12978, .alpha = 12978},
    /* 166 */ {.blue = 13107, .green = 13107, .red = 13107, .alpha = 13107},
    /* 167 */ {.blue = 13235, .green = 13235, .red = 13235, .alpha = 13235},
    /* 168 */ {.blue = 13364, .green = 13364, .red = 13364, .alpha = 13364},
    /* 169 */ {.blue = 13492, .green = 13492, .red = 13492, .alpha = 13492},
    /* 170 */ {.blue = 13621, .green = 13621, .red = 13621, .alpha = 13621},
    /* 171 */ {.blue = 13749, .green = 13749, .red = 13749, .alpha = 13749},
    /* 172 */ {.blue = 13878, .green = 13878, .red = 13878, .alpha = 13878},
    /* 173 */ {.blue = 14006, .green = 14006, .red = 14006, .alpha = 14006},
    /* 174 */ {.blue = 14135, .green = 14135, .red = 14135, .alpha = 14135},
    /* 175 */ {.blue = 14263, .green = 14263, .red = 14263, .alpha = 14263},
    /* 176 */ {.blue = 14392, .green = 14392, .red = 14392, .alpha = 14392},
    /* 177 */ {.blue = 14520, .green = 14520, .red = 14520, .alpha = 14520},
    /* 178 */ {.blue = 14649, .green = 14649, .red = 14649, .alpha = 14649},
    /* 179 */ {.blue = 14777, .green = 14777, .red = 14777, .alpha = 14777},
    /* 180 */ {.blue = 14906, .green = 14906, .red = 14906, .alpha = 14906},
    /* 181 */ {.blue = 15034, .green = 15034, .red = 15034, .alpha = 15034},
    /* 182 */ {.blue = 15163, .green = 15163, .red = 15163, .alpha = 15163},
    /* 183 */ {.blue = 15291, .green = 15291, .red = 15291, .alpha = 15291},
    /* 184 */ {.blue = 15420, .green = 15420, .red = 15420, .alpha = 15420},
    /* 185 */ {.blue = 15548, .green = 15548, .red = 15548, .alpha = 15548},
    /* 186 */ {.blue = 15677, .green = 15677, .red = 15677, .alpha = 15677},
    /* 187 */ {.blue = 15805, .green = 15805, .red = 15805, .alpha = 15805},
    /* 188 */ {.blue = 15934, .green = 15934, .red = 15934, .alpha = 15934},
    /* 189 */ {.blue = 16062, .green = 16062, .red = 16062, .alpha = 16062},
    /* 190 */ {.blue = 16191, .green = 16191, .red = 16191, .alpha = 16191},
    /* 191 */ {.blue = 16319, .green = 16319, .red = 16319, .alpha = 16319},
    /* 192 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 193 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 194 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 195 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 196 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 197 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 198 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 199 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 200 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 201 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 202 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 203 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 204 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 205 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 206 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 207 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 208 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 209 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 210 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 211 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 212 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 213 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 214 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 215 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 216 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 217 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 218 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 219 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 220 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 221 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 222 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 223 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 224 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 225 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 226 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 227 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 228 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 229 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 230 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 231 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 232 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 233 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 234 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 235 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 236 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 237 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 238 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 239 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 240 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 241 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 242 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 243 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 244 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 245 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 246 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 247 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 248 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 249 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 250 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 251 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 252 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 253 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 254 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /* 255 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384}};

/* 00420720 g_UiScalerFirstPixelWeights */
__declspec(align(16)) SoftwareBgraWordLanes g_UiScalerFirstPixelWeights[256] = {
    /*   0 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*   1 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*   2 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*   3 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*   4 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*   5 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*   6 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*   7 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*   8 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*   9 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  10 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  11 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  12 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  13 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  14 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  15 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  16 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  17 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  18 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  19 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  20 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  21 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  22 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  23 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  24 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  25 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  26 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  27 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  28 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  29 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  30 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  31 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  32 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  33 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  34 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  35 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  36 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  37 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  38 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  39 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  40 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  41 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  42 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  43 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  44 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  45 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  46 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  47 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  48 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  49 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  50 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  51 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  52 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  53 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  54 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  55 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  56 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  57 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  58 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  59 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  60 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  61 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  62 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  63 */ {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384},
    /*  64 */ {.blue = 16448, .green = 16448, .red = 16448, .alpha = 16448},
    /*  65 */ {.blue = 16319, .green = 16319, .red = 16319, .alpha = 16319},
    /*  66 */ {.blue = 16191, .green = 16191, .red = 16191, .alpha = 16191},
    /*  67 */ {.blue = 16062, .green = 16062, .red = 16062, .alpha = 16062},
    /*  68 */ {.blue = 15934, .green = 15934, .red = 15934, .alpha = 15934},
    /*  69 */ {.blue = 15805, .green = 15805, .red = 15805, .alpha = 15805},
    /*  70 */ {.blue = 15677, .green = 15677, .red = 15677, .alpha = 15677},
    /*  71 */ {.blue = 15548, .green = 15548, .red = 15548, .alpha = 15548},
    /*  72 */ {.blue = 15420, .green = 15420, .red = 15420, .alpha = 15420},
    /*  73 */ {.blue = 15291, .green = 15291, .red = 15291, .alpha = 15291},
    /*  74 */ {.blue = 15163, .green = 15163, .red = 15163, .alpha = 15163},
    /*  75 */ {.blue = 15034, .green = 15034, .red = 15034, .alpha = 15034},
    /*  76 */ {.blue = 14906, .green = 14906, .red = 14906, .alpha = 14906},
    /*  77 */ {.blue = 14777, .green = 14777, .red = 14777, .alpha = 14777},
    /*  78 */ {.blue = 14649, .green = 14649, .red = 14649, .alpha = 14649},
    /*  79 */ {.blue = 14520, .green = 14520, .red = 14520, .alpha = 14520},
    /*  80 */ {.blue = 14392, .green = 14392, .red = 14392, .alpha = 14392},
    /*  81 */ {.blue = 14263, .green = 14263, .red = 14263, .alpha = 14263},
    /*  82 */ {.blue = 14135, .green = 14135, .red = 14135, .alpha = 14135},
    /*  83 */ {.blue = 14006, .green = 14006, .red = 14006, .alpha = 14006},
    /*  84 */ {.blue = 13878, .green = 13878, .red = 13878, .alpha = 13878},
    /*  85 */ {.blue = 13749, .green = 13749, .red = 13749, .alpha = 13749},
    /*  86 */ {.blue = 13621, .green = 13621, .red = 13621, .alpha = 13621},
    /*  87 */ {.blue = 13492, .green = 13492, .red = 13492, .alpha = 13492},
    /*  88 */ {.blue = 13364, .green = 13364, .red = 13364, .alpha = 13364},
    /*  89 */ {.blue = 13235, .green = 13235, .red = 13235, .alpha = 13235},
    /*  90 */ {.blue = 13107, .green = 13107, .red = 13107, .alpha = 13107},
    /*  91 */ {.blue = 12978, .green = 12978, .red = 12978, .alpha = 12978},
    /*  92 */ {.blue = 12850, .green = 12850, .red = 12850, .alpha = 12850},
    /*  93 */ {.blue = 12721, .green = 12721, .red = 12721, .alpha = 12721},
    /*  94 */ {.blue = 12593, .green = 12593, .red = 12593, .alpha = 12593},
    /*  95 */ {.blue = 12464, .green = 12464, .red = 12464, .alpha = 12464},
    /*  96 */ {.blue = 12336, .green = 12336, .red = 12336, .alpha = 12336},
    /*  97 */ {.blue = 12207, .green = 12207, .red = 12207, .alpha = 12207},
    /*  98 */ {.blue = 12079, .green = 12079, .red = 12079, .alpha = 12079},
    /*  99 */ {.blue = 11950, .green = 11950, .red = 11950, .alpha = 11950},
    /* 100 */ {.blue = 11822, .green = 11822, .red = 11822, .alpha = 11822},
    /* 101 */ {.blue = 11693, .green = 11693, .red = 11693, .alpha = 11693},
    /* 102 */ {.blue = 11565, .green = 11565, .red = 11565, .alpha = 11565},
    /* 103 */ {.blue = 11436, .green = 11436, .red = 11436, .alpha = 11436},
    /* 104 */ {.blue = 11308, .green = 11308, .red = 11308, .alpha = 11308},
    /* 105 */ {.blue = 11179, .green = 11179, .red = 11179, .alpha = 11179},
    /* 106 */ {.blue = 11051, .green = 11051, .red = 11051, .alpha = 11051},
    /* 107 */ {.blue = 10922, .green = 10922, .red = 10922, .alpha = 10922},
    /* 108 */ {.blue = 10794, .green = 10794, .red = 10794, .alpha = 10794},
    /* 109 */ {.blue = 10665, .green = 10665, .red = 10665, .alpha = 10665},
    /* 110 */ {.blue = 10537, .green = 10537, .red = 10537, .alpha = 10537},
    /* 111 */ {.blue = 10408, .green = 10408, .red = 10408, .alpha = 10408},
    /* 112 */ {.blue = 10280, .green = 10280, .red = 10280, .alpha = 10280},
    /* 113 */ {.blue = 10151, .green = 10151, .red = 10151, .alpha = 10151},
    /* 114 */ {.blue = 10023, .green = 10023, .red = 10023, .alpha = 10023},
    /* 115 */ {.blue = 9894, .green = 9894, .red = 9894, .alpha = 9894},
    /* 116 */ {.blue = 9766, .green = 9766, .red = 9766, .alpha = 9766},
    /* 117 */ {.blue = 9637, .green = 9637, .red = 9637, .alpha = 9637},
    /* 118 */ {.blue = 9509, .green = 9509, .red = 9509, .alpha = 9509},
    /* 119 */ {.blue = 9380, .green = 9380, .red = 9380, .alpha = 9380},
    /* 120 */ {.blue = 9252, .green = 9252, .red = 9252, .alpha = 9252},
    /* 121 */ {.blue = 9123, .green = 9123, .red = 9123, .alpha = 9123},
    /* 122 */ {.blue = 8995, .green = 8995, .red = 8995, .alpha = 8995},
    /* 123 */ {.blue = 8866, .green = 8866, .red = 8866, .alpha = 8866},
    /* 124 */ {.blue = 8738, .green = 8738, .red = 8738, .alpha = 8738},
    /* 125 */ {.blue = 8609, .green = 8609, .red = 8609, .alpha = 8609},
    /* 126 */ {.blue = 8481, .green = 8481, .red = 8481, .alpha = 8481},
    /* 127 */ {.blue = 8352, .green = 8352, .red = 8352, .alpha = 8352},
    /* 128 */ {.blue = 8224, .green = 8224, .red = 8224, .alpha = 8224},
    /* 129 */ {.blue = 8095, .green = 8095, .red = 8095, .alpha = 8095},
    /* 130 */ {.blue = 7967, .green = 7967, .red = 7967, .alpha = 7967},
    /* 131 */ {.blue = 7838, .green = 7838, .red = 7838, .alpha = 7838},
    /* 132 */ {.blue = 7710, .green = 7710, .red = 7710, .alpha = 7710},
    /* 133 */ {.blue = 7581, .green = 7581, .red = 7581, .alpha = 7581},
    /* 134 */ {.blue = 7453, .green = 7453, .red = 7453, .alpha = 7453},
    /* 135 */ {.blue = 7324, .green = 7324, .red = 7324, .alpha = 7324},
    /* 136 */ {.blue = 7196, .green = 7196, .red = 7196, .alpha = 7196},
    /* 137 */ {.blue = 7067, .green = 7067, .red = 7067, .alpha = 7067},
    /* 138 */ {.blue = 6939, .green = 6939, .red = 6939, .alpha = 6939},
    /* 139 */ {.blue = 6810, .green = 6810, .red = 6810, .alpha = 6810},
    /* 140 */ {.blue = 6682, .green = 6682, .red = 6682, .alpha = 6682},
    /* 141 */ {.blue = 6553, .green = 6553, .red = 6553, .alpha = 6553},
    /* 142 */ {.blue = 6425, .green = 6425, .red = 6425, .alpha = 6425},
    /* 143 */ {.blue = 6296, .green = 6296, .red = 6296, .alpha = 6296},
    /* 144 */ {.blue = 6168, .green = 6168, .red = 6168, .alpha = 6168},
    /* 145 */ {.blue = 6039, .green = 6039, .red = 6039, .alpha = 6039},
    /* 146 */ {.blue = 5911, .green = 5911, .red = 5911, .alpha = 5911},
    /* 147 */ {.blue = 5782, .green = 5782, .red = 5782, .alpha = 5782},
    /* 148 */ {.blue = 5654, .green = 5654, .red = 5654, .alpha = 5654},
    /* 149 */ {.blue = 5525, .green = 5525, .red = 5525, .alpha = 5525},
    /* 150 */ {.blue = 5397, .green = 5397, .red = 5397, .alpha = 5397},
    /* 151 */ {.blue = 5268, .green = 5268, .red = 5268, .alpha = 5268},
    /* 152 */ {.blue = 5140, .green = 5140, .red = 5140, .alpha = 5140},
    /* 153 */ {.blue = 5011, .green = 5011, .red = 5011, .alpha = 5011},
    /* 154 */ {.blue = 4883, .green = 4883, .red = 4883, .alpha = 4883},
    /* 155 */ {.blue = 4754, .green = 4754, .red = 4754, .alpha = 4754},
    /* 156 */ {.blue = 4626, .green = 4626, .red = 4626, .alpha = 4626},
    /* 157 */ {.blue = 4497, .green = 4497, .red = 4497, .alpha = 4497},
    /* 158 */ {.blue = 4369, .green = 4369, .red = 4369, .alpha = 4369},
    /* 159 */ {.blue = 4240, .green = 4240, .red = 4240, .alpha = 4240},
    /* 160 */ {.blue = 4112, .green = 4112, .red = 4112, .alpha = 4112},
    /* 161 */ {.blue = 3983, .green = 3983, .red = 3983, .alpha = 3983},
    /* 162 */ {.blue = 3855, .green = 3855, .red = 3855, .alpha = 3855},
    /* 163 */ {.blue = 3726, .green = 3726, .red = 3726, .alpha = 3726},
    /* 164 */ {.blue = 3598, .green = 3598, .red = 3598, .alpha = 3598},
    /* 165 */ {.blue = 3469, .green = 3469, .red = 3469, .alpha = 3469},
    /* 166 */ {.blue = 3341, .green = 3341, .red = 3341, .alpha = 3341},
    /* 167 */ {.blue = 3212, .green = 3212, .red = 3212, .alpha = 3212},
    /* 168 */ {.blue = 3084, .green = 3084, .red = 3084, .alpha = 3084},
    /* 169 */ {.blue = 2955, .green = 2955, .red = 2955, .alpha = 2955},
    /* 170 */ {.blue = 2827, .green = 2827, .red = 2827, .alpha = 2827},
    /* 171 */ {.blue = 2698, .green = 2698, .red = 2698, .alpha = 2698},
    /* 172 */ {.blue = 2570, .green = 2570, .red = 2570, .alpha = 2570},
    /* 173 */ {.blue = 2441, .green = 2441, .red = 2441, .alpha = 2441},
    /* 174 */ {.blue = 2313, .green = 2313, .red = 2313, .alpha = 2313},
    /* 175 */ {.blue = 2184, .green = 2184, .red = 2184, .alpha = 2184},
    /* 176 */ {.blue = 2056, .green = 2056, .red = 2056, .alpha = 2056},
    /* 177 */ {.blue = 1927, .green = 1927, .red = 1927, .alpha = 1927},
    /* 178 */ {.blue = 1799, .green = 1799, .red = 1799, .alpha = 1799},
    /* 179 */ {.blue = 1670, .green = 1670, .red = 1670, .alpha = 1670},
    /* 180 */ {.blue = 1542, .green = 1542, .red = 1542, .alpha = 1542},
    /* 181 */ {.blue = 1413, .green = 1413, .red = 1413, .alpha = 1413},
    /* 182 */ {.blue = 1285, .green = 1285, .red = 1285, .alpha = 1285},
    /* 183 */ {.blue = 1156, .green = 1156, .red = 1156, .alpha = 1156},
    /* 184 */ {.blue = 1028, .green = 1028, .red = 1028, .alpha = 1028},
    /* 185 */ {.blue = 899, .green = 899, .red = 899, .alpha = 899},
    /* 186 */ {.blue = 771, .green = 771, .red = 771, .alpha = 771},
    /* 187 */ {.blue = 642, .green = 642, .red = 642, .alpha = 642},
    /* 188 */ {.blue = 514, .green = 514, .red = 514, .alpha = 514},
    /* 189 */ {.blue = 385, .green = 385, .red = 385, .alpha = 385},
    /* 190 */ {.blue = 257, .green = 257, .red = 257, .alpha = 257},
    /* 191 */ {.blue = 128, .green = 128, .red = 128, .alpha = 128}};

/* 00422720 g_UiGraphicsAdapterTextButtonVtable */
__declspec(align(16)) UiNodeVtable g_UiGraphicsAdapterTextButtonVtable = {
        .relocate = (void *)UiTextButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiGraphicsAdapterTextButton_DrawFormattedAdapterText,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiTextButtonControl_NonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiTextButtonControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 00422768 g_GraphicsAdapterFormatScratch0Utf16 */
__declspec(align(8)) uint16_t g_GraphicsAdapterFormatScratch0Utf16[16] = {0};

/* 00422788 g_GraphicsAdapterFormatScratch1Utf16 (followed by 8 bytes of 0x90 alignment padding, dropped) */
__declspec(align(8)) uint16_t g_GraphicsAdapterFormatScratch1Utf16[16] = {0};

/* 004229B4 g_UiDisplaySettingsRootTemplate */
__declspec(align(4)) DisplaySettingsUiImage g_UiDisplaySettingsRootTemplate = {
        { /* +0000 displaySettingsWindow g_UiResizableWindowControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x78), .parent = UI_TEMPLATE_NO_LINK,
            .vtable = (void *)&g_UiResizableWindowControlVtable,
            .leftOffset = -216, .topOffset = -144, .rightOffset = 216, .bottomOffset = 144,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x21},
        {
            0x00000007, 0x00000000, 0x00000000, 0x00000108},
        { /* +0078 cancelButton g_UiNodeVtable_004B1D80 */
            .nextSibling = UI_TEMPLATE_LINK(0xD4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiNodeVtable_004B1D80,
            .leftOffset = 16, .topOffset = 232, .rightOffset = 112, .bottomOffset = 256,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x2},
        {
            0x00000008, 0x0000020E, 0x00000101},
        { /* +00D4 applyButton g_UiNodeVtable_004B1D80 */
            .nextSibling = UI_TEMPLATE_LINK(0x160), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiNodeVtable_004B1D80,
            .leftOffset = 128, .topOffset = 232, .rightOffset = 240, .bottomOffset = 256,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000004, 0x00000200, 0x00000100},
        { /* +0160 resolutionHeading g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x1BC), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiFocusProxyControlVtable,
            .leftOffset = 160, .topOffset = 8, .rightOffset = 248, .bottomOffset = 28,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x0000010C},
        { /* +01BC colorDepthHeading g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x218), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiFocusProxyControlVtable,
            .leftOffset = 24, .topOffset = 8, .rightOffset = 120, .bottomOffset = 28,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x0000010D},
        { /* +0218 adapterHeading g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x280), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiFocusProxyControlVtable,
            .leftOffset = 24, .topOffset = 112, .rightOffset = 120, .bottomOffset = 132,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000000, 0x00000000, 0x0000010F},
        {0},
        { /* +0280 colorDepthOption1 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x2E8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 16, .topOffset = 28, .rightOffset = 120, .bottomOffset = 48,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000480, 0x00000201, 0x00000106},
        {0},
        { /* +02E8 colorDepthOption2 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x350), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 16, .topOffset = 48, .rightOffset = 120, .bottomOffset = 68,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000480, 0x00000202, 0x00000106},
        {0},
        { /* +0350 colorDepthOption3 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x3B8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 16, .topOffset = 68, .rightOffset = 120, .bottomOffset = 88,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000480, 0x00000203, 0x00000106},
        {0},
        { /* +03B8 colorDepthOption4 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x420), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 16, .topOffset = 88, .rightOffset = 120, .bottomOffset = 108,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000480, 0x00000204, 0x00000106},
        {0},
        { /* +0420 resolutionOption1 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x488), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 152, .topOffset = 28, .rightOffset = 248, .bottomOffset = 48,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000400, 0x00000205, 0x00000107},
        {0},
        { /* +0488 resolutionOption2 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x4F0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 152, .topOffset = 48, .rightOffset = 248, .bottomOffset = 68,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000400, 0x00000206, 0x00000107},
        {0},
        { /* +04F0 resolutionOption3 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x558), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 152, .topOffset = 68, .rightOffset = 248, .bottomOffset = 88,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000400, 0x00000207, 0x00000107},
        {0},
        { /* +0558 resolutionOption4 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x5C0), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 152, .topOffset = 88, .rightOffset = 248, .bottomOffset = 108,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000400, 0x00000208, 0x00000107},
        {0},
        { /* +05C0 resolutionOption5 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x628), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 152, .topOffset = 108, .rightOffset = 248, .bottomOffset = 128,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000400, 0x00000209, 0x00000107},
        {0},
        { /* +0628 resolutionOption6 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x690), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 152, .topOffset = 128, .rightOffset = 248, .bottomOffset = 148,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000400, 0x0000020A, 0x00000107},
        {0},
        { /* +0690 resolutionOption7 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x6F8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 152, .topOffset = 148, .rightOffset = 248, .bottomOffset = 168,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000400, 0x0000020B, 0x00000107},
        {0},
        { /* +06F8 resolutionOption8 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x760), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 152, .topOffset = 168, .rightOffset = 248, .bottomOffset = 188,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000400, 0x0000020C, 0x00000107},
        {0},
        { /* +0760 adapterOption1 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x7C8), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 16, .topOffset = 132, .rightOffset = 144, .bottomOffset = 152,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000C00, 0x0000020F, 0x00000110},
        {0},
        { /* +07C8 adapterOption2 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x830), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 16, .topOffset = 152, .rightOffset = 144, .bottomOffset = 172,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000C00, 0x00000210, 0x00000110},
        {0},
        { /* +0830 adapterOption3 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x898), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 16, .topOffset = 172, .rightOffset = 144, .bottomOffset = 192,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000C00, 0x00000211, 0x00000110},
        {0},
        { /* +0898 adapterOption4 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x900), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 16, .topOffset = 192, .rightOffset = 144, .bottomOffset = 212,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000C00, 0x00000212, 0x00000110},
        {0},
        { /* +0900 adapterOption5 g_UiGraphicsAdapterTextButtonVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x95C), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiGraphicsAdapterTextButtonVtable,
            .leftOffset = 16, .topOffset = 212, .rightOffset = 144, .bottomOffset = 232,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x28},
        {
            0x00000C00, 0x00000213, 0x00000110},
        { /* +095C colorScaleSliderFrame g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xA1C), .firstChild = UI_TEMPLATE_LINK(0x9B8), .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiFocusProxyControlVtable,
            .leftOffset = 256, .topOffset = 8, .rightOffset = 336, .bottomOffset = 208,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000001, 0x000009B8, 0x0000010A},
        { /* +09B8 colorScaleSlider g_UiRangeSliderControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x95C),
            .vtable = (void *)&g_UiRangeSliderControlVtable,
            .leftOffset = -8, .topOffset = 24, .rightOffset = 8,
            .leftAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000001, 0x00008000, 0x00020000, 0x00000000, 0x00000800, 0xFFFFFFFF},
        { /* +0A1C colorBiasSliderFrame g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xADC), .firstChild = UI_TEMPLATE_LINK(0xA78), .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiFocusProxyControlVtable,
            .leftOffset = 336, .topOffset = 8, .rightOffset = 416, .bottomOffset = 208,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000001, 0x00000A78, 0x0000010B},
        { /* +0A78 colorBiasSlider g_UiRangeSliderControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0xA1C),
            .vtable = (void *)&g_UiRangeSliderControlVtable,
            .leftOffset = -8, .topOffset = 24, .rightOffset = 8,
            .leftAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000001, 0xFFC00000, 0x00400000, 0x00000000, 0x00020000, 0xFFFFFFFF},
        { /* +0ADC colorScaleValueText g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB38), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiFocusProxyControlVtable,
            .leftOffset = 256, .topOffset = 208, .rightOffset = 336, .bottomOffset = 228,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000011},
        { /* +0B38 colorBiasValueText g_UiFocusProxyControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiFocusProxyControlVtable,
            .leftOffset = 336, .topOffset = 208, .rightOffset = 416, .bottomOffset = 228,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000011},
};

/* 00423588 g_UiDisplayModeSelectionActionHandlers20 */
__declspec(align(8)) UiDisplayModeSelectionActionHandlerTable g_UiDisplayModeSelectionActionHandlers20 = {
    .handlers = {
        /*  0 */ (void *)UiDisplayModeAction_ApplyPendingMode,
        /*  1 */ (void *)UiDisplayModeAction_UpdateColorDepthSelection,
        /*  2 */ (void *)UiDisplayModeAction_UpdateColorDepthSelection,
        /*  3 */ (void *)UiDisplayModeAction_UpdateColorDepthSelection,
        /*  4 */ (void *)UiDisplayModeAction_UpdateColorDepthSelection,
        /*  5 */ (void *)UiDisplayModeAction_UpdateResolutionSelection,
        /*  6 */ (void *)UiDisplayModeAction_UpdateResolutionSelection,
        /*  7 */ (void *)UiDisplayModeAction_UpdateResolutionSelection,
        /*  8 */ (void *)UiDisplayModeAction_UpdateResolutionSelection,
        /*  9 */ (void *)UiDisplayModeAction_UpdateResolutionSelection,
        /* 10 */ (void *)UiDisplayModeAction_UpdateResolutionSelection,
        /* 11 */ (void *)UiDisplayModeAction_UpdateResolutionSelection,
        /* 12 */ (void *)UiDisplayModeAction_UpdateResolutionSelection,
        /* 13 */ (void *)UiDisplayModeAction_RevertAndReopenSettings,
        /* 14 */ (void *)UiDisplayModeAction_CancelAndRebuildPixelPacking,
        /* 15 */ (void *)UiDisplayModeAction_UpdateAdapterSelection,
        /* 16 */ (void *)UiDisplayModeAction_UpdateAdapterSelection,
        /* 17 */ (void *)UiDisplayModeAction_UpdateAdapterSelection,
        /* 18 */ (void *)UiDisplayModeAction_UpdateAdapterSelection,
        /* 19 */ (void *)UiDisplayModeAction_UpdateAdapterSelection
    }};

/* 004235D8 g_UiDisplayModeDistinctValueScratch: sorted distinct-value slots of the display settings dialog */
__declspec(align(8)) DisplayModeScratchWord g_UiDisplayModeDistinctValueScratch[8] = {0};

/* 004A8E70 g_FramebufferAccess */
__declspec(align(16)) SoftwareFramebufferAccess *g_FramebufferAccess = 0;

/* 004A8E84 g_FramebufferRowStrideBytes */
__declspec(align(4)) uint32_t g_FramebufferRowStrideBytes = 0;

/* 004A8E8C g_FramebufferHeight */
__declspec(align(4)) uint32_t g_FramebufferHeight = 0;

/* 004A8EE0 g_GraphicsFramebufferPresent */
__declspec(align(16)) GraphicsFramebufferPresentProc *g_GraphicsFramebufferPresent = 0;

/* 004A8EEC g_GraphicsFramebufferBeginAccess */
__declspec(align(4)) GraphicsFramebufferBeginAccessProc *g_GraphicsFramebufferBeginAccess = (void *)GraphicsFramebuffer_BeginAccessStub;

/* 004A8EF0 g_GraphicsFramebufferEndAccess */
__declspec(align(16)) GraphicsFramebufferEndAccessProc *g_GraphicsFramebufferEndAccess = (void *)GraphicsFramebuffer_EndAccessStub;

/* 004A8EF4 g_GraphicsTextureSourceGetLogicalSize */
__declspec(align(4)) GraphicsTextureSourceGetLogicalSizeProc *g_GraphicsTextureSourceGetLogicalSize = (void *)GraphicsTextureSource_GetLogicalSize;

/* 004A8EF8 g_GraphicsTextureSourceTestOpaquePixel */
__declspec(align(8)) GraphicsTextureSourceTestOpaquePixelProc *g_GraphicsTextureSourceTestOpaquePixel = (void *)GraphicsTextureSource_TestOpaquePixel;

/* 004A8EFC g_GraphicsTextureSourceBlitSourceAlpha */
__declspec(align(4)) GraphicsTextureSourceBlitProc *g_GraphicsTextureSourceBlitSourceAlpha = 0;

/* 004A8F00 g_GraphicsTextureSourceBlitTiledSourceAlpha */
__declspec(align(16)) GraphicsTextureSourceTiledBlitProc *g_GraphicsTextureSourceBlitTiledSourceAlpha = (void *)GraphicsTextureSource_BlitTiledSourceAlpha;

/* 004A8F18 g_GraphicsTextureSourceBlitModulatedSourceAlpha */
__declspec(align(8)) GraphicsTextureSourceBlitModulatedSourceAlphaProc *g_GraphicsTextureSourceBlitModulatedSourceAlpha = 0;

/* 004A8F30 g_GraphicsFramebufferFillRectArgb */
__declspec(align(16)) GraphicsFramebufferFillRectArgbProc *g_GraphicsFramebufferFillRectArgb = 0;

/* 004AE984 g_UiRuntimeFrameLock */
__declspec(align(4)) RuntimeSpinLockValue *g_UiRuntimeFrameLock = 0;

/* 004AE988 g_UiRuntimePostUnlockCallback */
__declspec(align(8)) UiRuntimePostUnlockCallbackProc *g_UiRuntimePostUnlockCallback = 0;

/* 004AF1E0 g_UiTooltipState */
__declspec(align(16)) UiTooltipState g_UiTooltipState = {.countdownFrames = 8};

/* 004AF1F0 g_UiPendingFrameTicks */
__declspec(align(16)) uint32_t g_UiPendingFrameTicks = 0;

/* 004AF208 g_UiInvalidationSuppressed */
__declspec(align(8)) uint32_t g_UiInvalidationSuppressed = 0;

/* 004B0E30 g_UiRootNode */
__declspec(align(16)) UiRootNode *g_UiRootNode = (void *)0xFFFFFFFF;

/* 004B0E34 g_UiWindowTextureSource */
__declspec(align(4)) GraphicsTextureSourceAsset *g_UiWindowTextureSource = (void *)0xFFFFFFFF;

/* 004B0E38 g_UiWindowClassTextureSource */
__declspec(align(8)) GraphicsTextureSourceAsset *g_UiWindowClassTextureSource = 0;

/* 004B0E3C g_UiPointerCaptureTarget */
__declspec(align(4)) UiNodeBase *g_UiPointerCaptureTarget = (void *)0xFFFFFFFF;

/* 004B0E40 g_UiKeyboardFocusNode */
__declspec(align(16)) UiNodeBase *g_UiKeyboardFocusNode = (void *)0xFFFFFFFF;

/* 004B0E44 g_UiSoundGainQ15 */
__declspec(align(4)) AudioMixerGainQ15 g_UiSoundGainQ15 = 32768;

/* 004B0E48 g_UiTooltipDelayFrames */
__declspec(align(8)) UiFrameDelayFrames g_UiTooltipDelayFrames = 12;

/* 004B0E4C g_UiTooltipTextStyle */
__declspec(align(4)) uint32_t g_UiTooltipTextStyle = 0;

/* 004B0E50 g_UiScrollWheelDefaultStep: int32_t, 14: pixels scrolled per mouse-wheel step in a scrollable control whose child is not a list (UiScrollableControl wheel handler, src/ui/controls/lists.c). */
__declspec(align(16)) int32_t g_UiScrollWheelDefaultStep = 14;

/* 004B0E54 g_UiScrollWheelListStep: int32_t, 15: pixels per mouse-wheel step when the scrollable control's child is a list/text list/timed list control (src/ui/controls/lists.c). */
__declspec(align(4)) int32_t g_UiScrollWheelListStep = 15;

/* 004B0E58 g_UiRangeSliderDragScale: int32_t, 1: multiplier of wheelDelta * stepValue when the mouse wheel moves a range slider (src/ui/controls/input.c). */
__declspec(align(8)) int32_t g_UiRangeSliderDragScale = 1;

/* 004B0E5C g_UiTimedListActionDelayFrames */
__declspec(align(4)) UiFrameDelayFrames g_UiTimedListActionDelayFrames = 8;

/* 004B0E60 g_UiListActivationPulseFrames: UiFrameDelayFrames, 8: frames of the activation pulse after Enter on a list/text list before its action is queued (src/ui/controls/lists.c, text.c). */
__declspec(align(16)) UiFrameDelayFrames g_UiListActivationPulseFrames = 8;

/* 004B0E64 g_UiResizableWindowTitleTextTopOffset: int32_t, 5: pixels from the window top to the title text line of a resizable window (src/ui/controls/layout.c). */
__declspec(align(4)) int32_t g_UiResizableWindowTitleTextTopOffset = 5;

/* 004B0E68 g_UiResizableWindowTitleTextStyle: UiPackedTextStyle, 2: packed rich-text style of the resizable window title (src/ui/controls/layout.c). */
__declspec(align(8)) UiPackedTextStyle g_UiResizableWindowTitleTextStyle = 2;

/* 004B0E6C g_UiWindowMoveHandleWidth: int32_t, 19 (0x13): height in pixels of the top strip that drags a movable root window (src/ui/controls/layout.c). */
__declspec(align(4)) int32_t g_UiWindowMoveHandleWidth = 19;

/* 004B0E70 g_UiWindowResizeBorderThickness */
__declspec(align(16)) int32_t g_UiWindowResizeBorderThickness = 19;

/* 004B0E74 g_UiTextStyleSelected: UiPackedTextStyle, 0x10000 (palette byte 1): text style of the selected/highlighted row or item (src/ui/controls/text.c). */
__declspec(align(4)) UiPackedTextStyle g_UiTextStyleSelected = 0x10000;

/* 004B0E78 g_UiTextStyleNormal */
__declspec(align(8)) uint32_t g_UiTextStyleNormal = 0;

/* 004B0E7C g_UiTextStyleDisabled: UiPackedTextStyle, 0x20000 (palette byte 2): text style of disabled items (src/ui/controls/text.c). */
__declspec(align(4)) UiPackedTextStyle g_UiTextStyleDisabled = 0x20000;

/* 004B0E80 g_UiTextStyleAlternate */
__declspec(align(16)) uint32_t g_UiTextStyleAlternate = 0;

/* 004B0E88 g_UiWindowTitleTextStyle */
__declspec(align(8)) uint32_t g_UiWindowTitleTextStyle = 0;

/* 004B0E8C g_UiWindowFrameInset */
__declspec(align(4)) int32_t g_UiWindowFrameInset = 2;

/* 004B0E90 g_UiListTextStyle */
__declspec(align(16)) uint32_t g_UiListTextStyle = 0;

/* 004B0E98 g_UiTextEditActiveTextStyle */
__declspec(align(8)) uint32_t g_UiTextEditActiveTextStyle = 0;

/* 004B0E9C g_UiTextEditInactiveTextStyle */
__declspec(align(4)) uint32_t g_UiTextEditInactiveTextStyle = 0;

/* 004B0EA0 g_UiTextEditDisabledTextStyle */
__declspec(align(16)) uint32_t g_UiTextEditDisabledTextStyle = 0;

/* 004B0EA4 g_UiTextEditCaretBlinkPhaseStep: UiFrameDelayFrames, 8: frames per caret blink phase of a focused text edit, reloaded into the counter byte of editStateFlags (src/ui/controls/text.c). */
__declspec(align(4)) UiFrameDelayFrames g_UiTextEditCaretBlinkPhaseStep = 8;

/* 004B0EA8 g_UiHorizontalGaugeLabelTopInset: int32_t, 4: pixels from the gauge top to its label line (src/ui/controls/layout.c). */
__declspec(align(8)) int32_t g_UiHorizontalGaugeLabelTopInset = 4;

/* 004B0EAC g_UiHorizontalGaugeLabelTextStyle */
__declspec(align(4)) uint32_t g_UiHorizontalGaugeLabelTextStyle = 0;

/* 004B0EB8 g_UiWindowClassTexturePathUtf16 */
__declspec(align(8)) uint16_t g_UiWindowClassTexturePathUtf16[20] = L"engine\\winclass.gfx";

/* 004B0EE0 g_UiWindowClassTextPathUtf16 */
__declspec(align(16)) uint16_t g_UiWindowClassTextPathUtf16[19] = L"texte\\winclass.str";

/* 004B0F06 g_UiWindowTexturePathUtf16 */
__declspec(align(4)) uint16_t g_UiWindowTexturePathUtf16[15] = L"engine\\win.gfx";

/* 004B0F24 g_UiPointerCaptureButton */
__declspec(align(4)) UiPointerCaptureButton g_UiPointerCaptureButton = 255;

/* 004B0F28 g_UiImageControlHoverTarget */
__declspec(align(8)) UiImageControl * g_UiImageControlHoverTarget = 0;

/* 004B15D0 g_UiSpriteButtonControlVtable */
__declspec(align(16)) UiNodeVtable g_UiSpriteButtonControlVtable = {
    .relocate = (void *)UiSpriteButtonControl_Relocate,
    .method04 = (void *)UiNode_DefaultMethod04_NoOp,
    .drawClipped = (void *)UiSpriteButtonControl_DrawClipped,
    .layout = (void *)UiContainer_LayoutChildren,
    .nonRightPress = (void *)UiSpriteButtonControl_NonRightPress,
    .nonRightRelease = (void *)UiSpriteButtonControl_NonRightRelease,
    .rightPress = (void *)UiNode_ForwardRightPressToParent,
    .rightRelease = (void *)UiNode_DefaultRightRelease,
    .nonRightDrag = (void *)UiSpriteButtonControl_NonRightDrag,
    .rightDrag = (void *)UiNode_DefaultRightDrag,
    .pointerMove = (void *)UiNode_DefaultPointerMove,
    .hitTest = (void *)UiSpriteButtonControl_HitTestOpaque,
    .keyboardEvent = (void *)UiSelectableControl_KeyboardEvent,
    .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
    .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
    .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
    .tick = (void *)UiNode_DefaultTick,
    .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B1D80 g_UiNodeVtable_004B1D80 */
__declspec(align(16)) UiNodeVtable g_UiNodeVtable_004B1D80 = {
        .relocate = (void *)UiFramedTextButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiFramedTextButtonControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiFramedTextButtonControl_NonRightPress,
        .nonRightRelease = (void *)UiFramedTextButtonControl_NonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiFramedTextButtonControl_NonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiFramedTextButtonControl_HitTestRect,
        .keyboardEvent = (void *)UiSelectableControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B2740 g_UiWindowControlVtable */
__declspec(align(16)) UiNodeVtable g_UiWindowControlVtable = {
        .relocate = (void *)UiWindowControl_RelocateWithFrameInset,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiWindowControl_DrawFramedTextAndChrome,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiFramedTextButtonControl_NonRightPress,
        .nonRightRelease = (void *)UiFramedTextButtonControl_NonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiFramedTextButtonControl_NonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiFramedTextButtonControl_HitTestRect,
        .keyboardEvent = (void *)UiSelectableControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B2CE0 g_UiNodeVtable_004B2CE0 */
__declspec(align(16)) UiNodeVtable g_UiNodeVtable_004B2CE0 = {
        .relocate = (void *)UiTextButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiTextButtonControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiTextButtonControl_NonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiTextButtonControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B33D0 g_UiTitledWindowControlVtable */
__declspec(align(16)) UiNodeVtable g_UiTitledWindowControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiTitledWindowControl_DrawFrameTitleAndChildren,
        .layout = (void *)UiTitledWindowControl_LayoutFrameTitleAndChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B3770 g_UiImagePanelControlVtable */
__declspec(align(16)) UiNodeVtable g_UiImagePanelControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiImagePanelControl_DrawAlignedTextureAndChildren,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiImagePanelControl_HitTestAlignedTextureAndChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B3A50 g_UiFillPanelControlVtable */
__declspec(align(16)) UiNodeVtable g_UiFillPanelControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiFillPanelControl_DrawColorOrTiledTextureAndChildren,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiFillPanelControl_HitTestChildrenOnly,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B3C20 g_UiHorizontalGaugeControlVtable */
__declspec(align(16)) UiNodeVtable g_UiHorizontalGaugeControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiHorizontalGaugeControl_DrawFrameFillAndLabel,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiHorizontalGaugeControl_PointerMoveBusyCursor,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B3C68 g_UiWindowPercentTextUtf16 */
__declspec(align(8)) uint16_t g_UiWindowPercentTextUtf16[5] = {0};

/* 004B3EF0 g_UiRangeSliderControlVtable */
__declspec(align(16)) UiNodeVtable g_UiRangeSliderControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiRangeSliderControl_DrawTrackAndThumb,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiRangeSliderControl_BeginThumbDrag,
        .nonRightRelease = (void *)UiRangeSliderControl_EndThumbDrag,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiRangeSliderControl_UpdateValueFromPointer,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiRangeSliderControl_HandleKeyboard,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiRangeSliderControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiRangeSliderControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiRangeSliderControl_HandlePointerWheel};

/* 004B4650 g_UiLayoutContainerControlVtable */
__declspec(align(16)) UiNodeVtable g_UiLayoutContainerControlVtable = {
        .relocate = (void *)UiLayoutContainerControl_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiContainer_DrawIntersectingChildren,
        .layout = (void *)UiLayoutContainerControl_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiLayoutContainerControl_HitTestChildrenOnly,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiLayoutContainerControl_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiLayoutContainerControl_SuppressActionIdRecursive,
        .unsuppressActionId = (void *)UiLayoutContainerControl_UnsuppressActionIdRecursive,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B4950 g_UiPanelControlVtable */
__declspec(align(16)) UiNodeVtable g_UiPanelControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B4CC0 g_UiResizableWindowControlVtable */
__declspec(align(16)) UiNodeVtable g_UiResizableWindowControlVtable = {
        .relocate = (void *)UiResizableWindowControl_RelocateAndRefreshInteractionState,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiResizableWindowControl_DrawFrameTitleAndChildren,
        .layout = (void *)UiContainer_LayoutWithOptionalWindowHeaderOffset,
        .nonRightPress = (void *)UiResizableWindowControl_BeginMoveResizeOrWindowAction,
        .nonRightRelease = (void *)UiResizableWindowControl_EndMoveResizeAndHandleWindowActions,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiResizableWindowControl_UpdateMoveOrResize,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiResizableWindowControl_QueryResizeCursorCode,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiResizableWindowControl_HandleWindowHotkeys,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B58A0 g_UiNumericTextEditControlVtable */
__declspec(align(16)) UiNodeVtable g_UiNumericTextEditControlVtable = {
        .relocate = (void *)UiNumericTextEditControl_RelocateAndRebuildText,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiTextEditControl_DrawTextSelectionAndCaret,
        .layout = (void *)UiTextEditControl_RecomputeLayoutAndClampScroll,
        .nonRightPress = (void *)UiTextEditControl_BeginSelectionAtPointer,
        .nonRightRelease = (void *)UiTextEditControl_EndSelection,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiTextEditControl_UpdateSelectionFromPointer,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiNumericTextEditControl_HandleKeyboardAndCommit,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiTextEditControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiTextEditControl_UnsuppressIfActionId,
        .tick = (void *)UiTextEditControl_TickCaretBlink,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B6800 g_UiPathTextEditControlVtable */
__declspec(align(16)) UiNodeVtable g_UiPathTextEditControlVtable = {
        .relocate = (void *)UiPathTextEditControl_RelocateAndValidateDos83,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiTextEditControl_DrawTextSelectionAndCaret,
        .layout = (void *)UiTextEditControl_RecomputeLayoutAndClampScroll,
        .nonRightPress = (void *)UiTextEditControl_BeginSelectionAtPointer,
        .nonRightRelease = (void *)UiTextEditControl_EndSelection,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiTextEditControl_UpdateSelectionFromPointer,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiPathTextEditControl_HandleKeyboardAndValidate,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiTextEditControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiTextEditControl_UnsuppressIfActionId,
        .tick = (void *)UiTextEditControl_TickCaretBlink,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B7050 g_UiRequiredTextEditControlVtable */
__declspec(align(16)) UiNodeVtable g_UiRequiredTextEditControlVtable = {
        .relocate = (void *)UiRequiredTextEditControl_RelocateAndValidateNonEmpty,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiTextEditControl_DrawTextSelectionAndCaret,
        .layout = (void *)UiTextEditControl_RecomputeLayoutAndClampScroll,
        .nonRightPress = (void *)UiTextEditControl_BeginSelectionAtPointer,
        .nonRightRelease = (void *)UiTextEditControl_EndSelection,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiTextEditControl_UpdateSelectionFromPointer,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiRequiredTextEditControl_HandleKeyboardAndValidate,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiTextEditControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiTextEditControl_UnsuppressIfActionId,
        .tick = (void *)UiTextEditControl_TickCaretBlink,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004B7920 g_UiScrollableControlVtable */
__declspec(align(16)) UiNodeVtable g_UiScrollableControlVtable = {
    .relocate = (void *)UiScrollableControl_RelocateChildren,
    .method04 = (void *)UiNode_DefaultMethod04_NoOp,
    .drawClipped = (void *)UiScrollableControl_DrawFrameContentAndScrollbars,
    .layout = (void *)UiScrollableControl_RebuildViewportAndScrollbars,
    .nonRightPress = (void *)UiScrollableControl_BeginPrimaryScrollInteraction,
    .nonRightRelease = (void *)UiScrollableControl_EndPrimaryScrollInteraction,
    .rightPress = (void *)UiScrollableControl_BeginSecondaryScrollInteraction,
    .rightRelease = (void *)UiScrollableControl_EndSecondaryScrollInteraction,
    .nonRightDrag = (void *)UiScrollableControl_UpdatePrimaryScrollDrag,
    .rightDrag = (void *)UiScrollableControl_UpdateSecondaryScrollDrag,
    .pointerMove = (void *)UiScrollableControl_QueryPointerRegion,
    .hitTest = (void *)UiScrollableControl_HitTestContentAndScrollbars,
    .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
    .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
    .suppressActionId = (void *)UiContainer_SuppressActionId,
    .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
    .tick = (void *)UiScrollableControl_TickAutoScroll,
    .pointerWheel = (void *)UiScrollableControl_HandlePointerWheel};

/* 004B9530 g_UiFocusProxyControlVtable */
__declspec(align(16)) UiNodeVtable g_UiFocusProxyControlVtable = {
        .relocate = (void *)UiSingleLineTextControl_RelocateChild,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiSingleLineTextControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiSingleLineTextControl_ForwardNonRightPressToChild,
        .nonRightRelease = (void *)UiSingleLineTextControl_ForwardNonRightReleaseToChild,
        .rightPress = (void *)UiSingleLineTextControl_ForwardRightPressToChild,
        .rightRelease = (void *)UiSingleLineTextControl_ForwardRightReleaseToChild,
        .nonRightDrag = (void *)UiSingleLineTextControl_ForwardNonRightDragToChild,
        .rightDrag = (void *)UiSingleLineTextControl_ForwardRightDragToChild,
        .pointerMove = (void *)UiSingleLineTextControl_ForwardPointerMoveToChild,
        .hitTest = (void *)UiSingleLineTextControl_HitTestChildProxy,
        .keyboardEvent = (void *)UiSingleLineTextControl_ForwardKeyboardEventToChild,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiSingleLineTextControl_ForwardTickToChild,
        .pointerWheel = (void *)UiSingleLineTextControl_ForwardPointerWheelToChildOrParent};

/* 004B9E40 g_UiTextListControlVtable */
__declspec(align(16)) UiNodeVtable g_UiTextListControlVtable = {
    .relocate = (void *)UiContainer_RelocateChildren,
    .method04 = (void *)UiNode_DefaultMethod04_NoOp,
    .drawClipped = (void *)UiTextListControl_DrawRowsAndSelection,
    .layout = (void *)UiContainer_LayoutChildren,
    .nonRightPress = (void *)UiTextListControl_SelectRowFromPointer,
    .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
    .rightPress = (void *)UiNode_ForwardRightPressToParent,
    .rightRelease = (void *)UiNode_DefaultRightRelease,
    .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
    .rightDrag = (void *)UiNode_DefaultRightDrag,
    .pointerMove = (void *)UiNode_DefaultPointerMove,
    .hitTest = (void *)UiContainer_HitTestChildren,
    .keyboardEvent = (void *)UiTextListControl_HandleKeyboardNavigationAndSearch,
    .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
    .suppressActionId = (void *)UiTextListControl_SuppressIfActionId,
    .unsuppressActionId = (void *)UiTextListControl_UnsuppressIfActionId,
    .tick = (void *)UiTextListControl_TickActivationPulse,
    .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004BA590 g_UiListControlVtable */
__declspec(align(16)) UiNodeVtable g_UiListControlVtable = {
    .relocate = (void *)UiContainer_RelocateChildren,
    .method04 = (void *)UiNode_DefaultMethod04_NoOp,
    .drawClipped = (void *)UiListControl_DrawRowsAndSelection,
    .layout = (void *)UiContainer_LayoutChildren,
    .nonRightPress = (void *)UiListControl_SelectRowFromPointer,
    .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
    .rightPress = (void *)UiNode_ForwardRightPressToParent,
    .rightRelease = (void *)UiNode_DefaultRightRelease,
    .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
    .rightDrag = (void *)UiNode_DefaultRightDrag,
    .pointerMove = (void *)UiNode_DefaultPointerMove,
    .hitTest = (void *)UiContainer_HitTestChildren,
    .keyboardEvent = (void *)UiListControl_HandleKeyboardNavigation,
    .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
    .suppressActionId = (void *)UiListControl_SuppressIfActionId,
    .unsuppressActionId = (void *)UiListControl_UnsuppressIfActionId,
    .tick = (void *)UiListControl_TickActivationPulse,
    .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004BA5D8 g_UiPointerListExpandedLeftTextUtf16: 1 KiB expansion scratch of UiPointerList_CompareExpandedText */
__declspec(align(8)) uint16_t g_UiPointerListExpandedLeftTextUtf16[512] = {0};

/* 004BA9D8 g_UiPointerListExpandedRightTextUtf16: 1 KiB expansion scratch of UiPointerList_CompareExpandedText */
__declspec(align(8)) uint16_t g_UiPointerListExpandedRightTextUtf16[512] = {0};

/* 004BB990 g_UiTimedListControlVtable */
__declspec(align(16)) UiNodeVtable g_UiTimedListControlVtable = {
    .relocate = (void *)UiTimedListControl_RelocateChildren,
    .method04 = (void *)UiNode_DefaultMethod04_NoOp,
    .drawClipped = (void *)UiTimedListControl_DrawRowsAndSelection,
    .layout = (void *)UiContainer_LayoutChildren,
    .nonRightPress = (void *)UiTimedListControl_SelectRowFromPointer,
    .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
    .rightPress = (void *)UiNode_ForwardRightPressToParent,
    .rightRelease = (void *)UiNode_DefaultRightRelease,
    .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
    .rightDrag = (void *)UiNode_DefaultRightDrag,
    .pointerMove = (void *)UiNode_DefaultPointerMove,
    .hitTest = (void *)UiContainer_HitTestChildren,
    .keyboardEvent = (void *)UiTimedListControl_HandleKeyboardNavigation,
    .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
    .suppressActionId = (void *)UiContainer_SuppressActionId,
    .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
    .tick = (void *)UiTimedListControl_TickActionDelay,
    .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004BC410 g_UiListOffsetControlVtable */
__declspec(align(16)) UiNodeVtable g_UiListOffsetControlVtable = {
        .relocate = (void *)UiWrappedTextControl_RelocateAndApplyDeferredOffset,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiWrappedTextControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004BC570 g_UiNodeVtable_004BC570 */
__declspec(align(16)) UiNodeVtable g_UiNodeVtable_004BC570 = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiImageControl_DrawClipped,
        .layout = (void *)UiImageControl_LayoutChildrenToParent,
        .nonRightPress = (void *)UiImageControl_NonRightPress,
        .nonRightRelease = (void *)UiImageControl_NonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiImageControl_NonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiImageControl_PointerMove,
        .hitTest = (void *)UiImageControl_HitTestOpaque,
        .keyboardEvent = (void *)UiSelectableControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiImageControl_TickHover,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 004BCC30 g_UiNineSlicePanelControlVtable */
__declspec(align(16)) UiNodeVtable g_UiNineSlicePanelControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiNineSlicePanelControl_DrawTextureFrameAndChildren,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent};

/* 00514FC0 g_UiImageActionControlVtable (followed by 0x90 code filler) */
__declspec(align(16)) UiNodeVtable g_UiImageActionControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiImageActionControl_DrawImageAndChildren,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiImageActionControl_EnqueuePrimaryAction,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiImageActionControl_EnqueueSecondaryAction,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiImageActionControl_QueryPointerCode,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiImageActionControl_HandleKeyboardActivation,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 00515290 g_UiConditionalActionControlVtable (followed by 0x90 code filler) */
__declspec(align(16)) UiNodeVtable g_UiConditionalActionControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiConditionalActionControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiConditionalActionControl_EnqueuePrimaryActionIfEnabled,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiConditionalActionControl_QueryPointerCode,
        .hitTest = (void *)UiConditionalActionControl_HitTestWhenEnabled,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 00515610 g_UiNumericPairTextButtonVtable */
__declspec(align(16)) UiNodeVtable g_UiNumericPairTextButtonVtable = {
        .relocate = (void *)UiTextButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiNumericPairTextButton_DrawFormattedValues,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiTextButtonControl_NonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiTextButtonControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 00515658 g_UiNumericPairFirstValueScratchUtf16 */
__declspec(align(8)) uint16_t g_UiNumericPairFirstValueScratchUtf16[16] = {0};

/* 00515678 g_UiNumericPairSecondValueScratchUtf16 (followed by 0x90 code filler up to 005156A0) */
__declspec(align(8)) uint16_t g_UiNumericPairSecondValueScratchUtf16[16] = {0};

/* 00515730 g_UiPayloadPairTextButtonVtable (followed by 0x90 code filler) */
__declspec(align(16)) UiNodeVtable g_UiPayloadPairTextButtonVtable = {
        .relocate = (void *)UiTextButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiPayloadPairTextButton_DrawFormattedPayloads,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiTextButtonControl_NonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiTextButtonControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 005157E0 g_UiFormattedContainerVtable (followed by 0x90 code filler) */
__declspec(align(16)) UiNodeVtable g_UiFormattedContainerVtable = {
        .relocate = (void *)UiFormattedContainer_RelocateWithPatchedTextPayloads,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiFormattedContainer_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 00515C70 g_UiSelectionGeometryControlVtable (followed by 0x90 code filler) */
__declspec(align(16)) UiNodeVtable g_UiSelectionGeometryControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiSelectionGeometryControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiSelectionGeometryControl_ConvertPointerAndEnqueueAction,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 00516510 g_UiCatalogEntryRichTextScratchUtf16 */
__declspec(align(16)) uint16_t g_UiCatalogEntryRichTextScratchUtf16[16] = {0};

/* 00516530 g_UiNodeVtable_00516530 (followed by 0x90 code filler) */
__declspec(align(16)) UiNodeVtable g_UiNodeVtable_00516530 = {
        .relocate = (void *)UiSpriteButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiCatalogEntryControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiCommandSpriteButtonControl_BeginPress,
        .nonRightRelease = (void *)UiCatalogEntryControl_NonRightRelease,
        .rightPress = (void *)UiCommandSpriteButtonControl_BeginPress,
        .rightRelease = (void *)UiCommandSpriteButtonControl_RightRelease,
        .nonRightDrag = (void *)UiSpriteButtonControl_NonRightDrag,
        .rightDrag = (void *)UiSpriteButtonControl_NonRightDrag,
        .pointerMove = (void *)UiCatalogEntryControl_PointerMove,
        .hitTest = (void *)UiSpriteButtonControl_HitTestOpaque,
        .keyboardEvent = (void *)UiSelectableControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 00516CC0 g_UiArmyMetricsPanelVtable (followed by 0x90 code filler) */
__declspec(align(16)) UiNodeVtable g_UiArmyMetricsPanelVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiArmyMetricsPanel_DrawTextureMetricsAndChildren,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiNode_DefaultNonRightPress,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiImagePanelControl_HitTestAlignedTextureAndChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 00517DE0 g_UiTransferProgressGaugeVtable */
__declspec(align(16)) UiNodeVtable g_UiTransferProgressGaugeVtable = {
    .relocate = (void *)UiContainer_RelocateChildren,
    .method04 = (void *)UiNode_DefaultMethod04_NoOp,
    .drawClipped = (void *)UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw,
    .layout = (void *)UiContainer_LayoutChildren,
    .nonRightPress = (void *)UiNode_DefaultNonRightPress,
    .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
    .rightPress = (void *)UiNode_ForwardRightPressToParent,
    .rightRelease = (void *)UiNode_DefaultRightRelease,
    .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
    .rightDrag = (void *)UiNode_DefaultRightDrag,
    .pointerMove = (void *)UiNode_DefaultPointerMove,
    .hitTest = (void *)UiContainer_HitTestChildren,
    .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
    .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
    .suppressActionId = (void *)UiContainer_SuppressActionId,
    .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
    .tick = (void *)UiNode_DefaultTick,
    .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 00517F10 g_UiCommandVisibilityWrappedTextVtable */
__declspec(align(16)) UiNodeVtable g_UiCommandVisibilityWrappedTextVtable = {
    .relocate = (void *)UiWrappedTextControl_RelocateAndApplyDeferredOffset,
    .method04 = (void *)UiNode_DefaultMethod04_NoOp,
    .drawClipped = (void *)UiCommandVisibilityWrappedText_DrawWhenAllowed,
    .layout = (void *)UiContainer_LayoutChildren,
    .nonRightPress = (void *)UiNode_DefaultNonRightPress,
    .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
    .rightPress = (void *)UiNode_ForwardRightPressToParent,
    .rightRelease = (void *)UiNode_DefaultRightRelease,
    .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
    .rightDrag = (void *)UiNode_DefaultRightDrag,
    .pointerMove = (void *)UiNode_DefaultPointerMove,
    .hitTest = (void *)FrontendResultsTable_HitTestAlwaysNone,
    .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
    .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
    .suppressActionId = (void *)UiContainer_SuppressActionId,
    .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
    .tick = (void *)UiNode_DefaultTick,
    .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 00517FC0 g_UiCommandVisibilitySingleLineTextVtable */
__declspec(align(16)) UiNodeVtable g_UiCommandVisibilitySingleLineTextVtable = {
    .relocate = (void *)UiSingleLineTextControl_RelocateChild,
    .method04 = (void *)UiNode_DefaultMethod04_NoOp,
    .drawClipped = (void *)UiCommandVisibilitySingleLineText_DrawWhenAllowed,
    .layout = (void *)UiContainer_LayoutChildren,
    .nonRightPress = (void *)UiNode_DefaultNonRightPress,
    .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
    .rightPress = (void *)UiNode_ForwardRightPressToParent,
    .rightRelease = (void *)UiNode_DefaultRightRelease,
    .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
    .rightDrag = (void *)UiNode_DefaultRightDrag,
    .pointerMove = (void *)UiNode_DefaultPointerMove,
    .hitTest = (void *)FrontendResultsTable_HitTestAlwaysNone,
    .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
    .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
    .suppressActionId = (void *)UiContainer_SuppressActionId,
    .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
    .tick = (void *)UiNode_DefaultTick,
    .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 00518C90 g_UiSoftwareTexturePreviewControlVtable */
__declspec(align(16)) UiNodeVtable g_UiSoftwareTexturePreviewControlVtable = {
    .relocate = (void *)UiContainer_RelocateChildren,
    .method04 = (void *)UiNode_DefaultMethod04_NoOp,
    .drawClipped = (void *)UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren,
    .layout = (void *)UiContainer_LayoutChildren,
    .nonRightPress = (void *)UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress,
    .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
    .rightPress = (void *)UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress,
    .rightRelease = (void *)UiNode_DefaultRightRelease,
    .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
    .rightDrag = (void *)UiNode_DefaultRightDrag,
    .pointerMove = (void *)UiNode_DefaultPointerMove,
    .hitTest = (void *)UiContainer_HitTestChildren,
    .keyboardEvent = (void *)UiSoftwareTexturePreviewControl_HandleKeyboardActivation,
    .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
    .suppressActionId = (void *)UiContainer_SuppressActionId,
    .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
    .tick = (void *)UiNode_DefaultTick,
    .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

/* 00562648 g_UiCatalogGroup48ColumnCount */
__declspec(align(8)) uint32_t g_UiCatalogGroup48ColumnCount = 0;

/* 0056264C g_UiCatalogGroup42ColumnCount */
__declspec(align(4)) uint32_t g_UiCatalogGroup42ColumnCount = 0;

/* 00562654 g_UiCatalogGroup48OffsetTables */
__declspec(align(4)) int32_t *g_UiCatalogGroup48OffsetTables[9] = {
    /* 0 */ (void *)&g_UiCatalogGroup48OffsetsDefault,
    /* 1 */ (void *)&g_UiCatalogGroup48OffsetsDefault,
    /* 2 */ (void *)&g_UiCatalogGroup48OffsetsDefault,
    /* 3 */ (void *)&g_UiCatalogGroup48OffsetsDefault,
    /* 4 */ (void *)&g_UiCatalogGroup48OffsetsDefault,
    /* 5 */ (void *)&g_UiCatalogGroup48Offsets5Columns,
    /* 6 */ (void *)&g_UiCatalogGroup48Offsets6Columns,
    /* 7 */ (void *)&g_UiCatalogGroup48Offsets7Columns,
    /* 8 */ (void *)&g_UiCatalogGroup48Offsets8Columns};

/* 00562678 g_UiCatalogGroup42OffsetTables */
__declspec(align(8)) int32_t *g_UiCatalogGroup42OffsetTables[7] = {
    /* 0 */ (void *)&g_UiCatalogGroup42OffsetsDefault,
    /* 1 */ (void *)&g_UiCatalogGroup42OffsetsDefault,
    /* 2 */ (void *)&g_UiCatalogGroup42OffsetsDefault,
    /* 3 */ (void *)&g_UiCatalogGroup42OffsetsDefault,
    /* 4 */ (void *)&g_UiCatalogGroup42OffsetsDefault,
    /* 5 */ (void *)&g_UiCatalogGroup42Offsets5Columns,
    /* 6 */ (void *)&g_UiCatalogGroup42Offsets6Columns};

/* 005626A8 g_UiCatalogGroup48OffsetsDefault */
__declspec(align(8)) int32_t g_UiCatalogGroup48OffsetsDefault[48] = {
    /*  0 */ 24188, 24316, 24444, 24572, 24700, 24828, 24956, 25084,
    /*  8 */ 25212, 25340, 25468, 25596, 25724, 25852, 25980, 26108,
    /* 16 */ 26236, 26364, 26492, 26620, 26748, 26876, 27004, 27132,
    /* 24 */ 27260, 27388, 27516, 27644, 27772, 27900, 28028, 28156,
    /* 32 */ 28284, 28412, 28540, 28668, 28796, 28924, 29052, 29180,
    /* 40 */ 29308, 29436, 29564, 29692, 29820, 29948, 30076, 30204};

/* 00562768 g_UiCatalogGroup48Offsets5Columns */
__declspec(align(8)) int32_t g_UiCatalogGroup48Offsets5Columns[48] = {
    /*  0 */ 24188, 24316, 24444, 24572, 27260, 24700, 24828, 24956,
    /*  8 */ 25084, 27388, 25212, 25340, 25468, 25596, 27516, 25724,
    /* 16 */ 25852, 25980, 26108, 27644, 26236, 26364, 26492, 26620,
    /* 24 */ 27772, 26748, 26876, 27004, 27132, 27900, 28028, 28156,
    /* 32 */ 28284, 28412, 28540, 28668, 28796, 28924, 29052, 29180,
    /* 40 */ 29308, 29436, 29564, 29692, 29820, 29948, 30076, 30204};

/* 00562828 g_UiCatalogGroup48Offsets6Columns */
__declspec(align(8)) int32_t g_UiCatalogGroup48Offsets6Columns[48] = {
    /*  0 */ 24188, 24316, 24444, 24572, 27260, 28028, 24700, 24828,
    /*  8 */ 24956, 25084, 27388, 28156, 25212, 25340, 25468, 25596,
    /* 16 */ 27516, 28284, 25724, 25852, 25980, 26108, 27644, 28412,
    /* 24 */ 26236, 26364, 26492, 26620, 27772, 28540, 26748, 26876,
    /* 32 */ 27004, 27132, 27900, 28668, 28796, 28924, 29052, 29180,
    /* 40 */ 29308, 29436, 29564, 29692, 29820, 29948, 30076, 30204};

/* 005628E8 g_UiCatalogGroup48Offsets7Columns */
__declspec(align(8)) int32_t g_UiCatalogGroup48Offsets7Columns[48] = {
    /*  0 */ 24188, 24316, 24444, 24572, 27260, 28028, 28796, 24700,
    /*  8 */ 24828, 24956, 25084, 27388, 28156, 28924, 25212, 25340,
    /* 16 */ 25468, 25596, 27516, 28284, 29052, 25724, 25852, 25980,
    /* 24 */ 26108, 27644, 28412, 29180, 26236, 26364, 26492, 26620,
    /* 32 */ 27772, 28540, 29308, 26748, 26876, 27004, 27132, 27900,
    /* 40 */ 28668, 29436, 29564, 29692, 29820, 29948, 30076, 30204};

/* 005629A8 g_UiCatalogGroup48Offsets8Columns */
__declspec(align(8)) int32_t g_UiCatalogGroup48Offsets8Columns[48] = {
    /*  0 */ 24188, 24316, 24444, 24572, 27260, 28028, 28796, 29564,
    /*  8 */ 24700, 24828, 24956, 25084, 27388, 28156, 28924, 29692,
    /* 16 */ 25212, 25340, 25468, 25596, 27516, 28284, 29052, 29820,
    /* 24 */ 25724, 25852, 25980, 26108, 27644, 28412, 29180, 29948,
    /* 32 */ 26236, 26364, 26492, 26620, 27772, 28540, 29308, 30076,
    /* 40 */ 26748, 26876, 27004, 27132, 27900, 28668, 29436, 30204};

/* 00562A68 g_UiCatalogGroup42OffsetsDefault */
__declspec(align(8)) int32_t g_UiCatalogGroup42OffsetsDefault[42] = {
    /*  0 */ 30536, 30664, 30792, 30920, 31048, 31176, 31304, 31432,
    /*  8 */ 31560, 31688, 31816, 31944, 32072, 32200, 32328, 32456,
    /* 16 */ 32584, 32712, 32840, 32968, 33096, 33224, 33352, 33480,
    /* 24 */ 33608, 33736, 33864, 33992, 34120, 34248, 34376, 34504,
    /* 32 */ 34632, 34760, 34888, 35016, 35144, 35272, 35400, 35528,
    /* 40 */ 35656, 35784};

/* 00562B10 g_UiCatalogGroup42Offsets5Columns */
__declspec(align(16)) int32_t g_UiCatalogGroup42Offsets5Columns[42] = {
    /*  0 */ 30536, 30664, 30792, 30920, 34120, 31048, 31176, 31304,
    /*  8 */ 31432, 34248, 31560, 31688, 31816, 31944, 34376, 32072,
    /* 16 */ 32200, 32328, 32456, 34504, 32584, 32712, 32840, 32968,
    /* 24 */ 34632, 33096, 33224, 33352, 33480, 34760, 33608, 33736,
    /* 32 */ 33864, 33992, 34888, 35016, 35144, 35272, 35400, 35528,
    /* 40 */ 35656, 35784};

/* 00562BB8 g_UiCatalogGroup42Offsets6Columns */
__declspec(align(8)) int32_t g_UiCatalogGroup42Offsets6Columns[42] = {
    /*  0 */ 30536, 30664, 30792, 30920, 34120, 35016, 31048, 31176,
    /*  8 */ 31304, 31432, 34248, 35144, 31560, 31688, 31816, 31944,
    /* 16 */ 34376, 35272, 32072, 32200, 32328, 32456, 34504, 35400,
    /* 24 */ 32584, 32712, 32840, 32968, 34632, 35528, 33096, 33224,
    /* 32 */ 33352, 33480, 34760, 35656, 33608, 33736, 33864, 33992,
    /* 40 */ 34888, 35784};

/* 00576C24 g_DirectInputMouseRefreshCountdown */
__declspec(align(4)) UiFrameRefreshCountdownFrames g_DirectInputMouseRefreshCountdown = 16;
