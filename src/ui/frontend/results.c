/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/results.c
 * Reverse engineering by idkFoxes 2026
 */

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
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawColumnSequenceByType
          (int clipTop,int clipLeft,int clipBottom,int clipRight,
          FrontendResultsColumnSequenceControl68 *control)

{
  UiPixelCoordinate drawY;
  SoftwareFramebufferAccess *pSVar1;
  void *pvVar2;
  word *pwVar3;
  uint uVar4;
  dword dVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  dword *pdVar9;
  bool bVar10;
  TextResourceResolveEaxCf5 TVar11;
  
  if ((control->modeFlags & 1) == 0) {
    drawY = (control->base).left;
    iVar6 = (control->base).top;
    dVar5 = control->columnTypeCount;
    pdVar9 = &control->columnTypes0;
    bVar10 = (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar10) {
      do {
                    // WARNING: Switch is manually overridden
        switch(*pdVar9) {
        case 0:
          iVar6 = iVar6 + g_FrontendResultsColumnAdvance00Pixels;
          break;
        case 1:
          iVar6 = iVar6 + g_FrontendResultsColumnAdvance01Pixels;
          break;
        case 2:
          FrontendResultsTable_DrawColourColumn
                    (clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceColourPixels;
          break;
        case 3:
          FrontendResultsTable_DrawEconomyColumn
                    (clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceEconomyPixels;
          break;
        case 4:
          FrontendResultsTable_DrawMilitaryColumn
                    (clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceMilitaryPixels;
          break;
        case 5:
          FrontendResultsTable_DrawPointsColumn
                    (clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvancePointsPixels;
          break;
        case 6:
          FrontendResultsTable_DrawPlayerColumn
                    (clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvancePlayerPixels;
          break;
        case 7:
          FrontendResultsTable_DrawFactionColumn
                    (clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceFactionPixels;
          break;
        case 8:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c1,0x21b6,0x98,clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceFactionField98Pixels;
          break;
        case 9:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21b7,0x9c,clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceFactionField9CPixels;
          break;
        case 10:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c3,0x21b8,0xa0,clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceFactionFieldA0Pixels;
          break;
        case 0xb:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c3,0x21b9,0xa4,clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceFactionFieldA4Pixels;
          break;
        case 0xc:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21ba,0xa8,clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceFactionFieldA8Pixels;
          break;
        case 0xd:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21bb,0xac,clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceFactionFieldACPixels;
          break;
        case 0xe:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21bc,0xb0,clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceFactionFieldB0Pixels;
          break;
        case 0xf:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21bd,0xb4,clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceFactionFieldB4Pixels;
          break;
        case 0x10:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21be,0xb8,clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceFactionFieldB8Pixels;
          break;
        case 0x11:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21bf,0xbc,clipTop,clipLeft,clipBottom,clipRight,iVar6,drawY,
                     (FrontendResultsRowMetrics *)control);
          iVar6 = iVar6 + g_FrontendResultsColumnAdvanceFactionFieldBCPixels;
        }
        pdVar9 = pdVar9 + 1;
        dVar5 = dVar5 - 1;
      } while (dVar5 != 0);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  else {
    iVar6 = 7;
    pdVar9 = g_FrontendResultsFactionPackedPixelColors;
    iVar8 = 0x50fa80;
    do {
      TVar11 = TextResource_Resolve(*(int *)(iVar8 + 0x38) + 0x2173);
      pwVar3 = TVar11.eax;
      *pdVar9 = (((byte)pwVar3[8] & 0xf) << 0x18 | (uint)(byte)pwVar3[7] << 0x1c) +
                *(int *)((int)g_SoftwarePixelPackTables->red +
                        ((((byte)pwVar3[6] & 0xf) << 0x18 | (uint)(byte)pwVar3[5] << 0x1c) >> 0x16))
                + *(int *)((int)g_SoftwarePixelPackTables->green +
                          ((((byte)pwVar3[4] & 0xf) << 0x18 | (uint)(byte)pwVar3[3] << 0x1c) >> 0x16
                          )) +
                g_SoftwarePixelPackTables->blue
                [(((byte)pwVar3[2] & 0xf) << 0x18 | (uint)(byte)pwVar3[1] << 0x1c) >> 0x18];
      pvVar2 = g_GameStatTableImage;
      pSVar1 = g_FramebufferAccess;
      iVar8 = iVar8 + 0x740;
      pdVar9 = pdVar9 + 1;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
    iVar6 = (control->base).layoutWidth;
    uVar7 = g_GameFactionRuntimeImage.tail.simulationTick >> 7;
    bVar10 = (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar10) {
      g_FrontendResultsFramebufferBytesPerPixel = pSVar1->bytesPerPixel;
      g_FrontendResultsFramebufferScanlineStrideBytes =
           pSVar1->width * g_FrontendResultsFramebufferBytesPerPixel;
      uVar4 = 0;
      do {
        (*control->factionWeightRaster)
                  ((control->base).bottom,(control->base).top,uVar4 + (control->base).left,
                   (FrontendResultsFactionWeightPair8 *)
                   ((int)(((ulonglong)uVar4 * (ulonglong)uVar7) /
                         (ulonglong)(uint)(control->base).layoutWidth) * 0x38 + (int)pvVar2));
        uVar4 = uVar4 + 1;
        iVar6 = iVar6 + -1;
      } while (iVar6 != 0);
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
UiNodeBase * __thandor_eax_preserve_ecx_edx
FrontendResultsTable_HitTestAlwaysNone
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  return (UiNodeBase *)0xffffffff;
}


/* Address: 0x005174E0.
   Ownership: ui/frontend/results.
   Purpose: Draws the faction-weight sum column in the frontend results graph.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsGraph_DrawFactionWeightSumColumn
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair8 *factionWeights)

{
  dword dVar1;
  SoftwareFramebufferAccess *pSVar2;
  uint uVar3;
  uint uVar4;
  byte *pbVar5;
  int iVar6;
  int iStack_24;
  uint uStack_20;
  
  pSVar2 = g_FramebufferAccess;
  uVar3 = (*factionWeights).lane0 + factionWeights[1].lane0 + factionWeights[2].lane0 +
          factionWeights[3].lane0 + factionWeights[4].lane0 + factionWeights[5].lane0 +
          factionWeights[6].lane0 +
          (*factionWeights).lane1 + factionWeights[1].lane1 + factionWeights[2].lane1 +
          factionWeights[3].lane1 + factionWeights[4].lane1 + factionWeights[5].lane1 +
          factionWeights[6].lane1;
  if (uVar3 == 0) {
    uVar4 = 1;
    do {
      if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar4] != 0) {
        factionWeights[uVar4 - 1].lane0 = factionWeights[uVar4 - 1].lane0 + 1;
        uVar3 = uVar3 + 1;
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < 8);
  }
  pbVar5 = pSVar2->pixels +
           (spanStartY * pSVar2->width + drawX) * g_FrontendResultsFramebufferBytesPerPixel;
  uVar4 = 0;
  uStack_20 = 0;
  iVar6 = 0;
  do {
    uStack_20 = uStack_20 + factionWeights->lane0 + factionWeights->lane1;
    iStack_24 = (int)(((ulonglong)uStack_20 * (ulonglong)(uint)(spanEndY - spanStartY)) /
                     (ulonglong)uVar3) - iVar6;
    if (iStack_24 != 0) {
      iVar6 = iVar6 + iStack_24;
      dVar1 = g_FrontendResultsFactionPackedPixelColors[uVar4];
      if (g_FrontendResultsFramebufferBytesPerPixel != 2) {
        *(short *)pbVar5 = (short)dVar1;
        pbVar5 = pbVar5 + g_FrontendResultsFramebufferScanlineStrideBytes;
        iStack_24 = iStack_24 + -1;
        if (iStack_24 == 0) goto LAB_005175d0;
      }
      do {
        *(short *)pbVar5 = (short)dVar1;
        pbVar5 = pbVar5 + g_FrontendResultsFramebufferScanlineStrideBytes;
        iStack_24 = iStack_24 + -1;
      } while (iStack_24 != 0);
    }
LAB_005175d0:
    uVar4 = uVar4 + 1;
    factionWeights = factionWeights + 1;
    if (6 < uVar4) {
      return;
    }
  } while( true );
}

/* Address: 0x005175F0.
   Ownership: ui/frontend/results.
   Purpose: Draws faction-weight lane 0 in the frontend results graph.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsGraph_DrawFactionWeightLane0Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair8 *factionWeights)

{
  dword dVar1;
  SoftwareFramebufferAccess *pSVar2;
  uint uVar3;
  byte *pbVar4;
  int iVar5;
  uint uVar6;
  int iStack_24;
  uint uStack_20;
  
  pSVar2 = g_FramebufferAccess;
  uVar6 = (*factionWeights).lane0 + factionWeights[1].lane0 + factionWeights[2].lane0 +
          factionWeights[3].lane0 + factionWeights[4].lane0 + factionWeights[5].lane0 +
          factionWeights[6].lane0;
  if (uVar6 == 0) {
    uVar3 = 1;
    do {
      if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar3] != 0) {
        factionWeights[uVar3 - 1].lane0 = factionWeights[uVar3 - 1].lane0 + 1;
        uVar6 = uVar6 + 1;
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 < 8);
  }
  pbVar4 = pSVar2->pixels +
           (spanStartY * pSVar2->width + drawX) * g_FrontendResultsFramebufferBytesPerPixel;
  uVar3 = 0;
  uStack_20 = 0;
  iVar5 = 0;
  do {
    uStack_20 = uStack_20 + factionWeights->lane0;
    iStack_24 = (int)(((ulonglong)uStack_20 * (ulonglong)(uint)(spanEndY - spanStartY)) /
                     (ulonglong)uVar6) - iVar5;
    if (iStack_24 != 0) {
      iVar5 = iVar5 + iStack_24;
      dVar1 = g_FrontendResultsFactionPackedPixelColors[uVar3];
      if (g_FrontendResultsFramebufferBytesPerPixel != 2) {
        *(short *)pbVar4 = (short)dVar1;
        pbVar4 = pbVar4 + g_FrontendResultsFramebufferScanlineStrideBytes;
        iStack_24 = iStack_24 + -1;
        if (iStack_24 == 0) goto LAB_005176d0;
      }
      do {
        *(short *)pbVar4 = (short)dVar1;
        pbVar4 = pbVar4 + g_FrontendResultsFramebufferScanlineStrideBytes;
        iStack_24 = iStack_24 + -1;
      } while (iStack_24 != 0);
    }
LAB_005176d0:
    uVar3 = uVar3 + 1;
    factionWeights = factionWeights + 1;
    if (6 < uVar3) {
      return;
    }
  } while( true );
}

/* Address: 0x005176F0.
   Ownership: ui/frontend/results.
   Purpose: Draws faction-weight lane 1 in the frontend results graph.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsGraph_DrawFactionWeightLane1Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair8 *factionWeights)

{
  dword dVar1;
  SoftwareFramebufferAccess *pSVar2;
  uint uVar3;
  byte *pbVar4;
  int iVar5;
  uint uVar6;
  int iStack_24;
  uint uStack_20;
  
  pSVar2 = g_FramebufferAccess;
  uVar6 = (*factionWeights).lane1 + factionWeights[1].lane1 + factionWeights[2].lane1 +
          factionWeights[3].lane1 + factionWeights[4].lane1 + factionWeights[5].lane1 +
          factionWeights[6].lane1;
  if (uVar6 == 0) {
    uVar3 = 1;
    do {
      if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar3] != 0) {
        factionWeights[uVar3 - 1].lane1 = factionWeights[uVar3 - 1].lane1 + 1;
        uVar6 = uVar6 + 1;
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 < 8);
  }
  pbVar4 = pSVar2->pixels +
           (spanStartY * pSVar2->width + drawX) * g_FrontendResultsFramebufferBytesPerPixel;
  uVar3 = 0;
  uStack_20 = 0;
  iVar5 = 0;
  do {
    uStack_20 = uStack_20 + factionWeights->lane1;
    iStack_24 = (int)(((ulonglong)uStack_20 * (ulonglong)(uint)(spanEndY - spanStartY)) /
                     (ulonglong)uVar6) - iVar5;
    if (iStack_24 != 0) {
      iVar5 = iVar5 + iStack_24;
      dVar1 = g_FrontendResultsFactionPackedPixelColors[uVar3];
      if (g_FrontendResultsFramebufferBytesPerPixel != 2) {
        *(short *)pbVar4 = (short)dVar1;
        pbVar4 = pbVar4 + g_FrontendResultsFramebufferScanlineStrideBytes;
        iStack_24 = iStack_24 + -1;
        if (iStack_24 == 0) goto LAB_005177d0;
      }
      do {
        *(short *)pbVar4 = (short)dVar1;
        pbVar4 = pbVar4 + g_FrontendResultsFramebufferScanlineStrideBytes;
        iStack_24 = iStack_24 + -1;
      } while (iStack_24 != 0);
    }
LAB_005177d0:
    uVar3 = uVar3 + 1;
    factionWeights = factionWeights + 1;
    if (6 < uVar3) {
      return;
    }
  } while( true );
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
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawColourColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int baselineY;
  TextResourceResolveEaxCf5 TVar4;
  
  TVar4 = TextResource_Resolve(0x21b2);
  iVar1 = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + iVar1;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,TVar4.eax,drawX + 6,baselineY);
  uVar2 = 1;
  iVar3 = 0x50fa80;
  iVar1 = (baselineY - iVar1) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      TVar4 = TextResource_Resolve(*(int *)(iVar3 + 0x38) + 0x2173);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,TVar4.eax,drawX + 6,iVar1);
      iVar1 = iVar1 + rowMetrics->rowAdvancePixels;
    }
    uVar2 = uVar2 + 1;
    iVar3 = iVar3 + 0x740;
  } while (uVar2 < 8);
  return;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawFactionColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int iVar1;
  uint uVar2;
  int baselineY;
  TextResourceResolveEaxCf5 TVar3;
  
  TVar3 = TextResource_Resolve(0x21b5);
  iVar1 = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + iVar1;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,TVar3.eax,drawX + 6,baselineY);
  uVar2 = 1;
  iVar1 = (baselineY - iVar1) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      TVar3 = TextResource_Resolve(uVar2 + 0x2190);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,TVar3.eax,drawX + 6,iVar1);
      iVar1 = iVar1 + rowMetrics->rowAdvancePixels;
    }
    uVar2 = uVar2 + 1;
  } while (uVar2 < 8);
  return;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawFormattedFactionFieldColumn
          (TextResourceId valueFormatResourceId,TextResourceId headerResourceId,
          FrontendResultsFactionFieldByteOffset factionFieldOffset,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,
          UiPixelCoordinate drawX,UiPixelCoordinate drawY,FrontendResultsRowMetrics *rowMetrics)

{
  int iVar1;
  uint uVar2;
  byte *pbVar3;
  int baselineY;
  TextResourceResolveEaxCf5 TVar4;
  
  TVar4 = TextResource_Resolve(headerResourceId);
  iVar1 = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + iVar1;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,TVar4.eax,drawX + 6,baselineY);
  uVar2 = 1;
  iVar1 = (baselineY - iVar1) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  pbVar3 = g_GameFactionRuntimeImage.records[1].reserved78_87 + (factionFieldOffset - 0x78);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,*(sdword *)pbVar3,
                 (word *)&g_FrontendResultsValueTextUtf16);
      TVar4 = TextResource_Resolve(valueFormatResourceId);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,TVar4.eax);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,TVar4.eax,drawX + 6,iVar1);
      iVar1 = iVar1 + rowMetrics->rowAdvancePixels;
    }
    uVar2 = uVar2 + 1;
    pbVar3 = pbVar3 + 0x740;
  } while (uVar2 < 8);
  return;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawPointsColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int baselineY;
  TextResourceResolveEaxCf5 TVar4;
  
  TVar4 = TextResource_Resolve(0x21b3);
  iVar1 = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + iVar1;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,TVar4.eax,drawX + 6,baselineY);
  uVar2 = 1;
  iVar3 = 0x50fa80;
  iVar1 = (baselineY - iVar1) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 *(int *)(iVar3 + 0x90) + *(int *)(iVar3 + 0x94),
                 (word *)&g_FrontendResultsValueTextUtf16);
      TVar4 = TextResource_Resolve(0x21c4);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,TVar4.eax);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,TVar4.eax,drawX + 6,iVar1);
      iVar1 = iVar1 + rowMetrics->rowAdvancePixels;
    }
    uVar2 = uVar2 + 1;
    iVar3 = iVar3 + 0x740;
  } while (uVar2 < 8);
  return;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawEconomyColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int baselineY;
  TextResourceResolveEaxCf5 TVar4;
  
  TVar4 = TextResource_Resolve(0x21b0);
  iVar1 = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + iVar1;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,TVar4.eax,drawX + 6,baselineY);
  uVar2 = 1;
  iVar3 = 0x50fa80;
  iVar1 = (baselineY - iVar1) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 *(sdword *)(iVar3 + 0x90),(word *)&g_FrontendResultsValueTextUtf16);
      TVar4 = TextResource_Resolve(0x21c4);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,TVar4.eax);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,TVar4.eax,drawX + 6,iVar1);
      iVar1 = iVar1 + rowMetrics->rowAdvancePixels;
    }
    uVar2 = uVar2 + 1;
    iVar3 = iVar3 + 0x740;
  } while (uVar2 < 8);
  return;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawMilitaryColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int baselineY;
  TextResourceResolveEaxCf5 TVar4;
  
  TVar4 = TextResource_Resolve(0x21b1);
  iVar1 = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + iVar1;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,TVar4.eax,drawX + 6,baselineY);
  uVar2 = 1;
  iVar3 = 0x50fa80;
  iVar1 = (baselineY - iVar1) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 *(sdword *)(iVar3 + 0x94),(word *)&g_FrontendResultsValueTextUtf16);
      TVar4 = TextResource_Resolve(0x21c4);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,TVar4.eax);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,TVar4.eax,drawX + 6,iVar1);
      iVar1 = iVar1 + rowMetrics->rowAdvancePixels;
    }
    uVar2 = uVar2 + 1;
    iVar3 = iVar3 + 0x740;
  } while (uVar2 < 8);
  return;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawPlayerColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int iVar1;
  uint uVar2;
  FrontendPlayerNameUtf16_28 *commandStream;
  FrontendPlayerRuntimeBlockCount FVar3;
  int drawX_00;
  int iVar4;
  TextResourceResolveEaxCf5 TVar5;
  UiPixelCoordinate clipLeft_00;
  UiPixelCoordinate clipRight_00;
  uint uVar6;
  int local_24;
  int local_20;
  
  TVar5 = TextResource_Resolve(0x21b4);
  iVar1 = rowMetrics->headerBaselineOffsetPixels + -4;
  iVar4 = drawY + iVar1;
  drawX_00 = drawX + 6;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,TVar5.eax,drawX_00,iVar4);
  local_20 = (iVar4 - iVar1) + rowMetrics->headerBaselineOffsetPixels;
  uVar2 = 1;
  local_24 = rowMetrics->rowAdvancePixels + local_20;
  iVar1 = local_20 + (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar2] != 0) {
      uVar6 = 0;
      commandStream = &g_FrontendPlayerRuntimeBlocks->playerName;
      FVar3 = g_FrontendPlayerRuntimeBlockCount;
      iVar4 = drawX_00;
      do {
        if ((uVar2 == *(FrontendFactionAssignmentIndex *)
                       ((int)((UiTransferEndpointDescriptor *)(commandStream + 1) + 1) + 8)) &&
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
          RichTextCommandStream_DrawSingleLine
                    (clipTop,clipLeft_00,clipBottom,clipRight_00,2,commandStream->textUtf16,iVar4,
                     iVar1);
          iVar4 = iVar4 + 0x1a;
        }
        commandStream = commandStream + 0x7e;
        FVar3 = FVar3 - 1;
      } while (FVar3 != 0);
      iVar4 = rowMetrics->rowAdvancePixels;
      iVar1 = iVar1 + iVar4;
      local_20 = local_20 + iVar4;
      local_24 = local_24 + iVar4;
    }
    uVar2 = uVar2 + 1;
  } while (uVar2 < 8);
  return;
}

