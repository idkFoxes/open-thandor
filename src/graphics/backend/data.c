/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/graphics/backend/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 0041F688 g_SoftwareBilinearPackedByteClampMask */
__declspec(align(8)) uint64_t g_SoftwareBilinearPackedByteClampMask = 0xFFFFFFFFull;

/* 0041F6E0 g_SoftwarePixelMmxConstants */
__declspec(align(16)) SoftwarePixelMmxConstants g_SoftwarePixelMmxConstants = {0};

/* 0041FF20 g_SoftwareBilinearForwardFactors */
__declspec(align(16)) SoftwareBgraWordLanes g_SoftwareBilinearForwardFactors[257] = {
    /*   0 */ {0},
    /*   1 */ {.blue = 64, .green = 64, .red = 64, .alpha = 64},
    /*   2 */ {.blue = 128, .green = 128, .red = 128, .alpha = 128},
    /*   3 */ {.blue = 192, .green = 192, .red = 192, .alpha = 192},
    /*   4 */ {.blue = 257, .green = 257, .red = 257, .alpha = 257},
    /*   5 */ {.blue = 321, .green = 321, .red = 321, .alpha = 321},
    /*   6 */ {.blue = 385, .green = 385, .red = 385, .alpha = 385},
    /*   7 */ {.blue = 449, .green = 449, .red = 449, .alpha = 449},
    /*   8 */ {.blue = 514, .green = 514, .red = 514, .alpha = 514},
    /*   9 */ {.blue = 578, .green = 578, .red = 578, .alpha = 578},
    /*  10 */ {.blue = 642, .green = 642, .red = 642, .alpha = 642},
    /*  11 */ {.blue = 706, .green = 706, .red = 706, .alpha = 706},
    /*  12 */ {.blue = 771, .green = 771, .red = 771, .alpha = 771},
    /*  13 */ {.blue = 835, .green = 835, .red = 835, .alpha = 835},
    /*  14 */ {.blue = 899, .green = 899, .red = 899, .alpha = 899},
    /*  15 */ {.blue = 963, .green = 963, .red = 963, .alpha = 963},
    /*  16 */ {.blue = 1028, .green = 1028, .red = 1028, .alpha = 1028},
    /*  17 */ {.blue = 1092, .green = 1092, .red = 1092, .alpha = 1092},
    /*  18 */ {.blue = 1156, .green = 1156, .red = 1156, .alpha = 1156},
    /*  19 */ {.blue = 1220, .green = 1220, .red = 1220, .alpha = 1220},
    /*  20 */ {.blue = 1285, .green = 1285, .red = 1285, .alpha = 1285},
    /*  21 */ {.blue = 1349, .green = 1349, .red = 1349, .alpha = 1349},
    /*  22 */ {.blue = 1413, .green = 1413, .red = 1413, .alpha = 1413},
    /*  23 */ {.blue = 1477, .green = 1477, .red = 1477, .alpha = 1477},
    /*  24 */ {.blue = 1542, .green = 1542, .red = 1542, .alpha = 1542},
    /*  25 */ {.blue = 1606, .green = 1606, .red = 1606, .alpha = 1606},
    /*  26 */ {.blue = 1670, .green = 1670, .red = 1670, .alpha = 1670},
    /*  27 */ {.blue = 1734, .green = 1734, .red = 1734, .alpha = 1734},
    /*  28 */ {.blue = 1799, .green = 1799, .red = 1799, .alpha = 1799},
    /*  29 */ {.blue = 1863, .green = 1863, .red = 1863, .alpha = 1863},
    /*  30 */ {.blue = 1927, .green = 1927, .red = 1927, .alpha = 1927},
    /*  31 */ {.blue = 1991, .green = 1991, .red = 1991, .alpha = 1991},
    /*  32 */ {.blue = 2056, .green = 2056, .red = 2056, .alpha = 2056},
    /*  33 */ {.blue = 2120, .green = 2120, .red = 2120, .alpha = 2120},
    /*  34 */ {.blue = 2184, .green = 2184, .red = 2184, .alpha = 2184},
    /*  35 */ {.blue = 2248, .green = 2248, .red = 2248, .alpha = 2248},
    /*  36 */ {.blue = 2313, .green = 2313, .red = 2313, .alpha = 2313},
    /*  37 */ {.blue = 2377, .green = 2377, .red = 2377, .alpha = 2377},
    /*  38 */ {.blue = 2441, .green = 2441, .red = 2441, .alpha = 2441},
    /*  39 */ {.blue = 2505, .green = 2505, .red = 2505, .alpha = 2505},
    /*  40 */ {.blue = 2570, .green = 2570, .red = 2570, .alpha = 2570},
    /*  41 */ {.blue = 2634, .green = 2634, .red = 2634, .alpha = 2634},
    /*  42 */ {.blue = 2698, .green = 2698, .red = 2698, .alpha = 2698},
    /*  43 */ {.blue = 2762, .green = 2762, .red = 2762, .alpha = 2762},
    /*  44 */ {.blue = 2827, .green = 2827, .red = 2827, .alpha = 2827},
    /*  45 */ {.blue = 2891, .green = 2891, .red = 2891, .alpha = 2891},
    /*  46 */ {.blue = 2955, .green = 2955, .red = 2955, .alpha = 2955},
    /*  47 */ {.blue = 3019, .green = 3019, .red = 3019, .alpha = 3019},
    /*  48 */ {.blue = 3084, .green = 3084, .red = 3084, .alpha = 3084},
    /*  49 */ {.blue = 3148, .green = 3148, .red = 3148, .alpha = 3148},
    /*  50 */ {.blue = 3212, .green = 3212, .red = 3212, .alpha = 3212},
    /*  51 */ {.blue = 3276, .green = 3276, .red = 3276, .alpha = 3276},
    /*  52 */ {.blue = 3341, .green = 3341, .red = 3341, .alpha = 3341},
    /*  53 */ {.blue = 3405, .green = 3405, .red = 3405, .alpha = 3405},
    /*  54 */ {.blue = 3469, .green = 3469, .red = 3469, .alpha = 3469},
    /*  55 */ {.blue = 3533, .green = 3533, .red = 3533, .alpha = 3533},
    /*  56 */ {.blue = 3598, .green = 3598, .red = 3598, .alpha = 3598},
    /*  57 */ {.blue = 3662, .green = 3662, .red = 3662, .alpha = 3662},
    /*  58 */ {.blue = 3726, .green = 3726, .red = 3726, .alpha = 3726},
    /*  59 */ {.blue = 3790, .green = 3790, .red = 3790, .alpha = 3790},
    /*  60 */ {.blue = 3855, .green = 3855, .red = 3855, .alpha = 3855},
    /*  61 */ {.blue = 3919, .green = 3919, .red = 3919, .alpha = 3919},
    /*  62 */ {.blue = 3983, .green = 3983, .red = 3983, .alpha = 3983},
    /*  63 */ {.blue = 4047, .green = 4047, .red = 4047, .alpha = 4047},
    /*  64 */ {.blue = 4112, .green = 4112, .red = 4112, .alpha = 4112},
    /*  65 */ {.blue = 4176, .green = 4176, .red = 4176, .alpha = 4176},
    /*  66 */ {.blue = 4240, .green = 4240, .red = 4240, .alpha = 4240},
    /*  67 */ {.blue = 4304, .green = 4304, .red = 4304, .alpha = 4304},
    /*  68 */ {.blue = 4369, .green = 4369, .red = 4369, .alpha = 4369},
    /*  69 */ {.blue = 4433, .green = 4433, .red = 4433, .alpha = 4433},
    /*  70 */ {.blue = 4497, .green = 4497, .red = 4497, .alpha = 4497},
    /*  71 */ {.blue = 4561, .green = 4561, .red = 4561, .alpha = 4561},
    /*  72 */ {.blue = 4626, .green = 4626, .red = 4626, .alpha = 4626},
    /*  73 */ {.blue = 4690, .green = 4690, .red = 4690, .alpha = 4690},
    /*  74 */ {.blue = 4754, .green = 4754, .red = 4754, .alpha = 4754},
    /*  75 */ {.blue = 4818, .green = 4818, .red = 4818, .alpha = 4818},
    /*  76 */ {.blue = 4883, .green = 4883, .red = 4883, .alpha = 4883},
    /*  77 */ {.blue = 4947, .green = 4947, .red = 4947, .alpha = 4947},
    /*  78 */ {.blue = 5011, .green = 5011, .red = 5011, .alpha = 5011},
    /*  79 */ {.blue = 5075, .green = 5075, .red = 5075, .alpha = 5075},
    /*  80 */ {.blue = 5140, .green = 5140, .red = 5140, .alpha = 5140},
    /*  81 */ {.blue = 5204, .green = 5204, .red = 5204, .alpha = 5204},
    /*  82 */ {.blue = 5268, .green = 5268, .red = 5268, .alpha = 5268},
    /*  83 */ {.blue = 5332, .green = 5332, .red = 5332, .alpha = 5332},
    /*  84 */ {.blue = 5397, .green = 5397, .red = 5397, .alpha = 5397},
    /*  85 */ {.blue = 5461, .green = 5461, .red = 5461, .alpha = 5461},
    /*  86 */ {.blue = 5525, .green = 5525, .red = 5525, .alpha = 5525},
    /*  87 */ {.blue = 5589, .green = 5589, .red = 5589, .alpha = 5589},
    /*  88 */ {.blue = 5654, .green = 5654, .red = 5654, .alpha = 5654},
    /*  89 */ {.blue = 5718, .green = 5718, .red = 5718, .alpha = 5718},
    /*  90 */ {.blue = 5782, .green = 5782, .red = 5782, .alpha = 5782},
    /*  91 */ {.blue = 5846, .green = 5846, .red = 5846, .alpha = 5846},
    /*  92 */ {.blue = 5911, .green = 5911, .red = 5911, .alpha = 5911},
    /*  93 */ {.blue = 5975, .green = 5975, .red = 5975, .alpha = 5975},
    /*  94 */ {.blue = 6039, .green = 6039, .red = 6039, .alpha = 6039},
    /*  95 */ {.blue = 6103, .green = 6103, .red = 6103, .alpha = 6103},
    /*  96 */ {.blue = 6168, .green = 6168, .red = 6168, .alpha = 6168},
    /*  97 */ {.blue = 6232, .green = 6232, .red = 6232, .alpha = 6232},
    /*  98 */ {.blue = 6296, .green = 6296, .red = 6296, .alpha = 6296},
    /*  99 */ {.blue = 6360, .green = 6360, .red = 6360, .alpha = 6360},
    /* 100 */ {.blue = 6425, .green = 6425, .red = 6425, .alpha = 6425},
    /* 101 */ {.blue = 6489, .green = 6489, .red = 6489, .alpha = 6489},
    /* 102 */ {.blue = 6553, .green = 6553, .red = 6553, .alpha = 6553},
    /* 103 */ {.blue = 6617, .green = 6617, .red = 6617, .alpha = 6617},
    /* 104 */ {.blue = 6682, .green = 6682, .red = 6682, .alpha = 6682},
    /* 105 */ {.blue = 6746, .green = 6746, .red = 6746, .alpha = 6746},
    /* 106 */ {.blue = 6810, .green = 6810, .red = 6810, .alpha = 6810},
    /* 107 */ {.blue = 6874, .green = 6874, .red = 6874, .alpha = 6874},
    /* 108 */ {.blue = 6939, .green = 6939, .red = 6939, .alpha = 6939},
    /* 109 */ {.blue = 7003, .green = 7003, .red = 7003, .alpha = 7003},
    /* 110 */ {.blue = 7067, .green = 7067, .red = 7067, .alpha = 7067},
    /* 111 */ {.blue = 7131, .green = 7131, .red = 7131, .alpha = 7131},
    /* 112 */ {.blue = 7196, .green = 7196, .red = 7196, .alpha = 7196},
    /* 113 */ {.blue = 7260, .green = 7260, .red = 7260, .alpha = 7260},
    /* 114 */ {.blue = 7324, .green = 7324, .red = 7324, .alpha = 7324},
    /* 115 */ {.blue = 7388, .green = 7388, .red = 7388, .alpha = 7388},
    /* 116 */ {.blue = 7453, .green = 7453, .red = 7453, .alpha = 7453},
    /* 117 */ {.blue = 7517, .green = 7517, .red = 7517, .alpha = 7517},
    /* 118 */ {.blue = 7581, .green = 7581, .red = 7581, .alpha = 7581},
    /* 119 */ {.blue = 7645, .green = 7645, .red = 7645, .alpha = 7645},
    /* 120 */ {.blue = 7710, .green = 7710, .red = 7710, .alpha = 7710},
    /* 121 */ {.blue = 7774, .green = 7774, .red = 7774, .alpha = 7774},
    /* 122 */ {.blue = 7838, .green = 7838, .red = 7838, .alpha = 7838},
    /* 123 */ {.blue = 7902, .green = 7902, .red = 7902, .alpha = 7902},
    /* 124 */ {.blue = 7967, .green = 7967, .red = 7967, .alpha = 7967},
    /* 125 */ {.blue = 8031, .green = 8031, .red = 8031, .alpha = 8031},
    /* 126 */ {.blue = 8095, .green = 8095, .red = 8095, .alpha = 8095},
    /* 127 */ {.blue = 8159, .green = 8159, .red = 8159, .alpha = 8159},
    /* 128 */ {.blue = 8224, .green = 8224, .red = 8224, .alpha = 8224},
    /* 129 */ {.blue = 8288, .green = 8288, .red = 8288, .alpha = 8288},
    /* 130 */ {.blue = 8352, .green = 8352, .red = 8352, .alpha = 8352},
    /* 131 */ {.blue = 8416, .green = 8416, .red = 8416, .alpha = 8416},
    /* 132 */ {.blue = 8481, .green = 8481, .red = 8481, .alpha = 8481},
    /* 133 */ {.blue = 8545, .green = 8545, .red = 8545, .alpha = 8545},
    /* 134 */ {.blue = 8609, .green = 8609, .red = 8609, .alpha = 8609},
    /* 135 */ {.blue = 8673, .green = 8673, .red = 8673, .alpha = 8673},
    /* 136 */ {.blue = 8738, .green = 8738, .red = 8738, .alpha = 8738},
    /* 137 */ {.blue = 8802, .green = 8802, .red = 8802, .alpha = 8802},
    /* 138 */ {.blue = 8866, .green = 8866, .red = 8866, .alpha = 8866},
    /* 139 */ {.blue = 8930, .green = 8930, .red = 8930, .alpha = 8930},
    /* 140 */ {.blue = 8995, .green = 8995, .red = 8995, .alpha = 8995},
    /* 141 */ {.blue = 9059, .green = 9059, .red = 9059, .alpha = 9059},
    /* 142 */ {.blue = 9123, .green = 9123, .red = 9123, .alpha = 9123},
    /* 143 */ {.blue = 9187, .green = 9187, .red = 9187, .alpha = 9187},
    /* 144 */ {.blue = 9252, .green = 9252, .red = 9252, .alpha = 9252},
    /* 145 */ {.blue = 9316, .green = 9316, .red = 9316, .alpha = 9316},
    /* 146 */ {.blue = 9380, .green = 9380, .red = 9380, .alpha = 9380},
    /* 147 */ {.blue = 9444, .green = 9444, .red = 9444, .alpha = 9444},
    /* 148 */ {.blue = 9509, .green = 9509, .red = 9509, .alpha = 9509},
    /* 149 */ {.blue = 9573, .green = 9573, .red = 9573, .alpha = 9573},
    /* 150 */ {.blue = 9637, .green = 9637, .red = 9637, .alpha = 9637},
    /* 151 */ {.blue = 9701, .green = 9701, .red = 9701, .alpha = 9701},
    /* 152 */ {.blue = 9766, .green = 9766, .red = 9766, .alpha = 9766},
    /* 153 */ {.blue = 9830, .green = 9830, .red = 9830, .alpha = 9830},
    /* 154 */ {.blue = 9894, .green = 9894, .red = 9894, .alpha = 9894},
    /* 155 */ {.blue = 9958, .green = 9958, .red = 9958, .alpha = 9958},
    /* 156 */ {.blue = 10023, .green = 10023, .red = 10023, .alpha = 10023},
    /* 157 */ {.blue = 10087, .green = 10087, .red = 10087, .alpha = 10087},
    /* 158 */ {.blue = 10151, .green = 10151, .red = 10151, .alpha = 10151},
    /* 159 */ {.blue = 10215, .green = 10215, .red = 10215, .alpha = 10215},
    /* 160 */ {.blue = 10280, .green = 10280, .red = 10280, .alpha = 10280},
    /* 161 */ {.blue = 10344, .green = 10344, .red = 10344, .alpha = 10344},
    /* 162 */ {.blue = 10408, .green = 10408, .red = 10408, .alpha = 10408},
    /* 163 */ {.blue = 10472, .green = 10472, .red = 10472, .alpha = 10472},
    /* 164 */ {.blue = 10537, .green = 10537, .red = 10537, .alpha = 10537},
    /* 165 */ {.blue = 10601, .green = 10601, .red = 10601, .alpha = 10601},
    /* 166 */ {.blue = 10665, .green = 10665, .red = 10665, .alpha = 10665},
    /* 167 */ {.blue = 10729, .green = 10729, .red = 10729, .alpha = 10729},
    /* 168 */ {.blue = 10794, .green = 10794, .red = 10794, .alpha = 10794},
    /* 169 */ {.blue = 10858, .green = 10858, .red = 10858, .alpha = 10858},
    /* 170 */ {.blue = 10922, .green = 10922, .red = 10922, .alpha = 10922},
    /* 171 */ {.blue = 10986, .green = 10986, .red = 10986, .alpha = 10986},
    /* 172 */ {.blue = 11051, .green = 11051, .red = 11051, .alpha = 11051},
    /* 173 */ {.blue = 11115, .green = 11115, .red = 11115, .alpha = 11115},
    /* 174 */ {.blue = 11179, .green = 11179, .red = 11179, .alpha = 11179},
    /* 175 */ {.blue = 11243, .green = 11243, .red = 11243, .alpha = 11243},
    /* 176 */ {.blue = 11308, .green = 11308, .red = 11308, .alpha = 11308},
    /* 177 */ {.blue = 11372, .green = 11372, .red = 11372, .alpha = 11372},
    /* 178 */ {.blue = 11436, .green = 11436, .red = 11436, .alpha = 11436},
    /* 179 */ {.blue = 11500, .green = 11500, .red = 11500, .alpha = 11500},
    /* 180 */ {.blue = 11565, .green = 11565, .red = 11565, .alpha = 11565},
    /* 181 */ {.blue = 11629, .green = 11629, .red = 11629, .alpha = 11629},
    /* 182 */ {.blue = 11693, .green = 11693, .red = 11693, .alpha = 11693},
    /* 183 */ {.blue = 11757, .green = 11757, .red = 11757, .alpha = 11757},
    /* 184 */ {.blue = 11822, .green = 11822, .red = 11822, .alpha = 11822},
    /* 185 */ {.blue = 11886, .green = 11886, .red = 11886, .alpha = 11886},
    /* 186 */ {.blue = 11950, .green = 11950, .red = 11950, .alpha = 11950},
    /* 187 */ {.blue = 12014, .green = 12014, .red = 12014, .alpha = 12014},
    /* 188 */ {.blue = 12079, .green = 12079, .red = 12079, .alpha = 12079},
    /* 189 */ {.blue = 12143, .green = 12143, .red = 12143, .alpha = 12143},
    /* 190 */ {.blue = 12207, .green = 12207, .red = 12207, .alpha = 12207},
    /* 191 */ {.blue = 12271, .green = 12271, .red = 12271, .alpha = 12271},
    /* 192 */ {.blue = 12336, .green = 12336, .red = 12336, .alpha = 12336},
    /* 193 */ {.blue = 12400, .green = 12400, .red = 12400, .alpha = 12400},
    /* 194 */ {.blue = 12464, .green = 12464, .red = 12464, .alpha = 12464},
    /* 195 */ {.blue = 12528, .green = 12528, .red = 12528, .alpha = 12528},
    /* 196 */ {.blue = 12593, .green = 12593, .red = 12593, .alpha = 12593},
    /* 197 */ {.blue = 12657, .green = 12657, .red = 12657, .alpha = 12657},
    /* 198 */ {.blue = 12721, .green = 12721, .red = 12721, .alpha = 12721},
    /* 199 */ {.blue = 12785, .green = 12785, .red = 12785, .alpha = 12785},
    /* 200 */ {.blue = 12850, .green = 12850, .red = 12850, .alpha = 12850},
    /* 201 */ {.blue = 12914, .green = 12914, .red = 12914, .alpha = 12914},
    /* 202 */ {.blue = 12978, .green = 12978, .red = 12978, .alpha = 12978},
    /* 203 */ {.blue = 13042, .green = 13042, .red = 13042, .alpha = 13042},
    /* 204 */ {.blue = 13107, .green = 13107, .red = 13107, .alpha = 13107},
    /* 205 */ {.blue = 13171, .green = 13171, .red = 13171, .alpha = 13171},
    /* 206 */ {.blue = 13235, .green = 13235, .red = 13235, .alpha = 13235},
    /* 207 */ {.blue = 13299, .green = 13299, .red = 13299, .alpha = 13299},
    /* 208 */ {.blue = 13364, .green = 13364, .red = 13364, .alpha = 13364},
    /* 209 */ {.blue = 13428, .green = 13428, .red = 13428, .alpha = 13428},
    /* 210 */ {.blue = 13492, .green = 13492, .red = 13492, .alpha = 13492},
    /* 211 */ {.blue = 13556, .green = 13556, .red = 13556, .alpha = 13556},
    /* 212 */ {.blue = 13621, .green = 13621, .red = 13621, .alpha = 13621},
    /* 213 */ {.blue = 13685, .green = 13685, .red = 13685, .alpha = 13685},
    /* 214 */ {.blue = 13749, .green = 13749, .red = 13749, .alpha = 13749},
    /* 215 */ {.blue = 13813, .green = 13813, .red = 13813, .alpha = 13813},
    /* 216 */ {.blue = 13878, .green = 13878, .red = 13878, .alpha = 13878},
    /* 217 */ {.blue = 13942, .green = 13942, .red = 13942, .alpha = 13942},
    /* 218 */ {.blue = 14006, .green = 14006, .red = 14006, .alpha = 14006},
    /* 219 */ {.blue = 14070, .green = 14070, .red = 14070, .alpha = 14070},
    /* 220 */ {.blue = 14135, .green = 14135, .red = 14135, .alpha = 14135},
    /* 221 */ {.blue = 14199, .green = 14199, .red = 14199, .alpha = 14199},
    /* 222 */ {.blue = 14263, .green = 14263, .red = 14263, .alpha = 14263},
    /* 223 */ {.blue = 14327, .green = 14327, .red = 14327, .alpha = 14327},
    /* 224 */ {.blue = 14392, .green = 14392, .red = 14392, .alpha = 14392},
    /* 225 */ {.blue = 14456, .green = 14456, .red = 14456, .alpha = 14456},
    /* 226 */ {.blue = 14520, .green = 14520, .red = 14520, .alpha = 14520},
    /* 227 */ {.blue = 14584, .green = 14584, .red = 14584, .alpha = 14584},
    /* 228 */ {.blue = 14649, .green = 14649, .red = 14649, .alpha = 14649},
    /* 229 */ {.blue = 14713, .green = 14713, .red = 14713, .alpha = 14713},
    /* 230 */ {.blue = 14777, .green = 14777, .red = 14777, .alpha = 14777},
    /* 231 */ {.blue = 14841, .green = 14841, .red = 14841, .alpha = 14841},
    /* 232 */ {.blue = 14906, .green = 14906, .red = 14906, .alpha = 14906},
    /* 233 */ {.blue = 14970, .green = 14970, .red = 14970, .alpha = 14970},
    /* 234 */ {.blue = 15034, .green = 15034, .red = 15034, .alpha = 15034},
    /* 235 */ {.blue = 15098, .green = 15098, .red = 15098, .alpha = 15098},
    /* 236 */ {.blue = 15163, .green = 15163, .red = 15163, .alpha = 15163},
    /* 237 */ {.blue = 15227, .green = 15227, .red = 15227, .alpha = 15227},
    /* 238 */ {.blue = 15291, .green = 15291, .red = 15291, .alpha = 15291},
    /* 239 */ {.blue = 15355, .green = 15355, .red = 15355, .alpha = 15355},
    /* 240 */ {.blue = 15420, .green = 15420, .red = 15420, .alpha = 15420},
    /* 241 */ {.blue = 15484, .green = 15484, .red = 15484, .alpha = 15484},
    /* 242 */ {.blue = 15548, .green = 15548, .red = 15548, .alpha = 15548},
    /* 243 */ {.blue = 15612, .green = 15612, .red = 15612, .alpha = 15612},
    /* 244 */ {.blue = 15677, .green = 15677, .red = 15677, .alpha = 15677},
    /* 245 */ {.blue = 15741, .green = 15741, .red = 15741, .alpha = 15741},
    /* 246 */ {.blue = 15805, .green = 15805, .red = 15805, .alpha = 15805},
    /* 247 */ {.blue = 15869, .green = 15869, .red = 15869, .alpha = 15869},
    /* 248 */ {.blue = 15934, .green = 15934, .red = 15934, .alpha = 15934},
    /* 249 */ {.blue = 15998, .green = 15998, .red = 15998, .alpha = 15998},
    /* 250 */ {.blue = 16062, .green = 16062, .red = 16062, .alpha = 16062},
    /* 251 */ {.blue = 16126, .green = 16126, .red = 16126, .alpha = 16126},
    /* 252 */ {.blue = 16191, .green = 16191, .red = 16191, .alpha = 16191},
    /* 253 */ {.blue = 16255, .green = 16255, .red = 16255, .alpha = 16255},
    /* 254 */ {.blue = 16319, .green = 16319, .red = 16319, .alpha = 16319},
    /* 255 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16383},
    /* 256: Original quirk: WorldLightingRuntime_UpdateInterpolatedTerrainLighting reads index 256 when
       the lighting cycle phase is 0 (cosine exactly 1.0); in the original that read the first entry
       of g_UiScalerFirstPixelWeights, which followed the table, so its value is kept here. */
    {.blue = 16384, .green = 16384, .red = 16384, .alpha = 16384}};

/* 00420F20 g_SoftwareBilinearInverseFactors */
__declspec(align(16)) SoftwareBgraWordLanes g_SoftwareBilinearInverseFactors[257] = {
    /*   0 */ {.blue = 16448, .green = 16448, .red = 16448, .alpha = 16448},
    /*   1 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16383},
    /*   2 */ {.blue = 16319, .green = 16319, .red = 16319, .alpha = 16319},
    /*   3 */ {.blue = 16255, .green = 16255, .red = 16255, .alpha = 16255},
    /*   4 */ {.blue = 16191, .green = 16191, .red = 16191, .alpha = 16191},
    /*   5 */ {.blue = 16126, .green = 16126, .red = 16126, .alpha = 16126},
    /*   6 */ {.blue = 16062, .green = 16062, .red = 16062, .alpha = 16062},
    /*   7 */ {.blue = 15998, .green = 15998, .red = 15998, .alpha = 15998},
    /*   8 */ {.blue = 15934, .green = 15934, .red = 15934, .alpha = 15934},
    /*   9 */ {.blue = 15869, .green = 15869, .red = 15869, .alpha = 15869},
    /*  10 */ {.blue = 15805, .green = 15805, .red = 15805, .alpha = 15805},
    /*  11 */ {.blue = 15741, .green = 15741, .red = 15741, .alpha = 15741},
    /*  12 */ {.blue = 15677, .green = 15677, .red = 15677, .alpha = 15677},
    /*  13 */ {.blue = 15612, .green = 15612, .red = 15612, .alpha = 15612},
    /*  14 */ {.blue = 15548, .green = 15548, .red = 15548, .alpha = 15548},
    /*  15 */ {.blue = 15484, .green = 15484, .red = 15484, .alpha = 15484},
    /*  16 */ {.blue = 15420, .green = 15420, .red = 15420, .alpha = 15420},
    /*  17 */ {.blue = 15355, .green = 15355, .red = 15355, .alpha = 15355},
    /*  18 */ {.blue = 15291, .green = 15291, .red = 15291, .alpha = 15291},
    /*  19 */ {.blue = 15227, .green = 15227, .red = 15227, .alpha = 15227},
    /*  20 */ {.blue = 15163, .green = 15163, .red = 15163, .alpha = 15163},
    /*  21 */ {.blue = 15098, .green = 15098, .red = 15098, .alpha = 15098},
    /*  22 */ {.blue = 15034, .green = 15034, .red = 15034, .alpha = 15034},
    /*  23 */ {.blue = 14970, .green = 14970, .red = 14970, .alpha = 14970},
    /*  24 */ {.blue = 14906, .green = 14906, .red = 14906, .alpha = 14906},
    /*  25 */ {.blue = 14841, .green = 14841, .red = 14841, .alpha = 14841},
    /*  26 */ {.blue = 14777, .green = 14777, .red = 14777, .alpha = 14777},
    /*  27 */ {.blue = 14713, .green = 14713, .red = 14713, .alpha = 14713},
    /*  28 */ {.blue = 14649, .green = 14649, .red = 14649, .alpha = 14649},
    /*  29 */ {.blue = 14584, .green = 14584, .red = 14584, .alpha = 14584},
    /*  30 */ {.blue = 14520, .green = 14520, .red = 14520, .alpha = 14520},
    /*  31 */ {.blue = 14456, .green = 14456, .red = 14456, .alpha = 14456},
    /*  32 */ {.blue = 14392, .green = 14392, .red = 14392, .alpha = 14392},
    /*  33 */ {.blue = 14327, .green = 14327, .red = 14327, .alpha = 14327},
    /*  34 */ {.blue = 14263, .green = 14263, .red = 14263, .alpha = 14263},
    /*  35 */ {.blue = 14199, .green = 14199, .red = 14199, .alpha = 14199},
    /*  36 */ {.blue = 14135, .green = 14135, .red = 14135, .alpha = 14135},
    /*  37 */ {.blue = 14070, .green = 14070, .red = 14070, .alpha = 14070},
    /*  38 */ {.blue = 14006, .green = 14006, .red = 14006, .alpha = 14006},
    /*  39 */ {.blue = 13942, .green = 13942, .red = 13942, .alpha = 13942},
    /*  40 */ {.blue = 13878, .green = 13878, .red = 13878, .alpha = 13878},
    /*  41 */ {.blue = 13813, .green = 13813, .red = 13813, .alpha = 13813},
    /*  42 */ {.blue = 13749, .green = 13749, .red = 13749, .alpha = 13749},
    /*  43 */ {.blue = 13685, .green = 13685, .red = 13685, .alpha = 13685},
    /*  44 */ {.blue = 13621, .green = 13621, .red = 13621, .alpha = 13621},
    /*  45 */ {.blue = 13556, .green = 13556, .red = 13556, .alpha = 13556},
    /*  46 */ {.blue = 13492, .green = 13492, .red = 13492, .alpha = 13492},
    /*  47 */ {.blue = 13428, .green = 13428, .red = 13428, .alpha = 13428},
    /*  48 */ {.blue = 13364, .green = 13364, .red = 13364, .alpha = 13364},
    /*  49 */ {.blue = 13299, .green = 13299, .red = 13299, .alpha = 13299},
    /*  50 */ {.blue = 13235, .green = 13235, .red = 13235, .alpha = 13235},
    /*  51 */ {.blue = 13171, .green = 13171, .red = 13171, .alpha = 13171},
    /*  52 */ {.blue = 13107, .green = 13107, .red = 13107, .alpha = 13107},
    /*  53 */ {.blue = 13042, .green = 13042, .red = 13042, .alpha = 13042},
    /*  54 */ {.blue = 12978, .green = 12978, .red = 12978, .alpha = 12978},
    /*  55 */ {.blue = 12914, .green = 12914, .red = 12914, .alpha = 12914},
    /*  56 */ {.blue = 12850, .green = 12850, .red = 12850, .alpha = 12850},
    /*  57 */ {.blue = 12785, .green = 12785, .red = 12785, .alpha = 12785},
    /*  58 */ {.blue = 12721, .green = 12721, .red = 12721, .alpha = 12721},
    /*  59 */ {.blue = 12657, .green = 12657, .red = 12657, .alpha = 12657},
    /*  60 */ {.blue = 12593, .green = 12593, .red = 12593, .alpha = 12593},
    /*  61 */ {.blue = 12528, .green = 12528, .red = 12528, .alpha = 12528},
    /*  62 */ {.blue = 12464, .green = 12464, .red = 12464, .alpha = 12464},
    /*  63 */ {.blue = 12400, .green = 12400, .red = 12400, .alpha = 12400},
    /*  64 */ {.blue = 12336, .green = 12336, .red = 12336, .alpha = 12336},
    /*  65 */ {.blue = 12271, .green = 12271, .red = 12271, .alpha = 12271},
    /*  66 */ {.blue = 12207, .green = 12207, .red = 12207, .alpha = 12207},
    /*  67 */ {.blue = 12143, .green = 12143, .red = 12143, .alpha = 12143},
    /*  68 */ {.blue = 12079, .green = 12079, .red = 12079, .alpha = 12079},
    /*  69 */ {.blue = 12014, .green = 12014, .red = 12014, .alpha = 12014},
    /*  70 */ {.blue = 11950, .green = 11950, .red = 11950, .alpha = 11950},
    /*  71 */ {.blue = 11886, .green = 11886, .red = 11886, .alpha = 11886},
    /*  72 */ {.blue = 11822, .green = 11822, .red = 11822, .alpha = 11822},
    /*  73 */ {.blue = 11757, .green = 11757, .red = 11757, .alpha = 11757},
    /*  74 */ {.blue = 11693, .green = 11693, .red = 11693, .alpha = 11693},
    /*  75 */ {.blue = 11629, .green = 11629, .red = 11629, .alpha = 11629},
    /*  76 */ {.blue = 11565, .green = 11565, .red = 11565, .alpha = 11565},
    /*  77 */ {.blue = 11500, .green = 11500, .red = 11500, .alpha = 11500},
    /*  78 */ {.blue = 11436, .green = 11436, .red = 11436, .alpha = 11436},
    /*  79 */ {.blue = 11372, .green = 11372, .red = 11372, .alpha = 11372},
    /*  80 */ {.blue = 11308, .green = 11308, .red = 11308, .alpha = 11308},
    /*  81 */ {.blue = 11243, .green = 11243, .red = 11243, .alpha = 11243},
    /*  82 */ {.blue = 11179, .green = 11179, .red = 11179, .alpha = 11179},
    /*  83 */ {.blue = 11115, .green = 11115, .red = 11115, .alpha = 11115},
    /*  84 */ {.blue = 11051, .green = 11051, .red = 11051, .alpha = 11051},
    /*  85 */ {.blue = 10986, .green = 10986, .red = 10986, .alpha = 10986},
    /*  86 */ {.blue = 10922, .green = 10922, .red = 10922, .alpha = 10922},
    /*  87 */ {.blue = 10858, .green = 10858, .red = 10858, .alpha = 10858},
    /*  88 */ {.blue = 10794, .green = 10794, .red = 10794, .alpha = 10794},
    /*  89 */ {.blue = 10729, .green = 10729, .red = 10729, .alpha = 10729},
    /*  90 */ {.blue = 10665, .green = 10665, .red = 10665, .alpha = 10665},
    /*  91 */ {.blue = 10601, .green = 10601, .red = 10601, .alpha = 10601},
    /*  92 */ {.blue = 10537, .green = 10537, .red = 10537, .alpha = 10537},
    /*  93 */ {.blue = 10472, .green = 10472, .red = 10472, .alpha = 10472},
    /*  94 */ {.blue = 10408, .green = 10408, .red = 10408, .alpha = 10408},
    /*  95 */ {.blue = 10344, .green = 10344, .red = 10344, .alpha = 10344},
    /*  96 */ {.blue = 10280, .green = 10280, .red = 10280, .alpha = 10280},
    /*  97 */ {.blue = 10215, .green = 10215, .red = 10215, .alpha = 10215},
    /*  98 */ {.blue = 10151, .green = 10151, .red = 10151, .alpha = 10151},
    /*  99 */ {.blue = 10087, .green = 10087, .red = 10087, .alpha = 10087},
    /* 100 */ {.blue = 10023, .green = 10023, .red = 10023, .alpha = 10023},
    /* 101 */ {.blue = 9958, .green = 9958, .red = 9958, .alpha = 9958},
    /* 102 */ {.blue = 9894, .green = 9894, .red = 9894, .alpha = 9894},
    /* 103 */ {.blue = 9830, .green = 9830, .red = 9830, .alpha = 9830},
    /* 104 */ {.blue = 9766, .green = 9766, .red = 9766, .alpha = 9766},
    /* 105 */ {.blue = 9701, .green = 9701, .red = 9701, .alpha = 9701},
    /* 106 */ {.blue = 9637, .green = 9637, .red = 9637, .alpha = 9637},
    /* 107 */ {.blue = 9573, .green = 9573, .red = 9573, .alpha = 9573},
    /* 108 */ {.blue = 9509, .green = 9509, .red = 9509, .alpha = 9509},
    /* 109 */ {.blue = 9444, .green = 9444, .red = 9444, .alpha = 9444},
    /* 110 */ {.blue = 9380, .green = 9380, .red = 9380, .alpha = 9380},
    /* 111 */ {.blue = 9316, .green = 9316, .red = 9316, .alpha = 9316},
    /* 112 */ {.blue = 9252, .green = 9252, .red = 9252, .alpha = 9252},
    /* 113 */ {.blue = 9187, .green = 9187, .red = 9187, .alpha = 9187},
    /* 114 */ {.blue = 9123, .green = 9123, .red = 9123, .alpha = 9123},
    /* 115 */ {.blue = 9059, .green = 9059, .red = 9059, .alpha = 9059},
    /* 116 */ {.blue = 8995, .green = 8995, .red = 8995, .alpha = 8995},
    /* 117 */ {.blue = 8930, .green = 8930, .red = 8930, .alpha = 8930},
    /* 118 */ {.blue = 8866, .green = 8866, .red = 8866, .alpha = 8866},
    /* 119 */ {.blue = 8802, .green = 8802, .red = 8802, .alpha = 8802},
    /* 120 */ {.blue = 8738, .green = 8738, .red = 8738, .alpha = 8738},
    /* 121 */ {.blue = 8673, .green = 8673, .red = 8673, .alpha = 8673},
    /* 122 */ {.blue = 8609, .green = 8609, .red = 8609, .alpha = 8609},
    /* 123 */ {.blue = 8545, .green = 8545, .red = 8545, .alpha = 8545},
    /* 124 */ {.blue = 8481, .green = 8481, .red = 8481, .alpha = 8481},
    /* 125 */ {.blue = 8416, .green = 8416, .red = 8416, .alpha = 8416},
    /* 126 */ {.blue = 8352, .green = 8352, .red = 8352, .alpha = 8352},
    /* 127 */ {.blue = 8288, .green = 8288, .red = 8288, .alpha = 8288},
    /* 128 */ {.blue = 8224, .green = 8224, .red = 8224, .alpha = 8224},
    /* 129 */ {.blue = 8159, .green = 8159, .red = 8159, .alpha = 8159},
    /* 130 */ {.blue = 8095, .green = 8095, .red = 8095, .alpha = 8095},
    /* 131 */ {.blue = 8031, .green = 8031, .red = 8031, .alpha = 8031},
    /* 132 */ {.blue = 7967, .green = 7967, .red = 7967, .alpha = 7967},
    /* 133 */ {.blue = 7902, .green = 7902, .red = 7902, .alpha = 7902},
    /* 134 */ {.blue = 7838, .green = 7838, .red = 7838, .alpha = 7838},
    /* 135 */ {.blue = 7774, .green = 7774, .red = 7774, .alpha = 7774},
    /* 136 */ {.blue = 7710, .green = 7710, .red = 7710, .alpha = 7710},
    /* 137 */ {.blue = 7645, .green = 7645, .red = 7645, .alpha = 7645},
    /* 138 */ {.blue = 7581, .green = 7581, .red = 7581, .alpha = 7581},
    /* 139 */ {.blue = 7517, .green = 7517, .red = 7517, .alpha = 7517},
    /* 140 */ {.blue = 7453, .green = 7453, .red = 7453, .alpha = 7453},
    /* 141 */ {.blue = 7388, .green = 7388, .red = 7388, .alpha = 7388},
    /* 142 */ {.blue = 7324, .green = 7324, .red = 7324, .alpha = 7324},
    /* 143 */ {.blue = 7260, .green = 7260, .red = 7260, .alpha = 7260},
    /* 144 */ {.blue = 7196, .green = 7196, .red = 7196, .alpha = 7196},
    /* 145 */ {.blue = 7131, .green = 7131, .red = 7131, .alpha = 7131},
    /* 146 */ {.blue = 7067, .green = 7067, .red = 7067, .alpha = 7067},
    /* 147 */ {.blue = 7003, .green = 7003, .red = 7003, .alpha = 7003},
    /* 148 */ {.blue = 6939, .green = 6939, .red = 6939, .alpha = 6939},
    /* 149 */ {.blue = 6874, .green = 6874, .red = 6874, .alpha = 6874},
    /* 150 */ {.blue = 6810, .green = 6810, .red = 6810, .alpha = 6810},
    /* 151 */ {.blue = 6746, .green = 6746, .red = 6746, .alpha = 6746},
    /* 152 */ {.blue = 6682, .green = 6682, .red = 6682, .alpha = 6682},
    /* 153 */ {.blue = 6617, .green = 6617, .red = 6617, .alpha = 6617},
    /* 154 */ {.blue = 6553, .green = 6553, .red = 6553, .alpha = 6553},
    /* 155 */ {.blue = 6489, .green = 6489, .red = 6489, .alpha = 6489},
    /* 156 */ {.blue = 6425, .green = 6425, .red = 6425, .alpha = 6425},
    /* 157 */ {.blue = 6360, .green = 6360, .red = 6360, .alpha = 6360},
    /* 158 */ {.blue = 6296, .green = 6296, .red = 6296, .alpha = 6296},
    /* 159 */ {.blue = 6232, .green = 6232, .red = 6232, .alpha = 6232},
    /* 160 */ {.blue = 6168, .green = 6168, .red = 6168, .alpha = 6168},
    /* 161 */ {.blue = 6103, .green = 6103, .red = 6103, .alpha = 6103},
    /* 162 */ {.blue = 6039, .green = 6039, .red = 6039, .alpha = 6039},
    /* 163 */ {.blue = 5975, .green = 5975, .red = 5975, .alpha = 5975},
    /* 164 */ {.blue = 5911, .green = 5911, .red = 5911, .alpha = 5911},
    /* 165 */ {.blue = 5846, .green = 5846, .red = 5846, .alpha = 5846},
    /* 166 */ {.blue = 5782, .green = 5782, .red = 5782, .alpha = 5782},
    /* 167 */ {.blue = 5718, .green = 5718, .red = 5718, .alpha = 5718},
    /* 168 */ {.blue = 5654, .green = 5654, .red = 5654, .alpha = 5654},
    /* 169 */ {.blue = 5589, .green = 5589, .red = 5589, .alpha = 5589},
    /* 170 */ {.blue = 5525, .green = 5525, .red = 5525, .alpha = 5525},
    /* 171 */ {.blue = 5461, .green = 5461, .red = 5461, .alpha = 5461},
    /* 172 */ {.blue = 5397, .green = 5397, .red = 5397, .alpha = 5397},
    /* 173 */ {.blue = 5332, .green = 5332, .red = 5332, .alpha = 5332},
    /* 174 */ {.blue = 5268, .green = 5268, .red = 5268, .alpha = 5268},
    /* 175 */ {.blue = 5204, .green = 5204, .red = 5204, .alpha = 5204},
    /* 176 */ {.blue = 5140, .green = 5140, .red = 5140, .alpha = 5140},
    /* 177 */ {.blue = 5075, .green = 5075, .red = 5075, .alpha = 5075},
    /* 178 */ {.blue = 5011, .green = 5011, .red = 5011, .alpha = 5011},
    /* 179 */ {.blue = 4947, .green = 4947, .red = 4947, .alpha = 4947},
    /* 180 */ {.blue = 4883, .green = 4883, .red = 4883, .alpha = 4883},
    /* 181 */ {.blue = 4818, .green = 4818, .red = 4818, .alpha = 4818},
    /* 182 */ {.blue = 4754, .green = 4754, .red = 4754, .alpha = 4754},
    /* 183 */ {.blue = 4690, .green = 4690, .red = 4690, .alpha = 4690},
    /* 184 */ {.blue = 4626, .green = 4626, .red = 4626, .alpha = 4626},
    /* 185 */ {.blue = 4561, .green = 4561, .red = 4561, .alpha = 4561},
    /* 186 */ {.blue = 4497, .green = 4497, .red = 4497, .alpha = 4497},
    /* 187 */ {.blue = 4433, .green = 4433, .red = 4433, .alpha = 4433},
    /* 188 */ {.blue = 4369, .green = 4369, .red = 4369, .alpha = 4369},
    /* 189 */ {.blue = 4304, .green = 4304, .red = 4304, .alpha = 4304},
    /* 190 */ {.blue = 4240, .green = 4240, .red = 4240, .alpha = 4240},
    /* 191 */ {.blue = 4176, .green = 4176, .red = 4176, .alpha = 4176},
    /* 192 */ {.blue = 4112, .green = 4112, .red = 4112, .alpha = 4112},
    /* 193 */ {.blue = 4047, .green = 4047, .red = 4047, .alpha = 4047},
    /* 194 */ {.blue = 3983, .green = 3983, .red = 3983, .alpha = 3983},
    /* 195 */ {.blue = 3919, .green = 3919, .red = 3919, .alpha = 3919},
    /* 196 */ {.blue = 3855, .green = 3855, .red = 3855, .alpha = 3855},
    /* 197 */ {.blue = 3790, .green = 3790, .red = 3790, .alpha = 3790},
    /* 198 */ {.blue = 3726, .green = 3726, .red = 3726, .alpha = 3726},
    /* 199 */ {.blue = 3662, .green = 3662, .red = 3662, .alpha = 3662},
    /* 200 */ {.blue = 3598, .green = 3598, .red = 3598, .alpha = 3598},
    /* 201 */ {.blue = 3533, .green = 3533, .red = 3533, .alpha = 3533},
    /* 202 */ {.blue = 3469, .green = 3469, .red = 3469, .alpha = 3469},
    /* 203 */ {.blue = 3405, .green = 3405, .red = 3405, .alpha = 3405},
    /* 204 */ {.blue = 3341, .green = 3341, .red = 3341, .alpha = 3341},
    /* 205 */ {.blue = 3276, .green = 3276, .red = 3276, .alpha = 3276},
    /* 206 */ {.blue = 3212, .green = 3212, .red = 3212, .alpha = 3212},
    /* 207 */ {.blue = 3148, .green = 3148, .red = 3148, .alpha = 3148},
    /* 208 */ {.blue = 3084, .green = 3084, .red = 3084, .alpha = 3084},
    /* 209 */ {.blue = 3019, .green = 3019, .red = 3019, .alpha = 3019},
    /* 210 */ {.blue = 2955, .green = 2955, .red = 2955, .alpha = 2955},
    /* 211 */ {.blue = 2891, .green = 2891, .red = 2891, .alpha = 2891},
    /* 212 */ {.blue = 2827, .green = 2827, .red = 2827, .alpha = 2827},
    /* 213 */ {.blue = 2762, .green = 2762, .red = 2762, .alpha = 2762},
    /* 214 */ {.blue = 2698, .green = 2698, .red = 2698, .alpha = 2698},
    /* 215 */ {.blue = 2634, .green = 2634, .red = 2634, .alpha = 2634},
    /* 216 */ {.blue = 2570, .green = 2570, .red = 2570, .alpha = 2570},
    /* 217 */ {.blue = 2505, .green = 2505, .red = 2505, .alpha = 2505},
    /* 218 */ {.blue = 2441, .green = 2441, .red = 2441, .alpha = 2441},
    /* 219 */ {.blue = 2377, .green = 2377, .red = 2377, .alpha = 2377},
    /* 220 */ {.blue = 2313, .green = 2313, .red = 2313, .alpha = 2313},
    /* 221 */ {.blue = 2248, .green = 2248, .red = 2248, .alpha = 2248},
    /* 222 */ {.blue = 2184, .green = 2184, .red = 2184, .alpha = 2184},
    /* 223 */ {.blue = 2120, .green = 2120, .red = 2120, .alpha = 2120},
    /* 224 */ {.blue = 2056, .green = 2056, .red = 2056, .alpha = 2056},
    /* 225 */ {.blue = 1991, .green = 1991, .red = 1991, .alpha = 1991},
    /* 226 */ {.blue = 1927, .green = 1927, .red = 1927, .alpha = 1927},
    /* 227 */ {.blue = 1863, .green = 1863, .red = 1863, .alpha = 1863},
    /* 228 */ {.blue = 1799, .green = 1799, .red = 1799, .alpha = 1799},
    /* 229 */ {.blue = 1734, .green = 1734, .red = 1734, .alpha = 1734},
    /* 230 */ {.blue = 1670, .green = 1670, .red = 1670, .alpha = 1670},
    /* 231 */ {.blue = 1606, .green = 1606, .red = 1606, .alpha = 1606},
    /* 232 */ {.blue = 1542, .green = 1542, .red = 1542, .alpha = 1542},
    /* 233 */ {.blue = 1477, .green = 1477, .red = 1477, .alpha = 1477},
    /* 234 */ {.blue = 1413, .green = 1413, .red = 1413, .alpha = 1413},
    /* 235 */ {.blue = 1349, .green = 1349, .red = 1349, .alpha = 1349},
    /* 236 */ {.blue = 1285, .green = 1285, .red = 1285, .alpha = 1285},
    /* 237 */ {.blue = 1220, .green = 1220, .red = 1220, .alpha = 1220},
    /* 238 */ {.blue = 1156, .green = 1156, .red = 1156, .alpha = 1156},
    /* 239 */ {.blue = 1092, .green = 1092, .red = 1092, .alpha = 1092},
    /* 240 */ {.blue = 1028, .green = 1028, .red = 1028, .alpha = 1028},
    /* 241 */ {.blue = 963, .green = 963, .red = 963, .alpha = 963},
    /* 242 */ {.blue = 899, .green = 899, .red = 899, .alpha = 899},
    /* 243 */ {.blue = 835, .green = 835, .red = 835, .alpha = 835},
    /* 244 */ {.blue = 771, .green = 771, .red = 771, .alpha = 771},
    /* 245 */ {.blue = 706, .green = 706, .red = 706, .alpha = 706},
    /* 246 */ {.blue = 642, .green = 642, .red = 642, .alpha = 642},
    /* 247 */ {.blue = 578, .green = 578, .red = 578, .alpha = 578},
    /* 248 */ {.blue = 514, .green = 514, .red = 514, .alpha = 514},
    /* 249 */ {.blue = 449, .green = 449, .red = 449, .alpha = 449},
    /* 250 */ {.blue = 385, .green = 385, .red = 385, .alpha = 385},
    /* 251 */ {.blue = 321, .green = 321, .red = 321, .alpha = 321},
    /* 252 */ {.blue = 257, .green = 257, .red = 257, .alpha = 257},
    /* 253 */ {.blue = 192, .green = 192, .red = 192, .alpha = 192},
    /* 254 */ {.blue = 128, .green = 128, .red = 128, .alpha = 128},
    /* 255 */ {.blue = 64, .green = 64, .red = 64, .alpha = 64},
    /* 256: Original quirk: WorldLightingRuntime_UpdateInterpolatedTerrainLighting reads index 256 when
       the lighting cycle phase is 0 (cosine exactly 1.0); in the original that read the first entry
       of g_SoftwareBlendAlphaFactors, which followed the table, so its value is kept here. */
    {0}};

/* 00421720 g_SoftwareBlendAlphaFactors */
__declspec(align(16)) SoftwareRgbWordLanes g_SoftwareBlendAlphaFactors[256] = {
    /*   0 */ {0},
    /*   1 */ {.blue = 64, .green = 64, .red = 64},
    /*   2 */ {.blue = 128, .green = 128, .red = 128},
    /*   3 */ {.blue = 192, .green = 192, .red = 192},
    /*   4 */ {.blue = 257, .green = 257, .red = 257},
    /*   5 */ {.blue = 321, .green = 321, .red = 321},
    /*   6 */ {.blue = 385, .green = 385, .red = 385},
    /*   7 */ {.blue = 449, .green = 449, .red = 449},
    /*   8 */ {.blue = 514, .green = 514, .red = 514},
    /*   9 */ {.blue = 578, .green = 578, .red = 578},
    /*  10 */ {.blue = 642, .green = 642, .red = 642},
    /*  11 */ {.blue = 706, .green = 706, .red = 706},
    /*  12 */ {.blue = 771, .green = 771, .red = 771},
    /*  13 */ {.blue = 835, .green = 835, .red = 835},
    /*  14 */ {.blue = 899, .green = 899, .red = 899},
    /*  15 */ {.blue = 963, .green = 963, .red = 963},
    /*  16 */ {.blue = 1028, .green = 1028, .red = 1028},
    /*  17 */ {.blue = 1092, .green = 1092, .red = 1092},
    /*  18 */ {.blue = 1156, .green = 1156, .red = 1156},
    /*  19 */ {.blue = 1220, .green = 1220, .red = 1220},
    /*  20 */ {.blue = 1285, .green = 1285, .red = 1285},
    /*  21 */ {.blue = 1349, .green = 1349, .red = 1349},
    /*  22 */ {.blue = 1413, .green = 1413, .red = 1413},
    /*  23 */ {.blue = 1477, .green = 1477, .red = 1477},
    /*  24 */ {.blue = 1542, .green = 1542, .red = 1542},
    /*  25 */ {.blue = 1606, .green = 1606, .red = 1606},
    /*  26 */ {.blue = 1670, .green = 1670, .red = 1670},
    /*  27 */ {.blue = 1734, .green = 1734, .red = 1734},
    /*  28 */ {.blue = 1799, .green = 1799, .red = 1799},
    /*  29 */ {.blue = 1863, .green = 1863, .red = 1863},
    /*  30 */ {.blue = 1927, .green = 1927, .red = 1927},
    /*  31 */ {.blue = 1991, .green = 1991, .red = 1991},
    /*  32 */ {.blue = 2056, .green = 2056, .red = 2056},
    /*  33 */ {.blue = 2120, .green = 2120, .red = 2120},
    /*  34 */ {.blue = 2184, .green = 2184, .red = 2184},
    /*  35 */ {.blue = 2248, .green = 2248, .red = 2248},
    /*  36 */ {.blue = 2313, .green = 2313, .red = 2313},
    /*  37 */ {.blue = 2377, .green = 2377, .red = 2377},
    /*  38 */ {.blue = 2441, .green = 2441, .red = 2441},
    /*  39 */ {.blue = 2505, .green = 2505, .red = 2505},
    /*  40 */ {.blue = 2570, .green = 2570, .red = 2570},
    /*  41 */ {.blue = 2634, .green = 2634, .red = 2634},
    /*  42 */ {.blue = 2698, .green = 2698, .red = 2698},
    /*  43 */ {.blue = 2762, .green = 2762, .red = 2762},
    /*  44 */ {.blue = 2827, .green = 2827, .red = 2827},
    /*  45 */ {.blue = 2891, .green = 2891, .red = 2891},
    /*  46 */ {.blue = 2955, .green = 2955, .red = 2955},
    /*  47 */ {.blue = 3019, .green = 3019, .red = 3019},
    /*  48 */ {.blue = 3084, .green = 3084, .red = 3084},
    /*  49 */ {.blue = 3148, .green = 3148, .red = 3148},
    /*  50 */ {.blue = 3212, .green = 3212, .red = 3212},
    /*  51 */ {.blue = 3276, .green = 3276, .red = 3276},
    /*  52 */ {.blue = 3341, .green = 3341, .red = 3341},
    /*  53 */ {.blue = 3405, .green = 3405, .red = 3405},
    /*  54 */ {.blue = 3469, .green = 3469, .red = 3469},
    /*  55 */ {.blue = 3533, .green = 3533, .red = 3533},
    /*  56 */ {.blue = 3598, .green = 3598, .red = 3598},
    /*  57 */ {.blue = 3662, .green = 3662, .red = 3662},
    /*  58 */ {.blue = 3726, .green = 3726, .red = 3726},
    /*  59 */ {.blue = 3790, .green = 3790, .red = 3790},
    /*  60 */ {.blue = 3855, .green = 3855, .red = 3855},
    /*  61 */ {.blue = 3919, .green = 3919, .red = 3919},
    /*  62 */ {.blue = 3983, .green = 3983, .red = 3983},
    /*  63 */ {.blue = 4047, .green = 4047, .red = 4047},
    /*  64 */ {.blue = 4112, .green = 4112, .red = 4112},
    /*  65 */ {.blue = 4176, .green = 4176, .red = 4176},
    /*  66 */ {.blue = 4240, .green = 4240, .red = 4240},
    /*  67 */ {.blue = 4304, .green = 4304, .red = 4304},
    /*  68 */ {.blue = 4369, .green = 4369, .red = 4369},
    /*  69 */ {.blue = 4433, .green = 4433, .red = 4433},
    /*  70 */ {.blue = 4497, .green = 4497, .red = 4497},
    /*  71 */ {.blue = 4561, .green = 4561, .red = 4561},
    /*  72 */ {.blue = 4626, .green = 4626, .red = 4626},
    /*  73 */ {.blue = 4690, .green = 4690, .red = 4690},
    /*  74 */ {.blue = 4754, .green = 4754, .red = 4754},
    /*  75 */ {.blue = 4818, .green = 4818, .red = 4818},
    /*  76 */ {.blue = 4883, .green = 4883, .red = 4883},
    /*  77 */ {.blue = 4947, .green = 4947, .red = 4947},
    /*  78 */ {.blue = 5011, .green = 5011, .red = 5011},
    /*  79 */ {.blue = 5075, .green = 5075, .red = 5075},
    /*  80 */ {.blue = 5140, .green = 5140, .red = 5140},
    /*  81 */ {.blue = 5204, .green = 5204, .red = 5204},
    /*  82 */ {.blue = 5268, .green = 5268, .red = 5268},
    /*  83 */ {.blue = 5332, .green = 5332, .red = 5332},
    /*  84 */ {.blue = 5397, .green = 5397, .red = 5397},
    /*  85 */ {.blue = 5461, .green = 5461, .red = 5461},
    /*  86 */ {.blue = 5525, .green = 5525, .red = 5525},
    /*  87 */ {.blue = 5589, .green = 5589, .red = 5589},
    /*  88 */ {.blue = 5654, .green = 5654, .red = 5654},
    /*  89 */ {.blue = 5718, .green = 5718, .red = 5718},
    /*  90 */ {.blue = 5782, .green = 5782, .red = 5782},
    /*  91 */ {.blue = 5846, .green = 5846, .red = 5846},
    /*  92 */ {.blue = 5911, .green = 5911, .red = 5911},
    /*  93 */ {.blue = 5975, .green = 5975, .red = 5975},
    /*  94 */ {.blue = 6039, .green = 6039, .red = 6039},
    /*  95 */ {.blue = 6103, .green = 6103, .red = 6103},
    /*  96 */ {.blue = 6168, .green = 6168, .red = 6168},
    /*  97 */ {.blue = 6232, .green = 6232, .red = 6232},
    /*  98 */ {.blue = 6296, .green = 6296, .red = 6296},
    /*  99 */ {.blue = 6360, .green = 6360, .red = 6360},
    /* 100 */ {.blue = 6425, .green = 6425, .red = 6425},
    /* 101 */ {.blue = 6489, .green = 6489, .red = 6489},
    /* 102 */ {.blue = 6553, .green = 6553, .red = 6553},
    /* 103 */ {.blue = 6617, .green = 6617, .red = 6617},
    /* 104 */ {.blue = 6682, .green = 6682, .red = 6682},
    /* 105 */ {.blue = 6746, .green = 6746, .red = 6746},
    /* 106 */ {.blue = 6810, .green = 6810, .red = 6810},
    /* 107 */ {.blue = 6874, .green = 6874, .red = 6874},
    /* 108 */ {.blue = 6939, .green = 6939, .red = 6939},
    /* 109 */ {.blue = 7003, .green = 7003, .red = 7003},
    /* 110 */ {.blue = 7067, .green = 7067, .red = 7067},
    /* 111 */ {.blue = 7131, .green = 7131, .red = 7131},
    /* 112 */ {.blue = 7196, .green = 7196, .red = 7196},
    /* 113 */ {.blue = 7260, .green = 7260, .red = 7260},
    /* 114 */ {.blue = 7324, .green = 7324, .red = 7324},
    /* 115 */ {.blue = 7388, .green = 7388, .red = 7388},
    /* 116 */ {.blue = 7453, .green = 7453, .red = 7453},
    /* 117 */ {.blue = 7517, .green = 7517, .red = 7517},
    /* 118 */ {.blue = 7581, .green = 7581, .red = 7581},
    /* 119 */ {.blue = 7645, .green = 7645, .red = 7645},
    /* 120 */ {.blue = 7710, .green = 7710, .red = 7710},
    /* 121 */ {.blue = 7774, .green = 7774, .red = 7774},
    /* 122 */ {.blue = 7838, .green = 7838, .red = 7838},
    /* 123 */ {.blue = 7902, .green = 7902, .red = 7902},
    /* 124 */ {.blue = 7967, .green = 7967, .red = 7967},
    /* 125 */ {.blue = 8031, .green = 8031, .red = 8031},
    /* 126 */ {.blue = 8095, .green = 8095, .red = 8095},
    /* 127 */ {.blue = 8159, .green = 8159, .red = 8159},
    /* 128 */ {.blue = 8224, .green = 8224, .red = 8224},
    /* 129 */ {.blue = 8288, .green = 8288, .red = 8288},
    /* 130 */ {.blue = 8352, .green = 8352, .red = 8352},
    /* 131 */ {.blue = 8416, .green = 8416, .red = 8416},
    /* 132 */ {.blue = 8481, .green = 8481, .red = 8481},
    /* 133 */ {.blue = 8545, .green = 8545, .red = 8545},
    /* 134 */ {.blue = 8609, .green = 8609, .red = 8609},
    /* 135 */ {.blue = 8673, .green = 8673, .red = 8673},
    /* 136 */ {.blue = 8738, .green = 8738, .red = 8738},
    /* 137 */ {.blue = 8802, .green = 8802, .red = 8802},
    /* 138 */ {.blue = 8866, .green = 8866, .red = 8866},
    /* 139 */ {.blue = 8930, .green = 8930, .red = 8930},
    /* 140 */ {.blue = 8995, .green = 8995, .red = 8995},
    /* 141 */ {.blue = 9059, .green = 9059, .red = 9059},
    /* 142 */ {.blue = 9123, .green = 9123, .red = 9123},
    /* 143 */ {.blue = 9187, .green = 9187, .red = 9187},
    /* 144 */ {.blue = 9252, .green = 9252, .red = 9252},
    /* 145 */ {.blue = 9316, .green = 9316, .red = 9316},
    /* 146 */ {.blue = 9380, .green = 9380, .red = 9380},
    /* 147 */ {.blue = 9444, .green = 9444, .red = 9444},
    /* 148 */ {.blue = 9509, .green = 9509, .red = 9509},
    /* 149 */ {.blue = 9573, .green = 9573, .red = 9573},
    /* 150 */ {.blue = 9637, .green = 9637, .red = 9637},
    /* 151 */ {.blue = 9701, .green = 9701, .red = 9701},
    /* 152 */ {.blue = 9766, .green = 9766, .red = 9766},
    /* 153 */ {.blue = 9830, .green = 9830, .red = 9830},
    /* 154 */ {.blue = 9894, .green = 9894, .red = 9894},
    /* 155 */ {.blue = 9958, .green = 9958, .red = 9958},
    /* 156 */ {.blue = 10023, .green = 10023, .red = 10023},
    /* 157 */ {.blue = 10087, .green = 10087, .red = 10087},
    /* 158 */ {.blue = 10151, .green = 10151, .red = 10151},
    /* 159 */ {.blue = 10215, .green = 10215, .red = 10215},
    /* 160 */ {.blue = 10280, .green = 10280, .red = 10280},
    /* 161 */ {.blue = 10344, .green = 10344, .red = 10344},
    /* 162 */ {.blue = 10408, .green = 10408, .red = 10408},
    /* 163 */ {.blue = 10472, .green = 10472, .red = 10472},
    /* 164 */ {.blue = 10537, .green = 10537, .red = 10537},
    /* 165 */ {.blue = 10601, .green = 10601, .red = 10601},
    /* 166 */ {.blue = 10665, .green = 10665, .red = 10665},
    /* 167 */ {.blue = 10729, .green = 10729, .red = 10729},
    /* 168 */ {.blue = 10794, .green = 10794, .red = 10794},
    /* 169 */ {.blue = 10858, .green = 10858, .red = 10858},
    /* 170 */ {.blue = 10922, .green = 10922, .red = 10922},
    /* 171 */ {.blue = 10986, .green = 10986, .red = 10986},
    /* 172 */ {.blue = 11051, .green = 11051, .red = 11051},
    /* 173 */ {.blue = 11115, .green = 11115, .red = 11115},
    /* 174 */ {.blue = 11179, .green = 11179, .red = 11179},
    /* 175 */ {.blue = 11243, .green = 11243, .red = 11243},
    /* 176 */ {.blue = 11308, .green = 11308, .red = 11308},
    /* 177 */ {.blue = 11372, .green = 11372, .red = 11372},
    /* 178 */ {.blue = 11436, .green = 11436, .red = 11436},
    /* 179 */ {.blue = 11500, .green = 11500, .red = 11500},
    /* 180 */ {.blue = 11565, .green = 11565, .red = 11565},
    /* 181 */ {.blue = 11629, .green = 11629, .red = 11629},
    /* 182 */ {.blue = 11693, .green = 11693, .red = 11693},
    /* 183 */ {.blue = 11757, .green = 11757, .red = 11757},
    /* 184 */ {.blue = 11822, .green = 11822, .red = 11822},
    /* 185 */ {.blue = 11886, .green = 11886, .red = 11886},
    /* 186 */ {.blue = 11950, .green = 11950, .red = 11950},
    /* 187 */ {.blue = 12014, .green = 12014, .red = 12014},
    /* 188 */ {.blue = 12079, .green = 12079, .red = 12079},
    /* 189 */ {.blue = 12143, .green = 12143, .red = 12143},
    /* 190 */ {.blue = 12207, .green = 12207, .red = 12207},
    /* 191 */ {.blue = 12271, .green = 12271, .red = 12271},
    /* 192 */ {.blue = 12336, .green = 12336, .red = 12336},
    /* 193 */ {.blue = 12400, .green = 12400, .red = 12400},
    /* 194 */ {.blue = 12464, .green = 12464, .red = 12464},
    /* 195 */ {.blue = 12528, .green = 12528, .red = 12528},
    /* 196 */ {.blue = 12593, .green = 12593, .red = 12593},
    /* 197 */ {.blue = 12657, .green = 12657, .red = 12657},
    /* 198 */ {.blue = 12721, .green = 12721, .red = 12721},
    /* 199 */ {.blue = 12785, .green = 12785, .red = 12785},
    /* 200 */ {.blue = 12850, .green = 12850, .red = 12850},
    /* 201 */ {.blue = 12914, .green = 12914, .red = 12914},
    /* 202 */ {.blue = 12978, .green = 12978, .red = 12978},
    /* 203 */ {.blue = 13042, .green = 13042, .red = 13042},
    /* 204 */ {.blue = 13107, .green = 13107, .red = 13107},
    /* 205 */ {.blue = 13171, .green = 13171, .red = 13171},
    /* 206 */ {.blue = 13235, .green = 13235, .red = 13235},
    /* 207 */ {.blue = 13299, .green = 13299, .red = 13299},
    /* 208 */ {.blue = 13364, .green = 13364, .red = 13364},
    /* 209 */ {.blue = 13428, .green = 13428, .red = 13428},
    /* 210 */ {.blue = 13492, .green = 13492, .red = 13492},
    /* 211 */ {.blue = 13556, .green = 13556, .red = 13556},
    /* 212 */ {.blue = 13621, .green = 13621, .red = 13621},
    /* 213 */ {.blue = 13685, .green = 13685, .red = 13685},
    /* 214 */ {.blue = 13749, .green = 13749, .red = 13749},
    /* 215 */ {.blue = 13813, .green = 13813, .red = 13813},
    /* 216 */ {.blue = 13878, .green = 13878, .red = 13878},
    /* 217 */ {.blue = 13942, .green = 13942, .red = 13942},
    /* 218 */ {.blue = 14006, .green = 14006, .red = 14006},
    /* 219 */ {.blue = 14070, .green = 14070, .red = 14070},
    /* 220 */ {.blue = 14135, .green = 14135, .red = 14135},
    /* 221 */ {.blue = 14199, .green = 14199, .red = 14199},
    /* 222 */ {.blue = 14263, .green = 14263, .red = 14263},
    /* 223 */ {.blue = 14327, .green = 14327, .red = 14327},
    /* 224 */ {.blue = 14392, .green = 14392, .red = 14392},
    /* 225 */ {.blue = 14456, .green = 14456, .red = 14456},
    /* 226 */ {.blue = 14520, .green = 14520, .red = 14520},
    /* 227 */ {.blue = 14584, .green = 14584, .red = 14584},
    /* 228 */ {.blue = 14649, .green = 14649, .red = 14649},
    /* 229 */ {.blue = 14713, .green = 14713, .red = 14713},
    /* 230 */ {.blue = 14777, .green = 14777, .red = 14777},
    /* 231 */ {.blue = 14841, .green = 14841, .red = 14841},
    /* 232 */ {.blue = 14906, .green = 14906, .red = 14906},
    /* 233 */ {.blue = 14970, .green = 14970, .red = 14970},
    /* 234 */ {.blue = 15034, .green = 15034, .red = 15034},
    /* 235 */ {.blue = 15098, .green = 15098, .red = 15098},
    /* 236 */ {.blue = 15163, .green = 15163, .red = 15163},
    /* 237 */ {.blue = 15227, .green = 15227, .red = 15227},
    /* 238 */ {.blue = 15291, .green = 15291, .red = 15291},
    /* 239 */ {.blue = 15355, .green = 15355, .red = 15355},
    /* 240 */ {.blue = 15420, .green = 15420, .red = 15420},
    /* 241 */ {.blue = 15484, .green = 15484, .red = 15484},
    /* 242 */ {.blue = 15548, .green = 15548, .red = 15548},
    /* 243 */ {.blue = 15612, .green = 15612, .red = 15612},
    /* 244 */ {.blue = 15677, .green = 15677, .red = 15677},
    /* 245 */ {.blue = 15741, .green = 15741, .red = 15741},
    /* 246 */ {.blue = 15805, .green = 15805, .red = 15805},
    /* 247 */ {.blue = 15869, .green = 15869, .red = 15869},
    /* 248 */ {.blue = 15934, .green = 15934, .red = 15934},
    /* 249 */ {.blue = 15998, .green = 15998, .red = 15998},
    /* 250 */ {.blue = 16062, .green = 16062, .red = 16062},
    /* 251 */ {.blue = 16126, .green = 16126, .red = 16126},
    /* 252 */ {.blue = 16191, .green = 16191, .red = 16191},
    /* 253 */ {.blue = 16255, .green = 16255, .red = 16255},
    /* 254 */ {.blue = 16319, .green = 16319, .red = 16319},
    /* 255 */ {.blue = 16383, .green = 16383, .red = 16383}};

/* 00421F20 g_SoftwareBlendInverseAlphaFactors */
__declspec(align(16)) SoftwareRgbWordLanes g_SoftwareBlendInverseAlphaFactors[256] = {
    /*   0 */ {.blue = 16448, .green = 16448, .red = 16448},
    /*   1 */ {.blue = 16383, .green = 16383, .red = 16383},
    /*   2 */ {.blue = 16319, .green = 16319, .red = 16319},
    /*   3 */ {.blue = 16255, .green = 16255, .red = 16255},
    /*   4 */ {.blue = 16191, .green = 16191, .red = 16191},
    /*   5 */ {.blue = 16126, .green = 16126, .red = 16126},
    /*   6 */ {.blue = 16062, .green = 16062, .red = 16062},
    /*   7 */ {.blue = 15998, .green = 15998, .red = 15998},
    /*   8 */ {.blue = 15934, .green = 15934, .red = 15934},
    /*   9 */ {.blue = 15869, .green = 15869, .red = 15869},
    /*  10 */ {.blue = 15805, .green = 15805, .red = 15805},
    /*  11 */ {.blue = 15741, .green = 15741, .red = 15741},
    /*  12 */ {.blue = 15677, .green = 15677, .red = 15677},
    /*  13 */ {.blue = 15612, .green = 15612, .red = 15612},
    /*  14 */ {.blue = 15548, .green = 15548, .red = 15548},
    /*  15 */ {.blue = 15484, .green = 15484, .red = 15484},
    /*  16 */ {.blue = 15420, .green = 15420, .red = 15420},
    /*  17 */ {.blue = 15355, .green = 15355, .red = 15355},
    /*  18 */ {.blue = 15291, .green = 15291, .red = 15291},
    /*  19 */ {.blue = 15227, .green = 15227, .red = 15227},
    /*  20 */ {.blue = 15163, .green = 15163, .red = 15163},
    /*  21 */ {.blue = 15098, .green = 15098, .red = 15098},
    /*  22 */ {.blue = 15034, .green = 15034, .red = 15034},
    /*  23 */ {.blue = 14970, .green = 14970, .red = 14970},
    /*  24 */ {.blue = 14906, .green = 14906, .red = 14906},
    /*  25 */ {.blue = 14841, .green = 14841, .red = 14841},
    /*  26 */ {.blue = 14777, .green = 14777, .red = 14777},
    /*  27 */ {.blue = 14713, .green = 14713, .red = 14713},
    /*  28 */ {.blue = 14649, .green = 14649, .red = 14649},
    /*  29 */ {.blue = 14584, .green = 14584, .red = 14584},
    /*  30 */ {.blue = 14520, .green = 14520, .red = 14520},
    /*  31 */ {.blue = 14456, .green = 14456, .red = 14456},
    /*  32 */ {.blue = 14392, .green = 14392, .red = 14392},
    /*  33 */ {.blue = 14327, .green = 14327, .red = 14327},
    /*  34 */ {.blue = 14263, .green = 14263, .red = 14263},
    /*  35 */ {.blue = 14199, .green = 14199, .red = 14199},
    /*  36 */ {.blue = 14135, .green = 14135, .red = 14135},
    /*  37 */ {.blue = 14070, .green = 14070, .red = 14070},
    /*  38 */ {.blue = 14006, .green = 14006, .red = 14006},
    /*  39 */ {.blue = 13942, .green = 13942, .red = 13942},
    /*  40 */ {.blue = 13878, .green = 13878, .red = 13878},
    /*  41 */ {.blue = 13813, .green = 13813, .red = 13813},
    /*  42 */ {.blue = 13749, .green = 13749, .red = 13749},
    /*  43 */ {.blue = 13685, .green = 13685, .red = 13685},
    /*  44 */ {.blue = 13621, .green = 13621, .red = 13621},
    /*  45 */ {.blue = 13556, .green = 13556, .red = 13556},
    /*  46 */ {.blue = 13492, .green = 13492, .red = 13492},
    /*  47 */ {.blue = 13428, .green = 13428, .red = 13428},
    /*  48 */ {.blue = 13364, .green = 13364, .red = 13364},
    /*  49 */ {.blue = 13299, .green = 13299, .red = 13299},
    /*  50 */ {.blue = 13235, .green = 13235, .red = 13235},
    /*  51 */ {.blue = 13171, .green = 13171, .red = 13171},
    /*  52 */ {.blue = 13107, .green = 13107, .red = 13107},
    /*  53 */ {.blue = 13042, .green = 13042, .red = 13042},
    /*  54 */ {.blue = 12978, .green = 12978, .red = 12978},
    /*  55 */ {.blue = 12914, .green = 12914, .red = 12914},
    /*  56 */ {.blue = 12850, .green = 12850, .red = 12850},
    /*  57 */ {.blue = 12785, .green = 12785, .red = 12785},
    /*  58 */ {.blue = 12721, .green = 12721, .red = 12721},
    /*  59 */ {.blue = 12657, .green = 12657, .red = 12657},
    /*  60 */ {.blue = 12593, .green = 12593, .red = 12593},
    /*  61 */ {.blue = 12528, .green = 12528, .red = 12528},
    /*  62 */ {.blue = 12464, .green = 12464, .red = 12464},
    /*  63 */ {.blue = 12400, .green = 12400, .red = 12400},
    /*  64 */ {.blue = 12336, .green = 12336, .red = 12336},
    /*  65 */ {.blue = 12271, .green = 12271, .red = 12271},
    /*  66 */ {.blue = 12207, .green = 12207, .red = 12207},
    /*  67 */ {.blue = 12143, .green = 12143, .red = 12143},
    /*  68 */ {.blue = 12079, .green = 12079, .red = 12079},
    /*  69 */ {.blue = 12014, .green = 12014, .red = 12014},
    /*  70 */ {.blue = 11950, .green = 11950, .red = 11950},
    /*  71 */ {.blue = 11886, .green = 11886, .red = 11886},
    /*  72 */ {.blue = 11822, .green = 11822, .red = 11822},
    /*  73 */ {.blue = 11757, .green = 11757, .red = 11757},
    /*  74 */ {.blue = 11693, .green = 11693, .red = 11693},
    /*  75 */ {.blue = 11629, .green = 11629, .red = 11629},
    /*  76 */ {.blue = 11565, .green = 11565, .red = 11565},
    /*  77 */ {.blue = 11500, .green = 11500, .red = 11500},
    /*  78 */ {.blue = 11436, .green = 11436, .red = 11436},
    /*  79 */ {.blue = 11372, .green = 11372, .red = 11372},
    /*  80 */ {.blue = 11308, .green = 11308, .red = 11308},
    /*  81 */ {.blue = 11243, .green = 11243, .red = 11243},
    /*  82 */ {.blue = 11179, .green = 11179, .red = 11179},
    /*  83 */ {.blue = 11115, .green = 11115, .red = 11115},
    /*  84 */ {.blue = 11051, .green = 11051, .red = 11051},
    /*  85 */ {.blue = 10986, .green = 10986, .red = 10986},
    /*  86 */ {.blue = 10922, .green = 10922, .red = 10922},
    /*  87 */ {.blue = 10858, .green = 10858, .red = 10858},
    /*  88 */ {.blue = 10794, .green = 10794, .red = 10794},
    /*  89 */ {.blue = 10729, .green = 10729, .red = 10729},
    /*  90 */ {.blue = 10665, .green = 10665, .red = 10665},
    /*  91 */ {.blue = 10601, .green = 10601, .red = 10601},
    /*  92 */ {.blue = 10537, .green = 10537, .red = 10537},
    /*  93 */ {.blue = 10472, .green = 10472, .red = 10472},
    /*  94 */ {.blue = 10408, .green = 10408, .red = 10408},
    /*  95 */ {.blue = 10344, .green = 10344, .red = 10344},
    /*  96 */ {.blue = 10280, .green = 10280, .red = 10280},
    /*  97 */ {.blue = 10215, .green = 10215, .red = 10215},
    /*  98 */ {.blue = 10151, .green = 10151, .red = 10151},
    /*  99 */ {.blue = 10087, .green = 10087, .red = 10087},
    /* 100 */ {.blue = 10023, .green = 10023, .red = 10023},
    /* 101 */ {.blue = 9958, .green = 9958, .red = 9958},
    /* 102 */ {.blue = 9894, .green = 9894, .red = 9894},
    /* 103 */ {.blue = 9830, .green = 9830, .red = 9830},
    /* 104 */ {.blue = 9766, .green = 9766, .red = 9766},
    /* 105 */ {.blue = 9701, .green = 9701, .red = 9701},
    /* 106 */ {.blue = 9637, .green = 9637, .red = 9637},
    /* 107 */ {.blue = 9573, .green = 9573, .red = 9573},
    /* 108 */ {.blue = 9509, .green = 9509, .red = 9509},
    /* 109 */ {.blue = 9444, .green = 9444, .red = 9444},
    /* 110 */ {.blue = 9380, .green = 9380, .red = 9380},
    /* 111 */ {.blue = 9316, .green = 9316, .red = 9316},
    /* 112 */ {.blue = 9252, .green = 9252, .red = 9252},
    /* 113 */ {.blue = 9187, .green = 9187, .red = 9187},
    /* 114 */ {.blue = 9123, .green = 9123, .red = 9123},
    /* 115 */ {.blue = 9059, .green = 9059, .red = 9059},
    /* 116 */ {.blue = 8995, .green = 8995, .red = 8995},
    /* 117 */ {.blue = 8930, .green = 8930, .red = 8930},
    /* 118 */ {.blue = 8866, .green = 8866, .red = 8866},
    /* 119 */ {.blue = 8802, .green = 8802, .red = 8802},
    /* 120 */ {.blue = 8738, .green = 8738, .red = 8738},
    /* 121 */ {.blue = 8673, .green = 8673, .red = 8673},
    /* 122 */ {.blue = 8609, .green = 8609, .red = 8609},
    /* 123 */ {.blue = 8545, .green = 8545, .red = 8545},
    /* 124 */ {.blue = 8481, .green = 8481, .red = 8481},
    /* 125 */ {.blue = 8416, .green = 8416, .red = 8416},
    /* 126 */ {.blue = 8352, .green = 8352, .red = 8352},
    /* 127 */ {.blue = 8288, .green = 8288, .red = 8288},
    /* 128 */ {.blue = 8224, .green = 8224, .red = 8224},
    /* 129 */ {.blue = 8159, .green = 8159, .red = 8159},
    /* 130 */ {.blue = 8095, .green = 8095, .red = 8095},
    /* 131 */ {.blue = 8031, .green = 8031, .red = 8031},
    /* 132 */ {.blue = 7967, .green = 7967, .red = 7967},
    /* 133 */ {.blue = 7902, .green = 7902, .red = 7902},
    /* 134 */ {.blue = 7838, .green = 7838, .red = 7838},
    /* 135 */ {.blue = 7774, .green = 7774, .red = 7774},
    /* 136 */ {.blue = 7710, .green = 7710, .red = 7710},
    /* 137 */ {.blue = 7645, .green = 7645, .red = 7645},
    /* 138 */ {.blue = 7581, .green = 7581, .red = 7581},
    /* 139 */ {.blue = 7517, .green = 7517, .red = 7517},
    /* 140 */ {.blue = 7453, .green = 7453, .red = 7453},
    /* 141 */ {.blue = 7388, .green = 7388, .red = 7388},
    /* 142 */ {.blue = 7324, .green = 7324, .red = 7324},
    /* 143 */ {.blue = 7260, .green = 7260, .red = 7260},
    /* 144 */ {.blue = 7196, .green = 7196, .red = 7196},
    /* 145 */ {.blue = 7131, .green = 7131, .red = 7131},
    /* 146 */ {.blue = 7067, .green = 7067, .red = 7067},
    /* 147 */ {.blue = 7003, .green = 7003, .red = 7003},
    /* 148 */ {.blue = 6939, .green = 6939, .red = 6939},
    /* 149 */ {.blue = 6874, .green = 6874, .red = 6874},
    /* 150 */ {.blue = 6810, .green = 6810, .red = 6810},
    /* 151 */ {.blue = 6746, .green = 6746, .red = 6746},
    /* 152 */ {.blue = 6682, .green = 6682, .red = 6682},
    /* 153 */ {.blue = 6617, .green = 6617, .red = 6617},
    /* 154 */ {.blue = 6553, .green = 6553, .red = 6553},
    /* 155 */ {.blue = 6489, .green = 6489, .red = 6489},
    /* 156 */ {.blue = 6425, .green = 6425, .red = 6425},
    /* 157 */ {.blue = 6360, .green = 6360, .red = 6360},
    /* 158 */ {.blue = 6296, .green = 6296, .red = 6296},
    /* 159 */ {.blue = 6232, .green = 6232, .red = 6232},
    /* 160 */ {.blue = 6168, .green = 6168, .red = 6168},
    /* 161 */ {.blue = 6103, .green = 6103, .red = 6103},
    /* 162 */ {.blue = 6039, .green = 6039, .red = 6039},
    /* 163 */ {.blue = 5975, .green = 5975, .red = 5975},
    /* 164 */ {.blue = 5911, .green = 5911, .red = 5911},
    /* 165 */ {.blue = 5846, .green = 5846, .red = 5846},
    /* 166 */ {.blue = 5782, .green = 5782, .red = 5782},
    /* 167 */ {.blue = 5718, .green = 5718, .red = 5718},
    /* 168 */ {.blue = 5654, .green = 5654, .red = 5654},
    /* 169 */ {.blue = 5589, .green = 5589, .red = 5589},
    /* 170 */ {.blue = 5525, .green = 5525, .red = 5525},
    /* 171 */ {.blue = 5461, .green = 5461, .red = 5461},
    /* 172 */ {.blue = 5397, .green = 5397, .red = 5397},
    /* 173 */ {.blue = 5332, .green = 5332, .red = 5332},
    /* 174 */ {.blue = 5268, .green = 5268, .red = 5268},
    /* 175 */ {.blue = 5204, .green = 5204, .red = 5204},
    /* 176 */ {.blue = 5140, .green = 5140, .red = 5140},
    /* 177 */ {.blue = 5075, .green = 5075, .red = 5075},
    /* 178 */ {.blue = 5011, .green = 5011, .red = 5011},
    /* 179 */ {.blue = 4947, .green = 4947, .red = 4947},
    /* 180 */ {.blue = 4883, .green = 4883, .red = 4883},
    /* 181 */ {.blue = 4818, .green = 4818, .red = 4818},
    /* 182 */ {.blue = 4754, .green = 4754, .red = 4754},
    /* 183 */ {.blue = 4690, .green = 4690, .red = 4690},
    /* 184 */ {.blue = 4626, .green = 4626, .red = 4626},
    /* 185 */ {.blue = 4561, .green = 4561, .red = 4561},
    /* 186 */ {.blue = 4497, .green = 4497, .red = 4497},
    /* 187 */ {.blue = 4433, .green = 4433, .red = 4433},
    /* 188 */ {.blue = 4369, .green = 4369, .red = 4369},
    /* 189 */ {.blue = 4304, .green = 4304, .red = 4304},
    /* 190 */ {.blue = 4240, .green = 4240, .red = 4240},
    /* 191 */ {.blue = 4176, .green = 4176, .red = 4176},
    /* 192 */ {.blue = 4112, .green = 4112, .red = 4112},
    /* 193 */ {.blue = 4047, .green = 4047, .red = 4047},
    /* 194 */ {.blue = 3983, .green = 3983, .red = 3983},
    /* 195 */ {.blue = 3919, .green = 3919, .red = 3919},
    /* 196 */ {.blue = 3855, .green = 3855, .red = 3855},
    /* 197 */ {.blue = 3790, .green = 3790, .red = 3790},
    /* 198 */ {.blue = 3726, .green = 3726, .red = 3726},
    /* 199 */ {.blue = 3662, .green = 3662, .red = 3662},
    /* 200 */ {.blue = 3598, .green = 3598, .red = 3598},
    /* 201 */ {.blue = 3533, .green = 3533, .red = 3533},
    /* 202 */ {.blue = 3469, .green = 3469, .red = 3469},
    /* 203 */ {.blue = 3405, .green = 3405, .red = 3405},
    /* 204 */ {.blue = 3341, .green = 3341, .red = 3341},
    /* 205 */ {.blue = 3276, .green = 3276, .red = 3276},
    /* 206 */ {.blue = 3212, .green = 3212, .red = 3212},
    /* 207 */ {.blue = 3148, .green = 3148, .red = 3148},
    /* 208 */ {.blue = 3084, .green = 3084, .red = 3084},
    /* 209 */ {.blue = 3019, .green = 3019, .red = 3019},
    /* 210 */ {.blue = 2955, .green = 2955, .red = 2955},
    /* 211 */ {.blue = 2891, .green = 2891, .red = 2891},
    /* 212 */ {.blue = 2827, .green = 2827, .red = 2827},
    /* 213 */ {.blue = 2762, .green = 2762, .red = 2762},
    /* 214 */ {.blue = 2698, .green = 2698, .red = 2698},
    /* 215 */ {.blue = 2634, .green = 2634, .red = 2634},
    /* 216 */ {.blue = 2570, .green = 2570, .red = 2570},
    /* 217 */ {.blue = 2505, .green = 2505, .red = 2505},
    /* 218 */ {.blue = 2441, .green = 2441, .red = 2441},
    /* 219 */ {.blue = 2377, .green = 2377, .red = 2377},
    /* 220 */ {.blue = 2313, .green = 2313, .red = 2313},
    /* 221 */ {.blue = 2248, .green = 2248, .red = 2248},
    /* 222 */ {.blue = 2184, .green = 2184, .red = 2184},
    /* 223 */ {.blue = 2120, .green = 2120, .red = 2120},
    /* 224 */ {.blue = 2056, .green = 2056, .red = 2056},
    /* 225 */ {.blue = 1991, .green = 1991, .red = 1991},
    /* 226 */ {.blue = 1927, .green = 1927, .red = 1927},
    /* 227 */ {.blue = 1863, .green = 1863, .red = 1863},
    /* 228 */ {.blue = 1799, .green = 1799, .red = 1799},
    /* 229 */ {.blue = 1734, .green = 1734, .red = 1734},
    /* 230 */ {.blue = 1670, .green = 1670, .red = 1670},
    /* 231 */ {.blue = 1606, .green = 1606, .red = 1606},
    /* 232 */ {.blue = 1542, .green = 1542, .red = 1542},
    /* 233 */ {.blue = 1477, .green = 1477, .red = 1477},
    /* 234 */ {.blue = 1413, .green = 1413, .red = 1413},
    /* 235 */ {.blue = 1349, .green = 1349, .red = 1349},
    /* 236 */ {.blue = 1285, .green = 1285, .red = 1285},
    /* 237 */ {.blue = 1220, .green = 1220, .red = 1220},
    /* 238 */ {.blue = 1156, .green = 1156, .red = 1156},
    /* 239 */ {.blue = 1092, .green = 1092, .red = 1092},
    /* 240 */ {.blue = 1028, .green = 1028, .red = 1028},
    /* 241 */ {.blue = 963, .green = 963, .red = 963},
    /* 242 */ {.blue = 899, .green = 899, .red = 899},
    /* 243 */ {.blue = 835, .green = 835, .red = 835},
    /* 244 */ {.blue = 771, .green = 771, .red = 771},
    /* 245 */ {.blue = 706, .green = 706, .red = 706},
    /* 246 */ {.blue = 642, .green = 642, .red = 642},
    /* 247 */ {.blue = 578, .green = 578, .red = 578},
    /* 248 */ {.blue = 514, .green = 514, .red = 514},
    /* 249 */ {.blue = 449, .green = 449, .red = 449},
    /* 250 */ {.blue = 385, .green = 385, .red = 385},
    /* 251 */ {.blue = 321, .green = 321, .red = 321},
    /* 252 */ {.blue = 257, .green = 257, .red = 257},
    /* 253 */ {.blue = 192, .green = 192, .red = 192},
    /* 254 */ {.blue = 128, .green = 128, .red = 128},
    /* 255 */ {.blue = 64, .green = 64, .red = 64}};

/* 00485824 g_GraphicsEndScene */
__declspec(align(4)) GraphicsEndSceneProc *g_GraphicsEndScene = (void *)SoftwareGraphicsDispatch_NoOp;

/* 00485850 g_PrimitiveDrawCallCount */
__declspec(align(16)) GraphicsDiagnosticCounter g_PrimitiveDrawCallCount = 0;

/* 00485854 g_TextureBindStateChangeCount */
__declspec(align(4)) GraphicsDiagnosticCounter g_TextureBindStateChangeCount = 0;

/* 00485858 g_TextureDeviceReloadCount */
__declspec(align(8)) GraphicsDiagnosticCounter g_TextureDeviceReloadCount = 0;

/* 004A8E80 g_SoftwarePixelPackTables */
__declspec(align(16)) SoftwarePixelPackTables *g_SoftwarePixelPackTables = 0;

/* 004A8E90 g_ActiveGraphicsAdapterIndex: uint32_t index into g_GraphicsAdapters of the active graphics adapter; 0xFFFFFFFF (GRAPHICS_ADAPTER_INDEX_NONE) before a display mode is set. */
__declspec(align(16)) uint32_t g_ActiveGraphicsAdapterIndex = 4294967295u;

/* 004A8E94 g_SoftwareColorScaleQ16 */
__declspec(align(4)) int32_t g_SoftwareColorScaleQ16 = 0x10000;

/* 004A8E98 g_SoftwareColorBiasQ16 */
__declspec(align(8)) int32_t g_SoftwareColorBiasQ16 = 0;

/* 004A8E9C g_GraphicsDisplayModes */
__declspec(align(4)) GraphicsDisplayMode *g_GraphicsDisplayModes = 0;

/* 004A8EA0 g_GraphicsDisplayModeCount */
__declspec(align(16)) GraphicsDisplayModeCount g_GraphicsDisplayModeCount = 0;

/* 004A8EA4 g_GraphicsAdapters */
__declspec(align(4)) GraphicsAdapterRecord *g_GraphicsAdapters = 0;

/* 004A8EA8 g_GraphicsAdapterCount */
__declspec(align(8)) uint32_t g_GraphicsAdapterCount = 0;

/* 004A8EAC g_SoftwarePixelFormatConfig */
__declspec(align(4)) SoftwarePixelFormatConfig g_SoftwarePixelFormatConfig = {0};

/* 004A8ED0 g_GraphicsSetDisplayMode */
__declspec(align(16)) SoftwareDisplayModeHookProc *g_GraphicsSetDisplayMode = (void *)SoftwarePixelFormat_BaseDisplayModeHook;

/* 004A8EDC g_SoftwareFramebufferDestroy: SoftwareFramebufferDestroyProc * hook slot, statically SoftwareFramebuffer_Destroy (graphics/backend/software.c). */
__declspec(align(4)) SoftwareFramebufferDestroyProc *g_SoftwareFramebufferDestroy = (void *)SoftwareFramebuffer_Destroy;

/* 004A8EE4 g_GraphicsFramebufferCaptureRegion */
__declspec(align(4)) GraphicsFramebufferCaptureRegionProc *g_GraphicsFramebufferCaptureRegion = 0;

/* 004A8EE8 g_SoftwareBuildPixelPackTables */
__declspec(align(8)) SoftwareBuildPixelPackTablesProc *g_SoftwareBuildPixelPackTables = (void *)SoftwarePixelFormat_BuildChannelPackTables;

/* 004A8F04 g_GraphicsTextureSourceBlitHalfSourceRgb */
__declspec(align(4)) GraphicsTextureSourceBlitProc *g_GraphicsTextureSourceBlitHalfSourceRgb = 0;

/* 004A8F0C g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha */
__declspec(align(4)) GraphicsTextureSourceBlitIntegerScaledSourceAlphaProc *g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha = 0;

/* 004A8F10 g_GraphicsTextureSourceStretchDirectColorBilinear */
__declspec(align(16)) GraphicsTextureSourceStretchDirectColorBilinearProc *g_GraphicsTextureSourceStretchDirectColorBilinear = 0;

/* 004A8F14 g_GraphicsTextureSourceBlitSourceAlphaPaletteBank */
__declspec(align(4)) GraphicsTextureSourceBlitSourceAlphaPaletteBankProc *g_GraphicsTextureSourceBlitSourceAlphaPaletteBank = 0;

/* 004A8F1C g_GraphicsTextureSourceBlitSaturatedAddRgb */
__declspec(align(4)) GraphicsTextureSourceSaturatedAddRgbProc *g_GraphicsTextureSourceBlitSaturatedAddRgb = 0;

/* 004A8F24 g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd */
__declspec(align(4)) GraphicsTextureSourceSaturatedAddRgbProc *g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd = 0;

/* 004A8F34 g_GraphicsFramebufferCopyRegionToOrigin: GraphicsFramebufferCopyRegionToOriginProc * hook slot, statically SoftwareFramebuffer_CopyRegionToOrigin (software.c). */
__declspec(align(4)) GraphicsFramebufferCopyRegionToOriginProc *g_GraphicsFramebufferCopyRegionToOrigin = (void *)SoftwareFramebuffer_CopyRegionToOrigin;

/* 004A8F38 g_GraphicsFramebufferCopyOriginToRegion: GraphicsFramebufferCopyOriginToRegionProc * hook slot, statically SoftwareFramebuffer_CopyOriginToRegion (software.c). */
__declspec(align(8)) GraphicsFramebufferCopyOriginToRegionProc *g_GraphicsFramebufferCopyOriginToRegion = (void *)SoftwareFramebuffer_CopyOriginToRegion;

/* 004D1234 g_SoftwareDepthRowStrideBytes */
__declspec(align(4)) uint32_t g_SoftwareDepthRowStrideBytes = 0;

/* 004D123C g_SoftwareAuxiliaryTargetBase */
__declspec(align(4)) void *g_SoftwareAuxiliaryTargetBase = 0;

/* 004D1240 g_SoftwareDepthEpoch */
__declspec(align(16)) int32_t g_SoftwareDepthEpoch = 0;

/* 004D1244 g_SoftwareChainedSetDisplayMode */
__declspec(align(4)) SoftwareDisplayModeHookProc *g_SoftwareChainedSetDisplayMode = 0;

/* 004D1248 g_SoftwareDrawQueue */
__declspec(align(8)) SoftwareDrawQueueProc *g_SoftwareDrawQueue = 0;

/* 004D1260 g_SoftwareRasterHandlers16Bit */
__declspec(align(16)) SoftwareRasterHandler *g_SoftwareRasterHandlers16Bit[64] = {
    /*  0 */ (void *)SoftwareRaster16_Mode00,
    /*  1 */ (void *)SoftwareRaster16_Mode01,
    /*  2 */ (void *)SoftwareRaster16_Mode02,
    /*  3 */ 0,
    /*  4 */ (void *)SoftwareRaster16_Mode04,
    /*  5 */ 0,
    /*  6 */ (void *)SoftwareRaster16_Mode06,
    /*  7 */ 0,
    /*  8 */ (void *)SoftwareRaster16_Mode08,
    /*  9 */ (void *)SoftwareRaster16_Mode09,
    /* 10 */ (void *)SoftwareRaster16_Mode10,
    /* 11 */ 0,
    /* 12 */ (void *)SoftwareRaster16_Mode12,
    /* 13 */ 0,
    /* 14 */ (void *)SoftwareRaster16_Mode14,
    /* 15 */ 0,
    /* 16 */ (void *)SoftwareRaster16_Mode16,
    /* 17 */ (void *)SoftwareRaster16_Mode17,
    /* 18 */ (void *)SoftwareRaster16_Mode18,
    /* 19 */ 0,
    /* 20 */ (void *)SoftwareRaster16_Mode20,
    /* 21 */ 0,
    /* 22 */ (void *)SoftwareRaster16_Mode22,
    /* 23 */ 0,
    /* 24 */ (void *)SoftwareRaster16_Mode24,
    /* 25 */ (void *)SoftwareRaster16_Mode25,
    /* 26 */ (void *)SoftwareRaster16_Mode26,
    /* 27 */ 0,
    /* 28 */ (void *)SoftwareRaster16_Mode28,
    /* 29 */ 0,
    /* 30 */ (void *)SoftwareRaster16_Mode30,
    /* 31 */ 0,
    /* 32 */ (void *)SoftwareRaster16_Mode01,
    /* 33 */ (void *)SoftwareRaster16_Mode01,
    /* 34 */ (void *)SoftwareRaster16_Mode02,
    /* 35 */ 0,
    /* 36 */ (void *)SoftwareRaster16_Mode01,
    /* 37 */ 0,
    /* 38 */ (void *)SoftwareRaster16_Mode01,
    /* 39 */ 0,
    /* 40 */ (void *)SoftwareRaster16_Mode09,
    /* 41 */ (void *)SoftwareRaster16_Mode09,
    /* 42 */ (void *)SoftwareRaster16_Mode10,
    /* 43 */ 0,
    /* 44 */ (void *)SoftwareRaster16_Mode09,
    /* 45 */ 0,
    /* 46 */ (void *)SoftwareRaster16_Mode09,
    /* 47 */ 0,
    /* 48 */ (void *)SoftwareRaster16_Mode17,
    /* 49 */ (void *)SoftwareRaster16_Mode17,
    /* 50 */ (void *)SoftwareRaster16_Mode18,
    /* 51 */ 0,
    /* 52 */ (void *)SoftwareRaster16_Mode17,
    /* 53 */ 0,
    /* 54 */ (void *)SoftwareRaster16_Mode17,
    /* 55 */ 0,
    /* 56 */ (void *)SoftwareRaster16_Mode25,
    /* 57 */ (void *)SoftwareRaster16_Mode25,
    /* 58 */ (void *)SoftwareRaster16_Mode26,
    /* 59 */ 0,
    /* 60 */ (void *)SoftwareRaster16_Mode25,
    /* 61 */ 0,
    /* 62 */ (void *)SoftwareRaster16_Mode25};

/* 004D1360 g_SoftwareRasterHandlersNon16Bit */
__declspec(align(16)) SoftwareRasterHandler *g_SoftwareRasterHandlersNon16Bit[64] = {
    /*  0 */ (void *)SoftwareRasterNon16_Mode00,
    /*  1 */ (void *)SoftwareRasterNon16_Mode01,
    /*  2 */ (void *)SoftwareRasterNon16_Mode02,
    /*  3 */ 0,
    /*  4 */ (void *)SoftwareRasterNon16_Mode04,
    /*  5 */ 0,
    /*  6 */ (void *)SoftwareRasterNon16_Mode06,
    /*  7 */ 0,
    /*  8 */ (void *)SoftwareRasterNon16_Mode08,
    /*  9 */ (void *)SoftwareRasterNon16_Mode09,
    /* 10 */ (void *)SoftwareRasterNon16_Mode10,
    /* 11 */ 0,
    /* 12 */ (void *)SoftwareRasterNon16_Mode12,
    /* 13 */ 0,
    /* 14 */ (void *)SoftwareRasterNon16_Mode14,
    /* 15 */ 0,
    /* 16 */ (void *)SoftwareRasterNon16_Mode16,
    /* 17 */ (void *)SoftwareRasterNon16_Mode17,
    /* 18 */ (void *)SoftwareRasterNon16_Mode18,
    /* 19 */ 0,
    /* 20 */ (void *)SoftwareRasterNon16_Mode20,
    /* 21 */ 0,
    /* 22 */ (void *)SoftwareRasterNon16_Mode22,
    /* 23 */ 0,
    /* 24 */ (void *)SoftwareRasterNon16_Mode24,
    /* 25 */ (void *)SoftwareRasterNon16_Mode25,
    /* 26 */ (void *)SoftwareRasterNon16_Mode26,
    /* 27 */ 0,
    /* 28 */ (void *)SoftwareRasterNon16_Mode28,
    /* 29 */ 0,
    /* 30 */ (void *)SoftwareRasterNon16_Mode30,
    /* 31 */ 0,
    /* 32 */ (void *)SoftwareRasterNon16_Mode01,
    /* 33 */ (void *)SoftwareRasterNon16_Mode01,
    /* 34 */ (void *)SoftwareRasterNon16_Mode02,
    /* 35 */ 0,
    /* 36 */ (void *)SoftwareRasterNon16_Mode01,
    /* 37 */ 0,
    /* 38 */ (void *)SoftwareRasterNon16_Mode01,
    /* 39 */ 0,
    /* 40 */ (void *)SoftwareRasterNon16_Mode09,
    /* 41 */ (void *)SoftwareRasterNon16_Mode09,
    /* 42 */ (void *)SoftwareRasterNon16_Mode10,
    /* 43 */ 0,
    /* 44 */ (void *)SoftwareRasterNon16_Mode09,
    /* 45 */ 0,
    /* 46 */ (void *)SoftwareRasterNon16_Mode09,
    /* 47 */ 0,
    /* 48 */ (void *)SoftwareRasterNon16_Mode17,
    /* 49 */ (void *)SoftwareRasterNon16_Mode17,
    /* 50 */ (void *)SoftwareRasterNon16_Mode18,
    /* 51 */ 0,
    /* 52 */ (void *)SoftwareRasterNon16_Mode17,
    /* 53 */ 0,
    /* 54 */ (void *)SoftwareRasterNon16_Mode17,
    /* 55 */ 0,
    /* 56 */ (void *)SoftwareRasterNon16_Mode25,
    /* 57 */ (void *)SoftwareRasterNon16_Mode25,
    /* 58 */ (void *)SoftwareRasterNon16_Mode26,
    /* 59 */ 0,
    /* 60 */ (void *)SoftwareRasterNon16_Mode25,
    /* 61 */ 0,
    /* 62 */ (void *)SoftwareRasterNon16_Mode25};

/* 004D1460 g_SoftwareRasterHandlersAuxiliary */
__declspec(align(16)) SoftwareRasterHandler *g_SoftwareRasterHandlersAuxiliary[64] = {
    /*  0 */ (void *)SoftwareRasterAux_Mode00,
    /*  1 */ (void *)SoftwareRasterAux_Mode01,
    /*  2 */ (void *)SoftwareRasterAux_Mode02,
    /*  3 */ 0,
    /*  4 */ (void *)SoftwareRasterAux_Mode04,
    /*  5 */ 0,
    /*  6 */ (void *)SoftwareRasterAux_Mode06,
    /*  7 */ 0,
    /*  8 */ (void *)SoftwareRasterAux_Mode08,
    /*  9 */ (void *)SoftwareRasterAux_Mode09,
    /* 10 */ (void *)SoftwareRasterAux_Mode10,
    /* 11 */ 0,
    /* 12 */ (void *)SoftwareRasterAux_Mode12,
    /* 13 */ 0,
    /* 14 */ (void *)SoftwareRasterAux_Mode14,
    /* 15 */ 0,
    /* 16 */ (void *)SoftwareRasterAux_Mode16,
    /* 17 */ (void *)SoftwareRasterAux_Mode17,
    /* 18 */ (void *)SoftwareRasterAux_Mode18,
    /* 19 */ 0,
    /* 20 */ (void *)SoftwareRasterAux_Mode20,
    /* 21 */ 0,
    /* 22 */ (void *)SoftwareRasterAux_Mode22,
    /* 23 */ 0,
    /* 24 */ (void *)SoftwareRasterAux_Mode24,
    /* 25 */ (void *)SoftwareRasterAux_Mode25,
    /* 26 */ (void *)SoftwareRasterAux_Mode26,
    /* 27 */ 0,
    /* 28 */ (void *)SoftwareRasterAux_Mode28,
    /* 29 */ 0,
    /* 30 */ (void *)SoftwareRasterAux_Mode30,
    /* 31 */ 0,
    /* 32 */ (void *)SoftwareRasterAux_Mode01,
    /* 33 */ (void *)SoftwareRasterAux_Mode01,
    /* 34 */ (void *)SoftwareRasterAux_Mode02,
    /* 35 */ 0,
    /* 36 */ (void *)SoftwareRasterAux_Mode01,
    /* 37 */ 0,
    /* 38 */ (void *)SoftwareRasterAux_Mode01,
    /* 39 */ 0,
    /* 40 */ (void *)SoftwareRasterAux_Mode09,
    /* 41 */ (void *)SoftwareRasterAux_Mode09,
    /* 42 */ (void *)SoftwareRasterAux_Mode10,
    /* 43 */ 0,
    /* 44 */ (void *)SoftwareRasterAux_Mode09,
    /* 45 */ 0,
    /* 46 */ (void *)SoftwareRasterAux_Mode09,
    /* 47 */ 0,
    /* 48 */ (void *)SoftwareRasterAux_Mode17,
    /* 49 */ (void *)SoftwareRasterAux_Mode17,
    /* 50 */ (void *)SoftwareRasterAux_Mode18,
    /* 51 */ 0,
    /* 52 */ (void *)SoftwareRasterAux_Mode17,
    /* 53 */ 0,
    /* 54 */ (void *)SoftwareRasterAux_Mode17,
    /* 55 */ 0,
    /* 56 */ (void *)SoftwareRasterAux_Mode25,
    /* 57 */ (void *)SoftwareRasterAux_Mode25,
    /* 58 */ (void *)SoftwareRasterAux_Mode26,
    /* 59 */ 0,
    /* 60 */ (void *)SoftwareRasterAux_Mode25,
    /* 61 */ 0,
    /* 62 */ (void *)SoftwareRasterAux_Mode25};

/* 00518080 g_SoftwareBilinearPackedInterpolationWeights256: int16_t[256][4] MMX word lanes per 8-bit fraction f: lane0 = 0x4040 - 0x40*f, lane1 = 0x40*f (sum 0x4040), lanes 2/3 zero; PMADDWD horizontal weights of SoftwareTexture_SampleIntensity (graphics/backend/software.c) */
__declspec(align(16)) int16_t g_SoftwareBilinearPackedInterpolationWeights256[256][4] = {
    {16448, 0, 0, 0},
    {16383, 64, 0, 0},
    {16319, 128, 0, 0},
    {16255, 192, 0, 0},
    {16191, 257, 0, 0},
    {16126, 321, 0, 0},
    {16062, 385, 0, 0},
    {15998, 449, 0, 0},
    {15934, 514, 0, 0},
    {15869, 578, 0, 0},
    {15805, 642, 0, 0},
    {15741, 706, 0, 0},
    {15677, 771, 0, 0},
    {15612, 835, 0, 0},
    {15548, 899, 0, 0},
    {15484, 963, 0, 0},
    {15420, 1028, 0, 0},
    {15355, 1092, 0, 0},
    {15291, 1156, 0, 0},
    {15227, 1220, 0, 0},
    {15163, 1285, 0, 0},
    {15098, 1349, 0, 0},
    {15034, 1413, 0, 0},
    {14970, 1477, 0, 0},
    {14906, 1542, 0, 0},
    {14841, 1606, 0, 0},
    {14777, 1670, 0, 0},
    {14713, 1734, 0, 0},
    {14649, 1799, 0, 0},
    {14584, 1863, 0, 0},
    {14520, 1927, 0, 0},
    {14456, 1991, 0, 0},
    {14392, 2056, 0, 0},
    {14327, 2120, 0, 0},
    {14263, 2184, 0, 0},
    {14199, 2248, 0, 0},
    {14135, 2313, 0, 0},
    {14070, 2377, 0, 0},
    {14006, 2441, 0, 0},
    {13942, 2505, 0, 0},
    {13878, 2570, 0, 0},
    {13813, 2634, 0, 0},
    {13749, 2698, 0, 0},
    {13685, 2762, 0, 0},
    {13621, 2827, 0, 0},
    {13556, 2891, 0, 0},
    {13492, 2955, 0, 0},
    {13428, 3019, 0, 0},
    {13364, 3084, 0, 0},
    {13299, 3148, 0, 0},
    {13235, 3212, 0, 0},
    {13171, 3276, 0, 0},
    {13107, 3341, 0, 0},
    {13042, 3405, 0, 0},
    {12978, 3469, 0, 0},
    {12914, 3533, 0, 0},
    {12850, 3598, 0, 0},
    {12785, 3662, 0, 0},
    {12721, 3726, 0, 0},
    {12657, 3790, 0, 0},
    {12593, 3855, 0, 0},
    {12528, 3919, 0, 0},
    {12464, 3983, 0, 0},
    {12400, 4047, 0, 0},
    {12336, 4112, 0, 0},
    {12271, 4176, 0, 0},
    {12207, 4240, 0, 0},
    {12143, 4304, 0, 0},
    {12079, 4369, 0, 0},
    {12014, 4433, 0, 0},
    {11950, 4497, 0, 0},
    {11886, 4561, 0, 0},
    {11822, 4626, 0, 0},
    {11757, 4690, 0, 0},
    {11693, 4754, 0, 0},
    {11629, 4818, 0, 0},
    {11565, 4883, 0, 0},
    {11500, 4947, 0, 0},
    {11436, 5011, 0, 0},
    {11372, 5075, 0, 0},
    {11308, 5140, 0, 0},
    {11243, 5204, 0, 0},
    {11179, 5268, 0, 0},
    {11115, 5332, 0, 0},
    {11051, 5397, 0, 0},
    {10986, 5461, 0, 0},
    {10922, 5525, 0, 0},
    {10858, 5589, 0, 0},
    {10794, 5654, 0, 0},
    {10729, 5718, 0, 0},
    {10665, 5782, 0, 0},
    {10601, 5846, 0, 0},
    {10537, 5911, 0, 0},
    {10472, 5975, 0, 0},
    {10408, 6039, 0, 0},
    {10344, 6103, 0, 0},
    {10280, 6168, 0, 0},
    {10215, 6232, 0, 0},
    {10151, 6296, 0, 0},
    {10087, 6360, 0, 0},
    {10023, 6425, 0, 0},
    {9958, 6489, 0, 0},
    {9894, 6553, 0, 0},
    {9830, 6617, 0, 0},
    {9766, 6682, 0, 0},
    {9701, 6746, 0, 0},
    {9637, 6810, 0, 0},
    {9573, 6874, 0, 0},
    {9509, 6939, 0, 0},
    {9444, 7003, 0, 0},
    {9380, 7067, 0, 0},
    {9316, 7131, 0, 0},
    {9252, 7196, 0, 0},
    {9187, 7260, 0, 0},
    {9123, 7324, 0, 0},
    {9059, 7388, 0, 0},
    {8995, 7453, 0, 0},
    {8930, 7517, 0, 0},
    {8866, 7581, 0, 0},
    {8802, 7645, 0, 0},
    {8738, 7710, 0, 0},
    {8673, 7774, 0, 0},
    {8609, 7838, 0, 0},
    {8545, 7902, 0, 0},
    {8481, 7967, 0, 0},
    {8416, 8031, 0, 0},
    {8352, 8095, 0, 0},
    {8288, 8159, 0, 0},
    {8224, 8224, 0, 0},
    {8159, 8288, 0, 0},
    {8095, 8352, 0, 0},
    {8031, 8416, 0, 0},
    {7967, 8481, 0, 0},
    {7902, 8545, 0, 0},
    {7838, 8609, 0, 0},
    {7774, 8673, 0, 0},
    {7710, 8738, 0, 0},
    {7645, 8802, 0, 0},
    {7581, 8866, 0, 0},
    {7517, 8930, 0, 0},
    {7453, 8995, 0, 0},
    {7388, 9059, 0, 0},
    {7324, 9123, 0, 0},
    {7260, 9187, 0, 0},
    {7196, 9252, 0, 0},
    {7131, 9316, 0, 0},
    {7067, 9380, 0, 0},
    {7003, 9444, 0, 0},
    {6939, 9509, 0, 0},
    {6874, 9573, 0, 0},
    {6810, 9637, 0, 0},
    {6746, 9701, 0, 0},
    {6682, 9766, 0, 0},
    {6617, 9830, 0, 0},
    {6553, 9894, 0, 0},
    {6489, 9958, 0, 0},
    {6425, 10023, 0, 0},
    {6360, 10087, 0, 0},
    {6296, 10151, 0, 0},
    {6232, 10215, 0, 0},
    {6168, 10280, 0, 0},
    {6103, 10344, 0, 0},
    {6039, 10408, 0, 0},
    {5975, 10472, 0, 0},
    {5911, 10537, 0, 0},
    {5846, 10601, 0, 0},
    {5782, 10665, 0, 0},
    {5718, 10729, 0, 0},
    {5654, 10794, 0, 0},
    {5589, 10858, 0, 0},
    {5525, 10922, 0, 0},
    {5461, 10986, 0, 0},
    {5397, 11051, 0, 0},
    {5332, 11115, 0, 0},
    {5268, 11179, 0, 0},
    {5204, 11243, 0, 0},
    {5140, 11308, 0, 0},
    {5075, 11372, 0, 0},
    {5011, 11436, 0, 0},
    {4947, 11500, 0, 0},
    {4883, 11565, 0, 0},
    {4818, 11629, 0, 0},
    {4754, 11693, 0, 0},
    {4690, 11757, 0, 0},
    {4626, 11822, 0, 0},
    {4561, 11886, 0, 0},
    {4497, 11950, 0, 0},
    {4433, 12014, 0, 0},
    {4369, 12079, 0, 0},
    {4304, 12143, 0, 0},
    {4240, 12207, 0, 0},
    {4176, 12271, 0, 0},
    {4112, 12336, 0, 0},
    {4047, 12400, 0, 0},
    {3983, 12464, 0, 0},
    {3919, 12528, 0, 0},
    {3855, 12593, 0, 0},
    {3790, 12657, 0, 0},
    {3726, 12721, 0, 0},
    {3662, 12785, 0, 0},
    {3598, 12850, 0, 0},
    {3533, 12914, 0, 0},
    {3469, 12978, 0, 0},
    {3405, 13042, 0, 0},
    {3341, 13107, 0, 0},
    {3276, 13171, 0, 0},
    {3212, 13235, 0, 0},
    {3148, 13299, 0, 0},
    {3084, 13364, 0, 0},
    {3019, 13428, 0, 0},
    {2955, 13492, 0, 0},
    {2891, 13556, 0, 0},
    {2827, 13621, 0, 0},
    {2762, 13685, 0, 0},
    {2698, 13749, 0, 0},
    {2634, 13813, 0, 0},
    {2570, 13878, 0, 0},
    {2505, 13942, 0, 0},
    {2441, 14006, 0, 0},
    {2377, 14070, 0, 0},
    {2313, 14135, 0, 0},
    {2248, 14199, 0, 0},
    {2184, 14263, 0, 0},
    {2120, 14327, 0, 0},
    {2056, 14392, 0, 0},
    {1991, 14456, 0, 0},
    {1927, 14520, 0, 0},
    {1863, 14584, 0, 0},
    {1799, 14649, 0, 0},
    {1734, 14713, 0, 0},
    {1670, 14777, 0, 0},
    {1606, 14841, 0, 0},
    {1542, 14906, 0, 0},
    {1477, 14970, 0, 0},
    {1413, 15034, 0, 0},
    {1349, 15098, 0, 0},
    {1285, 15163, 0, 0},
    {1220, 15227, 0, 0},
    {1156, 15291, 0, 0},
    {1092, 15355, 0, 0},
    {1028, 15420, 0, 0},
    {963, 15484, 0, 0},
    {899, 15548, 0, 0},
    {835, 15612, 0, 0},
    {771, 15677, 0, 0},
    {706, 15741, 0, 0},
    {642, 15805, 0, 0},
    {578, 15869, 0, 0},
    {514, 15934, 0, 0},
    {449, 15998, 0, 0},
    {385, 16062, 0, 0},
    {321, 16126, 0, 0},
    {257, 16191, 0, 0},
    {192, 16255, 0, 0},
    {128, 16319, 0, 0},
    {64, 16383, 0, 0}};

/* 00518880 g_SoftwarePixelIntensityToNativeColorLut256 */
__declspec(align(16)) uint32_t g_SoftwarePixelIntensityToNativeColorLut256[256] = {0};

/* 00518C80 g_SoftwareBlendUnityWordLanesQ14 */
__declspec(align(16)) uint64_t g_SoftwareBlendUnityWordLanesQ14 = 0x4000400040004000ull;

/* 00573FD8 g_GlideImportBindings */
__declspec(align(8)) GlideImportBinding g_GlideImportBindings[89] = {
    /*  0 */ {.importName = (void *)g_GlideImportName_grAADrawTriangle},
    /*  1 */ {.importName = (void *)g_GlideImportName_grAlphaBlendFunction},
    /*  2 */ {.importName = (void *)g_GlideImportName_grAlphaCombine},
    /*  3 */ {.importName = (void *)g_GlideImportName_grAlphaControlsITRGBLighting},
    /*  4 */ {.importName = (void *)g_GlideImportName_grAlphaTestFunction},
    /*  5 */ {.importName = (void *)g_GlideImportName_grAlphaTestReferenceValue},
    /*  6 */ {.importName = (void *)g_GlideImportName_grBufferClear},
    /*  7 */ {.importName = (void *)g_GlideImportName_grBufferSwap},
    /*  8 */ {.importName = (void *)g_GlideImportName_grChromakeyMode},
    /*  9 */ {.importName = (void *)g_GlideImportName_grChromakeyValue},
    /* 10 */ {.importName = g_GlideImportName_grClipWindow},
    /* 11 */ {.importName = g_GlideImportName_grColorCombine},
    /* 12 */ {.importName = g_GlideImportName_grColorMask},
    /* 13 */ {.importName = g_GlideImportName_grConstantColorValue},
    /* 14 */ {.importName = g_GlideImportName_grCoordinateSpace},
    /* 15 */ {.importName = g_GlideImportName_grCullMode},
    /* 16 */ {.importName = g_GlideImportName_grDepthBiasLevel},
    /* 17 */ {.importName = g_GlideImportName_grDepthBufferFunction},
    /* 18 */ {.importName = g_GlideImportName_grDepthBufferMode},
    /* 19 */ {.importName = g_GlideImportName_grDepthMask},
    /* 20 */ {.importName = g_GlideImportName_grDepthRange},
    /* 21 */ {.importName = g_GlideImportName_grDisable},
    /* 22 */ {.importName = g_GlideImportName_grDisableAllEffects},
    /* 23 */ {.importName = g_GlideImportName_grDitherMode},
    /* 24 */ {.importName = g_GlideImportName_grDrawLine},
    /* 25 */ {.importName = g_GlideImportName_grDrawPoint},
    /* 26 */ {.importName = g_GlideImportName_grDrawTriangle},
    /* 27 */ {.importName = g_GlideImportName_grDrawVertexArray},
    /* 28 */ {.importName = g_GlideImportName_grDrawVertexArrayContiguous},
    /* 29 */ {.importName = g_GlideImportName_grEnable},
    /* 30 */ {.importName = g_GlideImportName_grErrorSetCallback},
    /* 31 */ {.importName = g_GlideImportName_grFinish},
    /* 32 */ {.importName = g_GlideImportName_grFlush},
    /* 33 */ {.importName = g_GlideImportName_grFogColorValue},
    /* 34 */ {.importName = g_GlideImportName_grFogMode},
    /* 35 */ {.importName = g_GlideImportName_grFogTable},
    /* 36 */ {.importName = (void *)g_GlideImportName_grGet},
    /* 37 */ {.importName = (void *)g_GlideImportName_grGetProcAddress},
    /* 38 */ {.importName = (void *)g_GlideImportName_grGetString},
    /* 39 */ {.importName = (void *)g_GlideImportName_grGlideGetState},
    /* 40 */ {.importName = (void *)g_GlideImportName_grGlideGetVertexLayout},
    /* 41 */ {.importName = (void *)g_GlideImportName_grGlideInit},
    /* 42 */ {.importName = (void *)g_GlideImportName_grGlideSetState},
    /* 43 */ {.importName = (void *)g_GlideImportName_grGlideSetVertexLayout},
    /* 44 */ {.importName = (void *)g_GlideImportName_grGlideShutdown},
    /* 45 */ {.importName = (void *)g_GlideImportName_grLfbConstantAlpha},
    /* 46 */ {.importName = (void *)g_GlideImportName_grLfbConstantDepth},
    /* 47 */ {.importName = (void *)g_GlideImportName_grLfbLock},
    /* 48 */ {.importName = (void *)g_GlideImportName_grLfbReadRegion},
    /* 49 */ {.importName = (void *)g_GlideImportName_grLfbUnlock},
    /* 50 */ {.importName = (void *)g_GlideImportName_grLfbWriteRegion},
    /* 51 */ {.importName = (void *)g_GlideImportName_grLoadGammaTable},
    /* 52 */ {.importName = (void *)g_GlideImportName_grQueryResolutions},
    /* 53 */ {.importName = (void *)g_GlideImportName_grRenderBuffer},
    /* 54 */ {.importName = (void *)g_GlideImportName_grReset},
    /* 55 */ {.importName = (void *)g_GlideImportName_grSelectContext},
    /* 56 */ {.importName = (void *)g_GlideImportName_grSstOrigin},
    /* 57 */ {.importName = (void *)g_GlideImportName_grSstSelect},
    /* 58 */ {.importName = (void *)g_GlideImportName_grSstWinClose},
    /* 59 */ {.importName = (void *)g_GlideImportName_grSstWinOpen},
    /* 60 */ {.importName = (void *)g_GlideImportName_grTexCalcMemRequired},
    /* 61 */ {.importName = (void *)g_GlideImportName_grTexClampMode},
    /* 62 */ {.importName = (void *)g_GlideImportName_grTexCombine},
    /* 63 */ {.importName = (void *)g_GlideImportName_grTexDetailControl},
    /* 64 */ {.importName = (void *)g_GlideImportName_grTexDownloadMipMap},
    /* 65 */ {.importName = (void *)g_GlideImportName_grTexDownloadMipMapLevel},
    /* 66 */ {.importName = (void *)g_GlideImportName_grTexDownloadMipMapLevelPartial},
    /* 67 */ {.importName = (void *)g_GlideImportName_grTexDownloadTable},
    /* 68 */ {.importName = (void *)g_GlideImportName_grTexDownloadTablePartial},
    /* 69 */ {.importName = (void *)g_GlideImportName_grTexFilterMode},
    /* 70 */ {.importName = (void *)g_GlideImportName_grTexLodBiasValue},
    /* 71 */ {.importName = (void *)g_GlideImportName_grTexMaxAddress},
    /* 72 */ {.importName = (void *)g_GlideImportName_grTexMinAddress},
    /* 73 */ {.importName = (void *)g_GlideImportName_grTexMipMapMode},
    /* 74 */ {.importName = (void *)g_GlideImportName_grTexMultibase},
    /* 75 */ {.importName = (void *)g_GlideImportName_grTexMultibaseAddress},
    /* 76 */ {.importName = (void *)g_GlideImportName_grTexNCCTable},
    /* 77 */ {.importName = (void *)g_GlideImportName_grTexSource},
    /* 78 */ {.importName = (void *)g_GlideImportName_grTexTextureMemRequired},
    /* 79 */ {.importName = (void *)g_GlideImportName_grVertexLayout},
    /* 80 */ {.importName = (void *)g_GlideImportName_grViewport},
    /* 81 */ {.importName = (void *)g_GlideImportName_gu3dfGetInfo},
    /* 82 */ {.importName = (void *)g_GlideImportName_gu3dfLoad},
    /* 83 */ {.importName = (void *)g_GlideImportName_guFogGenerateExp},
    /* 84 */ {.importName = (void *)g_GlideImportName_guFogGenerateExp2},
    /* 85 */ {.importName = (void *)g_GlideImportName_guFogGenerateLinear},
    /* 86 */ {.importName = (void *)&g_GlideImportName_guFogTableIndexToW},
    /* 87 */ {.importName = (void *)&g_GlideImportName_guGammaCorrectionRGB}};

/* 005744CA sz_GLIDE3X */
__declspec(align(4)) char sz_GLIDE3X[8] = "GLIDE3X";

/* 005745FA dynapi_24 */
__declspec(align(4)) char g_GlideImportName_grAADrawTriangle[21] = "_grAADrawTriangle@24";

/* 00574610 dynapi_25 */
__declspec(align(16)) char g_GlideImportName_grAlphaBlendFunction[25] = "_grAlphaBlendFunction@16";

/* 0057462A dynapi_26 */
__declspec(align(4)) char g_GlideImportName_grAlphaCombine[19] = "_grAlphaCombine@20";

/* 0057463E dynapi_27 */
__declspec(align(4)) char g_GlideImportName_grAlphaControlsITRGBLighting[32] = "_grAlphaControlsITRGBLighting@4";

/* 0057465E dynapi_28 */
__declspec(align(4)) char g_GlideImportName_grAlphaTestFunction[23] = "_grAlphaTestFunction@4";

/* 00574676 dynapi_29 */
__declspec(align(4)) char g_GlideImportName_grAlphaTestReferenceValue[29] = "_grAlphaTestReferenceValue@4";

/* 00574694 dynapi_30 */
__declspec(align(4)) char g_GlideImportName_grBufferClear[18] = "_grBufferClear@12";

/* 005746A6 dynapi_31 */
__declspec(align(4)) char g_GlideImportName_grBufferSwap[16] = "_grBufferSwap@4";

/* 005746B6 dynapi_32 */
__declspec(align(4)) char g_GlideImportName_grChromakeyMode[19] = "_grChromakeyMode@4";

/* 005746CA dynapi_33 */
__declspec(align(4)) char g_GlideImportName_grChromakeyValue[20] = "_grChromakeyValue@4";

/* 00574706 dynapi_36 */
__declspec(align(4)) char g_GlideImportName_grClipWindow[17] = "_grClipWindow@16";

/* 00574718 dynapi_37 */
__declspec(align(8)) char g_GlideImportName_grColorCombine[19] = "_grColorCombine@20";

/* 0057472C dynapi_38 */
__declspec(align(4)) char g_GlideImportName_grColorMask[15] = "_grColorMask@8";

/* 0057473C dynapi_39 */
__declspec(align(4)) char g_GlideImportName_grConstantColorValue[24] = "_grConstantColorValue@4";

/* 00574754 dynapi_40 */
__declspec(align(4)) char g_GlideImportName_grCoordinateSpace[21] = "_grCoordinateSpace@4";

/* 0057476A dynapi_41 */
__declspec(align(4)) char g_GlideImportName_grCullMode[14] = "_grCullMode@4";

/* 00574778 dynapi_42 */
__declspec(align(8)) char g_GlideImportName_grDepthBiasLevel[20] = "_grDepthBiasLevel@4";

/* 0057478C dynapi_43 */
__declspec(align(4)) char g_GlideImportName_grDepthBufferFunction[25] = "_grDepthBufferFunction@4";

/* 005747A6 dynapi_44 */
__declspec(align(4)) char g_GlideImportName_grDepthBufferMode[21] = "_grDepthBufferMode@4";

/* 005747BC dynapi_45 */
__declspec(align(4)) char g_GlideImportName_grDepthMask[15] = "_grDepthMask@4";

/* 005747CC dynapi_46 */
__declspec(align(4)) char g_GlideImportName_grDepthRange[16] = "_grDepthRange@8";

/* 005747DC dynapi_47 */
__declspec(align(4)) char g_GlideImportName_grDisable[13] = "_grDisable@4";

/* 005747EA dynapi_48 */
__declspec(align(4)) char g_GlideImportName_grDisableAllEffects[23] = "_grDisableAllEffects@0";

/* 00574802 dynapi_49 */
__declspec(align(4)) char g_GlideImportName_grDitherMode[16] = "_grDitherMode@4";

/* 00574812 dynapi_50 */
__declspec(align(4)) char g_GlideImportName_grDrawLine[14] = "_grDrawLine@8";

/* 00574820 dynapi_51 */
__declspec(align(16)) char g_GlideImportName_grDrawPoint[15] = "_grDrawPoint@4";

/* 00574830 dynapi_52 */
__declspec(align(16)) char g_GlideImportName_grDrawTriangle[19] = "_grDrawTriangle@12";

/* 00574844 dynapi_53 */
__declspec(align(4)) char g_GlideImportName_grDrawVertexArray[22] = "_grDrawVertexArray@12";

/* 0057485A dynapi_54 */
__declspec(align(4)) char g_GlideImportName_grDrawVertexArrayContiguous[32] = "_grDrawVertexArrayContiguous@16";

/* 0057487A dynapi_55 */
__declspec(align(4)) char g_GlideImportName_grEnable[12] = "_grEnable@4";

/* 00574886 dynapi_56 */
__declspec(align(4)) char g_GlideImportName_grErrorSetCallback[22] = "_grErrorSetCallback@4";

/* 0057489C dynapi_57 */
__declspec(align(4)) char g_GlideImportName_grFinish[12] = "_grFinish@0";

/* 005748A8 dynapi_58 */
__declspec(align(8)) char g_GlideImportName_grFlush[11] = "_grFlush@0";

/* 005748B4 dynapi_59 */
__declspec(align(4)) char g_GlideImportName_grFogColorValue[19] = "_grFogColorValue@4";

/* 005748C8 dynapi_60 */
__declspec(align(8)) char g_GlideImportName_grFogMode[13] = "_grFogMode@4";

/* 005748D6 dynapi_61 */
__declspec(align(4)) char g_GlideImportName_grFogTable[14] = "_grFogTable@4";

/* 005748E4 dynapi_62 */
__declspec(align(4)) char g_GlideImportName_grGet[10] = "_grGet@12";

/* 005748EE dynapi_63 */
__declspec(align(4)) char g_GlideImportName_grGetProcAddress[20] = "_grGetProcAddress@4";

/* 00574902 dynapi_64 */
__declspec(align(4)) char g_GlideImportName_grGetString[15] = "_grGetString@4";

/* 00574912 dynapi_65 */
__declspec(align(4)) char g_GlideImportName_grGlideGetState[19] = "_grGlideGetState@4";

/* 00574926 dynapi_66 */
__declspec(align(4)) char g_GlideImportName_grGlideGetVertexLayout[26] = "_grGlideGetVertexLayout@4";

/* 00574940 dynapi_67 */
__declspec(align(16)) char g_GlideImportName_grGlideInit[15] = "_grGlideInit@0";

/* 00574950 dynapi_68 */
__declspec(align(16)) char g_GlideImportName_grGlideSetState[19] = "_grGlideSetState@4";

/* 00574964 dynapi_69 */
__declspec(align(4)) char g_GlideImportName_grGlideSetVertexLayout[26] = "_grGlideSetVertexLayout@4";

/* 0057497E dynapi_70 */
__declspec(align(4)) char g_GlideImportName_grGlideShutdown[19] = "_grGlideShutdown@0";

/* 00574992 dynapi_71 */
__declspec(align(4)) char g_GlideImportName_grLfbConstantAlpha[22] = "_grLfbConstantAlpha@4";

/* 005749A8 dynapi_72 */
__declspec(align(8)) char g_GlideImportName_grLfbConstantDepth[22] = "_grLfbConstantDepth@4";

/* 005749BE dynapi_73 */
__declspec(align(4)) char g_GlideImportName_grLfbLock[14] = "_grLfbLock@24";

/* 005749CC dynapi_74 */
__declspec(align(4)) char g_GlideImportName_grLfbReadRegion[20] = "_grLfbReadRegion@28";

/* 005749E0 dynapi_75 */
__declspec(align(16)) char g_GlideImportName_grLfbUnlock[15] = "_grLfbUnlock@8";

/* 005749F0 dynapi_76 */
__declspec(align(16)) char g_GlideImportName_grLfbWriteRegion[21] = "_grLfbWriteRegion@36";

/* 00574A06 dynapi_77 */
__declspec(align(4)) char g_GlideImportName_grLoadGammaTable[21] = "_grLoadGammaTable@16";

/* 00574A1C dynapi_78 */
__declspec(align(4)) char g_GlideImportName_grQueryResolutions[22] = "_grQueryResolutions@8";

/* 00574A32 dynapi_79 */
__declspec(align(4)) char g_GlideImportName_grRenderBuffer[18] = "_grRenderBuffer@4";

/* 00574A44 dynapi_80 */
__declspec(align(4)) char g_GlideImportName_grReset[11] = "_grReset@4";

/* 00574A50 dynapi_81 */
__declspec(align(16)) char g_GlideImportName_grSelectContext[19] = "_grSelectContext@4";

/* 00574A64 dynapi_82 */
__declspec(align(4)) char g_GlideImportName_grSstOrigin[15] = "_grSstOrigin@4";

/* 00574A74 dynapi_83 */
__declspec(align(4)) char g_GlideImportName_grSstSelect[15] = "_grSstSelect@4";

/* 00574A84 dynapi_84 */
__declspec(align(4)) char g_GlideImportName_grSstWinClose[17] = "_grSstWinClose@4";

/* 00574A96 dynapi_85 */
__declspec(align(4)) char g_GlideImportName_grSstWinOpen[17] = "_grSstWinOpen@28";

/* 00574AA8 dynapi_86 */
__declspec(align(8)) char g_GlideImportName_grTexCalcMemRequired[25] = "_grTexCalcMemRequired@16";

/* 00574AF0 dynapi_89 */
__declspec(align(16)) char g_GlideImportName_grTexClampMode[19] = "_grTexClampMode@12";

/* 00574B04 dynapi_90 */
__declspec(align(4)) char g_GlideImportName_grTexCombine[17] = "_grTexCombine@28";

/* 00574B16 dynapi_91 */
__declspec(align(4)) char g_GlideImportName_grTexDetailControl[23] = "_grTexDetailControl@16";

/* 00574B2E dynapi_92 */
__declspec(align(4)) char g_GlideImportName_grTexDownloadMipMap[24] = "_grTexDownloadMipMap@16";

/* 00574B46 dynapi_93 */
__declspec(align(4)) char g_GlideImportName_grTexDownloadMipMapLevel[29] = "_grTexDownloadMipMapLevel@32";

/* 00574B64 dynapi_94 */
__declspec(align(4)) char g_GlideImportName_grTexDownloadMipMapLevelPartial[36] = "_grTexDownloadMipMapLevelPartial@40";

/* 00574B88 dynapi_95 */
__declspec(align(8)) char g_GlideImportName_grTexDownloadTable[22] = "_grTexDownloadTable@8";

/* 00574B9E dynapi_96 */
__declspec(align(4)) char g_GlideImportName_grTexDownloadTablePartial[30] = "_grTexDownloadTablePartial@16";

/* 00574BBC dynapi_97 */
__declspec(align(4)) char g_GlideImportName_grTexFilterMode[20] = "_grTexFilterMode@12";

/* 00574BD0 dynapi_98 */
__declspec(align(16)) char g_GlideImportName_grTexLodBiasValue[21] = "_grTexLodBiasValue@8";

/* 00574BE6 dynapi_99 */
__declspec(align(4)) char g_GlideImportName_grTexMaxAddress[19] = "_grTexMaxAddress@4";

/* 00574BFA dynapi_100 */
__declspec(align(4)) char g_GlideImportName_grTexMinAddress[19] = "_grTexMinAddress@4";

/* 00574C0E dynapi_101 */
__declspec(align(4)) char g_GlideImportName_grTexMipMapMode[20] = "_grTexMipMapMode@12";

/* 00574C22 dynapi_102 */
__declspec(align(4)) char g_GlideImportName_grTexMultibase[18] = "_grTexMultibase@8";

/* 00574C34 dynapi_103 */
__declspec(align(4)) char g_GlideImportName_grTexMultibaseAddress[26] = "_grTexMultibaseAddress@20";

/* 00574C4E dynapi_104 */
__declspec(align(4)) char g_GlideImportName_grTexNCCTable[17] = "_grTexNCCTable@4";

/* 00574C60 dynapi_105 */
__declspec(align(16)) char g_GlideImportName_grTexSource[16] = "_grTexSource@16";

/* 00574C70 dynapi_106 */
__declspec(align(16)) char g_GlideImportName_grTexTextureMemRequired[27] = "_grTexTextureMemRequired@8";

/* 00574C8C dynapi_107 */
__declspec(align(4)) char g_GlideImportName_grVertexLayout[19] = "_grVertexLayout@12";

/* 00574CA0 dynapi_108 */
__declspec(align(16)) char g_GlideImportName_grViewport[15] = "_grViewport@16";

/* 00574CB0 dynapi_109 */
__declspec(align(16)) char g_GlideImportName_gu3dfGetInfo[16] = "_gu3dfGetInfo@8";

/* 00574CC0 dynapi_110 */
__declspec(align(16)) char g_GlideImportName_gu3dfLoad[13] = "_gu3dfLoad@8";

/* 00574CCE dynapi_111 */
__declspec(align(4)) char g_GlideImportName_guFogGenerateExp[20] = "_guFogGenerateExp@8";

/* 00574CE2 dynapi_112 */
__declspec(align(4)) char g_GlideImportName_guFogGenerateExp2[21] = "_guFogGenerateExp2@8";

/* 00574CF8 dynapi_113 */
__declspec(align(8)) char g_GlideImportName_guFogGenerateLinear[24] = "_guFogGenerateLinear@12";

/* 00574D10 dynapi_114 */
__declspec(align(16)) char g_GlideImportName_guFogTableIndexToW[22] = "_guFogTableIndexToW@4";

/* 00574D26 dynapi_115 */
__declspec(align(4)) char g_GlideImportName_guGammaCorrectionRGB[25] = "_guGammaCorrectionRGB@12";

/* 00577C00 IID_IDirectDraw2_Local */
__declspec(align(16)) TH_LEGACY_GUID IID_IDirectDraw2_Local = {.Data1 = 0xB3A6F3E0, .Data2 = 11075, .Data3 = 4559, .Data4 = {162, 222, 0, 170, 0, 185, 51, 86}};

/* 00577C10 IID_IDirectDrawSurface3_Local */
__declspec(align(16)) TH_LEGACY_GUID IID_IDirectDrawSurface3_Local = {.Data1 = 0xDA044E00, .Data2 = 27058, .Data3 = 4560, .Data4 = {161, 213, 0, 170, 0, 184, 223, 187}};

/* 00577C20 IID_IDirect3D2_Local */
__declspec(align(16)) TH_LEGACY_GUID IID_IDirect3D2_Local = {.Data1 = 0x6AAE1EC1, .Data2 = 26154, .Data3 = 4560, .Data4 = {136, 157, 0, 170, 0, 187, 183, 106}};

/* 00577C40 g_DirectDraw */
__declspec(align(16)) IDirectDraw *g_DirectDraw = 0;

/* 00577C44 g_DirectDraw2 */
__declspec(align(4)) IDirectDraw2 *g_DirectDraw2 = 0;

/* 00577C48 g_PrimarySurfaceBase */
__declspec(align(8)) IDirectDrawSurface *g_PrimarySurfaceBase = 0;

/* 00577C50 g_BackSurfaceBase */
__declspec(align(16)) IDirectDrawSurface *g_BackSurfaceBase = 0;

/* 00577C58 g_Direct3D2 */
__declspec(align(8)) IDirect3D2 *g_Direct3D2 = 0;

/* 00577C5C g_Direct3DDevice2 */
__declspec(align(4)) IDirect3DDevice2 *g_Direct3DDevice2 = 0;

/* 00577C60 g_ZSurfaceBase */
__declspec(align(16)) IDirectDrawSurface *g_ZSurfaceBase = 0;

/* 00577C64 g_ZSurface3 */
__declspec(align(4)) IDirectDrawSurface3 *g_ZSurface3 = 0;

/* 00577D90 g_Direct3DOpaqueTextureFormat */
__declspec(align(16)) DDPIXELFORMAT g_Direct3DOpaqueTextureFormat = {0};

/* 00577DB0 g_Direct3DAlphaTextureFormat */
__declspec(align(16)) DDPIXELFORMAT g_Direct3DAlphaTextureFormat = {0};

/* 00577DD0 g_Direct3DSelectedOpaqueTextureFormat */
__declspec(align(16)) DDPIXELFORMAT g_Direct3DSelectedOpaqueTextureFormat = {0};

/* 00577DF0 g_Direct3DSelectedAlphaTextureFormat */
__declspec(align(16)) DDPIXELFORMAT g_Direct3DSelectedAlphaTextureFormat = {0};

/* 00577E10 g_Direct3DTextureFilterMode: uint32_t: D3DRENDERSTATE_TEXTUREMAG/MIN filter (2 = D3DFILTER_LINEAR) reapplied by the device setup; graphics/backend direct3d/directdraw */
__declspec(align(16)) uint32_t g_Direct3DTextureFilterMode = 2;

/* 00577E14 g_Direct3DTexturePerspectiveEnabled: uint32_t: D3DRENDERSTATE_TEXTUREPERSPECTIVE value (1) reapplied by the device setup; graphics/backend direct3d/directdraw */
__declspec(align(4)) uint32_t g_Direct3DTexturePerspectiveEnabled = 1;

/* 00577E18 g_Direct3DAntialiasMode */
__declspec(align(8)) uint32_t g_Direct3DAntialiasMode = 0;

/* 00577E1C g_PrimitiveRenderStateCache */
__declspec(align(4)) GraphicsPrimitiveRenderStateCache g_PrimitiveRenderStateCache = {
    .zWriteEnable = GRAPHICS_STATE_ENABLED,
    .sourceBlend = D3DBLEND_ONE,
    .destinationBlend = D3DBLEND_ZERO};

/* 00577E2C g_BoundTextureHandle */
__declspec(align(4)) uint32_t g_BoundTextureHandle = 0;

/* 00577E30 g_GraphicsDisplayModeFinalize */
__declspec(align(16)) SoftwareDisplayModeHookProc *g_GraphicsDisplayModeFinalize = 0;

/* 00577E34 g_CursorCurrentVisibilityToken */
__declspec(align(4)) int32_t g_CursorCurrentVisibilityToken = 0;

/* 00577E38 g_CursorAlternateVisibilityToken */
__declspec(align(8)) int32_t g_CursorAlternateVisibilityToken = 0;

/* 00577E4C g_GraphicsBackendAccessState */
__declspec(align(4)) int32_t g_GraphicsBackendAccessState = -0x1;

/* 00577E50 g_PrimitiveRenderStatePresets */
__declspec(align(16)) GraphicsPrimitiveRenderStatePreset g_PrimitiveRenderStatePresets[5] = {
    /* 0 */ {
    .sourceBlend = D3DBLEND_ONE,
    .destinationBlend = D3DBLEND_ZERO,
    .zWriteEnable = GRAPHICS_STATE_ENABLED},
    /* 1 */ {
    .sourceBlend = D3DBLEND_SRCALPHA,
    .destinationBlend = D3DBLEND_INVSRCALPHA,
    .alphaBlendEnable = GRAPHICS_STATE_ENABLED,
    .zWriteEnable = GRAPHICS_STATE_ENABLED},
    /* 2 */ {
    .sourceBlend = D3DBLEND_SRCALPHA,
    .destinationBlend = D3DBLEND_INVSRCALPHA,
    .alphaBlendEnable = GRAPHICS_STATE_ENABLED},
    /* 3 */ {
    .sourceBlend = D3DBLEND_SRCALPHA,
    .destinationBlend = D3DBLEND_INVSRCALPHA,
    .alphaBlendEnable = GRAPHICS_STATE_ENABLED,
    .zWriteEnable = GRAPHICS_STATE_ENABLED},
    /* 4 */ {
    .sourceBlend = D3DBLEND_ONE,
    .destinationBlend = D3DBLEND_ONE,
    .alphaBlendEnable = GRAPHICS_STATE_ENABLED}};

/* 00577EA0 g_GraphicsDispatchTable */
__declspec(align(16)) GraphicsDispatchTable g_GraphicsDispatchTable = {
        .colorUpload = {
            /* 0 */ (void *)GraphicsTexture_UploadColor_1x,
            /* 1 */ (void *)GraphicsTexture_UploadColor_2x,
            /* 2 */ (void *)GraphicsTexture_UploadColor_4x
        },
        .alphaUpload = {
            /* 0 */ (void *)GraphicsTexture_UploadAlpha_1x,
            /* 1 */ (void *)GraphicsTexture_UploadAlpha_2x,
            /* 2 */ (void *)GraphicsTexture_UploadAlpha_4x
        },
        .primitive = {
            /*  0 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset0,
            /*  1 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset2,
            /*  2 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset4,
            /*  3 */ 0,
            /*  4 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset0,
            /*  5 */ 0,
            /*  6 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset3,
            /*  7 */ 0,
            /*  8 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset0,
            /*  9 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset2,
            /* 10 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset4,
            /* 11 */ 0,
            /* 12 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset0,
            /* 13 */ 0,
            /* 14 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset3,
            /* 15 */ 0,
            /* 16 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset0,
            /* 17 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset2,
            /* 18 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset4,
            /* 19 */ 0,
            /* 20 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset1,
            /* 21 */ 0,
            /* 22 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset3,
            /* 23 */ 0,
            /* 24 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset0,
            /* 25 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset2,
            /* 26 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset4,
            /* 27 */ 0,
            /* 28 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset1,
            /* 29 */ 0,
            /* 30 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset3,
            /* 31 */ 0,
            /* 32 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset2,
            /* 33 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset2,
            /* 34 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset4,
            /* 35 */ 0,
            /* 36 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset2,
            /* 37 */ 0,
            /* 38 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset2,
            /* 39 */ 0,
            /* 40 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset2,
            /* 41 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset2,
            /* 42 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset4,
            /* 43 */ 0,
            /* 44 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset2,
            /* 45 */ 0,
            /* 46 */ (void *)Direct3D_PrimitiveHandler_UntexturedPreset2,
            /* 47 */ 0,
            /* 48 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset2,
            /* 49 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset2,
            /* 50 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset4,
            /* 51 */ 0,
            /* 52 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset2,
            /* 53 */ 0,
            /* 54 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset2,
            /* 55 */ 0,
            /* 56 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset2,
            /* 57 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset2,
            /* 58 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset4,
            /* 59 */ 0,
            /* 60 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset2,
            /* 61 */ 0,
            /* 62 */ (void *)Direct3D_PrimitiveHandler_TexturedPreset2
        }};

/* 00577FC0 g_ImmediateTLVertices */
__declspec(align(16)) D3DTLVERTEX_DX6 g_ImmediateTLVertices[4] = {
    /* 0 */ {.sx = 0.0f, .sy = 0.0f, .sz = 0.0f, .rhw = 0.0f, .tu = 0.0f, .tv = 0.0f},
    /* 1 */ {.sx = 0.0f, .sy = 0.0f, .sz = 0.0f, .rhw = 0.0f, .tu = 0.0f, .tv = 0.0f},
    /* 2 */ {.sx = 0.0f, .sy = 0.0f, .sz = 0.0f, .rhw = 0.0f, .tu = 0.0f, .tv = 0.0f},
    /* 3 */ {.sx = 0.0f, .sy = 0.0f, .sz = 0.0f, .rhw = 0.0f, .tu = 0.0f, .tv = 0.0f}};

/* 00578050 g_ImmediateVertexCount */
__declspec(align(16)) uint32_t g_ImmediateVertexCount = 0;

/* 00578054 g_TextureUseSerial */
__declspec(align(4)) uint32_t g_TextureUseSerial = 0;

/* 0057805C g_GraphicsTextureSlots */
__declspec(align(4)) GraphicsTextureResource **g_GraphicsTextureSlots = 0;

/* 00578060 g_DisplayFramebufferAccess */
__declspec(align(16)) SoftwareFramebufferAccess g_DisplayFramebufferAccess = {0};

/* 00578074 g_GraphicsEnumerateAllDevicesFlag */
__declspec(align(4)) uint32_t g_GraphicsEnumerateAllDevicesFlag = 0;

/* 0057ECD0 g_GlideEnumerationResolutionQuery: GrResolution: grQueryResolutions template {GR_QUERY_ANY, GR_QUERY_ANY, 2 colour buffers, 1 aux buffer} for enumerating modes; Glide3_InitAndEnumerate */
__declspec(align(16)) GrResolution g_GlideEnumerationResolutionQuery = {.resolution = -1, .refresh = 0xFFFFFFFF, .numColorBuffers = 2, .numAuxBuffers = 1};

/* 0057ECE0 g_GlideSelectedResolutionQuery: GrResolution: grQueryResolutions template {resolution set at run time, GR_QUERY_ANY refresh, 2, 1} to pick the best refresh rate; GraphicsGlide3_ApplyDisplayModeAndInitializeResources */
__declspec(align(16)) GrResolution g_GlideSelectedResolutionQuery = {.refresh = 0xFFFFFFFF, .numColorBuffers = 2, .numAuxBuffers = 1};

/* 0057ECF0 g_GlideRefreshRatesHz: uint32_t[9]: Hz per GR_REFRESH_* code (60,70,72,75,80,90,100,85,120); graphics/backend/glide.c */
__declspec(align(16)) uint32_t g_GlideRefreshRatesHz[9] = {60, 70, 72, 75, 80, 90, 100, 85, 120};

/* 0057ED14 g_GlideWindowContextHandle */
__declspec(align(4)) uint32_t g_GlideWindowContextHandle = 0;

/* 0057ED18 g_GlideRuntimeActiveCount */
__declspec(align(8)) uint32_t g_GlideRuntimeActiveCount = 0;

/* 0057ED20 g_GlidePrimaryLfbInfo */
__declspec(align(16)) GlideLfbInfo g_GlidePrimaryLfbInfo = {.size = 20};

/* 0057ED34 g_GlideSecondaryLfbInfo */
__declspec(align(4)) GlideLfbInfo g_GlideSecondaryLfbInfo = {.size = 20};

/* 0057ED54 g_GlideVertices: uint32_t[3][8]: the three GrVertex records (float bit patterns) Glide3_DrawPrimitiveQueue fills and hands to grDrawTriangle; fields GLIDE_VERTEX_* (graphics/backend/glide.h), dword 4 of each record is unused */
__declspec(align(4)) uint32_t g_GlideVertices[3][8] = {0};

/* 0057EDC8 g_GlideTmuCount */
__declspec(align(8)) uint32_t g_GlideTmuCount = 0;

/* 0057EDCC g_GlideTextureColorUpload */
__declspec(align(4)) GlideTextureUploadProc *g_GlideTextureColorUpload[3] = {(void *)Glide3_TextureUpload_1x, (void *)Glide3_TextureUpload_2x, (void *)Glide3_TextureUpload_4x};

/* 0057EDD8 g_GlideTextureAlphaUpload */
__declspec(align(8)) GlideTextureUploadProc *g_GlideTextureAlphaUpload[3] = {(void *)Glide3_TextureUpload_1x, (void *)Glide3_TextureUpload_2x, (void *)Glide3_TextureUpload_4x};

/* 0057EDE4 g_GlideTexturingDisabledState */
__declspec(align(4)) uint32_t g_GlideTexturingDisabledState = 0;

/* 0057EDE8 g_GlideBlendModeState */
__declspec(align(8)) uint32_t g_GlideBlendModeState = 0;

/* 0057EDEC g_GlideDepthWriteEnabledState */
__declspec(align(4)) uint32_t g_GlideDepthWriteEnabledState = 0;

/* 0057EDF0 g_GlideBoundTexture */
__declspec(align(16)) GraphicsTextureResource *g_GlideBoundTexture = 0;

/* 0057EDF4 g_GlideTmuMinAddress */
__declspec(align(4)) uint32_t g_GlideTmuMinAddress[16] = {0};

/* 0057EE34 g_GlideTmuMaxAddress */
__declspec(align(4)) uint32_t g_GlideTmuMaxAddress[16] = {0};

/* 0057EE74 g_GlideResidentTextureTail */
__declspec(align(4)) GraphicsTextureResource *g_GlideResidentTextureTail = 0;

/* 0057EE78 g_GlideResidentTextureHead */
__declspec(align(8)) GraphicsTextureResource *g_GlideResidentTextureHead = 0;

/* 0057EE7C g_GlideSecondBufferOffset */
__declspec(align(4)) int32_t g_GlideSecondBufferOffset = 0;

/* 0057EE80 g_GlideSecondBufferBase */
__declspec(align(16)) uint8_t *g_GlideSecondBufferBase = 0;
