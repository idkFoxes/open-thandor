/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/task_assignment.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_TASK_ASSIGNMENT_H
#define THANDOR_UI_FRONTEND_TASK_ASSIGNMENT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/task_assignment. */

/* Functions are grouped by semantic ownership. */

void FrontendTaskAssignmentPage_Initialize(FrontendTaskAssignmentPageInitView *frontendRootPage);

void FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(UiRootNode *taskAssignmentRoot);

extern FrontendTaskAssignmentControlOffsetTables g_FrontendTaskAssignmentControlOffsets;

#endif /* THANDOR_UI_FRONTEND_TASK_ASSIGNMENT_H */
