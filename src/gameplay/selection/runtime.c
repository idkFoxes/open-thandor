#include <thandor/gameplay/selection/runtime.h>

/* Implementation ownership: gameplay/selection/runtime. */

/* Address: 0x0052E350.
   Ownership: gameplay/selection/runtime.
   Purpose: Begins framebuffer access and composes the complete army and model-runtime selection panel from
   hierarchy metrics, status flags, icons, number cells, bars, and segmented indicators before ending access. Typed
   parameters: p2 clipTop→UiPixelCoordinate_V297, p3 clipLeft→UiPixelCoordinate_V297, p4
   clipBottom→UiPixelCoordinate_V297, p5 clipRight→UiPixelCoordinate_V297. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p6 panelBottom→UiPixelCoordinate_V297, p7 panelRight→UiPixelCoordinate_V297, p8
   panelTop→UiPixelCoordinate_V297, p9 panelLeft→UiPixelCoordinate_V297. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs, SelectionPanel_DrawIconCellAndAdvanceRegs,
   SelectionPanel_DrawNumberCellAndAdvanceRegs, SelectionPanel_DrawProportionalCappedBar,
   SelectionPanel_DrawSolidCappedBar, SelectionPanel_DrawSegmentedCappedBar, SelectionPanel_DrawForwardCappedBar.
   Cross-module calls: ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs [world/model/runtime],
   GameFactionRuntime_FindRuntimeGroupIndexCf [gameplay/faction/runtime],
   ModelRuntime_QueryHierarchyScaleRatioQ12Regs [world/model/runtime], ArmyRuntime_QueryMetric6CAndDefinitionC4Regs
   [gameplay/army/runtime].
*/
void SelectionPanel_RenderArmyRuntimeMetrics
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiPixelCoordinate panelBottom,
               UiPixelCoordinate panelRight,UiPixelCoordinate panelTop,UiPixelCoordinate panelLeft,
               ArmyRuntimeSlot *armyRuntime)

{
  GameEntityRuntime *pGVar1;
  int *piVar2;
  dword dVar3;
  int iVar4;
  int extraout_ECX;
  uint uVar5;
  uint extraout_ECX_00;
  SelectionPanelSegmentCount totalSegmentCount;
  uint extraout_ECX_01;
  SelectionPanelSegmentCount totalSegmentCount_00;
  uint extraout_ECX_02;
  SelectionPanelSegmentCount totalSegmentCount_01;
  int iVar6;
  int iVar7;
  int *piVar8;
  bool bVar9;
  undefined1 uVar10;
  ModelRuntimeActiveTotalMetricRegisterPair MVar11;
  undefined8 uVar12;
  undefined8 uVar13;
  undefined8 uVar14;
  undefined8 uVar15;
  ModelRuntimeScaleRatioRegisterPairQ12 MVar16;
  
  bVar9 = &stack0xffffffe4 < (undefined1 *)0x8;
  (*g_GraphicsFramebufferBeginAccess)();
  if (bVar9) {
    return;
  }
  pGVar1 = armyRuntime->linkedEntityRuntime;
  piVar8 = armyRuntime->definitionOrAsset;
  if (armyRuntime->factionIndex == *(int *)(extraout_ECX + 0xa80)) {
    if (pGVar1 != (GameEntityRuntime *)0x0) {
      if (pGVar1 == (GameEntityRuntime *)0x1) {
        if ((piVar8[0x2e] == 1) &&
           ((uVar5 = *(uint *)(*piVar8 + 0x4c), uVar10 = uVar5 < 0xd, uVar5 == 0xd ||
            (uVar5 = *(uint *)(*piVar8 + 0x4c), uVar10 = uVar5 < 0xb, uVar5 == 0xb)))) {
          MVar11 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(armyRuntime);
          uVar12 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,
                              (UiNumericValue32)(MVar11 >> 0x20),(UiNumericValue32)MVar11,0x12);
          dVar3 = GameFactionRuntime_FindRuntimeGroupIndexCf(armyRuntime);
          if ((bool)uVar10) {
            uVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            uVar13 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,dVar3,0xb)
            ;
          }
          uVar14 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          uVar15 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,(UiPixelCoordinate)uVar13,
                     (UiPixelCoordinate)uVar12,piVar8[0x1a],piVar8[0x19],0x16);
          MVar16 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(armyRuntime);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,(UiPixelCoordinate)uVar15,
                     (UiPixelCoordinate)uVar14,(UiNumericValue32)(MVar16 >> 0x20),
                     (UiNumericValue32)MVar16,0x19);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,
                     (UiPixelCoordinate)((ulonglong)uVar14 >> 0x20),
                     (UiPixelCoordinate)((ulonglong)uVar12 >> 0x20),panelLeft,5);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,
                     (UiPixelCoordinate)((ulonglong)uVar15 >> 0x20),
                     (UiPixelCoordinate)((ulonglong)uVar13 >> 0x20),panelRight,6);
          goto SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn;
        }
      }
      else if (pGVar1 < (GameEntityRuntime *)0x3) {
        piVar2 = (int *)piVar8[0x50];
        if (((piVar8[3] != 0) && (piVar2 != (int *)0x0)) &&
           ((iVar7 = *piVar2, *(int *)(iVar7 + 0x4c) == 5 ||
            (((*(int *)(iVar7 + 0x4c) == 6 || (*(int *)(iVar7 + 0x4c) == 7)) ||
             (*(int *)(iVar7 + 0x4c) == 8)))))) {
          iVar7 = piVar2[9];
          iVar4 = *(int *)(*piVar2 + 0x30);
          MVar11 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(armyRuntime);
          iVar6 = (int)(MVar11 >> 0x20);
          uVar10 = 0;
          if (iVar6 == 0) {
            uVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
          }
          else {
            uVar12 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,iVar6,
                                (UiNumericValue32)MVar11,0x12);
          }
          dVar3 = GameFactionRuntime_FindRuntimeGroupIndexCf(armyRuntime);
          if ((bool)uVar10) {
            uVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            uVar13 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,dVar3,0xb)
            ;
          }
          uVar14 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          uVar15 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,(UiPixelCoordinate)uVar13,
                     (UiPixelCoordinate)uVar12,iVar4,iVar4 - iVar7,0x16);
          MVar16 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(armyRuntime);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,(UiPixelCoordinate)uVar15,
                     (UiPixelCoordinate)uVar14,(UiNumericValue32)(MVar16 >> 0x20),
                     (UiNumericValue32)MVar16,0x19);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,
                     (UiPixelCoordinate)((ulonglong)uVar14 >> 0x20),
                     (UiPixelCoordinate)((ulonglong)uVar12 >> 0x20),panelLeft,5);
          SelectionPanel_DrawSolidCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,
                     (UiPixelCoordinate)((ulonglong)uVar15 >> 0x20),
                     (UiPixelCoordinate)((ulonglong)uVar13 >> 0x20),panelRight,6);
          goto SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn;
        }
      }
      else if (pGVar1 == (GameEntityRuntime *)0x3) {
        piVar2 = (int *)piVar8[0x50];
        if ((piVar8[3] != 0) && (piVar2 != (int *)0x0)) {
          piVar8 = (int *)*piVar2;
          uVar5 = 0xffffffff;
          iVar7 = 7;
          if (piVar8[0x13] == 9) {
            do {
              if ((uint)piVar2[iVar7 + 0x18] < uVar5) {
                uVar5 = piVar2[iVar7 + 0x18];
              }
              iVar7 = iVar7 + -1;
            } while (-1 < iVar7);
            if ((int)uVar5 < piVar2[0x20]) {
              uVar5 = piVar2[0x20];
            }
            iVar4 = 0;
            iVar7 = piVar8[0xc];
            if (0 < (int)uVar5) {
              iVar4 = -uVar5;
            }
            MVar11 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(armyRuntime);
            iVar6 = (int)(MVar11 >> 0x20);
            uVar10 = 0;
            if (iVar6 == 0) {
              uVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
            }
            else {
              uVar12 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,iVar6,
                                  (UiNumericValue32)MVar11,0x12);
            }
            dVar3 = GameFactionRuntime_FindRuntimeGroupIndexCf(armyRuntime);
            if ((bool)uVar10) {
              uVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
            }
            else {
              uVar13 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                                 (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,dVar3,
                                  0xb);
            }
            uVar14 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
            uVar15 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
            SelectionPanel_DrawProportionalCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,panelTop,(UiPixelCoordinate)uVar13,
                       (UiPixelCoordinate)uVar12,iVar7,iVar4 + iVar7,0x16);
            MVar16 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(armyRuntime);
            SelectionPanel_DrawProportionalCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,panelBottom,(UiPixelCoordinate)uVar15,
                       (UiPixelCoordinate)uVar14,(UiNumericValue32)(MVar16 >> 0x20),
                       (UiNumericValue32)MVar16,0x19);
            SelectionPanel_DrawSolidCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,
                       (UiPixelCoordinate)((ulonglong)uVar14 >> 0x20),
                       (UiPixelCoordinate)((ulonglong)uVar12 >> 0x20),panelLeft,5);
            SelectionPanel_DrawSolidCappedBar
                      (clipTop,clipLeft,clipBottom,clipRight,
                       (UiPixelCoordinate)((ulonglong)uVar15 >> 0x20),
                       (UiPixelCoordinate)((ulonglong)uVar13 >> 0x20),panelRight,6);
            goto SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn;
          }
        }
      }
      else if (((GameEntityRuntime *)0x3 < pGVar1) && (*(int *)(*piVar8 + 0x4c) == 0x16)) {
        uVar10 = piVar8[0x2b] == 0;
        if (piVar8[0x2b] == 1) {
          MVar11 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(armyRuntime);
          uVar12 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,
                              (UiNumericValue32)(MVar11 >> 0x20),(UiNumericValue32)MVar11,0x12);
          dVar3 = GameFactionRuntime_FindRuntimeGroupIndexCf(armyRuntime);
          if ((bool)uVar10) {
            uVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            uVar13 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,dVar3,0xb)
            ;
          }
          uVar14 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          uVar15 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,(UiPixelCoordinate)uVar13,
                     (UiPixelCoordinate)uVar12,piVar8[0x1a],piVar8[0x19],0x16);
          MVar16 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(armyRuntime);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,(UiPixelCoordinate)uVar15,
                     (UiPixelCoordinate)uVar14,(UiNumericValue32)(MVar16 >> 0x20),
                     (UiNumericValue32)MVar16,0x19);
          ArmyRuntime_QueryMetric6CAndDefinitionC4Regs();
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,
                     (UiPixelCoordinate)((ulonglong)uVar14 >> 0x20),
                     (UiPixelCoordinate)((ulonglong)uVar12 >> 0x20),panelLeft,
                     extraout_ECX_00 - (extraout_ECX_00 >> 1),(int)pGVar1 - ((uint)pGVar1 >> 1),0xf)
          ;
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,
                     (UiPixelCoordinate)((ulonglong)uVar15 >> 0x20),
                     (UiPixelCoordinate)((ulonglong)uVar13 >> 0x20),panelRight,totalSegmentCount,
                     (uint)pGVar1 >> 1,0x10);
        }
        else if ((piVar8[0x3b] & 0xc0U) == 0) {
          MVar11 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(armyRuntime);
          iVar7 = (int)(MVar11 >> 0x20);
          uVar10 = 0;
          if (iVar7 == 0) {
            uVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
          }
          else {
            uVar12 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,iVar7,
                                (UiNumericValue32)MVar11,0x12);
          }
          dVar3 = GameFactionRuntime_FindRuntimeGroupIndexCf(armyRuntime);
          if ((bool)uVar10) {
            uVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            uVar13 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,dVar3,0xb)
            ;
          }
          uVar14 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          uVar15 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawForwardCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,(UiPixelCoordinate)uVar13,
                     (UiPixelCoordinate)uVar12,4);
          MVar16 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(armyRuntime);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,(UiPixelCoordinate)uVar15,
                     (UiPixelCoordinate)uVar14,(UiNumericValue32)(MVar16 >> 0x20),
                     (UiNumericValue32)MVar16,0x19);
          ArmyRuntime_QueryMetric6CAndDefinitionC4Regs();
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,
                     (UiPixelCoordinate)((ulonglong)uVar14 >> 0x20),
                     (UiPixelCoordinate)((ulonglong)uVar12 >> 0x20),panelLeft,
                     extraout_ECX_01 - (extraout_ECX_01 >> 1),(int)pGVar1 - ((uint)pGVar1 >> 1),0xf)
          ;
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,
                     (UiPixelCoordinate)((ulonglong)uVar15 >> 0x20),
                     (UiPixelCoordinate)((ulonglong)uVar13 >> 0x20),panelRight,totalSegmentCount_00,
                     (uint)pGVar1 >> 1,0x10);
        }
        else {
          MVar11 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(armyRuntime);
          iVar7 = (int)(MVar11 >> 0x20);
          uVar10 = 0;
          if (iVar7 == 0) {
            uVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
          }
          else {
            uVar12 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,iVar7,
                                (UiNumericValue32)MVar11,0x12);
          }
          dVar3 = GameFactionRuntime_FindRuntimeGroupIndexCf(armyRuntime);
          if ((bool)uVar10) {
            uVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
          }
          else {
            uVar13 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                               (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,dVar3,0xb)
            ;
          }
          uVar14 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
          uVar15 = SelectionPanel_DrawIconCellAndAdvanceRegs
                             (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelTop,(UiPixelCoordinate)uVar13,
                     (UiPixelCoordinate)uVar12,piVar8[0x41],piVar8[0x42],0x16);
          MVar16 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(armyRuntime);
          SelectionPanel_DrawProportionalCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,panelBottom,(UiPixelCoordinate)uVar15,
                     (UiPixelCoordinate)uVar14,(UiNumericValue32)(MVar16 >> 0x20),
                     (UiNumericValue32)MVar16,0x19);
          ArmyRuntime_QueryMetric6CAndDefinitionC4Regs();
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,
                     (UiPixelCoordinate)((ulonglong)uVar14 >> 0x20),
                     (UiPixelCoordinate)((ulonglong)uVar12 >> 0x20),panelLeft,
                     extraout_ECX_02 - (extraout_ECX_02 >> 1),(int)pGVar1 - ((uint)pGVar1 >> 1),0xf)
          ;
          SelectionPanel_DrawSegmentedCappedBar
                    (clipTop,clipLeft,clipBottom,clipRight,
                     (UiPixelCoordinate)((ulonglong)uVar15 >> 0x20),
                     (UiPixelCoordinate)((ulonglong)uVar13 >> 0x20),panelRight,totalSegmentCount_01,
                     (uint)pGVar1 >> 1,0x10);
        }
        goto SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn;
      }
    }
    if ((piVar8[0x3b] & 0xc0U) == 0) {
      MVar11 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(armyRuntime);
      iVar7 = (int)(MVar11 >> 0x20);
      uVar10 = 0;
      if (iVar7 == 0) {
        uVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
      }
      else {
        uVar12 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,iVar7,
                            (UiNumericValue32)MVar11,0x12);
      }
      dVar3 = GameFactionRuntime_FindRuntimeGroupIndexCf(armyRuntime);
      if ((bool)uVar10) {
        uVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
      }
      else {
        uVar13 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,dVar3,0xb);
      }
      uVar14 = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
      uVar15 = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
      SelectionPanel_DrawForwardCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelTop,(UiPixelCoordinate)uVar13,
                 (UiPixelCoordinate)uVar12,4);
      MVar16 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(armyRuntime);
      SelectionPanel_DrawProportionalCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelBottom,(UiPixelCoordinate)uVar15,
                 (UiPixelCoordinate)uVar14,(UiNumericValue32)(MVar16 >> 0x20),
                 (UiNumericValue32)MVar16,0x19);
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,
                 (UiPixelCoordinate)((ulonglong)uVar14 >> 0x20),
                 (UiPixelCoordinate)((ulonglong)uVar12 >> 0x20),panelLeft,5);
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,
                 (UiPixelCoordinate)((ulonglong)uVar15 >> 0x20),
                 (UiPixelCoordinate)((ulonglong)uVar13 >> 0x20),panelRight,6);
    }
    else {
      MVar11 = ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(armyRuntime);
      iVar7 = (int)(MVar11 >> 0x20);
      uVar10 = 0;
      if (iVar7 == 0) {
        uVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
      }
      else {
        uVar12 = SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,iVar7,
                            (UiNumericValue32)MVar11,0x12);
      }
      dVar3 = GameFactionRuntime_FindRuntimeGroupIndexCf(armyRuntime);
      if ((bool)uVar10) {
        uVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
      }
      else {
        uVar13 = SelectionPanel_DrawNumberCellAndAdvanceRegs
                           (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,dVar3,0xb);
      }
      uVar14 = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
      uVar15 = SelectionPanel_DrawIconCellAndAdvanceRegs
                         (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
      SelectionPanel_DrawProportionalCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelTop,(UiPixelCoordinate)uVar13,
                 (UiPixelCoordinate)uVar12,piVar8[0x41],piVar8[0x42],0x16);
      MVar16 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(armyRuntime);
      SelectionPanel_DrawProportionalCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,panelBottom,(UiPixelCoordinate)uVar15,
                 (UiPixelCoordinate)uVar14,(UiNumericValue32)(MVar16 >> 0x20),
                 (UiNumericValue32)MVar16,0x19);
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,
                 (UiPixelCoordinate)((ulonglong)uVar14 >> 0x20),
                 (UiPixelCoordinate)((ulonglong)uVar12 >> 0x20),panelLeft,5);
      SelectionPanel_DrawSolidCappedBar
                (clipTop,clipLeft,clipBottom,clipRight,
                 (UiPixelCoordinate)((ulonglong)uVar15 >> 0x20),
                 (UiPixelCoordinate)((ulonglong)uVar13 >> 0x20),panelRight,6);
    }
  }
  else {
    uVar12 = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelLeft,0);
    uVar13 = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelTop,panelRight,1);
    uVar14 = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelLeft,2);
    uVar15 = SelectionPanel_DrawIconCellAndAdvanceRegs
                       (clipTop,clipLeft,clipBottom,clipRight,panelBottom,panelRight,3);
    SelectionPanel_DrawForwardCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,panelTop,(UiPixelCoordinate)uVar13,
               (UiPixelCoordinate)uVar12,4);
    MVar16 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs(armyRuntime);
    SelectionPanel_DrawProportionalCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,panelBottom,(UiPixelCoordinate)uVar15,
               (UiPixelCoordinate)uVar14,(UiNumericValue32)(MVar16 >> 0x20),(UiNumericValue32)MVar16
               ,0x19);
    SelectionPanel_DrawSolidCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,(UiPixelCoordinate)((ulonglong)uVar14 >> 0x20),
               (UiPixelCoordinate)((ulonglong)uVar12 >> 0x20),panelLeft,5);
    SelectionPanel_DrawSolidCappedBar
              (clipTop,clipLeft,clipBottom,clipRight,(UiPixelCoordinate)((ulonglong)uVar15 >> 0x20),
               (UiPixelCoordinate)((ulonglong)uVar13 >> 0x20),panelRight,6);
  }
SelectionPanel_RenderArmyRuntimeMetrics_EndFramebufferAccessAndReturn:
  (*g_GraphicsFramebufferEndAccess)();
  return;
}

/* Address: 0x0055FA20.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles in game selection rebuild owned class16 selection.
   Local calls: SelectionPointerArray_Clear32, SelectionPointerArray_InsertUniqueAndRecenter.
   Cross-module calls: InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime], UiCatalogGroup48_RebuildGrid
   [ui/ingame/technology].
*/
undefined8 __fastcall
InGameSelection_RebuildOwnedClass16Selection(undefined4 param_1,undefined4 param_2,int param_3)

{
  GameEntityRuntime *entityRuntime;
  int iVar1;
  SelectionPlayerRuntimeBlock *selection;
  undefined4 in_EAX;
  int iVar2;
  
  iVar2 = SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[param_3]->selection);
  for (iVar2 = *(int *)(iVar2 + 0xb08); iVar2 != 0; iVar2 = *(int *)(iVar2 + 4)) {
    if (*(int *)(iVar2 + 0xa4) == 0) {
      entityRuntime = (GameEntityRuntime *)(*(int **)(iVar2 + 0x48))[2];
      iVar1 = **(int **)(iVar2 + 0x48);
      selection = g_SelectionPlayerRuntimeBlockPointers[param_3];
      if ((*(int *)(iVar1 + 0x4c) == 0x16) &&
         ((entityRuntime->common).ownership.ownerIndex == selection->primaryEntityOrFactionToken8080
         )) {
        SelectionPointerArray_InsertUniqueAndRecenter
                  (iVar1,selection,entityRuntime,&selection->selection);
      }
    }
  }
  if (param_3 == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
  return CONCAT44(param_2,in_EAX);
}

/* Address: 0x0055FB30.
   Ownership: gameplay/selection/runtime.
   Purpose: Replaces one player selection with the army runtime identified by the rebased index and refreshes local
   UI when applicable. Kept distinct from frontend slot indices, faction runtime indices, network endpoint
   identity, and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention, storage,
   body bytes, control flow, and executable data remain unchanged. Typed parameters: p3
   armyRuntimeIndex→RuntimeToken.
   Local calls: SelectionPointerArray_Clear32, SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity.
   Cross-module calls: InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime], UiCatalogGroup48_RebuildGrid
   [ui/ingame/technology].
*/
void InGamePlayerSelection_ReplaceWithArmyRuntimeIndex
               (PlayerRuntimeId playerId,dword payloadDword04,dword payloadDword08,
               RuntimeToken armyRuntimeIndex)

{
  ArmyRuntimeSlot *sourceArmyRuntime;
  
  if (armyRuntimeIndex != 0) {
    sourceArmyRuntime =
         (ArmyRuntimeSlot *)
         SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
    if (sourceArmyRuntime->modelNodeRuntime != (ModelRuntimeNode *)0x0) {
      SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity
                (sourceArmyRuntime,&g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
      if (playerId == g_LocalPlayerRuntimeId) {
        InGameSelectionDetailPanel_Rebuild();
        UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
      }
    }
  }
  return;
}

/* Address: 0x0055FE70.
   Ownership: gameplay/selection/runtime.
   Purpose: Forwards command payload coordinates to position-command variant B using the player selection array.
   Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK asset
   identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention, storage, body bytes, control
   flow, and executable data remain unchanged. Typed parameters: p2 payloadDword08→CommandPayloadDword08_V343, p3
   payloadDword0C→CommandPayloadDword0C_V343.
   Local calls: SelectionPointerArray_ApplyPositionCommandVariantB.
*/
void InGamePlayerSelection_ApplyPositionCommandVariantB
               (PlayerRuntimeId playerId,dword payloadDword04,CommandPayloadDword08 payloadDword08,
               CommandPayloadDword0C payloadDword0C)

{
  SelectionPointerArray_ApplyPositionCommandVariantB
            (payloadDword08,payloadDword0C,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}

/* Address: 0x0055FEA0.
   Ownership: gameplay/selection/runtime.
   Purpose: Forwards command payload coordinates to SelectionPointerArray_ApplyPositionCommand for one player
   selection array. Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity,
   and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention, storage, body
   bytes, control flow, and executable data remain unchanged. Typed parameters: p2
   payloadDword08→CommandPayloadDword08_V343, p3 payloadDword0C→CommandPayloadDword0C_V343.
   Local calls: SelectionPointerArray_ApplyPositionCommand.
*/
void InGamePlayerSelection_ApplyPositionCommand
               (PlayerRuntimeId playerId,dword payloadDword04,CommandPayloadDword08 payloadDword08,
               CommandPayloadDword0C payloadDword0C)

{
  SelectionPointerArray_ApplyPositionCommand
            (payloadDword08,payloadDword0C,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}

/* Address: 0x0055FED0.
   Ownership: gameplay/selection/runtime.
   Purpose: Selects the army runtime identified by the rebased index for one player when the runtime is eligible.
   Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK asset
   identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention, storage, body bytes, control
   flow, and executable data remain unchanged. Typed parameters: p3 armyRuntimeIndex→RuntimeToken.
   Local calls: SelectionPointerArray_ApplyArmyRuntimeTarget.
*/
void InGamePlayerSelection_SelectArmyRuntimeIndex
               (PlayerRuntimeId playerId,dword payloadDword04,dword payloadDword08,
               RuntimeToken armyRuntimeIndex)

{
  if ((armyRuntimeIndex != 0) &&
     (((ArmyRuntimeSlot *)(armyRuntimeIndex + (int)g_ArmyRuntimeRebaseBaseMinusOne))->
      modelNodeRuntime != (ModelRuntimeNode *)0x0)) {
    SelectionPointerArray_ApplyArmyRuntimeTarget
              ((ArmyRuntimeSlot *)(armyRuntimeIndex + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  }
  return;
}

/* Address: 0x0055FF10.
   Ownership: gameplay/selection/runtime.
   Purpose: Forwards a four-dword in-game command to the target-position helper using one player selection array.
   Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK asset
   identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention, storage, body bytes, control
   flow, and executable data remain unchanged. Typed parameters: p1 payloadDword04→CommandPayloadDword04_V343, p2
   payloadDword08→CommandPayloadDword08_V343, p3 payloadDword0C→CommandPayloadDword0C_V343.
   Local calls: SelectionPointerArray_ApplyTargetPositionCommand.
*/
void InGamePlayerSelection_ApplyTargetPositionCommand
               (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
               CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C)

{
  SelectionPointerArray_ApplyTargetPositionCommand
            (payloadDword04,payloadDword08,payloadDword0C,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}

/* Address: 0x0055FF40.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles player selection reset movement prune and recenter entries.
   Local calls: SelectionRuntime_ResetMovementPruneAndRecenterEntries.
*/
void PlayerSelection_ResetMovementPruneAndRecenterEntries
               (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
               CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C)

{
  SelectionRuntime_ResetMovementPruneAndRecenterEntries
            ((GameEntityRuntime **)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}

/* Address: 0x0055FF60.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles player selection reset movement anchors and clear flag200 for eligible entries.
   Local calls: SelectionRuntime_ResetMovementAnchorsAndClearFlag200ForEligibleEntries.
*/
void PlayerSelection_ResetMovementAnchorsAndClearFlag200ForEligibleEntries
               (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
               CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C)

{
  SelectionRuntime_ResetMovementAnchorsAndClearFlag200ForEligibleEntries
            ((int *)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}

/* Address: 0x0055FF80.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles player selection interrupt targets and clear flag10 for eligible entries.
   Local calls: SelectionRuntime_InterruptTargetsAndClearFlag10ForEligibleEntries.
*/
void PlayerSelection_InterruptTargetsAndClearFlag10ForEligibleEntries
               (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
               CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C)

{
  SelectionRuntime_InterruptTargetsAndClearFlag10ForEligibleEntries
            ((undefined4 *)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}

/* Address: 0x0055FFA0.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles player selection apply flags418 unless bit8 to eligible entries.
   Local calls: SelectionRuntime_ApplyFlags418UnlessBit8ToEligibleEntries.
*/
void PlayerSelection_ApplyFlags418UnlessBit8ToEligibleEntries
               (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
               CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C)

{
  SelectionRuntime_ApplyFlags418UnlessBit8ToEligibleEntries
            ((undefined4 *)g_SelectionPlayerRuntimeBlockPointers[playerId]);
  return;
}

/* Address: 0x0055FFC0.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles in game selection apply type16 marker coordinates variant1.
   Local calls: SelectionPointerArray_ApplyType16MarkerCoordinates.
*/
undefined4
InGameSelection_ApplyType16MarkerCoordinatesVariant1
          (int param_1,SelectionMarkerCoordinateValue32 param_2,
          SelectionMarkerCoordinateValue32 param_3,SelectionMarkerCoordinateValue32 param_4)

{
  undefined4 in_EAX;
  
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (1,param_2,param_3,param_4,&g_SelectionPlayerRuntimeBlockPointers[param_1]->selection);
  return in_EAX;
}

/* Address: 0x0055FFF0.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles in game selection apply type16 marker coordinates variant2.
   Local calls: SelectionPointerArray_ApplyType16MarkerCoordinates.
*/
undefined4
InGameSelection_ApplyType16MarkerCoordinatesVariant2
          (int param_1,SelectionMarkerCoordinateValue32 param_2,
          SelectionMarkerCoordinateValue32 param_3,SelectionMarkerCoordinateValue32 param_4)

{
  undefined4 in_EAX;
  
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (2,param_2,param_3,param_4,&g_SelectionPlayerRuntimeBlockPointers[param_1]->selection);
  return in_EAX;
}

/* Address: 0x00562050.
   Ownership: gameplay/selection/runtime.
   Purpose: Resolves the selected player primary entity, reapplies its current position to the entity and command-
   state coordinate fields, invokes the type-specific position callback, and refreshes dependent selection state.
   Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK asset
   identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling convention, storage, body bytes,
   control flow, and executable data remain unchanged.
   Local calls: SelectionPointerArray_ContainsCf.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ModelNodeRuntime_UpdateDepthBinMasks [world/model/hierarchy].
*/
void __fastcall
SelectionPlayerRuntime_ReissuePrimarySelectionPosition
          (undefined4 param_1,undefined4 param_2,PlayerRuntimeId playerRuntimeId)

{
  dword dVar1;
  ModelRuntimeNode *modelNode;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 *extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  int extraout_EDX;
  int worldXQ12;
  undefined4 extraout_EDX_00;
  WorldRuntimeContext *worldRuntime;
  undefined8 uVar5;
  
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  dVar1 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->primarySelectionEntityOffset8094;
  if (dVar1 != 0) {
    iVar4 = SelectionPointerArray_ContainsCf
                      ((GameEntityRuntime *)((int)g_ArmyRuntimeRebaseBaseMinusOne + dVar1),
                       &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    modelNode = (ModelRuntimeNode *)extraout_ECX[1];
    iVar4 = iVar4 + (modelNode->worldTransform).translation.x;
    worldXQ12 = extraout_EDX + (modelNode->worldTransform).translation.y;
    iVar2 = *(int *)*extraout_ECX;
    extraout_ECX[0x2e] = iVar4;
    extraout_ECX[0x2f] = worldXQ12;
    extraout_ECX[0x1e] = iVar4;
    extraout_ECX[0x1f] = worldXQ12;
    extraout_ECX[0x16] = iVar4;
    extraout_ECX[0x17] = worldXQ12;
    (modelNode->worldTransform).translation.x = iVar4;
    iVar3 = *(int *)(iVar2 + 0x278);
    (modelNode->worldTransform).translation.y = worldXQ12;
    if (iVar3 == 3) {
      ((modelNode->runtimePayload).armyRuntime)->movementTarget0Q12 = 0x7fffffff;
    }
    (*g_ArmyPlacementContactKindDispatchTable.callbacks[iVar3])
              (*(Q12 *)(iVar2 + 0x54),worldXQ12,iVar4,modelNode,worldRuntime);
    uVar5 = ModelNodeRuntime_RebuildTransformsFromRoot(extraout_ECX_00,extraout_EDX_00,modelNode);
    ModelNodeRuntime_UpdateDepthBinMasks
              (extraout_ECX_01,(int)((ulonglong)uVar5 >> 0x20),
               *(DepthIntervalRadius32 *)(iVar2 + 0xdc),modelNode);
  }
  return;
}

/* Address: 0x00562220.
   Ownership: gameplay/selection/runtime.
   Purpose: Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK
   asset identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling convention, storage, body
   bytes, control flow, and executable data remain unchanged.
   Local calls: SelectionPointerArray_ContainsCf.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/
void __fastcall
SelectionPlayerRuntime_AdvancePrimarySelectionCycle
          (undefined4 param_1,undefined4 param_2,PlayerRuntimeId playerRuntimeId)

{
  dword dVar1;
  ModelRuntimeNode *modelNodeRuntime;
  int iVar2;
  int extraout_ECX;
  undefined4 extraout_EDX;
  
  dVar1 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->primarySelectionEntityOffset8094;
  if (dVar1 != 0) {
    iVar2 = SelectionPointerArray_ContainsCf
                      ((GameEntityRuntime *)((int)g_ArmyRuntimeRebaseBaseMinusOne + dVar1),
                       &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    modelNodeRuntime = *(ModelRuntimeNode **)(extraout_ECX + 4);
    (modelNodeRuntime->modelPayload).worldRotationAngle2 =
         iVar2 + (modelNodeRuntime->modelPayload).worldRotationAngle2 & 0xffff;
    ModelNodeRuntime_RebuildTransformsFromRoot(extraout_ECX,extraout_EDX,modelNodeRuntime);
  }
  return;
}

/* Address: 0x0052CEE0.
   Ownership: gameplay/selection/runtime.
   Purpose: Loads gfx\panel\select.gfx and info.gfx plus their 0x1A4-byte .dat tables, clears eight fixed runtime
   blocks, stores the caller's entity-slot table, and patches verified texture-source sequence descriptors used by
   the selection and information panels. The .dat record semantics remain opaque.
   Cross-module calls: Package_LoadEntry [assets/package/runtime].
*/
void SelectionInfoPanel_InitResources(SelectionInfoEntitySlots *entitySlots)

{
  byte *pbVar1;
  undefined4 uVar2;
  AssetRelativeOffset AVar3;
  GraphicsTextureSourceAsset *pGVar4;
  void *pvVar5;
  dword in_ECX;
  dword arg0;
  int iVar6;
  dword in_EDX;
  dword arg1;
  int iVar7;
  SelectionPlayerRuntimeBlock *pSVar8;
  undefined1 in_CF;
  
  pGVar4 = (*g_GraphicsTextureSourceLoadPackageAsset)
                     (in_ECX,in_EDX,(word *)u_gfx_panel_select_gfx_0052ce18);
  if (!(bool)in_CF) {
    g_SelectionPanelTextureSource = pGVar4;
    pGVar4 = (*g_GraphicsTextureSourceLoadPackageAsset)
                       (arg0,arg1,(word *)u_gfx_panel_info_gfx_0052ce42);
    if (!(bool)in_CF) {
      g_InfoPanelTextureSource = pGVar4;
      pvVar5 = Package_LoadEntry((word *)u_gfx_panel_select_dat_0052ce68);
      if (!(bool)in_CF) {
        g_SelectionPanelData = pvVar5;
        pvVar5 = Package_LoadEntry((word *)u_gfx_panel_info_dat_0052ce92);
        if (!(bool)in_CF) {
          iVar7 = 8;
          pSVar8 = g_SelectionPlayerBlocks;
          g_InfoPanelData = pvVar5;
          do {
            for (iVar6 = 0x20; pGVar4 = g_SelectionPanelTextureSource, iVar6 != 0;
                iVar6 = iVar6 + -1) {
              (pSVar8->selection).entries[0] = (GameEntityRuntime *)0x0;
              pSVar8 = (SelectionPlayerRuntimeBlock *)((pSVar8->selection).entries + 1);
            }
            pSVar8 = (SelectionPlayerRuntimeBlock *)&pSVar8->pendingSelectionEntityOffset8098;
            iVar7 = iVar7 + -1;
          } while (iVar7 != 0);
          g_SelectionInfoEntitySlots = entitySlots;
          AVar3 = (g_SelectionPanelTextureSource->tableDescriptor).subresourceTableOffset;
          LOCK();
          uVar2 = *(undefined4 *)
                   (g_SelectionPanelTextureSource[2].opaqueTablePayloadBC_1FF + AVar3 + 0x110);
          *(undefined4 *)(g_SelectionPanelTextureSource[2].opaqueTablePayloadBC_1FF + AVar3 + 0x110)
               = *(undefined4 *)
                  (g_SelectionPanelTextureSource[2].opaqueTablePayloadBC_1FF + AVar3 + 0xf0);
          UNLOCK();
          *(undefined4 *)(pGVar4[2].opaqueTablePayloadBC_1FF + AVar3 + 0xf0) = uVar2;
          pGVar4 = g_InfoPanelTextureSource;
          AVar3 = (g_InfoPanelTextureSource->tableDescriptor).subresourceTableOffset;
          uVar2 = *(undefined4 *)
                   ((g_InfoPanelTextureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                   *(int *)(g_InfoPanelTextureSource[2].opaqueTablePayloadBC_1FF + AVar3 + 0xd0) +
                   -0x28);
          iVar7 = *(int *)(g_InfoPanelTextureSource[2].opaqueTablePayloadBC_1FF + AVar3 + 0x130);
          pbVar1 = (g_InfoPanelTextureSource->common).buildMetadata.assetRelativeAddressAnchor28 +
                   iVar7 + -0x28;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(undefined4 *)
           ((pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x24) = uVar2;
          iVar7 = *(int *)(pGVar4[3].common.buildMetadata.assetRelativeAddressAnchor28 +
                          (AVar3 - 0x1c));
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x28;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(undefined4 *)
           ((pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x24) = uVar2;
          iVar7 = *(int *)(pGVar4[3].common.buildMetadata.assetRelativeAddressAnchor28 + AVar3 + 4);
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x28;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(undefined4 *)
           ((pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x24) = uVar2;
          *(undefined4 *)((int)pGVar4[3].common.buildMetadata.names.producerName + AVar3 + 0x2c) = 4
          ;
          *(undefined4 *)((int)pGVar4[3].common.buildMetadata.names.producerName + AVar3 + 0x14) = 4
          ;
          *(undefined4 *)((int)pGVar4[3].common.buildMetadata.names.sourceName + AVar3 + 0xc) = 4;
          *(undefined4 *)((int)pGVar4[3].common.buildMetadata.names.producerName + AVar3 + 0x34) = 4
          ;
          pbVar1 = pGVar4[2].opaqueTablePayloadBC_1FF + AVar3 + 0x100;
          pbVar1[0] = 4;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          pbVar1 = pGVar4[2].opaqueTablePayloadBC_1FF + AVar3 + 0xe8;
          pbVar1[0] = 4;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          pbVar1 = pGVar4[2].opaqueTablePayloadBC_1FF + AVar3 + 0x120;
          pbVar1[0] = 4;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          pbVar1 = pGVar4[2].opaqueTablePayloadBC_1FF + AVar3 + 0x108;
          pbVar1[0] = 4;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          iVar7 = *(int *)((int)pGVar4[3].common.buildMetadata.names.producerName + AVar3 + 0x1c);
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x28;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(undefined4 *)
           ((pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x24) = uVar2;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x20;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x1c;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x18;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x14;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x10;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(undefined4 *)
           ((pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0xc) = uVar2;
          iVar7 = *(int *)((int)pGVar4[3].common.buildMetadata.names.producerName + AVar3 + 0x3c);
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x28;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(undefined4 *)
           ((pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x24) = uVar2;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x20;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x1c;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x18;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x14;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x10;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          *(undefined4 *)
           ((pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0xc) = uVar2;
          iVar7 = *(int *)(pGVar4[2].opaqueTablePayloadBC_1FF + AVar3 + 0xf0);
          *(undefined4 *)
           ((pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x28) = uVar2;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x24;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x20;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x1c;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x18;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x14;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          *(undefined4 *)
           ((pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x10) = uVar2;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0xc;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          iVar7 = *(int *)(pGVar4[2].opaqueTablePayloadBC_1FF + AVar3 + 0x110);
          *(undefined4 *)
           ((pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x28) = uVar2;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x24;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x20;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x1c;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x18;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x14;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0xff;
          pbVar1[3] = 0xff;
          *(undefined4 *)
           ((pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0x10) = uVar2;
          pbVar1 = (pGVar4->common).buildMetadata.assetRelativeAddressAnchor28 + iVar7 + -0xc;
          pbVar1[0] = 0;
          pbVar1[1] = 0;
          pbVar1[2] = 0;
          pbVar1[3] = 0;
          return;
        }
      }
    }
  }
  return;
}

/* Address: 0x0052D0F0.
   Ownership: gameplay/selection/runtime.
   Purpose: Releases both panel texture sources and both package-loaded .dat tables, then clears the four global
   resource pointers.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void SelectionInfoPanel_ShutdownResources(void)

{
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_SelectionPanelTextureSource);
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_InfoPanelTextureSource);
  Resource_Release(g_SelectionPanelData);
  Resource_Release(g_InfoPanelData);
  g_SelectionPanelTextureSource = (GraphicsTextureSourceAsset *)0x0;
  g_InfoPanelTextureSource = (GraphicsTextureSourceAsset *)0x0;
  g_SelectionPanelData = (void *)0x0;
  g_InfoPanelData = (void *)0x0;
  return;
}

/* Address: 0x0052FB20.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the first 32 pointers in each of eight exact 0x8118-byte player blocks and clears every entry
   equal to target. All incoming general registers are restored.
*/
void SelectionPlayerBlocks_RemovePointer(GameEntityRuntime *target)

{
  int entriesRemainingInBlock;
  int playerBlocksRemaining;
  SelectionPlayerRuntimeBlock *currentSelectionEntry;
  SelectionPlayerRuntimeBlock *selectionEntryCursor;
  
  playerBlocksRemaining = 8;
  entriesRemainingInBlock = 0x20;
  selectionEntryCursor = g_SelectionPlayerBlocks;
  do {
    do {
      currentSelectionEntry = selectionEntryCursor;
      if (target == (currentSelectionEntry->selection).entries[0]) {
        (currentSelectionEntry->selection).entries[0] = (GameEntityRuntime *)0x0;
      }
      entriesRemainingInBlock = entriesRemainingInBlock + -1;
      selectionEntryCursor =
           (SelectionPlayerRuntimeBlock *)((currentSelectionEntry->selection).entries + 1);
    } while (entriesRemainingInBlock != 0);
    entriesRemainingInBlock = 0x20;
    playerBlocksRemaining = playerBlocksRemaining + -1;
    selectionEntryCursor =
         (SelectionPlayerRuntimeBlock *)&currentSelectionEntry->packedSelectionState809C;
  } while (playerBlocksRemaining != 0);
  return;
}

/* Address: 0x0052FB70.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the 32 global selection-info entity slots, sums entity position fields +0x94/+0x98/+0x9C, and
   returns their signed averages in EAX/ECX/EDX. CF is set when no slot is populated and clear on success.
*/
undefined8 __cdecl SelectionInfoEntitySlots_ComputeAverageWorldPositionRegsCf(void)

{
  int worldXAggregateQ12;
  int worldZAggregateQ12;
  GameEntityRuntime **selectionEntitySlotCursor;
  int selectedEntityCount;
  int selectionSlotsRemaining;
  ModelRuntimeNode *selectedModelNode;
  
  worldXAggregateQ12 = 0;
  worldZAggregateQ12 = 0;
  selectionSlotsRemaining = 0x20;
  selectedEntityCount = 0;
  selectionEntitySlotCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntitySlotCursor != (GameEntityRuntime *)0x0) {
      selectedModelNode = ((*selectionEntitySlotCursor)->common).ownership.modelNode;
      worldXAggregateQ12 = worldXAggregateQ12 + (selectedModelNode->worldTransform).translation.x;
      worldZAggregateQ12 = worldZAggregateQ12 + (selectedModelNode->worldTransform).translation.z;
      selectedEntityCount = selectedEntityCount + 1;
    }
    selectionEntitySlotCursor = selectionEntitySlotCursor + 1;
    selectionSlotsRemaining = selectionSlotsRemaining + -1;
  } while (selectionSlotsRemaining != 0);
  if (selectedEntityCount != 0) {
    worldXAggregateQ12 = worldXAggregateQ12 / selectedEntityCount;
    worldZAggregateQ12 = worldZAggregateQ12 / selectedEntityCount;
  }
  return CONCAT44(worldZAggregateQ12,worldXAggregateQ12);
}

/* Address: 0x0052FD60.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans one exact 32-entry pointer array and clears only the first entry equal to target. EAX is
   preserved.
*/
undefined4
SelectionPointerArray_RemoveFirstMatch(GameEntityRuntime *target,SelectionPointerArray32 *array)

{
  undefined4 in_EAX;
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  undefined1 in_ZF;
  
  entriesRemaining = 0x20;
  do {
    selectionEntryCursor = array->entries;
    if (entriesRemaining == 0) break;
    entriesRemaining = entriesRemaining + -1;
    selectionEntryCursor = (GameEntityRuntime **)((int)array + 4);
    in_ZF = target == array->entries[0];
    array = (SelectionPointerArray32 *)selectionEntryCursor;
  } while (!(bool)in_ZF);
  if ((bool)in_ZF) {
    selectionEntryCursor[-1] = (GameEntityRuntime *)0x0;
  }
  return in_EAX;
}

/* Address: 0x0052FDC0.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the global exact 32-entry selection array. CF set means at least one entry is non-null; CF clear
   means all entries are null. EAX is restored before return.
*/
void __cdecl SelectionInfo_HasAnyEntryCf(void)

{
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  GameEntityRuntime *currentEntry;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (entriesRemaining == 0) {
      return;
    }
    entriesRemaining = entriesRemaining + -1;
    currentEntry = *selectionEntryCursor;
    selectionEntryCursor = selectionEntryCursor + 1;
  } while (currentEntry == (GameEntityRuntime *)0x0);
  return;
}

/* Address: 0x0052FDE0.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the global 32-entry selection array. Null entries are accepted; every non-null entry must have
   owner/index dword +0x0C equal to ownerIndex. CF clear means all entries satisfy the condition, and CF set means
   the first mismatch. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or
   codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
*/
undefined8 SelectionInfo_AllEntriesEmptyOrMatchOwnerCf(FactionRuntimeIndex ownerIndex)

{
  undefined4 in_EAX;
  int entriesRemaining;
  undefined4 in_EDX;
  GameEntityRuntime **selectionEntryCursor;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  while ((*selectionEntryCursor == (GameEntityRuntime *)0x0 ||
         (ownerIndex == ((*selectionEntryCursor)->common).ownership.ownerIndex))) {
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
    if (entriesRemaining == 0) {
      return CONCAT44(in_EDX,in_EAX);
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0052FE30.
   Ownership: gameplay/selection/runtime.
   Purpose: Validates the global 32-entry selection array for one owner index. Every non-null entry must match
   ownerIndex, resolve to nested type 0x16, and at least one matching nested object must have a positive dword at
   +0x70. CF clear means the complete condition holds; CF set means mismatch or no active entry. EAX is preserved.
   It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed
   ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
*/
undefined8 SelectionInfo_ValidateOwnerType16AndAnyActiveCf(FactionRuntimeIndex ownerIndex)

{
  int *piVar1;
  undefined4 in_EAX;
  int entriesRemaining;
  undefined4 in_EDX;
  int activeEntryCount;
  GameEntityRuntime **selectionEntryCursor;
  GameEntityRuntime *currentEntry;
  
  entriesRemaining = 0x20;
  activeEntryCount = 0;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    currentEntry = *selectionEntryCursor;
    if (currentEntry != (GameEntityRuntime *)0x0) {
      piVar1 = (currentEntry->common).ownership.definitionOrClassRecord;
      if ((ownerIndex != (currentEntry->common).ownership.ownerIndex) ||
         (*(int *)(*piVar1 + 0x4c) != 0x16))
      goto SelectionInfo_ValidateOwnerType16AndAnyActiveCf_ReturnInvalidOrNoActiveWithCarrySet;
      if (piVar1[0x1c] != 0) {
        activeEntryCount = activeEntryCount + 1;
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  if (activeEntryCount != 0) {
    return CONCAT44(in_EDX,in_EAX);
  }
SelectionInfo_ValidateOwnerType16AndAnyActiveCf_ReturnInvalidOrNoActiveWithCarrySet:
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0052FEB0.
   Ownership: gameplay/selection/runtime.
   Purpose: Returns through CF whether the current selection contains an active definition or the exact single-
   class-13 fallback condition.
*/
void SelectionInfo_TestAnyActiveOrSingleClass13Cf(void)

{
  int entriesRemaining;
  int selectedEntryCount;
  GameEntityRuntime **selectionEntryCursor;
  bool selectedEntryIsClass13;
  int selectedDefinitionRecordAddress;
  
  entriesRemaining = 0x20;
  selectedEntryCount = 0;
  selectedEntryIsClass13 = false;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      selectedEntryCount = selectedEntryCount + 1;
      selectedDefinitionRecordAddress =
           *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord;
      if (*(int *)(selectedDefinitionRecordAddress + 0x18) != 0) {
        return;
      }
      selectedEntryIsClass13 = *(int *)(selectedDefinitionRecordAddress + 0x4c) == 0xd;
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  if ((selectedEntryCount == 1) && (selectedEntryIsClass13)) {
    return;
  }
  return;
}

/* Address: 0x0052FF30.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection info test position command at world point carry-flag result.
   Cross-module calls: GridScratch_TestProjectedCellMaskBandsCf [world/pathing/grid],
   ArmyRuntimeNode_DispatchTypedCallback [gameplay/army/runtime].
*/
void SelectionInfo_TestPositionCommandAtWorldPointCf
               (Q12 worldXQ12,Q12 worldYQ12,WorldRuntimeContext *inGameRuntime)

{
  ModelRuntimeNode *pMVar1;
  int iVar2;
  uint uVar3;
  GraphicsWorldCoordinateQ12 extraout_EAX;
  int iVar4;
  byte highBandIndex;
  GraphicsWorldCoordinateQ12 extraout_EDX;
  GameEntityRuntime **ppGVar5;
  GameEntityRuntime *selectedEntity;
  
  iVar4 = 0x20;
  ppGVar5 = g_SelectionInfoEntitySlots->entries;
  while ((selectedEntity = *ppGVar5, selectedEntity == (GameEntityRuntime *)0x0 ||
         (pMVar1 = (selectedEntity->common).ownership.modelNode,
         *(int *)(*(int *)(selectedEntity->common).ownership.definitionOrClassRecord + 0x18) == 0)))
  {
    ppGVar5 = ppGVar5 + 1;
    iVar4 = iVar4 + -1;
    if (iVar4 == 0) {
      iVar4 = 0x20;
      ppGVar5 = g_SelectionInfoEntitySlots->entries;
      while ((*ppGVar5 == (GameEntityRuntime *)0x0 ||
             (iVar2 = *(int *)((*ppGVar5)->common).ownership.definitionOrClassRecord,
             *(int *)(iVar2 + 0x4c) != 0xd))) {
        ppGVar5 = ppGVar5 + 1;
        iVar4 = iVar4 + -1;
        if (iVar4 == 0) {
          return;
        }
      }
      uVar3 = *(uint *)(iVar2 + 0xc4);
      highBandIndex = 3;
      if (((uVar3 & 0x80) == 0) && (highBandIndex = 1, (uVar3 & 4) == 0)) {
        highBandIndex = 6;
      }
      GridScratch_TestProjectedCellMaskBandsCf(worldXQ12,worldYQ12,7,highBandIndex);
      return;
    }
  }
  LOCK();
  (pMVar1->worldTransform).translation.x = worldYQ12;
  UNLOCK();
  LOCK();
  (pMVar1->worldTransform).translation.y = worldXQ12;
  UNLOCK();
  ArmyRuntimeNode_DispatchTypedCallback((ArmyRuntimeSlot **)selectedEntity,inGameRuntime);
  (pMVar1->worldTransform).translation.x = extraout_EAX;
  (pMVar1->worldTransform).translation.y = extraout_EDX;
  return;
}

/* Address: 0x00530050.
   Ownership: gameplay/selection/runtime.
   Purpose: Returns through CF when no selected runtime reports a positive state value at offset 0x100.
   Cross-module calls: ArmyRuntime_TestStateField100NonnegativeCf [gameplay/army/runtime],
   ArmyRuntime_TestStateField100ZeroCf [gameplay/army/runtime].
*/
void SelectionInfo_TestAllStateField100NonpositiveCf(void)

{
  int entriesRemaining;
  int extraout_ECX;
  int extraout_ECX_00;
  GameEntityRuntime **selectionEntryCursor;
  bool bVar1;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    bVar1 = false;
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      ArmyRuntime_TestStateField100NonnegativeCf((ArmyRuntimeSlot *)*selectionEntryCursor);
      entriesRemaining = extraout_ECX;
      if (bVar1) {
        ArmyRuntime_TestStateField100ZeroCf();
        entriesRemaining = extraout_ECX_00;
        if (!bVar1) {
          return;
        }
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  return;
}

/* Address: 0x005300A0.
   Ownership: gameplay/selection/runtime.
   Purpose: Returns through CF when any selected runtime reports a nonnegative state value at offset 0x100.
   Cross-module calls: ArmyRuntime_TestStateField100NonnegativeCf [gameplay/army/runtime].
*/
void SelectionInfo_TestAnyStateField100NonnegativeCf(void)

{
  int entriesRemaining;
  int extraout_ECX;
  GameEntityRuntime **selectionEntryCursor;
  bool bVar1;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    bVar1 = false;
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      ArmyRuntime_TestStateField100NonnegativeCf((ArmyRuntimeSlot *)*selectionEntryCursor);
      entriesRemaining = extraout_ECX;
      if (bVar1) {
        return;
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  return;
}

/* Address: 0x005300E0.
   Ownership: gameplay/selection/runtime.
   Purpose: Returns the first non-null pointer in the fixed 32-entry selection-info array, or null when every slot
   is empty.
*/
GameEntityRuntime * __cdecl SelectionInfo_GetFirstEntry(void)

{
  GameEntityRuntime *firstEntry;
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  GameEntityRuntime **nextSelectionEntryCursor;
  bool currentEntryIsEmpty;
  
  entriesRemaining = 0x20;
  firstEntry = (GameEntityRuntime *)0x0;
  currentEntryIsEmpty = true;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    nextSelectionEntryCursor = selectionEntryCursor;
    if (entriesRemaining == 0) break;
    entriesRemaining = entriesRemaining + -1;
    nextSelectionEntryCursor = selectionEntryCursor + 1;
    currentEntryIsEmpty = *selectionEntryCursor == (GameEntityRuntime *)0x0;
    selectionEntryCursor = nextSelectionEntryCursor;
  } while (currentEntryIsEmpty);
  if (!currentEntryIsEmpty) {
    firstEntry = nextSelectionEntryCursor[-1];
  }
  return firstEntry;
}

/* Address: 0x00530100.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the fixed 32-entry selection-info array for entry. CF is clear when found and set when absent;
   EAX is preserved.
*/
undefined4 SelectionInfo_FindEntryCf(GameEntityRuntime *entry)

{
  undefined4 in_EAX;
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  GameEntityRuntime *currentEntry;
  
  entriesRemaining = 0x20;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (entriesRemaining == 0) {
      return in_EAX;
    }
    entriesRemaining = entriesRemaining + -1;
    currentEntry = *selectionEntryCursor;
    selectionEntryCursor = selectionEntryCursor + 1;
  } while (entry != currentEntry);
  return in_EAX;
}

/* Address: 0x00530770.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection info collect attachment effect variant mask.
   Cross-module calls: ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs [gameplay/army/runtime].
*/
dword SelectionInfo_CollectAttachmentEffectVariantMask(void)

{
  dword effectVariantMask;
  uint extraout_EAX;
  int entriesRemaining;
  int extraout_ECX;
  uint unaff_EBX;
  GameEntityRuntime **selectionEntryCursor;
  
  entriesRemaining = 0x20;
  effectVariantMask = 0;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs
                (((*selectionEntryCursor)->common).ownership.definitionOrClassRecord);
      effectVariantMask = extraout_EAX | unaff_EBX;
      entriesRemaining = extraout_ECX;
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  return effectVariantMask;
}

/* Address: 0x005307C0.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the fixed 32-entry selection-info array. Entries whose nested type is 0x16 contribute flag 0x08;
   type 0x0D entries contribute their nested capability dword at +0xC4. Returns the OR-combined mask.
*/
dword __cdecl SelectionInfo_CollectCapabilityFlags(void)

{
  dword capabilityMask;
  int entriesRemaining;
  GameEntityRuntime **selectionEntryCursor;
  int currentEntityDefinition;
  
  entriesRemaining = 0x20;
  capabilityMask = 0;
  selectionEntryCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      currentEntityDefinition =
           *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord;
      if (*(int *)(currentEntityDefinition + 0x4c) == 0x16) {
        capabilityMask = capabilityMask | 8;
      }
      else if (*(int *)(currentEntityDefinition + 0x4c) == 0xd) {
        capabilityMask = capabilityMask | *(uint *)(currentEntityDefinition + 0xc4);
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  return capabilityMask;
}

/* Address: 0x00561000.
   Ownership: gameplay/selection/runtime.
   Purpose: Clears the selected player terrain-edit state field and clears the matching local root field when the
   selected player is the local player. Kept distinct from frontend slot indices, faction runtime indices, network
   endpoint identity, and PCK asset identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling
   convention, storage, body bytes, control flow, and executable data remain unchanged.
*/
void __fastcall
SelectionPlayerRuntime_ClearTerrainEditSelectionState
          (undefined4 param_1,undefined4 param_2,PlayerRuntimeId playerRuntimeId)

{
  InGameRuntimeRootImageC3E4 *inGameRuntimeRoot;
  
  inGameRuntimeRoot = g_InGameRuntimeRoot;
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->activePairCount8084 = 0;
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    inGameRuntimeRoot->localPlayerPairCount0BA4 = 0;
  }
  return;
}

/* Address: 0x00571020.
   Ownership: gameplay/selection/runtime.
   Purpose: CF=0 reports an existing pair and CF=1 reports absence. Archived binary body 00571020-0057108B; EAX,
   ECX, and EDX are preserved or incidental caller state rather than synthetic parameters or normal returns.
*/
void SelectionPlayerPairList_ContainsPairCf
               (SelectionPlayerPairValue pairValue,SelectionPlayerPairKey pairKey,
               PlayerRuntimeId playerRuntimeId)

{
  dword pairRecordsRemaining;
  SelectionPlayerPairRecord *pairRecordCursor;
  
  pairRecordsRemaining = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->activePairCount8084
  ;
  pairRecordCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pairRecords80_807F;
  while( true ) {
    if (pairRecordsRemaining == 0) {
      return;
    }
    if ((pairKey == pairRecordCursor->pairKey) && (pairValue == pairRecordCursor->pairValue)) break;
    pairRecordCursor = pairRecordCursor + 1;
    pairRecordsRemaining = pairRecordsRemaining - 1;
  }
  return;
}

/* Address: 0x005302B0.
   Ownership: gameplay/selection/runtime.
   Purpose: Typed parameters: p0 coordinateA→Q12, p1 coordinateB→Q12. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: SelectionPointerArray_IsSpatialSpreadTooLargeCf, SelectionPointerArray_Clear32.
   Cross-module calls: ArmyRuntime_QueueOrStartMoveCommandVariantA [gameplay/army/movement].
*/
void SelectionPointerArray_ApplyPositionCommandVariantB
               (Q12 coordinateA,Q12 coordinateB,SelectionPointerArray32 *selection)

{
  ArmyMovementRuntime *movementRuntime;
  void *pvVar1;
  int extraout_EAX;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar2;
  int targetWorldY;
  int extraout_EDX;
  int selectedEntryCount;
  Q12 targetWorldX;
  int *singleClass13Entry;
  GameEntityRuntime **ppGVar3;
  GameEntityRuntime **selectionEntryCursor;
  byte in_CF;
  int entityDefinitionAddress;
  
  SelectionPointerArray_IsSpatialSpreadTooLargeCf(selection);
  iVar2 = extraout_ECX;
  targetWorldY = coordinateA;
  targetWorldX = coordinateB;
  ppGVar3 = selection->entries;
  do {
    movementRuntime = (ArmyMovementRuntime *)*ppGVar3;
    if (movementRuntime != (ArmyMovementRuntime *)0x0) {
      if ((in_CF & 1) == 0) {
        targetWorldX = targetWorldX - movementRuntime->classState60;
        targetWorldY = targetWorldY - movementRuntime->ownerValue64;
      }
      ArmyRuntime_QueueOrStartMoveCommandVariantA(targetWorldY,targetWorldX,movementRuntime);
      iVar2 = extraout_ECX_00;
      targetWorldY = extraout_EDX;
      if ((in_CF & 1) == 0) {
        targetWorldX = targetWorldX + *(int *)(extraout_EAX + 0x60);
        targetWorldY = extraout_EDX + *(int *)(extraout_EAX + 100);
      }
    }
    ppGVar3 = ppGVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  iVar2 = 0x20;
  selectedEntryCount = 0;
  singleClass13Entry = (int *)0x0;
  selectionEntryCursor = selection->entries;
  do {
    if (*selectionEntryCursor != (GameEntityRuntime *)0x0) {
      selectedEntryCount = selectedEntryCount + 1;
      entityDefinitionAddress =
           *(int *)((*selectionEntryCursor)->common).ownership.definitionOrClassRecord;
      if (*(int *)(entityDefinitionAddress + 0x18) != 0) {
        return;
      }
      if (*(int *)(entityDefinitionAddress + 0x4c) == 0xd) {
        singleClass13Entry = (int *)*selectionEntryCursor;
      }
    }
    selectionEntryCursor = selectionEntryCursor + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  if ((selectedEntryCount == 1) &&
     ((GameEntityRuntime *)singleClass13Entry != (GameEntityRuntime *)0x0)) {
    pvVar1 = (((GameEntityRuntime *)singleClass13Entry)->common).ownership.definitionOrClassRecord;
    *(Q12 *)((int)pvVar1 + 0x78) = coordinateB;
    *(Q12 *)((int)pvVar1 + 0x7c) = coordinateA;
    *(uint *)((int)pvVar1 + 0xec) = *(uint *)((int)pvVar1 + 0xec) | 0x800;
    SelectionPointerArray_Clear32(selection);
  }
  return;
}

/* Address: 0x00530420.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection runtime reset movement prune and recenter entries.
   Local calls: SelectionPointerArray_RecenterOffsetsAroundAveragePosition, SelectionPointerArray_Clear32.
   Cross-module calls: ArmyRuntime_ResetMovementStateFromModel [gameplay/army/movement],
   ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions], ModelNodeRuntime_TransformLocalPointRegs
   [world/model/hierarchy].
*/
void SelectionRuntime_ResetMovementPruneAndRecenterEntries(GameEntityRuntime **selectionEntries)

{
  int *piVar1;
  int iVar2;
  void *pvVar3;
  ModelRuntimeNode *modelNodeRuntime;
  int *extraout_EAX;
  ModelPackedPointRecord *localPointRecord;
  int iVar4;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  int extraout_EDX;
  int iVar5;
  GameEntityRuntime *pGVar6;
  GameEntityRuntime **ppGVar7;
  bool bVar8;
  undefined8 uVar9;
  
  iVar4 = 0x20;
  ppGVar7 = selectionEntries;
  do {
    pGVar6 = *ppGVar7;
    if ((pGVar6 != (GameEntityRuntime *)0x0) && (((pGVar6->common).commandFlags & 2) == 0)) {
      ArmyRuntime_ResetMovementStateFromModel((ArmyRuntimeSlot *)pGVar6);
      extraout_EAX[0xb] = extraout_EAX[0xb] & 0xffffffef;
      extraout_EAX[6] = extraout_EAX[6] & 0xfffffdff;
      piVar1 = (int *)*extraout_EAX;
      iVar4 = extraout_ECX;
      if (*(int *)(*piVar1 + 0x4c) == 0x16) {
        *ppGVar7 = (GameEntityRuntime *)0x0;
        piVar1[0x37] = 0;
      }
    }
    ppGVar7 = ppGVar7 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  pGVar6 = (GameEntityRuntime *)0x0;
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition
            ((SelectionPointerArray32 *)selectionEntries);
  iVar4 = extraout_ECX_00;
  iVar5 = extraout_EDX;
  ppGVar7 = selectionEntries;
  do {
    if (*ppGVar7 != (GameEntityRuntime *)0x0) {
      iVar5 = iVar5 + 1;
      iVar2 = *(int *)((*ppGVar7)->common).ownership.definitionOrClassRecord;
      if (*(int *)(iVar2 + 0x18) != 0) {
        return;
      }
      if (*(int *)(iVar2 + 0x4c) == 0xd) {
        pGVar6 = *ppGVar7;
      }
    }
    ppGVar7 = ppGVar7 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  if ((iVar5 == 1) && (pGVar6 != (GameEntityRuntime *)0x0)) {
    pvVar3 = (pGVar6->common).ownership.definitionOrClassRecord;
    modelNodeRuntime = (pGVar6->common).ownership.modelNode;
    bVar8 = false;
    *(uint *)((int)pvVar3 + 0xec) = *(uint *)((int)pvVar3 + 0xec) & 0xfffff7ff;
    localPointRecord =
         (ModelPackedPointRecord *)
         ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNodeRuntime->modelPayload).modelResource);
    if (!bVar8) {
      uVar9 = ModelNodeRuntime_TransformLocalPointRegs
                        (extraout_ECX_01,localPointRecord,modelNodeRuntime);
      *(int *)((int)pvVar3 + 0x78) = (int)uVar9;
      *(undefined4 *)((int)pvVar3 + 0x7c) = extraout_ECX_02;
      SelectionPointerArray_Clear32((SelectionPointerArray32 *)selectionEntries);
    }
  }
  return;
}

/* Address: 0x0052FCE0.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection pointer array add world entries matching runtime identity.
   Local calls: SelectionPointerArray_InsertUniqueAndRecenter.
*/
void SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity
               (ArmyRuntimeSlot *sourceArmyRuntime,SelectionPointerArray32 *selection)

{
  GameEntityRuntime *entityRuntime;
  RuntimeToken extraout_ECX;
  RuntimeToken runtimeIdentity;
  int extraout_EDX;
  int ownerIndex;
  WorldRuntimeNode *worldNodeCursor;
  
  runtimeIdentity = sourceArmyRuntime->armyAssetId;
  ownerIndex = sourceArmyRuntime->factionIndex;
  for (worldNodeCursor = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      worldNodeCursor != (WorldRuntimeNode *)0x0;
      worldNodeCursor = (worldNodeCursor->common).nextNode) {
    if (((worldNodeCursor[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
        (entityRuntime = *(GameEntityRuntime **)((int)worldNodeCursor->runtimePayload + 8),
        runtimeIdentity == (entityRuntime->common).runtimeIdentityOrArmyAssetId)) &&
       (ownerIndex == (entityRuntime->common).ownership.ownerIndex)) {
      SelectionPointerArray_InsertUniqueAndRecenter
                (runtimeIdentity,ownerIndex,entityRuntime,selection);
      runtimeIdentity = extraout_ECX;
      ownerIndex = extraout_EDX;
    }
  }
  return;
}

/* Address: 0x005303A0.
   Ownership: gameplay/selection/runtime.
   Purpose: Probes the fixed selection spread, then applies the existing position command to each of 32 possible
   entries. EAX and CF-derived behavior are preserved. Typed parameters: p0 coordinateA→Q12, p1 coordinateB→Q12.
   Calling convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: SelectionPointerArray_IsSpatialSpreadTooLargeCf.
   Cross-module calls: ArmyRuntime_QueueWaypointOrStartMoveVariantA [gameplay/army/movement].
*/
void SelectionPointerArray_ApplyPositionCommand
               (Q12 coordinateA,Q12 coordinateB,SelectionPointerArray32 *selection)

{
  ArmyMovementRuntime *movementRuntime;
  int extraout_EAX;
  int extraout_ECX;
  int extraout_ECX_00;
  int entriesRemaining;
  int extraout_EDX;
  byte in_CF;
  
  SelectionPointerArray_IsSpatialSpreadTooLargeCf(selection);
  entriesRemaining = extraout_ECX;
  do {
    movementRuntime = *(ArmyMovementRuntime **)selection;
    if (movementRuntime != (ArmyMovementRuntime *)0x0) {
      if ((in_CF & 1) == 0) {
        coordinateB = coordinateB - movementRuntime->classState60;
        coordinateA = coordinateA - movementRuntime->ownerValue64;
      }
      ArmyRuntime_QueueWaypointOrStartMoveVariantA(coordinateA,coordinateB,movementRuntime);
      entriesRemaining = extraout_ECX_00;
      coordinateA = extraout_EDX;
      if ((in_CF & 1) == 0) {
        coordinateB = coordinateB + *(int *)(extraout_EAX + 0x60);
        coordinateA = extraout_EDX + *(int *)(extraout_EAX + 100);
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  return;
}

/* Address: 0x0052D600.
   Ownership: gameplay/selection/runtime.
   Purpose: Formats a signed number, measures and draws it centered over the selected panel-cell sprite, and
   returns the next layout coordinates while respecting horizontal and vertical suppression flags. Typed
   parameters: p4 drawX→UiPixelCoordinate_V297, p5 drawY→UiPixelCoordinate_V297, p7
   cellIndex→SelectionPanelCellIndex_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p6
   value→SelectionPanelNumericValue32_V343. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: RichTextCommandStream_MeasureRegs [assets/text/richtext],
   RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
undefined8
SelectionPanel_DrawNumberCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          SelectionPanelNumericValue32 value,SelectionPanelCellIndex cellIndex)

{
  uint uVar1;
  void *pvVar2;
  int iVar3;
  qword qVar7;
  int extraout_ECX;
  int iVar4;
  int arg5;
  uint *puVar5;
  RichTextExtentRegs RVar6;
  
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,0xf,1,value,
             (word *)&g_SelectionPanelNumberScratchUtf16);
  RVar6 = RichTextCommandStream_MeasureRegs
                    (g_SelectionPanelNumberTextStyle,(word *)&g_SelectionPanelNumberScratchUtf16);
  pvVar2 = g_SelectionPanelData;
  puVar5 = (uint *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  arg5 = drawY + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc);
  qVar7._0_4_ = (*g_SelectionPanelBlitOpaque)
                          (clipTop,clipLeft,clipBottom,clipRight,
                           drawX + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10),
                           arg5,*(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8),
                           g_SelectionPanelTextureSource,g_FramebufferAccess);
  qVar7 = (*g_GraphicsTextureSourceGetLogicalSize)
                    ((dword)(qword)qVar7,g_SelectionPanelTextureSource);
  iVar4 = (int)(qVar7 >> 0x20);
  iVar3 = (int)qVar7;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,g_SelectionPanelNumberTextStyle,
             (word *)&g_SelectionPanelNumberScratchUtf16,
             ((int)(iVar4 - RVar6.heightPixels) >> 1) + extraout_ECX,
             ((int)(iVar3 - RVar6.widthPixels) >> 1) + arg5);
  uVar1 = *puVar5;
  if ((uVar1 & 4) != 0) {
    iVar3 = 0;
  }
  if ((uVar1 & 8) != 0) {
    iVar4 = 0;
  }
  return CONCAT44(iVar4 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0x10) + drawX,
                  iVar3 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0xc) + drawY);
}

/* Address: 0x0052D6F0.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws the selected panel icon cell and returns the next layout coordinates while respecting the
   verified horizontal and vertical suppression flags. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1
   clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4
   drawX→UiPixelCoordinate_V297, p5 drawY→UiPixelCoordinate_V297, p6 cellIndex→SelectionPanelCellIndex_V342.
   Calling convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
undefined8
SelectionPanel_DrawIconCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          SelectionPanelCellIndex cellIndex)

{
  dword arg6;
  uint uVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
  uint *puVar5;
  qword qVar6;
  
  pvVar2 = g_SelectionPanelData;
  puVar5 = (uint *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  arg6 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,
             drawX + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10),
             drawY + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc),arg6,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  qVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(arg6,g_SelectionPanelTextureSource);
  iVar4 = (int)(qVar6 >> 0x20);
  iVar3 = (int)qVar6;
  uVar1 = *puVar5;
  if ((uVar1 & 4) != 0) {
    iVar3 = 0;
  }
  if ((uVar1 & 8) != 0) {
    iVar4 = 0;
  }
  return CONCAT44(iVar4 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0x10) + drawX,
                  iVar3 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0xc) + drawY);
}

/* Address: 0x0052D770.
   Ownership: gameplay/selection/runtime.
   Purpose: Clamps the current and maximum values, chooses the stepped meter subresource frame, draws the base and
   meter frame, and returns the next layout coordinates. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1
   clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4
   drawX→UiPixelCoordinate_V297, p5 drawY→UiPixelCoordinate_V297, p6 maximumValue→UiNumericValue32_V308, p7
   currentValue→UiNumericValue32_V308, p8 cellIndex→SelectionPanelCellIndex_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
undefined8
SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex)

{
  dword arg6;
  uint uVar1;
  void *pvVar2;
  int iVar3;
  qword qVar6;
  int iVar4;
  uint *puVar5;
  
  pvVar2 = g_SelectionPanelData;
  if (currentValue < 0) {
    currentValue = 0;
  }
  else if (maximumValue < currentValue) {
    currentValue = maximumValue;
  }
  if (maximumValue == 0) {
    iVar3 = 0x11;
  }
  else {
    iVar3 = ((uint)(currentValue * 0x20 + maximumValue) / (uint)maximumValue >> 1) + 1;
  }
  puVar5 = (uint *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  iVar4 = *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  arg6 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  qVar6._0_4_ = (*g_SelectionPanelBlitOpaque)
                          (clipTop,clipLeft,clipBottom,clipRight,
                           drawX + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0x10),
                           drawY + *(int *)((int)g_SelectionPanelData + cellIndex * 0x10 + 0xc),arg6
                           ,g_SelectionPanelTextureSource,g_FramebufferAccess);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,qVar6._4_4_,(sdword)(qword)qVar6,iVar3 + iVar4,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  qVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(arg6,g_SelectionPanelTextureSource);
  iVar4 = (int)(qVar6 >> 0x20);
  iVar3 = (int)qVar6;
  uVar1 = *puVar5;
  if ((uVar1 & 4) != 0) {
    iVar3 = 0;
  }
  if ((uVar1 & 8) != 0) {
    iVar4 = 0;
  }
  return CONCAT44(iVar4 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0x10) + drawX,
                  iVar3 + *(int *)((int)pvVar2 + cellIndex * 0x10 + 0xc) + drawY);
}

/* Address: 0x0052D850.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws left and right cap sprites, clamps current against maximum, rounds the proportional interior
   width and six-step fill frame, and draws the two interior bar spans. Typed parameters: p2
   clipTop→UiPixelCoordinate_V297, p3 clipLeft→UiPixelCoordinate_V297, p4 clipBottom→UiPixelCoordinate_V297, p5
   clipRight→UiPixelCoordinate_V297, p7 barEndCoordinate→UiPixelCoordinate_V297, p8
   barStartCoordinate→UiPixelCoordinate_V297, p9 maximumValue→UiNumericValue32_V308, p10
   currentValue→UiNumericValue32_V308, p11 cellIndex→SelectionPanelCellIndex_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
void SelectionPanel_DrawProportionalCappedBar
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,undefined4 param_5,UiPixelCoordinate barEndCoordinate,
               UiPixelCoordinate barStartCoordinate,UiNumericValue32 maximumValue,
               UiNumericValue32 currentValue,SelectionPanelCellIndex cellIndex)

{
  int arg7;
  int arg5;
  dword arg0;
  longlong lVar1;
  int iVar2;
  int iVar3;
  sdword arg7_00;
  sdword arg4;
  qword qVar4;
  sdword arg4_00;
  sdword arg6;
  sdword arg6_00;
  
  arg0 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_SelectionPanelTextureSource);
  qVar4._0_4_ = (*g_SelectionPanelBlitOpaque)
                          (clipTop,clipLeft,clipBottom,clipRight,arg4,barStartCoordinate,arg0,
                           g_SelectionPanelTextureSource,g_FramebufferAccess);
  arg7 = barStartCoordinate + (int)(qword)qVar4;
  qVar4._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
  arg5 = barEndCoordinate - (int)(qword)qVar4;
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,arg4_00,arg5,arg0 + 2,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  iVar2 = arg5 - arg7;
  if (currentValue < 0) {
    currentValue = 0;
  }
  else if (maximumValue < currentValue) {
    currentValue = maximumValue;
  }
  if ((maximumValue != 0) &&
     (lVar1 = (longlong)iVar2, iVar2 = (int)((lVar1 * currentValue) / (longlong)maximumValue),
     maximumValue < (int)((lVar1 * currentValue) % (longlong)maximumValue) * 2)) {
    iVar2 = iVar2 + 1;
  }
  if (maximumValue == 0) {
    iVar3 = 6;
  }
  else {
    iVar3 = (int)(((longlong)currentValue * 6) / (longlong)maximumValue);
    if (maximumValue < (int)(((longlong)currentValue * 6) % (longlong)maximumValue) * 2) {
      iVar3 = iVar3 + 1;
    }
  }
  arg7_00 = (*g_SelectionPanelBlitClipped)
                      (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,iVar2 + arg7,arg6,arg7,
                       iVar3 + 2 + arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  (*g_SelectionPanelBlitClipped)
            (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,arg5,arg6_00,arg7_00,arg0 + 1,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  return;
}

/* Address: 0x0052DAF0.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws the two cap sprites and fills the span between them with the verified middle bar subresource.
   Typed parameters: p2 clipTop→UiPixelCoordinate_V297, p3 clipLeft→UiPixelCoordinate_V297, p4
   clipBottom→UiPixelCoordinate_V297, p5 clipRight→UiPixelCoordinate_V297, p7
   barEndCoordinate→UiPixelCoordinate_V297, p8 barStartCoordinate→UiPixelCoordinate_V297, p9
   cellIndex→SelectionPanelCellIndex_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void SelectionPanel_DrawForwardCappedBar
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,undefined4 param_5,UiPixelCoordinate barEndCoordinate,
               UiPixelCoordinate barStartCoordinate,SelectionPanelCellIndex cellIndex)

{
  dword arg0;
  sdword arg4;
  qword qVar1;
  qword qVar2;
  sdword arg6;
  
  arg0 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_SelectionPanelTextureSource);
  qVar1._0_4_ = (*g_SelectionPanelBlitOpaque)
                          (clipTop,clipLeft,clipBottom,clipRight,arg4,barStartCoordinate,arg0,
                           g_SelectionPanelTextureSource,g_FramebufferAccess);
  qVar2._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,qVar2._4_4_,barEndCoordinate - (int)(qword)qVar2,
             arg0 + 2,g_SelectionPanelTextureSource,g_FramebufferAccess);
  (*g_SelectionPanelBlitClipped)
            (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,barEndCoordinate - (int)(qword)qVar2,
             arg6,barStartCoordinate + (int)(qword)qVar1,arg0 + 1,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  return;
}

/* Address: 0x0052DBC0.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws the selected left and right cap sprites and fills the complete interior span with the verified
   middle bar subresource. Typed parameters: p2 clipTop→UiPixelCoordinate_V297, p3 clipLeft→UiPixelCoordinate_V297,
   p4 clipBottom→UiPixelCoordinate_V297, p5 clipRight→UiPixelCoordinate_V297, p6
   barEndCoordinate→UiPixelCoordinate_V297, p7 barStartCoordinate→UiPixelCoordinate_V297, p9
   cellIndex→SelectionPanelCellIndex_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void SelectionPanel_DrawSolidCappedBar
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiPixelCoordinate barEndCoordinate,
               UiPixelCoordinate barStartCoordinate,undefined4 param_7,
               SelectionPanelCellIndex cellIndex)

{
  dword arg0;
  sdword arg5;
  sdword arg5_00;
  sdword arg7;
  int extraout_EDX;
  int extraout_EDX_00;
  
  arg0 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_SelectionPanelTextureSource);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,arg5,arg0,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate - extraout_EDX_00,arg5_00,
             arg0 + 2,g_SelectionPanelTextureSource,g_FramebufferAccess);
  (*g_SelectionPanelBlitClipped)
            (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate - extraout_EDX_00,-0x80000000,
             barStartCoordinate + extraout_EDX,arg7,arg0 + 1,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  return;
}

/* Address: 0x0052DFF0.
   Ownership: gameplay/selection/runtime.
   Purpose: Draws a capped segmented bar, emits optional marker and segment sprites according to the panel flags
   and counts, and fills the remaining span. Typed parameters: p2 clipTop→UiPixelCoordinate_V297, p3
   clipLeft→UiPixelCoordinate_V297, p4 clipBottom→UiPixelCoordinate_V297, p5 clipRight→UiPixelCoordinate_V297, p6
   barEndCoordinate→UiPixelCoordinate_V297, p7 barStartCoordinate→UiPixelCoordinate_V297, p11
   cellIndex→SelectionPanelCellIndex_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p9
   totalSegmentCount→SelectionPanelSegmentCount_V343, p10 filledSegmentCount→SelectionPanelSegmentCount_V343.
   Calling convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void SelectionPanel_DrawSegmentedCappedBar
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiPixelCoordinate barEndCoordinate,
               UiPixelCoordinate barStartCoordinate,undefined4 param_7,
               SelectionPanelSegmentCount totalSegmentCount,
               SelectionPanelSegmentCount filledSegmentCount,SelectionPanelCellIndex cellIndex)

{
  dword arg0;
  sdword arg5;
  sdword arg5_00;
  sdword arg7;
  sdword extraout_ECX;
  sdword extraout_ECX_00;
  sdword extraout_ECX_01;
  sdword extraout_ECX_02;
  sdword extraout_ECX_03;
  sdword extraout_ECX_04;
  sdword sVar1;
  sdword extraout_ECX_05;
  sdword extraout_ECX_06;
  sdword extraout_ECX_07;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int iVar2;
  int extraout_EDX_02;
  int extraout_EDX_03;
  int extraout_EDX_04;
  int extraout_EDX_05;
  int extraout_EDX_06;
  int extraout_EDX_07;
  int extraout_EDX_08;
  int extraout_EDX_09;
  int extraout_EDX_10;
  uint *puVar3;
  
  puVar3 = (uint *)((int)g_SelectionPanelData + cellIndex * 0x10 + 4);
  arg0 = *(dword *)((int)g_SelectionPanelData + cellIndex * 0x10 + 8);
  (*g_GraphicsTextureSourceGetLogicalSize)(arg0,g_SelectionPanelTextureSource);
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,arg5,arg0,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  barStartCoordinate = barStartCoordinate + extraout_EDX;
  (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 2,g_SelectionPanelTextureSource);
  barEndCoordinate = barEndCoordinate - extraout_EDX_00;
  (*g_SelectionPanelBlitOpaque)
            (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,arg5_00,arg0 + 2,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 3,g_SelectionPanelTextureSource);
  iVar2 = filledSegmentCount;
  if ((*puVar3 & 0x400) != 0) {
    iVar2 = totalSegmentCount;
  }
  iVar2 = extraout_EDX_01 * iVar2 + barStartCoordinate;
  if (barEndCoordinate < iVar2) {
    (*g_SelectionPanelBlitClipped)
              (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,-0x80000000,barStartCoordinate
               ,arg7,arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  else if ((*puVar3 & 0x100) == 0) {
    if ((*puVar3 & 0x200) == 0) {
      (*g_SelectionPanelBlitClipped)
                (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,-0x80000000,
                 barEndCoordinate - (barEndCoordinate - iVar2 >> 1),arg7,arg0 + 1,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 4,g_SelectionPanelTextureSource);
      sVar1 = extraout_ECX_05;
      barEndCoordinate = extraout_EDX_07;
      iVar2 = extraout_EDX_08;
      for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
        barEndCoordinate = barEndCoordinate - iVar2;
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,sVar1,arg0 + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount = totalSegmentCount + -1;
        sVar1 = extraout_ECX_06;
        iVar2 = extraout_EDX_09;
      }
      if ((*puVar3 & 0x400) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
          barEndCoordinate = barEndCoordinate - iVar2;
          (*g_SelectionPanelBlitOpaque)
                    (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,sVar1,arg0 + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
          sVar1 = extraout_ECX_07;
          iVar2 = extraout_EDX_10;
        }
      }
      (*g_SelectionPanelBlitClipped)
                (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,-0x80000000,
                 barStartCoordinate,sVar1,arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess
                );
    }
    else {
      (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 4,g_SelectionPanelTextureSource);
      sVar1 = extraout_ECX_02;
      iVar2 = extraout_EDX_04;
      for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
        barEndCoordinate = barEndCoordinate - iVar2;
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,sVar1,arg0 + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount = totalSegmentCount + -1;
        sVar1 = extraout_ECX_03;
        iVar2 = extraout_EDX_05;
      }
      if ((*puVar3 & 0x400) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
          barEndCoordinate = barEndCoordinate - iVar2;
          (*g_SelectionPanelBlitOpaque)
                    (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,sVar1,arg0 + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
          sVar1 = extraout_ECX_04;
          iVar2 = extraout_EDX_06;
        }
      }
      (*g_SelectionPanelBlitClipped)
                (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,-0x80000000,
                 barStartCoordinate,sVar1,arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess
                );
    }
  }
  else {
    (*g_GraphicsTextureSourceGetLogicalSize)(arg0 + 4,g_SelectionPanelTextureSource);
    sVar1 = extraout_ECX;
    for (; filledSegmentCount != 0; filledSegmentCount = filledSegmentCount + -1) {
      (*g_SelectionPanelBlitOpaque)
                (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,sVar1,arg0 + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      barStartCoordinate = barStartCoordinate + extraout_EDX_02;
      totalSegmentCount = totalSegmentCount + -1;
      sVar1 = extraout_ECX_00;
    }
    if ((*puVar3 & 0x400) != 0) {
      for (; totalSegmentCount != 0; totalSegmentCount = totalSegmentCount + -1) {
        (*g_SelectionPanelBlitOpaque)
                  (clipTop,clipLeft,clipBottom,clipRight,barStartCoordinate,sVar1,arg0 + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        barStartCoordinate = barStartCoordinate + extraout_EDX_03;
        sVar1 = extraout_ECX_01;
      }
    }
    (*g_SelectionPanelBlitClipped)
              (clipTop,clipLeft,clipBottom,clipRight,barEndCoordinate,-0x80000000,barStartCoordinate
               ,sVar1,arg0 + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  return;
}

/* Address: 0x00530130.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection pointer array apply army runtime target.
   Cross-module calls: ArmyRuntime_TestStateField100ZeroCf [gameplay/army/runtime],
   ArmyRuntime_ResolveCommandTarget [gameplay/army/runtime].
*/
void SelectionPointerArray_ApplyArmyRuntimeTarget
               (ArmyRuntimeSlot *targetArmyRuntime,SelectionPointerArray32 *selection)

{
  ArmyRuntimeSlot *armyRuntime;
  int extraout_EAX;
  int entriesRemaining;
  int extraout_ECX;
  int extraout_ECX_00;
  GameEntityRuntime *resolvedTarget;
  undefined4 extraout_EDX;
  bool bVar1;
  
  entriesRemaining = 0x20;
  do {
    bVar1 = false;
    if (*(int *)selection != 0) {
      armyRuntime = (ArmyRuntimeSlot *)ArmyRuntime_TestStateField100ZeroCf();
      entriesRemaining = extraout_ECX;
      if (!bVar1) {
        ArmyRuntime_ResolveCommandTarget(resolvedTarget,armyRuntime);
        *(undefined4 *)(extraout_EAX + 0x98) = extraout_EDX;
        *(uint *)(extraout_EAX + 0x2c) = *(uint *)(extraout_EAX + 0x2c) | 0x14;
        *(uint *)(extraout_EAX + 0x18) = *(uint *)(extraout_EAX + 0x18) & 0xfffffdff;
        entriesRemaining = extraout_ECX_00;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    entriesRemaining = entriesRemaining + -1;
  } while (entriesRemaining != 0);
  return;
}

/* Address: 0x00530190.
   Ownership: gameplay/selection/runtime.
   Purpose: Applies the target-position command tuple to every eligible selected runtime. Typed parameters: p0
   coordinateA→Q12, p2 coordinateC→Q12. Calling convention, complete VariableStorage serialization, function bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: ArmyRuntime_TestStateField100ZeroCf [gameplay/army/runtime],
   ArmyRuntime_ApplyTargetPositionCommand [gameplay/army/runtime].
*/
void SelectionPointerArray_ApplyTargetPositionCommand
               (Q12 coordinateA,dword coordinateB,Q12 coordinateC,SelectionPointerArray32 *selection
               )

{
  ArmyRuntimeSlot *armyRuntime;
  int extraout_EAX;
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  Q12 coordinateB_00;
  bool bVar2;
  
  iVar1 = 0x20;
  do {
    bVar2 = false;
    if (*(int *)selection != 0) {
      armyRuntime = (ArmyRuntimeSlot *)ArmyRuntime_TestStateField100ZeroCf();
      iVar1 = extraout_ECX;
      if (!bVar2) {
        ArmyRuntime_ApplyTargetPositionCommand(coordinateA,coordinateB_00,coordinateC,armyRuntime);
        *(uint *)(extraout_EAX + 0x2c) = *(uint *)(extraout_EAX + 0x2c) | 0x14;
        *(uint *)(extraout_EAX + 0x18) = *(uint *)(extraout_EAX + 0x18) & 0xfffffdff;
        *(int *)(extraout_EAX + 0x30) = *(int *)(extraout_EAX + 0x30) << 2;
        iVar1 = extraout_ECX_00;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}

/* Address: 0x00530540.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection runtime reset movement anchors and clear flag200 for eligible entries.
   Cross-module calls: GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel
   [gameplay/faction/runtime].
*/
void SelectionRuntime_ResetMovementAnchorsAndClearFlag200ForEligibleEntries(int *selectionEntries)

{
  int entityRuntime;
  int extraout_EAX;
  int iVar1;
  int extraout_ECX;
  
  iVar1 = 0x20;
  do {
    entityRuntime = *selectionEntries;
    if ((entityRuntime != 0) && ((*(uint *)(entityRuntime + 0x18) & 2) == 0)) {
      GameEntityRuntime_ResetMovementFlagsAndAnchorCoordinatesFromModel(entityRuntime);
      *(uint *)(extraout_EAX + 0x18) = *(uint *)(extraout_EAX + 0x18) & 0xfffffdff;
      iVar1 = extraout_ECX;
    }
    selectionEntries = selectionEntries + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}

/* Address: 0x005305A0.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection runtime interrupt targets and clear flag10 for eligible entries.
   Cross-module calls: ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration [gameplay/army/movement].
*/
void SelectionRuntime_InterruptTargetsAndClearFlag10ForEligibleEntries(undefined4 *selectionEntries)

{
  ArmyRuntimeSlot *armyRuntime;
  int extraout_EAX;
  int iVar1;
  int extraout_ECX;
  
  iVar1 = 0x20;
  do {
    armyRuntime = (ArmyRuntimeSlot *)*selectionEntries;
    if ((armyRuntime != (ArmyRuntimeSlot *)0x0) && ((armyRuntime->movementStateFlags & 2) == 0)) {
      ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(armyRuntime);
      *(uint *)(extraout_EAX + 0x2c) = *(uint *)(extraout_EAX + 0x2c) & 0xffffffef;
      iVar1 = extraout_ECX;
    }
    selectionEntries = selectionEntries + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}

/* Address: 0x00530600.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection runtime apply flags418 unless bit8 to eligible entries.
   Cross-module calls: ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive [world/model/hierarchy].
*/
void SelectionRuntime_ApplyFlags418UnlessBit8ToEligibleEntries(undefined4 *selectionEntries)

{
  int *modelRuntime;
  int iVar1;
  int extraout_ECX;
  WorldRuntimeContext *contextArg;
  
  iVar1 = 0x20;
  contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
  do {
    modelRuntime = (int *)*selectionEntries;
    if ((modelRuntime != (int *)0x0) && ((modelRuntime[6] & 2U) == 0)) {
      ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive(contextArg,modelRuntime);
      iVar1 = extraout_ECX;
    }
    selectionEntries = selectionEntries + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}

/* Address: 0x0052FCA0.
   Ownership: gameplay/selection/runtime.
   Purpose: Inserts a pointer into the first free slot of a 32-entry selection array when absent, then recomputes
   relative offsets around the average position.
   Local calls: SelectionPointerArray_RecenterOffsetsAroundAveragePosition.
*/
undefined4 __fastcall
SelectionPointerArray_InsertUniqueAndRecenter
          (undefined4 param_1,undefined4 param_2,GameEntityRuntime *entityRuntime,
          SelectionPointerArray32 *selection)

{
  undefined4 in_EAX;
  int iVar1;
  GameEntityRuntime **ppGVar2;
  GameEntityRuntime **ppGVar3;
  undefined1 in_ZF;
  bool bVar4;
  
  iVar1 = 0x20;
  ppGVar2 = selection->entries;
  do {
    if (iVar1 == 0) break;
    iVar1 = iVar1 + -1;
    in_ZF = entityRuntime == *ppGVar2;
    ppGVar2 = ppGVar2 + 1;
  } while (!(bool)in_ZF);
  if (!(bool)in_ZF) {
    bVar4 = true;
    iVar1 = 0x20;
    ppGVar2 = selection->entries;
    do {
      ppGVar3 = ppGVar2;
      if (iVar1 == 0) break;
      iVar1 = iVar1 + -1;
      ppGVar3 = ppGVar2 + 1;
      bVar4 = *ppGVar2 == (GameEntityRuntime *)0x0;
      ppGVar2 = ppGVar3;
    } while (!bVar4);
    if (bVar4) {
      ppGVar3[-1] = entityRuntime;
    }
  }
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition(selection);
  return in_EAX;
}

/* Address: 0x0052FBF0.
   Ownership: gameplay/selection/runtime.
   Purpose: Averages the positions referenced by up to 32 selection pointers and stores each selection record
   offset relative to that average.
*/
void SelectionPointerArray_RecenterOffsetsAroundAveragePosition(SelectionPointerArray32 *selection)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  GameEntityRuntime **ppGVar6;
  
  iVar2 = 0;
  iVar4 = 0;
  iVar3 = 0;
  iVar5 = 0x20;
  ppGVar6 = selection->entries;
  do {
    if (*ppGVar6 != (GameEntityRuntime *)0x0) {
      iVar1 = *(int *)((int)*ppGVar6 + 4);
      iVar3 = iVar3 + 1;
      iVar2 = iVar2 + *(int *)(iVar1 + 0x94);
      iVar4 = iVar4 + *(int *)(iVar1 + 0x98);
    }
    ppGVar6 = ppGVar6 + 1;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  if (iVar3 != 0) {
    iVar2 = iVar2 / iVar3;
    iVar4 = iVar4 / iVar3;
    iVar3 = 0x20;
    do {
      iVar5 = *(int *)selection;
      if (iVar5 != 0) {
        iVar1 = *(int *)(iVar5 + 4);
        iVar2 = iVar2 - *(int *)(iVar1 + 0x94);
        iVar4 = iVar4 - *(int *)(iVar1 + 0x98);
        *(int *)(iVar5 + 0x60) = iVar2;
        *(int *)(iVar5 + 100) = iVar4;
        iVar2 = iVar2 + *(int *)(iVar1 + 0x94);
        iVar4 = iVar4 + *(int *)(iVar1 + 0x98);
      }
      selection = (SelectionPointerArray32 *)((int)selection + 4);
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  return;
}

/* Address: 0x0052FD90.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans one exact 32-entry pointer array for target. CF clear means found, CF set means absent, and EAX
   is preserved.
*/
undefined4
SelectionPointerArray_ContainsCf(GameEntityRuntime *target,SelectionPointerArray32 *array)

{
  undefined4 in_EAX;
  int entriesRemaining;
  undefined1 in_ZF;
  
  entriesRemaining = 0x20;
  do {
    if (entriesRemaining == 0) break;
    entriesRemaining = entriesRemaining + -1;
    in_ZF = target == array->entries[0];
    array = (SelectionPointerArray32 *)((int)array + 4);
  } while (!(bool)in_ZF);
  if (!(bool)in_ZF) {
    return in_EAX;
  }
  return in_EAX;
}

/* Address: 0x005301F0.
   Ownership: gameplay/selection/runtime.
   Purpose: Scans the fixed 32-entry selection pointer array and computes bounds from entity coordinates +0x60 and
   +0x64. CF is set when either span exceeds 0x5000 or their sum exceeds 0x7000; CF is clear for an empty or
   compact selection. EAX is preserved.
*/
void SelectionPointerArray_IsSpatialSpreadTooLargeCf(SelectionPointerArray32 *selection)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int local_20;
  
  local_20 = 0x20;
  while (iVar5 = *(int *)selection, iVar5 == 0) {
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    local_20 = local_20 + -1;
    if (local_20 == 0) {
      return;
    }
  }
  iVar2 = *(int *)(iVar5 + 0x60);
  iVar4 = *(int *)(iVar5 + 100);
  iVar3 = iVar2;
  iVar5 = iVar4;
  do {
    iVar1 = *(int *)selection;
    if (iVar1 != 0) {
      if (*(int *)(iVar1 + 0x60) < iVar2) {
        iVar2 = *(int *)(iVar1 + 0x60);
      }
      if (*(int *)(iVar1 + 100) < iVar5) {
        iVar5 = *(int *)(iVar1 + 100);
      }
      if (iVar3 < *(int *)(iVar1 + 0x60)) {
        iVar3 = *(int *)(iVar1 + 0x60);
      }
      if (iVar4 < *(int *)(iVar1 + 100)) {
        iVar4 = *(int *)(iVar1 + 100);
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    local_20 = local_20 + -1;
  } while (local_20 != 0);
  if (((iVar3 - iVar2 < 0x5001) && (iVar4 - iVar5 < 0x5001)) &&
     ((iVar3 - iVar2) + (iVar4 - iVar5) < 0x7001)) {
    return;
  }
  return;
}

/* Address: 0x00530650.
   Ownership: gameplay/selection/runtime.
   Purpose: For each nonnull entry in the fixed 32-entry array whose nested type is 0x16, scans the exact 13-dword
   marker-source region, packs matches for three global marker IDs into three bytes, and conditionally writes
   marker bytes plus coordinate triples at +0xB8 through +0xD8 according to lane mask bits 1, 2, and 4. EAX is
   preserved. Typed parameters: p1 valueC→SelectionMarkerCoordinateValue32_V342, p2
   valueB→SelectionMarkerCoordinateValue32_V342, p3 valueA→SelectionMarkerCoordinateValue32_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged. Typed parameters: p0 laneMask→SelectionMarkerLaneMask_V343.
*/
void SelectionPointerArray_ApplyType16MarkerCoordinates
               (SelectionMarkerLaneMask laneMask,SelectionMarkerCoordinateValue32 valueC,
               SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA,
               SelectionPointerArray32 *selection)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar3 = 0x20;
  do {
    if ((*(int **)selection != (int *)0x0) &&
       (piVar1 = (int *)**(int **)selection, *(int *)(*piVar1 + 0x4c) == 0x16)) {
      iVar5 = 0xc;
      iVar4 = 0;
      do {
        iVar2 = piVar1[iVar5 + 0x1e];
        if (iVar2 == g_ArmyLinkedChildAssetIdSlot0) {
          iVar4 = iVar4 + 1;
        }
        if (iVar2 == g_ArmyLinkedChildAssetIdSlot1) {
          iVar4 = iVar4 + 0x100;
        }
        if (iVar2 == g_ArmyLinkedChildAssetIdSlot2) {
          iVar4 = iVar4 + 0x10000;
        }
        iVar5 = iVar5 + -1;
      } while (-1 < iVar5);
      if ((laneMask & 1) != 0) {
        *(char *)(piVar1 + 0x37) = (char)iVar4;
        piVar1[0x2e] = valueA;
        piVar1[0x2f] = valueB;
        piVar1[0x30] = valueC;
      }
      if ((laneMask & 2) != 0) {
        *(char *)((int)piVar1 + 0xdd) = (char)((uint)iVar4 >> 8);
        piVar1[0x31] = valueA;
        piVar1[0x32] = valueB;
        piVar1[0x33] = valueC;
      }
      if ((laneMask & 4) != 0) {
        *(char *)((int)piVar1 + 0xde) = (char)((uint)iVar4 >> 0x10);
        piVar1[0x34] = valueA;
        piVar1[0x35] = valueB;
        piVar1[0x36] = valueC;
      }
    }
    selection = (SelectionPointerArray32 *)((int)selection + 4);
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return;
}

/* Address: 0x0052FB00.
   Ownership: gameplay/selection/runtime.
   Purpose: Handles selection pointer array clear32.
*/
undefined4 SelectionPointerArray_Clear32(SelectionPointerArray32 *array)

{
  undefined4 in_EAX;
  int entriesRemaining;
  
  for (entriesRemaining = 0x20; entriesRemaining != 0; entriesRemaining = entriesRemaining + -1) {
    array->entries[0] = 0;
    array = (SelectionPointerArray32 *)((int)array + 4);
  }
  return in_EAX;
}
