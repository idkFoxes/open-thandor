/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/runtime/core.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/runtime/core.h>

/* Implementation ownership: world/runtime/core. */

/* Address: 0x00532FA0.
   Ownership: world/runtime/core.
   Purpose: Interpolates the level lighting color sets and angular parameters from the current runtime phase,
   applies the resulting terrain-lighting configuration, and refreshes field-region normals and lighting.
   Local calls: WorldRuntime_SetTerrainLightingConfiguration, WorldRuntime_RecomputeFieldRegionNormalsAndLighting.
*/

void __thandor_void_preserve_eax_ecx_edx
WorldLightingRuntime_UpdateInterpolatedTerrainLighting(void)

{
  SoftwareBgraWordLanes SVar1;
  SoftwareBgraWordLanes SVar2;
  PackedArgb32 PVar3;
  PackedArgb32 PVar4;
  PackedArgb32 PVar5;
  InGameLevelConditionStorageView800 *pIVar6;
  byte mm0PackedValue0ByteLane3;
  byte mm0PackedValue0ByteLane1;
  short sVar16;
  byte mm0PackedValue1ByteLane1;
  short sVar20;
  byte mm0PackedValue1ByteLane3;
  short sVar17;
  byte mm0PackedValue2ByteLane1;
  short sVar21;
  byte mm0PackedValue2ByteLane3;
  short sVar18;
  byte mm0PackedValue3ByteLane1;
  short sVar19;
  byte mm0PackedValue0ByteLane2;
  uint uVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  int iVar14;
  WorldRuntimeContext *worldRuntime;
  int iVar15;
  short sVar23;
  short sVar24;
  short sVar27;
  undefined8 mm0PackedValue0;
  byte mm0PackedValue1ByteLane2;
  undefined8 mm0PackedValue1;
  short sVar22;
  byte mm0PackedValue3ByteLane3;
  byte mm0PackedValue2ByteLane2;
  short sVar25;
  undefined8 mm0PackedValue2;
  byte mm0PackedValue3ByteLane2;
  short sVar26;
  undefined8 mm0PackedValue3;
  short sVar31;
  byte mm1PackedValue0ByteLane2;
  byte mm1PackedValue0ByteLane3;
  short sVar28;
  byte mm1PackedValue1ByteLane1;
  short sVar32;
  byte mm1PackedValue1ByteLane3;
  byte mm1PackedValue0ByteLane1;
  short sVar35;
  short sVar36;
  short sVar39;
  undefined8 mm1PackedValue0;
  byte mm1PackedValue1ByteLane2;
  short sVar40;
  undefined8 mm1PackedValue1;
  short sVar29;
  byte mm1PackedValue2ByteLane1;
  short sVar33;
  byte mm1PackedValue2ByteLane3;
  short sVar30;
  byte mm1PackedValue3ByteLane1;
  short sVar34;
  byte mm1PackedValue3ByteLane3;
  byte mm1PackedValue2ByteLane2;
  short sVar37;
  short sVar41;
  undefined8 mm1PackedValue2;
  byte mm1PackedValue3ByteLane2;
  short sVar38;
  short sVar42;
  undefined8 mm1PackedValue3;
  byte mm2PackedValue0ByteLane2;
  byte mm2PackedValue1ByteLane2;
  byte mm2PackedValue2ByteLane2;
  byte mm2PackedValue3ByteLane2;
  byte mm2PackedValue0ByteLane3;
  byte mm2PackedValue0ByteLane1;
  short sVar43;
  undefined8 mm2PackedValue0;
  byte mm2PackedValue1ByteLane1;
  byte mm2PackedValue1ByteLane3;
  undefined8 mm2PackedValue1;
  byte mm2PackedValue2ByteLane1;
  byte mm2PackedValue2ByteLane3;
  byte mm2PackedValue3ByteLane1;
  byte mm2PackedValue3ByteLane3;
  undefined8 mm2PackedValue2;
  undefined8 mm2PackedValue3;
  byte mm3PackedValue0ByteLane2;
  byte mm3PackedValue1ByteLane2;
  byte mm3PackedValue2ByteLane2;
  byte mm3PackedValue3ByteLane2;
  byte mm3PackedValue0ByteLane3;
  byte mm3PackedValue0ByteLane1;
  undefined8 mm3PackedValue0;
  byte mm3PackedValue1ByteLane1;
  byte mm3PackedValue1ByteLane3;
  undefined8 mm3PackedValue1;
  byte mm3PackedValue2ByteLane1;
  byte mm3PackedValue2ByteLane3;
  byte mm3PackedValue3ByteLane1;
  byte mm3PackedValue3ByteLane3;
  undefined8 mm3PackedValue2;
  undefined8 mm3PackedValue3;
  PackedArgb32 worldContext1;
  
  pIVar6 = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  uVar7 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
          terrainLightingCycleDurationTicks;
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  if (uVar7 != 0) {
    uVar7 = (g_GameFactionRuntimeImage.tail.simulationTick % uVar7 << 0x10) / uVar7;
    uVar11 = g_FixedCosQ28[uVar7] + 0x10000000U >> 0x15;
    worldContext1 =
         ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
         terrainBaseColorArgb;
    PVar3 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            terrainRampColor124Argb;
    PVar4 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainBaseColorArgb;
    PVar5 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainRampColor124Argb;
    SVar1 = g_SoftwareBilinearForwardFactors[uVar11];
    SVar2 = g_SoftwareBilinearInverseFactors[uVar11];
    mm0PackedValue0ByteLane3 = (byte)(worldContext1 >> 0x18);
    mm0PackedValue0ByteLane2 = (byte)(worldContext1 >> 0x10);
    mm0PackedValue0ByteLane1 = (byte)(worldContext1 >> 8);
    mm1PackedValue0ByteLane3 = (byte)(PVar3 >> 0x18);
    mm1PackedValue0ByteLane2 = (byte)(PVar3 >> 0x10);
    mm1PackedValue0ByteLane1 = (byte)(PVar3 >> 8);
    mm2PackedValue0ByteLane3 = (byte)(PVar4 >> 0x18);
    mm2PackedValue0ByteLane2 = (byte)(PVar4 >> 0x10);
    mm2PackedValue0ByteLane1 = (byte)(PVar4 >> 8);
    mm3PackedValue0ByteLane3 = (byte)(PVar5 >> 0x18);
    mm3PackedValue0ByteLane2 = (byte)(PVar5 >> 0x10);
    mm3PackedValue0ByteLane1 = (byte)(PVar5 >> 8);
    mm0PackedValue0 =
         pmulhw(CONCAT26(CONCAT11(mm0PackedValue0ByteLane3,mm0PackedValue0ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue0ByteLane3,
                                                                      mm0PackedValue0ByteLane3),
                                                             mm0PackedValue0ByteLane2),
                                                    CONCAT14(mm0PackedValue0ByteLane2,worldContext1)
                                                   ) >> 0x20) >> 6,
                                  CONCAT22(CONCAT11(mm0PackedValue0ByteLane1,
                                                    mm0PackedValue0ByteLane1) >> 6,
                                           CONCAT11((char)worldContext1,(char)worldContext1) >> 6)))
                ,SVar1);
    mm1PackedValue0 =
         pmulhw(CONCAT26(CONCAT11(mm1PackedValue0ByteLane3,mm1PackedValue0ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm1PackedValue0ByteLane3,
                                                                      mm1PackedValue0ByteLane3),
                                                             mm1PackedValue0ByteLane2),
                                                    CONCAT14(mm1PackedValue0ByteLane2,PVar3)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                    mm1PackedValue0ByteLane1) >> 6,
                                           CONCAT11((char)PVar3,(char)PVar3) >> 6))),SVar1);
    mm2PackedValue0 =
         pmulhw(CONCAT26(CONCAT11(mm2PackedValue0ByteLane3,mm2PackedValue0ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm2PackedValue0ByteLane3,
                                                                      mm2PackedValue0ByteLane3),
                                                             mm2PackedValue0ByteLane2),
                                                    CONCAT14(mm2PackedValue0ByteLane2,PVar4)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm2PackedValue0ByteLane1,
                                                    mm2PackedValue0ByteLane1) >> 6,
                                           CONCAT11((char)PVar4,(char)PVar4) >> 6))),SVar2);
    mm3PackedValue0 =
         pmulhw(CONCAT26(CONCAT11(mm3PackedValue0ByteLane3,mm3PackedValue0ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm3PackedValue0ByteLane3,
                                                                      mm3PackedValue0ByteLane3),
                                                             mm3PackedValue0ByteLane2),
                                                    CONCAT14(mm3PackedValue0ByteLane2,PVar5)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm3PackedValue0ByteLane1,
                                                    mm3PackedValue0ByteLane1) >> 6,
                                           CONCAT11((char)PVar5,(char)PVar5) >> 6))),SVar2);
    sVar16 = (short)mm0PackedValue0 + (short)mm2PackedValue0;
    sVar20 = (short)((ulonglong)mm0PackedValue0 >> 0x10) +
             (short)((ulonglong)mm2PackedValue0 >> 0x10);
    sVar24 = (short)((ulonglong)mm0PackedValue0 >> 0x20) +
             (short)((ulonglong)mm2PackedValue0 >> 0x20);
    sVar28 = (short)mm1PackedValue0 + (short)mm3PackedValue0;
    sVar32 = (short)((ulonglong)mm1PackedValue0 >> 0x10) +
             (short)((ulonglong)mm3PackedValue0 >> 0x10);
    sVar36 = (short)((ulonglong)mm1PackedValue0 >> 0x20) +
             (short)((ulonglong)mm3PackedValue0 >> 0x20);
    sVar40 = (short)((ulonglong)mm1PackedValue0 >> 0x30) +
             (short)((ulonglong)mm3PackedValue0 >> 0x30);
    worldContext1 =
         ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
         terrainLightingColor128Argb;
    PVar3 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            terrainRampColor12CArgb;
    PVar4 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainLightingColor128Argb;
    PVar5 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainRampColor12CArgb;
    mm0PackedValue1ByteLane3 = (byte)(worldContext1 >> 0x18);
    mm0PackedValue1ByteLane2 = (byte)(worldContext1 >> 0x10);
    mm0PackedValue1ByteLane1 = (byte)(worldContext1 >> 8);
    mm1PackedValue1ByteLane3 = (byte)(PVar3 >> 0x18);
    mm1PackedValue1ByteLane2 = (byte)(PVar3 >> 0x10);
    mm1PackedValue1ByteLane1 = (byte)(PVar3 >> 8);
    mm2PackedValue1ByteLane3 = (byte)(PVar4 >> 0x18);
    mm2PackedValue1ByteLane2 = (byte)(PVar4 >> 0x10);
    mm2PackedValue1ByteLane1 = (byte)(PVar4 >> 8);
    mm3PackedValue1ByteLane3 = (byte)(PVar5 >> 0x18);
    mm3PackedValue1ByteLane2 = (byte)(PVar5 >> 0x10);
    mm3PackedValue1ByteLane1 = (byte)(PVar5 >> 8);
    mm0PackedValue1 =
         pmulhw(CONCAT26(CONCAT11(mm0PackedValue1ByteLane3,mm0PackedValue1ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue1ByteLane3,
                                                                      mm0PackedValue1ByteLane3),
                                                             mm0PackedValue1ByteLane2),
                                                    CONCAT14(mm0PackedValue1ByteLane2,worldContext1)
                                                   ) >> 0x20) >> 6,
                                  CONCAT22(CONCAT11(mm0PackedValue1ByteLane1,
                                                    mm0PackedValue1ByteLane1) >> 6,
                                           CONCAT11((char)worldContext1,(char)worldContext1) >> 6)))
                ,SVar1);
    mm1PackedValue1 =
         pmulhw(CONCAT26(CONCAT11(mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm1PackedValue1ByteLane3,
                                                                      mm1PackedValue1ByteLane3),
                                                             mm1PackedValue1ByteLane2),
                                                    CONCAT14(mm1PackedValue1ByteLane2,PVar3)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm1PackedValue1ByteLane1,
                                                    mm1PackedValue1ByteLane1) >> 6,
                                           CONCAT11((char)PVar3,(char)PVar3) >> 6))),SVar1);
    mm2PackedValue1 =
         pmulhw(CONCAT26(CONCAT11(mm2PackedValue1ByteLane3,mm2PackedValue1ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm2PackedValue1ByteLane3,
                                                                      mm2PackedValue1ByteLane3),
                                                             mm2PackedValue1ByteLane2),
                                                    CONCAT14(mm2PackedValue1ByteLane2,PVar4)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm2PackedValue1ByteLane1,
                                                    mm2PackedValue1ByteLane1) >> 6,
                                           CONCAT11((char)PVar4,(char)PVar4) >> 6))),SVar2);
    mm3PackedValue1 =
         pmulhw(CONCAT26(CONCAT11(mm3PackedValue1ByteLane3,mm3PackedValue1ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm3PackedValue1ByteLane3,
                                                                      mm3PackedValue1ByteLane3),
                                                             mm3PackedValue1ByteLane2),
                                                    CONCAT14(mm3PackedValue1ByteLane2,PVar5)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm3PackedValue1ByteLane1,
                                                    mm3PackedValue1ByteLane1) >> 6,
                                           CONCAT11((char)PVar5,(char)PVar5) >> 6))),SVar2);
    sVar17 = (short)mm0PackedValue1 + (short)mm2PackedValue1;
    sVar21 = (short)((ulonglong)mm0PackedValue1 >> 0x10) +
             (short)((ulonglong)mm2PackedValue1 >> 0x10);
    sVar25 = (short)((ulonglong)mm0PackedValue1 >> 0x20) +
             (short)((ulonglong)mm2PackedValue1 >> 0x20);
    sVar29 = (short)mm1PackedValue1 + (short)mm3PackedValue1;
    sVar33 = (short)((ulonglong)mm1PackedValue1 >> 0x10) +
             (short)((ulonglong)mm3PackedValue1 >> 0x10);
    sVar37 = (short)((ulonglong)mm1PackedValue1 >> 0x20) +
             (short)((ulonglong)mm3PackedValue1 >> 0x20);
    sVar41 = (short)((ulonglong)mm1PackedValue1 >> 0x30) +
             (short)((ulonglong)mm3PackedValue1 >> 0x30);
    worldContext1 =
         ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
         terrainLightingColor130Argb;
    PVar3 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            terrainLightingColor134Argb;
    PVar4 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainLightingColor130Argb;
    PVar5 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainLightingColor134Argb;
    mm0PackedValue2ByteLane3 = (byte)(worldContext1 >> 0x18);
    mm0PackedValue2ByteLane2 = (byte)(worldContext1 >> 0x10);
    mm0PackedValue2ByteLane1 = (byte)(worldContext1 >> 8);
    mm1PackedValue2ByteLane3 = (byte)(PVar3 >> 0x18);
    mm1PackedValue2ByteLane2 = (byte)(PVar3 >> 0x10);
    mm1PackedValue2ByteLane1 = (byte)(PVar3 >> 8);
    mm2PackedValue2ByteLane3 = (byte)(PVar4 >> 0x18);
    mm2PackedValue2ByteLane2 = (byte)(PVar4 >> 0x10);
    mm2PackedValue2ByteLane1 = (byte)(PVar4 >> 8);
    mm3PackedValue2ByteLane3 = (byte)(PVar5 >> 0x18);
    mm3PackedValue2ByteLane2 = (byte)(PVar5 >> 0x10);
    mm3PackedValue2ByteLane1 = (byte)(PVar5 >> 8);
    mm0PackedValue2 =
         pmulhw(CONCAT26(CONCAT11(mm0PackedValue2ByteLane3,mm0PackedValue2ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue2ByteLane3,
                                                                      mm0PackedValue2ByteLane3),
                                                             mm0PackedValue2ByteLane2),
                                                    CONCAT14(mm0PackedValue2ByteLane2,worldContext1)
                                                   ) >> 0x20) >> 6,
                                  CONCAT22(CONCAT11(mm0PackedValue2ByteLane1,
                                                    mm0PackedValue2ByteLane1) >> 6,
                                           CONCAT11((char)worldContext1,(char)worldContext1) >> 6)))
                ,SVar1);
    mm1PackedValue2 =
         pmulhw(CONCAT26(CONCAT11(mm1PackedValue2ByteLane3,mm1PackedValue2ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm1PackedValue2ByteLane3,
                                                                      mm1PackedValue2ByteLane3),
                                                             mm1PackedValue2ByteLane2),
                                                    CONCAT14(mm1PackedValue2ByteLane2,PVar3)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm1PackedValue2ByteLane1,
                                                    mm1PackedValue2ByteLane1) >> 6,
                                           CONCAT11((char)PVar3,(char)PVar3) >> 6))),SVar1);
    mm2PackedValue2 =
         pmulhw(CONCAT26(CONCAT11(mm2PackedValue2ByteLane3,mm2PackedValue2ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm2PackedValue2ByteLane3,
                                                                      mm2PackedValue2ByteLane3),
                                                             mm2PackedValue2ByteLane2),
                                                    CONCAT14(mm2PackedValue2ByteLane2,PVar4)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm2PackedValue2ByteLane1,
                                                    mm2PackedValue2ByteLane1) >> 6,
                                           CONCAT11((char)PVar4,(char)PVar4) >> 6))),SVar2);
    mm3PackedValue2 =
         pmulhw(CONCAT26(CONCAT11(mm3PackedValue2ByteLane3,mm3PackedValue2ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm3PackedValue2ByteLane3,
                                                                      mm3PackedValue2ByteLane3),
                                                             mm3PackedValue2ByteLane2),
                                                    CONCAT14(mm3PackedValue2ByteLane2,PVar5)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm3PackedValue2ByteLane1,
                                                    mm3PackedValue2ByteLane1) >> 6,
                                           CONCAT11((char)PVar5,(char)PVar5) >> 6))),SVar2);
    sVar18 = (short)mm0PackedValue2 + (short)mm2PackedValue2;
    sVar22 = (short)((ulonglong)mm0PackedValue2 >> 0x10) +
             (short)((ulonglong)mm2PackedValue2 >> 0x10);
    sVar26 = (short)((ulonglong)mm0PackedValue2 >> 0x20) +
             (short)((ulonglong)mm2PackedValue2 >> 0x20);
    sVar30 = (short)mm1PackedValue2 + (short)mm3PackedValue2;
    sVar34 = (short)((ulonglong)mm1PackedValue2 >> 0x10) +
             (short)((ulonglong)mm3PackedValue2 >> 0x10);
    sVar38 = (short)((ulonglong)mm1PackedValue2 >> 0x20) +
             (short)((ulonglong)mm3PackedValue2 >> 0x20);
    sVar42 = (short)((ulonglong)mm1PackedValue2 >> 0x30) +
             (short)((ulonglong)mm3PackedValue2 >> 0x30);
    worldContext1 =
         ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
         terrainLightingColor138Argb;
    PVar3 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            terrainLightingColor13CArgb;
    PVar4 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainLightingColor138Argb;
    PVar5 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
            alternateTerrainLightingColor13CArgb;
    mm0PackedValue3ByteLane3 = (byte)(worldContext1 >> 0x18);
    mm0PackedValue3ByteLane2 = (byte)(worldContext1 >> 0x10);
    mm0PackedValue3ByteLane1 = (byte)(worldContext1 >> 8);
    mm1PackedValue3ByteLane3 = (byte)(PVar3 >> 0x18);
    mm1PackedValue3ByteLane2 = (byte)(PVar3 >> 0x10);
    mm1PackedValue3ByteLane1 = (byte)(PVar3 >> 8);
    mm2PackedValue3ByteLane3 = (byte)(PVar4 >> 0x18);
    mm2PackedValue3ByteLane2 = (byte)(PVar4 >> 0x10);
    mm2PackedValue3ByteLane1 = (byte)(PVar4 >> 8);
    mm3PackedValue3ByteLane3 = (byte)(PVar5 >> 0x18);
    mm3PackedValue3ByteLane2 = (byte)(PVar5 >> 0x10);
    mm3PackedValue3ByteLane1 = (byte)(PVar5 >> 8);
    mm0PackedValue3 =
         pmulhw(CONCAT26(CONCAT11(mm0PackedValue3ByteLane3,mm0PackedValue3ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue3ByteLane3,
                                                                      mm0PackedValue3ByteLane3),
                                                             mm0PackedValue3ByteLane2),
                                                    CONCAT14(mm0PackedValue3ByteLane2,worldContext1)
                                                   ) >> 0x20) >> 6,
                                  CONCAT22(CONCAT11(mm0PackedValue3ByteLane1,
                                                    mm0PackedValue3ByteLane1) >> 6,
                                           CONCAT11((char)worldContext1,(char)worldContext1) >> 6)))
                ,SVar1);
    mm1PackedValue3 =
         pmulhw(CONCAT26(CONCAT11(mm1PackedValue3ByteLane3,mm1PackedValue3ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm1PackedValue3ByteLane3,
                                                                      mm1PackedValue3ByteLane3),
                                                             mm1PackedValue3ByteLane2),
                                                    CONCAT14(mm1PackedValue3ByteLane2,PVar3)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm1PackedValue3ByteLane1,
                                                    mm1PackedValue3ByteLane1) >> 6,
                                           CONCAT11((char)PVar3,(char)PVar3) >> 6))),SVar1);
    mm2PackedValue3 =
         pmulhw(CONCAT26(CONCAT11(mm2PackedValue3ByteLane3,mm2PackedValue3ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm2PackedValue3ByteLane3,
                                                                      mm2PackedValue3ByteLane3),
                                                             mm2PackedValue3ByteLane2),
                                                    CONCAT14(mm2PackedValue3ByteLane2,PVar4)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm2PackedValue3ByteLane1,
                                                    mm2PackedValue3ByteLane1) >> 6,
                                           CONCAT11((char)PVar4,(char)PVar4) >> 6))),SVar2);
    mm3PackedValue3 =
         pmulhw(CONCAT26(CONCAT11(mm3PackedValue3ByteLane3,mm3PackedValue3ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm3PackedValue3ByteLane3,
                                                                      mm3PackedValue3ByteLane3),
                                                             mm3PackedValue3ByteLane2),
                                                    CONCAT14(mm3PackedValue3ByteLane2,PVar5)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm3PackedValue3ByteLane1,
                                                    mm3PackedValue3ByteLane1) >> 6,
                                           CONCAT11((char)PVar5,(char)PVar5) >> 6))),SVar2);
    sVar19 = (short)mm0PackedValue3 + (short)mm2PackedValue3;
    sVar23 = (short)((ulonglong)mm0PackedValue3 >> 0x10) +
             (short)((ulonglong)mm2PackedValue3 >> 0x10);
    sVar27 = (short)((ulonglong)mm0PackedValue3 >> 0x20) +
             (short)((ulonglong)mm2PackedValue3 >> 0x20);
    sVar31 = (short)mm1PackedValue3 + (short)mm3PackedValue3;
    sVar35 = (short)((ulonglong)mm1PackedValue3 >> 0x10) +
             (short)((ulonglong)mm3PackedValue3 >> 0x10);
    sVar39 = (short)((ulonglong)mm1PackedValue3 >> 0x20) +
             (short)((ulonglong)mm3PackedValue3 >> 0x20);
    sVar43 = (short)((ulonglong)mm1PackedValue3 >> 0x30) +
             (short)((ulonglong)mm3PackedValue3 >> 0x30);
    WorldRuntime_SetTerrainLightingConfiguration
              (CONCAT13((0 < sVar43) * (sVar43 < 0x100) * (char)sVar43 - (0xff < sVar43),
                        CONCAT12((0 < sVar39) * (sVar39 < 0x100) * (char)sVar39 - (0xff < sVar39),
                                 CONCAT11((0 < sVar35) * (sVar35 < 0x100) * (char)sVar35 -
                                          (0xff < sVar35),
                                          (0 < sVar31) * (sVar31 < 0x100) * (char)sVar31 -
                                          (0xff < sVar31)))) | 0xff000000,
               (uint)CONCAT12((0 < sVar27) * (sVar27 < 0x100) * (char)sVar27 - (0xff < sVar27),
                              CONCAT11((0 < sVar23) * (sVar23 < 0x100) * (char)sVar23 -
                                       (0xff < sVar23),
                                       (0 < sVar19) * (sVar19 < 0x100) * (char)sVar19 -
                                       (0xff < sVar19))),
               CONCAT13((0 < sVar42) * (sVar42 < 0x100) * (char)sVar42 - (0xff < sVar42),
                        CONCAT12((0 < sVar38) * (sVar38 < 0x100) * (char)sVar38 - (0xff < sVar38),
                                 CONCAT11((0 < sVar34) * (sVar34 < 0x100) * (char)sVar34 -
                                          (0xff < sVar34),
                                          (0 < sVar30) * (sVar30 < 0x100) * (char)sVar30 -
                                          (0xff < sVar30)))) | 0xff000000,
               (uint)CONCAT12((0 < sVar26) * (sVar26 < 0x100) * (char)sVar26 - (0xff < sVar26),
                              CONCAT11((0 < sVar22) * (sVar22 < 0x100) * (char)sVar22 -
                                       (0xff < sVar22),
                                       (0 < sVar18) * (sVar18 < 0x100) * (char)sVar18 -
                                       (0xff < sVar18))),
               CONCAT13((0 < sVar41) * (sVar41 < 0x100) * (char)sVar41 - (0xff < sVar41),
                        CONCAT12((0 < sVar37) * (sVar37 < 0x100) * (char)sVar37 - (0xff < sVar37),
                                 CONCAT11((0 < sVar33) * (sVar33 < 0x100) * (char)sVar33 -
                                          (0xff < sVar33),
                                          (0 < sVar29) * (sVar29 < 0x100) * (char)sVar29 -
                                          (0xff < sVar29)))) |
               ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->levelImage).runtimeTail2E0.
               terrainRampColor12CArgb & 0xff000000,
               (uint)CONCAT12((0 < sVar25) * (sVar25 < 0x100) * (char)sVar25 - (0xff < sVar25),
                              CONCAT11((0 < sVar21) * (sVar21 < 0x100) * (char)sVar21 -
                                       (0xff < sVar21),
                                       (0 < sVar17) * (sVar17 < 0x100) * (char)sVar17 -
                                       (0xff < sVar17))),
               CONCAT13((0 < sVar40) * (sVar40 < 0x100) * (char)sVar40 - (0xff < sVar40),
                        CONCAT12((0 < sVar36) * (sVar36 < 0x100) * (char)sVar36 - (0xff < sVar36),
                                 CONCAT11((0 < sVar32) * (sVar32 < 0x100) * (char)sVar32 -
                                          (0xff < sVar32),
                                          (0 < sVar28) * (sVar28 < 0x100) * (char)sVar28 -
                                          (0xff < sVar28)))) | 0xff000000,
               (uint)CONCAT12((0 < sVar24) * (sVar24 < 0x100) * (char)sVar24 - (0xff < sVar24),
                              CONCAT11((0 < sVar20) * (sVar20 < 0x100) * (char)sVar20 -
                                       (0xff < sVar20),
                                       (0 < sVar16) * (sVar16 < 0x100) * (char)sVar16 -
                                       (0xff < sVar16))),worldRuntime);
    uVar10 = uVar7 >> 8;
    uVar11 = (uint)(ushort)(pIVar6->levelImage).runtimeTail2E0.packedFieldRegionOriginYHigh16XLow16;
    uVar12 = (uint)(ushort)(pIVar6->levelImage).runtimeTail2E0.
                           alternatePackedFieldRegionOriginYHigh16XLow16;
    if (uVar10 < 0x80) {
      if (uVar12 < uVar11) {
        uVar12 = uVar12 + 0x10000;
      }
      iVar13 = uVar12 * uVar10;
      iVar8 = uVar11 * (0x80 - uVar10);
    }
    else {
      if (uVar11 < uVar12) {
        uVar11 = uVar11 + 0x10000;
      }
      iVar8 = uVar11 * (uVar10 - 0x80);
      iVar13 = uVar12 * (0x80 - (uVar10 - 0x80));
    }
    uVar12 = g_FixedCosQ28[uVar7] + 0x10000000U >> 0x15;
    iVar15 = 0x100 - uVar12;
    uVar11 = (uint)(ushort)(pIVar6->levelImage).runtimeTail2E0.
                           packedFieldRegionHeightHigh16WidthLow16;
    uVar7 = uVar7 >> 8;
    uVar10 = (uint)(ushort)(pIVar6->levelImage).runtimeTail2E0.
                           alternatePackedFieldRegionHeightHigh16WidthLow16;
    if (uVar7 < 0x80) {
      if (uVar10 < uVar11) {
        uVar10 = uVar10 + 0x10000;
      }
      iVar14 = uVar10 * uVar7;
      iVar9 = uVar11 * (0x80 - uVar7);
    }
    else {
      if (uVar11 < uVar10) {
        uVar11 = uVar11 + 0x10000;
      }
      iVar9 = uVar11 * (uVar7 - 0x80);
      iVar14 = uVar10 * (0x80 - (uVar7 - 0x80));
    }
    WorldRuntime_RecomputeFieldRegionNormalsAndLighting
              ((int)((uint)*(ushort *)
                            ((int)&(pIVar6->levelImage).runtimeTail2E0.
                                   alternatePackedFieldRegionHeightHigh16WidthLow16 + 2) * iVar15 +
                    (uint)*(ushort *)
                           ((int)&(pIVar6->levelImage).runtimeTail2E0.
                                  packedFieldRegionHeightHigh16WidthLow16 + 2) * (0x100 - iVar15))
               >> 8,(uint)(iVar9 + iVar14) >> 7 & 0xffff,
               (int)((uint)*(ushort *)
                            ((int)&(pIVar6->levelImage).runtimeTail2E0.
                                   alternatePackedFieldRegionOriginYHigh16XLow16 + 2) * iVar15 +
                    *(ushort *)
                     ((int)&(pIVar6->levelImage).runtimeTail2E0.packedFieldRegionOriginYHigh16XLow16
                     + 2) * uVar12) >> 8,(uint)(iVar8 + iVar13) >> 7 & 0xffff,worldRuntime);
  }
  return;
}


/* Address: 0x0050D100.
   Ownership: world/runtime/core.
   Purpose: Stores a three-component position at +0x60 through +0x68, computes its exact fixed-point distance from
   +0x80 through +0x88, publishes the result at +0x7C and +0x8C, and clears the field-grid dirty state.
   Local calls: WorldRuntime_ClearFieldGridDirtyFlag.
   Cross-module calls: FixedMath_Length3 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetPosition60AndDistanceFromPosition80
          (Q12 positionZ,Q12 positionY,Q12 positionX,WorldRuntimeContext *runtime)

{
  dword targetDistanceQ12;
  
  (runtime->motion).positionXQ12 = positionX;
  (runtime->motion).positionYQ12 = positionY;
  (runtime->motion).positionZQ12 = positionZ;
  targetDistanceQ12 =
       FixedMath_Length3(positionZ - (runtime->motion).targetPositionZQ12,
                         positionY - (runtime->motion).targetPositionYQ12,
                         positionX - (runtime->motion).targetPositionXQ12);
  (runtime->motion).targetDistanceQ12 = targetDistanceQ12;
  (runtime->motion).committedDistanceQ12 = targetDistanceQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(runtime);
  return;
}


/* Address: 0x0050D150.
   Ownership: world/runtime/core.
   Purpose: Stores motion fields +0x6C through +0x78 after enforcing magnitude >=0x400, masking the heading to 16
   bits, and clamping the signed pitch to runtime limits and then to plus or minus 0x4000. Typed parameters: p0
   value78→WorldMotionValue78_V344. Calling convention, complete VariableStorage serialization, function bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Local calls: WorldRuntime_ClearFieldGridDirtyFlag.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetMotionParameters6CThrough78Clamped
          (WorldMotionValue78 value78,AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 magnitude
          ,WorldRuntimeContext *runtime)

{
  if ((runtime->runtimeFlags & 0x40000) == 0) {
    if ((int)(runtime->motion).maximumPitchAngle < (int)pitchAngle) {
      pitchAngle = (runtime->motion).maximumPitchAngle;
    }
    else if ((int)pitchAngle < (int)(runtime->motion).minimumPitchAngle) {
      pitchAngle = (runtime->motion).minimumPitchAngle;
    }
  }
  if ((int)magnitude < 0x400) {
    magnitude = 0x400;
  }
  if ((int)pitchAngle < 0x4001) {
    if ((int)pitchAngle < -0x4000) {
      pitchAngle = 0xffffc000;
    }
  }
  else {
    pitchAngle = 0x4000;
  }
  (runtime->motion).positionMagnitudeQ12 = magnitude;
  (runtime->motion).headingAngle = headingAngle & 0xffff;
  (runtime->motion).pitchAngle = pitchAngle;
  (runtime->motion).motionValue78 = value78;
  WorldRuntime_ClearFieldGridDirtyFlag(runtime);
  return;
}


/* Address: 0x0050D1E0.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime set position80 and rebuild position60 from angles.
   Local calls: WorldRuntime_ClearFieldGridDirtyFlag.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
          (AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 distance,Q12 originZ,Q12 originY,
          Q12 originX,WorldRuntimeContext *runtime)

{
  FixedDirectionXZEdxEax8 positionOffsetXZQ12;
  FixedDirectionXyzRegs12 FVar1;
  
  (runtime->motion).targetPositionXQ12 = originX;
  (runtime->motion).targetPositionYQ12 = originY;
  (runtime->motion).targetPositionZQ12 = originZ;
  (runtime->motion).pitchAngle = pitchAngle;
  (runtime->motion).headingAngle = headingAngle;
  (runtime->motion).targetDistanceQ12 = distance;
  (runtime->motion).committedDistanceQ12 = distance;
  FVar1 = FixedMath_DirectionFromAnglesScaledRegs(-pitchAngle,headingAngle ^ 0x8000,distance);
  (runtime->motion).positionXQ12 = FVar1.eax + (runtime->motion).targetPositionXQ12;
  (runtime->motion).positionYQ12 = FVar1.ecx + (runtime->motion).targetPositionYQ12;
  (runtime->motion).positionZQ12 = FVar1.edx + (runtime->motion).targetPositionZQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(runtime);
  return;
}


/* Address: 0x0050D2C0.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime restore motion state from snapshot.
   Local calls: WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_RestoreMotionStateFromSnapshot(WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 AVar1;
  AngleTurn32 AVar2;
  Q12 snapshotPositionYQ12;
  Q12 snapshotPositionZQ12;
  UQ12 snapshotDistanceQ12;
  
  snapshotPositionYQ12 = (worldRuntime->snapshot).positionYQ12;
  snapshotPositionZQ12 = (worldRuntime->snapshot).positionZQ12;
  (worldRuntime->motion).positionXQ12 = (worldRuntime->snapshot).positionXQ12;
  (worldRuntime->motion).positionYQ12 = snapshotPositionYQ12;
  (worldRuntime->motion).positionZQ12 = snapshotPositionZQ12;
  AVar1 = (worldRuntime->snapshot).headingAngle;
  AVar2 = (worldRuntime->snapshot).pitchAngle;
  snapshotDistanceQ12 = (worldRuntime->snapshot).distanceQ12;
  (worldRuntime->motion).positionMagnitudeQ12 = (worldRuntime->snapshot).magnitudeQ12;
  (worldRuntime->motion).headingAngle = AVar1;
  (worldRuntime->motion).pitchAngle = AVar2;
  (worldRuntime->motion).targetDistanceQ12 = snapshotDistanceQ12;
  (worldRuntime->motion).committedDistanceQ12 = snapshotDistanceQ12;
  WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  return;
}


/* Address: 0x0050D670.
   Ownership: world/runtime/core.
   Purpose: Accepts only an asset whose first dword is the little-endian fld signature. A valid asset is stored at
   context offset 0x54, prepared through the existing field helper, and followed by clearing context flag
   0x00000800.
   Local calls: WorldRuntime_ClearFieldGridDirtyFlag.
   Cross-module calls: FieldGrid_RecomputeInteriorTriangleNormalAngles [world/terrain/grid].
*/
void __thandor_preserve_eax
WorldRuntime_AttachFieldGridAsset(FieldGridAsset *asset,WorldRuntimeContext *world)

{
  if ((asset->common).magic == ASSET_MAGIC_FLD) {
    world->fieldGrid = asset;
    FieldGrid_RecomputeInteriorTriangleNormalAngles(asset);
    WorldRuntime_ClearFieldGridDirtyFlag(world);
  }
  return;
}


/* Address: 0x00561E30.
   Ownership: world/runtime/core.
   Purpose: Adjusts the active field origin by signed Y and X deltas, wraps X to sixteen bits, clamps Y to the
   verified range -0x4000 through -0x1000, and reapplies the world-runtime origin state. Typed parameters: p4
   deltaWorldY→Q12, p5 deltaWorldX→Q12. Calling convention, complete VariableStorage serialization, function bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Local calls: WorldRuntime_RecomputeFieldRegionNormalsAndLighting.
*/
void __thandor_preserve_eax_edx
WorldRuntime_AdjustFieldOriginWrappedClamped
          (PlayerRuntimeId playerRuntimeId,dword reservedZero,Q12 deltaWorldY,Q12 deltaWorldX)

{
  FieldGridDimensionCells gridHeight;
  
  gridHeight = deltaWorldY + (g_InGameRuntimeRoot->worldRuntime0A30).fieldRegion.regionHeight;
  if (-0x1000 < gridHeight) {
    gridHeight = -0x1000;
  }
  if (gridHeight < -0x4000) {
    gridHeight = -0x4000;
  }
  WorldRuntime_RecomputeFieldRegionNormalsAndLighting
            (gridHeight,
             deltaWorldX + (g_InGameRuntimeRoot->worldRuntime0A30).fieldRegion.regionWidth & 0xffff,
             g_InGameRuntimeRoot->fieldRegionOriginWorldYQ12_0BAC,
             g_InGameRuntimeRoot->fieldRegionOriginWorldXQ12_0BA8,
             &g_InGameRuntimeRoot->worldRuntime0A30);
  return;
}


/* Address: 0x004BE760.
   Ownership: world/runtime/core.
   Purpose: Semantic ABI remains deferred.
   Cross-module calls: FieldGrid_InterpolateTerrainHeight [world/terrain/grid].
*/
Q12 WorldRuntime_InterpolateTerrainHeightOrSentinel
              (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  Q12 QVar1;
  FieldGridHeightEaxCf5 FVar2;
  
  QVar1 = 0x7ffff000;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    FVar2 = FieldGrid_InterpolateTerrainHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    QVar1 = FVar2.heightQ12;
  }
  return QVar1;
}


/* Address: 0x004BE790.
   Ownership: world/runtime/core.
   Purpose: Semantic ABI remains deferred.
   Cross-module calls: FieldGrid_InterpolateWaterSurfaceHeight [world/terrain/grid].
*/
Q12 WorldRuntime_InterpolateWaterSurfaceHeightOrSentinel
              (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  Q12 QVar1;
  FieldGridHeightEaxCf5 FVar2;
  
  QVar1 = 0x7ffff000;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    FVar2 = FieldGrid_InterpolateWaterSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    QVar1 = FVar2.heightQ12;
  }
  return QVar1;
}


/* Address: 0x004BE7C0.
   Ownership: world/runtime/core.
   Purpose: Returns the interpolated field-grid top surface height or the 0x7FFFF000 sentinel when the runtime has
   no field grid.
   Cross-module calls: FieldGrid_InterpolateTopSurfaceHeight [world/terrain/grid].
*/
dword WorldRuntime_InterpolateTopSurfaceHeightOrSentinel
                (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  dword topSurfaceHeightQ12;
  FieldGridHeightEaxCf5 FVar1;
  
  topSurfaceHeightQ12 = 0x7ffff000;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    FVar1 = FieldGrid_InterpolateTopSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    topSurfaceHeightQ12 = FVar1.heightQ12;
  }
  return topSurfaceHeightQ12;
}


/* Address: 0x0050A610.
   Ownership: world/runtime/core.
   Purpose: Transforms one runtime-node position to integer grid coordinates and returns carry set only when both
   coordinates lie inside the four inclusive bounds stored at offsets 0x160-0x16C. Typed parameters: p3
   boundsControl→WorldRuntimeExtendedMapControlAddress32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
WorldRuntimeNode_IsPositionInsideBoundsCf
          (WorldOwnerListNode100 *runtimeNode,WorldRuntimeExtendedMapControlView170 *boundsControl)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int boundsMaxX;
  int iVar4;
  int boundsMinX;
  int boundsMaxY;
  int boundsMinY;
  GraphicsProjectedPointEdxEax8 projectedPositionPair;
  
  FixedTransform_ApplyPoint
            (&g_GraphicsProjectionScratchVec3,(GraphicsFixedVec3 *)&runtimeNode->worldXQ12,
             &g_ViewProjectionMatrixFixed);
  projectedPositionPair =
       (GraphicsProjectedPointEdxEax8)Graphics_ProjectViewPoint(&g_GraphicsProjectionScratchVec3);
  boundsMinX = boundsControl->extendedCoordinate160;
  iVar1 = boundsControl->extendedCoordinate168;
  boundsMinY = boundsControl->extendedCoordinate164;
  iVar2 = boundsControl->extendedCoordinate16C;
  iVar3 = (int)projectedPositionPair >> 0xc;
  iVar4 = (int)((longlong)projectedPositionPair >> 0x2c);
  boundsMaxX = iVar1;
  if (iVar1 < boundsMinX) {
    boundsMaxX = boundsMinX;
    boundsMinX = iVar1;
  }
  boundsMaxY = iVar2;
  if (iVar2 < boundsMinY) {
    boundsMaxY = boundsMinY;
    boundsMinY = iVar2;
  }
  if ((((boundsMinX <= iVar3) && (iVar3 <= boundsMaxX)) && (boundsMinY <= iVar4)) &&
     (iVar4 <= boundsMaxY)) {
    return true;
  }
  return false;
}


/* Address: 0x0050D260.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime capture motion state to snapshot.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_CaptureMotionStateToSnapshot(WorldRuntimeContext *worldRuntime)

{
  UQ12 snapshotDistanceQ12;
  Q12 snapshotPositionYQ12;
  Q12 snapshotPositionZQ12;
  AngleTurn32 snapshotHeadingAngle;
  AngleTurn32 snapshotPitchAngle;
  
  snapshotPositionYQ12 = (worldRuntime->motion).positionYQ12;
  snapshotPositionZQ12 = (worldRuntime->motion).positionZQ12;
  (worldRuntime->snapshot).positionXQ12 = (worldRuntime->motion).positionXQ12;
  (worldRuntime->snapshot).positionYQ12 = snapshotPositionYQ12;
  (worldRuntime->snapshot).positionZQ12 = snapshotPositionZQ12;
  snapshotHeadingAngle = (worldRuntime->motion).headingAngle;
  snapshotPitchAngle = (worldRuntime->motion).pitchAngle;
  snapshotDistanceQ12 = (worldRuntime->motion).committedDistanceQ12;
  (worldRuntime->snapshot).magnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
  (worldRuntime->snapshot).headingAngle = snapshotHeadingAngle;
  (worldRuntime->snapshot).pitchAngle = snapshotPitchAngle;
  (worldRuntime->snapshot).distanceQ12 = snapshotDistanceQ12;
  return;
}


/* Address: 0x0050D330.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime motion state matches snapshot carry-flag result.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_MotionStateMatchesSnapshotCf(WorldRuntimeContext *worldRuntime)

{
  if ((((worldRuntime->motion).positionXQ12 == (worldRuntime->snapshot).positionXQ12) &&
      ((worldRuntime->motion).positionYQ12 == (worldRuntime->snapshot).positionYQ12)) &&
     ((worldRuntime->motion).positionZQ12 == (worldRuntime->snapshot).positionZQ12)) {
    if ((((worldRuntime->motion).positionMagnitudeQ12 == (worldRuntime->snapshot).magnitudeQ12) &&
        ((worldRuntime->motion).headingAngle == (worldRuntime->snapshot).headingAngle)) &&
       (((worldRuntime->motion).pitchAngle == (worldRuntime->snapshot).pitchAngle &&
        ((worldRuntime->motion).targetDistanceQ12 == (worldRuntime->snapshot).distanceQ12)))) {
      return;
    }
  }
  return;
}


/* Address: 0x0050D4F0.
   Ownership: world/runtime/core.
   Purpose: Copies the dword at context offset 0x8C into offset 0x7C. The surrounding world-runtime layout remains
   opaque.
*/
void __thandor_preserve_eax WorldRuntime_CommitScalar7CFrom8C(WorldRuntimeContext *world)

{
  (world->motion).committedDistanceQ12 = (world->motion).targetDistanceQ12;
  return;
}


/* Address: 0x0050D510.
   Ownership: world/runtime/core.
   Purpose: Stores arrayBase at context offset 0x58 and count at offset 0xAC. Verified callers attach arrays
   containing 0x100 or 0x4000 entries.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_AttachObjectArray
          (WorldObjectRecordCount count,WorldObjectRecord *objectArray,WorldRuntimeContext *world)

{
  world->objectArray = objectArray;
  world->objectCount = count;
  return;
}


/* Address: 0x0050D540.
   Ownership: world/runtime/core.
   Purpose: Replaces the dword at context offset 0xCC with flags. Typed parameters: p0 flags→WorldRuntimeFlags.
   Nearby but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  world->runtimeControlFlags = flags;
  return;
}


/* Address: 0x0050D560.
   Ownership: world/runtime/core.
   Purpose: ORs flags into the dword at context offset 0xCC. Typed parameters: p0 flags→WorldRuntimeFlags. Nearby
   but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_AddFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  world->runtimeControlFlags = world->runtimeControlFlags | flags;
  return;
}


/* Address: 0x0050D580.
   Ownership: world/runtime/core.
   Purpose: Clears every bit selected by flags from the dword at context offset 0xCC. Typed parameters: p0
   flags→WorldRuntimeFlags. Nearby but non-identical semantic domains were explicitly deferred. Calling convention,
   parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_ClearFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  world->runtimeControlFlags = world->runtimeControlFlags & ~flags;
  return;
}


/* Address: 0x0050D5A0.
   Ownership: world/runtime/core.
   Purpose: XORs flags into the dword at context offset 0xCC. Typed parameters: p0 flags→WorldRuntimeFlags. Nearby
   but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_ToggleFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  world->runtimeControlFlags = world->runtimeControlFlags ^ flags;
  return;
}


/* Address: 0x0050D610.
   Ownership: world/runtime/core.
   Purpose: Returns the three dwords at context offsets 0x60, 0x64, and 0x68 through EAX, ECX, and EDX
   respectively. The three-register return cannot be represented by an ordinary C prototype.
*/
WorldVector0EaxEcxEdx12 WorldRuntime_GetVector0Regs(WorldRuntimeContext *world)

{
  WorldVector0EaxEcxEdx12 WVar1;
  undefined8 uVar2;
  
  WVar1.xQ12 = (world->motion).positionXQ12;
  uVar2._4_4_ = (world->motion).positionYQ12;
  register0x00000008 = (world->motion).positionZQ12;
  return WVar1;
}


/* Address: 0x0050D630.
   Ownership: world/runtime/core.
   Purpose: Returns the three dwords at context offsets 0x6C, 0x70, and 0x74 through EAX, ECX, and EDX
   respectively. The three-register return cannot be represented by an ordinary C prototype.
*/
WorldVector1EaxEcxEdx12 WorldRuntime_GetVector1Regs(WorldRuntimeContext *world)

{
  WorldVector1EaxEcxEdx12 WVar1;
  undefined8 uVar2;
  
  WVar1.magnitudeQ12 = (world->motion).positionMagnitudeQ12;
  uVar2._4_4_ = (world->motion).headingAngle;
  register0x00000008 = (world->motion).pitchAngle;
  return WVar1;
}


/* Address: 0x0050D650.
   Ownership: world/runtime/core.
   Purpose: Returns the dword at context offset 0xCC in EAX and explicitly clears CF.
*/
WorldRuntimeFlagsEaxCf5 __thandor_eax_cf_preserve_ecx_edx
WorldRuntime_GetFlagsCf(WorldRuntimeContext *world)

{
  WorldRuntimeFlagsEaxCf5 WVar1;
  
  WVar1.carry = false;
  WVar1.flags = world->runtimeControlFlags;
  return WVar1;
}


/* Address: 0x0050D6A0.
   Ownership: world/runtime/core.
   Purpose: Returns the FieldGridAsset pointer stored at context offset 0x54.
*/
FieldGridAsset * WorldRuntime_GetFieldGridAsset(WorldRuntimeContext *world)

{
  return world->fieldGrid;
}

/* Address: 0x0050D6D0.
   Ownership: world/runtime/core.
   Purpose: Returns the dword at context offset 0x5C without modifying it.
*/
dword WorldRuntime_GetPendingToken(WorldRuntimeContext *world)

{
  return world->pendingToken;
}

/* Address: 0x0050D6F0.
   Ownership: world/runtime/core.
   Purpose: Atomically exchanges the dword at context offset 0x5C with zero and returns the previous value in EAX.
*/
dword WorldRuntime_TakePendingToken(WorldRuntimeContext *world)

{
  dword pendingToken;
  
  LOCK();
  pendingToken = world->pendingToken;
  world->pendingToken = 0;
  UNLOCK();
  return pendingToken;
}

/* Address: 0x0050D710.
   Ownership: world/runtime/core.
   Purpose: Stores array at context offset 0xC0 and count at 0xC4, then clears exactly count dwords beginning at
   array.
*/
void __thandor_void_preserve_eax_ecx
WorldRuntime_AttachAndClearDwordArray
          (WorldWorkspaceElementCount count,dword *array,WorldRuntimeContext *world)

{
  world->dwordArray = array;
  world->dwordArrayCount = count;
  for (; count != 0; count = count - 1) {
    *array = 0;
    array = array + 1;
  }
  return;
}


/* Address: 0x0050D740.
   Ownership: world/runtime/core.
   Purpose: Returns the dword-array pointer stored at context offset 0xC0.
*/
dword * WorldRuntime_GetDwordArray(WorldRuntimeContext *world)

{
  return world->dwordArray;
}

/* Address: 0x0050D7D0.
   Ownership: world/runtime/core.
   Purpose: Scans the attached fixed-size 0x100-byte object records for a slot without allocation bit 0x40000000,
   marks the selected slot, stores its owning world runtime, and reports exhaustion or success through carry.
*/
WorldObjectRecordEaxCf5 __thandor_eax_cf_preserve_ecx_edx
WorldObjectArray_AllocateFreeRecordCf(WorldRuntimeContext *worldRuntime)

{
  WorldObjectRecordCount recordsRemaining;
  WorldObjectRecord *recordCursor;
  WorldObjectRecordEaxCf5 WVar1;
  WorldObjectRecordEaxCf5 WVar2;
  
  recordsRemaining = worldRuntime->objectCount;
  recordCursor = worldRuntime->objectArray;
  while( true ) {
    if (recordsRemaining == 0) {
      WVar1.carry = true;
      WVar1.recordOrError = (WorldObjectRecord *)0x14;
      return WVar1;
    }
    if (((recordCursor->common).allocationFlags & 0x40000000) == 0) break;
    recordCursor = recordCursor + 1;
    recordsRemaining = recordsRemaining - 1;
  }
  (recordCursor->common).allocationFlags = 0x40000000;
  (recordCursor->common).ownerWorld = worldRuntime;
  WVar2.carry = false;
  WVar2.recordOrError = recordCursor;
  return WVar2;
}


/* Address: 0x0050D830.
   Ownership: world/runtime/core.
   Purpose: Sets runtime flag 0x80000000 and atomically inserts the node at the head pointer stored at owner +0xD8,
   maintaining previous and next links at node +0x00 and +0x04.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_LinkNodeIntoOwnerListD8(WorldOwnerListNode100 *node)

{
  WorldOwnerListNode100 **ownerListHeadLink;
  WorldOwnerListNode100 *previousHeadNode;
  WorldRuntimeContext *ownerWorld;
  
  ownerWorld = node->ownerWorld;
  node->runtimeFlags = node->runtimeFlags | 0x80000000;
  LOCK();
  ownerListHeadLink = &ownerWorld->ownerListHead;
  previousHeadNode = *ownerListHeadLink;
  *ownerListHeadLink = node;
  UNLOCK();
  node->previousNode = (WorldOwnerListNode100 *)0x0;
  node->nextNode = previousHeadNode;
  if (previousHeadNode != (WorldOwnerListNode100 *)0x0) {
    previousHeadNode->previousNode = node;
  }
  return;
}


/* Address: 0x0050D880.
   Ownership: world/runtime/core.
   Purpose: When linked, removes the node from the owner +0xD8 intrusive list, repairs both neighbors or the head
   pointer, then clears the complete runtime flag dword at +0x4C.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_UnlinkNodeFromOwnerListD8(WorldOwnerListNode100 *node)

{
  WorldOwnerListNode100 *previousNode;
  WorldOwnerListNode100 *nextNode;
  
  if ((node->runtimeFlags & 0x80000000) != 0) {
    previousNode = node->previousNode;
    nextNode = node->nextNode;
    if (previousNode == (WorldOwnerListNode100 *)0x0) {
      node->ownerWorld->ownerListHead = nextNode;
    }
    else {
      previousNode->nextNode = nextNode;
    }
    if (nextNode != (WorldOwnerListNode100 *)0x0) {
      nextNode->previousNode = previousNode;
    }
  }
  node->runtimeFlags = 0;
  return;
}


/* Address: 0x0050D8F0.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime for each node in owner list d8.
*/
void __thandor_preserve_eax_edx
WorldRuntime_ForEachNodeInOwnerListD8
          (void *callbackContext,WorldRuntimeNodeTraversalCallback *callback,
          WorldRuntimeContext *world)

{
  WorldOwnerListNode100 *node;
  
  for (node = world->ownerListHead; node != (WorldOwnerListNode100 *)0x0; node = node->nextNode) {
    (*callback)(callbackContext,node);
  }
  return;
}


/* Address: 0x0050EC80.
   Ownership: world/runtime/core.
   Purpose: Returns g_GraphicsShadingRuntimeRecords in EAX and byte count 0x4000 in EDX, then inverts record zero
   serializationToggleDword before light.hex serialization.
*/
RuntimeImagePointerByteSizeEdxEax8 __cdecl RuntimeHexSegment_GetLightImageAndToggleFlagRegs(void)

{
  g_GraphicsShadingRuntimeRecords[0].serializationToggleDword =
       ~g_GraphicsShadingRuntimeRecords[0].serializationToggleDword;
  return 0x4000004bed50;
}

/* Address: 0x0050ECA0.
   Ownership: world/runtime/core.
   Purpose: Inverts record zero serializationToggleDword after light.hex serialization while the caller preserves
   serializer flags.
*/
void __cdecl RuntimeHexSegment_ToggleLightImageFlag(void)

{
  g_GraphicsShadingRuntimeRecords[0].serializationToggleDword =
       ~g_GraphicsShadingRuntimeRecords[0].serializationToggleDword;
  return;
}

/* Address: 0x0050ECB0.
   Ownership: world/runtime/core.
   Purpose: Missed pre-serializer provider for field.hex. From context +0x54 it returns the attached field-image
   pointer in EAX and that image's complete allocation size at +0x04 in EDX.
*/
ResourceRegistrationImagePair
RuntimeHexSegment_GetFieldImageRegs(InGameFieldImageSaveContext58 *fieldImageContext)

{
  return CONCAT44(fieldImageContext->fieldGridAsset,
                  (fieldImageContext->fieldGridAsset->common).allocationSizeBytes);
}

/* Address: 0x0050ECD0.
   Ownership: world/runtime/core.
   Purpose: One-argument field.hex post-serializer hook. It is an exact no-op invoked while the caller preserves
   the serializer CF result with PUSHF/POPF.
*/
void RuntimeHexSegment_AfterFieldImageNoOp(InGameFieldImageSaveContext58 *fieldImageContext)

{
  return;
}

/* Address: 0x0051BFA0.
   Ownership: world/runtime/core.
   Purpose: Owner-list callback that clears runtime-node fields which still reference a model or object being
   released. The node representation is selected by its verified kind field at +0xA4.
   Cross-module calls: ModelRuntimeHierarchy_ClearMatchingTargetRecursive [world/model/hierarchy].
*/
void __thandor_preserve_eax_edx
WorldRuntimeNode_ClearOwnedModelReferencesCallback(void *releasedObject,WorldOwnerListNode100 *node)

{
  int *modelRuntime;
  int linkedRuntimeStateAddress;
  
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    modelRuntime = node->runtimePayload;
    ModelRuntimeHierarchy_ClearMatchingTargetRecursive((RuntimeToken)releasedObject,modelRuntime);
    linkedRuntimeStateAddress = modelRuntime[2];
    if (releasedObject == *(void **)(linkedRuntimeStateAddress + 0x98)) {
      *(undefined4 *)(linkedRuntimeStateAddress + 0x98) = 0;
    }
    if (((*(uint *)(linkedRuntimeStateAddress + 0x2c) & 1) != 0) &&
       (releasedObject == *(void **)(linkedRuntimeStateAddress + 0x1c))) {
      *(undefined4 *)(linkedRuntimeStateAddress + 0x1c) = 0;
      *(uint *)(linkedRuntimeStateAddress + 0x2c) =
           *(uint *)(linkedRuntimeStateAddress + 0x2c) & 0xfffffff2;
    }
  }
  else if ((node->ownerClassId == WORLD_OWNER_RUNTIME_EFFECT) &&
          (releasedObject == *(void **)((int)node->runtimePayload + 0x1c))) {
    *(undefined4 *)((int)node->runtimePayload + 0x1c) = 0;
  }
  return;
}


/* Address: 0x0051D500.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime emit model definition overlay for matching entries.
   Cross-module calls: ModelDefinitionRegistry_FindByIdWithErrorCf [assets/model/definitions].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
          (void *sourceRuntime,WorldRuntimeContext *worldRuntime)

{
  dword dVar1;
  TerrainClassOverlayCallback *pTVar2;
  int iVar3;
  ModelDefinitionRecordPrefix *pMVar4;
  WorldOwnerListNode100 *pWVar5;
  ModelDefinitionLookupEaxCf5 MVar6;
  dword dStack_28;
  
  if (sourceRuntime != (void *)0x0) {
    MVar6 = ModelDefinitionRegistry_FindByIdWithErrorCf
                      (*(PckModelDefinitionIdCatalog *)(*(int *)((int)sourceRuntime + 0xc) + 0x20));
    pMVar4 = MVar6.modelDefinition;
    if (!MVar6.carry) {
      dStack_28 = 0xffffffff;
      pWVar5 = worldRuntime->ownerListHead;
      dVar1 = pMVar4[0x23].flags;
      if (pWVar5 != (WorldOwnerListNode100 *)0x0) {
        if (pMVar4[6].flags == 0xe) {
          dStack_28 = 0x800 << ((byte)pMVar4[0x10].byteSize & 0x1f);
        }
        pTVar2 = g_TerrainClassPlacementAndOverlayCallbacks10.overlayCallbacks
                 [pMVar4[0x34].definitionId];
        do {
          if (((pWVar5->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
              (worldRuntime->activeFactionRuntimeIndex ==
               *(int *)(*(int *)((int)pWVar5->runtimePayload + 8) + 0xc))) &&
             (iVar3 = *(int *)(*(int *)pWVar5->runtimePayload + 0x19c), iVar3 != 0)) {
            (*pTVar2)(dStack_28,-1,iVar3 + dVar1,pWVar5->worldYQ12,pWVar5->worldXQ12,
                      worldRuntime->fieldGrid);
          }
          pWVar5 = pWVar5->nextNode;
        } while (pWVar5 != (WorldOwnerListNode100 *)0x0);
      }
    }
  }
  return;
}


/* Address: 0x005233F0.
   Ownership: world/runtime/core.
   Purpose: Exact two-argument no-op installed in unified runtime table method slot 5. It preserves the existing
   EAX and flags contract and returns with ret 0x08. Runtime-update partition slots 0-23 receive (worldRuntime,
   armyRuntime).
*/
void UnifiedRuntimeTable_Method5_TwoArgNoOp
               (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  return;
}


/* Address: 0x00523400.
   Ownership: world/runtime/core.
   Purpose: Exact two-argument no-op installed in unified runtime table method slot 6. It preserves the existing
   EAX and flags contract and returns with ret 0x08. Runtime-update partition slots 0-23 receive (worldRuntime,
   armyRuntime).
*/
void UnifiedRuntimeTable_Method6_TwoArgNoOp
               (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime)

{
  return;
}


/* Address: 0x00527B70.
   Ownership: world/runtime/core.
   Purpose: Third exact one-argument no-op reused across many unified runtime object method-table entries. It
   returns with ret 0x04. Model-unrebase partition slots 48-71 receive one ModelRuntimeSlot pointer.
*/
void __thandor_void_preserve_eax_ecx_edx
UnifiedRuntimeDefault_OneArgNoOpC(ModelRuntimeSlot *modelRuntime)

{
  return;
}


/* Address: 0x00527BA0.
   Ownership: world/runtime/core.
   Purpose: Second exact two-argument no-op reused across unified runtime object method tables. It returns with ret
   0x08. Model release partition slots 0-23 receive (modelDefinition, modelRuntime).
*/
void UnifiedRuntimeDefault_TwoArgNoOpB
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  return;
}

/* Address: 0x00527BB0.
   Ownership: world/runtime/core.
   Purpose: Exact one-argument default that clears EAX and returns zero with ret 0x04.
*/
dword __thandor_eax_preserve_ecx_edx UnifiedRuntimeDefault_OneArgReturnZero(void *context)

{
  return 0;
}


/* Address: 0x00527BE0.
   Ownership: world/runtime/core.
   Purpose: Exact two-argument default that returns CF clear with ret 0x08 while preserving EAX. Placement-
   validation partition slots 24-47 receive (worldRuntime, armyRuntime), with CF carrying acceptance.
*/
bool __thandor_cf_preserve_eax_ecx_edx
UnifiedRuntimeDefault_TwoArgSuccessCf
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView200 *modelRuntime)

{
  return false;
}


/* Address: 0x00527BF0.
   Ownership: world/runtime/core.
   Purpose: Fourth exact two-argument no-op reused across unified runtime object method tables. It returns with ret
   0x08. Class method-D partition slots 24-47 receive (worldRuntime, armyRuntime).
*/
void __thandor_void_preserve_eax_ecx_edx
UnifiedRuntimeDefault_TwoArgNoOpD(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  return;
}


/* Address: 0x00529430.
   Ownership: world/runtime/core.
   Purpose: Owner-list callback that clears kind-specific entity/model references before the referenced runtime
   hierarchy is detached and released.
*/
void __thandor_preserve_eax_edx
WorldRuntimeNode_ClearDetachedEntityReferencesCallback
          (void *detachedObject,WorldOwnerListNode100 *node)

{
  int *entityRuntimePayloadWords;
  
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_EFFECT) {
    if (detachedObject == *(void **)((int)node->runtimePayload + 0x1c)) {
      *(undefined4 *)((int)node->runtimePayload + 0x1c) = 0;
    }
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    entityRuntimePayloadWords = node->runtimePayload;
    if (detachedObject == (void *)entityRuntimePayloadWords[0x3c]) {
      entityRuntimePayloadWords[0x3c] = 0;
    }
    if ((*(int *)(*entityRuntimePayloadWords + 0x4c) == 0x15) &&
       (detachedObject == (void *)entityRuntimePayloadWords[0x18])) {
      entityRuntimePayloadWords[0x18] = 0;
    }
  }
  else if ((node->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) &&
          (detachedObject == *(void **)((int)node->runtimePayload + 0x14))) {
    *(undefined4 *)((int)node->runtimePayload + 0x14) = 0;
  }
  return;
}


/* Address: 0x00565110.
   Ownership: world/runtime/core.
   Purpose: In-game shutdown owner-list callback. It releases kind-0 bindings and clears the verified kind-1 and
   kind-2 back-reference fields before level resources are destroyed.
   Cross-module calls: ArmyRuntime_DestroyInstanceAndRefreshUi [gameplay/army/runtime].
*/
void __thandor_preserve_eax_edx
WorldRuntimeNode_ReleaseShutdownBindingsCallback
          (WorldRuntimeContext *shutdownContext,WorldOwnerListNode100 *node)

{
  if (node->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
    ArmyRuntime_DestroyInstanceAndRefreshUi
              (shutdownContext,*(GameEntityRuntime **)((int)node->runtimePayload + 8));
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_SHOT) {
    node->runtimeFlags = node->runtimeFlags & 0x3fffffff;
    *(undefined4 *)((int)node->runtimePayload + 0x10) = 0;
  }
  else if (node->ownerClassId == WORLD_OWNER_RUNTIME_EFFECT) {
    node->runtimeFlags = node->runtimeFlags & 0x3fffffff;
    *(undefined4 *)((int)node->runtimePayload + 4) = 0;
  }
  return;
}


/* Address: 0x0050D3B0.
   Ownership: world/runtime/core.
   Purpose: Raycasts the terrain and secondary field surfaces according to the runtime surface-selection flag,
   chooses the nearest accepted travel distance, rebuilds the endpoint coordinates and distance, and clears the
   field-grid dirty flag.
   Local calls: WorldRuntime_ClearFieldGridDirtyFlag.
   Cross-module calls: FieldGrid_RaycastTerrainSurfaceDistanceCf [world/terrain/grid], FixedMath_SinCosScaled
   [core/math/fixed], FixedMath_Length3 [core/math/fixed], FieldGrid_RaycastSecondarySurfaceDistanceCf
   [world/terrain/grid], FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 AVar1;
  UQ12 scale;
  dword dVar2;
  int iVar3;
  FixedSinCosEdxEax8 FVar4;
  FieldGridRaycastEaxEdxCf9 FVar5;
  FixedDirectionXyzRegs12 FVar6;
  
  if ((worldRuntime->runtimeFlags & 0x1000000) == 0) {
    iVar3 = worldRuntime->maximumCameraDistanceQ12 << 2;
    FVar5 = FieldGrid_RaycastTerrainSurfaceDistanceCf
                      ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,iVar3,
                       (worldRuntime->motion).positionZQ12,(worldRuntime->motion).positionYQ12,
                       (worldRuntime->motion).positionXQ12,worldRuntime->fieldGrid);
    scale = FVar5.distanceQ12;
    if (!FVar5.carry) {
WorldRuntime_RecomputeMotionEndpoint_UseGroundPlaneFallback:
      AVar1 = (worldRuntime->motion).pitchAngle;
      FVar4 = FixedMath_SinCosScaled
                        ((worldRuntime->motion).headingAngle,
                         (FixedMathScale32)
                         (((longlong)(worldRuntime->motion).positionZQ12 *
                          (longlong)g_FixedCosQ28[-AVar1]) / (longlong)g_FixedSinQ28[-AVar1]));
      iVar3 = (int)(FVar4 >> 0x20);
      (worldRuntime->motion).targetPositionXQ12 = (int)FVar4 + (worldRuntime->motion).positionXQ12;
      (worldRuntime->motion).targetPositionYQ12 = iVar3 + (worldRuntime->motion).positionYQ12;
      (worldRuntime->motion).targetPositionZQ12 = 0;
      dVar2 = FixedMath_Length3((worldRuntime->motion).positionZQ12,iVar3,(int)FVar4);
      (worldRuntime->motion).targetDistanceQ12 = dVar2;
      WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
      return;
    }
    FVar5 = FieldGrid_RaycastSecondarySurfaceDistanceCf
                      ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,iVar3,
                       (worldRuntime->motion).positionZQ12,(worldRuntime->motion).positionYQ12,
                       (worldRuntime->motion).positionXQ12,worldRuntime->fieldGrid);
    if ((FVar5.carry) && (FVar5.distanceQ12 < (int)scale)) {
      scale = FVar5.distanceQ12;
    }
  }
  else {
    FVar5 = FieldGrid_RaycastSecondarySurfaceDistanceCf
                      ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,
                       worldRuntime->maximumCameraDistanceQ12 << 2,
                       (worldRuntime->motion).positionZQ12,(worldRuntime->motion).positionYQ12,
                       (worldRuntime->motion).positionXQ12,worldRuntime->fieldGrid);
    scale = FVar5.distanceQ12;
    if (!FVar5.carry) goto WorldRuntime_RecomputeMotionEndpoint_UseGroundPlaneFallback;
  }
  (worldRuntime->motion).targetDistanceQ12 = scale;
  FVar6 = FixedMath_DirectionFromAnglesScaledRegs
                    ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,scale);
  (worldRuntime->motion).targetPositionXQ12 = FVar6.eax + (worldRuntime->motion).positionXQ12;
  (worldRuntime->motion).targetPositionYQ12 = FVar6.ecx + (worldRuntime->motion).positionYQ12;
  (worldRuntime->motion).targetPositionZQ12 = FVar6.edx + (worldRuntime->motion).positionZQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}


/* Address: 0x0050D760.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime set terrain lighting configuration.
   Cross-module calls: TerrainLighting_BuildColorRampAndSetBaseColor [world/terrain/visuals].
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_SetTerrainLightingConfiguration
          (PackedArgb32 lightingColor13CArgb,PackedArgb32 lightingColor138Argb,
          PackedArgb32 lightingColor134Argb,PackedArgb32 lightingColor130Argb,
          PackedArgb32 rampColor12CArgb,PackedArgb32 lightingColor128Argb,
          PackedArgb32 rampColor124Argb,PackedArgb32 baseColorArgb,WorldRuntimeContext *worldRuntime
          )

{
  (worldRuntime->lighting).color130Argb = lightingColor130Argb;
  (worldRuntime->lighting).color134Argb = lightingColor134Argb;
  (worldRuntime->lighting).color128Argb = lightingColor128Argb;
  (worldRuntime->lighting).color138Argb = lightingColor138Argb;
  (worldRuntime->lighting).color13CArgb = lightingColor13CArgb;
  (worldRuntime->lighting).baseColorArgb = baseColorArgb;
  (worldRuntime->lighting).rampColorArgb = rampColor124Argb;
  (worldRuntime->lighting).color12CArgb = rampColor12CArgb;
  TerrainLighting_BuildColorRampAndSetBaseColor(rampColor12CArgb,rampColor124Argb,baseColorArgb);
  return;
}


/* Address: 0x0050D5C0.
   Ownership: world/runtime/core.
   Purpose: Typed parameters: p2 gridHeight→FieldGridDimensionCells_V343, p3
   gridWidth→FieldGridDimensionCells_V343. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: FieldGrid_RecomputeInteriorTriangleNormalAngles [world/terrain/grid],
   FieldGrid_RecomputeInteriorDirectionalLighting [world/terrain/grid].
*/
void __thandor_void_preserve_ecx_edx
WorldRuntime_RecomputeFieldRegionNormalsAndLighting
          (FieldGridDimensionCells gridHeight,FieldGridDimensionCells gridWidth,Q12 originWorldYQ12,
          Q12 originWorldXQ12,WorldRuntimeContext *worldRuntime)

{
  *(Q12 *)(worldRuntime[1].interaction.reserved00_47 + 0x1c) = originWorldXQ12;
  *(Q12 *)(worldRuntime[1].interaction.reserved00_47 + 0x20) = originWorldYQ12;
  FieldGrid_RecomputeInteriorTriangleNormalAngles(worldRuntime->fieldGrid);
  FieldGrid_RecomputeInteriorDirectionalLighting
            (originWorldYQ12,originWorldXQ12,worldRuntime->fieldGrid);
  (worldRuntime->fieldRegion).regionWidth = gridWidth;
  (worldRuntime->fieldRegion).regionHeight = gridHeight;
  return;
}


/* Address: 0x0050D6B0.
   Ownership: world/runtime/core.
   Purpose: Clears bit 0x00000800 in the dword at context offset 0x4C.
*/
void __thandor_void_preserve_eax_ecx_edx
WorldRuntime_ClearFieldGridDirtyFlag(WorldRuntimeContext *world)

{
  world->runtimeFlags = world->runtimeFlags & 0xfffff7ff;
  return;
}

