/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/model_lighting.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/model.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* SoftwareBgraWordLanes[819] (MODEL_LIGHTING_MMX_ROW_COUNT), PMULHW
   multipliers (alpha lane 0x4000) of the model vertex lighting. One table in the original, reached from two base rows:
   ModelRender_ComputeVertexIntensityDefaultPath indexes from row MODEL_DISTANCE_ATTENUATION_ROW0 (136)
   with the signed light-facing dot >> 21, ModelRender_ComputeVertexIntensityScaledPath from row
   MODEL_LIGHTING_SCALE_ROW0 (682) with (dot / lightingScaleQ12) >> 9; negative indices of either run
   into the rows before. Row numbers in the comments below restart at each former start. */
SoftwareBgraWordLanes g_ModelLightingMmxMultiplierRows[819] = {
    /* rows 0..135 (former g_ModelDistanceAttenuationMmxNegativeRows): attenuation rows -136..-1; B/G/R
       lanes 0x007F (row -1) rising by 0x80 to 0x3F7F, then 0x3FFF */
    /*   0 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
    /*   1 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
    /*   2 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
    /*   3 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
    /*   4 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
    /*   5 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
    /*   6 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
    /*   7 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
    /*   8 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
    /*   9 */ {.blue = 16255, .green = 16255, .red = 16255, .alpha = 16384},
    /*  10 */ {.blue = 16127, .green = 16127, .red = 16127, .alpha = 16384},
    /*  11 */ {.blue = 15999, .green = 15999, .red = 15999, .alpha = 16384},
    /*  12 */ {.blue = 15871, .green = 15871, .red = 15871, .alpha = 16384},
    /*  13 */ {.blue = 15743, .green = 15743, .red = 15743, .alpha = 16384},
    /*  14 */ {.blue = 15615, .green = 15615, .red = 15615, .alpha = 16384},
    /*  15 */ {.blue = 15487, .green = 15487, .red = 15487, .alpha = 16384},
    /*  16 */ {.blue = 15359, .green = 15359, .red = 15359, .alpha = 16384},
    /*  17 */ {.blue = 15231, .green = 15231, .red = 15231, .alpha = 16384},
    /*  18 */ {.blue = 15103, .green = 15103, .red = 15103, .alpha = 16384},
    /*  19 */ {.blue = 14975, .green = 14975, .red = 14975, .alpha = 16384},
    /*  20 */ {.blue = 14847, .green = 14847, .red = 14847, .alpha = 16384},
    /*  21 */ {.blue = 14719, .green = 14719, .red = 14719, .alpha = 16384},
    /*  22 */ {.blue = 14591, .green = 14591, .red = 14591, .alpha = 16384},
    /*  23 */ {.blue = 14463, .green = 14463, .red = 14463, .alpha = 16384},
    /*  24 */ {.blue = 14335, .green = 14335, .red = 14335, .alpha = 16384},
    /*  25 */ {.blue = 14207, .green = 14207, .red = 14207, .alpha = 16384},
    /*  26 */ {.blue = 14079, .green = 14079, .red = 14079, .alpha = 16384},
    /*  27 */ {.blue = 13951, .green = 13951, .red = 13951, .alpha = 16384},
    /*  28 */ {.blue = 13823, .green = 13823, .red = 13823, .alpha = 16384},
    /*  29 */ {.blue = 13695, .green = 13695, .red = 13695, .alpha = 16384},
    /*  30 */ {.blue = 13567, .green = 13567, .red = 13567, .alpha = 16384},
    /*  31 */ {.blue = 13439, .green = 13439, .red = 13439, .alpha = 16384},
    /*  32 */ {.blue = 13311, .green = 13311, .red = 13311, .alpha = 16384},
    /*  33 */ {.blue = 13183, .green = 13183, .red = 13183, .alpha = 16384},
    /*  34 */ {.blue = 13055, .green = 13055, .red = 13055, .alpha = 16384},
    /*  35 */ {.blue = 12927, .green = 12927, .red = 12927, .alpha = 16384},
    /*  36 */ {.blue = 12799, .green = 12799, .red = 12799, .alpha = 16384},
    /*  37 */ {.blue = 12671, .green = 12671, .red = 12671, .alpha = 16384},
    /*  38 */ {.blue = 12543, .green = 12543, .red = 12543, .alpha = 16384},
    /*  39 */ {.blue = 12415, .green = 12415, .red = 12415, .alpha = 16384},
    /*  40 */ {.blue = 12287, .green = 12287, .red = 12287, .alpha = 16384},
    /*  41 */ {.blue = 12159, .green = 12159, .red = 12159, .alpha = 16384},
    /*  42 */ {.blue = 12031, .green = 12031, .red = 12031, .alpha = 16384},
    /*  43 */ {.blue = 11903, .green = 11903, .red = 11903, .alpha = 16384},
    /*  44 */ {.blue = 11775, .green = 11775, .red = 11775, .alpha = 16384},
    /*  45 */ {.blue = 11647, .green = 11647, .red = 11647, .alpha = 16384},
    /*  46 */ {.blue = 11519, .green = 11519, .red = 11519, .alpha = 16384},
    /*  47 */ {.blue = 11391, .green = 11391, .red = 11391, .alpha = 16384},
    /*  48 */ {.blue = 11263, .green = 11263, .red = 11263, .alpha = 16384},
    /*  49 */ {.blue = 11135, .green = 11135, .red = 11135, .alpha = 16384},
    /*  50 */ {.blue = 11007, .green = 11007, .red = 11007, .alpha = 16384},
    /*  51 */ {.blue = 10879, .green = 10879, .red = 10879, .alpha = 16384},
    /*  52 */ {.blue = 10751, .green = 10751, .red = 10751, .alpha = 16384},
    /*  53 */ {.blue = 10623, .green = 10623, .red = 10623, .alpha = 16384},
    /*  54 */ {.blue = 10495, .green = 10495, .red = 10495, .alpha = 16384},
    /*  55 */ {.blue = 10367, .green = 10367, .red = 10367, .alpha = 16384},
    /*  56 */ {.blue = 10239, .green = 10239, .red = 10239, .alpha = 16384},
    /*  57 */ {.blue = 10111, .green = 10111, .red = 10111, .alpha = 16384},
    /*  58 */ {.blue = 9983, .green = 9983, .red = 9983, .alpha = 16384},
    /*  59 */ {.blue = 9855, .green = 9855, .red = 9855, .alpha = 16384},
    /*  60 */ {.blue = 9727, .green = 9727, .red = 9727, .alpha = 16384},
    /*  61 */ {.blue = 9599, .green = 9599, .red = 9599, .alpha = 16384},
    /*  62 */ {.blue = 9471, .green = 9471, .red = 9471, .alpha = 16384},
    /*  63 */ {.blue = 9343, .green = 9343, .red = 9343, .alpha = 16384},
    /*  64 */ {.blue = 9215, .green = 9215, .red = 9215, .alpha = 16384},
    /*  65 */ {.blue = 9087, .green = 9087, .red = 9087, .alpha = 16384},
    /*  66 */ {.blue = 8959, .green = 8959, .red = 8959, .alpha = 16384},
    /*  67 */ {.blue = 8831, .green = 8831, .red = 8831, .alpha = 16384},
    /*  68 */ {.blue = 8703, .green = 8703, .red = 8703, .alpha = 16384},
    /*  69 */ {.blue = 8575, .green = 8575, .red = 8575, .alpha = 16384},
    /*  70 */ {.blue = 8447, .green = 8447, .red = 8447, .alpha = 16384},
    /*  71 */ {.blue = 8319, .green = 8319, .red = 8319, .alpha = 16384},
    /*  72 */ {.blue = 8191, .green = 8191, .red = 8191, .alpha = 16384},
    /*  73 */ {.blue = 8063, .green = 8063, .red = 8063, .alpha = 16384},
    /*  74 */ {.blue = 7935, .green = 7935, .red = 7935, .alpha = 16384},
    /*  75 */ {.blue = 7807, .green = 7807, .red = 7807, .alpha = 16384},
    /*  76 */ {.blue = 7679, .green = 7679, .red = 7679, .alpha = 16384},
    /*  77 */ {.blue = 7551, .green = 7551, .red = 7551, .alpha = 16384},
    /*  78 */ {.blue = 7423, .green = 7423, .red = 7423, .alpha = 16384},
    /*  79 */ {.blue = 7295, .green = 7295, .red = 7295, .alpha = 16384},
    /*  80 */ {.blue = 7167, .green = 7167, .red = 7167, .alpha = 16384},
    /*  81 */ {.blue = 7039, .green = 7039, .red = 7039, .alpha = 16384},
    /*  82 */ {.blue = 6911, .green = 6911, .red = 6911, .alpha = 16384},
    /*  83 */ {.blue = 6783, .green = 6783, .red = 6783, .alpha = 16384},
    /*  84 */ {.blue = 6655, .green = 6655, .red = 6655, .alpha = 16384},
    /*  85 */ {.blue = 6527, .green = 6527, .red = 6527, .alpha = 16384},
    /*  86 */ {.blue = 6399, .green = 6399, .red = 6399, .alpha = 16384},
    /*  87 */ {.blue = 6271, .green = 6271, .red = 6271, .alpha = 16384},
    /*  88 */ {.blue = 6143, .green = 6143, .red = 6143, .alpha = 16384},
    /*  89 */ {.blue = 6015, .green = 6015, .red = 6015, .alpha = 16384},
    /*  90 */ {.blue = 5887, .green = 5887, .red = 5887, .alpha = 16384},
    /*  91 */ {.blue = 5759, .green = 5759, .red = 5759, .alpha = 16384},
    /*  92 */ {.blue = 5631, .green = 5631, .red = 5631, .alpha = 16384},
    /*  93 */ {.blue = 5503, .green = 5503, .red = 5503, .alpha = 16384},
    /*  94 */ {.blue = 5375, .green = 5375, .red = 5375, .alpha = 16384},
    /*  95 */ {.blue = 5247, .green = 5247, .red = 5247, .alpha = 16384},
    /*  96 */ {.blue = 5119, .green = 5119, .red = 5119, .alpha = 16384},
    /*  97 */ {.blue = 4991, .green = 4991, .red = 4991, .alpha = 16384},
    /*  98 */ {.blue = 4863, .green = 4863, .red = 4863, .alpha = 16384},
    /*  99 */ {.blue = 4735, .green = 4735, .red = 4735, .alpha = 16384},
    /* 100 */ {.blue = 4607, .green = 4607, .red = 4607, .alpha = 16384},
    /* 101 */ {.blue = 4479, .green = 4479, .red = 4479, .alpha = 16384},
    /* 102 */ {.blue = 4351, .green = 4351, .red = 4351, .alpha = 16384},
    /* 103 */ {.blue = 4223, .green = 4223, .red = 4223, .alpha = 16384},
    /* 104 */ {.blue = 4095, .green = 4095, .red = 4095, .alpha = 16384},
    /* 105 */ {.blue = 3967, .green = 3967, .red = 3967, .alpha = 16384},
    /* 106 */ {.blue = 3839, .green = 3839, .red = 3839, .alpha = 16384},
    /* 107 */ {.blue = 3711, .green = 3711, .red = 3711, .alpha = 16384},
    /* 108 */ {.blue = 3583, .green = 3583, .red = 3583, .alpha = 16384},
    /* 109 */ {.blue = 3455, .green = 3455, .red = 3455, .alpha = 16384},
    /* 110 */ {.blue = 3327, .green = 3327, .red = 3327, .alpha = 16384},
    /* 111 */ {.blue = 3199, .green = 3199, .red = 3199, .alpha = 16384},
    /* 112 */ {.blue = 3071, .green = 3071, .red = 3071, .alpha = 16384},
    /* 113 */ {.blue = 2943, .green = 2943, .red = 2943, .alpha = 16384},
    /* 114 */ {.blue = 2815, .green = 2815, .red = 2815, .alpha = 16384},
    /* 115 */ {.blue = 2687, .green = 2687, .red = 2687, .alpha = 16384},
    /* 116 */ {.blue = 2559, .green = 2559, .red = 2559, .alpha = 16384},
    /* 117 */ {.blue = 2431, .green = 2431, .red = 2431, .alpha = 16384},
    /* 118 */ {.blue = 2303, .green = 2303, .red = 2303, .alpha = 16384},
    /* 119 */ {.blue = 2175, .green = 2175, .red = 2175, .alpha = 16384},
    /* 120 */ {.blue = 2047, .green = 2047, .red = 2047, .alpha = 16384},
    /* 121 */ {.blue = 1919, .green = 1919, .red = 1919, .alpha = 16384},
    /* 122 */ {.blue = 1791, .green = 1791, .red = 1791, .alpha = 16384},
    /* 123 */ {.blue = 1663, .green = 1663, .red = 1663, .alpha = 16384},
    /* 124 */ {.blue = 1535, .green = 1535, .red = 1535, .alpha = 16384},
    /* 125 */ {.blue = 1407, .green = 1407, .red = 1407, .alpha = 16384},
    /* 126 */ {.blue = 1279, .green = 1279, .red = 1279, .alpha = 16384},
    /* 127 */ {.blue = 1151, .green = 1151, .red = 1151, .alpha = 16384},
    /* 128 */ {.blue = 1023, .green = 1023, .red = 1023, .alpha = 16384},
    /* 129 */ {.blue = 895, .green = 895, .red = 895, .alpha = 16384},
    /* 130 */ {.blue = 767, .green = 767, .red = 767, .alpha = 16384},
    /* 131 */ {.blue = 639, .green = 639, .red = 639, .alpha = 16384},
    /* 132 */ {.blue = 511, .green = 511, .red = 511, .alpha = 16384},
    /* 133 */ {.blue = 383, .green = 383, .red = 383, .alpha = 16384},
    /* 134 */ {.blue = 255, .green = 255, .red = 255, .alpha = 16384},
    /* 135 */ {.blue = 127, .green = 127, .red = 127, .alpha = 16384},
    /* rows 136..681 (MODEL_DISTANCE_ATTENUATION_ROW0, former g_ModelDistanceAttenuationMmx): MMX
       distance attenuation per (light-facing dot >> 21), rows 0..545 */
        /*   0 */ {.alpha = 16384},
        /*   1 */ {.alpha = 16384},
        /*   2 */ {.alpha = 16384},
        /*   3 */ {.alpha = 16384},
        /*   4 */ {.alpha = 16384},
        /*   5 */ {.alpha = 16384},
        /*   6 */ {.alpha = 16384},
        /*   7 */ {.alpha = 16384},
        /*   8 */ {.alpha = 16384},
        /*   9 */ {.alpha = 16384},
        /*  10 */ {.alpha = 16384},
        /*  11 */ {.alpha = 16384},
        /*  12 */ {.alpha = 16384},
        /*  13 */ {.alpha = 16384},
        /*  14 */ {.alpha = 16384},
        /*  15 */ {.alpha = 16384},
        /*  16 */ {.alpha = 16384},
        /*  17 */ {.alpha = 16384},
        /*  18 */ {.alpha = 16384},
        /*  19 */ {.alpha = 16384},
        /*  20 */ {.alpha = 16384},
        /*  21 */ {.alpha = 16384},
        /*  22 */ {.alpha = 16384},
        /*  23 */ {.alpha = 16384},
        /*  24 */ {.alpha = 16384},
        /*  25 */ {.alpha = 16384},
        /*  26 */ {.alpha = 16384},
        /*  27 */ {.alpha = 16384},
        /*  28 */ {.alpha = 16384},
        /*  29 */ {.alpha = 16384},
        /*  30 */ {.alpha = 16384},
        /*  31 */ {.alpha = 16384},
        /*  32 */ {.alpha = 16384},
        /*  33 */ {.alpha = 16384},
        /*  34 */ {.alpha = 16384},
        /*  35 */ {.alpha = 16384},
        /*  36 */ {.alpha = 16384},
        /*  37 */ {.alpha = 16384},
        /*  38 */ {.alpha = 16384},
        /*  39 */ {.alpha = 16384},
        /*  40 */ {.alpha = 16384},
        /*  41 */ {.alpha = 16384},
        /*  42 */ {.alpha = 16384},
        /*  43 */ {.alpha = 16384},
        /*  44 */ {.alpha = 16384},
        /*  45 */ {.alpha = 16384},
        /*  46 */ {.alpha = 16384},
        /*  47 */ {.alpha = 16384},
        /*  48 */ {.alpha = 16384},
        /*  49 */ {.alpha = 16384},
        /*  50 */ {.alpha = 16384},
        /*  51 */ {.alpha = 16384},
        /*  52 */ {.alpha = 16384},
        /*  53 */ {.alpha = 16384},
        /*  54 */ {.alpha = 16384},
        /*  55 */ {.alpha = 16384},
        /*  56 */ {.alpha = 16384},
        /*  57 */ {.alpha = 16384},
        /*  58 */ {.alpha = 16384},
        /*  59 */ {.alpha = 16384},
        /*  60 */ {.alpha = 16384},
        /*  61 */ {.alpha = 16384},
        /*  62 */ {.alpha = 16384},
        /*  63 */ {.alpha = 16384},
        /*  64 */ {.alpha = 16384},
        /*  65 */ {.alpha = 16384},
        /*  66 */ {.alpha = 16384},
        /*  67 */ {.alpha = 16384},
        /*  68 */ {.alpha = 16384},
        /*  69 */ {.alpha = 16384},
        /*  70 */ {.alpha = 16384},
        /*  71 */ {.alpha = 16384},
        /*  72 */ {.alpha = 16384},
        /*  73 */ {.alpha = 16384},
        /*  74 */ {.alpha = 16384},
        /*  75 */ {.alpha = 16384},
        /*  76 */ {.alpha = 16384},
        /*  77 */ {.alpha = 16384},
        /*  78 */ {.alpha = 16384},
        /*  79 */ {.alpha = 16384},
        /*  80 */ {.alpha = 16384},
        /*  81 */ {.alpha = 16384},
        /*  82 */ {.alpha = 16384},
        /*  83 */ {.alpha = 16384},
        /*  84 */ {.alpha = 16384},
        /*  85 */ {.alpha = 16384},
        /*  86 */ {.alpha = 16384},
        /*  87 */ {.alpha = 16384},
        /*  88 */ {.alpha = 16384},
        /*  89 */ {.alpha = 16384},
        /*  90 */ {.alpha = 16384},
        /*  91 */ {.alpha = 16384},
        /*  92 */ {.alpha = 16384},
        /*  93 */ {.alpha = 16384},
        /*  94 */ {.alpha = 16384},
        /*  95 */ {.alpha = 16384},
        /*  96 */ {.alpha = 16384},
        /*  97 */ {.alpha = 16384},
        /*  98 */ {.alpha = 16384},
        /*  99 */ {.alpha = 16384},
        /* 100 */ {.alpha = 16384},
        /* 101 */ {.alpha = 16384},
        /* 102 */ {.alpha = 16384},
        /* 103 */ {.alpha = 16384},
        /* 104 */ {.alpha = 16384},
        /* 105 */ {.alpha = 16384},
        /* 106 */ {.alpha = 16384},
        /* 107 */ {.alpha = 16384},
        /* 108 */ {.alpha = 16384},
        /* 109 */ {.alpha = 16384},
        /* 110 */ {.alpha = 16384},
        /* 111 */ {.alpha = 16384},
        /* 112 */ {.alpha = 16384},
        /* 113 */ {.alpha = 16384},
        /* 114 */ {.alpha = 16384},
        /* 115 */ {.alpha = 16384},
        /* 116 */ {.alpha = 16384},
        /* 117 */ {.alpha = 16384},
        /* 118 */ {.alpha = 16384},
        /* 119 */ {.alpha = 16384},
        /* 120 */ {.alpha = 16384},
        /* 121 */ {.alpha = 16384},
        /* 122 */ {.alpha = 16384},
        /* 123 */ {.alpha = 16384},
        /* 124 */ {.alpha = 16384},
        /* 125 */ {.alpha = 16384},
        /* 126 */ {.alpha = 16384},
        /* 127 */ {.alpha = 16384},
        /* 128 */ {.alpha = 16384},
        /* 129 */ {.alpha = 16384},
        /* 130 */ {.alpha = 16384},
        /* 131 */ {.alpha = 16384},
        /* 132 */ {.alpha = 16384},
        /* 133 */ {.alpha = 16384},
        /* 134 */ {.alpha = 16384},
        /* 135 */ {.alpha = 16384},
        /* 136 */ {.alpha = 16384},
        /* 137 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 138 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 139 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 140 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 141 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 142 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 143 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 144 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 145 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 146 */ {.blue = 16255, .green = 16255, .red = 16255, .alpha = 16384},
        /* 147 */ {.blue = 16127, .green = 16127, .red = 16127, .alpha = 16384},
        /* 148 */ {.blue = 15999, .green = 15999, .red = 15999, .alpha = 16384},
        /* 149 */ {.blue = 15871, .green = 15871, .red = 15871, .alpha = 16384},
        /* 150 */ {.blue = 15743, .green = 15743, .red = 15743, .alpha = 16384},
        /* 151 */ {.blue = 15615, .green = 15615, .red = 15615, .alpha = 16384},
        /* 152 */ {.blue = 15487, .green = 15487, .red = 15487, .alpha = 16384},
        /* 153 */ {.blue = 15359, .green = 15359, .red = 15359, .alpha = 16384},
        /* 154 */ {.blue = 15231, .green = 15231, .red = 15231, .alpha = 16384},
        /* 155 */ {.blue = 15103, .green = 15103, .red = 15103, .alpha = 16384},
        /* 156 */ {.blue = 14975, .green = 14975, .red = 14975, .alpha = 16384},
        /* 157 */ {.blue = 14847, .green = 14847, .red = 14847, .alpha = 16384},
        /* 158 */ {.blue = 14719, .green = 14719, .red = 14719, .alpha = 16384},
        /* 159 */ {.blue = 14591, .green = 14591, .red = 14591, .alpha = 16384},
        /* 160 */ {.blue = 14463, .green = 14463, .red = 14463, .alpha = 16384},
        /* 161 */ {.blue = 14335, .green = 14335, .red = 14335, .alpha = 16384},
        /* 162 */ {.blue = 14207, .green = 14207, .red = 14207, .alpha = 16384},
        /* 163 */ {.blue = 14079, .green = 14079, .red = 14079, .alpha = 16384},
        /* 164 */ {.blue = 13951, .green = 13951, .red = 13951, .alpha = 16384},
        /* 165 */ {.blue = 13823, .green = 13823, .red = 13823, .alpha = 16384},
        /* 166 */ {.blue = 13695, .green = 13695, .red = 13695, .alpha = 16384},
        /* 167 */ {.blue = 13567, .green = 13567, .red = 13567, .alpha = 16384},
        /* 168 */ {.blue = 13439, .green = 13439, .red = 13439, .alpha = 16384},
        /* 169 */ {.blue = 13311, .green = 13311, .red = 13311, .alpha = 16384},
        /* 170 */ {.blue = 13183, .green = 13183, .red = 13183, .alpha = 16384},
        /* 171 */ {.blue = 13055, .green = 13055, .red = 13055, .alpha = 16384},
        /* 172 */ {.blue = 12927, .green = 12927, .red = 12927, .alpha = 16384},
        /* 173 */ {.blue = 12799, .green = 12799, .red = 12799, .alpha = 16384},
        /* 174 */ {.blue = 12671, .green = 12671, .red = 12671, .alpha = 16384},
        /* 175 */ {.blue = 12543, .green = 12543, .red = 12543, .alpha = 16384},
        /* 176 */ {.blue = 12415, .green = 12415, .red = 12415, .alpha = 16384},
        /* 177 */ {.blue = 12287, .green = 12287, .red = 12287, .alpha = 16384},
        /* 178 */ {.blue = 12159, .green = 12159, .red = 12159, .alpha = 16384},
        /* 179 */ {.blue = 12031, .green = 12031, .red = 12031, .alpha = 16384},
        /* 180 */ {.blue = 11903, .green = 11903, .red = 11903, .alpha = 16384},
        /* 181 */ {.blue = 11775, .green = 11775, .red = 11775, .alpha = 16384},
        /* 182 */ {.blue = 11647, .green = 11647, .red = 11647, .alpha = 16384},
        /* 183 */ {.blue = 11519, .green = 11519, .red = 11519, .alpha = 16384},
        /* 184 */ {.blue = 11391, .green = 11391, .red = 11391, .alpha = 16384},
        /* 185 */ {.blue = 11263, .green = 11263, .red = 11263, .alpha = 16384},
        /* 186 */ {.blue = 11135, .green = 11135, .red = 11135, .alpha = 16384},
        /* 187 */ {.blue = 11007, .green = 11007, .red = 11007, .alpha = 16384},
        /* 188 */ {.blue = 10879, .green = 10879, .red = 10879, .alpha = 16384},
        /* 189 */ {.blue = 10751, .green = 10751, .red = 10751, .alpha = 16384},
        /* 190 */ {.blue = 10623, .green = 10623, .red = 10623, .alpha = 16384},
        /* 191 */ {.blue = 10495, .green = 10495, .red = 10495, .alpha = 16384},
        /* 192 */ {.blue = 10367, .green = 10367, .red = 10367, .alpha = 16384},
        /* 193 */ {.blue = 10239, .green = 10239, .red = 10239, .alpha = 16384},
        /* 194 */ {.blue = 10111, .green = 10111, .red = 10111, .alpha = 16384},
        /* 195 */ {.blue = 9983, .green = 9983, .red = 9983, .alpha = 16384},
        /* 196 */ {.blue = 9855, .green = 9855, .red = 9855, .alpha = 16384},
        /* 197 */ {.blue = 9727, .green = 9727, .red = 9727, .alpha = 16384},
        /* 198 */ {.blue = 9599, .green = 9599, .red = 9599, .alpha = 16384},
        /* 199 */ {.blue = 9471, .green = 9471, .red = 9471, .alpha = 16384},
        /* 200 */ {.blue = 9343, .green = 9343, .red = 9343, .alpha = 16384},
        /* 201 */ {.blue = 9215, .green = 9215, .red = 9215, .alpha = 16384},
        /* 202 */ {.blue = 9087, .green = 9087, .red = 9087, .alpha = 16384},
        /* 203 */ {.blue = 8959, .green = 8959, .red = 8959, .alpha = 16384},
        /* 204 */ {.blue = 8831, .green = 8831, .red = 8831, .alpha = 16384},
        /* 205 */ {.blue = 8703, .green = 8703, .red = 8703, .alpha = 16384},
        /* 206 */ {.blue = 8575, .green = 8575, .red = 8575, .alpha = 16384},
        /* 207 */ {.blue = 8447, .green = 8447, .red = 8447, .alpha = 16384},
        /* 208 */ {.blue = 8319, .green = 8319, .red = 8319, .alpha = 16384},
        /* 209 */ {.blue = 8191, .green = 8191, .red = 8191, .alpha = 16384},
        /* 210 */ {.blue = 8063, .green = 8063, .red = 8063, .alpha = 16384},
        /* 211 */ {.blue = 7935, .green = 7935, .red = 7935, .alpha = 16384},
        /* 212 */ {.blue = 7807, .green = 7807, .red = 7807, .alpha = 16384},
        /* 213 */ {.blue = 7679, .green = 7679, .red = 7679, .alpha = 16384},
        /* 214 */ {.blue = 7551, .green = 7551, .red = 7551, .alpha = 16384},
        /* 215 */ {.blue = 7423, .green = 7423, .red = 7423, .alpha = 16384},
        /* 216 */ {.blue = 7295, .green = 7295, .red = 7295, .alpha = 16384},
        /* 217 */ {.blue = 7167, .green = 7167, .red = 7167, .alpha = 16384},
        /* 218 */ {.blue = 7039, .green = 7039, .red = 7039, .alpha = 16384},
        /* 219 */ {.blue = 6911, .green = 6911, .red = 6911, .alpha = 16384},
        /* 220 */ {.blue = 6783, .green = 6783, .red = 6783, .alpha = 16384},
        /* 221 */ {.blue = 6655, .green = 6655, .red = 6655, .alpha = 16384},
        /* 222 */ {.blue = 6527, .green = 6527, .red = 6527, .alpha = 16384},
        /* 223 */ {.blue = 6399, .green = 6399, .red = 6399, .alpha = 16384},
        /* 224 */ {.blue = 6271, .green = 6271, .red = 6271, .alpha = 16384},
        /* 225 */ {.blue = 6143, .green = 6143, .red = 6143, .alpha = 16384},
        /* 226 */ {.blue = 6015, .green = 6015, .red = 6015, .alpha = 16384},
        /* 227 */ {.blue = 5887, .green = 5887, .red = 5887, .alpha = 16384},
        /* 228 */ {.blue = 5759, .green = 5759, .red = 5759, .alpha = 16384},
        /* 229 */ {.blue = 5631, .green = 5631, .red = 5631, .alpha = 16384},
        /* 230 */ {.blue = 5503, .green = 5503, .red = 5503, .alpha = 16384},
        /* 231 */ {.blue = 5375, .green = 5375, .red = 5375, .alpha = 16384},
        /* 232 */ {.blue = 5247, .green = 5247, .red = 5247, .alpha = 16384},
        /* 233 */ {.blue = 5119, .green = 5119, .red = 5119, .alpha = 16384},
        /* 234 */ {.blue = 4991, .green = 4991, .red = 4991, .alpha = 16384},
        /* 235 */ {.blue = 4863, .green = 4863, .red = 4863, .alpha = 16384},
        /* 236 */ {.blue = 4735, .green = 4735, .red = 4735, .alpha = 16384},
        /* 237 */ {.blue = 4607, .green = 4607, .red = 4607, .alpha = 16384},
        /* 238 */ {.blue = 4479, .green = 4479, .red = 4479, .alpha = 16384},
        /* 239 */ {.blue = 4351, .green = 4351, .red = 4351, .alpha = 16384},
        /* 240 */ {.blue = 4223, .green = 4223, .red = 4223, .alpha = 16384},
        /* 241 */ {.blue = 4095, .green = 4095, .red = 4095, .alpha = 16384},
        /* 242 */ {.blue = 3967, .green = 3967, .red = 3967, .alpha = 16384},
        /* 243 */ {.blue = 3839, .green = 3839, .red = 3839, .alpha = 16384},
        /* 244 */ {.blue = 3711, .green = 3711, .red = 3711, .alpha = 16384},
        /* 245 */ {.blue = 3583, .green = 3583, .red = 3583, .alpha = 16384},
        /* 246 */ {.blue = 3455, .green = 3455, .red = 3455, .alpha = 16384},
        /* 247 */ {.blue = 3327, .green = 3327, .red = 3327, .alpha = 16384},
        /* 248 */ {.blue = 3199, .green = 3199, .red = 3199, .alpha = 16384},
        /* 249 */ {.blue = 3071, .green = 3071, .red = 3071, .alpha = 16384},
        /* 250 */ {.blue = 2943, .green = 2943, .red = 2943, .alpha = 16384},
        /* 251 */ {.blue = 2815, .green = 2815, .red = 2815, .alpha = 16384},
        /* 252 */ {.blue = 2687, .green = 2687, .red = 2687, .alpha = 16384},
        /* 253 */ {.blue = 2559, .green = 2559, .red = 2559, .alpha = 16384},
        /* 254 */ {.blue = 2431, .green = 2431, .red = 2431, .alpha = 16384},
        /* 255 */ {.blue = 2303, .green = 2303, .red = 2303, .alpha = 16384},
        /* 256 */ {.blue = 2175, .green = 2175, .red = 2175, .alpha = 16384},
        /* 257 */ {.blue = 2047, .green = 2047, .red = 2047, .alpha = 16384},
        /* 258 */ {.blue = 1919, .green = 1919, .red = 1919, .alpha = 16384},
        /* 259 */ {.blue = 1791, .green = 1791, .red = 1791, .alpha = 16384},
        /* 260 */ {.blue = 1663, .green = 1663, .red = 1663, .alpha = 16384},
        /* 261 */ {.blue = 1535, .green = 1535, .red = 1535, .alpha = 16384},
        /* 262 */ {.blue = 1407, .green = 1407, .red = 1407, .alpha = 16384},
        /* 263 */ {.blue = 1279, .green = 1279, .red = 1279, .alpha = 16384},
        /* 264 */ {.blue = 1151, .green = 1151, .red = 1151, .alpha = 16384},
        /* 265 */ {.blue = 1023, .green = 1023, .red = 1023, .alpha = 16384},
        /* 266 */ {.blue = 895, .green = 895, .red = 895, .alpha = 16384},
        /* 267 */ {.blue = 767, .green = 767, .red = 767, .alpha = 16384},
        /* 268 */ {.blue = 639, .green = 639, .red = 639, .alpha = 16384},
        /* 269 */ {.blue = 511, .green = 511, .red = 511, .alpha = 16384},
        /* 270 */ {.blue = 383, .green = 383, .red = 383, .alpha = 16384},
        /* 271 */ {.blue = 255, .green = 255, .red = 255, .alpha = 16384},
        /* 272 */ {.blue = 127, .green = 127, .red = 127, .alpha = 16384},
        /* 273 */ {.alpha = 16384},
        /* 274 */ {.blue = 127, .green = 127, .red = 127, .alpha = 16384},
        /* 275 */ {.blue = 255, .green = 255, .red = 255, .alpha = 16384},
        /* 276 */ {.blue = 383, .green = 383, .red = 383, .alpha = 16384},
        /* 277 */ {.blue = 511, .green = 511, .red = 511, .alpha = 16384},
        /* 278 */ {.blue = 639, .green = 639, .red = 639, .alpha = 16384},
        /* 279 */ {.blue = 767, .green = 767, .red = 767, .alpha = 16384},
        /* 280 */ {.blue = 895, .green = 895, .red = 895, .alpha = 16384},
        /* 281 */ {.blue = 1023, .green = 1023, .red = 1023, .alpha = 16384},
        /* 282 */ {.blue = 1151, .green = 1151, .red = 1151, .alpha = 16384},
        /* 283 */ {.blue = 1279, .green = 1279, .red = 1279, .alpha = 16384},
        /* 284 */ {.blue = 1407, .green = 1407, .red = 1407, .alpha = 16384},
        /* 285 */ {.blue = 1535, .green = 1535, .red = 1535, .alpha = 16384},
        /* 286 */ {.blue = 1663, .green = 1663, .red = 1663, .alpha = 16384},
        /* 287 */ {.blue = 1791, .green = 1791, .red = 1791, .alpha = 16384},
        /* 288 */ {.blue = 1919, .green = 1919, .red = 1919, .alpha = 16384},
        /* 289 */ {.blue = 2047, .green = 2047, .red = 2047, .alpha = 16384},
        /* 290 */ {.blue = 2175, .green = 2175, .red = 2175, .alpha = 16384},
        /* 291 */ {.blue = 2303, .green = 2303, .red = 2303, .alpha = 16384},
        /* 292 */ {.blue = 2431, .green = 2431, .red = 2431, .alpha = 16384},
        /* 293 */ {.blue = 2559, .green = 2559, .red = 2559, .alpha = 16384},
        /* 294 */ {.blue = 2687, .green = 2687, .red = 2687, .alpha = 16384},
        /* 295 */ {.blue = 2815, .green = 2815, .red = 2815, .alpha = 16384},
        /* 296 */ {.blue = 2943, .green = 2943, .red = 2943, .alpha = 16384},
        /* 297 */ {.blue = 3071, .green = 3071, .red = 3071, .alpha = 16384},
        /* 298 */ {.blue = 3199, .green = 3199, .red = 3199, .alpha = 16384},
        /* 299 */ {.blue = 3327, .green = 3327, .red = 3327, .alpha = 16384},
        /* 300 */ {.blue = 3455, .green = 3455, .red = 3455, .alpha = 16384},
        /* 301 */ {.blue = 3583, .green = 3583, .red = 3583, .alpha = 16384},
        /* 302 */ {.blue = 3711, .green = 3711, .red = 3711, .alpha = 16384},
        /* 303 */ {.blue = 3839, .green = 3839, .red = 3839, .alpha = 16384},
        /* 304 */ {.blue = 3967, .green = 3967, .red = 3967, .alpha = 16384},
        /* 305 */ {.blue = 4095, .green = 4095, .red = 4095, .alpha = 16384},
        /* 306 */ {.blue = 4223, .green = 4223, .red = 4223, .alpha = 16384},
        /* 307 */ {.blue = 4351, .green = 4351, .red = 4351, .alpha = 16384},
        /* 308 */ {.blue = 4479, .green = 4479, .red = 4479, .alpha = 16384},
        /* 309 */ {.blue = 4607, .green = 4607, .red = 4607, .alpha = 16384},
        /* 310 */ {.blue = 4735, .green = 4735, .red = 4735, .alpha = 16384},
        /* 311 */ {.blue = 4863, .green = 4863, .red = 4863, .alpha = 16384},
        /* 312 */ {.blue = 4991, .green = 4991, .red = 4991, .alpha = 16384},
        /* 313 */ {.blue = 5119, .green = 5119, .red = 5119, .alpha = 16384},
        /* 314 */ {.blue = 5247, .green = 5247, .red = 5247, .alpha = 16384},
        /* 315 */ {.blue = 5375, .green = 5375, .red = 5375, .alpha = 16384},
        /* 316 */ {.blue = 5503, .green = 5503, .red = 5503, .alpha = 16384},
        /* 317 */ {.blue = 5631, .green = 5631, .red = 5631, .alpha = 16384},
        /* 318 */ {.blue = 5759, .green = 5759, .red = 5759, .alpha = 16384},
        /* 319 */ {.blue = 5887, .green = 5887, .red = 5887, .alpha = 16384},
        /* 320 */ {.blue = 6015, .green = 6015, .red = 6015, .alpha = 16384},
        /* 321 */ {.blue = 6143, .green = 6143, .red = 6143, .alpha = 16384},
        /* 322 */ {.blue = 6271, .green = 6271, .red = 6271, .alpha = 16384},
        /* 323 */ {.blue = 6399, .green = 6399, .red = 6399, .alpha = 16384},
        /* 324 */ {.blue = 6527, .green = 6527, .red = 6527, .alpha = 16384},
        /* 325 */ {.blue = 6655, .green = 6655, .red = 6655, .alpha = 16384},
        /* 326 */ {.blue = 6783, .green = 6783, .red = 6783, .alpha = 16384},
        /* 327 */ {.blue = 6911, .green = 6911, .red = 6911, .alpha = 16384},
        /* 328 */ {.blue = 7039, .green = 7039, .red = 7039, .alpha = 16384},
        /* 329 */ {.blue = 7167, .green = 7167, .red = 7167, .alpha = 16384},
        /* 330 */ {.blue = 7295, .green = 7295, .red = 7295, .alpha = 16384},
        /* 331 */ {.blue = 7423, .green = 7423, .red = 7423, .alpha = 16384},
        /* 332 */ {.blue = 7551, .green = 7551, .red = 7551, .alpha = 16384},
        /* 333 */ {.blue = 7679, .green = 7679, .red = 7679, .alpha = 16384},
        /* 334 */ {.blue = 7807, .green = 7807, .red = 7807, .alpha = 16384},
        /* 335 */ {.blue = 7935, .green = 7935, .red = 7935, .alpha = 16384},
        /* 336 */ {.blue = 8063, .green = 8063, .red = 8063, .alpha = 16384},
        /* 337 */ {.blue = 8191, .green = 8191, .red = 8191, .alpha = 16384},
        /* 338 */ {.blue = 8319, .green = 8319, .red = 8319, .alpha = 16384},
        /* 339 */ {.blue = 8447, .green = 8447, .red = 8447, .alpha = 16384},
        /* 340 */ {.blue = 8575, .green = 8575, .red = 8575, .alpha = 16384},
        /* 341 */ {.blue = 8703, .green = 8703, .red = 8703, .alpha = 16384},
        /* 342 */ {.blue = 8831, .green = 8831, .red = 8831, .alpha = 16384},
        /* 343 */ {.blue = 8959, .green = 8959, .red = 8959, .alpha = 16384},
        /* 344 */ {.blue = 9087, .green = 9087, .red = 9087, .alpha = 16384},
        /* 345 */ {.blue = 9215, .green = 9215, .red = 9215, .alpha = 16384},
        /* 346 */ {.blue = 9343, .green = 9343, .red = 9343, .alpha = 16384},
        /* 347 */ {.blue = 9471, .green = 9471, .red = 9471, .alpha = 16384},
        /* 348 */ {.blue = 9599, .green = 9599, .red = 9599, .alpha = 16384},
        /* 349 */ {.blue = 9727, .green = 9727, .red = 9727, .alpha = 16384},
        /* 350 */ {.blue = 9855, .green = 9855, .red = 9855, .alpha = 16384},
        /* 351 */ {.blue = 9983, .green = 9983, .red = 9983, .alpha = 16384},
        /* 352 */ {.blue = 10111, .green = 10111, .red = 10111, .alpha = 16384},
        /* 353 */ {.blue = 10239, .green = 10239, .red = 10239, .alpha = 16384},
        /* 354 */ {.blue = 10367, .green = 10367, .red = 10367, .alpha = 16384},
        /* 355 */ {.blue = 10495, .green = 10495, .red = 10495, .alpha = 16384},
        /* 356 */ {.blue = 10623, .green = 10623, .red = 10623, .alpha = 16384},
        /* 357 */ {.blue = 10751, .green = 10751, .red = 10751, .alpha = 16384},
        /* 358 */ {.blue = 10879, .green = 10879, .red = 10879, .alpha = 16384},
        /* 359 */ {.blue = 11007, .green = 11007, .red = 11007, .alpha = 16384},
        /* 360 */ {.blue = 11135, .green = 11135, .red = 11135, .alpha = 16384},
        /* 361 */ {.blue = 11263, .green = 11263, .red = 11263, .alpha = 16384},
        /* 362 */ {.blue = 11391, .green = 11391, .red = 11391, .alpha = 16384},
        /* 363 */ {.blue = 11519, .green = 11519, .red = 11519, .alpha = 16384},
        /* 364 */ {.blue = 11647, .green = 11647, .red = 11647, .alpha = 16384},
        /* 365 */ {.blue = 11775, .green = 11775, .red = 11775, .alpha = 16384},
        /* 366 */ {.blue = 11903, .green = 11903, .red = 11903, .alpha = 16384},
        /* 367 */ {.blue = 12031, .green = 12031, .red = 12031, .alpha = 16384},
        /* 368 */ {.blue = 12159, .green = 12159, .red = 12159, .alpha = 16384},
        /* 369 */ {.blue = 12287, .green = 12287, .red = 12287, .alpha = 16384},
        /* 370 */ {.blue = 12415, .green = 12415, .red = 12415, .alpha = 16384},
        /* 371 */ {.blue = 12543, .green = 12543, .red = 12543, .alpha = 16384},
        /* 372 */ {.blue = 12671, .green = 12671, .red = 12671, .alpha = 16384},
        /* 373 */ {.blue = 12799, .green = 12799, .red = 12799, .alpha = 16384},
        /* 374 */ {.blue = 12927, .green = 12927, .red = 12927, .alpha = 16384},
        /* 375 */ {.blue = 13055, .green = 13055, .red = 13055, .alpha = 16384},
        /* 376 */ {.blue = 13183, .green = 13183, .red = 13183, .alpha = 16384},
        /* 377 */ {.blue = 13311, .green = 13311, .red = 13311, .alpha = 16384},
        /* 378 */ {.blue = 13439, .green = 13439, .red = 13439, .alpha = 16384},
        /* 379 */ {.blue = 13567, .green = 13567, .red = 13567, .alpha = 16384},
        /* 380 */ {.blue = 13695, .green = 13695, .red = 13695, .alpha = 16384},
        /* 381 */ {.blue = 13823, .green = 13823, .red = 13823, .alpha = 16384},
        /* 382 */ {.blue = 13951, .green = 13951, .red = 13951, .alpha = 16384},
        /* 383 */ {.blue = 14079, .green = 14079, .red = 14079, .alpha = 16384},
        /* 384 */ {.blue = 14207, .green = 14207, .red = 14207, .alpha = 16384},
        /* 385 */ {.blue = 14335, .green = 14335, .red = 14335, .alpha = 16384},
        /* 386 */ {.blue = 14463, .green = 14463, .red = 14463, .alpha = 16384},
        /* 387 */ {.blue = 14591, .green = 14591, .red = 14591, .alpha = 16384},
        /* 388 */ {.blue = 14719, .green = 14719, .red = 14719, .alpha = 16384},
        /* 389 */ {.blue = 14847, .green = 14847, .red = 14847, .alpha = 16384},
        /* 390 */ {.blue = 14975, .green = 14975, .red = 14975, .alpha = 16384},
        /* 391 */ {.blue = 15103, .green = 15103, .red = 15103, .alpha = 16384},
        /* 392 */ {.blue = 15231, .green = 15231, .red = 15231, .alpha = 16384},
        /* 393 */ {.blue = 15359, .green = 15359, .red = 15359, .alpha = 16384},
        /* 394 */ {.blue = 15487, .green = 15487, .red = 15487, .alpha = 16384},
        /* 395 */ {.blue = 15615, .green = 15615, .red = 15615, .alpha = 16384},
        /* 396 */ {.blue = 15743, .green = 15743, .red = 15743, .alpha = 16384},
        /* 397 */ {.blue = 15871, .green = 15871, .red = 15871, .alpha = 16384},
        /* 398 */ {.blue = 15999, .green = 15999, .red = 15999, .alpha = 16384},
        /* 399 */ {.blue = 16127, .green = 16127, .red = 16127, .alpha = 16384},
        /* 400 */ {.blue = 16255, .green = 16255, .red = 16255, .alpha = 16384},
        /* 401 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 402 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 403 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 404 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 405 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 406 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 407 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 408 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 409 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 410 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 411 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 412 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 413 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 414 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 415 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 416 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 417 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 418 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 419 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 420 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 421 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 422 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 423 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 424 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 425 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 426 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 427 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 428 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 429 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 430 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 431 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 432 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 433 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 434 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 435 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 436 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 437 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 438 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 439 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 440 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 441 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 442 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 443 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 444 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 445 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 446 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 447 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 448 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 449 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 450 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 451 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 452 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 453 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 454 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 455 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 456 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 457 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 458 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 459 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 460 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 461 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 462 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 463 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 464 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 465 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 466 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 467 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 468 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 469 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 470 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 471 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 472 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 473 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 474 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 475 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 476 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 477 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 478 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 479 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 480 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 481 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 482 */ {.blue = 16383, .green = 16383, .red = 16383, .alpha = 16384},
        /* 483 */ {.blue = 16255, .green = 16255, .red = 16255, .alpha = 16384},
        /* 484 */ {.blue = 16127, .green = 16127, .red = 16127, .alpha = 16384},
        /* 485 */ {.blue = 15999, .green = 15999, .red = 15999, .alpha = 16384},
        /* 486 */ {.blue = 15871, .green = 15871, .red = 15871, .alpha = 16384},
        /* 487 */ {.blue = 15743, .green = 15743, .red = 15743, .alpha = 16384},
        /* 488 */ {.blue = 15615, .green = 15615, .red = 15615, .alpha = 16384},
        /* 489 */ {.blue = 15487, .green = 15487, .red = 15487, .alpha = 16384},
        /* 490 */ {.blue = 15359, .green = 15359, .red = 15359, .alpha = 16384},
        /* 491 */ {.blue = 15231, .green = 15231, .red = 15231, .alpha = 16384},
        /* 492 */ {.blue = 15103, .green = 15103, .red = 15103, .alpha = 16384},
        /* 493 */ {.blue = 14975, .green = 14975, .red = 14975, .alpha = 16384},
        /* 494 */ {.blue = 14847, .green = 14847, .red = 14847, .alpha = 16384},
        /* 495 */ {.blue = 14719, .green = 14719, .red = 14719, .alpha = 16384},
        /* 496 */ {.blue = 14591, .green = 14591, .red = 14591, .alpha = 16384},
        /* 497 */ {.blue = 14463, .green = 14463, .red = 14463, .alpha = 16384},
        /* 498 */ {.blue = 14335, .green = 14335, .red = 14335, .alpha = 16384},
        /* 499 */ {.blue = 14207, .green = 14207, .red = 14207, .alpha = 16384},
        /* 500 */ {.blue = 14079, .green = 14079, .red = 14079, .alpha = 16384},
        /* 501 */ {.blue = 13951, .green = 13951, .red = 13951, .alpha = 16384},
        /* 502 */ {.blue = 13823, .green = 13823, .red = 13823, .alpha = 16384},
        /* 503 */ {.blue = 13695, .green = 13695, .red = 13695, .alpha = 16384},
        /* 504 */ {.blue = 13567, .green = 13567, .red = 13567, .alpha = 16384},
        /* 505 */ {.blue = 13439, .green = 13439, .red = 13439, .alpha = 16384},
        /* 506 */ {.blue = 13311, .green = 13311, .red = 13311, .alpha = 16384},
        /* 507 */ {.blue = 13183, .green = 13183, .red = 13183, .alpha = 16384},
        /* 508 */ {.blue = 13055, .green = 13055, .red = 13055, .alpha = 16384},
        /* 509 */ {.blue = 12927, .green = 12927, .red = 12927, .alpha = 16384},
        /* 510 */ {.blue = 12799, .green = 12799, .red = 12799, .alpha = 16384},
        /* 511 */ {.blue = 12671, .green = 12671, .red = 12671, .alpha = 16384},
        /* 512 */ {.blue = 12543, .green = 12543, .red = 12543, .alpha = 16384},
        /* 513 */ {.blue = 12415, .green = 12415, .red = 12415, .alpha = 16384},
        /* 514 */ {.blue = 12287, .green = 12287, .red = 12287, .alpha = 16384},
        /* 515 */ {.blue = 12159, .green = 12159, .red = 12159, .alpha = 16384},
        /* 516 */ {.blue = 12031, .green = 12031, .red = 12031, .alpha = 16384},
        /* 517 */ {.blue = 11903, .green = 11903, .red = 11903, .alpha = 16384},
        /* 518 */ {.blue = 11775, .green = 11775, .red = 11775, .alpha = 16384},
        /* 519 */ {.blue = 11647, .green = 11647, .red = 11647, .alpha = 16384},
        /* 520 */ {.blue = 11519, .green = 11519, .red = 11519, .alpha = 16384},
        /* 521 */ {.blue = 11391, .green = 11391, .red = 11391, .alpha = 16384},
        /* 522 */ {.blue = 11263, .green = 11263, .red = 11263, .alpha = 16384},
        /* 523 */ {.blue = 11135, .green = 11135, .red = 11135, .alpha = 16384},
        /* 524 */ {.blue = 11007, .green = 11007, .red = 11007, .alpha = 16384},
        /* 525 */ {.blue = 10879, .green = 10879, .red = 10879, .alpha = 16384},
        /* 526 */ {.blue = 10751, .green = 10751, .red = 10751, .alpha = 16384},
        /* 527 */ {.blue = 10623, .green = 10623, .red = 10623, .alpha = 16384},
        /* 528 */ {.blue = 10495, .green = 10495, .red = 10495, .alpha = 16384},
        /* 529 */ {.blue = 10367, .green = 10367, .red = 10367, .alpha = 16384},
        /* 530 */ {.blue = 10239, .green = 10239, .red = 10239, .alpha = 16384},
        /* 531 */ {.blue = 10111, .green = 10111, .red = 10111, .alpha = 16384},
        /* 532 */ {.blue = 9983, .green = 9983, .red = 9983, .alpha = 16384},
        /* 533 */ {.blue = 9855, .green = 9855, .red = 9855, .alpha = 16384},
        /* 534 */ {.blue = 9727, .green = 9727, .red = 9727, .alpha = 16384},
        /* 535 */ {.blue = 9599, .green = 9599, .red = 9599, .alpha = 16384},
        /* 536 */ {.blue = 9471, .green = 9471, .red = 9471, .alpha = 16384},
        /* 537 */ {.blue = 9343, .green = 9343, .red = 9343, .alpha = 16384},
        /* 538 */ {.blue = 9215, .green = 9215, .red = 9215, .alpha = 16384},
        /* 539 */ {.blue = 9087, .green = 9087, .red = 9087, .alpha = 16384},
        /* 540 */ {.blue = 8959, .green = 8959, .red = 8959, .alpha = 16384},
        /* 541 */ {.blue = 8831, .green = 8831, .red = 8831, .alpha = 16384},
        /* 542 */ {.blue = 8703, .green = 8703, .red = 8703, .alpha = 16384},
        /* 543 */ {.blue = 8575, .green = 8575, .red = 8575, .alpha = 16384},
        /* 544 */ {.blue = 8447, .green = 8447, .red = 8447, .alpha = 16384},
        /* 545 */ {.blue = 8319, .green = 8319, .red = 8319, .alpha = 16384},
    /* rows 682..818 (MODEL_LIGHTING_SCALE_ROW0, former g_ModelLightingScaleMmxMultiplierTable):
       multipliers for ModelRender_ComputeVertexIntensityScaledPath, rows 0..136; B/G/R lanes 0x1FFF at row 0
       falling by 0x80 to 0x007F, then 0 */
    /*   0 */ {.blue = 8191, .green = 8191, .red = 8191, .alpha = 16384},
    /*   1 */ {.blue = 8063, .green = 8063, .red = 8063, .alpha = 16384},
    /*   2 */ {.blue = 7935, .green = 7935, .red = 7935, .alpha = 16384},
    /*   3 */ {.blue = 7807, .green = 7807, .red = 7807, .alpha = 16384},
    /*   4 */ {.blue = 7679, .green = 7679, .red = 7679, .alpha = 16384},
    /*   5 */ {.blue = 7551, .green = 7551, .red = 7551, .alpha = 16384},
    /*   6 */ {.blue = 7423, .green = 7423, .red = 7423, .alpha = 16384},
    /*   7 */ {.blue = 7295, .green = 7295, .red = 7295, .alpha = 16384},
    /*   8 */ {.blue = 7167, .green = 7167, .red = 7167, .alpha = 16384},
    /*   9 */ {.blue = 7039, .green = 7039, .red = 7039, .alpha = 16384},
    /*  10 */ {.blue = 6911, .green = 6911, .red = 6911, .alpha = 16384},
    /*  11 */ {.blue = 6783, .green = 6783, .red = 6783, .alpha = 16384},
    /*  12 */ {.blue = 6655, .green = 6655, .red = 6655, .alpha = 16384},
    /*  13 */ {.blue = 6527, .green = 6527, .red = 6527, .alpha = 16384},
    /*  14 */ {.blue = 6399, .green = 6399, .red = 6399, .alpha = 16384},
    /*  15 */ {.blue = 6271, .green = 6271, .red = 6271, .alpha = 16384},
    /*  16 */ {.blue = 6143, .green = 6143, .red = 6143, .alpha = 16384},
    /*  17 */ {.blue = 6015, .green = 6015, .red = 6015, .alpha = 16384},
    /*  18 */ {.blue = 5887, .green = 5887, .red = 5887, .alpha = 16384},
    /*  19 */ {.blue = 5759, .green = 5759, .red = 5759, .alpha = 16384},
    /*  20 */ {.blue = 5631, .green = 5631, .red = 5631, .alpha = 16384},
    /*  21 */ {.blue = 5503, .green = 5503, .red = 5503, .alpha = 16384},
    /*  22 */ {.blue = 5375, .green = 5375, .red = 5375, .alpha = 16384},
    /*  23 */ {.blue = 5247, .green = 5247, .red = 5247, .alpha = 16384},
    /*  24 */ {.blue = 5119, .green = 5119, .red = 5119, .alpha = 16384},
    /*  25 */ {.blue = 4991, .green = 4991, .red = 4991, .alpha = 16384},
    /*  26 */ {.blue = 4863, .green = 4863, .red = 4863, .alpha = 16384},
    /*  27 */ {.blue = 4735, .green = 4735, .red = 4735, .alpha = 16384},
    /*  28 */ {.blue = 4607, .green = 4607, .red = 4607, .alpha = 16384},
    /*  29 */ {.blue = 4479, .green = 4479, .red = 4479, .alpha = 16384},
    /*  30 */ {.blue = 4351, .green = 4351, .red = 4351, .alpha = 16384},
    /*  31 */ {.blue = 4223, .green = 4223, .red = 4223, .alpha = 16384},
    /*  32 */ {.blue = 4095, .green = 4095, .red = 4095, .alpha = 16384},
    /*  33 */ {.blue = 3967, .green = 3967, .red = 3967, .alpha = 16384},
    /*  34 */ {.blue = 3839, .green = 3839, .red = 3839, .alpha = 16384},
    /*  35 */ {.blue = 3711, .green = 3711, .red = 3711, .alpha = 16384},
    /*  36 */ {.blue = 3583, .green = 3583, .red = 3583, .alpha = 16384},
    /*  37 */ {.blue = 3455, .green = 3455, .red = 3455, .alpha = 16384},
    /*  38 */ {.blue = 3327, .green = 3327, .red = 3327, .alpha = 16384},
    /*  39 */ {.blue = 3199, .green = 3199, .red = 3199, .alpha = 16384},
    /*  40 */ {.blue = 3071, .green = 3071, .red = 3071, .alpha = 16384},
    /*  41 */ {.blue = 2943, .green = 2943, .red = 2943, .alpha = 16384},
    /*  42 */ {.blue = 2815, .green = 2815, .red = 2815, .alpha = 16384},
    /*  43 */ {.blue = 2687, .green = 2687, .red = 2687, .alpha = 16384},
    /*  44 */ {.blue = 2559, .green = 2559, .red = 2559, .alpha = 16384},
    /*  45 */ {.blue = 2431, .green = 2431, .red = 2431, .alpha = 16384},
    /*  46 */ {.blue = 2303, .green = 2303, .red = 2303, .alpha = 16384},
    /*  47 */ {.blue = 2175, .green = 2175, .red = 2175, .alpha = 16384},
    /*  48 */ {.blue = 2047, .green = 2047, .red = 2047, .alpha = 16384},
    /*  49 */ {.blue = 1919, .green = 1919, .red = 1919, .alpha = 16384},
    /*  50 */ {.blue = 1791, .green = 1791, .red = 1791, .alpha = 16384},
    /*  51 */ {.blue = 1663, .green = 1663, .red = 1663, .alpha = 16384},
    /*  52 */ {.blue = 1535, .green = 1535, .red = 1535, .alpha = 16384},
    /*  53 */ {.blue = 1407, .green = 1407, .red = 1407, .alpha = 16384},
    /*  54 */ {.blue = 1279, .green = 1279, .red = 1279, .alpha = 16384},
    /*  55 */ {.blue = 1151, .green = 1151, .red = 1151, .alpha = 16384},
    /*  56 */ {.blue = 1023, .green = 1023, .red = 1023, .alpha = 16384},
    /*  57 */ {.blue = 895, .green = 895, .red = 895, .alpha = 16384},
    /*  58 */ {.blue = 767, .green = 767, .red = 767, .alpha = 16384},
    /*  59 */ {.blue = 639, .green = 639, .red = 639, .alpha = 16384},
    /*  60 */ {.blue = 511, .green = 511, .red = 511, .alpha = 16384},
    /*  61 */ {.blue = 383, .green = 383, .red = 383, .alpha = 16384},
    /*  62 */ {.blue = 255, .green = 255, .red = 255, .alpha = 16384},
    /*  63 */ {.blue = 127, .green = 127, .red = 127, .alpha = 16384},
    /*  64 */ {.alpha = 16384},
    /*  65 */ {.alpha = 16384},
    /*  66 */ {.alpha = 16384},
    /*  67 */ {.alpha = 16384},
    /*  68 */ {.alpha = 16384},
    /*  69 */ {.alpha = 16384},
    /*  70 */ {.alpha = 16384},
    /*  71 */ {.alpha = 16384},
    /*  72 */ {.alpha = 16384},
    /*  73 */ {.alpha = 16384},
    /*  74 */ {.alpha = 16384},
    /*  75 */ {.alpha = 16384},
    /*  76 */ {.alpha = 16384},
    /*  77 */ {.alpha = 16384},
    /*  78 */ {.alpha = 16384},
    /*  79 */ {.alpha = 16384},
    /*  80 */ {.alpha = 16384},
    /*  81 */ {.alpha = 16384},
    /*  82 */ {.alpha = 16384},
    /*  83 */ {.alpha = 16384},
    /*  84 */ {.alpha = 16384},
    /*  85 */ {.alpha = 16384},
    /*  86 */ {.alpha = 16384},
    /*  87 */ {.alpha = 16384},
    /*  88 */ {.alpha = 16384},
    /*  89 */ {.alpha = 16384},
    /*  90 */ {.alpha = 16384},
    /*  91 */ {.alpha = 16384},
    /*  92 */ {.alpha = 16384},
    /*  93 */ {.alpha = 16384},
    /*  94 */ {.alpha = 16384},
    /*  95 */ {.alpha = 16384},
    /*  96 */ {.alpha = 16384},
    /*  97 */ {.alpha = 16384},
    /*  98 */ {.alpha = 16384},
    /*  99 */ {.alpha = 16384},
    /* 100 */ {.alpha = 16384},
    /* 101 */ {.alpha = 16384},
    /* 102 */ {.alpha = 16384},
    /* 103 */ {.alpha = 16384},
    /* 104 */ {.alpha = 16384},
    /* 105 */ {.alpha = 16384},
    /* 106 */ {.alpha = 16384},
    /* 107 */ {.alpha = 16384},
    /* 108 */ {.alpha = 16384},
    /* 109 */ {.alpha = 16384},
    /* 110 */ {.alpha = 16384},
    /* 111 */ {.alpha = 16384},
    /* 112 */ {.alpha = 16384},
    /* 113 */ {.alpha = 16384},
    /* 114 */ {.alpha = 16384},
    /* 115 */ {.alpha = 16384},
    /* 116 */ {.alpha = 16384},
    /* 117 */ {.alpha = 16384},
    /* 118 */ {.alpha = 16384},
    /* 119 */ {.alpha = 16384},
    /* 120 */ {.alpha = 16384},
    /* 121 */ {.alpha = 16384},
    /* 122 */ {.alpha = 16384},
    /* 123 */ {.alpha = 16384},
    /* 124 */ {.alpha = 16384},
    /* 125 */ {.alpha = 16384},
    /* 126 */ {.alpha = 16384},
    /* 127 */ {.alpha = 16384},
    /* 128 */ {.alpha = 16384},
    /* 129 */ {.alpha = 16384},
    /* 130 */ {.alpha = 16384},
    /* 131 */ {.alpha = 16384},
    /* 132 */ {.alpha = 16384},
    /* 133 */ {.alpha = 16384},
    /* 134 */ {.alpha = 16384},
    /* 135 */ {.alpha = 16384},
    /* 136 */ {.alpha = 16384}};

GraphicsFixedVec3 g_ModelLightingVertexToLightVectorScratch = {0};

GraphicsFixedVec3 g_ModelLightingTransformedSurfaceNormalScratch = {0};

/* Implementation ownership: graphics/render/model_lighting. */


/* MMX lane helpers for the rewritten lighting routines (Intel SDM semantics). These are C helpers, not
   functions of the original executable. */

/* movd + punpcklbw mm,mm + psrlw mm,shift: each byte b becomes the word (b * 0x101) >> shift. */
static void ModelLighting_UnpackBytes(uint32_t packed, int shift, short lanes[4])
{
  int i;
  for (i = 0; i < 4; i++) {
    lanes[i] = (short)((((packed >> (8 * i)) & ARGB8888_CHANNEL_MASK) * COLOR_CHANNEL_TO_WORD_LANE) >> shift);
  }
}

/* pmulhw */
static void ModelLighting_MulHigh(short lanes[4], const short factors[4])
{
  int i;
  for (i = 0; i < 4; i++) {
    lanes[i] = (short)(((int)lanes[i] * (int)factors[i]) >> 16);
  }
}

/* paddsw */
static void ModelLighting_AddSaturate(short lanes[4], const short addends[4])
{
  int i;
  for (i = 0; i < 4; i++) {
    int sum = (int)lanes[i] + (int)addends[i];
    lanes[i] = (short)(sum > 32767 ? 32767 : sum < -32768 ? -32768 : sum);
  }
}

/* packuswb mm,mm + movd */
static PackedArgb32 ModelLighting_PackUnsigned(const short lanes[4])
{
  uint32_t packed = 0;
  int i;
  for (i = 0; i < 4; i++) {
    int v = lanes[i] < 0 ? 0 : lanes[i] > ARGB8888_CHANNEL_MAX ? ARGB8888_CHANNEL_MAX : lanes[i];
    packed |= (uint32_t)v << (8 * i);
  }
  return packed;
}

/* The same lane operations on 64-bit MMX register images (ThandorMmx, core/x86_emulation.h). */

/* movd + punpcklbw mm,mm + psrlw mm,shift: byte k of packed becomes word lane k = (b * 0x101) >> shift. */
static __inline uint64_t ModelLighting_UnpackBytesMmx(uint32_t packed, int shift)
{
  ThandorMmx lanes;
  int i;
  for (i = 0; i < 4; i++) {
    lanes.uw[i] = (uint16_t)((((packed >> (8 * i)) & ARGB8888_CHANNEL_MASK) * COLOR_CHANNEL_TO_WORD_LANE) >> shift);
  }
  return lanes.q;
}

/* paddw (wrapping word add) */
static __inline uint64_t ModelLighting_AddWordsMmx(uint64_t a, uint64_t b)
{
  ThandorMmx x, y, r;
  int i;
  x.q = a;
  y.q = b;
  for (i = 0; i < 4; i++) {
    r.uw[i] = (uint16_t)(x.uw[i] + y.uw[i]);
  }
  return r.q;
}

/* packuswb mm,mm + movd */
static __inline PackedArgb32 ModelLighting_PackUnsignedMmx(uint64_t lanes)
{
  ThandorMmx x;
  x.q = lanes;
  return ModelLighting_PackUnsigned(x.sw);
}













/* Original quirk: ModelRender_ComputeVertexIntensityDefaultPath reads its directional weight (the PMULHW
   multiplier row) at distanceAttenuationTable (0x004CB1A0, row MODEL_DISTANCE_ATTENUATION_ROW0 of
   g_ModelLightingMmxMultiplierRows) + (dot >> 21) * 8 without a range check. FixedVec3_DotQ12 returns
   the low dword of the 64-bit sum >> 12, so dot is any int32 and the row offset any value in -1024..1023:
   the original reads 8 bytes anywhere in 0x004C91A0..0x004CD1A0, while the table covers only offsets -136..682.
   With the Q28 light direction (g_ModelAuxiliaryForwardDirectionLocal) and a normal of L units (Q12) the offset is
   floor(128 * L * cos): it leaves the table for L * cos < -1.0625 or > 5.33 and the dot wraps from |L * cos| >= 8
   (normals that are not unit length in the model data, or a node transform whose basis lengthens the direction).
   ModelLighting_ReadOriginalImageQword rebuilds those bytes from the variables that hold the original addresses
   today (live values) and the original machine code and 0x90 filler between them (constant, copied from the
   original executable's .text; nothing writes there), so the result does not depend on the linker's layout.
   Each original byte comes from its own range, so a qword spanning two variables (0x004CAD50, 0x004CC700) is
   assembled like the original read. */
#define MODEL_LIGHTING_MMX_ROWS_ORIGINAL_ADDRESS 0x004CAD60u

/* original 0x004CAD58..0x004CAD60: 0x90 filler between g_GraphicsShadingNearbyRecordCount and the table */
static const uint8_t s_ModelLightingOriginalFiller004CAD58[0x8] = {
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 /* 004CAD58 */
};
/* original 0x004CC710..0x004CCE00: machine code of 0x004CC710 (ModelRender_ComputeVertexIntensityDefaultPath),
   0x004CC820 (ModelRender_ComputeVertexIntensityScaledPath) and the following shading routines up to 0x004CCDEB,
   then 0x90 filler up to 0x004CCE00 */
static const uint8_t s_ModelLightingOriginalCode004CC710[0x6F0] = {
  0x53, 0x51, 0x52, 0x57, 0x56, 0x55, 0x89, 0xE5, 0xFF, 0x75, 0x38, 0xFF, 0x75, 0x30, 0xE8, 0x8D, /* 004CC710 */
  0x8F, 0xFB, 0xFF, 0x0F, 0x6E, 0x45, 0x2C, 0xC1, 0xF8, 0x15, 0x0F, 0x60, 0xC0, 0x8B, 0x55, 0x24, /* 004CC720 */
  0x0F, 0x71, 0xD0, 0x02, 0x0F, 0x6E, 0x55, 0x28, 0x0F, 0x6E, 0x5D, 0x34, 0x0F, 0x60, 0xD2, 0x0F, /* 004CC730 */
  0x60, 0xDB, 0x0F, 0xE5, 0x04, 0xC2, 0x0F, 0x71, 0xD2, 0x04, 0x0F, 0x71, 0xD3, 0x02, 0x8B, 0x0D, /* 004CC740 */
  0x54, 0xAD, 0x4C, 0x00, 0x0F, 0xFD, 0xC2, 0xBF, 0x54, 0x6D, 0x4C, 0x00, 0x0F, 0xE5, 0xC3, 0x0F, /* 004CC750 */
  0x6E, 0x6D, 0x1C, 0x85, 0xC9, 0x0F, 0x84, 0x8C, 0x00, 0x00, 0x00, 0x8B, 0x75, 0x20, 0x90, 0x90, /* 004CC760 */
  0x83, 0x7F, 0x20, 0x00, 0x0F, 0x84, 0x70, 0x00, 0x00, 0x00, 0x51, 0x8B, 0x86, 0x00, 0x00, 0x00, /* 004CC770 */
  0x00, 0x8B, 0x5F, 0x10, 0x8B, 0x4F, 0x14, 0x2B, 0x87, 0x00, 0x00, 0x00, 0x00, 0xF7, 0xE8, 0x2B, /* 004CC780 */
  0xD8, 0x1B, 0xCA, 0x78, 0x54, 0x8B, 0x86, 0x04, 0x00, 0x00, 0x00, 0x2B, 0x87, 0x04, 0x00, 0x00, /* 004CC790 */
  0x00, 0xF7, 0xE8, 0x2B, 0xD8, 0x1B, 0xCA, 0x78, 0x40, 0x8B, 0x86, 0x08, 0x00, 0x00, 0x00, 0x2B, /* 004CC7A0 */
  0x87, 0x08, 0x00, 0x00, 0x00, 0xF7, 0xE8, 0x2B, 0xD8, 0x1B, 0xCA, 0x78, 0x2C, 0x0F, 0xA4, 0xD9, /* 004CC7B0 */
  0x1B, 0x8B, 0x57, 0x10, 0x8B, 0x5F, 0x14, 0x8B, 0xC1, 0x0F, 0x6E, 0x67, 0x0C, 0x0F, 0xA4, 0xD3, /* 004CC7C0 */
  0x14, 0x74, 0x16, 0x0F, 0x60, 0xE4, 0x33, 0xD2, 0x0F, 0x71, 0xD4, 0x02, 0xF7, 0xF3, 0x0F, 0xE5, /* 004CC7D0 */
  0x24, 0xC5, 0x80, 0xDE, 0x41, 0x00, 0x0F, 0xDC, 0xC4, 0x59, 0x81, 0xC7, 0x40, 0x00, 0x00, 0x00, /* 004CC7E0 */
  0x49, 0x0F, 0x85, 0x79, 0xFF, 0xFF, 0xFF, 0x0F, 0x60, 0xED, 0x0F, 0x71, 0xD5, 0x02, 0x0F, 0xE5, /* 004CC7F0 */
  0xC5, 0x0F, 0x67, 0xC0, 0x0F, 0x7E, 0xC0, 0x89, 0xEC, 0x5D, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0xC2, /* 004CC800 */
  0x20, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CC810 */
  0x53, 0x51, 0x52, 0x57, 0x56, 0x55, 0x89, 0xE5, 0xFF, 0x75, 0x38, 0xFF, 0x75, 0x30, 0xE8, 0x7D, /* 004CC820 */
  0x8E, 0xFB, 0xFF, 0x99, 0xF7, 0x7D, 0x24, 0x0F, 0x6E, 0x45, 0x2C, 0xC1, 0xF8, 0x09, 0x0F, 0x60, /* 004CC830 */
  0xC0, 0x0F, 0x71, 0xD0, 0x02, 0x0F, 0x6E, 0x55, 0x28, 0x0F, 0x6E, 0x5D, 0x34, 0x0F, 0x60, 0xD2, /* 004CC840 */
  0x0F, 0x60, 0xDB, 0x0F, 0xE5, 0x04, 0xC5, 0xB0, 0xC2, 0x4C, 0x00, 0x0F, 0x71, 0xD2, 0x04, 0x0F, /* 004CC850 */
  0x71, 0xD3, 0x02, 0x8B, 0x0D, 0x54, 0xAD, 0x4C, 0x00, 0x0F, 0xFD, 0xC2, 0xBF, 0x54, 0x6D, 0x4C, /* 004CC860 */
  0x00, 0x0F, 0xE5, 0xC3, 0x0F, 0x6E, 0x6D, 0x1C, 0x85, 0xC9, 0x0F, 0x84, 0x97, 0x00, 0x00, 0x00, /* 004CC870 */
  0x8B, 0x75, 0x20, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CC880 */
  0x83, 0x7F, 0x20, 0x00, 0x0F, 0x84, 0x70, 0x00, 0x00, 0x00, 0x51, 0x8B, 0x86, 0x00, 0x00, 0x00, /* 004CC890 */
  0x00, 0x8B, 0x5F, 0x10, 0x8B, 0x4F, 0x14, 0x2B, 0x87, 0x00, 0x00, 0x00, 0x00, 0xF7, 0xE8, 0x2B, /* 004CC8A0 */
  0xD8, 0x1B, 0xCA, 0x78, 0x54, 0x8B, 0x86, 0x04, 0x00, 0x00, 0x00, 0x2B, 0x87, 0x04, 0x00, 0x00, /* 004CC8B0 */
  0x00, 0xF7, 0xE8, 0x2B, 0xD8, 0x1B, 0xCA, 0x78, 0x40, 0x8B, 0x86, 0x08, 0x00, 0x00, 0x00, 0x2B, /* 004CC8C0 */
  0x87, 0x08, 0x00, 0x00, 0x00, 0xF7, 0xE8, 0x2B, 0xD8, 0x1B, 0xCA, 0x78, 0x2C, 0x0F, 0xA4, 0xD9, /* 004CC8D0 */
  0x1B, 0x8B, 0x57, 0x10, 0x8B, 0x5F, 0x14, 0x8B, 0xC1, 0x0F, 0x6E, 0x67, 0x0C, 0x0F, 0xA4, 0xD3, /* 004CC8E0 */
  0x14, 0x74, 0x16, 0x0F, 0x60, 0xE4, 0x33, 0xD2, 0x0F, 0x71, 0xD4, 0x02, 0xF7, 0xF3, 0x0F, 0xE5, /* 004CC8F0 */
  0x24, 0xC5, 0x80, 0xDE, 0x41, 0x00, 0x0F, 0xDC, 0xC4, 0x59, 0x81, 0xC7, 0x40, 0x00, 0x00, 0x00, /* 004CC900 */
  0x49, 0x0F, 0x85, 0x79, 0xFF, 0xFF, 0xFF, 0x0F, 0x60, 0xED, 0x0F, 0x71, 0xD5, 0x02, 0x0F, 0xE5, /* 004CC910 */
  0xC5, 0x0F, 0x67, 0xC0, 0x0F, 0x7E, 0xC0, 0x89, 0xEC, 0x5D, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0xC2, /* 004CC920 */
  0x20, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CC930 */
  0x53, 0x51, 0x52, 0x57, 0x56, 0x55, 0x89, 0xE5, 0x81, 0x4D, 0x24, 0x00, 0x00, 0x00, 0xFF, 0x68, /* 004CC940 */
  0x50, 0xD4, 0x4B, 0x00, 0xFF, 0x75, 0x2C, 0x68, 0x04, 0xC7, 0x4C, 0x00, 0xE8, 0xAF, 0x85, 0xFB, /* 004CC950 */
  0xFF, 0x0F, 0x6E, 0x45, 0x24, 0x0F, 0x6E, 0x5D, 0x28, 0x0F, 0x60, 0xC0, 0x0F, 0x60, 0xDB, 0x0F, /* 004CC960 */
  0x71, 0xD0, 0x03, 0x0F, 0x71, 0xD3, 0x03, 0x8B, 0x0D, 0x54, 0xAD, 0x4C, 0x00, 0xBF, 0x54, 0x6D, /* 004CC970 */
  0x4C, 0x00, 0x0F, 0xE5, 0xC3, 0x0F, 0x6E, 0x6D, 0x1C, 0x85, 0xC9, 0x0F, 0x84, 0xDD, 0x00, 0x00, /* 004CC980 */
  0x00, 0x8B, 0x75, 0x20, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CC990 */
  0x83, 0x7F, 0x20, 0x00, 0x0F, 0x84, 0xB7, 0x00, 0x00, 0x00, 0x51, 0x8B, 0x87, 0x00, 0x00, 0x00, /* 004CC9A0 */
  0x00, 0x2B, 0x86, 0x00, 0x00, 0x00, 0x00, 0xA3, 0xF8, 0xC6, 0x4C, 0x00, 0xF7, 0xE8, 0x8B, 0xD8, /* 004CC9B0 */
  0x8B, 0xCA, 0x8B, 0x87, 0x04, 0x00, 0x00, 0x00, 0x2B, 0x86, 0x04, 0x00, 0x00, 0x00, 0xA3, 0xFC, /* 004CC9C0 */
  0xC6, 0x4C, 0x00, 0xF7, 0xE8, 0x03, 0xD8, 0x13, 0xCA, 0x8B, 0x87, 0x08, 0x00, 0x00, 0x00, 0x2B, /* 004CC9D0 */
  0x86, 0x08, 0x00, 0x00, 0x00, 0xA3, 0x00, 0xC7, 0x4C, 0x00, 0xF7, 0xE8, 0x03, 0xD8, 0x13, 0xCA, /* 004CC9E0 */
  0x8B, 0x57, 0x14, 0x8B, 0x47, 0x10, 0x3B, 0xCA, 0x7F, 0x66, 0x75, 0x04, 0x3B, 0xD8, 0x73, 0x60, /* 004CC9F0 */
  0x0F, 0xA4, 0xD9, 0x03, 0xC1, 0xE3, 0x03, 0x03, 0xD8, 0x13, 0xCA, 0x0F, 0xA4, 0xC2, 0x11, 0x0F, /* 004CCA00 */
  0xA4, 0xD9, 0x0C, 0x74, 0x4B, 0x8D, 0x14, 0xD2, 0x68, 0xF8, 0xC6, 0x4C, 0x00, 0x68, 0xF8, 0xC6, /* 004CCA10 */
  0x4C, 0x00, 0xE8, 0x79, 0x8D, 0xFB, 0xFF, 0x68, 0x04, 0xC7, 0x4C, 0x00, 0x68, 0xF8, 0xC6, 0x4C, /* 004CCA20 */
  0x00, 0xE8, 0x7A, 0x8C, 0xFB, 0xFF, 0x33, 0xDB, 0x85, 0xC0, 0x78, 0x02, 0x8B, 0xD8, 0x8B, 0xC2, /* 004CCA30 */
  0x0F, 0x6E, 0x67, 0x0C, 0x0F, 0x60, 0xE4, 0x33, 0xD2, 0x0F, 0x71, 0xD4, 0x02, 0xF7, 0xF9, 0xF7, /* 004CCA40 */
  0xEB, 0x0F, 0xAC, 0xD0, 0x1C, 0x0F, 0xE5, 0x24, 0xC5, 0x80, 0xDE, 0x41, 0x00, 0x0F, 0xED, 0xC4, /* 004CCA50 */
  0x59, 0x81, 0xC7, 0x40, 0x00, 0x00, 0x00, 0x49, 0x0F, 0x85, 0x32, 0xFF, 0xFF, 0xFF, 0x0F, 0x60, /* 004CCA60 */
  0xED, 0x0F, 0x71, 0xD5, 0x02, 0x0F, 0xE5, 0xC5, 0x0F, 0x67, 0xC0, 0x0F, 0x7E, 0xC0, 0x89, 0xEC, /* 004CCA70 */
  0x5D, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0xC2, 0x14, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCA80 */
  0x53, 0x51, 0x52, 0x57, 0x56, 0x8B, 0x0D, 0x50, 0x6D, 0x4C, 0x00, 0xBF, 0x50, 0x2D, 0x4C, 0x00, /* 004CCA90 */
  0x85, 0xC9, 0x0F, 0x84, 0x8F, 0x00, 0x00, 0x00, 0x8B, 0xF2, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCAA0 */
  0x83, 0x7F, 0x20, 0x00, 0x0F, 0x84, 0x70, 0x00, 0x00, 0x00, 0x51, 0x8B, 0x86, 0x00, 0x00, 0x00, /* 004CCAB0 */
  0x00, 0x8B, 0x5F, 0x10, 0x8B, 0x4F, 0x14, 0x2B, 0x87, 0x00, 0x00, 0x00, 0x00, 0xF7, 0xE8, 0x2B, /* 004CCAC0 */
  0xD8, 0x1B, 0xCA, 0x78, 0x54, 0x8B, 0x86, 0x04, 0x00, 0x00, 0x00, 0x2B, 0x87, 0x04, 0x00, 0x00, /* 004CCAD0 */
  0x00, 0xF7, 0xE8, 0x2B, 0xD8, 0x1B, 0xCA, 0x78, 0x40, 0x8B, 0x86, 0x08, 0x00, 0x00, 0x00, 0x2B, /* 004CCAE0 */
  0x87, 0x08, 0x00, 0x00, 0x00, 0xF7, 0xE8, 0x2B, 0xD8, 0x1B, 0xCA, 0x78, 0x2C, 0x0F, 0xA4, 0xD9, /* 004CCAF0 */
  0x1B, 0x8B, 0x57, 0x10, 0x8B, 0x5F, 0x14, 0x8B, 0xC1, 0x0F, 0x6E, 0x67, 0x0C, 0x0F, 0xA4, 0xD3, /* 004CCB00 */
  0x14, 0x74, 0x16, 0x0F, 0x60, 0xE4, 0x33, 0xD2, 0x0F, 0x71, 0xD4, 0x02, 0xF7, 0xF3, 0x0F, 0xE5, /* 004CCB10 */
  0x24, 0xC5, 0x80, 0xDE, 0x41, 0x00, 0x0F, 0xDD, 0xC4, 0x59, 0x81, 0xC7, 0x40, 0x00, 0x00, 0x00, /* 004CCB20 */
  0x49, 0x0F, 0x85, 0x79, 0xFF, 0xFF, 0xFF, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0xC3, 0x90, 0x90, 0x90, /* 004CCB30 */
  0x53, 0x51, 0x52, 0x57, 0x55, 0x89, 0xE5, 0x83, 0x7D, 0x20, 0x00, 0x74, 0x23, 0xBF, 0x50, 0xED, /* 004CCB40 */
  0x4B, 0x00, 0xB9, 0x00, 0x01, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCB50 */
  0x83, 0x7F, 0x0C, 0x00, 0x74, 0x1A, 0x81, 0xC7, 0x40, 0x00, 0x00, 0x00, 0x49, 0x75, 0xF1, 0x90, /* 004CCB60 */
  0x33, 0xC0, 0xF9, 0x89, 0xEC, 0x5D, 0x5F, 0x5A, 0x59, 0x5B, 0xC2, 0x18, 0x00, 0x90, 0x90, 0x90, /* 004CCB70 */
  0x8B, 0x45, 0x1C, 0x8B, 0x4D, 0x20, 0x8B, 0x5D, 0x18, 0x89, 0x47, 0x20, 0x81, 0xE1, 0xFF, 0xFF, /* 004CCB80 */
  0xFF, 0x00, 0xF7, 0xE8, 0x89, 0x4F, 0x0C, 0x85, 0xDB, 0x75, 0x16, 0x89, 0x57, 0x14, 0x89, 0x47, /* 004CCB90 */
  0x10, 0xC7, 0x47, 0x18, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x47, 0x1C, 0x00, 0x00, 0x00, 0x00, 0xEB, /* 004CCBA0 */
  0x18, 0xC7, 0x47, 0x14, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x47, 0x10, 0x00, 0x00, 0x00, 0x00, 0x89, /* 004CCBB0 */
  0x5F, 0x18, 0xC7, 0x47, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x45, 0x2C, 0x8B, 0x4D, 0x28, 0x8B, /* 004CCBC0 */
  0x55, 0x24, 0x89, 0x87, 0x00, 0x00, 0x00, 0x00, 0x89, 0x8F, 0x04, 0x00, 0x00, 0x00, 0x89, 0x97, /* 004CCBD0 */
  0x08, 0x00, 0x00, 0x00, 0x8B, 0xC7, 0xF8, 0x89, 0xEC, 0x5D, 0x5F, 0x5A, 0x59, 0x5B, 0xC2, 0x18, /* 004CCBE0 */
  0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCBF0 */
  0x50, 0x57, 0x55, 0x89, 0xE5, 0x8B, 0x7D, 0x14, 0x33, 0xC0, 0x85, 0xFF, 0x74, 0x3C, 0x2B, 0x45, /* 004CCC00 */
  0x10, 0x79, 0x06, 0x83, 0x7F, 0x18, 0x00, 0x7D, 0x0E, 0x89, 0x47, 0x14, 0x89, 0x47, 0x10, 0x89, /* 004CCC10 */
  0x47, 0x0C, 0x89, 0x47, 0x20, 0xEB, 0x23, 0x83, 0x7F, 0x18, 0x00, 0x75, 0x08, 0x89, 0x47, 0x18, /* 004CCC20 */
  0x89, 0x47, 0x1C, 0xEB, 0x15, 0x51, 0x8B, 0xC8, 0x52, 0x87, 0x4F, 0x18, 0xF7, 0x6F, 0x1C, 0xF7, /* 004CCC30 */
  0xF9, 0x89, 0x47, 0x1C, 0x5A, 0x59, 0x85, 0xC0, 0x74, 0xCF, 0x89, 0xEC, 0x5D, 0x5F, 0x58, 0xC2, /* 004CCC40 */
  0x08, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCC50 */
  0x50, 0x51, 0x57, 0xBF, 0x50, 0xED, 0x4B, 0x00, 0xB9, 0x00, 0x10, 0x00, 0x00, 0x33, 0xC0, 0xFC, /* 004CCC60 */
  0xF3, 0xAB, 0x5F, 0x59, 0x58, 0xC3, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCC70 */
  0x50, 0x53, 0x51, 0x52, 0x57, 0x56, 0x55, 0x89, 0xE5, 0xBE, 0x50, 0xED, 0x4B, 0x00, 0x8B, 0x7D, /* 004CCC80 */
  0x20, 0xB9, 0x00, 0x01, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCC90 */
  0x8B, 0x5E, 0x18, 0x83, 0x7E, 0x0C, 0x00, 0x74, 0x3B, 0x85, 0xDB, 0x74, 0x37, 0x8B, 0x46, 0x20, /* 004CCCA0 */
  0xF7, 0x6E, 0x1C, 0xF7, 0xFB, 0xF7, 0xE8, 0x01, 0x7E, 0x1C, 0x89, 0x56, 0x14, 0x89, 0x46, 0x10, /* 004CCCB0 */
  0x33, 0xD2, 0x85, 0xDB, 0x79, 0x13, 0x3B, 0x56, 0x1C, 0x7F, 0x19, 0x89, 0x56, 0x14, 0x89, 0x56, /* 004CCCC0 */
  0x10, 0x89, 0x56, 0x20, 0x89, 0x56, 0x0C, 0xEB, 0x05, 0x3B, 0x5E, 0x1C, 0x7D, 0x06, 0x89, 0x56, /* 004CCCD0 */
  0x1C, 0x89, 0x56, 0x18, 0x81, 0xC6, 0x40, 0x00, 0x00, 0x00, 0x49, 0x75, 0xB3, 0x89, 0xEC, 0x5D, /* 004CCCE0 */
  0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0x58, 0xC2, 0x04, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCCF0 */
  0x50, 0x53, 0x51, 0x52, 0x57, 0x56, 0xBE, 0x50, 0xED, 0x4B, 0x00, 0xBF, 0x50, 0x2D, 0x4C, 0x00, /* 004CCD00 */
  0xBB, 0x00, 0x01, 0x00, 0x00, 0x33, 0xD2, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCD10 */
  0x83, 0x7E, 0x0C, 0x00, 0x74, 0x2F, 0x8D, 0x06, 0x8D, 0x0F, 0x68, 0xA0, 0x58, 0x48, 0x00, 0x50, /* 004CCD20 */
  0x51, 0xE8, 0x3A, 0x81, 0xFB, 0xFF, 0x8B, 0x46, 0x0C, 0x8B, 0x4E, 0x20, 0x89, 0x47, 0x0C, 0x89, /* 004CCD30 */
  0x4F, 0x20, 0x8B, 0x46, 0x10, 0x8B, 0x4E, 0x14, 0x42, 0x89, 0x47, 0x10, 0x89, 0x4F, 0x14, 0x81, /* 004CCD40 */
  0xC7, 0x40, 0x00, 0x00, 0x00, 0x81, 0xC6, 0x40, 0x00, 0x00, 0x00, 0x4B, 0x75, 0xC2, 0x89, 0x15, /* 004CCD50 */
  0x50, 0x6D, 0x4C, 0x00, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0x58, 0xC3, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCD60 */
  0x50, 0x53, 0x51, 0x52, 0x57, 0x56, 0x55, 0x89, 0xE5, 0xBE, 0x50, 0x2D, 0x4C, 0x00, 0xBF, 0x54, /* 004CCD70 */
  0x6D, 0x4C, 0x00, 0x8B, 0x1D, 0x50, 0x6D, 0x4C, 0x00, 0x33, 0xD2, 0x85, 0xDB, 0x74, 0x4A, 0x90, /* 004CCD80 */
  0x52, 0x8B, 0x45, 0x2C, 0x53, 0x2B, 0x06, 0xF7, 0xE8, 0x8B, 0xD8, 0x8B, 0x45, 0x28, 0x8B, 0xCA, /* 004CCD90 */
  0x2B, 0x46, 0x04, 0xF7, 0xE8, 0x03, 0xD8, 0x8B, 0x45, 0x24, 0x13, 0xCA, 0x2B, 0x46, 0x08, 0xF7, /* 004CCDA0 */
  0xE8, 0x03, 0xD8, 0x8B, 0x45, 0x20, 0x13, 0xCA, 0x03, 0x46, 0x20, 0xF7, 0xE8, 0x2B, 0xC3, 0x1B, /* 004CCDB0 */
  0xD1, 0x5B, 0x5A, 0x78, 0x0B, 0xB9, 0x10, 0x00, 0x00, 0x00, 0x42, 0xFC, 0xF3, 0xA5, 0xEB, 0x06, /* 004CCDC0 */
  0x81, 0xC6, 0x40, 0x00, 0x00, 0x00, 0x4B, 0x75, 0xB7, 0x89, 0x15, 0x54, 0xAD, 0x4C, 0x00, 0x89, /* 004CCDD0 */
  0xEC, 0x5D, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0x58, 0xC2, 0x10, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CCDE0 */
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 /* 004CCDF0 */
};
/* original 0x004CCFF0..0x004CD1A0: machine code of 0x004CCFF0 (GraphicsShadingRuntime_InitializeGeneratedTextureCf) */
static const uint8_t s_ModelLightingOriginalCode004CCFF0[0x1B0] = {
  0x53, 0x51, 0x52, 0x57, 0x56, 0x55, 0x89, 0xE5, 0x8B, 0x4D, 0x20, 0x03, 0xC9, 0x0F, 0xAF, 0xC9, /* 004CCFF0 */
  0x51, 0xFF, 0x15, 0x00, 0x20, 0x40, 0x00, 0x0F, 0x82, 0x88, 0x01, 0x00, 0x00, 0x8B, 0x55, 0x20, /* 004CD000 */
  0xC1, 0xE9, 0x02, 0xC1, 0xEA, 0x01, 0xA3, 0x30, 0xCE, 0x4C, 0x00, 0x03, 0xD0, 0x8B, 0xF8, 0x03, /* 004CD010 */
  0xD1, 0x33, 0xC0, 0x89, 0x15, 0x34, 0xCE, 0x4C, 0x00, 0xFC, 0xF3, 0xAB, 0x8B, 0x4D, 0x24, 0x8B, /* 004CD020 */
  0x75, 0x1C, 0x0F, 0xAF, 0xC9, 0x81, 0xC1, 0x20, 0x00, 0x00, 0x00, 0x0F, 0xAF, 0xCE, 0x81, 0xC1, /* 004CD030 */
  0x00, 0x0A, 0x00, 0x00, 0x51, 0xFF, 0x15, 0x00, 0x20, 0x40, 0x00, 0x0F, 0x82, 0x44, 0x01, 0x00, /* 004CD040 */
  0x00, 0x8B, 0xF8, 0xA3, 0x2C, 0xCE, 0x4C, 0x00, 0x8B, 0xD1, 0x8B, 0xD8, 0xC1, 0xE9, 0x02, 0x33, /* 004CD050 */
  0xC0, 0xFC, 0xF3, 0xAB, 0x8B, 0x4D, 0x24, 0x8B, 0xFB, 0x51, 0x0F, 0xAF, 0xC9, 0xC7, 0x87, 0x00, /* 004CD060 */
  0x00, 0x00, 0x00, 0x67, 0x66, 0x78, 0x00, 0x89, 0x97, 0x04, 0x00, 0x00, 0x00, 0x89, 0xB7, 0xB0, /* 004CD070 */
  0x00, 0x00, 0x00, 0xC7, 0x87, 0xB4, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xC7, 0x87, 0xB8, /* 004CD080 */
  0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0xB8, 0xFF, 0xFF, 0xFF, 0x00, 0x8D, 0x9F, 0x00, 0x02, /* 004CD090 */
  0x00, 0x00, 0xBA, 0x00, 0x01, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CD0A0 */
  0x89, 0x03, 0x05, 0x00, 0x00, 0x00, 0x01, 0x83, 0xC3, 0x08, 0x4A, 0x75, 0xF3, 0xBA, 0x00, 0x0A, /* 004CD0B0 */
  0x00, 0x00, 0x89, 0x35, 0x18, 0xCE, 0x4C, 0x00, 0x8B, 0xDE, 0x03, 0xFA, 0xC1, 0xE6, 0x05, 0x58, /* 004CD0C0 */
  0x03, 0xD6, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, /* 004CD0D0 */
  0x89, 0x87, 0x00, 0x00, 0x00, 0x00, 0x89, 0x87, 0x04, 0x00, 0x00, 0x00, 0xC7, 0x87, 0x08, 0x00, /* 004CD0E0 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x89, 0x97, 0x0C, 0x00, 0x00, 0x00, 0xC7, 0x87, 0x10, 0x00, /* 004CD0F0 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x87, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 004CD100 */
  0x89, 0x87, 0x18, 0x00, 0x00, 0x00, 0x89, 0x87, 0x1C, 0x00, 0x00, 0x00, 0x03, 0xD1, 0x81, 0xC7, /* 004CD110 */
  0x20, 0x00, 0x00, 0x00, 0x4B, 0x75, 0xB9, 0x8B, 0x55, 0x20, 0xA3, 0x00, 0xCE, 0x4C, 0x00, 0x89, /* 004CD120 */
  0x15, 0x04, 0xCE, 0x4C, 0x00, 0x8B, 0xC8, 0xB8, 0x00, 0x00, 0x10, 0x00, 0xF7, 0xE2, 0xF7, 0xF1, /* 004CD130 */
  0xA3, 0x24, 0xCE, 0x4C, 0x00, 0xA3, 0x28, 0xCE, 0x4C, 0x00, 0xA1, 0x04, 0xCE, 0x4C, 0x00, 0x33, /* 004CD140 */
  0xC9, 0xC1, 0xF8, 0x01, 0x48, 0x2B, 0xC8, 0xC1, 0xE0, 0x0C, 0xC1, 0xE1, 0x0C, 0xA3, 0x40, 0xCE, /* 004CD150 */
  0x4C, 0x00, 0x89, 0x0D, 0x44, 0xCE, 0x4C, 0x00, 0xFF, 0x35, 0x2C, 0xCE, 0x4C, 0x00, 0xFF, 0x15, /* 004CD160 */
  0x34, 0x58, 0x48, 0x00, 0x72, 0x11, 0xA3, 0x38, 0xCE, 0x4C, 0x00, 0xF8, 0x89, 0xEC, 0x5D, 0x5E, /* 004CD170 */
  0x5F, 0x5A, 0x59, 0x5B, 0xC2, 0x0C, 0x00, 0x50, 0xFF, 0x35, 0x2C, 0xCE, 0x4C, 0x00, 0xFF, 0x15, /* 004CD180 */
  0x04, 0x20, 0x40, 0x00, 0x58, 0xF9, 0x89, 0xEC, 0x5D, 0x5E, 0x5F, 0x5A, 0x59, 0x5B, 0xC2, 0x0C /* 004CD190 */
};

/* One piece of the original address window: original bytes start..end live at bytes. */
typedef struct ModelLightingOriginalRange {
  uint32_t start;
  uint32_t end;
  const uint8_t *bytes;
} ModelLightingOriginalRange;

/* The original address window 0x004C6D54..0x004CD1A0 (whole variables listed, covers the default path's reach
   0x004C91A0..0x004CD1A0), in address order and without holes. */
#define MODEL_LIGHTING_ORIGINAL_WINDOW_START 0x004C6D54u
#define MODEL_LIGHTING_ORIGINAL_WINDOW_END 0x004CD1A0u
static const ModelLightingOriginalRange s_ModelLightingOriginalWindow[] = {
  {0x004C6D54, 0x004CAD54, (const uint8_t *)g_GraphicsShadingNearbyRecords},
  {0x004CAD54, 0x004CAD58, (const uint8_t *)&g_GraphicsShadingNearbyRecordCount},
  {0x004CAD58, 0x004CAD60, s_ModelLightingOriginalFiller004CAD58},
  {0x004CAD60, 0x004CC6F8, (const uint8_t *)g_ModelLightingMmxMultiplierRows},
  {0x004CC6F8, 0x004CC704, (const uint8_t *)&g_ModelLightingVertexToLightVectorScratch},
  {0x004CC704, 0x004CC710, (const uint8_t *)&g_ModelLightingTransformedSurfaceNormalScratch},
  {0x004CC710, 0x004CCE00, s_ModelLightingOriginalCode004CC710},
  {0x004CCE00, 0x004CCE04, (const uint8_t *)&g_GraphicsShadingTextureDimension},
  {0x004CCE04, 0x004CCE08, (const uint8_t *)&g_GraphicsShadingGridHalfSize},
  {0x004CCE08, 0x004CCE0C, (const uint8_t *)&g_GraphicsShadingGeneratedTexturePixelCursor},
  {0x004CCE0C, 0x004CCE10, (const uint8_t *)&g_GraphicsShadingGeneratedTextureTileX},
  {0x004CCE10, 0x004CCE14, (const uint8_t *)&g_GraphicsShadingGeneratedTextureTileY},
  {0x004CCE14, 0x004CCE18, (const uint8_t *)&g_GraphicsShadingGeneratedTextureSubresourceIndex},
  {0x004CCE18, 0x004CCE1C, (const uint8_t *)&g_GraphicsShadingSubresourceCount},
  {0x004CCE1C, 0x004CCE20, (const uint8_t *)&g_GraphicsShadingGeneratedTextureTileXQ20},
  {0x004CCE20, 0x004CCE24, (const uint8_t *)&g_GraphicsShadingGeneratedTextureTileYQ20},
  {0x004CCE24, 0x004CCE28, (const uint8_t *)&g_GraphicsShadingGridStepQ20},
  {0x004CCE28, 0x004CCE2C, (const uint8_t *)&g_GraphicsShadingGridStepQ20Current},
  {0x004CCE2C, 0x004CCE30, (const uint8_t *)&g_GraphicsShadingGeneratedAsset},
  {0x004CCE30, 0x004CCE34, (const uint8_t *)&g_GraphicsShadingGridScratch},
  {0x004CCE34, 0x004CCE38, (const uint8_t *)&g_GraphicsShadingGridScratchInterior},
  {0x004CCE38, 0x004CCE3C, (const uint8_t *)&g_GraphicsShadingTextureSet},
  {0x004CCE3C, 0x004CCE40, (const uint8_t *)&g_GraphicsShadingGeneratedTextureCompletedTraversalCount},
  {0x004CCE40, 0x004CCE44, (const uint8_t *)&g_GraphicsShadingPositiveGridOriginQ12},
  {0x004CCE44, 0x004CCE48, (const uint8_t *)&g_GraphicsShadingNegativeGridOriginQ12},
  {0x004CCE48, 0x004CCFF0, (const uint8_t *)&g_GeneratedTextureScratchRuntime},
  {0x004CCFF0, 0x004CD1A0, s_ModelLightingOriginalCode004CCFF0},
};

/* 5f-format: overread s_ModelLightingOriginalWindow - the window maps original 32-bit addresses onto today's
   variables and assumes their 32-bit layouts. On x64 the pointer variables are read as their low dwords and the
   records at their 32-bit offsets (other bytes than the original's), but never past the end of a variable. */
#if defined(_WIN64)
static_assert(sizeof g_GraphicsShadingNearbyRecords >= 0x4000 && sizeof g_GraphicsShadingNearbyRecordCount == 4 &&
              sizeof g_ModelLightingMmxMultiplierRows == 0x1998 && sizeof g_ModelLightingVertexToLightVectorScratch == 0xC &&
              sizeof g_ModelLightingTransformedSurfaceNormalScratch == 0xC &&
              sizeof g_GraphicsShadingGeneratedTexturePixelCursor >= 4 && sizeof g_GraphicsShadingGeneratedAsset >= 4 &&
              sizeof g_GraphicsShadingGridScratch >= 4 && sizeof g_GraphicsShadingGridScratchInterior >= 4 &&
              sizeof g_GraphicsShadingTextureSet >= 4 && sizeof g_GeneratedTextureScratchRuntime >= 0x1A8,
              "the lighting window must not read past a variable");
#else
/* The variables must still have their original sizes for the window above. */
typedef char ModelLightingOriginalWindowSizeCheck
  [(sizeof g_GraphicsShadingNearbyRecords == 0x4000 && sizeof g_GraphicsShadingNearbyRecordCount == 4 &&
    sizeof g_ModelLightingMmxMultiplierRows == 0x1998 && sizeof g_ModelLightingVertexToLightVectorScratch == 0xC &&
    sizeof g_ModelLightingTransformedSurfaceNormalScratch == 0xC &&
    sizeof g_GraphicsShadingGeneratedTexturePixelCursor == 4 && sizeof g_GraphicsShadingGeneratedAsset == 4 &&
    sizeof g_GraphicsShadingGridScratch == 4 && sizeof g_GraphicsShadingGridScratchInterior == 4 &&
    sizeof g_GraphicsShadingTextureSet == 4 && sizeof g_GeneratedTextureScratchRuntime == 0x1A8) ? 1 : -1];
#endif

/* The 8 bytes the original read at originalAddress, little-endian. Inside the window they are exact; outside it
   (only ModelRender_ComputeVertexIntensityScaledPath gets there, see its quirk) the original read unrelated
   memory far from this table or faulted, which cannot be reproduced: those reads give 0. */
static uint64_t ModelLighting_ReadOriginalImageQword(uint32_t originalAddress)
{
  uint64_t value = 0;
  uint32_t byteIndex;
  size_t rangeIndex;

  if (originalAddress < MODEL_LIGHTING_ORIGINAL_WINDOW_START ||
      originalAddress > MODEL_LIGHTING_ORIGINAL_WINDOW_END - 8) {
    return 0;
  }
  for (byteIndex = 0; byteIndex < 8; byteIndex++) {
    uint32_t address = originalAddress + byteIndex;
    for (rangeIndex = 0; rangeIndex < sizeof s_ModelLightingOriginalWindow / sizeof s_ModelLightingOriginalWindow[0];
         rangeIndex++) {
      const ModelLightingOriginalRange *range = &s_ModelLightingOriginalWindow[rangeIndex];
      if (range->start <= address && address < range->end) {
        value |= (uint64_t)range->bytes[address - range->start] << (8 * byteIndex);
        break;
      }
    }
  }
  return value;
}

/* The qword at byteOffset from the start of g_ModelLightingMmxMultiplierRows as the original's PMULHW operand:
   inside the table a plain read, outside it the original bytes at that address (see the quirks). */
static uint64_t ModelLighting_ReadMultiplierQword(int32_t byteOffset)
{
  if (byteOffset >= 0 && byteOffset <= (int32_t)sizeof g_ModelLightingMmxMultiplierRows - 8) {
    return *(const uint64_t *)((const uint8_t *)g_ModelLightingMmxMultiplierRows + byteOffset);
  }
  return ModelLighting_ReadOriginalImageQword(MODEL_LIGHTING_MMX_ROWS_ORIGINAL_ADDRESS + (uint32_t)byteOffset);
}

/* distanceAttenuationTable[rowOffset] (ModelRender_ComputeVertexIntensityDefaultPath). */
static uint64_t ModelLighting_ReadDistanceAttenuationRow
          (GraphicsDistanceAttenuationTableAddress32 distanceAttenuationTable, int32_t rowOffset)
{
  return ModelLighting_ReadMultiplierQword
           ((int32_t)(distanceAttenuationTable -
                      (GraphicsDistanceAttenuationTableAddress32)(uintptr_t)g_ModelLightingMmxMultiplierRows) +
            rowOffset * 8);
}


/* Lit colour of a mesh vertex for ModelRender_PrepareProjectedVertex, in MMX word lanes: the directional light
   (scenePackedColor1, weighted by the attenuation table entry for dot(lightDirection, normal) >> 21) plus a quarter
   of the ambient colour scenePackedColor0, times materialPackedColor (the node tint); then every nearby light of
   g_GraphicsShadingNearbyRecords whose sphere contains the vertex adds its colour weighted by
   g_PackedLightingLookupTable[((r^2 - d^2) >> 5) / (r^2 >> 12)]; the sum is modulated by the vertex's own colour.
*/
PackedArgb32
ModelRender_ComputeVertexIntensityDefaultPath
          (PackedArgb32 vertexPackedColor,int *vertexPositionQ12,
          GraphicsDistanceAttenuationTableAddress32 distanceAttenuationTable,
          PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1,
          GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor,
          GraphicsFixedVec3 *surfaceNormalQ12)

{
  PackedRgb24 lightPackedColor;
  int64_t axisDistanceSquared;
  int32_t lightFacingDotQ12;
  int remainderHigh;
  uint32_t remainderLow;
  int axisDelta;
  uint32_t axisSquareLow;
  GraphicsShadingRecordCount remainingRecords;
  uint32_t lookupDivisor;
  GraphicsShadingRuntimeRecord *shadingRecord;
  uint64_t directionalLanes;
  uint64_t accumulatedLanes;
  uint64_t lightLanes;

  lightFacingDotQ12 = FixedVec3_DotQ12(lightDirectionQ12,surfaceNormalQ12);
  directionalLanes =
       pmulhw(ModelLighting_UnpackBytesMmx(scenePackedColor1,2),
              ModelLighting_ReadDistanceAttenuationRow(distanceAttenuationTable,lightFacingDotQ12 >> 21));
  shadingRecord = g_GraphicsShadingNearbyRecords;
  accumulatedLanes =
       pmulhw(ModelLighting_AddWordsMmx(directionalLanes,ModelLighting_UnpackBytesMmx(scenePackedColor0,4)),
              ModelLighting_UnpackBytesMmx(materialPackedColor,2));
  for (remainingRecords = g_GraphicsShadingNearbyRecordCount; remainingRecords != 0; remainingRecords--) {
    if (shadingRecord->targetRadiusQ12 != 0) {
      /* r^2 - dx^2 - dy^2 - dz^2 as a 64-bit subtraction on dword halves (remainderHigh:remainderLow, the low
         dword borrowing from the high one); the light reaches the vertex while remainderHigh stays >= 0 */
      remainderLow = (uint32_t)shadingRecord->squaredRadiusQ24;
      remainderHigh = ((int *)&shadingRecord->squaredRadiusQ24)[1];
      axisDelta = *vertexPositionQ12 - shadingRecord->worldXQ12;
      axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
      axisSquareLow = (uint32_t)axisDistanceSquared;
      remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 32)) -
                      (uint32_t)(remainderLow < axisSquareLow);
      remainderLow = remainderLow - axisSquareLow;
      if (-1 < remainderHigh) {
        axisDelta = vertexPositionQ12[1] - shadingRecord->worldYQ12;
        axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
        axisSquareLow = (uint32_t)axisDistanceSquared;
        remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 32)) -
                        (uint32_t)(remainderLow < axisSquareLow);
        remainderLow = remainderLow - axisSquareLow;
        if (-1 < remainderHigh) {
          axisDelta = vertexPositionQ12[2] - shadingRecord->worldZQ12;
          axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
          axisSquareLow = (uint32_t)axisDistanceSquared;
          remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 32)) -
                          (uint32_t)(remainderLow < axisSquareLow);
          remainderLow = remainderLow - axisSquareLow;
          if (-1 < remainderHigh) {
            lightPackedColor = shadingRecord->packedColorRgbActive;
            /* divisor r^2 >> 12 (64-bit shift, low dword kept) */
            lookupDivisor = ((int *)&shadingRecord->squaredRadiusQ24)[1] << (32 - Q12_SHIFT) |
                     (uint32_t)shadingRecord->squaredRadiusQ24 >> Q12_SHIFT;
            if (lookupDivisor != 0) {
              /* table index: (remainder >> 5) / (r^2 >> 12), low dword only */
              lightLanes =
                   pmulhw(ModelLighting_UnpackBytesMmx(lightPackedColor,2),
                          g_PackedLightingLookupTable[(int32_t)((remainderHigh * (1 << 27) | remainderLow >> 5) /
                                                      lookupDivisor)]);
              /* PADDUSB (byte lanes) as in the original, although the lanes hold words. */
              accumulatedLanes = paddusb(accumulatedLanes,lightLanes);
            }
          }
        }
      }
    }
    shadingRecord = shadingRecord + 1;
  }
  accumulatedLanes = pmulhw(accumulatedLanes,ModelLighting_UnpackBytesMmx(vertexPackedColor,2));
  return ModelLighting_PackUnsignedMmx(accumulatedLanes);
}

/* The same vertex lighting as ModelRender_ComputeVertexIntensityDefaultPath for MODEL_TRIANGLE_LIGHTING_SCALED
   triangles: the directional weight comes from g_ModelLightingMmxMultiplierRows at MODEL_LIGHTING_SCALE_ROW0 plus
   the facing dot divided by the model resource's lightingScaleQ12 (>> 9).
   Original quirk: the row (signed dot / lightingScaleQ12, then >> 9) is read relative to the original address
   0x004CC2B0 (MODEL_LIGHTING_SCALE_ROW0) as the PMULHW multiplier without a range check. The normal here is
   the vertex position (Q12, P units along the light), the light direction is Q28, so dot = 2^28 * P (low
   dword, wraps from |P| >= 8) and the row offset is floor(trunc(dot / s) / 512) with s = lightingScaleQ12 = S units * 4096, i.e. about 128 * P / S. The table holds offsets -682..136
   (P / S in -5.33..1.06). For |s| >= 4096 any wrapped dot gives offsets -1024..1023 (0x004CA2B0..0x004CE2B0);
   smaller |s| reach up to +-2^22 rows (+-32 MB), far outside the original image. s == 0 and
   dot == INT_MIN with s == -1 fault (divide error) in the original as in this C division. Out-of-table rows go
   through ModelLighting_ReadMultiplierQword: exact original bytes within 0x004C6D54..0x004CD1A0, 0 beyond it.
*/
PackedArgb32
ModelRender_ComputeVertexIntensityScaledPath
          (PackedArgb32 vertexPackedColor,int *vertexPositionQ12,Q12 lightingScaleQ12,
          PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1,
          GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor,
          GraphicsFixedVec3 *surfaceNormalQ12)

{
  PackedRgb24 lightPackedColor;
  int64_t axisDistanceSquared;
  int32_t lightFacingDotQ12;
  int remainderHigh;
  uint32_t remainderLow;
  int axisDelta;
  uint32_t axisSquareLow;
  GraphicsShadingRecordCount remainingRecords;
  uint32_t lookupDivisor;
  GraphicsShadingRuntimeRecord *shadingRecord;
  uint64_t directionalLanes;
  uint64_t accumulatedLanes;
  uint64_t resultLanes;
  uint64_t lightLanes;

  lightFacingDotQ12 = FixedVec3_DotQ12(lightDirectionQ12,surfaceNormalQ12);
  directionalLanes =
       pmulhw(ModelLighting_UnpackBytesMmx(scenePackedColor1,2),
              ModelLighting_ReadMultiplierQword
                (((int32_t)MODEL_LIGHTING_SCALE_ROW0 + (lightFacingDotQ12 / lightingScaleQ12 >> 9)) * 8));
  shadingRecord = g_GraphicsShadingNearbyRecords;
  accumulatedLanes =
       pmulhw(ModelLighting_AddWordsMmx(directionalLanes,ModelLighting_UnpackBytesMmx(scenePackedColor0,4)),
              ModelLighting_UnpackBytesMmx(materialPackedColor,2));
  for (remainingRecords = g_GraphicsShadingNearbyRecordCount; remainingRecords != 0; remainingRecords--) {
    if (shadingRecord->targetRadiusQ12 != 0) {
      /* r^2 - dx^2 - dy^2 - dz^2 as a 64-bit subtraction on dword halves (remainderHigh:remainderLow, the low
         dword borrowing from the high one); the light reaches the vertex while remainderHigh stays >= 0 */
      remainderLow = (uint32_t)shadingRecord->squaredRadiusQ24;
      remainderHigh = ((int *)&shadingRecord->squaredRadiusQ24)[1];
      axisDelta = *vertexPositionQ12 - shadingRecord->worldXQ12;
      axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
      axisSquareLow = (uint32_t)axisDistanceSquared;
      remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 32)) -
                      (uint32_t)(remainderLow < axisSquareLow);
      remainderLow = remainderLow - axisSquareLow;
      if (-1 < remainderHigh) {
        axisDelta = vertexPositionQ12[1] - shadingRecord->worldYQ12;
        axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
        axisSquareLow = (uint32_t)axisDistanceSquared;
        remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 32)) -
                        (uint32_t)(remainderLow < axisSquareLow);
        remainderLow = remainderLow - axisSquareLow;
        if (-1 < remainderHigh) {
          axisDelta = vertexPositionQ12[2] - shadingRecord->worldZQ12;
          axisDistanceSquared = (int64_t)axisDelta * (int64_t)axisDelta;
          axisSquareLow = (uint32_t)axisDistanceSquared;
          remainderHigh = (remainderHigh - (int)((uint64_t)axisDistanceSquared >> 32)) -
                          (uint32_t)(remainderLow < axisSquareLow);
          remainderLow = remainderLow - axisSquareLow;
          if (-1 < remainderHigh) {
            lightPackedColor = shadingRecord->packedColorRgbActive;
            /* divisor r^2 >> 12 (64-bit shift, low dword kept) */
            lookupDivisor = ((int *)&shadingRecord->squaredRadiusQ24)[1] << (32 - Q12_SHIFT) |
                     (uint32_t)shadingRecord->squaredRadiusQ24 >> Q12_SHIFT;
            if (lookupDivisor != 0) {
              /* table index: (remainder >> 5) / (r^2 >> 12), low dword only */
              lightLanes =
                   pmulhw(ModelLighting_UnpackBytesMmx(lightPackedColor,2),
                          g_PackedLightingLookupTable[(int32_t)((remainderHigh * (1 << 27) | remainderLow >> 5) /
                                                      lookupDivisor)]);
              /* PADDUSB (byte lanes) as in the original, although the lanes hold words. */
              accumulatedLanes = paddusb(accumulatedLanes,lightLanes);
            }
          }
        }
      }
    }
    shadingRecord = shadingRecord + 1;
  }
  resultLanes = pmulhw(accumulatedLanes,ModelLighting_UnpackBytesMmx(vertexPackedColor,2));
  return ModelLighting_PackUnsignedMmx(resultLanes);
}

/* Vertex colour of the alternate model renderer (ModelRender_PrepareProjectedVertexAlternatePath): ambient
   scenePackedColor0 times the material colour, plus every nearby light whose sphere contains the vertex, weighted
   by g_PackedLightingLookupTable at (9 * r^2 / (8 * d^2 + r^2)) scaled by the facing of the view-space normal
   towards the light; the sum is modulated by the vertex's own colour.
*/
PackedArgb32
ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath
          (PackedArgb32 vertexPackedColor,GraphicsFixedVec3 *vertexPositionQ12,
          PackedArgb32 scenePackedColor0,PackedArgb32 materialPackedColor,
          GraphicsFixedVec3 *surfaceNormalQ12)

{
  /* The MMX lanes in plain C: color is the running accumulator every light is added to. */
  const short *lightingTable = (const short *)&g_PackedLightingLookupTable;
  const GraphicsShadingRuntimeRecord *record = g_GraphicsShadingNearbyRecords;
  GraphicsShadingRecordCount remaining;
  short color[4];
  short lanes[4];

  scenePackedColor0 = scenePackedColor0 | ARGB8888_ALPHA_MASK;
  FixedTransform_ApplyDirection
            (&g_ModelLightingTransformedSurfaceNormalScratch,surfaceNormalQ12,
             &g_ModelViewCompositeTransform);
  ModelLighting_UnpackBytes(scenePackedColor0,3,color);
  ModelLighting_UnpackBytes(materialPackedColor,3,lanes);
  ModelLighting_MulHigh(color,lanes);
  for (remaining = g_GraphicsShadingNearbyRecordCount; remaining != 0; remaining--, record++) {
    int64_t distanceSquared;
    int64_t radiusSquared;
    uint32_t divisor;
    uint32_t numerator;
    int32_t facing;
    int quotient;
    uint32_t tableIndex;

    if (record->targetRadiusQ12 == 0) {
      continue;
    }
    g_ModelLightingVertexToLightVectorScratch.x = record->worldXQ12 - vertexPositionQ12->x;
    g_ModelLightingVertexToLightVectorScratch.y = record->worldYQ12 - vertexPositionQ12->y;
    g_ModelLightingVertexToLightVectorScratch.z = record->worldZQ12 - vertexPositionQ12->z;
    distanceSquared =
         (int64_t)g_ModelLightingVertexToLightVectorScratch.x *
         g_ModelLightingVertexToLightVectorScratch.x +
         (int64_t)g_ModelLightingVertexToLightVectorScratch.y *
         g_ModelLightingVertexToLightVectorScratch.y +
         (int64_t)g_ModelLightingVertexToLightVectorScratch.z *
         g_ModelLightingVertexToLightVectorScratch.z;
    radiusSquared = (int64_t)record->squaredRadiusQ24;
    if (distanceSquared >= radiusSquared) {
      continue;
    }
    divisor = (uint32_t)((uint64_t)(distanceSquared * 8 + radiusSquared) >> 20);
    if (divisor == 0) {
      continue;
    }
    numerator = (uint32_t)((uint64_t)radiusSquared >> 15) * 9;
    FixedVec3_NormalizeQ28
              (&g_ModelLightingVertexToLightVectorScratch,&g_ModelLightingVertexToLightVectorScratch);
    facing = FixedVec3_DotQ12
                       (&g_ModelLightingVertexToLightVectorScratch,
                        &g_ModelLightingTransformedSurfaceNormalScratch);
    if (facing < 0) {
      facing = 0;
    }
    quotient = (int)((int64_t)numerator / (int)divisor);
    tableIndex = (uint32_t)(((int64_t)quotient * facing) >> 28);
    ModelLighting_UnpackBytes(record->packedColorRgbActive,2,lanes);
    ModelLighting_MulHigh(lanes,lightingTable + tableIndex * 4);
    ModelLighting_AddSaturate(color,lanes);
  }
  ModelLighting_UnpackBytes(vertexPackedColor,2,lanes);
  ModelLighting_MulHigh(color,lanes);
  return ModelLighting_PackUnsigned(color);
}



