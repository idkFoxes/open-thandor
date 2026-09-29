# Call graph: `network/backend/fallback_udp`

[Source](../../../../src/network/backend/fallback_udp.c) · [Header](../../../../include/thandor/network/backend/fallback_udp.h) · [Graphviz](fallback_udp.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-network-backend-fallback-udp) · [Developer notes](../../../../CHANGELOG.md#module-network-backend-fallback-udp) · [Full changelog](../../../../CHANGELOG_FULL.md#module-network-backend-fallback-udp)

19 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0041A580` **[`NetworkBackendFallback_SetSessionContext`](../../../../src/network/backend/fallback_udp.c#L5)**
- `0x0041A590` **[`NetworkBackendFallback_Cleanup`](../../../../src/network/backend/fallback_udp.c#L16)**
- `0x0041A5A0` **[`NetworkBackendFallback_OpenAndBindUdpSocket`](../../../../src/network/backend/fallback_udp.c#L26)**
- `0x0041A5B0` **[`NetworkBackendFallback_CloseActiveSocket`](../../../../src/network/backend/fallback_udp.c#L37)**
- `0x0041A5C0` **[`NetworkBackendFallback_ReceiveDatagram`](../../../../src/network/backend/fallback_udp.c#L47)**
- `0x0041A5D0` **[`NetworkBackendFallback_SendDatagram`](../../../../src/network/backend/fallback_udp.c#L59)**
- `0x0041A5E0` **[`NetworkBackendFallback_ParsePeerEndpoint`](../../../../src/network/backend/fallback_udp.c#L72)**
- `0x0041A5F0` **[`NetworkBackendFallback_FormatPeerAddress`](../../../../src/network/backend/fallback_udp.c#L84)**
- `0x00584E70` **[`NetworkFallback_NoOpBackendCleanup`](../../../../src/network/backend/fallback_udp.c#L99)**
- `0x00584E80` **[`NetworkFallback_OpenAndBindUdpSocket`](../../../../src/network/backend/fallback_udp.c#L109)**
- `0x00585030` **[`NetworkFallback_CloseActiveSocket`](../../../../src/network/backend/fallback_udp.c#L210)**
- `0x00585060` **[`NetworkFallback_ReceiveDatagram`](../../../../src/network/backend/fallback_udp.c#L230)**
- `0x005850B0` **[`NetworkFallback_SendDatagram`](../../../../src/network/backend/fallback_udp.c#L255)**
- `0x00585120` **[`NetworkFallback_ParsePeerEndpoint`](../../../../src/network/backend/fallback_udp.c#L280)** — cross: [`RichTextCommandStream_CopyToNarrow`](../../../../src/assets/text/richtext.c#L598) → [`assets/text/richtext`](../../assets/text/richtext.md)
- `0x005851C0` **[`NetworkFallback_FormatPeerAddress`](../../../../src/network/backend/fallback_udp.c#L325)** — cross: [`Text_CopyNarrowToUtf16`](../../../../src/core/text/string.c#L185) → [`core/text/string`](../../core/text/string.md)
- `0x00585450` **[`NetworkFallbackUdp_CloseSocket`](../../../../src/network/backend/fallback_udp.c#L348)**
- `0x00585480` **[`NetworkFallbackUdp_ReceiveDatagram`](../../../../src/network/backend/fallback_udp.c#L369)**
- `0x005854E0` **[`NetworkFallbackUdp_SendDatagram`](../../../../src/network/backend/fallback_udp.c#L391)**
- `0x00585640` **[`NetworkFallback_FormatAddressUtf16`](../../../../src/network/backend/fallback_udp.c#L415)** — cross: [`Text_CopyNarrowToUtf16`](../../../../src/core/text/string.c#L185) → [`core/text/string`](../../core/text/string.md)

## Called by

No cross-module callers are recorded.
