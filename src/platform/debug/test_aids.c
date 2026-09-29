/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/test_aids.c
 * Project code (not in the original game)
 */

#ifdef THANDOR_TEST_AIDS

/* Own translation unit: uses the real Windows SDK headers, not generated/types.h. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>

#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/test_aids.h>

/* Test aid (not in the original): see test_aids.h. */
int Thandor_TestAidScriptActive(void)
{
    static int active = -1;
    if (active < 0) {
        const char *value = getenv("OPEN_THANDOR_SCRIPT");
        active = value != NULL && value[0] != 0;
    }
    return active;
}

/* Test aid (not in the original): see test_aids.h. */
int Thandor_TestAidAllowSecondInstance(void)
{
    const char *value = getenv("OPEN_THANDOR_MULTI_INSTANCE");
    return value != NULL && value[0] == '1';
}

/* Test aid (not in the original): see test_aids.h. */
unsigned Thandor_TestAidNetworkBindPort(unsigned gamePort)
{
    const char *value = getenv("OPEN_THANDOR_NET_PORT");
    unsigned port;
    if (value == NULL) {
        return gamePort;
    }
    port = (unsigned)atoi(value);
    if (port == 0 || port > 0xffff) {
        return gamePort;
    }
    Thandor_Log("test aid: UDP socket bound to port %u instead of %u", port, gamePort);
    return port;
}

/* Test aid (not in the original): see test_aids.h. */
void Thandor_TestAidLogDatagram(const char *direction, const void *sockaddrIn, unsigned byteCount,
                                const void *buffer)
{
    static int enabled = -1;
    const unsigned char *address = (const unsigned char *)sockaddrIn;
    const unsigned *words = (const unsigned *)buffer;
    if (enabled < 0) {
        const char *value = getenv("OPEN_THANDOR_NETLOG");
        enabled = value != NULL && value[0] == '1';
    }
    if (!enabled || address == NULL || buffer == NULL) {
        return;
    }
    Thandor_Log("net %s %u.%u.%u.%u:%u %u bytes: %08x %08x %08x %08x", direction, address[4], address[5],
                address[6], address[7], (address[2] << 8) | address[3], byteCount, words[0],
                byteCount >= 8 ? words[1] : 0u, byteCount >= 12 ? words[2] : 0u, byteCount >= 16 ? words[3] : 0u);
}

#define TEST_AID_WINDOW_STYLE (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)

/* Test aid (not in the original): see test_aids.h. */
int Thandor_TestAidWindowed(void)
{
    static int enabled = -1;
    if (enabled < 0) {
        const char *value = getenv("OPEN_THANDOR_WINDOWED");
        enabled = value != NULL && value[0] == '1';
    }
    return enabled;
}

/* Test aid (not in the original): see test_aids.h. */
void *Thandor_TestAidCreateWindowedMainWindow(const char *className, const char *title, void *instance)
{
    const char *value;
    int x = 0;
    int y = 0;
    RECT rect = {0, 0, 640, 480};
    value = getenv("OPEN_THANDOR_WINDOW_X");
    if (value != NULL) {
        x = atoi(value);
    }
    value = getenv("OPEN_THANDOR_WINDOW_Y");
    if (value != NULL) {
        y = atoi(value);
    }
    AdjustWindowRect(&rect, TEST_AID_WINDOW_STYLE, FALSE);
    Thandor_Log("test aid: windowed mode, window at %d,%d", x, y);
    return CreateWindowExA(0, className, title, TEST_AID_WINDOW_STYLE, x, y, rect.right - rect.left,
                           rect.bottom - rect.top, NULL, NULL, (HINSTANCE)instance, NULL);
}

/* Test aid (not in the original): see test_aids.h. */
void Thandor_TestAidSetWindowClientSize(void *window, unsigned width, unsigned height)
{
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = (LONG)width;
    rect.bottom = (LONG)height;
    AdjustWindowRect(&rect, (DWORD)GetWindowLongA((HWND)window, GWL_STYLE), FALSE);
    SetWindowPos((HWND)window, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

/* Test aid (not in the original): see test_aids.h. */
void Thandor_TestAidClientOriginOnScreen(void *window, int *x, int *y)
{
    POINT origin = {0, 0};
    ClientToScreen((HWND)window, &origin);
    *x = origin.x;
    *y = origin.y;
}

#endif /* THANDOR_TEST_AIDS */
