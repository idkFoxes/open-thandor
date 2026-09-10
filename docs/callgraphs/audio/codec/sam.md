# Call graph: `audio/codec/sam`

[Source](../../../../src/audio/codec/sam.c) · [Header](../../../../include/thandor/audio/codec/sam.h) · [Graphviz](sam.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-audio-codec-sam)

4 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00417700` **[`SoundCoefficientTransform_ApplyCosineBanksMmx`](../../../../src/audio/codec/sam.c#L5)**
- `0x00418560` **[`SoundSample_DecodeCoefficientBlockToPcmMmx`](../../../../src/audio/codec/sam.c#L941)**
- `0x004193D0` **[`SoundSample_DecodeCoefficientBlockToMonoPcmMmx`](../../../../src/audio/codec/sam.c#L1870)**
- `0x0041A430` **[`SoundSample_DecodePackedCoefficientBlock`](../../../../src/audio/codec/sam.c#L2860)**

## Called by

- [`audio/backend/runtime`](../backend/runtime.md): [`DirectSound_CreateSampleVoiceSet`](../../../../src/audio/backend/runtime.c#L251) → [`SoundSample_DecodePackedCoefficientBlock`](../../../../src/audio/codec/sam.c#L2860); [`DirectSound_CreateSampleVoiceSet`](../../../../src/audio/backend/runtime.c#L251) → [`SoundSample_DecodeCoefficientBlockToPcmMmx`](../../../../src/audio/codec/sam.c#L941)
