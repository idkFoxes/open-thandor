/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/model_tint.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CORE_MODEL_TINT_H
#define THANDOR_UI_CORE_MODEL_TINT_H

#include <thandor/core/types.h>
#include <thandor/world/model/types.h>
#include <thandor/core/contracts.h>

void ModelNodeRuntime_RefreshStateTint(ModelRuntimeNode *modelNode);

/* ModelRuntimeNode_GetStateTintArgb results */
inline constexpr uint32_t UI_MODEL_TINT_OPAQUE_WHITE = 0xffffffffu; /* colours unchanged */

inline constexpr int32_t UI_MODEL_TINT_TRANSPARENT_WHITE = 0x00ffffff;

inline constexpr uint32_t UI_MODEL_TINT_OPAQUE_GREY = 0xff878787u;

PackedArgb32 ModelRuntimeNode_GetStateTintArgb(ModelRuntimeNode *node);

#endif /* THANDOR_UI_CORE_MODEL_TINT_H */
