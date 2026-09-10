# Call graph: `movie/runtime/playback`

18 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004A8040` **Movie_EncodeFlmBufferFromFrameProviderCf** — local: `Movie_EncodeFrame4x4Keyframe`, `Movie_EncodeFrame4x4Delta`
- `0x00563FF0` **MoviePlayback_AdvanceScheduledFrameAndTick** — local: `MoviePlayback_AdvanceToFrameAndPresent`; cross: `InGameRuntime_UpdateSimulationAndNetworkTick` → `gameplay/session/runtime`
- `0x004A8590` **Movie_Open** — cross: `WidePath_CombineDirectoryAndLeaf` → `core/text/path`, `Package_FindEntryAcrossMounts` → `assets/package/runtime`, `Random_NextPrimary` → `core/math/random`
- `0x004A8A20` **Movie_GetFrameDimensions**
- `0x004A8A40` **Movie_SetAudioGainQ15**
- `0x004A8C00` **Movie_StreamWorkerThread**
- `0x004A8D50` **Movie_Rewind**
- `0x004A8D90` **Movie_Close**
- `0x005657D0` **EndMovieUiRuntime_HandleModeTransitionCf** — cross: `FrontendClientSession_DecrementTimeoutsAndCompactPlayers` → `ui/frontend/session`, `FrontendHostSession_TickShutdownOrReadyConsensus` → `ui/frontend/session`
- `0x00565810` **EndMovieUiRuntime_DispatchCommandByFlagsCf**
- `0x005739C0` **IntroMovie_TimerTick**
- `0x004A7030` **Movie_EncodeFrame4x4Keyframe** — local: `MovieColor_ComputeLuma5FromRgb888`, `MovieColor_ComputeChromaCodeFromRgb888`
- `0x004A7770` **Movie_EncodeFrame4x4Delta** — local: `MovieColor_ComputeLuma5FromRgb888`, `MovieColor_ComputeChromaCodeFromRgb888`
- `0x004A8A60` **Movie_AdvanceFrame** — local: `Movie_DecodeFrame4x4Delta`
- `0x00564080` **MoviePlayback_AdvanceToFrameAndPresent** — local: `Movie_AdvanceFrame`; cross: `UiRootStack_InvalidateAll` → `ui/controls/layout`, `UiFrame_Draw` → `ui/controls/layout`
- `0x004A81C0` **Movie_DecodeFrame4x4Delta**
- `0x004A6FB0` **MovieColor_ComputeChromaCodeFromRgb888** — cross: `FixedMath_Vector2AngleAndLengthRegs` → `core/math/fixed`
- `0x004A7000` **MovieColor_ComputeLuma5FromRgb888**
