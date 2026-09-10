# Call graph: `network/backend/fallback_udp`

19 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0041A580` **NetworkBackendFallback_Slot0_ReturnError43Cf**
- `0x0041A590` **NetworkBackendFallback_Slot1_NoOp**
- `0x0041A5A0` **NetworkBackendFallback_Slot2_ReturnError43Cf**
- `0x0041A5B0` **NetworkBackendFallback_Slot3_NoOp**
- `0x0041A5C0` **NetworkBackendFallback_Slot4_ThreeArgFailureCf**
- `0x0041A5D0` **NetworkBackendFallback_Slot5_ThreeArgSuccessCf**
- `0x0041A5E0` **NetworkBackendFallback_Slot6_TwoArgFailureCf**
- `0x0041A5F0` **NetworkBackendFallback_Slot7_ClearOutput**
- `0x00584E70` **NetworkFallback_NoOpBackendCleanup**
- `0x00584E80` **NetworkFallback_OpenAndBindUdpSocketCf**
- `0x00585030` **NetworkFallback_CloseActiveSocket**
- `0x00585060` **NetworkFallback_ReceiveDatagramCf**
- `0x005850B0` **NetworkFallback_SendDatagramCf**
- `0x00585120` **NetworkFallback_ParsePeerEndpointCf** — cross: `RichTextCommandStream_CopyToNarrowCf` → `assets/text/richtext`
- `0x005851C0` **NetworkFallback_FormatPeerAddress** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`
- `0x00585450` **NetworkFallbackUdp_CloseSocket**
- `0x00585480` **NetworkFallbackUdp_ReceiveDatagram**
- `0x005854E0` **NetworkFallbackUdp_SendDatagram**
- `0x00585640` **NetworkFallback_FormatAddressUtf16** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`
