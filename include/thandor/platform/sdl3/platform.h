/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/sdl3/platform.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_SDL3_PLATFORM_H
#define THANDOR_PLATFORM_SDL3_PLATFORM_H

/* The SDL3 platform backend: window, event pump, keyboard, mouse, periodic timers, video presentation and audio on
   SDL3, in place of the original's Win32 window, DirectDraw, DirectInput, WinMM and DirectSound. The game reaches it
   through the original's function slots (g_Win32PumpMessages, g_TimerRegisterPeriodic, g_GraphicsSetDisplayMode,
   g_GraphicsFramebufferPresent, g_Sound*, g_Pointer*, ...); ProcessEntry (platform/bootstrap/runtime.cpp) and
   Runtime_Shutdown call the entry points below where the original called Graphics_Init, DirectInputMouse_Init and
   DirectSound_Init. This header has no SDL types; the backend's own shared declarations are in
   thandor/platform/sdl3/sdl_objects.h. */

#include <thandor/generated/types.h>

/* --- window and event pump (src/platform/sdl3/platform.cpp) --- */

/* Starts SDL (video, events), creates the main window (fullscreen desktop, or a 640x480 window at
   OPEN_THANDOR_WINDOW_X/Y in the developer tools' windowed mode) and its renderer, and publishes the window's
   HWND in g_MainWindow. Returns false (logged) when SDL or the window cannot be started. */
Bool8 SdlPlatform_CreateMainWindow(const char *title);
/* Where the original's TimerSystem_Init ran: installs the SDL timers in g_TimerRegisterPeriodic/g_TimerUnregisterPeriodic and
   SdlPlatform_PumpEvents in g_Win32PumpMessages. */
void SdlPlatform_InstallTimersAndPump(void);
/* g_Win32PumpMessages: runs the developer tools' pump hook, then handles every pending SDL event (keyboard,
   text, mouse, focus); a quit request shuts the game down and ends the process. */
void SdlPlatform_PumpEvents(void);
/* End of ProcessEntry, after Runtime_Shutdown: stops SDL. */
void SdlPlatform_Quit(void);

/* --- periodic timers (src/platform/sdl3/timer.cpp) --- */

/* g_TimerRegisterPeriodic: calls callback every 1000 / frequencyHz ms (truncated, as timeSetEvent) on SDL's timer
   thread; 32 slots, a request without a free slot is ignored. */
void SdlTimer_RegisterPeriodic(TimerFrequencyHz frequencyHz,TimerCallbackProc *callback);
/* g_TimerUnregisterPeriodic: stops the first timer registered with callback. */
void SdlTimer_UnregisterPeriodic(TimerCallbackProc *callback);
/* Runtime_Shutdown: stops every timer. */
void SdlTimer_Shutdown(void);

/* --- video (src/platform/sdl3/video.cpp) --- */

/* In place of the original's Graphics_Init: allocates the graphics tables, lists one adapter ("SDL") and the display modes
   (640x480 up to the desktop size, 16 and 32 bits per pixel) and installs SdlVideo_ApplyDisplayMode as the base
   display-mode step. Returns 0 or the error code. */
uint32_t SdlVideo_Init(void);
/* Runtime_Shutdown, after Graphics_Shutdown: releases the framebuffer, texture, renderer and window. */
void SdlVideo_Shutdown(void);
/* Base step of g_GraphicsSetDisplayMode: a w x h memory framebuffer in RGB565 or XRGB8888 (the desktop depth
   when windowed), a streaming texture of that format presented letterboxed, the pixel format, the published
   framebuffer and the 16/32-bit blitters; then the chained finalize step. */
Bool8 SdlVideo_ApplyDisplayMode(uint32_t adapterIndex,uint32_t bitsPerPixel,uint32_t height,uint32_t width,
                                uint32_t *errorCode);
/* g_GraphicsFramebufferPresent: composes the software cursor into the framebuffer, uploads and presents it and
   removes the cursor again. */
void SdlVideo_Present(SoftwareFramebufferAccess *framebuffer);
/* g_GraphicsFramebufferCaptureRegion: the memory framebuffer as a one-image ARGB8888 'gfx' asset. */
GraphicsCapturedTextureSourceAsset *SdlVideo_CaptureRegion16Bit(uint32_t captureHeight,uint32_t captureWidth,
                                                                int32_t sourceY,int32_t sourceX);
GraphicsCapturedTextureSourceAsset *SdlVideo_CaptureRegion32Bit(uint32_t captureHeight,uint32_t captureWidth,
                                                                int32_t sourceY,int32_t sourceX);

/* --- GPU primitive renderer (src/platform/sdl3/gpu_renderer.cpp, CMake option THANDOR_RENDERER_SDL_GPU) --- */

/* End of SdlVideo_Init: with OPEN_THANDOR_GPU=1 (or =compare in the developer tools) or the command-line option
   -GPU, creates an SDL_GPU device and installs the GPU rasterization of the primitive queues in
   g_GraphicsSetViewportAndClearDepth, g_GraphicsDrawPrimitiveQueue and g_GraphicsEndScene. Returns false (the
   software renderer stays) when it is not requested or the device cannot be set up (logged). */
bool SdlGpuRenderer_Init(void);
/* SdlVideo_Shutdown: puts the software renderer's slots back and releases the device. */
void SdlGpuRenderer_Shutdown(void);

/* --- keyboard and mouse (src/platform/sdl3/input.cpp) --- */

/* In place of the original's DirectInputMouse_Init: hides the system cursor, hooks the display-mode switch (cursor buffers),
   starts the cursor-animation timer, installs g_PointerFlushEvents/g_PointerSetPosition, loads the cursor assets
   and seeds the lock-key bits. Returns false with the error in *outError. */
Bool8 SdlInput_Init(uint32_t *outError);
/* Runtime_Shutdown: stops the cursor timer and gives the system cursor back. */
void SdlInput_Shutdown(void);
/* g_GraphicsSetDisplayMode hook: frees and recreates the cursor buffers around the chained mode switch. */
Bool8 SdlInput_SetDisplayMode(uint32_t adapterIndex,uint32_t bitsPerPixel,uint32_t height,uint32_t width,
                              uint32_t *errorCode);
/* g_PointerSetPosition */
void SdlInput_SetPosition(int32_t positionY,int32_t positionX);
/* g_PointerFlushEvents */
void SdlInput_FlushEvents(void);

/* --- audio (src/platform/sdl3/audio.cpp) --- */

/* In place of the original's DirectSound_Init: opens a 22050 Hz 16-bit stereo SDL audio stream with the mixer and installs the
   g_Sound* slots. Without an audio device the silent backend stays (returns 0, as the original's DirectSound_Init). Returns 0 or
   the allocator's error code. */
uint32_t SdlAudio_Init(void);
/* Runtime_Shutdown: closes the audio stream and frees the voice-set registry. */
void SdlAudio_Shutdown(void);
uint32_t SdlAudio_CreateSampleVoiceSet(SoundSampleAsset *sampleAsset,DirectSoundVoiceSet **outVoiceSet);
void SdlAudio_ReleaseSampleVoiceSet(DirectSoundVoiceSet *voiceSet);
Bool8 SdlAudio_PlayOneShot(uint32_t leftChannelGainQ15,uint32_t rightChannelGainQ15,DirectSoundVoiceSet *voiceSet,
                           IDirectSoundBuffer **outVoice);
Bool8 SdlAudio_PlayLooping(uint32_t leftChannelGainQ15,uint32_t rightChannelGainQ15,DirectSoundVoiceSet *voiceSet,
                           IDirectSoundBuffer **outVoice);
void SdlAudio_StopVoice(IDirectSoundBuffer *voice);
/* Inverted like the original's DirectSound_IsVoicePlaying: false while the voice plays, true when it is NULL or stopped. */
Bool8 SdlAudio_IsVoicePlaying(IDirectSoundBuffer *voice);
void SdlAudio_SetVoiceGains(uint32_t leftChannelGainQ15,uint32_t rightChannelGainQ15,IDirectSoundBuffer *voice);
void SdlAudio_StopAllVoices(void);

#endif /* THANDOR_PLATFORM_SDL3_PLATFORM_H */
