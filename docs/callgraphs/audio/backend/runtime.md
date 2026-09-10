# Call graph: `audio/backend/runtime`

24 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00583410` **DirectSound_Shutdown** — local: `SoundBackendDisabled_StopAllVoices`
- `0x00417570` **SoundBackendDisabled_CreateSampleVoiceSet**
- `0x00417580` **SoundBackendDisabled_ReleaseSampleVoiceSet**
- `0x00417590` **SoundBackendDisabled_CreatePcmVoiceSet**
- `0x004175A0` **SoundBackendDisabled_ReleasePcmVoiceSet**
- `0x004175B0` **SoundBackendDisabled_PlayOneShot**
- `0x004175C0` **SoundBackendDisabled_PlayLooping**
- `0x004175D0` **SoundBackendDisabled_StopVoice**
- `0x004175E0` **SoundBackendDisabled_IsVoicePlaying**
- `0x00417600` **SoundBackendDisabled_QueryVoiceRegs**
- `0x00417610` **SoundBackendDisabled_SetVoiceGains**
- `0x00583140` **DirectSound_Init** — cross: `DynDLL_Load` → `platform/bootstrap/runtime`, `DynAPI_Resolve` → `platform/bootstrap/runtime`, `Memory_ZeroDwords` → `core/memory/allocator`, `CosineDerivedLookupTables_InitCf` → `core/math/fixed`
- `0x00583490` **DirectSound_CreateSampleVoiceSet** — cross: `Memory_ZeroDwords` → `core/memory/allocator`, `SoundSample_DecodePackedCoefficientBlock` → `audio/codec/sam`, `SoundSample_DecodeCoefficientBlockToPcmMmx` → `audio/codec/sam`
- `0x00583690` **DirectSound_ReleaseSampleVoiceSet**
- `0x00583720` **DirectSound_CreatePcmVoiceSet** — cross: `Memory_ZeroDwords` → `core/memory/allocator`
- `0x005838D0` **DirectSound_ReleasePcmVoiceSet**
- `0x00583940` **DirectSound_PlayOneShot**
- `0x00583A70` **DirectSound_PlayLooping**
- `0x00583B90` **DirectSound_StopVoice**
- `0x00583BC0` **DirectSound_IsVoicePlaying**
- `0x00583C00` **DirectSound_StopAllVoices**
- `0x00583C60` **DirectSound_QueryVoiceRegsStub**
- `0x00583C70` **DirectSound_SetVoiceGains**
- `0x004175F0` **SoundBackendDisabled_StopAllVoices**
