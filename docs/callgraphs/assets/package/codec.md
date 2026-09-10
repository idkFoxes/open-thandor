# Call graph: `assets/package/codec`

[Source](../../../../src/assets/package/codec.c) · [Header](../../../../include/thandor/assets/package/codec.h) · [Graphviz](codec.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-assets-package-codec)

6 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0040A9C0` **[`PckCodec_EncodeFieldGrid`](../../../../src/assets/package/codec.c#L5)** — local: [`PckCodec_EncodeHuffmanRle`](../../../../src/assets/package/codec.c#L212)
- `0x0040AAA0` **[`PckCodec_DecodeFieldGrid`](../../../../src/assets/package/codec.c#L68)** — local: [`PckCodec_DecodeHuffmanRle`](../../../../src/assets/package/codec.c#L414)
- `0x0040A960` **[`PckCodec_EncodeStored`](../../../../src/assets/package/codec.c#L166)**
- `0x0040A9A0` **[`PckCodec_DecodeStored`](../../../../src/assets/package/codec.c#L190)**
- `0x0040A4C0` **[`PckCodec_EncodeHuffmanRle`](../../../../src/assets/package/codec.c#L212)**
- `0x0040A790` **[`PckCodec_DecodeHuffmanRle`](../../../../src/assets/package/codec.c#L414)**

## Called by

- [`assets/scenario/catalog`](../scenario/catalog.md): [`FrontendScenarioTransfer_ProcessReceivedAsset`](../../../../src/assets/scenario/catalog.c#L626) → [`PckCodec_DecodeHuffmanRle`](../../../../src/assets/package/codec.c#L414); [`FrontendScenarioTransfer_ProcessReceivedAsset`](../../../../src/assets/scenario/catalog.c#L626) → [`PckCodec_DecodeFieldGrid`](../../../../src/assets/package/codec.c#L68); [`FrontendScenarioSession_LoadOrRequestFieldGrid`](../../../../src/assets/scenario/catalog.c#L871) → [`PckCodec_EncodeFieldGrid`](../../../../src/assets/package/codec.c#L5); [`FrontendScenarioSession_LoadOrRequestCampaignBundle`](../../../../src/assets/scenario/catalog.c#L995) → [`PckCodec_EncodeHuffmanRle`](../../../../src/assets/package/codec.c#L212); [`FrontendScenarioSession_LoadOrRequestCampaignBundle`](../../../../src/assets/scenario/catalog.c#L995) → [`PckCodec_EncodeFieldGrid`](../../../../src/assets/package/codec.c#L5); [`FrontendScenarioSession_LoadOrRequestLevelAsset`](../../../../src/assets/scenario/catalog.c#L1381) → [`PckCodec_EncodeHuffmanRle`](../../../../src/assets/package/codec.c#L212)
- [`network/backend/runtime`](../../network/backend/runtime.md): [`FrontendNetwork_HostTickCommandAndSnapshotTransfer`](../../../../src/network/backend/runtime.c#L142) → [`PckCodec_EncodeHuffmanRle`](../../../../src/assets/package/codec.c#L212)
- [`ui/frontend/runtime`](../../ui/frontend/runtime.md): [`Frontend_MainLoop`](../../../../src/ui/frontend/runtime.c#L5) → [`PckCodec_DecodeHuffmanRle`](../../../../src/assets/package/codec.c#L414); [`Frontend_MainLoop`](../../../../src/ui/frontend/runtime.c#L5) → [`PckCodec_EncodeHuffmanRle`](../../../../src/assets/package/codec.c#L212); [`Frontend_MainLoop`](../../../../src/ui/frontend/runtime.c#L5) → [`PckCodec_EncodeFieldGrid`](../../../../src/assets/package/codec.c#L5)
