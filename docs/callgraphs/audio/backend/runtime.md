# Call graph: `audio/backend/runtime`

[Source](../../../../src/audio/backend/runtime.c) · [Header](../../../../include/thandor/audio/backend/runtime.h) · [Graphviz](runtime.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-audio-backend-runtime) · [Full changelog](../../../../CHANGELOG_FULL.md#module-audio-backend-runtime)

24 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00583410` **[`DirectSound_Shutdown`](../../../../src/audio/backend/runtime.c#L5)** — local: [`SoundBackendDisabled_StopAllVoices`](../../../../src/audio/backend/runtime.c#L771)
- `0x00417570` **[`SoundBackendDisabled_CreateSampleVoiceSet`](../../../../src/audio/backend/runtime.c#L29)**
- `0x00417580` **[`SoundBackendDisabled_ReleaseSampleVoiceSet`](../../../../src/audio/backend/runtime.c#L40)**
- `0x00417590` **[`SoundBackendDisabled_CreatePcmVoiceSet`](../../../../src/audio/backend/runtime.c#L50)**
- `0x004175A0` **[`SoundBackendDisabled_ReleasePcmVoiceSet`](../../../../src/audio/backend/runtime.c#L65)**
- `0x004175B0` **[`SoundBackendDisabled_PlayOneShot`](../../../../src/audio/backend/runtime.c#L75)**
- `0x004175C0` **[`SoundBackendDisabled_PlayLooping`](../../../../src/audio/backend/runtime.c#L90)**
- `0x004175D0` **[`SoundBackendDisabled_StopVoice`](../../../../src/audio/backend/runtime.c#L105)**
- `0x004175E0` **[`SoundBackendDisabled_IsVoicePlaying`](../../../../src/audio/backend/runtime.c#L115)**
- `0x00417600` **[`SoundBackendDisabled_QueryVoiceRegs`](../../../../src/audio/backend/runtime.c#L125)**
- `0x00417610` **[`SoundBackendDisabled_SetVoiceGains`](../../../../src/audio/backend/runtime.c#L135)**
- `0x00583140` **[`DirectSound_Init`](../../../../src/audio/backend/runtime.c#L147)** — cross: [`DynDLL_Load`](../../../../src/platform/bootstrap/runtime.c#L294) → [`platform/bootstrap/runtime`](../../platform/bootstrap/runtime.md), [`DynAPI_Resolve`](../../../../src/platform/bootstrap/runtime.c#L259) → [`platform/bootstrap/runtime`](../../platform/bootstrap/runtime.md), [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406) → [`core/memory/allocator`](../../core/memory/allocator.md), [`CosineDerivedLookupTables_InitCf`](../../../../src/core/math/fixed.c#L352) → [`core/math/fixed`](../../core/math/fixed.md)
- `0x00583490` **[`DirectSound_CreateSampleVoiceSet`](../../../../src/audio/backend/runtime.c#L251)** — cross: [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406) → [`core/memory/allocator`](../../core/memory/allocator.md), [`SoundSample_DecodePackedCoefficientBlock`](../../../../src/audio/codec/sam.c#L2860) → [`audio/codec/sam`](../codec/sam.md), [`SoundSample_DecodeCoefficientBlockToPcmMmx`](../../../../src/audio/codec/sam.c#L941) → [`audio/codec/sam`](../codec/sam.md)
- `0x00583690` **[`DirectSound_ReleaseSampleVoiceSet`](../../../../src/audio/backend/runtime.c#L350)**
- `0x00583720` **[`DirectSound_CreatePcmVoiceSet`](../../../../src/audio/backend/runtime.c#L393)** — cross: [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406) → [`core/memory/allocator`](../../core/memory/allocator.md)
- `0x005838D0` **[`DirectSound_ReleasePcmVoiceSet`](../../../../src/audio/backend/runtime.c#L492)**
- `0x00583940` **[`DirectSound_PlayOneShot`](../../../../src/audio/backend/runtime.c#L539)**
- `0x00583A70` **[`DirectSound_PlayLooping`](../../../../src/audio/backend/runtime.c#L597)**
- `0x00583B90` **[`DirectSound_StopVoice`](../../../../src/audio/backend/runtime.c#L647)**
- `0x00583BC0` **[`DirectSound_IsVoicePlaying`](../../../../src/audio/backend/runtime.c#L660)**
- `0x00583C00` **[`DirectSound_StopAllVoices`](../../../../src/audio/backend/runtime.c#L676)**
- `0x00583C60` **[`DirectSound_QueryVoiceRegsStub`](../../../../src/audio/backend/runtime.c#L724)**
- `0x00583C70` **[`DirectSound_SetVoiceGains`](../../../../src/audio/backend/runtime.c#L735)**
- `0x004175F0` **[`SoundBackendDisabled_StopAllVoices`](../../../../src/audio/backend/runtime.c#L771)**

## Called by

- [`core/memory/synchronization`](../../core/memory/synchronization.md): [`Runtime_Shutdown`](../../../../src/core/memory/synchronization.c#L79) → [`DirectSound_Shutdown`](../../../../src/audio/backend/runtime.c#L5)
- [`platform/bootstrap/runtime`](../../platform/bootstrap/runtime.md): [`ProcessEntry`](../../../../src/platform/bootstrap/runtime.c#L5) → [`DirectSound_Init`](../../../../src/audio/backend/runtime.c#L147)
