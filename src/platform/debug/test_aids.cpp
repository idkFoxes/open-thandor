/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/test_aids.cpp
 * Project code (not in the original game)
 */

/* Own translation unit: uses the real Windows SDK headers, not the game's type headers. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>

#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/test_aids.h>

/* Test aid (not in the original): see test_aids.h. */
int Thandor_TestAidScriptActive()
{
    static int active = -1;
    if (active < 0) {
        const char *value = getenv("OPEN_THANDOR_SCRIPT");
        active = value != nullptr && value[0] != 0;
    }
    return active;
}

/* Test aid (not in the original): see test_aids.h. */
int Thandor_TestAidStateHashActive()
{
    static int active = -1;
    if (active < 0) {
        const char *value = getenv("OPEN_THANDOR_STATEHASH");
        active = value != nullptr && atoi(value) > 0;
    }
    return active;
}

volatile int g_TestAidInSimulationStep;

/* Test aid (not in the original): see test_aids.h. Logs each distinct caller once per kind. */
void Thandor_TestAidNoteOutsideStep(const char *what, void *caller)
{
    static void *seen[64];
    static unsigned seenCount;
    unsigned i;
    if (g_TestAidInSimulationStep || !Thandor_TestAidStateHashActive()) {
        return;
    }
    for (i = 0; i < seenCount; i++) {
        if (seen[i] == caller) {
            return;
        }
    }
    if (seenCount < 64) {
        seen[seenCount++] = caller;
    }
    Thandor_Log("test aid: %s outside a simulation step, caller %p", what, caller);
}

/* Test aid (not in the original): see test_aids.h. */
int Thandor_TestAidAllowSecondInstance()
{
    const char *value = getenv("OPEN_THANDOR_MULTI_INSTANCE");
    return value != nullptr && value[0] == '1';
}

/* Test aid (not in the original): see test_aids.h. */
unsigned Thandor_TestAidNetworkBindPort(unsigned gamePort)
{
    const char *value = getenv("OPEN_THANDOR_NET_PORT");
    unsigned port;
    if (value == nullptr) {
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
        enabled = value != nullptr && value[0] == '1';
    }
    if (!enabled || address == nullptr || buffer == nullptr) {
        return;
    }
    Thandor_Log("net %s %u.%u.%u.%u:%u %u bytes: %08x %08x %08x %08x", direction, address[4], address[5],
                address[6], address[7], (address[2] << 8) | address[3], byteCount, words[0],
                byteCount >= 8 ? words[1] : 0u, byteCount >= 12 ? words[2] : 0u, byteCount >= 16 ? words[3] : 0u);
}

/* Test aid (not in the original): see test_aids.h. */
int Thandor_TestAidWindowed()
{
    static int enabled = -1;
    if (enabled < 0) {
        const char *value = getenv("OPEN_THANDOR_WINDOWED");
        enabled = (value != nullptr && value[0] == '1') || Thandor_TestAidWindowMinimized();
    }
    return enabled;
}

/* Test aid (not in the original): see test_aids.h. */
int Thandor_TestAidWindowMinimized()
{
    static int enabled = -1;
    if (enabled < 0) {
        const char *value = getenv("OPEN_THANDOR_WINDOW_MINIMIZED");
        enabled = value != nullptr && value[0] == '1';
    }
    return enabled;
}
