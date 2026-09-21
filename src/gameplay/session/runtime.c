/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/runtime.h>

/* Implementation ownership: gameplay/session/runtime. */

/* Address: 0x00564F70.
   Ownership: gameplay/session/runtime.
   Purpose: Chooses new-session or loaded-session initialization, processes frames until an exit flag is raised,
   performs the matching movie or frontend transition, and releases the in-game runtime resources. Typed
   parameters: p3 loadExistingSessionFlag→FrontendBooleanState32_V342. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: InGameRuntime_InitializeNewSession, InGameRuntime_InitializeLoadedSession,
   InGameRuntime_ShutdownAndReleaseResources.
   Cross-module calls: UiRootStack_InvalidateAll [ui/controls/layout], UiFrame_ProcessAndPresent
   [ui/controls/layout], GridScratch_ReleaseBuffers [world/pathing/grid],
   OldUnitRuntime_RebuildScenarioReplayTables [gameplay/faction/runtime], UiRuntime_SetSynchronizationHooks
   [ui/core/runtime], UiRootStack_PopUntilWindowTextureBoundaryCf [ui/controls/text].
*/
InGameRuntimeRunEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameRuntime_RunSessionUntilExit
          (LevelAssetRuntimeImagePrefix370 *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,word *levelPathUtf16)

{
  dword dVar1;
  InGameRuntimeInitEaxCf5 IVar2;
  InGameRuntimeLoadedInitEaxCf5 IVar3;
  InGameRuntimeRunEaxCf5 IVar4;
  InGameRuntimeRunEaxCf5 IVar5;
  InGameRuntimeRunEaxCf5 IVar6;
  InGameRuntimeRunEaxCf5 IVar7;
  
  if ((loadExistingSessionFlag & 1U) == 0) {
    IVar2 = InGameRuntime_InitializeNewSession(levelAsset,levelPathUtf16);
    dVar1 = IVar2.runtimeRootOrError;
    if (IVar2.carry)
    goto InGameRuntime_RunSessionUntilExit_ShutdownAndReturnStartupOrUiRootFailureWithCarrySet;
  }
  else {
    IVar3 = InGameRuntime_InitializeLoadedSession(levelPathUtf16);
    dVar1 = IVar3.runtimeRootOrError;
    if (IVar3.carry)
    goto InGameRuntime_RunSessionUntilExit_ShutdownAndReturnStartupOrUiRootFailureWithCarrySet;
  }
  do {
    g_InGamePendingSimulationTicks = g_InGamePendingSimulationTicks + -2;
    if (g_InGamePendingSimulationTicks < 0) {
      g_InGamePendingSimulationTicks = 0;
    }
    UiRootStack_InvalidateAll();
    UiFrame_ProcessAndPresent();
    if ((g_UiCommandRuntimeFlags & 0x10000) != 0) {
      (*g_SoundStopAllVoices)();
      (*g_TimerUnregisterPeriodic)(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      g_EndMovieSelectionIndex = 0;
      OldUnitRuntime_RebuildScenarioReplayTables();
      UiRuntime_SetSynchronizationHooks
                ((UiRuntimePostUnlockCallbackProc *)0x0,(RuntimeSpinLockValue *)0x0);
      UiRootStack_PopUntilWindowTextureBoundaryCf();
      InGameRuntime_ShutdownAndReleaseResources();
      IVar5.exitCodeOrError = 0xc;
      IVar5.carry = false;
      return IVar5;
    }
    if ((g_UiCommandRuntimeFlags & 0x800) != 0) {
      (*g_SoundStopAllVoices)();
      (*g_TimerUnregisterPeriodic)(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      UiRuntime_SetSynchronizationHooks(FrontendSession_PeriodicTick,&g_InGameStateTickSpinLock);
      Frontend_PlaySelectedEndMovie();
      OldUnitRuntime_RebuildScenarioReplayTables();
      UiRuntime_SetSynchronizationHooks
                ((UiRuntimePostUnlockCallbackProc *)0x0,(RuntimeSpinLockValue *)0x0);
      UiRootStack_PopUntilWindowTextureBoundaryCf();
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16 = 0;
      IVar6.exitCodeOrError = 0xc;
      IVar6.carry = false;
      return IVar6;
    }
    if ((g_UiCommandRuntimeFlags & 0x20000) != 0) {
      (*g_SoundStopAllVoices)();
      (*g_TimerUnregisterPeriodic)(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      UiRuntime_SetSynchronizationHooks
                ((UiRuntimePostUnlockCallbackProc *)0x0,(RuntimeSpinLockValue *)0x0);
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16 = 0;
      IVar4.exitCodeOrError = 0xc;
      IVar4.carry = false;
      return IVar4;
    }
  } while (g_UiRootNode != (UiRootNode *)0xffffffff);
  dVar1 = 0x14;
InGameRuntime_RunSessionUntilExit_ShutdownAndReturnStartupOrUiRootFailureWithCarrySet:
  InGameRuntime_ShutdownAndReleaseResources();
  IVar7.carry = true;
  IVar7.exitCodeOrError = dVar1;
  return IVar7;
}


/* Address: 0x00566290.
   Ownership: gameplay/session/runtime.
   Purpose: End-game results update callback that advances presentation state and handles current input.
   Local calls: InGameRuntime_UpdateCursorGridAndViewScaleCache.
   Cross-module calls: FrontendClientSession_DecrementTimeoutsAndCompactPlayers [ui/frontend/session],
   FrontendHostSession_TickShutdownOrReadyConsensus [ui/frontend/session], FieldGrid_SetAllCellOverlayColors
   [world/terrain/grid], WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries [world/runtime/core],
   RecentTextHistory_SortAndBuildPointerList [ui/support/runtime], InGameHud_UpdateStatusCountersAndSessionPrompts
   [ui/ingame/runtime].
*/

void __thandor_void_preserve_eax_ecx_edx
EndGameResultsUiRuntime_UpdateAndHandleInputCf(EndGameResultsRuntimeView44C4 *endGameResultsRuntime)

{
  Q12 *pQVar1;
  WorldMotionState *pWVar2;
  FieldGridAsset *pFVar3;
  ArmyRuntimeSlot *armyRuntime;
  LevelMusicSampleNumber LVar4;
  longlong lVar5;
  InGameLevelConditionStorageView800 *pIVar6;
  UiNodeBase *pUVar7;
  dword dVar8;
  int iVar9;
  AngleTurn32 AVar10;
  UQ12 UVar11;
  int iVar12;
  uint uVar13;
  InGamePresentationTick IVar14;
  int iVar15;
  WorldRuntimeContext *worldRuntime;
  int iVar16;
  int iVar17;
  uint uVar18;
  uint uVar19;
  InGameConditionScheduleImageView480 *pIVar20;
  bool bVar21;
  FieldGridCoordinatesEaxEdx8 FVar22;
  StatusValueEaxCf5 SVar23;
  SoundPlayVoiceEaxCf5 SVar24;
  dword dVar25;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      FrontendClientSession_DecrementTimeoutsAndCompactPlayers();
    }
  }
  else {
    FrontendHostSession_TickShutdownOrReadyConsensus();
  }
  if ((g_UiCommandRuntimeFlags & 0x10) == 0) {
    if ((g_UiCommandRuntimeFlags & 0x2000) == 0) {
      if ((g_UiCommandRuntimeFlags & 0x20) != 0) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x2000;
        FieldGrid_SetAllCellOverlayColors
                  (0xff808080,(endGameResultsRuntime->worldRuntime0A30).fieldGrid);
        WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
                  ((void *)g_InGamePendingPlacementArmyAsset,
                   &endGameResultsRuntime->worldRuntime0A30);
      }
    }
    else if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xffffdfff;
      FieldGrid_SetAllCellOverlayColors
                (0xffffffff,(endGameResultsRuntime->worldRuntime0A30).fieldGrid);
    }
    else if ((g_GameFactionRuntimeImage.tail.simulationTick & 7) == 0) {
      FieldGrid_SetAllCellOverlayColors
                (0xff808080,(endGameResultsRuntime->worldRuntime0A30).fieldGrid);
      WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
                ((void *)g_InGamePendingPlacementArmyAsset,&endGameResultsRuntime->worldRuntime0A30)
      ;
    }
    dVar25 = 0;
    pUVar7 = (*((endGameResultsRuntime->rootUi0000).base.vtable)->hitTest)
                       (g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)endGameResultsRuntime);
    if (pUVar7 != (UiNodeBase *)0xffffffff) {
      dVar25 = (*pUVar7->vtable->pointerMove)(g_CursorOverrideY,g_CursorOverrideX,pUVar7);
    }
    g_GameFactionRuntimeImage.tail.presentationTick =
         g_GameFactionRuntimeImage.tail.presentationTick + 1;
    RecentTextHistory_SortAndBuildPointerList(8,&endGameResultsRuntime->recentTextHistory09B8);
    IVar14 = g_GameFactionRuntimeImage.tail.presentationTick;
    worldRuntime = &endGameResultsRuntime->worldRuntime0A30;
    InGameHud_UpdateStatusCountersAndSessionPrompts();
    SVar23 = UiPageStack_ActivePageNotInListCf(&endGameResultsRuntime->endGameResultsPageStack09DC);
    if ((((((endGameResultsRuntime->worldRuntime0A30).runtimeFlags & 0x90) == 0) &&
         (SVar23.valueOrError == 0)) &&
        (((endGameResultsRuntime->worldRuntime0A30).interaction.interactionFlags48 & 8) == 0)) &&
       (((g_CursorButtonState & 4) == 0 &&
        (dVar8 = WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(worldRuntime), dVar8 != 0)))) {
      dVar25 = dVar8;
    }
    (*g_GraphicsCursorSetFrame)(dVar25);
    pFVar3 = (endGameResultsRuntime->worldRuntime0A30).fieldGrid;
    iVar17 = 0;
    FVar22 = FieldGrid_WorldToGridQ12
                       ((endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                        (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12);
    iVar15 = FVar22.rowQ12;
    iVar9 = FVar22.columnQ12;
    iVar16 = (iVar9 >> 0xc) + -8;
    iVar12 = (iVar15 >> 0xc) + -8;
    if (iVar16 < -0x10) {
      iVar9 = -0x8000;
      iVar17 = 1;
    }
    else if ((int)pFVar3->gridWidth < iVar16) {
      iVar17 = 1;
      iVar9 = (pFVar3->gridWidth + 8) * 0x1000;
    }
    if (iVar12 < -0x10) {
      iVar15 = -0x8000;
      iVar17 = iVar17 + 1;
    }
    else if ((int)pFVar3->gridHeight < iVar12) {
      iVar17 = iVar17 + 1;
      iVar15 = (pFVar3->gridHeight + 8) * 0x1000;
    }
    if (iVar17 != 0) {
      lVar5 = (longlong)(iVar15 + iVar9 * 2) * 0x901;
      iVar9 = ((int)((ulonglong)lVar5 >> 0x20) << 0x13 | (uint)lVar5 >> 0xd) -
              (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12;
      iVar12 = ((int)((ulonglong)((longlong)iVar15 * -1999) >> 0x20) << 0x14 |
               (uint)((longlong)iVar15 * -1999) >> 0xc) -
               (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12;
      pQVar1 = &(endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12;
      *pQVar1 = *pQVar1 + iVar9;
      pQVar1 = &(endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12;
      *pQVar1 = *pQVar1 + iVar12;
      pWVar2 = &(endGameResultsRuntime->worldRuntime0A30).motion;
      pWVar2->positionXQ12 = pWVar2->positionXQ12 + iVar9;
      pQVar1 = &(endGameResultsRuntime->worldRuntime0A30).motion.positionYQ12;
      *pQVar1 = *pQVar1 + iVar12;
      WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
    }
    if ((g_UiCommandRuntimeFlags & 4) == 0) {
      TerrainDirectionTable_AdvanceAndRebuildVectors();
      dVar25 = PersistentSettings_ReadDword(3,0x20);
      if ((dVar25 & 1) != 0) {
        if ((IVar14 & 7) == 0) {
          SpatialSoundPool_ClearDesiredGains();
          for (armyRuntime = (ArmyRuntimeSlot *)
                             (endGameResultsRuntime->worldRuntime0A30).ownerListHead;
              armyRuntime != (ArmyRuntimeSlot *)0x0;
              armyRuntime = (ArmyRuntimeSlot *)armyRuntime->modelNodeRuntime) {
            (*(&g_RuntimeMaintenanceCallbackPhases.audioRefresh.army)[armyRuntime->runtimeStateA4])
                      (worldRuntime,armyRuntime);
          }
          SpatialSoundPool_ApplyDesiredGains();
          InGameSelectionDetailPanel_Rebuild();
        }
        if (g_InGameEffectsEnabled == 0) {
          bVar21 = (*g_SoundIsVoicePlaying)(g_InGameActiveEffectVoice);
          if (bVar21) {
            g_InGameActiveEffectVoice = (IDirectSoundBuffer *)0x0;
            dVar25 = Random_NextPrimary();
            g_InGameEffectsEnabled = (dVar25 & 0x3f) + 1;
          }
        }
        else {
          g_InGameEffectsEnabled = g_InGameEffectsEnabled - 1;
          if (g_InGameEffectsEnabled == 0) {
            dVar25 = PersistentSettings_ReadDword(0x8000,0x24);
            dVar8 = Random_NextPrimary();
            SVar24 = (*g_SoundPlayOneShot)
                               (dVar25,dVar25,
                                (DirectSoundVoiceSet *)(&g_InGameLevelEffectVoiceSet0)[dVar8 & 3]);
            if (!SVar24.carry) {
              g_InGameActiveEffectVoice = SVar24.eax;
            }
          }
        }
      }
      dVar25 = PersistentSettings_ReadDword(3,0x20);
      pIVar6 = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
      if ((dVar25 & 2) != 0) {
        if (g_InGameMusicEnabled == 0) {
          bVar21 = (*g_SoundIsVoicePlaying)(g_InGameActiveMusicVoice);
          if (bVar21) {
            g_InGameActiveMusicVoice = (IDirectSoundBuffer *)0x0;
            dVar25 = Random_NextPrimary();
            g_InGameMusicEnabled = (dVar25 & 0x3f) + 1;
          }
        }
        else {
          g_InGameMusicEnabled = g_InGameMusicEnabled - 1;
          if (g_InGameMusicEnabled == 0) {
            dVar25 = 0;
            uVar13 = 0;
            uVar18 = 0;
            do {
              dVar8 = InGameMusic_ComputeTrackSuitabilityScore
                                ((pIVar6->levelImage).runtimeTail2E0.musicSampleNumbers[uVar18],
                                 worldRuntime);
              uVar19 = uVar18 + 1;
              if ((int)dVar25 < (int)dVar8) {
                uVar13 = uVar18;
                dVar25 = dVar8;
              }
              uVar18 = uVar19;
            } while (uVar19 < 4);
            if (dVar25 != 0) {
              LVar4 = (pIVar6->levelImage).runtimeTail2E0.musicSampleNumbers[uVar13];
              dVar25 = PersistentSettings_ReadDword(0x8000,0x2c);
              g_EndGameResultsCurrentMusicTrackId = LVar4;
              SVar24 = (*g_SoundPlayOneShot)
                                 (dVar25,dVar25,
                                  (DirectSoundVoiceSet *)(&g_InGameLevelMusicVoiceSet0)[uVar13]);
              if (!SVar24.carry) {
                g_InGameActiveMusicVoice = SVar24.eax;
              }
            }
          }
        }
      }
      if ((g_UiCommandRuntimeFlags & 1) != 0)
      goto EndGameResultsUiRuntime_UpdateAndHandleInput_UpdateCursorGridAndReturn;
      SVar23 = UiPageStack_ActivePageNotInListCf(&endGameResultsRuntime->technologyPageStack0BD0);
      if (SVar23.valueOrError == 2) {
        InGameTechnologyPanel_Rebuild(&endGameResultsRuntime->rootUi0000);
      }
      InterpolationStateTable_Advance256ByTicks(g_InGameSimulationStepTicks);
      if (g_KeyboardSpecialKeyDown[0x14] != 0) {
        dVar25 = PersistentSettings_ReadDword(0x20,0x48);
        WorldRuntime_TranslateCameraByScreenDelta(0,-dVar25,worldRuntime);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      }
      if (g_KeyboardSpecialKeyDown[0x16] != 0) {
        dVar25 = PersistentSettings_ReadDword(0x20,0x48);
        WorldRuntime_TranslateCameraByScreenDelta(0,dVar25,worldRuntime);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      }
      if (g_KeyboardSpecialKeyDown[0x11] != 0) {
        dVar25 = PersistentSettings_ReadDword(0x20,0x48);
        WorldRuntime_TranslateCameraByScreenDelta(-dVar25,0,worldRuntime);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      }
      if (g_KeyboardSpecialKeyDown[0x19] != 0) {
        dVar25 = PersistentSettings_ReadDword(0x20,0x48);
        WorldRuntime_TranslateCameraByScreenDelta(dVar25,0,worldRuntime);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      }
      if (g_KeyboardSpecialKeyDown[0x12] != 0) {
        AVar10 = (endGameResultsRuntime->worldRuntime0A30).motion.pitchAngle - 0x400;
        if ((int)AVar10 < (int)(endGameResultsRuntime->worldRuntime0A30).motion.minimumPitchAngle) {
          AVar10 = (endGameResultsRuntime->worldRuntime0A30).motion.minimumPitchAngle;
        }
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  (AVar10,(endGameResultsRuntime->worldRuntime0A30).motion.headingAngle,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetDistanceQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionZQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[0x1a] != 0) {
        AVar10 = (endGameResultsRuntime->worldRuntime0A30).motion.pitchAngle + 0x400;
        if ((int)(endGameResultsRuntime->worldRuntime0A30).motion.maximumPitchAngle < (int)AVar10) {
          AVar10 = (endGameResultsRuntime->worldRuntime0A30).motion.maximumPitchAngle;
        }
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  (AVar10,(endGameResultsRuntime->worldRuntime0A30).motion.headingAngle,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetDistanceQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionZQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[7] != 0) {
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  ((endGameResultsRuntime->worldRuntime0A30).motion.pitchAngle,
                   (endGameResultsRuntime->worldRuntime0A30).motion.headingAngle - 0x400 & 0xffff,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetDistanceQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionZQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[6] != 0) {
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  ((endGameResultsRuntime->worldRuntime0A30).motion.pitchAngle,
                   (endGameResultsRuntime->worldRuntime0A30).motion.headingAngle + 0x400 & 0xffff,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetDistanceQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionZQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[0x10] != 0) {
        UVar11 = (endGameResultsRuntime->worldRuntime0A30).motion.targetDistanceQ12 - 0x800;
        if ((int)UVar11 < (int)(endGameResultsRuntime->worldRuntime0A30).minimumCameraDistanceQ12) {
          UVar11 = (endGameResultsRuntime->worldRuntime0A30).minimumCameraDistanceQ12;
        }
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  ((endGameResultsRuntime->worldRuntime0A30).motion.pitchAngle,
                   (endGameResultsRuntime->worldRuntime0A30).motion.headingAngle,UVar11,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionZQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[0x18] != 0) {
        UVar11 = (endGameResultsRuntime->worldRuntime0A30).motion.targetDistanceQ12 + 0x800;
        if ((int)(endGameResultsRuntime->worldRuntime0A30).maximumCameraDistanceQ12 < (int)UVar11) {
          UVar11 = (endGameResultsRuntime->worldRuntime0A30).maximumCameraDistanceQ12;
        }
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  ((endGameResultsRuntime->worldRuntime0A30).motion.pitchAngle,
                   (endGameResultsRuntime->worldRuntime0A30).motion.headingAngle,UVar11,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionZQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      pIVar6 = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
      endGameResultsRuntime->sessionTimerNodeFlags44C0 =
           endGameResultsRuntime->sessionTimerNodeFlags44C0 & ~UI_NODE_SUPPRESSED;
      IVar14 = g_GameFactionRuntimeImage.tail.presentationTick;
      iVar9 = 0x40;
      pIVar20 = &pIVar6->schedule;
      do {
        if (((pIVar20->conditions[0].statusAndKind.kind & 0xfe) ==
             INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED) &&
           (uVar13 = pIVar20->conditions[0].payload.operands[1], uVar13 != 0)) {
          dVar25 = (*g_WideNumberFormatUtf16)
                             (WIDE_FORMAT_PAD_WITH_SPACE,0,2,1,uVar13 / 0x3c,(word *)0x550590);
          *(undefined2 *)(dVar25 + 0x550590) = 0x3a;
          (*g_WideNumberFormatUtf16)
                    (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,uVar13 % 0x3c,
                     (word *)(dVar25 + 0x550592));
          IVar14 = g_GameFactionRuntimeImage.tail.presentationTick;
          goto 
          EndGameResultsUiRuntime_UpdateAndHandleInput_RefreshTerrainCompositeOnPresentationCadence;
        }
        pIVar20 = (InGameConditionScheduleImageView480 *)(pIVar20->conditions + 1);
        iVar9 = iVar9 + -1;
      } while (iVar9 != 0);
      endGameResultsRuntime->sessionTimerNodeFlags44C0 =
           endGameResultsRuntime->sessionTimerNodeFlags44C0 | UI_NODE_SUPPRESSED;
    }
EndGameResultsUiRuntime_UpdateAndHandleInput_RefreshTerrainCompositeOnPresentationCadence:
    if ((IVar14 & 0x1f) == 0) {
      TerrainCompositeTexture_FillPlane1();
    }
    if ((IVar14 & 3) == 0) {
      TerrainCompositeTexture_RebuildPlane0();
    }
  }
EndGameResultsUiRuntime_UpdateAndHandleInput_UpdateCursorGridAndReturn:
  InGameRuntime_UpdateCursorGridAndViewScaleCache();
  return;
}


/* Address: 0x0050EA90.
   Ownership: gameplay/session/runtime.
   Purpose: Rebases exact 0x100-byte loaded condition records. RET 4 proves one stack argument and removes two
   false register parameters.
   Cross-module calls: SpriteAssetRegistry_FindById [assets/sprite/catalog].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameConditionRuntime_RebaseLoadedRecords(InGameConditionRuntime *runtime)

{
  FrontendPlayerRuntimeRecord *pFVar1;
  byte *pbVar2;
  void *pvVar3;
  GraphicsTextureSet *pGVar4;
  SpriteAssetHeader *pSVar5;
  byte *pbVar6;
  dword dVar7;
  GraphicsPaletteAsset *pGVar8;
  InGameConditionRecord *pIVar9;
  InGameConditionRecordCount IVar10;
  byte *pbVar11;
  InGameConditionRecord *pIVar12;
  ModelRuntimeSlot *modelSlot1;
  
  pIVar9 = runtime->records58;
  IVar10 = runtime->recordCountAC;
  do {
    if ((pIVar9->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) != 0) {
      pbVar2 = pIVar9->primaryRuntimePointer00;
      pbVar6 = pIVar9->secondaryRuntimePointer04;
      pbVar11 = pIVar9->nestedBaseC4;
      if (pbVar2 != (byte *)0x0) {
        pbVar2 = pbVar2 + (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      if (pbVar6 != (byte *)0x0) {
        pbVar6 = pbVar6 + (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      if (pbVar11 != (byte *)0x0) {
        pbVar11 = pbVar11 + (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      pIVar9->primaryRuntimePointer00 = pbVar2;
      pIVar9->secondaryRuntimePointer04 = pbVar6;
      pIVar9->nestedBaseC4 = pbVar11;
      pIVar9->ownerRuntime08 = runtime;
      pvVar3 = pIVar9->auxiliaryPointer5C;
      dVar7 = pIVar9->nestedCountC8;
      if (pvVar3 != (void *)0x0) {
        pvVar3 = (void *)((int)pvVar3 + 0x4bed4f);
      }
      pIVar9->auxiliaryPointer5C = pvVar3;
      pIVar12 = pIVar9;
      pGVar8 = g_ShotPalette;
      for (; g_ShotPalette = pGVar8, dVar7 != 0; dVar7 = dVar7 - 1) {
        if (pIVar12->nestedPointersCC[0] != (void *)0x0) {
          pIVar12->nestedPointersCC[0] =
               (byte *)((int)pIVar12->nestedPointersCC[0] + (int)g_RuntimeObjectRebaseBaseMinusOne);
        }
        pIVar12 = (InGameConditionRecord *)&pIVar12->secondaryRuntimePointer04;
        pGVar8 = g_ShotPalette;
      }
      modelSlot1 = (pIVar9->payload48).modelRuntime;
                    // WARNING: Switch is manually overridden
      switch(pIVar9->conditionKindA4) {
      case MODEL_RUNTIME:
        modelSlot1 = (ModelRuntimeSlot *)
                     (modelSlot1->reserved10_37 + g_ModelRuntimeRebaseDelta + -0x10);
        pGVar8 = g_ArmyGraphicsBindings[(int)pIVar9->textureSet34].paletteAsset;
        pIVar9->textureSet34 = g_ArmyGraphicsBindings[(int)pIVar9->textureSet34].textureSet;
        pIVar9->paletteAsset30 = pGVar8;
        break;
      case SHOT_RUNTIME:
        modelSlot1 = (ModelRuntimeSlot *)
                     (g_ShotRuntimeRebaseBaseMinusOne + (int)(modelSlot1->reserved10_37 + -0x10));
        pIVar9->textureSet34 = g_ShotTextureSet;
        pIVar9->paletteAsset30 = pGVar8;
        break;
      case EFFECT_RUNTIME:
        modelSlot1 = (ModelRuntimeSlot *)
                     (g_EffectRuntimeRebaseBaseMinusOne + (int)(modelSlot1->reserved10_37 + -0x10));
        pGVar4 = g_EffectTextureSet;
        pGVar8 = g_EffectPalette;
        if ((*(uint *)((modelSlot1->definitionOrSavedId).savedIdOrOffset + 0x30) & 2) != 0) {
          pGVar4 = g_ArmyGraphicsBindings[0].textureSet;
          pGVar8 = g_ArmyGraphicsBindings[0].paletteAsset;
        }
        pIVar9->textureSet34 = pGVar4;
        pIVar9->paletteAsset30 = pGVar8;
      }
      (pIVar9->payload48).modelRuntime = modelSlot1;
      pSVar5 = SpriteAssetRegistry_FindById((SpriteAssetId)pIVar9->spriteAsset40);
      pIVar9->spriteAsset40 = pSVar5;
    }
    pFVar1 = g_FrontendPlayerRuntimeBlocks;
    pIVar9 = pIVar9 + 1;
    IVar10 = IVar10 - 1;
  } while (IVar10 != 0);
  pvVar3 = runtime->records58[runtime->recordCountAC - 1].nestedPointersCC[0xc];
  pIVar9 = (InGameConditionRecord *)0x0;
  if (pvVar3 != (void *)0x0) {
    pIVar9 = (InGameConditionRecord *)(g_RuntimeObjectRebaseBaseMinusOne + (int)pvVar3);
  }
  runtime->tailRecordD8 = pIVar9;
  (pFVar1->factionAssignment).factionAssignmentIndex =
       (FrontendFactionAssignmentIndex)runtime->worldContext50;
  return;
}


/* Address: 0x00565E10.
   Ownership: gameplay/session/runtime.
   Purpose: In-game periodic timer callback. It decrements the startup/countdown field when nonzero and advances
   the shared runtime clock while the pause byte is clear.
*/
void __cdecl InGameRuntime_PeriodicCountdownAndClockTick(void)

{
  if (g_InGameNetworkTickCountdown != 0) {
    g_InGameNetworkTickCountdown = g_InGameNetworkTickCountdown + -1;
  }
  if (g_InGameResourceRegistrationBusyCount == '\0') {
    g_GameFactionRuntimeImage.tail.periodicClockTick =
         g_GameFactionRuntimeImage.tail.periodicClockTick + 1;
  }
  return;
}

/* Address: 0x00567060.
   Ownership: gameplay/session/runtime.
   Purpose: End-game results command dispatcher selected by command code and modifier flags. Typed parameters: p0
   modifierFlags→UiKeyboardStateMask_V297, p1 commandCode→UiActionId_V338. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
EndGameResultsUiRuntime_DispatchCommandByFlagsCf
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          EndGameResultsRuntimeView44C4 *endGameResultsRuntime)

{
  uint uVar1;
  UiCommandDispatchRecord *pUVar2;
  UiCommandDispatchRecord *pUVar3;
  bool bVar4;
  
  pUVar3 = g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30;
  do {
    while( true ) {
      while( true ) {
        while( true ) {
          do {
            pUVar2 = pUVar3;
            uVar1 = pUVar2->modifierClassFlags;
            if (pUVar2->commandCode == 0) {
              return false;
            }
            pUVar3 = pUVar2 + 1;
          } while (pUVar2->commandCode != commandCode);
          if (uVar1 != 0) break;
          bVar4 = false;
          if ((modifierFlags & 0x3c) == 0) {
            (*(code *)pUVar2->continuationEntryAddress)();
            return bVar4;
          }
        }
        if ((uVar1 & 0x30) != 0) break;
        if (((modifierFlags & 0xc) != 0) && (bVar4 = false, (modifierFlags & 0x30) == 0)) {
          (*(code *)pUVar2->continuationEntryAddress)();
          return bVar4;
        }
      }
      if ((uVar1 & 0xc) != 0) break;
      if (((modifierFlags & 0xc) == 0) && (bVar4 = false, (modifierFlags & 0x30) != 0)) {
        (*(code *)pUVar2->continuationEntryAddress)();
        return bVar4;
      }
    }
  } while (((modifierFlags & 0xc) == 0) || (bVar4 = false, (modifierFlags & 0x30) == 0));
  (*(code *)pUVar2->continuationEntryAddress)();
  return bVar4;
}


/* Address: 0x00569920.
   Ownership: gameplay/session/runtime.
   Purpose: Ten-millisecond gameplay timer that advances queued session notification and status records.
   Cross-module calls: Movie_Open [movie/runtime/playback], Movie_SetAudioGainQ15 [movie/runtime/playback],
   Movie_AdvanceFrame [movie/runtime/playback], Movie_Close [movie/runtime/playback].
*/
void __thandor_void_preserve_eax_ecx_edx InGameRuntime_ProcessQueuedSessionNotificationTimer(void)

{
  uint arg4;
  GraphicsTextureSourceAsset *pGVar1;
  int iVar2;
  InGameNotificationPayload18 *pIVar3;
  InGameNotificationQueueRecord20 *pIVar4;
  InGameNotificationPayload18 *pIVar5;
  InGameNotificationQueueRecord20 *pIVar6;
  MovieAdvanceFrameEaxCf5 MVar7;
  MovieOpenEaxCf5 MVar8;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  
  pGVar1 = g_InGamePanelTextureSource;
  gameRuntime1 = g_InGameRuntimeRoot;
  if (((g_InGameSessionNotificationTimeoutTicks != 0) &&
      (g_InGameSessionNotificationTimeoutTicks = g_InGameSessionNotificationTimeoutTicks - 1,
      g_InGameSessionNotificationTimeoutTicks == 0)) &&
     (g_InGameRuntimeRoot->sessionNotificationInteractionState9B4C == PAYLOAD_ACTIVE)) {
    g_InGameRuntimeRoot->sessionNotificationInteractionState9B4C = NONE;
  }
  if (pGVar1 == (GraphicsTextureSourceAsset *)gameRuntime1->observedSessionNotificationValue9B50) {
    if (gameRuntime1->notificationQueue9E60[0].priority04 != 0) {
      arg4 = gameRuntime1->notificationQueue9E60[0].notificationMovieId00;
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_PAD_WITH_ZERO,0,3,1,arg4,(word *)(u_flm_movie000_flm_0056314e + 9));
      MVar8 = Movie_Open(0x80000000,(word *)u_flm_movie000_flm_0056314e);
      if (!MVar8.carry) {
        if ((99 < arg4) && ((arg4 < 300 || ((699 < arg4 && (arg4 < 900)))))) {
          Movie_SetAudioGainQ15(g_MovieAlternateAudioGainQ15);
        }
        MVar7 = Movie_AdvanceFrame();
        if (!MVar7.carry) {
          gameRuntime1->observedSessionNotificationValue9B50 = MVar7.eax;
          gameRuntime1->notificationPlaybackCompletionCode9B54 = 0;
          pIVar3 = &gameRuntime1->notificationQueue9E60[0].payload08;
          pIVar5 = &gameRuntime1->activeNotificationPayload9E40;
          for (iVar2 = 6; iVar2 != 0; iVar2 = iVar2 + -1) {
            pIVar5->primaryWorldCoordinateQ12_00 = pIVar3->primaryWorldCoordinateQ12_00;
            pIVar3 = (InGameNotificationPayload18 *)&pIVar3->secondaryWorldCoordinateQ12_04;
            pIVar5 = (InGameNotificationPayload18 *)&pIVar5->secondaryWorldCoordinateQ12_04;
          }
          if (gameRuntime1->sessionNotificationInteractionState9B4C == PAYLOAD_ACTIVE) {
            gameRuntime1->sessionNotificationInteractionState9B4C = NONE;
          }
          g_InGameSessionNotificationTimeoutTicks = 0;
          if ((gameRuntime1->activeNotificationPayload9E40).payloadKind14 != NONE) {
            gameRuntime1->sessionNotificationInteractionState9B4C = PAYLOAD_ACTIVE;
          }
        }
      }
      pIVar4 = gameRuntime1->notificationQueue9E60 + 1;
      pIVar6 = gameRuntime1->notificationQueue9E60;
      for (iVar2 = 0x18; iVar2 != 0; iVar2 = iVar2 + -1) {
        pIVar6->notificationMovieId00 = pIVar4->notificationMovieId00;
        pIVar4 = (InGameNotificationQueueRecord20 *)&pIVar4->priority04;
        pIVar6 = (InGameNotificationQueueRecord20 *)&pIVar6->priority04;
      }
      for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
        pIVar6->notificationMovieId00 = 0;
        pIVar6 = (InGameNotificationQueueRecord20 *)&pIVar6->priority04;
      }
    }
  }
  else {
    MVar7 = Movie_AdvanceFrame();
    if (MVar7.carry) {
      Movie_Close();
      g_InGameSessionNotificationTimeoutTicks = 0x280;
      gameRuntime1->observedSessionNotificationValue9B50 = (dword)pGVar1;
      gameRuntime1->notificationPlaybackCompletionCode9B54 = 0x25;
    }
  }
  return;
}


/* Address: 0x005641D0.
   Ownership: gameplay/session/runtime.
   Purpose: Initializes a new in-game session from a level asset, builds player and UI runtime state, opens the
   level movie, prepares world, terrain, shading, technology, army, model, shot, effect, and audio resources, and
   reports failure through CF.
   Local calls: InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf,
   InGameRuntime_UpdateSimulationAndNetworkTick.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiRuntime_SetSynchronizationHooks
   [ui/core/runtime], TextResource_Resolve [assets/text/resources], SelectionInfoPanel_InitResources
   [gameplay/selection/runtime], InGameUiRuntime_InitializeControlTreeResourcesCf [ui/ingame/runtime],
   UiRootStack_Push [ui/controls/layout].
*/
InGameRuntimeInitEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameRuntime_InitializeNewSession(LevelAssetRuntimeImagePrefix370 *levelAsset,word *levelMoviePath)

{
  WorldRuntimeFlags *pWVar1;
  WorldRuntimeContext *world;
  word wVar2;
  UiTextResourceId UVar3;
  PlayerRuntimeId PVar4;
  word *pwVar5;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  dword dVar6;
  dword gridHalfSize;
  dword subresourceCount;
  int iVar7;
  FrontendPlayerRuntimeBlockCount FVar8;
  InGameNotificationMovieId notificationMovieId;
  InGameNotificationMovieId notificationMovieId_00;
  SelectionPlayerRuntimeBlock *pSVar9;
  FrontendPlayerNameUtf16_28 *pFVar10;
  undefined4 *puVar11;
  FrontendPlayerRemovalPacket10007 *pFVar12;
  SelectionPlayerRuntimeBlock *pSVar13;
  FrontendPlayerRuntimeRecord *pFVar14;
  byte *pbVar15;
  undefined2 *puVar16;
  word *pwVar17;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  InGameNotificationQueueRecord20 *pIVar18;
  bool bVar19;
  TextResourceResolveEaxCf5 TVar20;
  ArenaAllocEaxCf5 AVar21;
  StatusValueEaxCf5 SVar22;
  EndingMoviePathEaxCf5 EVar23;
  MovieOpenEaxCf5 MVar24;
  MovieAdvanceFrameEaxCf5 MVar25;
  InGameLevelDefaultLoadEaxCf5 IVar26;
  GridScratchAllocEaxCf5 GVar27;
  InGameRuntimeInitEaxCf5 IVar28;
  
  g_UiCommandRuntimeFlags = 0x11;
  g_SessionNetworkTickCounter = 1;
  g_HostCommandBatchSyncSentThisInterval = 0;
  g_GameFactionRuntimeImage.tail.simulationTick = 1;
  g_GameFactionRuntimeImage.tail.presentationTick = 0;
  g_InGameSessionNotificationTimeoutTicks = 0;
  g_InGameReadyStateToggleFlags = 0;
  g_EndGameResultsCurrentMusicTrackId = 0;
  g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
  g_InGameSessionStartedNetworked =
       (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
       SESSION_NETWORK_ROLE_LOCAL;
  g_TextureDownsampleShift = PersistentSettings_ReadDword(0,0x30);
  g_EndMovieSelectionIndex = 0xffffffff;
  g_EndMovieVariantIndex = 0;
  g_EndMoviePath = (word *)0x0;
  pFVar12 = &g_FrontendClientPlayerRemovalPacket10007;
  for (iVar7 = 0xa0; iVar7 != 0; iVar7 = iVar7 + -1) {
    (pFVar12->header).packedTypeAndUnitCount = 0;
    pFVar12 = (FrontendPlayerRemovalPacket10007 *)&(pFVar12->header).sequenceToken;
  }
  pSVar13 = g_SelectionPlayerBlocks;
  for (iVar7 = 0x10230; FVar8 = g_FrontendPlayerRuntimeBlockCount, pSVar9 = g_SelectionPlayerBlocks,
      pFVar14 = g_FrontendPlayerRuntimeBlocks, iVar7 != 0; iVar7 = iVar7 + -1) {
    (pSVar13->selection).entries[0] = (GameEntityRuntime *)0x0;
    pSVar13 = (SelectionPlayerRuntimeBlock *)((pSVar13->selection).entries + 1);
  }
  do {
    iVar7 = pFVar14->playerRuntimeId;
    (pFVar14->factionAssignment).readyOrWaitState = 0;
    pFVar14->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    pFVar14->heartbeatExpiryTicks = 0x400;
    dVar6 = (pFVar14->factionAssignment).factionAssignmentIndex;
    g_SelectionPlayerRuntimeBlockPointers[iVar7] = pSVar9;
    pSVar9->primaryEntityOrFactionToken8080 = dVar6;
    pSVar9->simulationStepTicks = 1;
    pFVar10 = &pFVar14->playerName;
    pbVar15 = pSVar9->reserved80B0_8117 + 0x40;
    for (iVar7 = 0x14; iVar7 != 0; iVar7 = iVar7 + -1) {
      *(undefined4 *)pbVar15 = *(undefined4 *)pFVar10->textUtf16;
      pFVar10 = (FrontendPlayerNameUtf16_28 *)(pFVar10->textUtf16 + 2);
      pbVar15 = pbVar15 + 4;
    }
    FVar8 = FVar8 - 1;
    pSVar9 = pSVar9 + 1;
    pFVar14 = pFVar14 + 1;
  } while (FVar8 != 0);
  g_SessionTransferTimeoutTicks = 0x400;
  g_InGameNetworkTickCountdown = 4;
  g_InGameStateTickSpinLock = 0;
  (*g_TimerRegisterPeriodic)(0x50,InGameRuntime_PeriodicCountdownAndClockTick);
  UiRuntime_SetSynchronizationHooks
            (InGameRuntime_UpdateSimulationAndNetworkTick,&g_InGameStateTickSpinLock);
  UVar3 = (levelAsset->header).titleTextResourceIndex;
  puVar16 = &g_InGameSessionNameScratchUtf16;
  for (iVar7 = 0x20; iVar7 != 0; iVar7 = iVar7 + -1) {
    *puVar16 = 0;
    puVar16 = puVar16 + 1;
  }
  TVar20 = TextResource_Resolve(UVar3 + 0x2230);
  pwVar5 = TVar20.eax;
  pwVar17 = &g_InGameSessionNameScratchUtf16;
  iVar7 = 0x1f;
  do {
    pwVar5 = pwVar5 + 1;
    wVar2 = *pwVar5;
    if (wVar2 == 0) break;
    if ((((((wVar2 != 0x2a) && (wVar2 != 0x3c)) && (wVar2 != 0x3e)) &&
         ((wVar2 != 0x22 && (wVar2 != 0x2f)))) &&
        ((wVar2 != 0x5c && ((wVar2 != 0x2e && (wVar2 != 0x3f)))))) &&
       ((wVar2 != 0x3a && (wVar2 != 0x7c)))) {
      *pwVar17 = wVar2;
      pwVar17 = pwVar17 + 1;
    }
    iVar7 = iVar7 + -1;
  } while (iVar7 != 0);
  AVar21 = (*g_MemoryApi.alloc)(0x400000);
  gameRuntime1 = (InGameRuntimeRootImageC3E4 *)AVar21.eax;
  if (!AVar21.carry) {
    g_RuntimeObjectRebaseBaseMinusOne = gameRuntime1[-1].opaqueA06C_C3E3 + 0x2377;
    g_InGameWorldObjectRecords = (WorldObjectRecord *)gameRuntime1;
    for (iVar7 = 0x100000; iVar7 != 0; iVar7 = iVar7 + -1) {
      (gameRuntime1->rootUi0000).base.nextSibling = (UiNodeBase *)0x0;
      gameRuntime1 = (InGameRuntimeRootImageC3E4 *)&(gameRuntime1->rootUi0000).base.firstChild;
    }
    SVar22 = SelectionInfoPanel_InitResources
                       ((SelectionInfoEntitySlots *)
                        g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId]);
    gameRuntime1 = (InGameRuntimeRootImageC3E4 *)SVar22.valueOrError;
    if (!SVar22.carry) {
      AVar21 = (*g_MemoryApi.alloc)(0xc3e4);
      inGameRoot = (InGameRuntimeRootImageC3E4 *)AVar21.eax;
      gameRuntime1 = inGameRoot;
      if (!AVar21.carry) {
        puVar11 = &g_InGameRuntimeDefaultImageTemplate;
        g_InGameRuntimeRoot = inGameRoot;
        for (iVar7 = 0x30f9; iVar7 != 0; iVar7 = iVar7 + -1) {
          (gameRuntime1->rootUi0000).base.nextSibling = (UiNodeBase *)*puVar11;
          puVar11 = puVar11 + 1;
          gameRuntime1 = (InGameRuntimeRootImageC3E4 *)&(gameRuntime1->rootUi0000).base.firstChild;
        }
        world = &inGameRoot->worldRuntime0A30;
        SVar22 = InGameUiRuntime_InitializeControlTreeResourcesCf((UiRootNode *)inGameRoot);
        gameRuntime1 = (InGameRuntimeRootImageC3E4 *)SVar22.valueOrError;
        if (!SVar22.carry) {
          inGameRoot->worldOverlayCallback0B8C =
               InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf;
          (inGameRoot->worldRuntime0A30).selection.dispatchCommandCallback =
               InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf;
          (inGameRoot->worldRuntime0A30).selection.resolveContextActionPrimaryCallback =
               InGameWorldInput_ResolveContextActionAndCursorCf;
          (inGameRoot->worldRuntime0A30).selection.resolveContextActionSecondaryCallback =
               InGameWorldInput_ResolveContextActionAndCursorCf;
          (inGameRoot->worldRuntime0A30).selection.beginPointerCaptureCallback =
               InGameWorldInput_BeginPointerCaptureCf;
          (inGameRoot->worldRuntime0A30).selection.updateDragSelectionCallback =
               InGameWorldInput_UpdateDragSelectionAndCameraCf;
          (inGameRoot->worldRuntime0A30).selection.commitPointerActionCallback =
               InGameWorldInput_CommitPointerActionCf;
          (inGameRoot->worldRuntime0A30).fieldRegion.clearTransientStateCallback =
               InGameUiRuntime_ClearTransientState1BCallback;
          (inGameRoot->worldRuntime0A30).selection.dispatchWorldContextActionCallback =
               InGameUiRuntime_DispatchWorldContextActionCallback;
          (inGameRoot->worldRuntime0A30).minimumCameraDistanceQ12 = 0x8000;
          (inGameRoot->worldRuntime0A30).maximumCameraDistanceQ12 = 0x13000;
          (inGameRoot->worldRuntime0A30).motion.minimumPitchAngle = 0xffffc400;
          (inGameRoot->worldRuntime0A30).motion.maximumPitchAngle = 0xffffe800;
          (inGameRoot->worldRuntime0A30).tickSpinLock = &g_InGameStateTickSpinLock;
          (inGameRoot->worldRuntime0A30).simulationAndNetworkTickCallback =
               InGameRuntime_UpdateSimulationAndNetworkTick;
          pSVar13 = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
          inGameRoot->localPlayerPairCount0BA4 = 0;
          inGameRoot->localPlayerPairRecords0BA0 = pSVar13->pairRecords80_807F;
          UiRootStack_Push(&g_UiRootCallbacks_0054FBC0,(UiRootNode *)inGameRoot);
          EVar23 = LevelAsset_PrepareEndingMoviePathCf(levelMoviePath,&levelAsset->header);
          gameRuntime1 = (InGameRuntimeRootImageC3E4 *)EVar23.moviePath;
          if (!EVar23.carry) {
            MVar24 = Movie_Open(0x80000000,(word *)gameRuntime1);
            gameRuntime1 = (InGameRuntimeRootImageC3E4 *)MVar24.eax;
            if (!MVar24.carry) {
              MVar25 = Movie_AdvanceFrame();
              gameRuntime1 = (InGameRuntimeRootImageC3E4 *)MVar25.eax;
              if (!MVar25.carry) {
                inGameRoot->levelMovieRuntime08D4 = (MovieRuntime *)gameRuntime1;
                g_MoviePlaybackBaseFrameGroup = 0;
                g_MoviePlaybackScheduleCounter = 0;
                g_MoviePlaybackScheduleSpan = 0;
                g_MoviePlaybackCurrentFrame = 0;
                MoviePlayback_AdvanceToFrameAndPresent(0);
                MoviePlayback_AdvanceToFrameAndPresent(1);
                RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory09B8);
                WorldRuntime_AttachObjectArray(0x4000,g_InGameWorldObjectRecords,world);
                PVar4 = g_LocalPlayerRuntimeId;
                dVar6 = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId]->
                        primaryEntityOrFactionToken8080;
                levelAsset->playerSlots[6].aiClassOrMode = dVar6;
                (inGameRoot->worldRuntime0A30).activeFactionRuntimeIndex = dVar6;
                (inGameRoot->worldRuntime0A30).selection.activePlayerRuntimeId = PVar4;
                WorldRuntime_AttachAndClearDwordArray
                          (0x100,(dword *)&g_InGameWorldRuntimeDwordArray256,world);
                SVar22 = GameData_ResetDefaults();
                gameRuntime1 = (InGameRuntimeRootImageC3E4 *)SVar22.valueOrError;
                if (!SVar22.carry) {
                  IVar26 = InGameLevelRuntime_LoadResourcesAfterDefaultResetCf(levelAsset,world);
                  gameRuntime1 = (InGameRuntimeRootImageC3E4 *)IVar26.errorOrValue;
                  if (!IVar26.carry) {
                    pIVar18 = inGameRoot->notificationQueue9E60;
                    for (iVar7 = 0x20; iVar7 != 0; iVar7 = iVar7 + -1) {
                      pIVar18->notificationMovieId00 = 0;
                      pIVar18 = (InGameNotificationQueueRecord20 *)&pIVar18->priority04;
                    }
                    SVar22 = TerrainCompositeTexture_Create();
                    gameRuntime1 = (InGameRuntimeRootImageC3E4 *)SVar22.valueOrError;
                    if (!SVar22.carry) {
                      (*g_SpinLockAcquire)(&g_InGameStateTickSpinLock);
                      g_InGameSimulationStepTicks = 1;
                      dVar6 = PersistentSettings_ReadDword(0x40,0x14);
                      gridHalfSize = PersistentSettings_ReadDword(0x20,0x10);
                      subresourceCount = PersistentSettings_ReadDword(0x10,0x18);
                      GraphicsShadingRuntime_InitializeGeneratedTextureCf
                                (subresourceCount,gridHalfSize,dVar6);
                      dVar6 = PersistentSettings_ReadDword(1,0x1c);
                      if (dVar6 == 0) {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 & 0xfffdffff;
                      }
                      else {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 | 0x20000;
                      }
                      dVar6 = PersistentSettings_ReadDword(0,0x5c);
                      if ((dVar6 & 1) == 0) {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 & 0xbfffffff;
                      }
                      else {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 | 0x40000000;
                      }
                      if ((dVar6 & 2) == 0) {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 & 0x7fffffff;
                      }
                      else {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 | 0x80000000;
                      }
                      if ((dVar6 & 4) == 0) {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 & 0xfbffffff;
                      }
                      else {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 | 0x4000000;
                      }
                      gameRuntime1 = (InGameRuntimeRootImageC3E4 *)
                                     PersistentSettings_ReadDword(0,0x40);
                      if (((uint)gameRuntime1 & 4) != 0) {
                        UiPageStack_SetActiveIndex(1,&inGameRoot->optionalUiPageStack40AC);
                        gameRuntime1 = (InGameRuntimeRootImageC3E4 *)
                                       &inGameRoot->optionalUiPageStack4530;
                        UiPageStack_SetActiveIndex(0,(UiPageStackControl *)gameRuntime1);
                        UiPageStack_SetActiveIndex(0,&inGameRoot->optionalUiPageStack4644);
                        inGameRoot->optionalUiLayoutState0A04 = 0;
                        UiContainer_LayoutChildren((UiNodeBase *)inGameRoot);
                      }
                      bVar19 = (bool)InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf
                                               ((dword)(inGameRoot->worldRuntime0A30).fieldGrid);
                      if (!bVar19) {
                        if (g_FrontendLoadedCampaignAsset == 0) {
                          OldUnitRuntime_ResetPendingTables();
                        }
                        else {
                          OldUnitRuntime_MergeMasksAndReplayRecords();
                        }
                        GVar27 = GridScratch_AllocateForFieldGridCf
                                           ((inGameRoot->worldRuntime0A30).fieldGrid);
                        gameRuntime1 = (InGameRuntimeRootImageC3E4 *)GVar27.eax;
                        if (!GVar27.carry) {
                          GridScratch_RebuildTerrainAndRuntimeClassificationMasks(world);
                          GridInfluence_ClearDistanceBandsAndRefreshEntities
                                    ((inGameRoot->worldRuntime0A30).ownerListHead);
                          TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks();
                          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                              SESSION_NETWORK_ROLE_LOCAL) {
                            inGameRoot->localNetworkUiStateFlags24E0 =
                                 inGameRoot->localNetworkUiStateFlags24E0 | 8;
                          }
                          else {
                            inGameRoot->localNetworkUiStateFlags24E0 =
                                 inGameRoot->localNetworkUiStateFlags24E0 & 0xfffffff7;
                          }
                          UiCatalogGroup48_RebuildGrid((UiNodeBase *)inGameRoot);
                          UiCatalogGroup42_RebuildGrid((UiNodeBase *)inGameRoot);
                          UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)inGameRoot);
                          InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)inGameRoot);
                          WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
                          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                              SESSION_NETWORK_ROLE_LOCAL) {
                            FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
                                      (g_LocalPlayerRuntimeId,0,0,0);
                          }
                          else {
                            InGameCommandQueue_AppendLocalPlayerCommand(0x550,0,0,0);
                          }
                          (*g_SpinLockRelease)(&g_InGameStateTickSpinLock);
                          UiFrame_FlushInputAndResetPendingTicks();
                          (*g_GraphicsCursorSetFrame)(6);
                          g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
                          InGamePanel_RebuildPlayerStatusRows(inGameRoot);
                          do {
                            UiNode_InvalidateRoot(&inGameRoot->playerStatusNode08E4);
                            InGamePanel_RebuildPlayerStatusRows(inGameRoot);
                            UiFrame_Update(0);
                            UiFrame_Draw();
                            (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
                            InGameRuntime_UpdateSimulationAndNetworkTick();
                          } while ((g_UiCommandRuntimeFlags & 0x10) != 0);
                          inGameRoot->levelMovieRuntime08D4 = (MovieRuntime *)0x0;
                          inGameRoot->playerStatusLayoutMetric093C = 0;
                          UiPageStack_SetActiveIndex(2,&inGameRoot->primaryPageStack017C);
                          Movie_Close();
                          (*g_GraphicsCursorSetFrame)(0);
                          (*g_TimerRegisterPeriodic)
                                    (10,InGameRuntime_ProcessQueuedSessionNotificationTimer);
                          if (notificationMovieId != 0) {
                            InGameNotificationQueue_InsertPriorityRecord
                                      (NONE,0,0,0,0,0,1,notificationMovieId);
                            InGameNotificationQueue_InsertPriorityRecord
                                      (NONE,0,0,0,0,0,1,notificationMovieId + 1);
                            InGameNotificationQueue_InsertPriorityRecord
                                      (NONE,0,0,0,0,0,1,notificationMovieId + 2);
                            InGameNotificationQueue_InsertPriorityRecord
                                      (NONE,0,0,0,0,0,1,notificationMovieId + 3);
                            InGameNotificationQueue_InsertPriorityRecord
                                      (NONE,0,0,0,0,0,1,notificationMovieId + 4);
                          }
                          return (InGameRuntimeInitEaxCf5)((uint5)AVar21 & 0xffffffff);
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  Movie_Close();
  IVar28.carry = true;
  IVar28.runtimeRootOrError = (dword)gameRuntime1;
  return IVar28;
}


/* Address: 0x00564920.
   Ownership: gameplay/session/runtime.
   Purpose: Initializes an in-game session from persisted save state, reconstructs player and world runtime data,
   restores assets and subsystem state, installs callbacks, and reports failure through CF.
   Local calls: InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf,
   InGameRuntime_UpdateSimulationAndNetworkTick.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], Package_Mount
   [assets/package/runtime], Package_LoadEntry [assets/package/runtime], UiRuntime_SetSynchronizationHooks
   [ui/core/runtime], SelectionInfoPanel_InitResources [gameplay/selection/runtime],
   InGameUiRuntime_InitializeControlTreeResourcesCf [ui/ingame/runtime].
*/
InGameRuntimeLoadedInitEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameRuntime_InitializeLoadedSession(word *savePackagePath)

{
  WorldRuntimeFlags *pWVar1;
  WorldRuntimeContext *world;
  SelectionPlayerRuntimeBlock *entitySlots;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  FrontendLoadedLevelRuntimeImage370 *levelImage;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  dword dVar2;
  dword gridHalfSize;
  dword subresourceCount;
  InGameRuntimeRootImageC3E4 *pIVar3;
  int iVar4;
  byte *pbVar5;
  undefined4 *puVar6;
  undefined2 *puVar7;
  byte *pbVar8;
  byte *pbVar9;
  uint uVar10;
  short *psVar11;
  SelectionPlayerRuntimeBlock *pSVar12;
  InGameNotificationQueueRecord20 *pIVar13;
  bool bVar14;
  StatusValueEaxCf5 SVar15;
  PackageLoadEntryEaxCf5 PVar16;
  ArenaAllocEaxCf5 AVar17;
  EndingMoviePathEaxCf5 EVar18;
  MovieOpenEaxCf5 MVar19;
  MovieAdvanceFrameEaxCf5 MVar20;
  InGameLevelLoadEaxCf5 IVar21;
  GridScratchAllocEaxCf5 GVar22;
  InGameRuntimeLoadedInitEaxCf5 IVar23;
  InGameRuntimeRootImageC3E4 *local_24;
  FrontendLoadedLevelRuntimeImage370 *loadedLevelAsset;
  
  g_TextureDownsampleShift = PersistentSettings_ReadDword(0,0x30);
  local_24 = (InGameRuntimeRootImageC3E4 *)0x0;
  loadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)0x0;
  SVar15 = Package_Mount(savePackagePath);
  pbVar5 = g_PackageScratchBuffer;
  gameRuntime1 = (InGameRuntimeRootImageC3E4 *)SVar15.valueOrError;
  pIVar3 = gameRuntime1;
  if (!SVar15.carry) {
    puVar7 = &g_InGameSessionNameScratchUtf16;
    for (iVar4 = 0x20; iVar4 != 0; iVar4 = iVar4 + -1) {
      *puVar7 = 0;
      puVar7 = puVar7 + 1;
    }
    (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,gameRuntime1);
    (*g_FileSystemReadExactCf)(0x200,pbVar5,gameRuntime1);
    pbVar5 = pbVar5 + 0x100;
    bVar14 = true;
    iVar4 = 0x24;
    pbVar8 = pbVar5;
    do {
      pbVar9 = pbVar8;
      if (iVar4 == 0) break;
      iVar4 = iVar4 + -1;
      pbVar9 = pbVar8 + 2;
      bVar14 = *(short *)pbVar8 == 0;
      pbVar8 = pbVar9;
    } while (!bVar14);
    if (bVar14) {
      pbVar9[-0xffffffff00000006] = 0;
      pbVar9[-0xffffffff00000005] = 0;
      pbVar9[-0xffffffff00000004] = 0;
      pbVar9[-0xffffffff00000003] = 0;
      pbVar9[-0xffffffff0000000a] = 0;
      pbVar9[-0xffffffff00000009] = 0;
      pbVar9[-0xffffffff00000008] = 0;
      pbVar9[-0xffffffff00000007] = 0;
      uVar10 = (uint)((int)pbVar9 - (int)pbVar5) >> 1;
      psVar11 = &g_InGameSessionNameScratchUtf16;
      if (0x1f < uVar10) {
        uVar10 = 0x1f;
      }
      for (; uVar10 != 0; uVar10 = uVar10 - 1) {
        *psVar11 = *(short *)pbVar5;
        pbVar5 = pbVar5 + 2;
        psVar11 = psVar11 + 1;
      }
    }
    PVar16 = Package_LoadEntry((word *)u_campagne_hex_0050e068);
    if (!PVar16.carry) {
      g_FrontendLoadedCampaignAsset = PVar16.bufferOrError;
    }
    PVar16 = Package_LoadEntry((word *)u_level_hex_0050e040);
    levelImage = PVar16.bufferOrError;
    pIVar3 = (InGameRuntimeRootImageC3E4 *)levelImage;
    local_24 = gameRuntime1;
    if (!PVar16.carry) {
      dVar2 = levelImage->playerSlots[6].aiClassOrMode;
      pSVar12 = g_SelectionPlayerBlocks;
      for (iVar4 = 0x10230; entitySlots = g_SelectionPlayerBlocks, iVar4 != 0; iVar4 = iVar4 + -1) {
        (pSVar12->selection).entries[0] = (GameEntityRuntime *)0x0;
        pSVar12 = (SelectionPlayerRuntimeBlock *)((pSVar12->selection).entries + 1);
      }
      g_EndMovieSelectionIndex = 0xffffffff;
      g_EndMovieVariantIndex = 0;
      g_EndMoviePath = (word *)0x0;
      g_LocalPlayerRuntimeId = 0;
      g_SelectionPlayerRuntimeBlockPointers[0] = g_SelectionPlayerBlocks;
      g_SelectionPlayerBlocks->primaryEntityOrFactionToken8080 = dVar2;
      entitySlots->simulationStepTicks = 1;
      g_UiCommandRuntimeFlags = 0x11;
      g_SessionNetworkTickCounter = 1;
      g_HostCommandBatchSyncSentThisInterval = 0;
      g_GameFactionRuntimeImage.tail.simulationTick = 1;
      g_GameFactionRuntimeImage.tail.presentationTick = 0;
      g_InGameSessionNotificationTimeoutTicks = 0;
      g_InGameReadyStateToggleFlags = 0;
      g_InGameNetworkTickCountdown = 4;
      g_InGameStateTickSpinLock = 0;
      g_EndGameResultsCurrentMusicTrackId = 0;
      g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
      (*g_TimerRegisterPeriodic)(0x50,InGameRuntime_PeriodicCountdownAndClockTick);
      UiRuntime_SetSynchronizationHooks
                (InGameRuntime_UpdateSimulationAndNetworkTick,&g_InGameStateTickSpinLock);
      AVar17 = (*g_MemoryApi.alloc)(0x400000);
      pIVar3 = (InGameRuntimeRootImageC3E4 *)AVar17.eax;
      loadedLevelAsset = levelImage;
      if (!AVar17.carry) {
        g_RuntimeObjectRebaseBaseMinusOne = pIVar3[-1].opaqueA06C_C3E3 + 0x2377;
        g_InGameWorldObjectRecords = (WorldObjectRecord *)pIVar3;
        for (iVar4 = 0x100000; iVar4 != 0; iVar4 = iVar4 + -1) {
          (pIVar3->rootUi0000).base.nextSibling = (UiNodeBase *)0x0;
          pIVar3 = (InGameRuntimeRootImageC3E4 *)&(pIVar3->rootUi0000).base.firstChild;
        }
        SVar15 = SelectionInfoPanel_InitResources((SelectionInfoEntitySlots *)entitySlots);
        pIVar3 = (InGameRuntimeRootImageC3E4 *)SVar15.valueOrError;
        if (!SVar15.carry) {
          AVar17 = (*g_MemoryApi.alloc)(0xc3e4);
          inGameRoot = (InGameRuntimeRootImageC3E4 *)AVar17.eax;
          pIVar3 = inGameRoot;
          if (!AVar17.carry) {
            puVar6 = &g_InGameRuntimeDefaultImageTemplate;
            g_InGameRuntimeRoot = inGameRoot;
            for (iVar4 = 0x30f9; iVar4 != 0; iVar4 = iVar4 + -1) {
              (pIVar3->rootUi0000).base.nextSibling = (UiNodeBase *)*puVar6;
              puVar6 = puVar6 + 1;
              pIVar3 = (InGameRuntimeRootImageC3E4 *)&(pIVar3->rootUi0000).base.firstChild;
            }
            world = &inGameRoot->worldRuntime0A30;
            SVar15 = InGameUiRuntime_InitializeControlTreeResourcesCf((UiRootNode *)inGameRoot);
            pIVar3 = (InGameRuntimeRootImageC3E4 *)SVar15.valueOrError;
            if (!SVar15.carry) {
              inGameRoot->worldOverlayCallback0B8C =
                   InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf;
              (inGameRoot->worldRuntime0A30).selection.dispatchCommandCallback =
                   InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf;
              (inGameRoot->worldRuntime0A30).selection.resolveContextActionPrimaryCallback =
                   InGameWorldInput_ResolveContextActionAndCursorCf;
              (inGameRoot->worldRuntime0A30).selection.resolveContextActionSecondaryCallback =
                   InGameWorldInput_ResolveContextActionAndCursorCf;
              (inGameRoot->worldRuntime0A30).selection.beginPointerCaptureCallback =
                   InGameWorldInput_BeginPointerCaptureCf;
              (inGameRoot->worldRuntime0A30).selection.updateDragSelectionCallback =
                   InGameWorldInput_UpdateDragSelectionAndCameraCf;
              (inGameRoot->worldRuntime0A30).selection.commitPointerActionCallback =
                   InGameWorldInput_CommitPointerActionCf;
              (inGameRoot->worldRuntime0A30).fieldRegion.clearTransientStateCallback =
                   InGameUiRuntime_ClearTransientState1BCallback;
              (inGameRoot->worldRuntime0A30).selection.dispatchWorldContextActionCallback =
                   InGameUiRuntime_DispatchWorldContextActionCallback;
              (inGameRoot->worldRuntime0A30).minimumCameraDistanceQ12 = 0x8000;
              (inGameRoot->worldRuntime0A30).maximumCameraDistanceQ12 = 0x13000;
              (inGameRoot->worldRuntime0A30).motion.minimumPitchAngle = 0xffffc400;
              (inGameRoot->worldRuntime0A30).motion.maximumPitchAngle = 0xffffe800;
              (inGameRoot->worldRuntime0A30).tickSpinLock = &g_InGameStateTickSpinLock;
              (inGameRoot->worldRuntime0A30).simulationAndNetworkTickCallback =
                   InGameRuntime_UpdateSimulationAndNetworkTick;
              pSVar12 = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
              inGameRoot->localPlayerPairCount0BA4 = 0;
              inGameRoot->localPlayerPairRecords0BA0 = pSVar12->pairRecords80_807F;
              UiRootStack_Push(&g_UiRootCallbacks_0054FBC0,(UiRootNode *)inGameRoot);
              WidePath_CombineDirectoryAndLeaf
                        (&g_FrontendScenarioPathScratchUtf16,
                         (word *)(levelImage->header).opaque100_16F,(word *)u_level_0050daac);
              WidePath_SetExtensionCode(0x76656c,&g_FrontendScenarioPathScratchUtf16);
              EVar18 = LevelAsset_PrepareEndingMoviePathCf
                                 (savePackagePath,(LevelAssetHeader *)levelImage);
              pIVar3 = (InGameRuntimeRootImageC3E4 *)EVar18.moviePath;
              if (!EVar18.carry) {
                MVar19 = Movie_Open(0x80000000,(word *)pIVar3);
                pIVar3 = (InGameRuntimeRootImageC3E4 *)MVar19.eax;
                if (!MVar19.carry) {
                  MVar20 = Movie_AdvanceFrame();
                  pIVar3 = (InGameRuntimeRootImageC3E4 *)MVar20.eax;
                  if (!MVar20.carry) {
                    inGameRoot->levelMovieRuntime08D4 = (MovieRuntime *)pIVar3;
                    g_MoviePlaybackBaseFrameGroup = 0;
                    g_MoviePlaybackScheduleCounter = 0;
                    g_MoviePlaybackScheduleSpan = 0;
                    g_MoviePlaybackCurrentFrame = 0;
                    MoviePlayback_AdvanceToFrameAndPresent(0);
                    MoviePlayback_AdvanceToFrameAndPresent(1);
                    RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory09B8);
                    pSVar12 = g_SelectionPlayerBlocks;
                    WorldRuntime_AttachObjectArray(0x4000,g_InGameWorldObjectRecords,world);
                    pIVar3 = (InGameRuntimeRootImageC3E4 *)pSVar12->primaryEntityOrFactionToken8080;
                    (inGameRoot->worldRuntime0A30).activeFactionRuntimeIndex =
                         (FactionRuntimeIndex)pIVar3;
                    (inGameRoot->worldRuntime0A30).selection.activePlayerRuntimeId = 0;
                    WorldRuntime_AttachAndClearDwordArray
                              (0x100,(dword *)&g_InGameWorldRuntimeDwordArray256,world);
                    bVar14 = GameData_LoadExternalTables();
                    if (!bVar14) {
                      PVar16 = Package_LoadEntry((word *)u_field_hex_0050e002);
                      pIVar3 = PVar16.bufferOrError;
                      if (!PVar16.carry) {
                        (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid =
                             (dword)pIVar3;
                        IVar21 = InGameLevelRuntime_LoadResourcesAfterExternalTablesCf
                                           (levelImage,world);
                        pIVar3 = (InGameRuntimeRootImageC3E4 *)IVar21.errorOrValue;
                        if (!IVar21.carry) {
                          pIVar13 = inGameRoot->notificationQueue9E60;
                          for (iVar4 = 0x20; iVar4 != 0; iVar4 = iVar4 + -1) {
                            pIVar13->notificationMovieId00 = 0;
                            pIVar13 = (InGameNotificationQueueRecord20 *)&pIVar13->priority04;
                          }
                          SVar15 = TerrainCompositeTexture_Create();
                          pIVar3 = (InGameRuntimeRootImageC3E4 *)SVar15.valueOrError;
                          if (!SVar15.carry) {
                            (*g_SpinLockAcquire)(&g_InGameStateTickSpinLock);
                            UiCatalogGroup48_RebuildGrid((UiNodeBase *)inGameRoot);
                            UiCatalogGroup42_RebuildGrid((UiNodeBase *)inGameRoot);
                            UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)inGameRoot);
                            InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)inGameRoot);
                            g_InGameSimulationStepTicks = 1;
                            dVar2 = PersistentSettings_ReadDword(0x40,0x14);
                            gridHalfSize = PersistentSettings_ReadDword(0x20,0x10);
                            subresourceCount = PersistentSettings_ReadDword(0x10,0x18);
                            GraphicsShadingRuntime_InitializeGeneratedTextureCf
                                      (subresourceCount,gridHalfSize,dVar2);
                            dVar2 = PersistentSettings_ReadDword(1,0x1c);
                            if (dVar2 == 0) {
                              pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *pWVar1 = *pWVar1 & 0xfffdffff;
                            }
                            else {
                              pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *pWVar1 = *pWVar1 | 0x20000;
                            }
                            pIVar3 = (InGameRuntimeRootImageC3E4 *)
                                     PersistentSettings_ReadDword(0,0x5c);
                            if (((uint)pIVar3 & 1) == 0) {
                              pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *pWVar1 = *pWVar1 & 0xbfffffff;
                            }
                            else {
                              pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *pWVar1 = *pWVar1 | 0x40000000;
                            }
                            if (((uint)pIVar3 & 2) == 0) {
                              pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *pWVar1 = *pWVar1 & 0x7fffffff;
                            }
                            else {
                              pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *pWVar1 = *pWVar1 | 0x80000000;
                            }
                            if (((uint)pIVar3 & 4) == 0) {
                              pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *pWVar1 = *pWVar1 & 0xfbffffff;
                            }
                            else {
                              pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *pWVar1 = *pWVar1 | 0x4000000;
                            }
                            bVar14 = (bool)InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf
                                                     ((dword)(inGameRoot->worldRuntime0A30).
                                                             fieldGrid);
                            if (!bVar14) {
                              GVar22 = GridScratch_AllocateForFieldGridCf
                                                 ((inGameRoot->worldRuntime0A30).fieldGrid);
                              pIVar3 = (InGameRuntimeRootImageC3E4 *)GVar22.eax;
                              if (!GVar22.carry) {
                                GridScratch_RebuildTerrainAndRuntimeClassificationMasks(world);
                                GridInfluence_ClearDistanceBandsAndRefreshEntities
                                          ((inGameRoot->worldRuntime0A30).ownerListHead);
                                TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks();
                                WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
                                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK
                                    ) == SESSION_NETWORK_ROLE_LOCAL) {
                                  inGameRoot->localNetworkUiStateFlags24E0 =
                                       inGameRoot->localNetworkUiStateFlags24E0 | 8;
                                }
                                else {
                                  inGameRoot->localNetworkUiStateFlags24E0 =
                                       inGameRoot->localNetworkUiStateFlags24E0 & 0xfffffff7;
                                }
                                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK
                                    ) == SESSION_NETWORK_ROLE_LOCAL) {
                                  FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
                                            (g_LocalPlayerRuntimeId,0,0,0);
                                }
                                else {
                                  InGameCommandQueue_AppendLocalPlayerCommand(0x550,0,0,0);
                                }
                                (*g_SpinLockRelease)(&g_InGameStateTickSpinLock);
                                UiFrame_FlushInputAndResetPendingTicks();
                                (*g_GraphicsCursorSetFrame)(6);
                                g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
                                InGamePanel_RebuildPlayerStatusRows(inGameRoot);
                                do {
                                  UiNode_InvalidateRoot(&inGameRoot->playerStatusNode08E4);
                                  InGamePanel_RebuildPlayerStatusRows(inGameRoot);
                                  UiFrame_Update(0);
                                  UiFrame_Draw();
                                  (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
                                  InGameRuntime_UpdateSimulationAndNetworkTick();
                                } while ((g_UiCommandRuntimeFlags & 0x10) != 0);
                                inGameRoot->levelMovieRuntime08D4 = (MovieRuntime *)0x0;
                                inGameRoot->playerStatusLayoutMetric093C = 0;
                                UiPageStack_SetActiveIndex(2,&inGameRoot->primaryPageStack017C);
                                Movie_Close();
                                Resource_Release(levelImage);
                                Package_Unmount((EngineFileHandle)gameRuntime1);
                                (*g_GraphicsCursorSetFrame)(0);
                                (*g_TimerRegisterPeriodic)
                                          (10,InGameRuntime_ProcessQueuedSessionNotificationTimer);
                                return (InGameRuntimeLoadedInitEaxCf5)((uint5)AVar17 & 0xffffffff);
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  Movie_Close();
  Resource_Release(loadedLevelAsset);
  Package_Unmount((EngineFileHandle)local_24);
  IVar23.carry = true;
  IVar23.runtimeRootOrError = (dword)pIVar3;
  return IVar23;
}


/* Address: 0x005651D0.
   Ownership: gameplay/session/runtime.
   Purpose: Unregisters the in-game timer, clears and presents the framebuffer, tears down the active root and
   world state, closes movie and terrain resources, releases four texture assets, resets registries and input, and
   decrements the cursor-visibility token.
   Local calls: InGameRuntime_PublishRootWorldStatePointer, InGameRuntime_ReleaseFactionScratchBuffers.
   Cross-module calls: GraphicsShadingRuntime_Shutdown [graphics/render/shading],
   WorldRuntime_ForEachNodeInOwnerListD8 [world/runtime/core], InGameLevelRuntime_ShutdownLoadedAssetResources
   [gameplay/session/level], UiRootStack_PopCf [ui/controls/layout], Movie_Close [movie/runtime/playback],
   TerrainCompositeTexture_Destroy [world/terrain/visuals].
*/
void __thandor_void_preserve_eax_ecx_edx InGameRuntime_ShutdownAndReleaseResources(void)

{
  WorldRuntimeContext *world;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  bool bVar1;
  
  (*g_TimerUnregisterPeriodic)(InGameRuntime_PeriodicCountdownAndClockTick);
  inGameRoot = g_InGameRuntimeRoot;
  (*g_GraphicsCursorSetFrame)(6);
  bVar1 = (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar1) {
    (*g_GraphicsFramebufferFillRectArgb)
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
  }
  (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  GraphicsShadingRuntime_Shutdown();
  if (inGameRoot != (InGameRuntimeRootImageC3E4 *)0x0) {
    InGameRuntime_PublishRootWorldStatePointer(&inGameRoot->rootUi0000);
    world = &inGameRoot->worldRuntime0A30;
    WorldRuntime_ForEachNodeInOwnerListD8
              (world,WorldRuntimeNode_ReleaseShutdownBindingsCallback,world);
    InGameLevelRuntime_ShutdownLoadedAssetResources(world);
    if ((inGameRoot->rootUi0000).previousRoot != (UiRootNode *)0x0) {
      UiRootStack_PopCf(&inGameRoot->rootUi0000);
    }
    (*g_MemoryApi.free)(inGameRoot);
    g_InGameRuntimeRoot = (InGameRuntimeRootImageC3E4 *)0x0;
  }
  InGameRuntime_ReleaseFactionScratchBuffers();
  (*g_MemoryApi.free)(g_InGameWorldObjectRecords);
  g_InGameWorldObjectRecords = (WorldObjectRecord *)0x0;
  Movie_Close();
  TerrainCompositeTexture_Destroy();
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_InGameDiagramTextureSource);
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_InGamePanelTextureSource);
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_InGameTechnologyTextureSource);
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_InGameWindowTextureSource);
  g_InGameDiagramTextureSource = (GraphicsTextureSourceAsset *)0x0;
  g_InGamePanelTextureSource = (GraphicsTextureSourceAsset *)0x0;
  g_InGameTechnologyTextureSource = (GraphicsTextureSourceAsset *)0x0;
  g_InGameWindowTextureSource = (GraphicsTextureSourceAsset *)0x0;
  GraphicsShadingRuntime_ClearRecordTable();
  SelectionInfoPanel_ShutdownResources();
  SpriteAssetRegistry_Reset();
  UiFrame_FlushInputAndResetPendingTicks();
  g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
  return;
}


/* Address: 0x0050E0D0.
   Ownership: gameplay/session/runtime.
   Purpose: Releases and clears the paired per-faction scratch-buffer arrays used by the in-game runtime.
*/
void __thandor_void_preserve_eax_ecx InGameRuntime_ReleaseFactionScratchBuffers(void)

{
  int iVar1;
  undefined4 *scratchBufferSetBCursor;
  undefined4 *scratchBufferSetACursor;
  
  iVar1 = 8;
  scratchBufferSetACursor = &g_InGameFactionScratchBufferSetA8;
  scratchBufferSetBCursor = &g_InGameFactionScratchBufferSetB8;
  do {
    (*g_MemoryApi.free)((void *)*scratchBufferSetACursor);
    (*g_MemoryApi.free)((void *)*scratchBufferSetBCursor);
    *scratchBufferSetACursor = 0;
    *scratchBufferSetBCursor = 0;
    scratchBufferSetACursor = scratchBufferSetACursor + 1;
    scratchBufferSetBCursor = scratchBufferSetBCursor + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}


/* Address: 0x0050E120.
   Ownership: gameplay/session/runtime.
   Purpose: Advances faction tail states, reevaluates the sixty-four scheduled condition records through the
   verified fourteen-way jump table, processes completion records and end-state effects, and performs the
   associated field-runtime cleanup paths. InGameConditionRuntimeStorageView800_V287 is a function-specific storage
   view, not a replacement for the existing InGameConditionRuntime record-array view.
   [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE] Retired detached enum dictionary InGameConditionExpressionOpcode after
   transferring its complete value vocabulary to code annotation. It is not a safe whole-value storage type.
   Values: 252=INGAME_CONDITION_EXPRESSION_END, 253=INGAME_CONDITION_EXPRESSION_NOT,
   254=INGAME_CONDITION_EXPRESSION_AND, 255=INGAME_CONDITION_EXPRESSION_OR [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE]
   Retired detached enum dictionary InGameEndConditionTriggerStateFlags after transferring its complete value
   vocabulary to code annotation.
   Cross-module calls: ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive [world/model/hierarchy].
*/

void __thandor_void_preserve_eax_ecx_edx InGameConditionRuntime_UpdateScheduledRecords(void)

{
  ResourceExtractionDescriptor32 *pRVar1;
  dword dVar2;
  int iVar3;
  int *modelRuntime;
  InGameLevelConditionStorageView800 *pIVar4;
  dword dVar5;
  uint uVar6;
  byte bVar7;
  int iVar8;
  InGameScheduledConditionKind IVar9;
  uint uVar10;
  byte *pbVar11;
  uint uVar12;
  FactionRuntimeLifecycleObservedState *pFVar13;
  WorldRuntimeContext *contextArg;
  FactionRuntimeLifecycleObservedState *pFVar14;
  WorldOwnerListNode100 *worldNode1;
  InGameConditionScheduleImageView480 *pIVar15;
  InGameEndConditionTriggerRecord8 *endTrigger;
  FieldGridAsset *fieldGrid1;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  InGameRuntimeRootImageC3E4 *gameRuntime2;
  
  pFVar14 = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
  iVar8 = 7;
  do {
    pFVar14 = pFVar14 + 1;
    if (*pFVar14 == FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING) {
      *pFVar14 = *pFVar14 + FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    }
    pIVar4 = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
    iVar8 = iVar8 + -1;
  } while (iVar8 != 0);
  iVar8 = 0x40;
  pIVar15 = &(g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule;
  do {
    IVar9 = pIVar15->conditions[0].statusAndKind.kind;
    pIVar15->conditions[0].statusAndKind.kind =
         pIVar15->conditions[0].statusAndKind.kind & 0xfffffffe;
                    // WARNING: Switch is manually overridden
    switch(IVar9 & 0xfe) {
    case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION:
      for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode1 != (WorldOwnerListNode100 *)0x0; worldNode1 = worldNode1->nextNode) {
        if ((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
           (pIVar15->conditions[0].payload.operands[0] ==
            *(dword *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xc)))
        goto InGameScheduledCondition_AdvanceToNextRecord;
      }
      goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
    case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION_AND_CLASS_COMMAND_GROUP_A:
      for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode1 != (WorldOwnerListNode100 *)0x0; worldNode1 = worldNode1->nextNode) {
        if (((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
             [*(int *)(*(int *)worldNode1->runtimePayload + 0x4c)] ==
             ArmyRuntime_ClassCommandHandlerGroupACf)) &&
           (*(dword *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xc) ==
            pIVar15->conditions[0].payload.operands[0]))
        goto InGameScheduledCondition_AdvanceToNextRecord;
      }
      goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
    case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION_AND_RUNTIME_ID:
      for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode1 != (WorldOwnerListNode100 *)0x0; worldNode1 = worldNode1->nextNode) {
        if (((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (iVar3 = *(int *)((int)worldNode1->runtimePayload + 8),
            pIVar15->conditions[0].payload.operands[0] == *(dword *)(iVar3 + 0xc))) &&
           (*(dword *)(iVar3 + 0xa0) == pIVar15->conditions[0].payload.operands[2]))
        goto InGameScheduledCondition_AdvanceToNextRecord;
      }
      goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
    case INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8:
      dVar5 = pIVar15->conditions[0].payload.operands[1];
      dVar2 = pIVar15->conditions[0].payload.operands[0];
      if (((g_GameFactionRuntimeImage.tail.factionLifecycleStates[dVar5] !=
            FACTION_RUNTIME_LIFECYCLE_ACTIVE) ||
          (g_GameFactionRuntimeImage.tail.factionLifecycleStates[dVar2] !=
           FACTION_RUNTIME_LIFECYCLE_ACTIVE)) ||
         (7 < (g_GameFactionRuntimeImage.records[dVar5].packedRelationStates >>
               ((char)dVar2 * '\x04' & 0x1fU) & 0xf)))
      goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
      break;
    case INGAME_SCHEDULED_CONDITION_PRIMARY_RESOURCE_CURRENT_AT_LEAST:
      if ((int)pIVar15->conditions[0].payload.operands[1] <=
          (int)g_GameFactionRuntimeImage.records[pIVar15->conditions[0].payload.operands[0]].
               xeniteCurrentQ4) {
        pIVar15->conditions[0].statusAndKind.kind = pIVar15->conditions[0].statusAndKind.kind | 1;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_SECONDARY_RESOURCE_CURRENT_AT_LEAST:
      if ((int)pIVar15->conditions[0].payload.operands[1] <=
          (int)g_GameFactionRuntimeImage.records[pIVar15->conditions[0].payload.operands[0]].
               tritiumCurrentQ4) {
        pIVar15->conditions[0].statusAndKind.kind = pIVar15->conditions[0].statusAndKind.kind | 1;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_ACTIVE_ARMY_SCALE_VALUE_AT_LEAST:
      if ((int)pIVar15->conditions[0].payload.operands[1] <=
          (int)g_GameFactionRuntimeImage.records[pIVar15->conditions[0].payload.operands[0]].
               tritiumExtractionRateQ4PerTick) {
        pIVar15->conditions[0].statusAndKind.kind = pIVar15->conditions[0].statusAndKind.kind | 1;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_MATCHING_DEFINITION_AND_RUNTIME_ID_ACTIVE_ENTITY_COUNT_AT_LEAST:
      dVar5 = pIVar15->conditions[0].payload.operands[1];
      for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode1 != (WorldOwnerListNode100 *)0x0; worldNode1 = worldNode1->nextNode) {
        if (((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (*(dword *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xc) ==
             pIVar15->conditions[0].payload.operands[0])) &&
           ((*(dword *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xa0) ==
             pIVar15->conditions[0].payload.operands[2] && (dVar5 = dVar5 - 1, dVar5 == 0))))
        goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST:
      fieldGrid1 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
      uVar12 = fieldGrid1->gridWidth * fieldGrid1->gridHeight;
      pbVar11 = fieldGrid1->cells[0].runtime0C_3F +
                (pIVar15->conditions[0].payload.operands[0] - 0xc);
      uVar6 = 0;
      uVar10 = uVar12;
      do {
        pRVar1 = (ResourceExtractionDescriptor32 *)(pbVar11 + 0x70);
        pbVar11 = pbVar11 + 0x80;
        uVar6 = uVar6 + ((*pRVar1 & 0xf9) != 0);
        uVar10 = uVar10 - 1;
      } while (uVar10 != 0);
      if ((int)pIVar15->conditions[0].payload.operands[1] <=
          (int)(((ulonglong)uVar6 * 100) / (ulonglong)uVar12)) {
        pIVar15->conditions[0].statusAndKind.kind = pIVar15->conditions[0].statusAndKind.kind | 1;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED:
      dVar5 = pIVar15->conditions[0].payload.operands[1] - g_InGameSimulationStepTicks;
      pIVar15->conditions[0].payload.operands[1] = dVar5;
      if ((int)dVar5 < 1) {
        pIVar15->conditions[0].statusAndKind.kind = pIVar15->conditions[0].statusAndKind.kind | 1;
        pIVar15->conditions[0].payload.operands[1] = 0;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_PRIMARY_RESOURCE_LIMIT_AT_MOST_0FA0:
      if ((int)g_GameFactionRuntimeImage.records[pIVar15->conditions[0].payload.operands[0]].
               xeniteStorageLimitQ4 < 0xfa1) {
        pIVar15->conditions[0].statusAndKind.kind = pIVar15->conditions[0].statusAndKind.kind | 1;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_CLASS_ID_OUTSIDE_CLASS_COMMAND_GROUP_A:
      for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode1 != (WorldOwnerListNode100 *)0x0; worldNode1 = worldNode1->nextNode) {
        if (((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (dVar5 = *(dword *)(*(int *)worldNode1->runtimePayload + 0x4c),
            g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[dVar5] !=
            ArmyRuntime_ClassCommandHandlerGroupACf)) &&
           (dVar5 == pIVar15->conditions[0].payload.operands[0]))
        goto InGameScheduledCondition_AdvanceToNextRecord;
      }
InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied:
      pIVar15->conditions[0].statusAndKind.kind = pIVar15->conditions[0].statusAndKind.kind | 1;
      break;
    case INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION:
      pbVar11 = (byte *)((int)&pIVar15->conditions[0].statusAndKind.kind + 1);
      IVar9 = INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED;
      while( true ) {
        while( true ) {
          while( true ) {
            while( true ) {
              bVar7 = *pbVar11;
              pbVar11 = pbVar11 + 1;
              if (bVar7 != 0xff) break;
              IVar9 = IVar9 >> 1 | IVar9 & 1;
            }
            if (bVar7 != 0xfe) break;
            IVar9 = IVar9 >> 1 & (IVar9 | 0xfffffffe);
          }
          if (bVar7 != 0xfd) break;
          IVar9 = IVar9 ^ 1;
        }
        if (bVar7 == 0xfc) break;
        IVar9 = ((pIVar4->schedule).conditions[bVar7].statusAndKind.kind & 1) + IVar9 * 2;
      }
      pIVar15->conditions[0].statusAndKind.kind =
           pIVar15->conditions[0].statusAndKind.kind | IVar9 & 1;
    }
InGameScheduledCondition_AdvanceToNextRecord:
    pIVar15 = (InGameConditionScheduleImageView480 *)(pIVar15->conditions + 1);
    iVar8 = iVar8 + -1;
    if (iVar8 == 0) {
      endTrigger = (InGameEndConditionTriggerRecord8 *)(pIVar4->schedule).triggers;
      iVar8 = 0x10;
      do {
        if ((endTrigger->stateFlags == INGAME_END_CONDITION_TRIGGER_ACTIVE) &&
           (((pIVar4->schedule).conditions[endTrigger->conditionIndex].statusAndKind.kind & 1) !=
            INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED)) {
          endTrigger->stateFlags = endTrigger->stateFlags | INGAME_END_CONDITION_TRIGGER_PROCESSED;
          gameRuntime1 = g_InGameRuntimeRoot;
          uVar10 = (uint)endTrigger->factionRuntimeIndex;
          if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar10] ==
              FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
            contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
            g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar10] =
                 FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING;
            if (endTrigger->skipArmyDisableWhenOne != 1) {
              worldNode1 = (gameRuntime1->worldRuntime0A30).ownerListHead;
              if (worldNode1 != (WorldOwnerListNode100 *)0x0) {
                do {
                  if ((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
                     (modelRuntime = *(int **)((int)worldNode1->runtimePayload + 8),
                     uVar10 == modelRuntime[3])) {
                    ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive(contextArg,modelRuntime);
                  }
                  worldNode1 = worldNode1->nextNode;
                } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
                g_GameFactionRuntimeImage.records[uVar10].secondaryArmyAssetCount = 0;
                g_GameFactionRuntimeImage.records[uVar10].primaryArmyAssetCount = 0;
              }
              gameRuntime2 = g_InGameRuntimeRoot;
              if (uVar10 == (gameRuntime1->worldRuntime0A30).activeFactionRuntimeIndex) {
                g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x108;
              }
              pFVar14 = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
              uVar10 = 1;
              bVar7 = 4;
              do {
                pFVar14 = pFVar14 + 1;
                if (*pFVar14 == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
                  uVar6 = uVar10 + 1;
                  pFVar13 = pFVar14;
                  do {
                    pFVar13 = pFVar13 + 1;
                    if ((*pFVar13 == FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
                       ((g_GameFactionRuntimeImage.records[uVar6].packedRelationStates >>
                         (bVar7 & 0x1f) & 0xf) < 8)) {
                      if ((uint)endTrigger->factionRuntimeIndex ==
                          (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex) {
                        g_InGameRuntimeRoot->observedRelationTransitionFlags4D54 =
                             g_InGameRuntimeRoot->observedRelationTransitionFlags4D54 | 8;
                        *(uint *)(gameRuntime2->opaque4D58_9A6B + 0x10a4) =
                             *(uint *)(gameRuntime2->opaque4D58_9A6B + 0x10a4) | 8;
                        *(uint *)(gameRuntime2->opaque4D58_9A6B + 0x2970) =
                             *(uint *)(gameRuntime2->opaque4D58_9A6B + 0x2970) | 8;
                        *(uint *)(gameRuntime2->opaque4D58_9A6B + 0x3f3c) =
                             *(uint *)(gameRuntime2->opaque4D58_9A6B + 0x3f3c) | 8;
                      }
                      return;
                    }
                    uVar6 = uVar6 + 1;
                  } while (uVar6 < 8);
                }
                uVar10 = uVar10 + 1;
                bVar7 = bVar7 + 4;
              } while (uVar10 < 7);
              contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
              uVar10 = (uint)endTrigger->factionRuntimeIndex;
            }
            uVar6 = contextArg->activeFactionRuntimeIndex;
            g_EndMovieVariantIndex = (uint)endTrigger->movieVariantSelector;
            if (((uVar6 != uVar10) &&
                (g_EndMovieVariantIndex = 0,
                g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar6] <
                FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED)) &&
               (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar6] != 0)) {
              g_EndMovieVariantIndex = endTrigger->movieVariantSelector ^ 1;
              if (3 < (g_GameFactionRuntimeImage.records[uVar6].packedRelationStates >>
                       ((char)uVar10 * '\x04' & 0x1fU) & 0xf)) {
                g_EndMovieVariantIndex = (uint)endTrigger->movieVariantSelector;
              }
            }
            g_EndMovieSelectionIndex = (uint)endTrigger->endMovieSelectionIndex;
            g_EndMoviePath = (word *)u_flm_ende0000_flm_0050df06;
            if (g_EndMovieVariantIndex == 0) {
              g_EndMoviePath = (word *)u_flm_ende0001_flm_0050df28;
            }
            g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x800;
            return;
          }
        }
        endTrigger = endTrigger + 1;
        iVar8 = iVar8 + -1;
        if (iVar8 == 0) {
          return;
        }
      } while( true );
    }
  } while( true );
}


/* Address: 0x00513160.
   Ownership: gameplay/session/runtime.
   Purpose: Clears per-frame faction accumulators, decays auxiliary counters, removes transient field-cell flags,
   computes connected terrain-region contributions, sorts active runtime-capacity records, updates faction capacity
   and notifications, and refreshes the persistent stat table. [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE]
   Faction economy owner. FLD 0x0800 connected regions feed Xenite stock/rate/total; FLD 0x1000 regions feed
   Tritium stock/rate/total. Energy supply = baselineEnergySupplyQ4 + Tritium stock * 0x10, capped by
   energyGenerationCapacityQ4. [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE] Economy scalar domains are versionless
   canonical types: XeniteAmountQ4, TritiumAmountQ4, EnergyAmountQ4, EnergyDemandQ4 and
   ResourceExtractionRateQ4PerTick.
   Cross-module calls: TerrainRegionCollection_CollectConnectedCellsRecursive [world/terrain/editing],
   InGameNotificationQueue_InsertPriorityRecord [ui/ingame/runtime],
   GameFactionRuntime_RecomputeProgressAndScoreMetrics [gameplay/faction/runtime].
*/

void __fastcall InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState(void)

{
  byte *pbVar1;
  FieldGridDimension FVar2;
  int *piVar3;
  InGameLevelConditionStorageView800 *pIVar4;
  InGameRuntimeRootImageC3E4 *pIVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  uint uVar12;
  FactionArmyAssetCount FVar13;
  uint uVar14;
  FieldCellPackedFlagsAndMaterial requiredOccupancyMask;
  uint *puVar15;
  uint uVar16;
  uint uVar17;
  TerrainRegionCollectionCount TVar18;
  WorldRuntimeContext *worldRuntime;
  GameFactionRuntimeRecord *factionRecord;
  int iVar19;
  GameFactionRuntimeImage *pGVar20;
  GameFactionRuntimeRecord *reverseFactionRecord;
  int *piVar21;
  dword *pairPressureRow;
  FieldGridCell *gridCell1;
  FieldGridCell *gridCell2;
  FieldGridCell *cell;
  uint uVar22;
  uint *puVar23;
  InGameNotificationMovieId notificationMovieId;
  FieldGridAsset *fieldGrid1;
  WorldOwnerListNode100 *worldNode1;
  
  pIVar5 = g_InGameRuntimeRoot;
  factionRecord = g_GameFactionRuntimeImage.records;
  pairPressureRow = g_GameDataAuxState.pairPressureMatrix8x8;
  iVar8 = 8;
  do {
    factionRecord->suppliedEnergyDemandQ4 = 0;
    factionRecord->unpoweredEnergyDemandQ4 = 0;
    factionRecord->xeniteExtractionRateQ4PerTick = 0;
    factionRecord->tritiumExtractionRateQ4PerTick = 0;
    *pairPressureRow = *pairPressureRow * 7 >> 3;
    pairPressureRow[1] = pairPressureRow[1] * 7 >> 3;
    pairPressureRow[2] = pairPressureRow[2] * 7 >> 3;
    pairPressureRow[3] = pairPressureRow[3] * 7 >> 3;
    pairPressureRow[4] = pairPressureRow[4] * 7 >> 3;
    pairPressureRow[5] = pairPressureRow[5] * 7 >> 3;
    pairPressureRow[6] = pairPressureRow[6] * 7 >> 3;
    pairPressureRow[7] = pairPressureRow[7] * 7 >> 3;
    if (factionRecord->anchorCooldown1 != 0) {
      factionRecord->anchorCooldown1 = factionRecord->anchorCooldown1 - 1;
    }
    if (factionRecord->anchorCooldown2 != 0) {
      factionRecord->anchorCooldown2 = factionRecord->anchorCooldown2 - 1;
    }
    if (factionRecord->primaryAnchorCooldown != 0) {
      factionRecord->primaryAnchorCooldown = factionRecord->primaryAnchorCooldown - 1;
    }
    if (factionRecord->anchorCooldown0 != 0) {
      factionRecord->anchorCooldown0 = factionRecord->anchorCooldown0 - 1;
    }
    factionRecord->relationTransitionTick = factionRecord->relationTransitionTick + 1;
    pairPressureRow = pairPressureRow + 8;
    factionRecord = factionRecord + 1;
    iVar8 = iVar8 + -1;
  } while (iVar8 != 0);
  fieldGrid1 = (pIVar5->worldRuntime0A30).fieldGrid;
  FVar2 = fieldGrid1->gridWidth;
  iVar9 = FVar2 * fieldGrid1->gridHeight;
  gridCell1 = fieldGrid1->cells;
  requiredOccupancyMask = FIELD_CELL_XENITE_SUPPORT;
  iVar19 = 0;
  iVar8 = iVar9;
  gridCell2 = gridCell1;
  do {
    do {
      gridCell2->flagsAndMaterial =
           gridCell2->flagsAndMaterial & ~FIELD_CELL_CONNECTED_REGION_VISITED;
      iVar8 = iVar8 + -1;
      iVar11 = iVar9;
      cell = gridCell1;
      gridCell2 = gridCell2 + 1;
    } while (iVar8 != 0);
    do {
      if (((cell->flagsAndMaterial & 0x88016000) == 0) &&
         ((cell->flagsAndMaterial & requiredOccupancyMask) != 0)) {
        g_TerrainRegionCollectionStoredCount = 0;
        g_TerrainRegionCollectionVisitedCount = 0;
        TerrainRegionCollection_CollectConnectedCellsRecursive
                  (requiredOccupancyMask,FVar2 << 7,cell);
        if (g_TerrainRegionCollectionStoredCount != 0) {
          iVar8 = (int)g_TerrainRegionCollectionVisitedCount /
                  (int)g_TerrainRegionCollectionStoredCount;
          puVar15 = g_TerrainRegionCollectionEntries;
          TVar18 = g_TerrainRegionCollectionStoredCount;
          do {
            uVar22 = *puVar15 >> 0xd & 0x7ff;
            uVar6 = iVar8 * 2 * (*puVar15 >> 0x18) *
                    g_GameFactionRuntimeImage.records[uVar22].terrainContributionScaleQ8 >> 0xf;
            uVar12 = puVar15[1];
            piVar21 = (int *)(iVar19 + 0x50f348 + uVar22 * 0x740);
            *piVar21 = *piVar21 + uVar6;
            iVar7 = uVar6 * g_InGameSimulationStepTicks;
            pbVar1 = g_GameFactionRuntimeImage.records[uVar22].reserved78_87 + iVar19 + -0x78;
            *(int *)pbVar1 = *(int *)pbVar1 + iVar7;
            piVar21 = (int *)(iVar19 + 0x50f34c + uVar22 * 0x740);
            *piVar21 = *piVar21 + iVar7;
            if ((uVar12 != 0) &&
               (iVar10 = uVar12 + g_ModelRuntimeRebaseDelta, *(int *)(iVar10 + 4) != 0)) {
              *(int *)(iVar10 + 0x60) = iVar7;
            }
            puVar15 = puVar15 + 2;
            TVar18 = TVar18 - 1;
          } while (TVar18 != 0);
        }
      }
      iVar11 = iVar11 + -1;
      cell = cell + 1;
    } while (iVar11 != 0);
    requiredOccupancyMask = requiredOccupancyMask * 2;
    iVar19 = iVar19 + 0x10;
    iVar8 = iVar9;
    gridCell2 = gridCell1;
  } while (requiredOccupancyMask == FIELD_CELL_TRITIUM_SUPPORT);
  pGVar20 = &g_GameFactionRuntimeImage;
  iVar8 = 8;
  do {
    uVar12 = pGVar20->records[0].xeniteStorageLimitQ4;
    uVar6 = pGVar20->records[0].tritiumStorageLimitQ4;
    if (uVar12 < pGVar20->records[0].xeniteCurrentQ4) {
      pGVar20->records[0].xeniteCurrentQ4 = uVar12;
    }
    if (uVar6 < pGVar20->records[0].tritiumCurrentQ4) {
      pGVar20->records[0].tritiumCurrentQ4 = uVar6;
    }
    pGVar20 = (GameFactionRuntimeImage *)(pGVar20->records + 1);
    iVar8 = iVar8 + -1;
  } while (iVar8 != 0);
  uVar12 = 0;
  puVar15 = g_TerrainRegionCollectionEntries;
  for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      worldNode1 != (WorldOwnerListNode100 *)0x0; worldNode1 = worldNode1->nextNode) {
    if ((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
       (piVar21 = worldNode1->runtimePayload, (piVar21[0x3b] & 0x10U) == 0)) {
      if (piVar21[0x3d] != 0) {
        if (0xff < uVar12)
        goto InGameRuntime_UpdateFactionTerrainAndCapacityState_AdvanceTerrainRegionCollectionScan;
        uVar6 = *(uint *)(piVar21[2] + 0xc);
        iVar8 = *(int *)(*piVar21 + 0x4c);
        *puVar15 = (uint)piVar21;
        puVar15[1] = uVar6;
        uVar6 = piVar21[0x3d];
        puVar15[3] = *(uint *)(&g_FactionEnergyAllocationPriorityByModelClass + iVar8 * 4);
        puVar15[2] = uVar6;
        uVar12 = uVar12 + 1;
        puVar15 = puVar15 + 4;
      }
      iVar8 = piVar21[3];
      if ((uVar12 < 0x100) && ((*(uint *)(*piVar21 + 0x68) & 0x80) != 0)) {
        for (; iVar8 != 0; iVar8 = iVar8 + -1) {
          piVar3 = (int *)piVar21[0x50];
          if (((piVar3 != (int *)0x0) && (piVar3[0x3d] != 0)) && (uVar12 < 0x100)) {
            uVar6 = *(uint *)(piVar3[2] + 0xc);
            iVar9 = *(int *)(*piVar3 + 0x4c);
            *puVar15 = (uint)piVar3;
            puVar15[1] = uVar6;
            uVar6 = piVar3[0x3d];
            puVar15[3] = *(uint *)(&g_FactionEnergyAllocationPriorityByModelClass + iVar9 * 4);
            puVar15[2] = uVar6;
            uVar12 = uVar12 + 1;
            puVar15 = puVar15 + 4;
          }
          piVar21 = piVar21 + 8;
        }
      }
    }
InGameRuntime_UpdateFactionTerrainAndCapacityState_AdvanceTerrainRegionCollectionScan:
  }
  if (uVar12 != 0) {
    if (1 < uVar12) {
      uVar6 = g_TerrainRegionCollectionEntries[3];
      puVar15 = g_TerrainRegionCollectionEntries + 4;
      iVar8 = uVar12 - 1;
      uVar22 = uVar12;
      puVar23 = g_TerrainRegionCollectionEntries;
      while( true ) {
        do {
          if (uVar6 < puVar15[3]) {
            LOCK();
            uVar16 = puVar15[3];
            puVar15[3] = uVar6;
            UNLOCK();
            puVar23[3] = uVar16;
            LOCK();
            uVar6 = *puVar15;
            *puVar15 = *puVar23;
            UNLOCK();
            *puVar23 = uVar6;
            LOCK();
            uVar6 = puVar15[2];
            puVar15[2] = puVar23[2];
            UNLOCK();
            puVar23[2] = uVar6;
            LOCK();
            uVar6 = puVar15[1];
            puVar15[1] = puVar23[1];
            UNLOCK();
            puVar23[1] = uVar6;
            uVar6 = uVar16;
          }
          puVar15 = puVar15 + 4;
          iVar8 = iVar8 + -1;
        } while (iVar8 != 0);
        if (uVar22 - 1 < 2) break;
        uVar6 = puVar23[7];
        puVar15 = puVar23 + 8;
        iVar8 = uVar22 - 2;
        uVar22 = uVar22 - 1;
        puVar23 = puVar23 + 4;
      }
    }
    uVar6 = 7;
    reverseFactionRecord = g_GameFactionRuntimeImage.records + 7;
    do {
      puVar15 = g_TerrainRegionCollectionEntries;
      iVar8 = 0;
      uVar22 = 0;
      for (FVar13 = reverseFactionRecord->primaryArmyAssetCount; FVar13 != 0; FVar13 = FVar13 - 1) {
        if (*(int *)(reverseFactionRecord->primaryArmyAssetPointersOrIds[iVar8] + 0x74) == 0) {
          uVar22 = uVar22 + 0x10;
        }
        else {
          uVar22 = uVar22 + 0x50;
        }
        iVar8 = iVar8 + 1;
      }
      uVar16 = reverseFactionRecord->tritiumCurrentQ4 * 0x10 +
               reverseFactionRecord->baselineEnergySupplyQ4;
      if ((int)reverseFactionRecord->energyGenerationCapacityQ4 < (int)uVar16) {
        uVar16 = reverseFactionRecord->energyGenerationCapacityQ4;
      }
      reverseFactionRecord->suppliedEnergyDemandQ4 =
           reverseFactionRecord->suppliedEnergyDemandQ4 + uVar22;
      uVar17 = uVar16 - uVar22;
      uVar14 = uVar12;
      if (uVar16 < uVar22) {
        uVar17 = 0;
      }
      do {
        uVar22 = puVar15[2];
        if (uVar6 == puVar15[1]) {
          uVar16 = *puVar15;
          if (uVar17 < uVar22) {
            puVar23 = (uint *)(uVar16 + 0xec);
            *puVar23 = *puVar23 | 1;
            reverseFactionRecord->unpoweredEnergyDemandQ4 =
                 reverseFactionRecord->unpoweredEnergyDemandQ4 + uVar22;
          }
          else {
            uVar17 = uVar17 - uVar22;
            reverseFactionRecord->suppliedEnergyDemandQ4 =
                 reverseFactionRecord->suppliedEnergyDemandQ4 + uVar22;
            puVar23 = (uint *)(uVar16 + 0xec);
            *puVar23 = *puVar23 & 0xfffffffe;
          }
        }
        pIVar4 = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
        puVar15 = puVar15 + 4;
        uVar14 = uVar14 - 1;
      } while (uVar14 != 0);
      uVar22 = reverseFactionRecord->suppliedEnergyDemandQ4;
      uVar16 = uVar22 - reverseFactionRecord->baselineEnergySupplyQ4;
      if (uVar22 < reverseFactionRecord->baselineEnergySupplyQ4) {
        uVar16 = 0;
      }
      if (reverseFactionRecord->unpoweredEnergyDemandQ4 == 0) {
        reverseFactionRecord->anchorCooldown1 = 0;
      }
      else if ((g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex == uVar6) {
        if (reverseFactionRecord->energyGenerationCapacityQ4 <
            uVar22 + reverseFactionRecord->unpoweredEnergyDemandQ4) {
          if (reverseFactionRecord->anchorCooldown1 == 0) {
            notificationMovieId = 400;
            reverseFactionRecord->anchorCooldown1 = 0x96;
InGameRuntime_UpdateFactionTerrainAndCapacityState_QueueLocalCapacityStatusNotification:
            if (((*(int *)(pIVar4->levelImage).header.opaque100_16F == 0x300074) &&
                (*(int *)((pIVar4->levelImage).header.opaque100_16F + 4) == 0x5f0030)) &&
               (*(int *)((pIVar4->levelImage).header.opaque100_16F + 8) == 0x750074)) {
              notificationMovieId = 0x192;
              reverseFactionRecord->anchorCooldown1 = 0x7fffffff;
              reverseFactionRecord->anchorCooldown2 = 0x7fffffff;
            }
            InGameNotificationQueue_InsertPriorityRecord(NONE,0,0,0,0,0,3,notificationMovieId);
          }
        }
        else if (reverseFactionRecord->anchorCooldown2 == 0) {
          notificationMovieId = 0x191;
          reverseFactionRecord->anchorCooldown2 = 0x96;
          goto 
          InGameRuntime_UpdateFactionTerrainAndCapacityState_QueueLocalCapacityStatusNotification;
        }
      }
      reverseFactionRecord->tritiumCurrentQ4 =
           reverseFactionRecord->tritiumCurrentQ4 - (uVar16 >> 4) * g_InGameSimulationStepTicks;
      reverseFactionRecord = reverseFactionRecord + -1;
      uVar6 = uVar6 - 1;
    } while (uVar6 != 0);
  }
  if ((g_GameFactionRuntimeImage.tail.simulationTick & 0x78) == 0) {
    iVar8 = 0x50fa80;
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
    if (g_GameFactionRuntimeImage.tail.simulationTick >> 7 < 0x1000) {
      uVar12 = 1;
      piVar21 = (int *)((g_GameFactionRuntimeImage.tail.simulationTick >> 7) * 0x38 +
                       (int)g_GameStatTableImage);
      do {
        GameFactionRuntime_RecomputeProgressAndScoreMetrics(uVar12,worldRuntime);
        iVar9 = *(int *)(iVar8 + 0x88);
        iVar19 = *(int *)(iVar8 + 0x8c);
        if (iVar9 < 0) {
          iVar9 = 0;
        }
        if (iVar19 < 0) {
          iVar19 = 0;
        }
        *piVar21 = iVar9;
        piVar21[1] = iVar19;
        uVar12 = uVar12 + 1;
        iVar8 = iVar8 + 0x740;
        piVar21 = piVar21 + 2;
      } while (uVar12 < 8);
    }
  }
  return;
}


/* Address: 0x0053D4F0.
   Ownership: gameplay/session/runtime.
   Purpose: Converts the global in-game cursor world position to grid coordinates and refreshes the cached view-
   scale and settings-derived fields.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid], PersistentSettings_ReadDword
   [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx InGameRuntime_UpdateCursorGridAndViewScaleCache(void)

{
  UQ12 UVar1;
  dword dVar2;
  FieldGridCoordinatesEaxEdx8 FVar3;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  
  gameRuntime1 = g_InGameRuntimeRoot;
  FVar3 = FieldGrid_WorldToGridQ12
                    ((g_InGameRuntimeRoot->worldRuntime0A30).motion.targetPositionYQ12,
                     (g_InGameRuntimeRoot->worldRuntime0A30).motion.targetPositionXQ12);
  gameRuntime1->fieldGridPosition9A6C = (FixedPlanarPointEdxEax8)FVar3;
  dVar2 = PersistentSettings_ReadDword(0,0x40);
  UVar1 = (gameRuntime1->worldRuntime0A30).motion.committedDistanceQ12;
  if ((dVar2 & 2) == 0) {
    *(AngleTurn32 *)(gameRuntime1->opaque9A74_9B4B + 4) =
         (gameRuntime1->worldRuntime0A30).motion.headingAngle;
  }
  if ((dVar2 & 1) == 0) {
    *(int *)gameRuntime1->opaque9A74_9B4B =
         (int)((ulonglong)((longlong)(int)UVar1 * 0x6000000) >> 0x20);
  }
  return;
}


/* Address: 0x005651A0.
   Ownership: gameplay/session/runtime.
   Purpose: Copies the world-state pointer at root offset 0x23D8 into the shared runtime mirror and global world-
   state slot before shutdown traversal.
*/
void __thandor_void_preserve_eax_ecx
InGameRuntime_PublishRootWorldStatePointer(UiRootNode *inGameRoot)

{
  g_InGameWorldStatePointerMirror = inGameRoot[0x68].base.right;
  g_SharedWorldStatePointer = g_InGameWorldStatePointerMirror;
  return;
}


/* Address: 0x00565E30.
   Ownership: gameplay/session/runtime.
   Purpose: Runs under the gameplay spin lock, advances command and synchronization state, processes host or client
   transfer records, updates simulation entities and periodic world systems, and dispatches the active tick phase.
   Table 00566040: 00566060, 00566080, 00566090, 005660A0, 005660F0, 00566100, 00566110, 00566120. The function has
   no semantic return value; the prior EDX:EAX result was preserved-register noise. It contains, but is broader
   than, switch(simulationTick & 7). Do not rename the whole function to a narrow wheel-only name.
   Local calls: InGameConditionRuntime_UpdateScheduledRecords,
   InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState.
   Cross-module calls: UiRuntimeRecordRing_DiscardOldestCf [ui/core/runtime],
   FrontendTransfer_BroadcastPendingCommandBatchAndSyncState [network/protocol/transfer],
   FrontendTransfer_HandleSyncRequest10021AndReply10023 [network/protocol/transfer],
   FrontendTransfer_DispatchStagedCommandRecords [network/protocol/transfer], UiRuntimeRecordRing_ContainsIdCf
   [ui/core/runtime], FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf [network/backend/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx InGameRuntime_UpdateSimulationAndNetworkTick(void)

{
  dword dVar1;
  uint uVar2;
  WorldRuntimeContext *worldRuntime;
  WorldOwnerListNode100 *worldNode1;
  ModelRuntimeNode *modelNode1;
  bool bVar3;
  UiRuntimeRecordRingDiscardEaxEdxCf9 UVar4;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  
  bVar3 = (*g_SpinLockTryAcquire)(&g_InGameStateTickSpinLock);
  gameRuntime1 = g_InGameRuntimeRoot;
  if (bVar3) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & 0x10) == 0) {
    if (2 < g_InGamePendingSimulationTicks) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
    g_InGamePendingSimulationTicks = g_InGamePendingSimulationTicks + 1;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if (g_InGameNetworkTickCountdown != 0) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
    g_InGameNetworkTickCountdown = 4;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
        while (g_HostCommandBatchSyncSentThisInterval == 0) {
          UVar4 = UiRuntimeRecordRing_DiscardOldestCf();
          if (UVar4.carryEmpty) {
            bVar3 = FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(1);
            if (bVar3) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
            break;
          }
          FrontendTransfer_HandleSyncRequest10021AndReply10023
                    ((NetworkSessionContext *)UVar4.edxEndpointOrReadIndex,
                     (FrontendTransferPacketUnion *)UVar4.eaxPayloadOrReadIndex);
        }
        FrontendTransfer_DispatchStagedCommandRecords();
        g_HostCommandBatchSyncSentThisInterval = 0;
      }
      else {
        while( true ) {
          UVar4 = UiRuntimeRecordRing_DiscardOldestCf();
          if (UVar4.carryEmpty) break;
          FrontendTransfer_HandleSyncRequest10021AndReply10023
                    ((NetworkSessionContext *)UVar4.edxEndpointOrReadIndex,
                     (FrontendTransferPacketUnion *)UVar4.eaxPayloadOrReadIndex);
        }
        if ((g_HostCommandBatchSyncSentThisInterval == 0) &&
           (bVar3 = FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(0), !bVar3)) {
          g_HostCommandBatchSyncSentThisInterval = 1;
        }
      }
    }
  }
  else {
    if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
      bVar3 = UiRuntimeRecordRing_ContainsIdCf(g_FrontendSessionToken);
      if (!bVar3) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
      do {
        UVar4 = UiRuntimeRecordRing_DiscardOldestCf();
        if (UVar4.carryEmpty) break;
        bVar3 = FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf
                          ((NetworkSessionContext *)UVar4.edxEndpointOrReadIndex,
                           (FrontendTransferPacketUnion *)UVar4.eaxPayloadOrReadIndex);
      } while (!bVar3);
      bVar3 = FrontendTransfer_ConsumeProcessedFlagCf();
      if (bVar3) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
    }
    else if (g_InGameNetworkTickCountdown != 0)
    goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
    g_InGameNetworkTickCountdown = 4;
  }
  g_SessionNetworkTickCounter = g_SessionNetworkTickCounter + 1;
  if ((g_UiCommandRuntimeFlags & 4) == 0) {
    if ((g_UiCommandRuntimeFlags & 0x11) == 0) {
      g_GameFactionRuntimeImage.tail.simulationTick =
           g_GameFactionRuntimeImage.tail.simulationTick + 1;
      worldRuntime = &gameRuntime1->worldRuntime0A30;
      uVar2 = g_GameFactionRuntimeImage.tail.simulationTick & 7;
      for (worldNode1 = (gameRuntime1->worldRuntime0A30).ownerListHead;
          worldNode1 != (WorldOwnerListNode100 *)0x0; worldNode1 = worldNode1->nextNode) {
        (*(&g_RuntimeMaintenanceCallbackPhases.primaryUpdate.army)[worldNode1->ownerClassId])
                  (worldRuntime,worldNode1);
      }
      if (g_GameFactionRuntimeImage.tail.simulationTick % 0x14 == 0) {
        InGameConditionRuntime_UpdateScheduledRecords();
      }
                    // WARNING: Switch is manually overridden
      switch(uVar2) {
      case 0:
        InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState();
        FrontendRuntime_UpdateCurrentFactionMetricCache();
        GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10();
        WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
        break;
      case 1:
        TerrainGrid_RelaxNeighborHeightsForwardWithSignGate
                  ((gameRuntime1->worldRuntime0A30).fieldGrid);
        break;
      case 2:
        AiFactionRuntime_RebuildPlanningCapacityState();
        break;
      case 3:
        modelNode1 = (ModelRuntimeNode *)(gameRuntime1->worldRuntime0A30).ownerListHead;
        FieldGrid_ApplyByteClampLookupToCells
                  ((gameRuntime1->worldRuntime0A30).activeFactionRuntimeIndex,
                   (gameRuntime1->worldRuntime0A30).fieldGrid);
        if (modelNode1 != (ModelRuntimeNode *)0x0) {
          do {
            (*(&g_RuntimeMaintenanceCallbackPhases.terrainStateRefresh.army)
              [modelNode1->ownerClassId])(worldRuntime,modelNode1);
            modelNode1 = (ModelRuntimeNode *)(modelNode1->common).nextNode;
          } while (modelNode1 != (ModelRuntimeNode *)0x0);
          FieldGrid_ApplyByteClampLookupToCells
                    ((gameRuntime1->worldRuntime0A30).activeFactionRuntimeIndex,
                     (gameRuntime1->worldRuntime0A30).fieldGrid);
        }
        break;
      case 4:
        GridInfluence_ClearDistanceBandsAndRefreshEntities
                  ((gameRuntime1->worldRuntime0A30).ownerListHead);
        break;
      case 5:
        TerrainGrid_RelaxNeighborHeightsReverseWithSignGate
                  ((gameRuntime1->worldRuntime0A30).fieldGrid);
        break;
      case 6:
        AiFactionRuntime_RebuildPlanningCapacityState();
        break;
      case 7:
        worldNode1 = (gameRuntime1->worldRuntime0A30).ownerListHead;
        FieldGrid_ClearOccupancyMaskBits0To6AllCells((gameRuntime1->worldRuntime0A30).fieldGrid);
        if ((g_UiCommandRuntimeFlags & 8) != 0) {
          FieldGrid_SetOccupancyMaskByteBit0AllCells
                    ((gameRuntime1->worldRuntime0A30).activeFactionRuntimeIndex,
                     (gameRuntime1->worldRuntime0A30).fieldGrid);
        }
        if (worldNode1 != (WorldOwnerListNode100 *)0x0) {
          do {
            (*(&g_RuntimeMaintenanceCallbackPhases.occupancyRebuild.army)[worldNode1->ownerClassId])
                      (worldRuntime,worldNode1);
            worldNode1 = worldNode1->nextNode;
          } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
          modelNode1 = (ModelRuntimeNode *)(gameRuntime1->worldRuntime0A30).ownerListHead;
          do {
            (*(&g_RuntimeMaintenanceCallbackPhases.terrainStateRefresh.army)
              [modelNode1->ownerClassId])(worldRuntime,modelNode1);
            modelNode1 = (ModelRuntimeNode *)(modelNode1->common).nextNode;
          } while (modelNode1 != (ModelRuntimeNode *)0x0);
          FieldGrid_ApplyByteClampLookupToCells
                    ((gameRuntime1->worldRuntime0A30).activeFactionRuntimeIndex,
                     (gameRuntime1->worldRuntime0A30).fieldGrid);
          GridScratch_PropagateFieldOccupancyMaskNeighborhood
                    ((gameRuntime1->worldRuntime0A30).fieldGrid);
        }
      }
    }
  }
  else {
    modelNode1 = (ModelRuntimeNode *)(gameRuntime1->worldRuntime0A30).ownerListHead;
    g_GameFactionRuntimeImage.tail.simulationTick =
         g_GameFactionRuntimeImage.tail.simulationTick + 1;
    if ((g_GameFactionRuntimeImage.tail.simulationTick & 1) == 0) {
      for (; modelNode1 != (ModelRuntimeNode *)0x0;
          modelNode1 = (ModelRuntimeNode *)(modelNode1->common).nextNode) {
        if (modelNode1->ownerClassId == MODEL_RUNTIME_CLASS_00) {
          dVar1 = (((modelNode1->runtimePayload).modelRuntime)->definitionOrSavedId).savedIdOrOffset
          ;
          (*g_ArmyPlacementContactKindDispatchTable.callbacks[*(int *)(dVar1 + 0x278)])
                    (*(Q12 *)(dVar1 + 0x54),(modelNode1->worldTransform).translation.y,
                     (modelNode1->worldTransform).translation.x,modelNode1,
                     &gameRuntime1->worldRuntime0A30);
          ModelNodeRuntime_RebuildTransformsFromRoot(modelNode1);
        }
      }
      if ((g_GameFactionRuntimeImage.tail.simulationTick & 0xc) == 0) {
        if ((g_GameFactionRuntimeImage.tail.simulationTick & 2) == 0) {
          GridInfluence_ClearDistanceBandsAndRefreshEntities
                    ((gameRuntime1->worldRuntime0A30).ownerListHead);
        }
        else {
          GridScratch_RebuildTerrainAndRuntimeClassificationMasks(&gameRuntime1->worldRuntime0A30);
        }
      }
    }
  }
InGameRuntime_ReleaseSimulationTickLockAndReturn:
  (*g_SpinLockRelease)(&g_InGameStateTickSpinLock);
  return;
}


/* Address: 0x0050E0B0.
   Ownership: gameplay/session/runtime.
   Purpose: Initialization callback used by both new-session and loaded-session setup. Both binary call sites pass
   one argument and branch on carry immediately after return. The reachable implementation clears carry and returns
   at 0050E0C1-0050E0C3. The detached bytes at 0050E0C4-0050E0CF form an unreachable carry-set epilogue with no
   direct branch, call, or absolute-pointer reference in the archived executable, so they are verified separately
   and are not included in the function range.
*/
byte __thandor_cf_preserve_eax_ecx_edx
InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf(dword unusedArgument)

{
  return 0;
}

