/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/windowed.c
 * Project code (not in the original game)
 */

/* Windowed mode (developer tools, OPEN_THANDOR_WINDOWED=1): the hooks of thandor/platform/debug/hooks.h that the
   main window, DirectDraw, the framebuffer present and DirectInput call, so two instances fit side by side on one
   monitor. DirectDraw stays at DDSCL_NORMAL, the display mode is not changed (the desktop colour depth is used),
   the software renderer blits into the client area through a clipper and DirectInput takes the mouse
   non-exclusively. The Win32 window helpers are in test_aids.c. */

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>
#include <thandor/platform/debug/test_aids.h>

/* The few IDirectDrawClipper methods the windowed mode uses, in vtable order (the generated types only know the
   clipper as an opaque pointer). */
typedef struct TestAidDirectDrawClipper TestAidDirectDrawClipper;
typedef struct TestAidDirectDrawClipperVtbl {
  void *QueryInterface;
  void *AddRef;
  TH_LEGACY_ULONG (__stdcall *Release)(TestAidDirectDrawClipper *);
  void *GetClipList;
  void *GetHWnd;
  void *Initialize;
  void *IsClipListChanged;
  void *SetClipList;
  TH_LEGACY_HRESULT (__stdcall *SetHWnd)(TestAidDirectDrawClipper *, TH_LEGACY_DWORD, void *);
} TestAidDirectDrawClipperVtbl;
struct TestAidDirectDrawClipper {
  TestAidDirectDrawClipperVtbl *lpVtbl;
};

/* Sizes the window to the mode and clips the primary surface to it. */
static TH_LEGACY_HRESULT DebugWindowed_AttachWindowClipper(uint32_t width,uint32_t height)
{
  TestAidDirectDrawClipper *clipper;
  TH_LEGACY_HRESULT result;

  Thandor_TestAidSetWindowClientSize(g_MainWindow,width,height);
  clipper = NULL;
  result = g_DirectDraw2->lpVtbl->CreateClipper(g_DirectDraw2,0,(TH_LEGACY_LPVOID *)&clipper,NULL);
  if (result == 0) {
    result = clipper->lpVtbl->SetHWnd(clipper,0,g_MainWindow);
    if (result == 0) {
      result = g_PrimarySurface3->lpVtbl->SetClipper(g_PrimarySurface3,clipper);
    }
    clipper->lpVtbl->Release(clipper); /* the primary surface keeps its own reference */
  }
  if (result != 0) {
    Thandor_Log("test aid: windowed mode could not attach a clipper (%08x)", (unsigned)result);
  }
  return result;
}

int DebugHook_Windowed(void)
{
  return Thandor_TestAidWindowed();
}

int DebugHook_CreateMainWindow(const char *className, const char *title, void *instance)
{
  if (!Thandor_TestAidWindowed()) {
    return 0;
  }
  g_MainWindow = (HWND)Thandor_TestAidCreateWindowedMainWindow(className,title,instance);
  return 1;
}

uint32_t DebugHook_DirectDrawCooperativeLevel(uint32_t fullScreenLevel)
{
  return Thandor_TestAidWindowed() ? DDSCL_NORMAL : fullScreenLevel;
}

void DebugHook_AfterPrimarySurfaceCreated(uint32_t width, uint32_t height)
{
  if (Thandor_TestAidWindowed()) {
    DebugWindowed_AttachWindowClipper(width,height);
  }
}

uint32_t DebugHook_SurfaceBitsPerPixel(uint32_t bitsPerPixel, uint32_t width, uint32_t height)
{
  if (Thandor_TestAidWindowed() && (bitsPerPixel != g_SurfaceDesc.ddpfPixelFormat.dwRGBBitCount)) {
    /* the surfaces have the desktop's depth, and the blitters chosen afterwards, the renderer's queue
       (bytesPerPixel) and the pixel packing must all follow the surfaces */
    Thandor_Log("test aid: windowed %ux%u uses the desktop depth of %u bits instead of %u",
                (unsigned)width,(unsigned)height,(unsigned)g_SurfaceDesc.ddpfPixelFormat.dwRGBBitCount,
                (unsigned)bitsPerPixel);
    return g_SurfaceDesc.ddpfPixelFormat.dwRGBBitCount;
  }
  return bitsPerPixel;
}

int DebugHook_PresentToWindow(TH_LEGACY_RECT *sourceRect)
{
  /* the primary surface is the whole desktop, so blit into the client area; Blt (unlike BltFast) honours the
     window's clipper */
  TH_LEGACY_RECT windowRect;
  int clientX;
  int clientY;
  if (!Thandor_TestAidWindowed()) {
    return 0;
  }
  Thandor_TestAidClientOriginOnScreen(g_MainWindow,&clientX,&clientY);
  windowRect.left = clientX;
  windowRect.top = clientY;
  windowRect.right = clientX + (int)g_FramebufferWidth;
  windowRect.bottom = clientY + (int)g_FramebufferHeight;
  g_PrimarySurface3->lpVtbl->Blt(g_PrimarySurface3,&windowRect,g_BackSurface3,sourceRect,DDBLT_WAIT,NULL);
  return 1;
}

uint32_t DebugHook_MouseCooperativeLevel(uint32_t exclusiveLevel)
{
  /* non-exclusive in windowed mode, so the desktop mouse is not grabbed */
  return Thandor_TestAidWindowed() ? (DISCL_NONEXCLUSIVE | DISCL_FOREGROUND) : exclusiveLevel;
}
