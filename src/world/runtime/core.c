#include <thandor/world/runtime/core.h>

/* Implementation ownership: world/runtime/core. */

/* Address: 0x00532FA0.
   Ownership: world/runtime/core.
   Purpose: Interpolates the level lighting color sets and angular parameters from the current runtime phase,
   applies the resulting terrain-lighting configuration, and refreshes field-region normals and lighting.
   Local calls: WorldRuntime_SetTerrainLightingConfiguration, WorldRuntime_RecomputeFieldRegionNormalsAndLighting.
*/
undefined8 WorldLightingRuntime_UpdateInterpolatedTerrainLighting(void)

{
  SoftwareBgraWordLanes SVar1;
  SoftwareBgraWordLanes SVar2;
  InGameConditionRecord *pIVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  InGameConditionRecordCount IVar8;
  InGameConditionRuntime *pIVar9;
  byte mm0PackedValue0ByteLane3;
  byte mm0PackedValue0ByteLane1;
  short sVar19;
  byte mm0PackedValue1ByteLane1;
  short sVar23;
  byte mm0PackedValue1ByteLane3;
  short sVar20;
  byte mm0PackedValue2ByteLane1;
  short sVar24;
  byte mm0PackedValue2ByteLane3;
  short sVar21;
  byte mm0PackedValue3ByteLane1;
  short sVar22;
  byte mm0PackedValue0ByteLane2;
  undefined4 in_EAX;
  uint uVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  undefined4 in_EDX;
  uint uVar14;
  uint uVar15;
  int iVar16;
  int iVar17;
  WorldRuntimeContext *worldRuntime;
  int iVar18;
  short sVar26;
  short sVar27;
  short sVar30;
  undefined8 mm0PackedValue0;
  byte mm0PackedValue1ByteLane2;
  undefined8 mm0PackedValue1;
  short sVar25;
  byte mm0PackedValue3ByteLane3;
  byte mm0PackedValue2ByteLane2;
  short sVar28;
  undefined8 mm0PackedValue2;
  byte mm0PackedValue3ByteLane2;
  short sVar29;
  undefined8 mm0PackedValue3;
  short sVar34;
  byte mm1PackedValue0ByteLane2;
  byte mm1PackedValue0ByteLane3;
  short sVar31;
  byte mm1PackedValue1ByteLane1;
  short sVar35;
  byte mm1PackedValue1ByteLane3;
  byte mm1PackedValue0ByteLane1;
  short sVar38;
  short sVar39;
  short sVar42;
  undefined8 mm1PackedValue0;
  byte mm1PackedValue1ByteLane2;
  short sVar43;
  undefined8 mm1PackedValue1;
  short sVar32;
  byte mm1PackedValue2ByteLane1;
  short sVar36;
  byte mm1PackedValue2ByteLane3;
  short sVar33;
  byte mm1PackedValue3ByteLane1;
  short sVar37;
  byte mm1PackedValue3ByteLane3;
  byte mm1PackedValue2ByteLane2;
  short sVar40;
  short sVar44;
  undefined8 mm1PackedValue2;
  byte mm1PackedValue3ByteLane2;
  short sVar41;
  short sVar45;
  undefined8 mm1PackedValue3;
  byte mm2PackedValue0ByteLane2;
  byte mm2PackedValue1ByteLane2;
  byte mm2PackedValue2ByteLane2;
  byte mm2PackedValue3ByteLane2;
  byte mm2PackedValue0ByteLane3;
  byte mm2PackedValue0ByteLane1;
  short sVar46;
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
  WorldRuntimeContext *worldContext1;
  
  pIVar9 = g_InGameConditionRuntime;
  pIVar3 = g_InGameConditionRuntime[3].records58;
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  if (pIVar3 != (InGameConditionRecord *)0x0) {
    uVar10 = (g_GameFactionRuntimeImage.tail.simulationTick % (uint)pIVar3 << 0x10) / (uint)pIVar3;
    uVar14 = g_FixedCosQ28[uVar10] + 0x10000000U >> 0x15;
    worldContext1 = g_InGameConditionRuntime[3].worldContext50;
    uVar4 = *(undefined4 *)g_InGameConditionRuntime[3].reserved54_57;
    uVar5 = *(undefined4 *)(g_InGameConditionRuntime[3].reserved5C_AB + 0x40);
    uVar6 = *(undefined4 *)(g_InGameConditionRuntime[3].reserved5C_AB + 0x44);
    SVar1 = g_SoftwareBilinearForwardFactors[uVar14];
    SVar2 = g_SoftwareBilinearInverseFactors[uVar14];
    mm0PackedValue0ByteLane3 = (byte)((uint)worldContext1 >> 0x18);
    mm0PackedValue0ByteLane2 = (byte)((uint)worldContext1 >> 0x10);
    mm0PackedValue0ByteLane1 = (byte)((uint)worldContext1 >> 8);
    mm1PackedValue0ByteLane3 = (byte)((uint)uVar4 >> 0x18);
    mm1PackedValue0ByteLane2 = (byte)((uint)uVar4 >> 0x10);
    mm1PackedValue0ByteLane1 = (byte)((uint)uVar4 >> 8);
    mm2PackedValue0ByteLane3 = (byte)((uint)uVar5 >> 0x18);
    mm2PackedValue0ByteLane2 = (byte)((uint)uVar5 >> 0x10);
    mm2PackedValue0ByteLane1 = (byte)((uint)uVar5 >> 8);
    mm3PackedValue0ByteLane3 = (byte)((uint)uVar6 >> 0x18);
    mm3PackedValue0ByteLane2 = (byte)((uint)uVar6 >> 0x10);
    mm3PackedValue0ByteLane1 = (byte)((uint)uVar6 >> 8);
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
                                                    CONCAT14(mm1PackedValue0ByteLane2,uVar4)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm1PackedValue0ByteLane1,
                                                    mm1PackedValue0ByteLane1) >> 6,
                                           CONCAT11((char)uVar4,(char)uVar4) >> 6))),SVar1);
    mm2PackedValue0 =
         pmulhw(CONCAT26(CONCAT11(mm2PackedValue0ByteLane3,mm2PackedValue0ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm2PackedValue0ByteLane3,
                                                                      mm2PackedValue0ByteLane3),
                                                             mm2PackedValue0ByteLane2),
                                                    CONCAT14(mm2PackedValue0ByteLane2,uVar5)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm2PackedValue0ByteLane1,
                                                    mm2PackedValue0ByteLane1) >> 6,
                                           CONCAT11((char)uVar5,(char)uVar5) >> 6))),SVar2);
    mm3PackedValue0 =
         pmulhw(CONCAT26(CONCAT11(mm3PackedValue0ByteLane3,mm3PackedValue0ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm3PackedValue0ByteLane3,
                                                                      mm3PackedValue0ByteLane3),
                                                             mm3PackedValue0ByteLane2),
                                                    CONCAT14(mm3PackedValue0ByteLane2,uVar6)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm3PackedValue0ByteLane1,
                                                    mm3PackedValue0ByteLane1) >> 6,
                                           CONCAT11((char)uVar6,(char)uVar6) >> 6))),SVar2);
    sVar19 = (short)mm0PackedValue0 + (short)mm2PackedValue0;
    sVar23 = (short)((ulonglong)mm0PackedValue0 >> 0x10) +
             (short)((ulonglong)mm2PackedValue0 >> 0x10);
    sVar27 = (short)((ulonglong)mm0PackedValue0 >> 0x20) +
             (short)((ulonglong)mm2PackedValue0 >> 0x20);
    sVar31 = (short)mm1PackedValue0 + (short)mm3PackedValue0;
    sVar35 = (short)((ulonglong)mm1PackedValue0 >> 0x10) +
             (short)((ulonglong)mm3PackedValue0 >> 0x10);
    sVar39 = (short)((ulonglong)mm1PackedValue0 >> 0x20) +
             (short)((ulonglong)mm3PackedValue0 >> 0x20);
    sVar43 = (short)((ulonglong)mm1PackedValue0 >> 0x30) +
             (short)((ulonglong)mm3PackedValue0 >> 0x30);
    uVar4 = *(undefined4 *)(g_InGameConditionRuntime[3].reserved5C_AB + 4);
    uVar5 = *(undefined4 *)(g_InGameConditionRuntime[3].reserved5C_AB + 8);
    uVar6 = *(undefined4 *)(g_InGameConditionRuntime[3].reserved5C_AB + 0x48);
    uVar7 = *(undefined4 *)(g_InGameConditionRuntime[3].reserved5C_AB + 0x4c);
    mm0PackedValue1ByteLane3 = (byte)((uint)uVar4 >> 0x18);
    mm0PackedValue1ByteLane2 = (byte)((uint)uVar4 >> 0x10);
    mm0PackedValue1ByteLane1 = (byte)((uint)uVar4 >> 8);
    mm1PackedValue1ByteLane3 = (byte)((uint)uVar5 >> 0x18);
    mm1PackedValue1ByteLane2 = (byte)((uint)uVar5 >> 0x10);
    mm1PackedValue1ByteLane1 = (byte)((uint)uVar5 >> 8);
    mm2PackedValue1ByteLane3 = (byte)((uint)uVar6 >> 0x18);
    mm2PackedValue1ByteLane2 = (byte)((uint)uVar6 >> 0x10);
    mm2PackedValue1ByteLane1 = (byte)((uint)uVar6 >> 8);
    mm3PackedValue1ByteLane3 = (byte)((uint)uVar7 >> 0x18);
    mm3PackedValue1ByteLane2 = (byte)((uint)uVar7 >> 0x10);
    mm3PackedValue1ByteLane1 = (byte)((uint)uVar7 >> 8);
    mm0PackedValue1 =
         pmulhw(CONCAT26(CONCAT11(mm0PackedValue1ByteLane3,mm0PackedValue1ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue1ByteLane3,
                                                                      mm0PackedValue1ByteLane3),
                                                             mm0PackedValue1ByteLane2),
                                                    CONCAT14(mm0PackedValue1ByteLane2,uVar4)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm0PackedValue1ByteLane1,
                                                    mm0PackedValue1ByteLane1) >> 6,
                                           CONCAT11((char)uVar4,(char)uVar4) >> 6))),SVar1);
    mm1PackedValue1 =
         pmulhw(CONCAT26(CONCAT11(mm1PackedValue1ByteLane3,mm1PackedValue1ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm1PackedValue1ByteLane3,
                                                                      mm1PackedValue1ByteLane3),
                                                             mm1PackedValue1ByteLane2),
                                                    CONCAT14(mm1PackedValue1ByteLane2,uVar5)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm1PackedValue1ByteLane1,
                                                    mm1PackedValue1ByteLane1) >> 6,
                                           CONCAT11((char)uVar5,(char)uVar5) >> 6))),SVar1);
    mm2PackedValue1 =
         pmulhw(CONCAT26(CONCAT11(mm2PackedValue1ByteLane3,mm2PackedValue1ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm2PackedValue1ByteLane3,
                                                                      mm2PackedValue1ByteLane3),
                                                             mm2PackedValue1ByteLane2),
                                                    CONCAT14(mm2PackedValue1ByteLane2,uVar6)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm2PackedValue1ByteLane1,
                                                    mm2PackedValue1ByteLane1) >> 6,
                                           CONCAT11((char)uVar6,(char)uVar6) >> 6))),SVar2);
    mm3PackedValue1 =
         pmulhw(CONCAT26(CONCAT11(mm3PackedValue1ByteLane3,mm3PackedValue1ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm3PackedValue1ByteLane3,
                                                                      mm3PackedValue1ByteLane3),
                                                             mm3PackedValue1ByteLane2),
                                                    CONCAT14(mm3PackedValue1ByteLane2,uVar7)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm3PackedValue1ByteLane1,
                                                    mm3PackedValue1ByteLane1) >> 6,
                                           CONCAT11((char)uVar7,(char)uVar7) >> 6))),SVar2);
    sVar20 = (short)mm0PackedValue1 + (short)mm2PackedValue1;
    sVar24 = (short)((ulonglong)mm0PackedValue1 >> 0x10) +
             (short)((ulonglong)mm2PackedValue1 >> 0x10);
    sVar28 = (short)((ulonglong)mm0PackedValue1 >> 0x20) +
             (short)((ulonglong)mm2PackedValue1 >> 0x20);
    sVar32 = (short)mm1PackedValue1 + (short)mm3PackedValue1;
    sVar36 = (short)((ulonglong)mm1PackedValue1 >> 0x10) +
             (short)((ulonglong)mm3PackedValue1 >> 0x10);
    sVar40 = (short)((ulonglong)mm1PackedValue1 >> 0x20) +
             (short)((ulonglong)mm3PackedValue1 >> 0x20);
    sVar44 = (short)((ulonglong)mm1PackedValue1 >> 0x30) +
             (short)((ulonglong)mm3PackedValue1 >> 0x30);
    uVar4 = *(undefined4 *)(g_InGameConditionRuntime[3].reserved5C_AB + 0x10);
    uVar5 = *(undefined4 *)(g_InGameConditionRuntime[3].reserved5C_AB + 0x14);
    IVar8 = g_InGameConditionRuntime[3].recordCountAC;
    uVar6 = *(undefined4 *)g_InGameConditionRuntime[3].reservedB0_D7;
    mm0PackedValue2ByteLane3 = (byte)((uint)uVar4 >> 0x18);
    mm0PackedValue2ByteLane2 = (byte)((uint)uVar4 >> 0x10);
    mm0PackedValue2ByteLane1 = (byte)((uint)uVar4 >> 8);
    mm1PackedValue2ByteLane3 = (byte)((uint)uVar5 >> 0x18);
    mm1PackedValue2ByteLane2 = (byte)((uint)uVar5 >> 0x10);
    mm1PackedValue2ByteLane1 = (byte)((uint)uVar5 >> 8);
    mm2PackedValue2ByteLane3 = (byte)(IVar8 >> 0x18);
    mm2PackedValue2ByteLane2 = (byte)(IVar8 >> 0x10);
    mm2PackedValue2ByteLane1 = (byte)(IVar8 >> 8);
    mm3PackedValue2ByteLane3 = (byte)((uint)uVar6 >> 0x18);
    mm3PackedValue2ByteLane2 = (byte)((uint)uVar6 >> 0x10);
    mm3PackedValue2ByteLane1 = (byte)((uint)uVar6 >> 8);
    mm0PackedValue2 =
         pmulhw(CONCAT26(CONCAT11(mm0PackedValue2ByteLane3,mm0PackedValue2ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue2ByteLane3,
                                                                      mm0PackedValue2ByteLane3),
                                                             mm0PackedValue2ByteLane2),
                                                    CONCAT14(mm0PackedValue2ByteLane2,uVar4)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm0PackedValue2ByteLane1,
                                                    mm0PackedValue2ByteLane1) >> 6,
                                           CONCAT11((char)uVar4,(char)uVar4) >> 6))),SVar1);
    mm1PackedValue2 =
         pmulhw(CONCAT26(CONCAT11(mm1PackedValue2ByteLane3,mm1PackedValue2ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm1PackedValue2ByteLane3,
                                                                      mm1PackedValue2ByteLane3),
                                                             mm1PackedValue2ByteLane2),
                                                    CONCAT14(mm1PackedValue2ByteLane2,uVar5)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm1PackedValue2ByteLane1,
                                                    mm1PackedValue2ByteLane1) >> 6,
                                           CONCAT11((char)uVar5,(char)uVar5) >> 6))),SVar1);
    mm2PackedValue2 =
         pmulhw(CONCAT26(CONCAT11(mm2PackedValue2ByteLane3,mm2PackedValue2ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm2PackedValue2ByteLane3,
                                                                      mm2PackedValue2ByteLane3),
                                                             mm2PackedValue2ByteLane2),
                                                    CONCAT14(mm2PackedValue2ByteLane2,IVar8)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm2PackedValue2ByteLane1,
                                                    mm2PackedValue2ByteLane1) >> 6,
                                           CONCAT11((char)IVar8,(char)IVar8) >> 6))),SVar2);
    mm3PackedValue2 =
         pmulhw(CONCAT26(CONCAT11(mm3PackedValue2ByteLane3,mm3PackedValue2ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm3PackedValue2ByteLane3,
                                                                      mm3PackedValue2ByteLane3),
                                                             mm3PackedValue2ByteLane2),
                                                    CONCAT14(mm3PackedValue2ByteLane2,uVar6)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm3PackedValue2ByteLane1,
                                                    mm3PackedValue2ByteLane1) >> 6,
                                           CONCAT11((char)uVar6,(char)uVar6) >> 6))),SVar2);
    sVar21 = (short)mm0PackedValue2 + (short)mm2PackedValue2;
    sVar25 = (short)((ulonglong)mm0PackedValue2 >> 0x10) +
             (short)((ulonglong)mm2PackedValue2 >> 0x10);
    sVar29 = (short)((ulonglong)mm0PackedValue2 >> 0x20) +
             (short)((ulonglong)mm2PackedValue2 >> 0x20);
    sVar33 = (short)mm1PackedValue2 + (short)mm3PackedValue2;
    sVar37 = (short)((ulonglong)mm1PackedValue2 >> 0x10) +
             (short)((ulonglong)mm3PackedValue2 >> 0x10);
    sVar41 = (short)((ulonglong)mm1PackedValue2 >> 0x20) +
             (short)((ulonglong)mm3PackedValue2 >> 0x20);
    sVar45 = (short)((ulonglong)mm1PackedValue2 >> 0x30) +
             (short)((ulonglong)mm3PackedValue2 >> 0x30);
    uVar4 = *(undefined4 *)(g_InGameConditionRuntime[3].reserved5C_AB + 0x18);
    uVar5 = *(undefined4 *)(g_InGameConditionRuntime[3].reserved5C_AB + 0x1c);
    uVar6 = *(undefined4 *)(g_InGameConditionRuntime[3].reservedB0_D7 + 4);
    uVar7 = *(undefined4 *)(g_InGameConditionRuntime[3].reservedB0_D7 + 8);
    mm0PackedValue3ByteLane3 = (byte)((uint)uVar4 >> 0x18);
    mm0PackedValue3ByteLane2 = (byte)((uint)uVar4 >> 0x10);
    mm0PackedValue3ByteLane1 = (byte)((uint)uVar4 >> 8);
    mm1PackedValue3ByteLane3 = (byte)((uint)uVar5 >> 0x18);
    mm1PackedValue3ByteLane2 = (byte)((uint)uVar5 >> 0x10);
    mm1PackedValue3ByteLane1 = (byte)((uint)uVar5 >> 8);
    mm2PackedValue3ByteLane3 = (byte)((uint)uVar6 >> 0x18);
    mm2PackedValue3ByteLane2 = (byte)((uint)uVar6 >> 0x10);
    mm2PackedValue3ByteLane1 = (byte)((uint)uVar6 >> 8);
    mm3PackedValue3ByteLane3 = (byte)((uint)uVar7 >> 0x18);
    mm3PackedValue3ByteLane2 = (byte)((uint)uVar7 >> 0x10);
    mm3PackedValue3ByteLane1 = (byte)((uint)uVar7 >> 8);
    mm0PackedValue3 =
         pmulhw(CONCAT26(CONCAT11(mm0PackedValue3ByteLane3,mm0PackedValue3ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm0PackedValue3ByteLane3,
                                                                      mm0PackedValue3ByteLane3),
                                                             mm0PackedValue3ByteLane2),
                                                    CONCAT14(mm0PackedValue3ByteLane2,uVar4)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm0PackedValue3ByteLane1,
                                                    mm0PackedValue3ByteLane1) >> 6,
                                           CONCAT11((char)uVar4,(char)uVar4) >> 6))),SVar1);
    mm1PackedValue3 =
         pmulhw(CONCAT26(CONCAT11(mm1PackedValue3ByteLane3,mm1PackedValue3ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm1PackedValue3ByteLane3,
                                                                      mm1PackedValue3ByteLane3),
                                                             mm1PackedValue3ByteLane2),
                                                    CONCAT14(mm1PackedValue3ByteLane2,uVar5)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm1PackedValue3ByteLane1,
                                                    mm1PackedValue3ByteLane1) >> 6,
                                           CONCAT11((char)uVar5,(char)uVar5) >> 6))),SVar1);
    mm2PackedValue3 =
         pmulhw(CONCAT26(CONCAT11(mm2PackedValue3ByteLane3,mm2PackedValue3ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm2PackedValue3ByteLane3,
                                                                      mm2PackedValue3ByteLane3),
                                                             mm2PackedValue3ByteLane2),
                                                    CONCAT14(mm2PackedValue3ByteLane2,uVar6)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm2PackedValue3ByteLane1,
                                                    mm2PackedValue3ByteLane1) >> 6,
                                           CONCAT11((char)uVar6,(char)uVar6) >> 6))),SVar2);
    mm3PackedValue3 =
         pmulhw(CONCAT26(CONCAT11(mm3PackedValue3ByteLane3,mm3PackedValue3ByteLane3) >> 6,
                         CONCAT24((ushort)(CONCAT35(CONCAT21(CONCAT11(mm3PackedValue3ByteLane3,
                                                                      mm3PackedValue3ByteLane3),
                                                             mm3PackedValue3ByteLane2),
                                                    CONCAT14(mm3PackedValue3ByteLane2,uVar7)) >>
                                          0x20) >> 6,
                                  CONCAT22(CONCAT11(mm3PackedValue3ByteLane1,
                                                    mm3PackedValue3ByteLane1) >> 6,
                                           CONCAT11((char)uVar7,(char)uVar7) >> 6))),SVar2);
    sVar22 = (short)mm0PackedValue3 + (short)mm2PackedValue3;
    sVar26 = (short)((ulonglong)mm0PackedValue3 >> 0x10) +
             (short)((ulonglong)mm2PackedValue3 >> 0x10);
    sVar30 = (short)((ulonglong)mm0PackedValue3 >> 0x20) +
             (short)((ulonglong)mm2PackedValue3 >> 0x20);
    sVar34 = (short)mm1PackedValue3 + (short)mm3PackedValue3;
    sVar38 = (short)((ulonglong)mm1PackedValue3 >> 0x10) +
             (short)((ulonglong)mm3PackedValue3 >> 0x10);
    sVar42 = (short)((ulonglong)mm1PackedValue3 >> 0x20) +
             (short)((ulonglong)mm3PackedValue3 >> 0x20);
    sVar46 = (short)((ulonglong)mm1PackedValue3 >> 0x30) +
             (short)((ulonglong)mm3PackedValue3 >> 0x30);
    WorldRuntime_SetTerrainLightingConfiguration
              (CONCAT13((0 < sVar46) * (sVar46 < 0x100) * (char)sVar46 - (0xff < sVar46),
                        CONCAT12((0 < sVar42) * (sVar42 < 0x100) * (char)sVar42 - (0xff < sVar42),
                                 CONCAT11((0 < sVar38) * (sVar38 < 0x100) * (char)sVar38 -
                                          (0xff < sVar38),
                                          (0 < sVar34) * (sVar34 < 0x100) * (char)sVar34 -
                                          (0xff < sVar34)))) | 0xff000000,
               (uint)CONCAT12((0 < sVar30) * (sVar30 < 0x100) * (char)sVar30 - (0xff < sVar30),
                              CONCAT11((0 < sVar26) * (sVar26 < 0x100) * (char)sVar26 -
                                       (0xff < sVar26),
                                       (0 < sVar22) * (sVar22 < 0x100) * (char)sVar22 -
                                       (0xff < sVar22))),
               CONCAT13((0 < sVar45) * (sVar45 < 0x100) * (char)sVar45 - (0xff < sVar45),
                        CONCAT12((0 < sVar41) * (sVar41 < 0x100) * (char)sVar41 - (0xff < sVar41),
                                 CONCAT11((0 < sVar37) * (sVar37 < 0x100) * (char)sVar37 -
                                          (0xff < sVar37),
                                          (0 < sVar33) * (sVar33 < 0x100) * (char)sVar33 -
                                          (0xff < sVar33)))) | 0xff000000,
               (uint)CONCAT12((0 < sVar29) * (sVar29 < 0x100) * (char)sVar29 - (0xff < sVar29),
                              CONCAT11((0 < sVar25) * (sVar25 < 0x100) * (char)sVar25 -
                                       (0xff < sVar25),
                                       (0 < sVar21) * (sVar21 < 0x100) * (char)sVar21 -
                                       (0xff < sVar21))),
               CONCAT13((0 < sVar44) * (sVar44 < 0x100) * (char)sVar44 - (0xff < sVar44),
                        CONCAT12((0 < sVar40) * (sVar40 < 0x100) * (char)sVar40 - (0xff < sVar40),
                                 CONCAT11((0 < sVar36) * (sVar36 < 0x100) * (char)sVar36 -
                                          (0xff < sVar36),
                                          (0 < sVar32) * (sVar32 < 0x100) * (char)sVar32 -
                                          (0xff < sVar32)))) |
               *(uint *)(g_InGameConditionRuntime[3].reserved5C_AB + 8) & 0xff000000,
               (uint)CONCAT12((0 < sVar28) * (sVar28 < 0x100) * (char)sVar28 - (0xff < sVar28),
                              CONCAT11((0 < sVar24) * (sVar24 < 0x100) * (char)sVar24 -
                                       (0xff < sVar24),
                                       (0 < sVar20) * (sVar20 < 0x100) * (char)sVar20 -
                                       (0xff < sVar20))),
               CONCAT13((0 < sVar43) * (sVar43 < 0x100) * (char)sVar43 - (0xff < sVar43),
                        CONCAT12((0 < sVar39) * (sVar39 < 0x100) * (char)sVar39 - (0xff < sVar39),
                                 CONCAT11((0 < sVar35) * (sVar35 < 0x100) * (char)sVar35 -
                                          (0xff < sVar35),
                                          (0 < sVar31) * (sVar31 < 0x100) * (char)sVar31 -
                                          (0xff < sVar31)))) | 0xff000000,
               (uint)CONCAT12((0 < sVar27) * (sVar27 < 0x100) * (char)sVar27 - (0xff < sVar27),
                              CONCAT11((0 < sVar23) * (sVar23 < 0x100) * (char)sVar23 -
                                       (0xff < sVar23),
                                       (0 < sVar19) * (sVar19 < 0x100) * (char)sVar19 -
                                       (0xff < sVar19))),worldRuntime);
    uVar13 = uVar10 >> 8;
    uVar14 = (uint)*(ushort *)(pIVar9[3].reserved00_4F + 0x4c);
    uVar15 = (uint)*(ushort *)(pIVar9[3].reserved5C_AB + 0x38);
    if (uVar13 < 0x80) {
      if (uVar15 < uVar14) {
        uVar15 = uVar15 + 0x10000;
      }
      iVar16 = uVar15 * uVar13;
      iVar11 = uVar14 * (0x80 - uVar13);
    }
    else {
      if (uVar14 < uVar15) {
        uVar14 = uVar14 + 0x10000;
      }
      iVar11 = uVar14 * (uVar13 - 0x80);
      iVar16 = uVar15 * (0x80 - (uVar13 - 0x80));
    }
    uVar15 = g_FixedCosQ28[uVar10] + 0x10000000U >> 0x15;
    iVar18 = 0x100 - uVar15;
    uVar14 = (uint)*(ushort *)pIVar9[3].reserved5C_AB;
    uVar10 = uVar10 >> 8;
    uVar13 = (uint)*(ushort *)(pIVar9[3].reserved5C_AB + 0x3c);
    if (uVar10 < 0x80) {
      if (uVar13 < uVar14) {
        uVar13 = uVar13 + 0x10000;
      }
      iVar17 = uVar13 * uVar10;
      iVar12 = uVar14 * (0x80 - uVar10);
    }
    else {
      if (uVar14 < uVar13) {
        uVar14 = uVar14 + 0x10000;
      }
      iVar12 = uVar14 * (uVar10 - 0x80);
      iVar17 = uVar13 * (0x80 - (uVar10 - 0x80));
    }
    WorldRuntime_RecomputeFieldRegionNormalsAndLighting
              ((int)((uint)*(ushort *)(pIVar9[3].reserved5C_AB + 0x3e) * iVar18 +
                    (uint)*(ushort *)(pIVar9[3].reserved5C_AB + 2) * (0x100 - iVar18)) >> 8,
               (uint)(iVar12 + iVar17) >> 7 & 0xffff,
               (int)((uint)*(ushort *)(pIVar9[3].reserved5C_AB + 0x3a) * iVar18 +
                    *(ushort *)(pIVar9[3].reserved00_4F + 0x4e) * uVar15) >> 8,
               (uint)(iVar11 + iVar16) >> 7 & 0xffff,worldRuntime);
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0050D100.
   Ownership: world/runtime/core.
   Purpose: Stores a three-component position at +0x60 through +0x68, computes its exact fixed-point distance from
   +0x80 through +0x88, publishes the result at +0x7C and +0x8C, and clears the field-grid dirty state.
   Local calls: WorldRuntime_ClearFieldGridDirtyFlag.
   Cross-module calls: FixedMath_Length3 [core/math/fixed].
*/
void WorldRuntime_SetPosition60AndDistanceFromPosition80
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
void WorldRuntime_SetMotionParameters6CThrough78Clamped
               (WorldMotionValue78 value78,AngleTurn32 pitchAngle,AngleTurn32 headingAngle,
               UQ12 magnitude,WorldRuntimeContext *runtime)

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
void WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
               (AngleTurn32 pitchAngle,AngleTurn32 headingAngle,UQ12 distance,Q12 originZ,
               Q12 originY,Q12 originX,WorldRuntimeContext *runtime)

{
  int extraout_ECX;
  FixedDirectionXZEdxEax8 positionOffsetXZQ12;
  
  (runtime->motion).targetPositionXQ12 = originX;
  (runtime->motion).targetPositionYQ12 = originY;
  (runtime->motion).targetPositionZQ12 = originZ;
  (runtime->motion).pitchAngle = pitchAngle;
  (runtime->motion).headingAngle = headingAngle;
  (runtime->motion).targetDistanceQ12 = distance;
  (runtime->motion).committedDistanceQ12 = distance;
  positionOffsetXZQ12 =
       FixedMath_DirectionFromAnglesScaledRegs(-pitchAngle,headingAngle ^ 0x8000,distance);
  (runtime->motion).positionXQ12 = (int)positionOffsetXZQ12 + (runtime->motion).targetPositionXQ12;
  (runtime->motion).positionYQ12 = extraout_ECX + (runtime->motion).targetPositionYQ12;
  (runtime->motion).positionZQ12 =
       (int)(positionOffsetXZQ12 >> 0x20) + (runtime->motion).targetPositionZQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(runtime);
  return;
}

/* Address: 0x0050D2C0.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime restore motion state from snapshot.
   Local calls: WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface.
*/
void WorldRuntime_RestoreMotionStateFromSnapshot(WorldRuntimeContext *worldRuntime)

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
void WorldRuntime_AttachFieldGridAsset(FieldGridAsset *asset,WorldRuntimeContext *world)

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
void WorldRuntime_AdjustFieldOriginWrappedClamped
               (undefined4 param_1,undefined4 param_2,Q12 deltaWorldY,Q12 deltaWorldX)

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
  
  QVar1 = 0x7ffff000;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    QVar1 = FieldGrid_InterpolateTerrainHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
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
  
  QVar1 = 0x7ffff000;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    QVar1 = FieldGrid_InterpolateWaterSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
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
  
  topSurfaceHeightQ12 = 0x7ffff000;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    topSurfaceHeightQ12 =
         FieldGrid_InterpolateTopSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
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
void WorldRuntimeNode_IsPositionInsideBoundsCf
               (WorldRuntimeNode *runtimeNode,WorldRuntimeExtendedMapControlAddress32 boundsControl)

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
            (&g_GraphicsProjectionScratchVec3,
             (GraphicsFixedVec3 *)((int)&runtimeNode[1].classPayload + 0x38),
             &g_ViewProjectionMatrixFixed);
  projectedPositionPair = Graphics_ProjectViewPoint(&g_GraphicsProjectionScratchVec3);
  boundsMinX = *(int *)(boundsControl + 0x160);
  iVar1 = *(int *)(boundsControl + 0x168);
  boundsMinY = *(int *)(boundsControl + 0x164);
  iVar2 = *(int *)(boundsControl + 0x16c);
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
    return;
  }
  return;
}

/* Address: 0x0050D260.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime capture motion state to snapshot.
*/
void WorldRuntime_CaptureMotionStateToSnapshot(WorldRuntimeContext *worldRuntime)

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
undefined8 WorldRuntime_MotionStateMatchesSnapshotCf(WorldRuntimeContext *worldRuntime)

{
  undefined4 in_EAX;
  undefined4 in_EDX;
  
  if ((((worldRuntime->motion).positionXQ12 == (worldRuntime->snapshot).positionXQ12) &&
      ((worldRuntime->motion).positionYQ12 == (worldRuntime->snapshot).positionYQ12)) &&
     ((worldRuntime->motion).positionZQ12 == (worldRuntime->snapshot).positionZQ12)) {
    if ((((worldRuntime->motion).positionMagnitudeQ12 == (worldRuntime->snapshot).magnitudeQ12) &&
        ((worldRuntime->motion).headingAngle == (worldRuntime->snapshot).headingAngle)) &&
       (((worldRuntime->motion).pitchAngle == (worldRuntime->snapshot).pitchAngle &&
        ((worldRuntime->motion).targetDistanceQ12 == (worldRuntime->snapshot).distanceQ12)))) {
      return CONCAT44(in_EDX,in_EAX);
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0050D4F0.
   Ownership: world/runtime/core.
   Purpose: Copies the dword at context offset 0x8C into offset 0x7C. The surrounding world-runtime layout remains
   opaque.
*/
undefined4 WorldRuntime_CommitScalar7CFrom8C(WorldRuntimeContext *world)

{
  undefined4 in_EAX;
  
  (world->motion).committedDistanceQ12 = (world->motion).targetDistanceQ12;
  return in_EAX;
}

/* Address: 0x0050D510.
   Ownership: world/runtime/core.
   Purpose: Stores arrayBase at context offset 0x58 and count at offset 0xAC. Verified callers attach arrays
   containing 0x100 or 0x4000 entries.
*/
void WorldRuntime_AttachObjectArray
               (WorldObjectRecordCount count,WorldObjectRecord *objectArray,
               WorldRuntimeContext *world)

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
undefined4 WorldRuntime_SetFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  undefined4 in_EAX;
  
  world->runtimeControlFlags = flags;
  return in_EAX;
}

/* Address: 0x0050D560.
   Ownership: world/runtime/core.
   Purpose: ORs flags into the dword at context offset 0xCC. Typed parameters: p0 flags→WorldRuntimeFlags. Nearby
   but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged.
*/
undefined4 WorldRuntime_AddFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  undefined4 in_EAX;
  
  world->runtimeControlFlags = world->runtimeControlFlags | flags;
  return in_EAX;
}

/* Address: 0x0050D580.
   Ownership: world/runtime/core.
   Purpose: Clears every bit selected by flags from the dword at context offset 0xCC. Typed parameters: p0
   flags→WorldRuntimeFlags. Nearby but non-identical semantic domains were explicitly deferred. Calling convention,
   parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
undefined4 WorldRuntime_ClearFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  undefined4 in_EAX;
  
  world->runtimeControlFlags = world->runtimeControlFlags & ~flags;
  return in_EAX;
}

/* Address: 0x0050D5A0.
   Ownership: world/runtime/core.
   Purpose: XORs flags into the dword at context offset 0xCC. Typed parameters: p0 flags→WorldRuntimeFlags. Nearby
   but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged.
*/
undefined4 WorldRuntime_ToggleFlags(WorldRuntimeFlags flags,WorldRuntimeContext *world)

{
  undefined4 in_EAX;
  
  world->runtimeControlFlags = world->runtimeControlFlags ^ flags;
  return in_EAX;
}

/* Address: 0x0050D610.
   Ownership: world/runtime/core.
   Purpose: Returns the three dwords at context offsets 0x60, 0x64, and 0x68 through EAX, ECX, and EDX
   respectively. The three-register return cannot be represented by an ordinary C prototype.
*/
undefined8 WorldRuntime_GetVector0Regs(WorldRuntimeContext *world)

{
  return CONCAT44((world->motion).positionZQ12,(world->motion).positionXQ12);
}

/* Address: 0x0050D630.
   Ownership: world/runtime/core.
   Purpose: Returns the three dwords at context offsets 0x6C, 0x70, and 0x74 through EAX, ECX, and EDX
   respectively. The three-register return cannot be represented by an ordinary C prototype.
*/
undefined8 WorldRuntime_GetVector1Regs(WorldRuntimeContext *world)

{
  return CONCAT44((world->motion).pitchAngle,(world->motion).positionMagnitudeQ12);
}

/* Address: 0x0050D650.
   Ownership: world/runtime/core.
   Purpose: Returns the dword at context offset 0xCC in EAX and explicitly clears CF.
*/
WorldRuntimeControlFlags WorldRuntime_GetFlagsCf(WorldRuntimeContext *world)

{
  return world->runtimeControlFlags;
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
void WorldRuntime_AttachAndClearDwordArray
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
int WorldObjectArray_AllocateFreeRecordCf(WorldRuntimeContext *worldRuntime)

{
  WorldObjectRecordCount recordsRemaining;
  WorldObjectRecord *recordCursor;
  
  recordsRemaining = worldRuntime->objectCount;
  recordCursor = worldRuntime->objectArray;
  while( true ) {
    if (recordsRemaining == 0) {
      return 0x14;
    }
    if (((recordCursor->common).allocationFlags & 0x40000000) == 0) break;
    recordCursor = recordCursor + 1;
    recordsRemaining = recordsRemaining - 1;
  }
  (recordCursor->common).allocationFlags = 0x40000000;
  (recordCursor->common).ownerWorld = worldRuntime;
  return (int)recordCursor;
}

/* Address: 0x0050D830.
   Ownership: world/runtime/core.
   Purpose: Sets runtime flag 0x80000000 and atomically inserts the node at the head pointer stored at owner +0xD8,
   maintaining previous and next links at node +0x00 and +0x04.
*/
undefined4 WorldRuntime_LinkNodeIntoOwnerListD8(WorldRuntimeNode *node)

{
  undefined4 in_EAX;
  WorldRuntimeNode **ownerListHeadLink;
  WorldRuntimeNode *previousHeadNode;
  WorldRuntimeContext *ownerWorld;
  
  ownerWorld = (node->common).ownerWorld;
  node->runtimeFlags = node->runtimeFlags | 0x80000000;
  LOCK();
  ownerListHeadLink = &ownerWorld->ownerListHead;
  previousHeadNode = *ownerListHeadLink;
  *ownerListHeadLink = node;
  UNLOCK();
  (node->common).previousNode = (WorldRuntimeNode *)0x0;
  (node->common).nextNode = previousHeadNode;
  if (previousHeadNode != (WorldRuntimeNode *)0x0) {
    (previousHeadNode->common).previousNode = node;
  }
  return in_EAX;
}

/* Address: 0x0050D880.
   Ownership: world/runtime/core.
   Purpose: When linked, removes the node from the owner +0xD8 intrusive list, repairs both neighbors or the head
   pointer, then clears the complete runtime flag dword at +0x4C.
*/
undefined8 WorldRuntime_UnlinkNodeFromOwnerListD8(WorldRuntimeNode *node)

{
  undefined4 in_EAX;
  undefined4 in_EDX;
  WorldRuntimeNode *previousNode;
  WorldRuntimeNode *nextNode;
  
  if ((node->runtimeFlags & 0x80000000) != 0) {
    previousNode = (node->common).previousNode;
    nextNode = (node->common).nextNode;
    if (previousNode == (WorldRuntimeNode *)0x0) {
      ((node->common).ownerWorld)->ownerListHead = nextNode;
    }
    else {
      (previousNode->common).nextNode = nextNode;
    }
    if (nextNode != (WorldRuntimeNode *)0x0) {
      (nextNode->common).previousNode = previousNode;
    }
  }
  node->runtimeFlags = 0;
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0050D8F0.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime for each node in owner list d8.
*/
void WorldRuntime_ForEachNodeInOwnerListD8
               (void *callbackContext,WorldRuntimeNodeTraversalCallback *callback,
               WorldRuntimeContext *world)

{
  WorldRuntimeNode *node;
  void *extraout_EDX;
  
  for (node = world->ownerListHead; node != (WorldRuntimeNode *)0x0; node = (node->common).nextNode)
  {
    (*callback)(callbackContext,node);
    callbackContext = extraout_EDX;
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
void WorldRuntimeNode_ClearOwnedModelReferencesCallback(void *releasedObject,WorldRuntimeNode *node)

{
  int *modelRuntime;
  int iVar1;
  int linkedRuntimeStateAddress;
  
  if (node[2].common.nextNode == (WorldRuntimeNode *)0x0) {
    modelRuntime = node->runtimePayload;
    iVar1 = ModelRuntimeHierarchy_ClearMatchingTargetRecursive
                      ((RuntimeToken)releasedObject,modelRuntime);
    linkedRuntimeStateAddress = modelRuntime[2];
    if (iVar1 == *(int *)(linkedRuntimeStateAddress + 0x98)) {
      *(undefined4 *)(linkedRuntimeStateAddress + 0x98) = 0;
    }
    if (((*(uint *)(linkedRuntimeStateAddress + 0x2c) & 1) != 0) &&
       (iVar1 == *(int *)(linkedRuntimeStateAddress + 0x1c))) {
      *(undefined4 *)(linkedRuntimeStateAddress + 0x1c) = 0;
      *(uint *)(linkedRuntimeStateAddress + 0x2c) =
           *(uint *)(linkedRuntimeStateAddress + 0x2c) & 0xfffffff2;
    }
  }
  else if ((node[2].common.nextNode == (WorldRuntimeNode *)0x2) &&
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
void WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
               (void *sourceRuntime,WorldRuntimeContext *worldRuntime)

{
  dword dVar1;
  TerrainClassOverlayCallback *pTVar2;
  int iVar3;
  ModelDefinitionRecordPrefix *pMVar4;
  WorldRuntimeNode *pWVar5;
  bool bVar6;
  dword dStack_28;
  
  bVar6 = false;
  if (sourceRuntime != (void *)0x0) {
    pMVar4 = ModelDefinitionRegistry_FindByIdWithErrorCf
                       (*(PckModelDefinitionIdCatalog *)(*(int *)((int)sourceRuntime + 0xc) + 0x20))
    ;
    if (!bVar6) {
      dStack_28 = 0xffffffff;
      pWVar5 = worldRuntime->ownerListHead;
      dVar1 = pMVar4[0x23].flags;
      if (pWVar5 != (WorldRuntimeNode *)0x0) {
        if (pMVar4[6].flags == 0xe) {
          dStack_28 = 0x800 << ((byte)pMVar4[0x10].byteSize & 0x1f);
        }
        pTVar2 = g_TerrainClassPlacementAndOverlayCallbacks10.overlayCallbacks
                 [pMVar4[0x34].definitionId];
        do {
          if (((pWVar5[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
              (worldRuntime->activeFactionRuntimeIndex ==
               *(int *)(*(int *)((int)pWVar5->runtimePayload + 8) + 0xc))) &&
             (iVar3 = *(int *)(*(int *)pWVar5->runtimePayload + 0x19c), iVar3 != 0)) {
            (*pTVar2)(dStack_28,-1,iVar3 + dVar1,(Q12)pWVar5[1].runtimePayload,
                      pWVar5[1].classPayload.model.meshGroupMask,worldRuntime->fieldGrid);
          }
          pWVar5 = (pWVar5->common).nextNode;
        } while (pWVar5 != (WorldRuntimeNode *)0x0);
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
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

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
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  return;
}

/* Address: 0x00527B70.
   Ownership: world/runtime/core.
   Purpose: Third exact one-argument no-op reused across many unified runtime object method-table entries. It
   returns with ret 0x04. Model-unrebase partition slots 48-71 receive one ModelRuntimeSlot pointer.
*/
void UnifiedRuntimeDefault_OneArgNoOpC(ModelRuntimeSlot *modelRuntime)

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
undefined4 UnifiedRuntimeDefault_OneArgReturnZero(void *context)

{
  return 0;
}

/* Address: 0x00527BE0.
   Ownership: world/runtime/core.
   Purpose: Exact two-argument default that returns CF clear with ret 0x08 while preserving EAX. Placement-
   validation partition slots 24-47 receive (worldRuntime, armyRuntime), with CF carrying acceptance.
*/
void UnifiedRuntimeDefault_TwoArgSuccessCf
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  return;
}

/* Address: 0x00527BF0.
   Ownership: world/runtime/core.
   Purpose: Fourth exact two-argument no-op reused across unified runtime object method tables. It returns with ret
   0x08. Class method-D partition slots 24-47 receive (worldRuntime, armyRuntime).
*/
void UnifiedRuntimeDefault_TwoArgNoOpD
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  return;
}

/* Address: 0x00529430.
   Ownership: world/runtime/core.
   Purpose: Owner-list callback that clears kind-specific entity/model references before the referenced runtime
   hierarchy is detached and released.
*/
void WorldRuntimeNode_ClearDetachedEntityReferencesCallback
               (void *detachedObject,WorldRuntimeNode *node)

{
  int *entityRuntimePayloadWords;
  
  if (node[2].common.nextNode == (WorldRuntimeNode *)0x2) {
    if (detachedObject == *(void **)((int)node->runtimePayload + 0x1c)) {
      *(undefined4 *)((int)node->runtimePayload + 0x1c) = 0;
    }
  }
  else if (node[2].common.nextNode == (WorldRuntimeNode *)0x0) {
    entityRuntimePayloadWords = node->runtimePayload;
    if (detachedObject == (void *)entityRuntimePayloadWords[0x3c]) {
      entityRuntimePayloadWords[0x3c] = 0;
    }
    if ((*(int *)(*entityRuntimePayloadWords + 0x4c) == 0x15) &&
       (detachedObject == (void *)entityRuntimePayloadWords[0x18])) {
      entityRuntimePayloadWords[0x18] = 0;
    }
  }
  else if ((node[2].common.nextNode == (WorldRuntimeNode *)0x1) &&
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
void WorldRuntimeNode_ReleaseShutdownBindingsCallback
               (WorldRuntimeContext *shutdownContext,WorldRuntimeNode *node)

{
  if (node[2].common.nextNode == (WorldRuntimeNode *)0x0) {
    ArmyRuntime_DestroyInstanceAndRefreshUi
              (shutdownContext,*(GameEntityRuntime **)((int)node->runtimePayload + 8));
  }
  else if (node[2].common.nextNode == (WorldRuntimeNode *)0x1) {
    node->runtimeFlags = node->runtimeFlags & 0x3fffffff;
    *(undefined4 *)((int)node->runtimePayload + 0x10) = 0;
  }
  else if (node[2].common.nextNode == (WorldRuntimeNode *)0x2) {
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
void WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 AVar1;
  UQ12 scale;
  dword dVar2;
  UQ12 extraout_ECX;
  int extraout_ECX_00;
  int iVar3;
  bool bVar4;
  ulonglong uVar5;
  FixedDirectionXZEdxEax8 FVar6;
  FixedSinCosEdxEax8 FVar7;
  
  if ((worldRuntime->runtimeFlags & 0x1000000) == 0) {
    iVar3 = worldRuntime->surfaceSelectionFlags << 2;
    bVar4 = (int)(worldRuntime->surfaceSelectionFlags << 1) < 0;
    FieldGrid_RaycastTerrainSurfaceDistanceCf
              ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,iVar3,
               (worldRuntime->motion).positionZQ12,(worldRuntime->motion).positionYQ12,
               (worldRuntime->motion).positionXQ12,worldRuntime->fieldGrid);
    if (!bVar4) {
WorldRuntime_RecomputeMotionEndpoint_UseGroundPlaneFallback:
      AVar1 = (worldRuntime->motion).pitchAngle;
      FVar7 = FixedMath_SinCosScaled
                        ((worldRuntime->motion).headingAngle,
                         (FixedMathScale32)
                         (((longlong)(worldRuntime->motion).positionZQ12 *
                          (longlong)g_FixedCosQ28[-AVar1]) / (longlong)g_FixedSinQ28[-AVar1]));
      iVar3 = (int)(FVar7 >> 0x20);
      (worldRuntime->motion).targetPositionXQ12 = (int)FVar7 + (worldRuntime->motion).positionXQ12;
      (worldRuntime->motion).targetPositionYQ12 = iVar3 + (worldRuntime->motion).positionYQ12;
      (worldRuntime->motion).targetPositionZQ12 = 0;
      dVar2 = FixedMath_Length3((worldRuntime->motion).positionZQ12,iVar3,(int)FVar7);
      (worldRuntime->motion).targetDistanceQ12 = dVar2;
      WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
      return;
    }
    uVar5 = FieldGrid_RaycastSecondarySurfaceDistanceCf
                      ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,iVar3,
                       (worldRuntime->motion).positionZQ12,(worldRuntime->motion).positionYQ12,
                       (worldRuntime->motion).positionXQ12,worldRuntime->fieldGrid);
    scale = extraout_ECX;
    if ((bVar4) && (scale = extraout_ECX, (int)(UQ12)uVar5 < (int)extraout_ECX)) {
      scale = (UQ12)uVar5;
    }
  }
  else {
    bVar4 = (int)(worldRuntime->surfaceSelectionFlags << 1) < 0;
    uVar5 = FieldGrid_RaycastSecondarySurfaceDistanceCf
                      ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,
                       worldRuntime->surfaceSelectionFlags << 2,(worldRuntime->motion).positionZQ12,
                       (worldRuntime->motion).positionYQ12,(worldRuntime->motion).positionXQ12,
                       worldRuntime->fieldGrid);
    scale = (UQ12)uVar5;
    if (!bVar4) goto WorldRuntime_RecomputeMotionEndpoint_UseGroundPlaneFallback;
  }
  (worldRuntime->motion).targetDistanceQ12 = scale;
  FVar6 = FixedMath_DirectionFromAnglesScaledRegs
                    ((worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,scale);
  (worldRuntime->motion).targetPositionXQ12 = (int)FVar6 + (worldRuntime->motion).positionXQ12;
  (worldRuntime->motion).targetPositionYQ12 = extraout_ECX_00 + (worldRuntime->motion).positionYQ12;
  (worldRuntime->motion).targetPositionZQ12 =
       (int)(FVar6 >> 0x20) + (worldRuntime->motion).positionZQ12;
  WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  return;
}

/* Address: 0x0050D760.
   Ownership: world/runtime/core.
   Purpose: Handles world runtime set terrain lighting configuration.
   Cross-module calls: TerrainLighting_BuildColorRampAndSetBaseColor [world/terrain/visuals].
*/
void WorldRuntime_SetTerrainLightingConfiguration
               (PackedArgb32 lightingColor13CArgb,PackedArgb32 lightingColor138Argb,
               PackedArgb32 lightingColor134Argb,PackedArgb32 lightingColor130Argb,
               PackedArgb32 rampColor12CArgb,PackedArgb32 lightingColor128Argb,
               PackedArgb32 rampColor124Argb,PackedArgb32 baseColorArgb,
               WorldRuntimeContext *worldRuntime)

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
void WorldRuntime_RecomputeFieldRegionNormalsAndLighting
               (FieldGridDimensionCells gridHeight,FieldGridDimensionCells gridWidth,
               Q12 originWorldYQ12,Q12 originWorldXQ12,WorldRuntimeContext *worldRuntime)

{
  AngleTurn32 lightAzimuthAngle;
  AngleTurn32 lightElevationAngle;
  
  *(Q12 *)(worldRuntime[1].interaction.reserved00_47 + 0x1c) = originWorldXQ12;
  *(Q12 *)(worldRuntime[1].interaction.reserved00_47 + 0x20) = originWorldYQ12;
  FieldGrid_RecomputeInteriorTriangleNormalAngles(worldRuntime->fieldGrid);
  FieldGrid_RecomputeInteriorDirectionalLighting
            (lightElevationAngle,lightAzimuthAngle,worldRuntime->fieldGrid);
  (worldRuntime->fieldRegion).regionWidth = gridWidth;
  (worldRuntime->fieldRegion).regionHeight = gridHeight;
  return;
}

/* Address: 0x0050D6B0.
   Ownership: world/runtime/core.
   Purpose: Clears bit 0x00000800 in the dword at context offset 0x4C.
*/
void WorldRuntime_ClearFieldGridDirtyFlag(WorldRuntimeContext *world)

{
  world->runtimeFlags = world->runtimeFlags & 0xfffff7ff;
  return;
}
