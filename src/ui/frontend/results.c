#include <thandor/ui/frontend/results.h>

/* Implementation ownership: ui/frontend/results. */

/* Address: 0x00517020.
   Ownership: ui/frontend/results.
   Purpose: The 18-entry table at 00517070 is a FrontendResultsColumnType computed-jump dispatch table whose
   targets are interior labels, not independent functions. [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE] Retired
   detached enum dictionary FrontendResultsColumnType after transferring its complete value vocabulary to code
   annotation. It is not a safe whole-value storage type. Values: 0=FRONTEND_RESULTS_COLUMN_RESERVED_00,
   1=FRONTEND_RESULTS_COLUMN_RESERVED_01, 2=FRONTEND_RESULTS_COLUMN_COLOUR, 3=FRONTEND_RESULTS_COLUMN_ECONOMY,
   4=FRONTEND_RESULTS_COLUMN_MILITARY, 5=FRONTEND_RESULTS_COLUMN_POINTS, 6=FRONTEND_RESULTS_COLUMN_PLAYER,
   7=FRONTEND_RESULTS_COLUMN_FACTION, 8=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_98,
   9=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_9C, 10=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_A0,
   11=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_A4, 12=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_A8,
   13=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_AC, 14=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_B0,
   15=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_B4, 16=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_B8,
   17=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_BC
   Local calls: FrontendResultsTable_DrawColourColumn, FrontendResultsTable_DrawEconomyColumn,
   FrontendResultsTable_DrawMilitaryColumn, FrontendResultsTable_DrawPointsColumn,
   FrontendResultsTable_DrawPlayerColumn, FrontendResultsTable_DrawFactionColumn,
   FrontendResultsTable_DrawFormattedFactionFieldColumn.
   Cross-module calls: TextResource_Resolve [assets/text/resources].
*/
void FrontendResultsTable_DrawColumnSequenceByType
               (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  UiPixelCoordinate drawY;
  void *pvVar1;
  word *pwVar2;
  uint uVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  int extraout_ECX_07;
  int extraout_ECX_08;
  int extraout_ECX_09;
  int extraout_ECX_10;
  int extraout_ECX_11;
  int extraout_ECX_12;
  int extraout_ECX_13;
  int extraout_ECX_14;
  int extraout_ECX_15;
  int iVar4;
  int extraout_ECX_16;
  uint extraout_ECX_17;
  uint uVar5;
  uint extraout_ECX_18;
  undefined4 *extraout_EDX;
  undefined4 *puVar6;
  int *extraout_EDX_00;
  int iVar7;
  int *piVar8;
  bool bVar9;
  undefined8 uVar10;
  
  bVar9 = false;
  if (((uint)control[1].nextSibling & 1) == 0) {
    drawY = control->left;
    iVar7 = control->top;
    (*g_GraphicsFramebufferBeginAccess)();
    iVar4 = extraout_ECX;
    puVar6 = extraout_EDX;
    if (!bVar9) {
      do {
                    
        switch(*puVar6) {
        case 0:
          iVar7 = iVar7 + g_FrontendResultsColumnAdvance00Pixels;
          break;
        case 1:
          iVar7 = iVar7 + g_FrontendResultsColumnAdvance01Pixels;
          break;
        case 2:
          uVar10 = FrontendResultsTable_DrawColourColumn
                             (clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceColourPixels;
          iVar4 = extraout_ECX_00;
          break;
        case 3:
          uVar10 = FrontendResultsTable_DrawEconomyColumn
                             (clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceEconomyPixels;
          iVar4 = extraout_ECX_01;
          break;
        case 4:
          uVar10 = FrontendResultsTable_DrawMilitaryColumn
                             (clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceMilitaryPixels;
          iVar4 = extraout_ECX_02;
          break;
        case 5:
          uVar10 = FrontendResultsTable_DrawPointsColumn
                             (clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvancePointsPixels;
          iVar4 = extraout_ECX_03;
          break;
        case 6:
          uVar10 = FrontendResultsTable_DrawPlayerColumn
                             (clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvancePlayerPixels;
          iVar4 = extraout_ECX_04;
          break;
        case 7:
          uVar10 = FrontendResultsTable_DrawFactionColumn
                             (clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceFactionPixels;
          iVar4 = extraout_ECX_05;
          break;
        case 8:
          uVar10 = FrontendResultsTable_DrawFormattedFactionFieldColumn
                             (0x21c1,0x21b6,0x98,clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceFactionField98Pixels;
          iVar4 = extraout_ECX_06;
          break;
        case 9:
          uVar10 = FrontendResultsTable_DrawFormattedFactionFieldColumn
                             (0x21c2,0x21b7,0x9c,clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceFactionField9CPixels;
          iVar4 = extraout_ECX_07;
          break;
        case 10:
          uVar10 = FrontendResultsTable_DrawFormattedFactionFieldColumn
                             (0x21c3,0x21b8,0xa0,clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceFactionFieldA0Pixels;
          iVar4 = extraout_ECX_08;
          break;
        case 0xb:
          uVar10 = FrontendResultsTable_DrawFormattedFactionFieldColumn
                             (0x21c3,0x21b9,0xa4,clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceFactionFieldA4Pixels;
          iVar4 = extraout_ECX_09;
          break;
        case 0xc:
          uVar10 = FrontendResultsTable_DrawFormattedFactionFieldColumn
                             (0x21c2,0x21ba,0xa8,clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceFactionFieldA8Pixels;
          iVar4 = extraout_ECX_10;
          break;
        case 0xd:
          uVar10 = FrontendResultsTable_DrawFormattedFactionFieldColumn
                             (0x21c2,0x21bb,0xac,clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceFactionFieldACPixels;
          iVar4 = extraout_ECX_11;
          break;
        case 0xe:
          uVar10 = FrontendResultsTable_DrawFormattedFactionFieldColumn
                             (0x21c2,0x21bc,0xb0,clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceFactionFieldB0Pixels;
          iVar4 = extraout_ECX_12;
          break;
        case 0xf:
          uVar10 = FrontendResultsTable_DrawFormattedFactionFieldColumn
                             (0x21c2,0x21bd,0xb4,clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceFactionFieldB4Pixels;
          iVar4 = extraout_ECX_13;
          break;
        case 0x10:
          uVar10 = FrontendResultsTable_DrawFormattedFactionFieldColumn
                             (0x21c2,0x21be,0xb8,clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceFactionFieldB8Pixels;
          iVar4 = extraout_ECX_14;
          break;
        case 0x11:
          uVar10 = FrontendResultsTable_DrawFormattedFactionFieldColumn
                             (0x21c2,0x21bf,0xbc,clipTop,clipLeft,clipBottom,clipRight,iVar7,drawY,
                              (FrontendResultsRowMetrics *)control);
          puVar6 = (undefined4 *)((ulonglong)uVar10 >> 0x20);
          iVar7 = iVar7 + g_FrontendResultsColumnAdvanceFactionFieldBCPixels;
          iVar4 = extraout_ECX_15;
        }
        puVar6 = puVar6 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  else {
    piVar8 = (int *)0x516ff8;
    iVar7 = 0x50fa80;
    do {
      pwVar2 = TextResource_Resolve(*(int *)(iVar7 + 0x38) + 0x2173);
      *piVar8 = (((byte)pwVar2[8] & 0xf) << 0x18 | (uint)(byte)pwVar2[7] << 0x1c) +
                *(int *)((int)g_SoftwarePixelPackTables->red +
                        ((((byte)pwVar2[6] & 0xf) << 0x18 | (uint)(byte)pwVar2[5] << 0x1c) >> 0x16))
                + *(int *)((int)g_SoftwarePixelPackTables->green +
                          ((((byte)pwVar2[4] & 0xf) << 0x18 | (uint)(byte)pwVar2[3] << 0x1c) >> 0x16
                          )) +
                g_SoftwarePixelPackTables->blue
                [(((byte)pwVar2[2] & 0xf) << 0x18 | (uint)(byte)pwVar2[1] << 0x1c) >> 0x18];
      pvVar1 = g_GameStatTableImage;
      iVar7 = iVar7 + 0x740;
      piVar8 = piVar8 + 1;
    } while (extraout_ECX_16 != 1);
    iVar7 = control->layoutWidth;
    bVar9 = (g_GameFactionRuntimeImage.tail.simulationTick >> 6 & 1) != 0;
    (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar9) {
      g_FrontendResultsFramebufferBytesPerPixel = extraout_EDX_00[2];
      g_FrontendResultsFramebufferScanlineStrideBytes =
           *extraout_EDX_00 * g_FrontendResultsFramebufferBytesPerPixel;
      uVar3 = 0;
      uVar5 = extraout_ECX_17;
      do {
        (*(code *)control[1].firstChild)
                  (control->bottom,control->top,uVar3 + control->left,
                   (void *)((int)(((ulonglong)uVar3 * (ulonglong)uVar5) /
                                 (ulonglong)(uint)control->layoutWidth) * 0x38 + (int)pvVar1));
        uVar3 = uVar3 + 1;
        iVar7 = iVar7 + -1;
        uVar5 = extraout_ECX_18;
      } while (iVar7 != 0);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  return;
}

/* Address: 0x00517FB0.
   Ownership: ui/frontend/results.
   Purpose: Shared UiNodeVtable hit-test callback for two frontend-results controls. It consumes the three hit-test
   arguments and returns the 0xFFFFFFFF no-hit sentinel.
*/
undefined4 FrontendResultsTable_HitTestAlwaysNone(void)

{
  return 0xffffffff;
}

/* Address: 0x005177F0.
   Ownership: ui/frontend/results.
   Purpose: Draws localized results-table header 0x21B2 and one colour resource per active faction slot 1 through
   7. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2
   clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4 drawX→UiPixelCoordinate_V297, p5
   drawY→UiPixelCoordinate_V297. Calling convention, exact VariableStorage serialization, function body bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext].
*/
undefined8
FrontendResultsTable_DrawColourColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  undefined4 in_EAX;
  word *pwVar1;
  int extraout_ECX;
  uint uVar2;
  uint extraout_ECX_00;
  undefined4 in_EDX;
  int iVar3;
  int iVar4;
  undefined8 uVar5;
  
  pwVar1 = TextResource_Resolve(0x21b2);
  iVar4 = drawY + rowMetrics->headerBaselineOffsetPixels + -4;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,pwVar1,drawX + 6,iVar4);
  uVar2 = 1;
  iVar3 = 0x50fa80;
  iVar4 = (iVar4 - extraout_ECX) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      pwVar1 = TextResource_Resolve(*(int *)(iVar3 + 0x38) + 0x2173);
      uVar5 = RichTextCommandStream_DrawSingleLine
                        (clipTop,clipLeft,clipBottom,clipRight,2,pwVar1,drawX + 6,iVar4);
      iVar3 = (int)((ulonglong)uVar5 >> 0x20);
      iVar4 = iVar4 + rowMetrics->rowAdvancePixels;
      uVar2 = extraout_ECX_00;
    }
    uVar2 = uVar2 + 1;
    iVar3 = iVar3 + 0x740;
  } while (uVar2 < 8);
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x005178B0.
   Ownership: ui/frontend/results.
   Purpose: Draws localized results-table header 0x21B5 and one localized faction name per active faction slot 1
   through 7. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2
   clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4 drawX→UiPixelCoordinate_V297, p5
   drawY→UiPixelCoordinate_V297. Calling convention, exact VariableStorage serialization, function body bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext].
*/
undefined8
FrontendResultsTable_DrawFactionColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  undefined4 in_EAX;
  word *pwVar1;
  int extraout_ECX;
  uint uVar2;
  uint extraout_ECX_00;
  undefined4 in_EDX;
  int iVar3;
  
  pwVar1 = TextResource_Resolve(0x21b5);
  iVar3 = drawY + rowMetrics->headerBaselineOffsetPixels + -4;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,pwVar1,drawX + 6,iVar3);
  uVar2 = 1;
  iVar3 = (iVar3 - extraout_ECX) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      pwVar1 = TextResource_Resolve(uVar2 + 0x2190);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,pwVar1,drawX + 6,iVar3);
      iVar3 = iVar3 + rowMetrics->rowAdvancePixels;
      uVar2 = extraout_ECX_00;
    }
    uVar2 = uVar2 + 1;
  } while (uVar2 < 8);
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00517960.
   Ownership: ui/frontend/results.
   Purpose: Draws a caller-selected header and formats one caller-selected signed faction-record field through a
   caller-selected localized template. Typed parameters: p0 valueFormatResourceId→TextResourceId_V338, p1
   headerResourceId→TextResourceId_V338. Nearby but non-identical semantic domains were explicitly deferred.
   Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p2 factionFieldOffset→FrontendResultsFactionFieldByteOffset_V342, p3
   clipTop→UiPixelCoordinate_V297, p4 clipLeft→UiPixelCoordinate_V297, p5 clipBottom→UiPixelCoordinate_V297, p6
   clipRight→UiPixelCoordinate_V297, p7 drawX→UiPixelCoordinate_V297, p8 drawY→UiPixelCoordinate_V297.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext].
*/
undefined8
FrontendResultsTable_DrawFormattedFactionFieldColumn
          (TextResourceId valueFormatResourceId,TextResourceId headerResourceId,
          FrontendResultsFactionFieldByteOffset factionFieldOffset,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,
          UiPixelCoordinate drawX,UiPixelCoordinate drawY,FrontendResultsRowMetrics *rowMetrics)

{
  undefined4 in_EAX;
  word *pwVar1;
  int extraout_ECX;
  uint uVar2;
  uint extraout_ECX_00;
  undefined4 in_EDX;
  byte *pbVar3;
  int iVar4;
  undefined8 uVar5;
  
  pwVar1 = TextResource_Resolve(headerResourceId);
  iVar4 = drawY + rowMetrics->headerBaselineOffsetPixels + -4;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,pwVar1,drawX + 6,iVar4);
  uVar2 = 1;
  iVar4 = (iVar4 - extraout_ECX) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  pbVar3 = g_GameFactionRuntimeImage.records[1].reserved78_87 + (factionFieldOffset - 0x78);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,*(sdword *)pbVar3,
                 (word *)&g_FrontendResultsValueTextUtf16);
      pwVar1 = TextResource_Resolve(valueFormatResourceId);
      uVar5 = RichTextCommandStream_PatchPayloadBySelector
                        (0,&g_FrontendResultsValueTextUtf16,pwVar1);
      uVar5 = RichTextCommandStream_DrawSingleLine
                        (clipTop,clipLeft,clipBottom,clipRight,2,(word *)uVar5,drawX + 6,iVar4);
      pbVar3 = (byte *)((ulonglong)uVar5 >> 0x20);
      iVar4 = iVar4 + rowMetrics->rowAdvancePixels;
      uVar2 = extraout_ECX_00;
    }
    uVar2 = uVar2 + 1;
    pbVar3 = pbVar3 + 0x740;
  } while (uVar2 < 8);
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00517A30.
   Ownership: ui/frontend/results.
   Purpose: Draws localized Points header 0x21B3 and formats the sum of faction fields +0x90 and +0x94. Typed
   parameters: p0 clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2
   clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4 drawX→UiPixelCoordinate_V297, p5
   drawY→UiPixelCoordinate_V297. Calling convention, exact VariableStorage serialization, function body bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext].
*/
undefined8
FrontendResultsTable_DrawPointsColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  undefined4 in_EAX;
  word *pwVar1;
  int extraout_ECX;
  uint uVar2;
  uint extraout_ECX_00;
  undefined4 in_EDX;
  int iVar3;
  int iVar4;
  undefined8 uVar5;
  
  pwVar1 = TextResource_Resolve(0x21b3);
  iVar4 = drawY + rowMetrics->headerBaselineOffsetPixels + -4;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,pwVar1,drawX + 6,iVar4);
  uVar2 = 1;
  iVar3 = 0x50fa80;
  iVar4 = (iVar4 - extraout_ECX) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 *(int *)(iVar3 + 0x90) + *(int *)(iVar3 + 0x94),
                 (word *)&g_FrontendResultsValueTextUtf16);
      pwVar1 = TextResource_Resolve(0x21c4);
      uVar5 = RichTextCommandStream_PatchPayloadBySelector
                        (0,&g_FrontendResultsValueTextUtf16,pwVar1);
      uVar5 = RichTextCommandStream_DrawSingleLine
                        (clipTop,clipLeft,clipBottom,clipRight,2,(word *)uVar5,drawX + 6,iVar4);
      iVar3 = (int)((ulonglong)uVar5 >> 0x20);
      iVar4 = iVar4 + rowMetrics->rowAdvancePixels;
      uVar2 = extraout_ECX_00;
    }
    uVar2 = uVar2 + 1;
    iVar3 = iVar3 + 0x740;
  } while (uVar2 < 8);
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00517B10.
   Ownership: ui/frontend/results.
   Purpose: Draws localized Economy header 0x21B0 and formats faction field +0x90. Typed parameters: p0
   clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3
   clipRight→UiPixelCoordinate_V297, p4 drawX→UiPixelCoordinate_V297, p5 drawY→UiPixelCoordinate_V297. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext].
*/
undefined8
FrontendResultsTable_DrawEconomyColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  undefined4 in_EAX;
  word *pwVar1;
  int extraout_ECX;
  uint uVar2;
  uint extraout_ECX_00;
  undefined4 in_EDX;
  int iVar3;
  int iVar4;
  undefined8 uVar5;
  
  pwVar1 = TextResource_Resolve(0x21b0);
  iVar4 = drawY + rowMetrics->headerBaselineOffsetPixels + -4;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,pwVar1,drawX + 6,iVar4);
  uVar2 = 1;
  iVar3 = 0x50fa80;
  iVar4 = (iVar4 - extraout_ECX) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 *(sdword *)(iVar3 + 0x90),(word *)&g_FrontendResultsValueTextUtf16);
      pwVar1 = TextResource_Resolve(0x21c4);
      uVar5 = RichTextCommandStream_PatchPayloadBySelector
                        (0,&g_FrontendResultsValueTextUtf16,pwVar1);
      uVar5 = RichTextCommandStream_DrawSingleLine
                        (clipTop,clipLeft,clipBottom,clipRight,2,(word *)uVar5,drawX + 6,iVar4);
      iVar3 = (int)((ulonglong)uVar5 >> 0x20);
      iVar4 = iVar4 + rowMetrics->rowAdvancePixels;
      uVar2 = extraout_ECX_00;
    }
    uVar2 = uVar2 + 1;
    iVar3 = iVar3 + 0x740;
  } while (uVar2 < 8);
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00517BF0.
   Ownership: ui/frontend/results.
   Purpose: Draws localized Military header 0x21B1 and formats faction field +0x94. Typed parameters: p0
   clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3
   clipRight→UiPixelCoordinate_V297, p4 drawX→UiPixelCoordinate_V297, p5 drawY→UiPixelCoordinate_V297. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext].
*/
undefined8
FrontendResultsTable_DrawMilitaryColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  undefined4 in_EAX;
  word *pwVar1;
  int extraout_ECX;
  uint uVar2;
  uint extraout_ECX_00;
  undefined4 in_EDX;
  int iVar3;
  int iVar4;
  undefined8 uVar5;
  
  pwVar1 = TextResource_Resolve(0x21b1);
  iVar4 = drawY + rowMetrics->headerBaselineOffsetPixels + -4;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,pwVar1,drawX + 6,iVar4);
  uVar2 = 1;
  iVar3 = 0x50fa80;
  iVar4 = (iVar4 - extraout_ECX) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 *(sdword *)(iVar3 + 0x94),(word *)&g_FrontendResultsValueTextUtf16);
      pwVar1 = TextResource_Resolve(0x21c4);
      uVar5 = RichTextCommandStream_PatchPayloadBySelector
                        (0,&g_FrontendResultsValueTextUtf16,pwVar1);
      uVar5 = RichTextCommandStream_DrawSingleLine
                        (clipTop,clipLeft,clipBottom,clipRight,2,(word *)uVar5,drawX + 6,iVar4);
      iVar3 = (int)((ulonglong)uVar5 >> 0x20);
      iVar4 = iVar4 + rowMetrics->rowAdvancePixels;
      uVar2 = extraout_ECX_00;
    }
    uVar2 = uVar2 + 1;
    iVar3 = iVar3 + 0x740;
  } while (uVar2 < 8);
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00517CD0.
   Ownership: ui/frontend/results.
   Purpose: Draws localized Player header 0x21B4 and up to three matching names from exact 0x13B0-byte frontend
   player blocks. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2
   clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4 drawX→UiPixelCoordinate_V297, p5
   drawY→UiPixelCoordinate_V297. Calling convention, exact VariableStorage serialization, function body bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext].
*/
undefined8
FrontendResultsTable_DrawPlayerColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  undefined4 in_EAX;
  word *commandStream;
  int extraout_ECX;
  uint uVar1;
  uint extraout_ECX_00;
  undefined4 in_EDX;
  FrontendPlayerNameUtf16_28 *commandStream_00;
  FrontendPlayerRuntimeBlockCount FVar2;
  int drawX_00;
  int iVar3;
  int iVar4;
  undefined8 uVar5;
  UiPixelCoordinate clipLeft_00;
  UiPixelCoordinate clipRight_00;
  uint uVar6;
  int local_24;
  int local_20;
  
  commandStream = TextResource_Resolve(0x21b4);
  iVar4 = drawY + rowMetrics->headerBaselineOffsetPixels + -4;
  drawX_00 = drawX + 6;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,commandStream,drawX_00,iVar4);
  local_20 = (iVar4 - extraout_ECX) + rowMetrics->headerBaselineOffsetPixels;
  uVar1 = 1;
  local_24 = rowMetrics->rowAdvancePixels + local_20;
  iVar4 = local_20 + (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar1] != 0) {
      uVar6 = 0;
      commandStream_00 = &g_FrontendPlayerRuntimeBlocks->playerName;
      FVar2 = g_FrontendPlayerRuntimeBlockCount;
      iVar3 = drawX_00;
      do {
        if ((uVar1 == *(FrontendFactionAssignmentIndex *)
                       ((int)((UiTransferEndpointDescriptor *)(commandStream_00 + 1) + 1) + 8)) &&
           (uVar6 < 3)) {
          uVar6 = uVar6 + 1;
          clipRight_00 = clipRight;
          if (clipRight < local_20) {
            clipRight_00 = local_20;
          }
          clipLeft_00 = clipLeft;
          if (local_24 < clipLeft) {
            clipLeft_00 = local_24;
          }
          uVar5 = RichTextCommandStream_DrawSingleLine
                            (clipTop,clipLeft_00,clipBottom,clipRight_00,2,
                             commandStream_00->textUtf16,iVar3,iVar4);
          commandStream_00 = (FrontendPlayerNameUtf16_28 *)((ulonglong)uVar5 >> 0x20);
          iVar3 = iVar3 + 0x1a;
          uVar1 = extraout_ECX_00;
        }
        commandStream_00 = commandStream_00 + 0x7e;
        FVar2 = FVar2 - 1;
      } while (FVar2 != 0);
      iVar3 = rowMetrics->rowAdvancePixels;
      iVar4 = iVar4 + iVar3;
      local_20 = local_20 + iVar3;
      local_24 = local_24 + iVar3;
    }
    uVar1 = uVar1 + 1;
  } while (uVar1 < 8);
  return CONCAT44(in_EDX,in_EAX);
}
