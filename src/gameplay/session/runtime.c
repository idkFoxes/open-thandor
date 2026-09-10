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
undefined8 __fastcall
InGameRuntime_RunSessionUntilExit
          (undefined4 param_1,undefined4 param_2,LevelAssetRuntimeImagePrefix370 *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,word *levelPathUtf16)

{
  undefined4 extraout_EAX;
  bool bVar1;
  
  bVar1 = false;
  if ((loadExistingSessionFlag & 1U) == 0) {
    InGameRuntime_InitializeNewSession(param_1,param_2,levelAsset,levelPathUtf16);
    if (bVar1)
    goto InGameRuntime_RunSessionUntilExit_ShutdownAndReturnStartupOrUiRootFailureWithCarrySet;
  }
  else {
    InGameRuntime_InitializeLoadedSession(param_1,param_2,levelPathUtf16);
    if (bVar1)
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
      return CONCAT44(param_2,0xc);
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
      return CONCAT44(param_2,0xc);
    }
    if ((g_UiCommandRuntimeFlags & 0x20000) != 0) {
      (*g_SoundStopAllVoices)();
      (*g_TimerUnregisterPeriodic)(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      UiRuntime_SetSynchronizationHooks
                ((UiRuntimePostUnlockCallbackProc *)0x0,(RuntimeSpinLockValue *)0x0);
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16 = 0;
      return CONCAT44(param_2,0xc);
    }
  } while (g_UiRootNode != (UiRootNode *)0xffffffff);
InGameRuntime_RunSessionUntilExit_ShutdownAndReturnStartupOrUiRootFailureWithCarrySet:
  InGameRuntime_ShutdownAndReleaseResources();
  return CONCAT44(param_2,extraout_EAX);
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
void EndGameResultsUiRuntime_UpdateAndHandleInputCf(void *endGameResultsRuntime)

{
  ArmyRuntimeSlot *armyRuntime;
  longlong lVar1;
  InGameConditionRuntime *pIVar2;
  WorldRuntimeContext *worldRuntime;
  WorldRuntimeContext *worldRuntime_00;
  int iVar3;
  dword dVar4;
  int extraout_EAX;
  dword dVar5;
  int iVar6;
  IDirectSoundBuffer *pIVar7;
  int extraout_EAX_00;
  AngleTurn32 AVar8;
  UQ12 UVar9;
  uint extraout_ECX;
  int iVar10;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  uint uVar11;
  int extraout_ECX_03;
  InGamePresentationTick IVar12;
  uint extraout_ECX_04;
  int iVar13;
  int extraout_EDX;
  dword arg0;
  dword extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  WorldRuntimeContext *worldRuntime_01;
  int iVar14;
  int iVar15;
  uint uVar16;
  uint uVar17;
  byte *pbVar18;
  bool bVar19;
  undefined1 uVar20;
  qword qVar21;
  
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
                  (0xff808080,*(FieldGridAsset **)((int)endGameResultsRuntime + 0xa84));
        WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
                  ((void *)g_InGamePendingPlacementArmyAsset,worldRuntime_00);
      }
    }
    else if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xffffdfff;
      FieldGrid_SetAllCellOverlayColors
                (0xffffffff,*(FieldGridAsset **)((int)endGameResultsRuntime + 0xa84));
    }
    else if ((g_GameFactionRuntimeImage.tail.simulationTick & 7) == 0) {
      FieldGrid_SetAllCellOverlayColors
                (0xff808080,*(FieldGridAsset **)((int)endGameResultsRuntime + 0xa84));
      WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
                ((void *)g_InGamePendingPlacementArmyAsset,worldRuntime);
    }
    dVar4 = g_CursorOverrideY;
    iVar3 = (**(code **)(*(int *)((int)endGameResultsRuntime + 0xc) + 0x2c))
                      (g_CursorOverrideY,g_CursorOverrideX,endGameResultsRuntime,0);
    if (iVar3 != -1) {
      dVar4 = (**(code **)(*(int *)(iVar3 + 0xc) + 0x28))(g_CursorOverrideY,g_CursorOverrideX,iVar3)
      ;
    }
    g_GameFactionRuntimeImage.tail.presentationTick =
         g_GameFactionRuntimeImage.tail.presentationTick + 1;
    RecentTextHistory_SortAndBuildPointerList
              (8,(RecentTextHistoryPointerList *)((int)endGameResultsRuntime + 0x9b8));
    worldRuntime_01 = (WorldRuntimeContext *)((int)endGameResultsRuntime + 0xa30);
    InGameHud_UpdateStatusCountersAndSessionPrompts();
    UiPageStack_ActivePageNotInListCf((UiPageStackControl *)((int)endGameResultsRuntime + 0x9dc));
    if (((((*(uint *)((int)endGameResultsRuntime + 0xa7c) & 0x90) == 0) && (extraout_EAX == 0)) &&
        ((*(uint *)((int)endGameResultsRuntime + 0xa78) & 8) == 0)) &&
       (((g_CursorButtonState & 4) == 0 &&
        (dVar5 = WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(worldRuntime_01), dVar5 != 0)))) {
      dVar4 = dVar5;
    }
    (*g_GraphicsCursorSetFrame)(dVar4);
    iVar3 = *(int *)((int)endGameResultsRuntime + 0xa84);
    iVar15 = 0;
    qVar21 = FieldGrid_WorldToGridQ12
                       (*(Q12 *)((int)endGameResultsRuntime + 0xab4),
                        *(Q12 *)((int)endGameResultsRuntime + 0xab0));
    iVar13 = (int)(qVar21 >> 0x20);
    iVar6 = (int)qVar21;
    iVar14 = (iVar6 >> 0xc) + -8;
    iVar10 = (iVar13 >> 0xc) + -8;
    if (iVar14 < -0x10) {
      iVar6 = -0x8000;
      iVar15 = 1;
    }
    else if (*(int *)(iVar3 + 0xb8) < iVar14) {
      iVar15 = 1;
      iVar6 = (*(int *)(iVar3 + 0xb8) + 8) * 0x1000;
    }
    if (iVar10 < -0x10) {
      iVar13 = -0x8000;
      iVar15 = iVar15 + 1;
    }
    else if (*(int *)(iVar3 + 0xbc) < iVar10) {
      iVar15 = iVar15 + 1;
      iVar13 = (*(int *)(iVar3 + 0xbc) + 8) * 0x1000;
    }
    IVar12 = extraout_ECX;
    if (iVar15 != 0) {
      lVar1 = (longlong)(iVar13 + iVar6 * 2) * 0x901;
      iVar3 = ((int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd) -
              *(int *)((int)endGameResultsRuntime + 0xab0);
      iVar13 = ((int)((ulonglong)((longlong)iVar13 * -1999) >> 0x20) << 0x14 |
               (uint)((longlong)iVar13 * -1999) >> 0xc) -
               *(int *)((int)endGameResultsRuntime + 0xab4);
      *(int *)((int)endGameResultsRuntime + 0xab0) =
           *(int *)((int)endGameResultsRuntime + 0xab0) + iVar3;
      *(int *)((int)endGameResultsRuntime + 0xab4) =
           *(int *)((int)endGameResultsRuntime + 0xab4) + iVar13;
      *(int *)((int)endGameResultsRuntime + 0xa90) =
           *(int *)((int)endGameResultsRuntime + 0xa90) + iVar3;
      *(int *)((int)endGameResultsRuntime + 0xa94) =
           *(int *)((int)endGameResultsRuntime + 0xa94) + iVar13;
      WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime_01);
      IVar12 = extraout_ECX_00;
      iVar13 = extraout_EDX;
    }
    if ((g_UiCommandRuntimeFlags & 4) == 0) {
      TerrainDirectionTable_AdvanceAndRebuildVectors(IVar12,iVar13);
      dVar4 = PersistentSettings_ReadDword(3,0x20);
      if ((dVar4 & 1) != 0) {
        if ((extraout_ECX_01 & 7) == 0) {
          SpatialSoundPool_ClearDesiredGains();
          for (armyRuntime = *(ArmyRuntimeSlot **)((int)endGameResultsRuntime + 0xb08);
              armyRuntime != (ArmyRuntimeSlot *)0x0;
              armyRuntime = (ArmyRuntimeSlot *)armyRuntime->modelNodeRuntime) {
            (*(&g_RuntimeMaintenanceCallbackPhases.audioRefresh.army)[armyRuntime->runtimeStateA4])
                      (worldRuntime_01,armyRuntime);
          }
          SpatialSoundPool_ApplyDesiredGains();
          InGameSelectionDetailPanel_Rebuild();
        }
        bVar19 = false;
        if (g_InGameEffectsEnabled == 0) {
          (*g_SoundIsVoicePlaying)(g_InGameActiveEffectVoice);
          if (bVar19) {
            g_InGameActiveEffectVoice = (IDirectSoundBuffer *)0x0;
            dVar4 = Random_NextPrimary();
            g_InGameEffectsEnabled = (dVar4 & 0x3f) + 1;
          }
        }
        else {
          g_InGameEffectsEnabled = g_InGameEffectsEnabled - 1;
          if (g_InGameEffectsEnabled == 0) {
            PersistentSettings_ReadDword(0x8000,0x24);
            dVar4 = Random_NextPrimary();
            bVar19 = false;
            pIVar7 = (*g_SoundPlayOneShot)
                               (arg0,arg0,
                                (DirectSoundVoiceSet *)(&g_InGameLevelEffectVoiceSet0)[dVar4 & 3]);
            if (!bVar19) {
              g_InGameActiveEffectVoice = pIVar7;
            }
          }
        }
      }
      dVar4 = PersistentSettings_ReadDword(3,0x20);
      pIVar2 = g_InGameConditionRuntime;
      if ((dVar4 & 2) != 0) {
        bVar19 = false;
        if (g_InGameMusicEnabled == 0) {
          (*g_SoundIsVoicePlaying)(g_InGameActiveMusicVoice);
          if (bVar19) {
            g_InGameActiveMusicVoice = (IDirectSoundBuffer *)0x0;
            dVar4 = Random_NextPrimary();
            g_InGameMusicEnabled = (dVar4 & 0x3f) + 1;
          }
        }
        else {
          g_InGameMusicEnabled = g_InGameMusicEnabled - 1;
          if (g_InGameMusicEnabled == 0) {
            uVar16 = 0;
            do {
              dVar5 = InGameMusic_ComputeTrackSuitabilityScore
                                (*(MusicTrackClassId *)(pIVar2[3].reservedB0_D7 + uVar16 * 4 + 0xc),
                                 worldRuntime_01);
              uVar17 = uVar16 + 1;
              uVar11 = extraout_ECX_02;
              dVar4 = extraout_EDX_00;
              if ((int)extraout_EDX_00 < (int)dVar5) {
                uVar11 = uVar16;
                dVar4 = dVar5;
              }
              uVar16 = uVar17;
            } while (uVar17 < 4);
            uVar20 = 0;
            if (dVar4 != 0) {
              dVar4 = *(dword *)(pIVar2[3].reservedB0_D7 + uVar11 * 4 + 0xc);
              dVar5 = PersistentSettings_ReadDword(0x8000,0x2c);
              g_EndGameResultsCurrentMusicTrackId = dVar4;
              pIVar7 = (*g_SoundPlayOneShot)
                                 (dVar5,dVar5,
                                  (DirectSoundVoiceSet *)
                                  (&g_InGameLevelMusicVoiceSet0)[extraout_ECX_03]);
              if (!(bool)uVar20) {
                g_InGameActiveMusicVoice = pIVar7;
              }
            }
          }
        }
      }
      if ((g_UiCommandRuntimeFlags & 1) != 0)
      goto EndGameResultsUiRuntime_UpdateAndHandleInput_UpdateCursorGridAndReturn;
      UiPageStack_ActivePageNotInListCf((UiPageStackControl *)((int)endGameResultsRuntime + 0xbd0));
      if (extraout_EAX_00 == 2) {
        InGameTechnologyPanel_Rebuild(endGameResultsRuntime);
      }
      InterpolationStateTable_Advance256ByTicks(g_InGameSimulationStepTicks);
      if (g_KeyboardSpecialKeyDown[0x14] != 0) {
        dVar4 = PersistentSettings_ReadDword(0x20,0x48);
        WorldRuntime_TranslateCameraByScreenDelta(0,extraout_EDX_01 - dVar4,worldRuntime_01);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime_01);
      }
      if (g_KeyboardSpecialKeyDown[0x16] != 0) {
        dVar4 = PersistentSettings_ReadDword(0x20,0x48);
        WorldRuntime_TranslateCameraByScreenDelta(0,dVar4,worldRuntime_01);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime_01);
      }
      if (g_KeyboardSpecialKeyDown[0x11] != 0) {
        dVar4 = PersistentSettings_ReadDword(0x20,0x48);
        WorldRuntime_TranslateCameraByScreenDelta(extraout_EDX_02 - dVar4,0,worldRuntime_01);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime_01);
      }
      if (g_KeyboardSpecialKeyDown[0x19] != 0) {
        dVar4 = PersistentSettings_ReadDword(0x20,0x48);
        WorldRuntime_TranslateCameraByScreenDelta(dVar4,0,worldRuntime_01);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime_01);
      }
      if (g_KeyboardSpecialKeyDown[0x12] != 0) {
        AVar8 = *(int *)((int)endGameResultsRuntime + 0xaa4) - 0x400;
        if ((int)AVar8 < *(int *)((int)endGameResultsRuntime + 0xac0)) {
          AVar8 = *(AngleTurn32 *)((int)endGameResultsRuntime + 0xac0);
        }
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  (AVar8,*(AngleTurn32 *)((int)endGameResultsRuntime + 0xaa0),
                   *(UQ12 *)((int)endGameResultsRuntime + 0xabc),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab8),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab4),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab0),worldRuntime_01);
      }
      if (g_KeyboardSpecialKeyDown[0x1a] != 0) {
        AVar8 = *(int *)((int)endGameResultsRuntime + 0xaa4) + 0x400;
        if (*(int *)((int)endGameResultsRuntime + 0xac4) < (int)AVar8) {
          AVar8 = *(AngleTurn32 *)((int)endGameResultsRuntime + 0xac4);
        }
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  (AVar8,*(AngleTurn32 *)((int)endGameResultsRuntime + 0xaa0),
                   *(UQ12 *)((int)endGameResultsRuntime + 0xabc),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab8),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab4),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab0),worldRuntime_01);
      }
      if (g_KeyboardSpecialKeyDown[7] != 0) {
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  (*(AngleTurn32 *)((int)endGameResultsRuntime + 0xaa4),
                   *(int *)((int)endGameResultsRuntime + 0xaa0) - 0x400U & 0xffff,
                   *(UQ12 *)((int)endGameResultsRuntime + 0xabc),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab8),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab4),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab0),worldRuntime_01);
      }
      if (g_KeyboardSpecialKeyDown[6] != 0) {
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  (*(AngleTurn32 *)((int)endGameResultsRuntime + 0xaa4),
                   *(int *)((int)endGameResultsRuntime + 0xaa0) + 0x400U & 0xffff,
                   *(UQ12 *)((int)endGameResultsRuntime + 0xabc),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab8),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab4),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab0),worldRuntime_01);
      }
      if (g_KeyboardSpecialKeyDown[0x10] != 0) {
        UVar9 = *(int *)((int)endGameResultsRuntime + 0xabc) - 0x800;
        if ((int)UVar9 < *(int *)((int)endGameResultsRuntime + 0xac8)) {
          UVar9 = *(UQ12 *)((int)endGameResultsRuntime + 0xac8);
        }
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  (*(AngleTurn32 *)((int)endGameResultsRuntime + 0xaa4),
                   *(AngleTurn32 *)((int)endGameResultsRuntime + 0xaa0),UVar9,
                   *(Q12 *)((int)endGameResultsRuntime + 0xab8),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab4),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab0),worldRuntime_01);
      }
      if (g_KeyboardSpecialKeyDown[0x18] != 0) {
        UVar9 = *(int *)((int)endGameResultsRuntime + 0xabc) + 0x800;
        if (*(int *)((int)endGameResultsRuntime + 0xacc) < (int)UVar9) {
          UVar9 = *(UQ12 *)((int)endGameResultsRuntime + 0xacc);
        }
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  (*(AngleTurn32 *)((int)endGameResultsRuntime + 0xaa4),
                   *(AngleTurn32 *)((int)endGameResultsRuntime + 0xaa0),UVar9,
                   *(Q12 *)((int)endGameResultsRuntime + 0xab8),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab4),
                   *(Q12 *)((int)endGameResultsRuntime + 0xab0),worldRuntime_01);
      }
      pIVar2 = g_InGameConditionRuntime;
      *(uint *)((int)endGameResultsRuntime + 0x44c0) =
           *(uint *)((int)endGameResultsRuntime + 0x44c0) & 0xfffffff7;
      IVar12 = g_GameFactionRuntimeImage.tail.presentationTick;
      iVar3 = 0x40;
      pbVar18 = pIVar2[4].reserved00_4F;
      do {
        pbVar18 = pbVar18 + 0x10;
        if (((*(uint *)pbVar18 & 0xfe) == 0x14) && (*(uint *)(pbVar18 + 8) != 0)) {
          dVar4 = (*g_WideNumberFormatUtf16)
                            (WIDE_FORMAT_PAD_WITH_SPACE,0,2,1,*(uint *)(pbVar18 + 8) / 0x3c,
                             (word *)0x550590);
          *(undefined2 *)((dword)dVar4 + 0x550590) = 0x3a;
          (*g_WideNumberFormatUtf16)
                    (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                     (sdword)((ulonglong)dVar4 >> 0x20),(word *)((dword)dVar4 + 0x550592));
          IVar12 = g_GameFactionRuntimeImage.tail.presentationTick;
          goto 
          EndGameResultsUiRuntime_UpdateAndHandleInput_RefreshTerrainCompositeOnPresentationCadence;
        }
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
      *(uint *)((int)endGameResultsRuntime + 0x44c0) =
           *(uint *)((int)endGameResultsRuntime + 0x44c0) | 8;
    }
EndGameResultsUiRuntime_UpdateAndHandleInput_RefreshTerrainCompositeOnPresentationCadence:
    if ((IVar12 & 0x1f) == 0) {
      TerrainCompositeTexture_FillPlane1();
      IVar12 = extraout_ECX_04;
    }
    if ((IVar12 & 3) == 0) {
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
void InGameConditionRuntime_RebaseLoadedRecords(InGameConditionRuntime *runtime)

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
  InGameConditionRecordCount extraout_EDX;
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
      IVar10 = extraout_EDX;
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
void EndGameResultsUiRuntime_DispatchCommandByFlagsCf
               (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *endGameResultsRuntime
               )

{
  uint uVar1;
  UiCommandDispatchRecord *pUVar2;
  UiCommandDispatchRecord *pUVar3;
  
  pUVar3 = g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30;
  do {
    while( true ) {
      while( true ) {
        while( true ) {
          do {
            pUVar2 = pUVar3;
            uVar1 = pUVar2->modifierClassFlags;
            if (pUVar2->commandCode == 0) {
              return;
            }
            pUVar3 = pUVar2 + 1;
          } while (pUVar2->commandCode != commandCode);
          if (uVar1 != 0) break;
          if ((modifierFlags & 0x3c) == 0) {
            (*(code *)pUVar2->continuationEntryAddress)();
            return;
          }
        }
        if ((uVar1 & 0x30) != 0) break;
        if (((modifierFlags & 0xc) != 0) && ((modifierFlags & 0x30) == 0)) {
          (*(code *)pUVar2->continuationEntryAddress)();
          return;
        }
      }
      if ((uVar1 & 0xc) != 0) break;
      if (((modifierFlags & 0xc) == 0) && ((modifierFlags & 0x30) != 0)) {
        (*(code *)pUVar2->continuationEntryAddress)();
        return;
      }
    }
  } while (((modifierFlags & 0xc) == 0) || ((modifierFlags & 0x30) == 0));
  (*(code *)pUVar2->continuationEntryAddress)();
  return;
}

/* Address: 0x00569920.
   Ownership: gameplay/session/runtime.
   Purpose: Ten-millisecond gameplay timer that advances queued session notification and status records.
   Cross-module calls: Movie_Open [movie/runtime/playback], Movie_SetAudioGainQ15 [movie/runtime/playback],
   Movie_AdvanceFrame [movie/runtime/playback], Movie_Close [movie/runtime/playback].
*/
void __cdecl InGameRuntime_ProcessQueuedSessionNotificationTimer(void)

{
  uint arg4;
  GraphicsTextureSourceAsset *pGVar1;
  MovieRuntime *pMVar2;
  int iVar3;
  dword extraout_EDX;
  InGameNotificationPayload18 *pIVar4;
  InGameNotificationQueueRecord20 *pIVar5;
  InGameNotificationPayload18 *pIVar6;
  InGameNotificationQueueRecord20 *pIVar7;
  bool bVar8;
  undefined1 uVar9;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  
  pGVar1 = g_InGamePanelTextureSource;
  gameRuntime1 = g_InGameRuntimeRoot;
  if (((g_InGameSessionNotificationTimeoutTicks != 0) &&
      (g_InGameSessionNotificationTimeoutTicks = g_InGameSessionNotificationTimeoutTicks - 1,
      g_InGameSessionNotificationTimeoutTicks == 0)) &&
     (g_InGameRuntimeRoot->sessionNotificationInteractionState9B4C == PAYLOAD_ACTIVE)) {
    g_InGameRuntimeRoot->sessionNotificationInteractionState9B4C = NONE;
  }
  bVar8 = pGVar1 < (GraphicsTextureSourceAsset *)gameRuntime1->observedSessionNotificationValue9B50;
  if (pGVar1 == (GraphicsTextureSourceAsset *)gameRuntime1->observedSessionNotificationValue9B50) {
    uVar9 = 0;
    if (gameRuntime1->notificationQueue9E60[0].priority04 != 0) {
      arg4 = gameRuntime1->notificationQueue9E60[0].notificationMovieId00;
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_PAD_WITH_ZERO,0,3,1,arg4,(word *)(u_flm_movie000_flm_0056314e + 9));
      Movie_Open(0x80000000,(word *)u_flm_movie000_flm_0056314e);
      if (!(bool)uVar9) {
        uVar9 = arg4 < 100;
        if ((!(bool)uVar9) &&
           ((uVar9 = true, arg4 < 300 ||
            ((uVar9 = arg4 < 700, !(bool)uVar9 && (uVar9 = arg4 < 900, (bool)uVar9)))))) {
          Movie_SetAudioGainQ15(g_MovieAlternateAudioGainQ15);
        }
        pMVar2 = Movie_AdvanceFrame();
        if (!(bool)uVar9) {
          gameRuntime1->observedSessionNotificationValue9B50 = (dword)pMVar2;
          gameRuntime1->notificationPlaybackCompletionCode9B54 = 0;
          pIVar4 = &gameRuntime1->notificationQueue9E60[0].payload08;
          pIVar6 = &gameRuntime1->activeNotificationPayload9E40;
          for (iVar3 = 6; iVar3 != 0; iVar3 = iVar3 + -1) {
            pIVar6->primaryWorldCoordinateQ12_00 = pIVar4->primaryWorldCoordinateQ12_00;
            pIVar4 = (InGameNotificationPayload18 *)&pIVar4->secondaryWorldCoordinateQ12_04;
            pIVar6 = (InGameNotificationPayload18 *)&pIVar6->secondaryWorldCoordinateQ12_04;
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
      pIVar5 = gameRuntime1->notificationQueue9E60 + 1;
      pIVar7 = gameRuntime1->notificationQueue9E60;
      for (iVar3 = 0x18; iVar3 != 0; iVar3 = iVar3 + -1) {
        pIVar7->notificationMovieId00 = pIVar5->notificationMovieId00;
        pIVar5 = (InGameNotificationQueueRecord20 *)&pIVar5->priority04;
        pIVar7 = (InGameNotificationQueueRecord20 *)&pIVar7->priority04;
      }
      for (iVar3 = 8; iVar3 != 0; iVar3 = iVar3 + -1) {
        pIVar7->notificationMovieId00 = 0;
        pIVar7 = (InGameNotificationQueueRecord20 *)&pIVar7->priority04;
      }
    }
  }
  else {
    Movie_AdvanceFrame();
    if (bVar8) {
      Movie_Close();
      g_InGameSessionNotificationTimeoutTicks = 0x280;
      gameRuntime1->observedSessionNotificationValue9B50 = extraout_EDX;
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
undefined8 __fastcall
InGameRuntime_InitializeNewSession
          (undefined4 param_1,undefined4 param_2,LevelAssetRuntimeImagePrefix370 *levelAsset,
          word *levelMoviePath)

{
  WorldRuntimeFlags *pWVar1;
  WorldRuntimeContext *world;
  word wVar2;
  UiTextResourceId UVar3;
  PlayerRuntimeId PVar4;
  word *pwVar5;
  WorldObjectRecord *pWVar6;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  MovieRuntime *pMVar7;
  dword dVar8;
  dword gridHalfSize;
  dword subresourceCount;
  undefined4 extraout_EAX;
  int iVar9;
  FrontendPlayerRuntimeBlockCount FVar10;
  uint extraout_ECX;
  uint uVar11;
  undefined4 extraout_ECX_00;
  dword registerContext;
  undefined4 extraout_ECX_01;
  undefined4 extraout_EDX;
  UiPageStackControl *stack;
  FactionRuntimeIndex factionIndex;
  InGameNotificationMovieId notificationMovieId;
  InGameNotificationMovieId notificationMovieId_01;
  InGameNotificationMovieId notificationMovieId_02;
  InGameNotificationMovieId notificationMovieId_03;
  InGameNotificationMovieId notificationMovieId_00;
  SelectionPlayerRuntimeBlock *pSVar12;
  FrontendPlayerNameUtf16_28 *pFVar13;
  undefined4 *puVar14;
  FrontendPlayerRemovalPacket10007 *pFVar15;
  SelectionPlayerRuntimeBlock *pSVar16;
  FrontendPlayerRuntimeRecord *pFVar17;
  byte *pbVar18;
  undefined2 *puVar19;
  word *pwVar20;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  InGameNotificationQueueRecord20 *pIVar21;
  bool bVar22;
  undefined1 uVar23;
  undefined8 uVar24;
  
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
  pFVar15 = &g_FrontendClientPlayerRemovalPacket10007;
  for (iVar9 = 0xa0; iVar9 != 0; iVar9 = iVar9 + -1) {
    (pFVar15->header).packedTypeAndUnitCount = 0;
    pFVar15 = (FrontendPlayerRemovalPacket10007 *)&(pFVar15->header).sequenceToken;
  }
  pSVar16 = g_SelectionPlayerBlocks;
  for (iVar9 = 0x10230; FVar10 = g_FrontendPlayerRuntimeBlockCount,
      pSVar12 = g_SelectionPlayerBlocks, pFVar17 = g_FrontendPlayerRuntimeBlocks, iVar9 != 0;
      iVar9 = iVar9 + -1) {
    (pSVar16->selection).entries[0] = (GameEntityRuntime *)0x0;
    pSVar16 = (SelectionPlayerRuntimeBlock *)((pSVar16->selection).entries + 1);
  }
  do {
    iVar9 = pFVar17->playerRuntimeId;
    (pFVar17->factionAssignment).readyOrWaitState = 0;
    pFVar17->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    pFVar17->heartbeatExpiryTicks = 0x400;
    dVar8 = (pFVar17->factionAssignment).factionAssignmentIndex;
    g_SelectionPlayerRuntimeBlockPointers[iVar9] = pSVar12;
    pSVar12->primaryEntityOrFactionToken8080 = dVar8;
    pSVar12->simulationStepTicks = 1;
    pFVar13 = &pFVar17->playerName;
    pbVar18 = pSVar12->reserved80B0_8117 + 0x40;
    for (iVar9 = 0x14; iVar9 != 0; iVar9 = iVar9 + -1) {
      *(undefined4 *)pbVar18 = *(undefined4 *)pFVar13->textUtf16;
      pFVar13 = (FrontendPlayerNameUtf16_28 *)(pFVar13->textUtf16 + 2);
      pbVar18 = pbVar18 + 4;
    }
    FVar10 = FVar10 - 1;
    pSVar12 = pSVar12 + 1;
    pFVar17 = pFVar17 + 1;
  } while (FVar10 != 0);
  g_SessionTransferTimeoutTicks = 0x400;
  g_InGameNetworkTickCountdown = 4;
  g_InGameStateTickSpinLock = 0;
  (*g_TimerRegisterPeriodic)(0x50,InGameRuntime_PeriodicCountdownAndClockTick);
  UiRuntime_SetSynchronizationHooks
            (InGameRuntime_UpdateSimulationAndNetworkTick,&g_InGameStateTickSpinLock);
  UVar3 = (levelAsset->header).titleTextResourceIndex;
  puVar19 = &g_InGameSessionNameScratchUtf16;
  for (iVar9 = 0x20; iVar9 != 0; iVar9 = iVar9 + -1) {
    *puVar19 = 0;
    puVar19 = puVar19 + 1;
  }
  pwVar5 = TextResource_Resolve(UVar3 + 0x2230);
  pwVar20 = &g_InGameSessionNameScratchUtf16;
  iVar9 = 0x1f;
  do {
    pwVar5 = pwVar5 + 1;
    wVar2 = *pwVar5;
    bVar22 = false;
    if (wVar2 == 0) break;
    if ((((((wVar2 != 0x2a) && (wVar2 != 0x3c)) && (wVar2 != 0x3e)) &&
         (((wVar2 != 0x22 && (wVar2 != 0x2f)) &&
          ((wVar2 != 0x5c && ((wVar2 != 0x2e && (wVar2 != 0x3f)))))))) && (wVar2 != 0x3a)) &&
       (wVar2 != 0x7c)) {
      *pwVar20 = wVar2;
      pwVar20 = pwVar20 + 1;
    }
    bVar22 = (word *)0xfffffffd < pwVar5;
    iVar9 = iVar9 + -1;
  } while (iVar9 != 0);
  pWVar6 = (*g_MemoryApi.alloc)(0x400000);
  if (!bVar22) {
    g_RuntimeObjectRebaseBaseMinusOne = pWVar6[-1].classPayload + 0xaf;
    bVar22 = false;
    g_InGameWorldObjectRecords = pWVar6;
    for (iVar9 = 0x100000; iVar9 != 0; iVar9 = iVar9 + -1) {
      (pWVar6->common).reserved00_07[0] = 0;
      (pWVar6->common).reserved00_07[1] = 0;
      (pWVar6->common).reserved00_07[2] = 0;
      (pWVar6->common).reserved00_07[3] = 0;
      pWVar6 = (WorldObjectRecord *)((pWVar6->common).reserved00_07 + 4);
    }
    SelectionInfoPanel_InitResources
              ((SelectionInfoEntitySlots *)
               g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId]);
    if (!bVar22) {
      inGameRoot = (*g_MemoryApi.alloc)(0xc3e4);
      if (!bVar22) {
        bVar22 = (extraout_ECX >> 1 & 1) != 0;
        puVar14 = &g_InGameRuntimeDefaultImageTemplate;
        gameRuntime1 = inGameRoot;
        g_InGameRuntimeRoot = inGameRoot;
        for (uVar11 = extraout_ECX >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
          (gameRuntime1->rootUi0000).base.nextSibling = (UiNodeBase *)*puVar14;
          puVar14 = puVar14 + 1;
          gameRuntime1 = (InGameRuntimeRootImageC3E4 *)&(gameRuntime1->rootUi0000).base.firstChild;
        }
        world = &inGameRoot->worldRuntime0A30;
        InGameUiRuntime_InitializeControlTreeResourcesCf(&inGameRoot->rootUi0000);
        if (!bVar22) {
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
          (inGameRoot->worldRuntime0A30).reserved98 = 0x8000;
          (inGameRoot->worldRuntime0A30).surfaceSelectionFlags = 0x13000;
          (inGameRoot->worldRuntime0A30).motion.minimumPitchAngle = 0xffffc400;
          (inGameRoot->worldRuntime0A30).motion.maximumPitchAngle = 0xffffe800;
          (inGameRoot->worldRuntime0A30).tickSpinLock = &g_InGameStateTickSpinLock;
          (inGameRoot->worldRuntime0A30).simulationAndNetworkTickCallback =
               InGameRuntime_UpdateSimulationAndNetworkTick;
          uVar23 = (SelectionPlayerRuntimeBlock *)0xffffff7f <
                   g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
          pSVar16 = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
          inGameRoot->localPlayerPairCount0BA4 = 0;
          inGameRoot->localPlayerPairRecords0BA0 = pSVar16->pairRecords80_807F;
          UiRootStack_Push(&g_UiRootCallbacks_0054FBC0,&inGameRoot->rootUi0000);
          uVar24 = LevelAsset_PrepareEndingMoviePathCf(levelMoviePath,&levelAsset->header);
          if (((!(bool)uVar23) && (Movie_Open(0x80000000,(word *)uVar24), !(bool)uVar23)) &&
             (pMVar7 = Movie_AdvanceFrame(), !(bool)uVar23)) {
            inGameRoot->levelMovieRuntime08D4 = pMVar7;
            g_MoviePlaybackBaseFrameGroup = 0;
            g_MoviePlaybackScheduleCounter = 0;
            g_MoviePlaybackScheduleSpan = 0;
            g_MoviePlaybackCurrentFrame = 0;
            MoviePlayback_AdvanceToFrameAndPresent(0);
            MoviePlayback_AdvanceToFrameAndPresent(1);
            RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory09B8);
            WorldRuntime_AttachObjectArray(0x4000,g_InGameWorldObjectRecords,world);
            PVar4 = g_LocalPlayerRuntimeId;
            dVar8 = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId]->
                    primaryEntityOrFactionToken8080;
            levelAsset->playerSlots[6].aiClassOrMode = dVar8;
            (inGameRoot->worldRuntime0A30).activeFactionRuntimeIndex = dVar8;
            (inGameRoot->worldRuntime0A30).selection.activePlayerRuntimeId = PVar4;
            WorldRuntime_AttachAndClearDwordArray
                      (0x100,(dword *)&g_InGameWorldRuntimeDwordArray256,world);
            GameData_ResetDefaults();
            if ((!(bool)uVar23) &&
               (InGameLevelRuntime_LoadResourcesAfterDefaultResetCf
                          (extraout_ECX_00,extraout_EDX,levelAsset,world), !(bool)uVar23)) {
              bVar22 = false;
              pIVar21 = inGameRoot->notificationQueue9E60;
              for (iVar9 = 0x20; iVar9 != 0; iVar9 = iVar9 + -1) {
                pIVar21->notificationMovieId00 = 0;
                pIVar21 = (InGameNotificationQueueRecord20 *)&pIVar21->priority04;
              }
              TerrainCompositeTexture_Create();
              if (!bVar22) {
                (*g_SpinLockAcquire)(&g_InGameStateTickSpinLock);
                g_InGameSimulationStepTicks = 1;
                dVar8 = PersistentSettings_ReadDword(0x40,0x14);
                gridHalfSize = PersistentSettings_ReadDword(0x20,0x10);
                subresourceCount = PersistentSettings_ReadDword(0x10,0x18);
                GraphicsShadingRuntime_InitializeGeneratedTextureCf
                          (subresourceCount,gridHalfSize,dVar8);
                dVar8 = PersistentSettings_ReadDword(1,0x1c);
                if (dVar8 == 0) {
                  pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                  *pWVar1 = *pWVar1 & 0xfffdffff;
                }
                else {
                  pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                  *pWVar1 = *pWVar1 | 0x20000;
                }
                dVar8 = PersistentSettings_ReadDword(0,0x5c);
                if ((dVar8 & 1) == 0) {
                  pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                  *pWVar1 = *pWVar1 & 0xbfffffff;
                }
                else {
                  pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                  *pWVar1 = *pWVar1 | 0x40000000;
                }
                if ((dVar8 & 2) == 0) {
                  pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                  *pWVar1 = *pWVar1 & 0x7fffffff;
                }
                else {
                  pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                  *pWVar1 = *pWVar1 | 0x80000000;
                }
                if ((dVar8 & 4) == 0) {
                  pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                  *pWVar1 = *pWVar1 & 0xfbffffff;
                }
                else {
                  pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                  *pWVar1 = *pWVar1 | 0x4000000;
                }
                dVar8 = PersistentSettings_ReadDword(0,0x40);
                uVar23 = 0;
                if ((dVar8 & 4) != 0) {
                  UiPageStack_SetActiveIndex(1,&inGameRoot->optionalUiPageStack40AC);
                  UiPageStack_SetActiveIndex(0,&inGameRoot->optionalUiPageStack4530);
                  UiPageStack_SetActiveIndex(0,stack);
                  inGameRoot->optionalUiLayoutState0A04 = 0;
                  UiContainer_LayoutChildren((UiNodeBase *)inGameRoot);
                }
                InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf();
                if (!(bool)uVar23) {
                  uVar23 = 0;
                  if (g_FrontendLoadedCampaignAsset == 0) {
                    OldUnitRuntime_ResetPendingTables();
                  }
                  else {
                    OldUnitRuntime_MergeMasksAndReplayRecords();
                  }
                  GridScratch_AllocateForFieldGridCf((inGameRoot->worldRuntime0A30).fieldGrid);
                  if (!(bool)uVar23) {
                    GridScratch_RebuildTerrainAndRuntimeClassificationMasks(world);
                    GridInfluence_ClearDistanceBandsAndRefreshEntities
                              ((inGameRoot->worldRuntime0A30).ownerListHead);
                    TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks
                              (registerContext,factionIndex);
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
                    uVar24 = WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
                    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                        SESSION_NETWORK_ROLE_LOCAL) {
                      FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
                                (extraout_ECX_01,(int)((ulonglong)uVar24 >> 0x20),
                                 g_LocalPlayerRuntimeId);
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
                                (NONE,0,0,0,0,0,1,notificationMovieId_01);
                      InGameNotificationQueue_InsertPriorityRecord
                                (NONE,0,0,0,0,0,1,notificationMovieId_02);
                      InGameNotificationQueue_InsertPriorityRecord
                                (NONE,0,0,0,0,0,1,notificationMovieId_03);
                      InGameNotificationQueue_InsertPriorityRecord
                                (NONE,0,0,0,0,0,1,notificationMovieId_00);
                    }
                    return CONCAT44(param_2,inGameRoot);
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
  return CONCAT44(param_2,extraout_EAX);
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
undefined8 __fastcall
InGameRuntime_InitializeLoadedSession(undefined4 param_1,undefined4 param_2,word *savePackagePath)

{
  WorldRuntimeFlags *pWVar1;
  WorldRuntimeContext *world;
  SelectionPlayerRuntimeBlock *entitySlots;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  void *pvVar2;
  InGameRuntimeRootImageC3E4 *levelImage;
  InGameRuntimeRootImageC3E4 *extraout_EAX;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  dword dVar3;
  dword gridHalfSize;
  dword subresourceCount;
  int iVar4;
  uint extraout_ECX;
  undefined4 extraout_ECX_00;
  dword registerContext;
  undefined4 extraout_ECX_01;
  int extraout_EDX;
  undefined4 extraout_EDX_00;
  FactionRuntimeIndex factionIndex;
  byte *pbVar5;
  undefined4 *puVar6;
  undefined2 *puVar7;
  byte *pbVar8;
  byte *pbVar9;
  uint uVar10;
  short *psVar11;
  SelectionPlayerRuntimeBlock *pSVar12;
  InGameNotificationQueueRecord20 *pIVar13;
  undefined1 uVar14;
  bool bVar15;
  bool bVar16;
  undefined8 uVar17;
  InGameRuntimeRootImageC3E4 *fileHandle;
  InGameRuntimeRootImageC3E4 *loadedLevelAsset;
  
  uVar14 = &stack0xffffffe8 < (undefined1 *)0xc;
  g_TextureDownsampleShift = PersistentSettings_ReadDword(0,0x30);
  fileHandle = (InGameRuntimeRootImageC3E4 *)0x0;
  loadedLevelAsset = (InGameRuntimeRootImageC3E4 *)0x0;
  gameRuntime1 = (InGameRuntimeRootImageC3E4 *)Package_Mount(savePackagePath);
  pbVar5 = g_PackageScratchBuffer;
  if (!(bool)uVar14) {
    puVar7 = &g_InGameSessionNameScratchUtf16;
    for (iVar4 = 0x20; iVar4 != 0; iVar4 = iVar4 + -1) {
      *puVar7 = 0;
      puVar7 = puVar7 + 1;
    }
    fileHandle = gameRuntime1;
    (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,gameRuntime1);
    (*g_FileSystemReadExactCf)(0x200,pbVar5,gameRuntime1);
    pbVar5 = pbVar5 + 0x100;
    bVar15 = false;
    bVar16 = true;
    iVar4 = 0x24;
    pbVar8 = pbVar5;
    do {
      pbVar9 = pbVar8;
      if (iVar4 == 0) break;
      iVar4 = iVar4 + -1;
      pbVar9 = pbVar8 + 2;
      bVar15 = *(short *)pbVar8 != 0;
      bVar16 = *(short *)pbVar8 == 0;
      pbVar8 = pbVar9;
    } while (!bVar16);
    if (bVar16) {
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
      bVar15 = uVar10 < 0x20;
      if (!bVar15) {
        uVar10 = 0x1f;
      }
      for (; uVar10 != 0; uVar10 = uVar10 - 1) {
        *psVar11 = *(short *)pbVar5;
        pbVar5 = pbVar5 + 2;
        psVar11 = psVar11 + 1;
      }
    }
    pvVar2 = Package_LoadEntry((word *)u_campagne_hex_0050e068);
    if (!bVar15) {
      g_FrontendLoadedCampaignAsset = pvVar2;
    }
    levelImage = Package_LoadEntry((word *)u_level_hex_0050e040);
    gameRuntime1 = levelImage;
    if (!bVar15) {
      dVar3 = *(dword *)(levelImage->opaque0234_02F7 + 0xa8);
      uVar14 = 0;
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
      g_SelectionPlayerBlocks->primaryEntityOrFactionToken8080 = dVar3;
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
      gameRuntime1 = (*g_MemoryApi.alloc)(0x400000);
      loadedLevelAsset = levelImage;
      if (!(bool)uVar14) {
        g_RuntimeObjectRebaseBaseMinusOne = gameRuntime1[-1].opaqueA06C_C3E3 + 0x2377;
        bVar15 = false;
        g_InGameWorldObjectRecords = (WorldObjectRecord *)gameRuntime1;
        for (iVar4 = 0x100000; iVar4 != 0; iVar4 = iVar4 + -1) {
          (gameRuntime1->rootUi0000).base.nextSibling = (UiNodeBase *)0x0;
          gameRuntime1 = (InGameRuntimeRootImageC3E4 *)&(gameRuntime1->rootUi0000).base.firstChild;
        }
        SelectionInfoPanel_InitResources((SelectionInfoEntitySlots *)entitySlots);
        gameRuntime1 = extraout_EAX;
        if (!bVar15) {
          inGameRoot = (*g_MemoryApi.alloc)(0xc3e4);
          gameRuntime1 = inGameRoot;
          if (!bVar15) {
            bVar15 = (extraout_ECX >> 1 & 1) != 0;
            puVar6 = &g_InGameRuntimeDefaultImageTemplate;
            g_InGameRuntimeRoot = inGameRoot;
            for (uVar10 = extraout_ECX >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
              (gameRuntime1->rootUi0000).base.nextSibling = (UiNodeBase *)*puVar6;
              puVar6 = puVar6 + 1;
              gameRuntime1 = (InGameRuntimeRootImageC3E4 *)
                             &(gameRuntime1->rootUi0000).base.firstChild;
            }
            world = &inGameRoot->worldRuntime0A30;
            gameRuntime1 = (InGameRuntimeRootImageC3E4 *)
                           InGameUiRuntime_InitializeControlTreeResourcesCf(&inGameRoot->rootUi0000)
            ;
            if (!bVar15) {
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
              (inGameRoot->worldRuntime0A30).reserved98 = 0x8000;
              (inGameRoot->worldRuntime0A30).surfaceSelectionFlags = 0x13000;
              (inGameRoot->worldRuntime0A30).motion.minimumPitchAngle = 0xffffc400;
              (inGameRoot->worldRuntime0A30).motion.maximumPitchAngle = 0xffffe800;
              (inGameRoot->worldRuntime0A30).tickSpinLock = &g_InGameStateTickSpinLock;
              (inGameRoot->worldRuntime0A30).simulationAndNetworkTickCallback =
                   InGameRuntime_UpdateSimulationAndNetworkTick;
              pSVar12 = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
              inGameRoot->localPlayerPairCount0BA4 = 0;
              inGameRoot->localPlayerPairRecords0BA0 = pSVar12->pairRecords80_807F;
              UiRootStack_Push(&g_UiRootCallbacks_0054FBC0,&inGameRoot->rootUi0000);
              uVar14 = (InGameRuntimeRootImageC3E4 *)0xfffffeff < levelImage;
              WidePath_CombineDirectoryAndLeaf
                        (&g_FrontendScenarioPathScratchUtf16,
                         (word *)(levelImage->opaque0058_017B + 0xa8),(word *)u_level_0050daac);
              WidePath_SetExtensionCode(0x76656c,&g_FrontendScenarioPathScratchUtf16);
              uVar17 = LevelAsset_PrepareEndingMoviePathCf
                                 (savePackagePath,(LevelAssetHeader *)levelImage);
              gameRuntime1 = (InGameRuntimeRootImageC3E4 *)uVar17;
              if (((!(bool)uVar14) &&
                  (gameRuntime1 = (InGameRuntimeRootImageC3E4 *)
                                  Movie_Open(0x80000000,(word *)gameRuntime1), !(bool)uVar14)) &&
                 (gameRuntime1 = (InGameRuntimeRootImageC3E4 *)Movie_AdvanceFrame(), !(bool)uVar14))
              {
                inGameRoot->levelMovieRuntime08D4 = (MovieRuntime *)gameRuntime1;
                g_MoviePlaybackBaseFrameGroup = 0;
                g_MoviePlaybackScheduleCounter = 0;
                g_MoviePlaybackScheduleSpan = 0;
                g_MoviePlaybackCurrentFrame = 0;
                MoviePlayback_AdvanceToFrameAndPresent(0);
                MoviePlayback_AdvanceToFrameAndPresent(1);
                RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory09B8);
                WorldRuntime_AttachObjectArray(0x4000,g_InGameWorldObjectRecords,world);
                uVar14 = 0;
                (inGameRoot->worldRuntime0A30).activeFactionRuntimeIndex =
                     *(FactionRuntimeIndex *)(extraout_EDX + 0x8080);
                (inGameRoot->worldRuntime0A30).selection.activePlayerRuntimeId = 0;
                WorldRuntime_AttachAndClearDwordArray
                          (0x100,(dword *)&g_InGameWorldRuntimeDwordArray256,world);
                uVar17 = GameData_LoadExternalTables();
                gameRuntime1 = (InGameRuntimeRootImageC3E4 *)uVar17;
                if ((!(bool)uVar14) &&
                   (gameRuntime1 = Package_LoadEntry((word *)u_field_hex_0050e002), !(bool)uVar14))
                {
                  *(InGameRuntimeRootImageC3E4 **)(levelImage->opaque0058_017B + 0x58) =
                       gameRuntime1;
                  uVar17 = InGameLevelRuntime_LoadResourcesAfterExternalTablesCf
                                     (extraout_ECX_00,extraout_EDX_00,
                                      (LevelAssetRuntimeImagePrefix370 *)levelImage,world);
                  gameRuntime1 = (InGameRuntimeRootImageC3E4 *)uVar17;
                  if (!(bool)uVar14) {
                    bVar15 = false;
                    pIVar13 = inGameRoot->notificationQueue9E60;
                    for (iVar4 = 0x20; iVar4 != 0; iVar4 = iVar4 + -1) {
                      pIVar13->notificationMovieId00 = 0;
                      pIVar13 = (InGameNotificationQueueRecord20 *)&pIVar13->priority04;
                    }
                    gameRuntime1 = (InGameRuntimeRootImageC3E4 *)TerrainCompositeTexture_Create();
                    if (!bVar15) {
                      (*g_SpinLockAcquire)(&g_InGameStateTickSpinLock);
                      UiCatalogGroup48_RebuildGrid((UiNodeBase *)inGameRoot);
                      UiCatalogGroup42_RebuildGrid((UiNodeBase *)inGameRoot);
                      UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)inGameRoot);
                      InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)inGameRoot);
                      g_InGameSimulationStepTicks = 1;
                      dVar3 = PersistentSettings_ReadDword(0x40,0x14);
                      gridHalfSize = PersistentSettings_ReadDword(0x20,0x10);
                      subresourceCount = PersistentSettings_ReadDword(0x10,0x18);
                      GraphicsShadingRuntime_InitializeGeneratedTextureCf
                                (subresourceCount,gridHalfSize,dVar3);
                      dVar3 = PersistentSettings_ReadDword(1,0x1c);
                      if (dVar3 == 0) {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 & 0xfffdffff;
                      }
                      else {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 | 0x20000;
                      }
                      dVar3 = PersistentSettings_ReadDword(0,0x5c);
                      if ((dVar3 & 1) == 0) {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 & 0xbfffffff;
                      }
                      else {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 | 0x40000000;
                      }
                      if ((dVar3 & 2) == 0) {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 & 0x7fffffff;
                      }
                      else {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 | 0x80000000;
                      }
                      if ((dVar3 & 4) == 0) {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 & 0xfbffffff;
                      }
                      else {
                        pWVar1 = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *pWVar1 = *pWVar1 | 0x4000000;
                      }
                      bVar15 = false;
                      gameRuntime1 = (InGameRuntimeRootImageC3E4 *)
                                     InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf();
                      if ((!bVar15) &&
                         (gameRuntime1 = (InGameRuntimeRootImageC3E4 *)
                                         GridScratch_AllocateForFieldGridCf
                                                   ((inGameRoot->worldRuntime0A30).fieldGrid),
                         !bVar15)) {
                        GridScratch_RebuildTerrainAndRuntimeClassificationMasks(world);
                        GridInfluence_ClearDistanceBandsAndRefreshEntities
                                  ((inGameRoot->worldRuntime0A30).ownerListHead);
                        TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks
                                  (registerContext,factionIndex);
                        uVar17 = WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
                        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                            SESSION_NETWORK_ROLE_LOCAL) {
                          inGameRoot->localNetworkUiStateFlags24E0 =
                               inGameRoot->localNetworkUiStateFlags24E0 | 8;
                        }
                        else {
                          inGameRoot->localNetworkUiStateFlags24E0 =
                               inGameRoot->localNetworkUiStateFlags24E0 & 0xfffffff7;
                        }
                        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                            SESSION_NETWORK_ROLE_LOCAL) {
                          FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
                                    (extraout_ECX_01,(int)((ulonglong)uVar17 >> 0x20),
                                     g_LocalPlayerRuntimeId);
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
                        Package_Unmount((EngineFileHandle)fileHandle);
                        (*g_GraphicsCursorSetFrame)(0);
                        (*g_TimerRegisterPeriodic)
                                  (10,InGameRuntime_ProcessQueuedSessionNotificationTimer);
                        return CONCAT44(param_2,inGameRoot);
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
  Package_Unmount((EngineFileHandle)fileHandle);
  return CONCAT44(param_2,gameRuntime1);
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
void InGameRuntime_ShutdownAndReleaseResources(void)

{
  WorldRuntimeContext *levelAsset;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  void *this;
  undefined1 in_CF;
  
  (*g_TimerUnregisterPeriodic)(InGameRuntime_PeriodicCountdownAndClockTick);
  inGameRoot = g_InGameRuntimeRoot;
  (*g_GraphicsCursorSetFrame)(6);
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)in_CF) {
    (*g_GraphicsFramebufferFillRectArgb)
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
  }
  (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  GraphicsShadingRuntime_Shutdown();
  if (inGameRoot != (InGameRuntimeRootImageC3E4 *)0x0) {
    InGameRuntime_PublishRootWorldStatePointer(&inGameRoot->rootUi0000);
    levelAsset = &inGameRoot->worldRuntime0A30;
    WorldRuntime_ForEachNodeInOwnerListD8
              (levelAsset,WorldRuntimeNode_ReleaseShutdownBindingsCallback,levelAsset);
    InGameLevelRuntime_ShutdownLoadedAssetResources(this,(LevelAssetHeader *)levelAsset);
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
void InGameRuntime_ReleaseFactionScratchBuffers(void)

{
  int extraout_ECX;
  undefined4 *scratchBufferSetBCursor;
  undefined4 *scratchBufferSetACursor;
  
  scratchBufferSetACursor = &g_InGameFactionScratchBufferSetA8;
  scratchBufferSetBCursor = &g_InGameFactionScratchBufferSetB8;
  do {
    (*g_MemoryApi.free)((void *)*scratchBufferSetACursor);
    (*g_MemoryApi.free)((void *)*scratchBufferSetBCursor);
    *scratchBufferSetACursor = 0;
    *scratchBufferSetBCursor = 0;
    scratchBufferSetACursor = scratchBufferSetACursor + 1;
    scratchBufferSetBCursor = scratchBufferSetBCursor + 1;
  } while (extraout_ECX != 1);
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
void __fastcall InGameConditionRuntime_UpdateScheduledRecords(void)

{
  TerrainOccupancyMask *pTVar1;
  int iVar2;
  int *modelRuntime;
  InGameConditionRuntime *pIVar3;
  uint uVar4;
  byte bVar5;
  int iVar6;
  uint uVar7;
  uint extraout_EDX;
  byte *pbVar8;
  uint uVar9;
  FactionRuntimeLifecycleObservedState *pFVar10;
  WorldRuntimeContext *contextArg;
  FactionRuntimeLifecycleObservedState *pFVar11;
  WorldRuntimeNode *worldNode1;
  byte *pbVar12;
  FieldGridAsset *fieldGrid1;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  InGameRuntimeRootImageC3E4 *gameRuntime2;
  
  pFVar11 = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
  iVar6 = 7;
  do {
    pFVar11 = pFVar11 + 1;
    if (*pFVar11 == FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING) {
      *pFVar11 = *pFVar11 + FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    }
    pIVar3 = g_InGameConditionRuntime;
    iVar6 = iVar6 + -1;
  } while (iVar6 != 0);
  iVar6 = 0x40;
  pbVar12 = g_InGameConditionRuntime[4].reserved00_4F;
  do {
    pbVar12 = pbVar12 + 0x10;
    uVar7 = *(uint *)pbVar12;
    *(uint *)pbVar12 = *(uint *)pbVar12 & 0xfffffffe;
                    
    switch(uVar7 & 0xfe) {
    case 2:
      for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode1 != (WorldRuntimeNode *)0x0; worldNode1 = (worldNode1->common).nextNode) {
        if ((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
           (*(uint *)(pbVar12 + 4) == *(uint *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xc)
           )) goto InGameScheduledCondition_AdvanceToNextRecord;
      }
      goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
    case 4:
      for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode1 != (WorldRuntimeNode *)0x0; worldNode1 = (worldNode1->common).nextNode) {
        if (((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
            (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
             [*(int *)(*(int *)worldNode1->runtimePayload + 0x4c)] ==
             ArmyRuntime_ClassCommandHandlerGroupACf)) &&
           (*(uint *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xc) == *(uint *)(pbVar12 + 4)
           )) goto InGameScheduledCondition_AdvanceToNextRecord;
      }
      goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
    case 6:
      for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode1 != (WorldRuntimeNode *)0x0; worldNode1 = (worldNode1->common).nextNode) {
        if (((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
            (iVar2 = *(int *)((int)worldNode1->runtimePayload + 8),
            *(uint *)(pbVar12 + 4) == *(uint *)(iVar2 + 0xc))) &&
           (*(uint *)(iVar2 + 0xa0) == *(uint *)(pbVar12 + 0xc)))
        goto InGameScheduledCondition_AdvanceToNextRecord;
      }
      goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
    case 8:
      if (((g_GameFactionRuntimeImage.tail.factionLifecycleStates[*(uint *)(pbVar12 + 8)] !=
            FACTION_RUNTIME_LIFECYCLE_ACTIVE) ||
          (g_GameFactionRuntimeImage.tail.factionLifecycleStates[*(uint *)(pbVar12 + 4)] !=
           FACTION_RUNTIME_LIFECYCLE_ACTIVE)) ||
         (7 < (g_GameFactionRuntimeImage.records[*(uint *)(pbVar12 + 8)].packedRelationStates >>
               ((char)*(uint *)(pbVar12 + 4) * '\x04' & 0x1fU) & 0xf)))
      goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
      break;
    case 10:
      if ((int)*(uint *)(pbVar12 + 8) <=
          (int)g_GameFactionRuntimeImage.records[*(uint *)(pbVar12 + 4)].xeniteCurrentQ4) {
        *(uint *)pbVar12 = *(uint *)pbVar12 | 1;
      }
      break;
    case 0xc:
      if ((int)*(uint *)(pbVar12 + 8) <=
          (int)g_GameFactionRuntimeImage.records[*(uint *)(pbVar12 + 4)].tritiumCurrentQ4) {
        *(uint *)pbVar12 = *(uint *)pbVar12 | 1;
      }
      break;
    case 0xe:
      if ((int)*(uint *)(pbVar12 + 8) <=
          (int)g_GameFactionRuntimeImage.records[*(uint *)(pbVar12 + 4)].
               tritiumExtractionRateQ4PerTick) {
        *(uint *)pbVar12 = *(uint *)pbVar12 | 1;
      }
      break;
    case 0x10:
      uVar7 = *(uint *)(pbVar12 + 8);
      for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode1 != (WorldRuntimeNode *)0x0; worldNode1 = (worldNode1->common).nextNode) {
        if (((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
            (*(uint *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xc) ==
             *(uint *)(pbVar12 + 4))) &&
           ((*(uint *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xa0) ==
             *(uint *)(pbVar12 + 0xc) && (uVar7 = uVar7 - 1, uVar7 == 0))))
        goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
      }
      break;
    case 0x12:
      fieldGrid1 = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
      uVar9 = fieldGrid1->gridWidth * fieldGrid1->gridHeight;
      pbVar8 = fieldGrid1->cells[0].runtime00_07 + *(uint *)(pbVar12 + 4);
      uVar4 = 0;
      uVar7 = uVar9;
      do {
        pTVar1 = (TerrainOccupancyMask *)(pbVar8 + 0x70);
        pbVar8 = pbVar8 + 0x80;
        uVar4 = uVar4 + ((*pTVar1 & 0xf9) != 0);
        uVar7 = uVar7 - 1;
      } while (uVar7 != 0);
      if ((int)*(uint *)(pbVar12 + 8) <= (int)(((ulonglong)uVar4 * 100) / (ulonglong)uVar9)) {
        *(uint *)pbVar12 = *(uint *)pbVar12 | 1;
      }
      break;
    case 0x14:
      uVar7 = *(uint *)(pbVar12 + 8) - g_InGameSimulationStepTicks;
      *(uint *)(pbVar12 + 8) = uVar7;
      if ((int)uVar7 < 1) {
        *(uint *)pbVar12 = *(uint *)pbVar12 | 1;
        pbVar12[8] = 0;
        pbVar12[9] = 0;
        pbVar12[10] = 0;
        pbVar12[0xb] = 0;
      }
      break;
    case 0x16:
      if ((int)g_GameFactionRuntimeImage.records[*(uint *)(pbVar12 + 4)].xeniteStorageLimitQ4 <
          0xfa1) {
        *(uint *)pbVar12 = *(uint *)pbVar12 | 1;
      }
      break;
    case 0x18:
      for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode1 != (WorldRuntimeNode *)0x0; worldNode1 = (worldNode1->common).nextNode) {
        if (((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
            (uVar7 = *(uint *)(*(int *)worldNode1->runtimePayload + 0x4c),
            g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[uVar7] !=
            ArmyRuntime_ClassCommandHandlerGroupACf)) && (uVar7 == *(uint *)(pbVar12 + 4)))
        goto InGameScheduledCondition_AdvanceToNextRecord;
      }
InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied:
      *(uint *)pbVar12 = *(uint *)pbVar12 | 1;
      break;
    case 0x1a:
      pbVar8 = pbVar12 + 1;
      uVar7 = 0;
      while( true ) {
        while( true ) {
          while( true ) {
            while( true ) {
              bVar5 = *pbVar8;
              pbVar8 = pbVar8 + 1;
              if (bVar5 != 0xff) break;
              uVar7 = uVar7 >> 1 | uVar7 & 1;
            }
            if (bVar5 != 0xfe) break;
            uVar7 = uVar7 >> 1 & (uVar7 | 0xfffffffe);
          }
          if (bVar5 != 0xfd) break;
          uVar7 = uVar7 ^ 1;
        }
        if (bVar5 == 0xfc) break;
        uVar7 = (*(uint *)(pIVar3[4].reserved00_4F + (uint)bVar5 * 0x10 + 0x10) & 1) + uVar7 * 2;
      }
      *(uint *)pbVar12 = *(uint *)pbVar12 | uVar7 & 1;
    }
InGameScheduledCondition_AdvanceToNextRecord:
    iVar6 = iVar6 + -1;
    if (iVar6 == 0) {
      pbVar12 = pIVar3[8].reserved5C_AB + 0x44;
      iVar6 = 0x10;
      do {
        if ((*pbVar12 == 1) &&
           ((*(uint *)(pIVar3[4].reserved00_4F + (uint)pbVar12[6] * 0x10 + 0x10) & 1) != 0)) {
          *pbVar12 = *pbVar12 | 2;
          gameRuntime1 = g_InGameRuntimeRoot;
          uVar7 = (uint)pbVar12[4];
          if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar7] ==
              FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
            contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
            g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar7] =
                 FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING;
            if (pbVar12[2] != 1) {
              worldNode1 = (gameRuntime1->worldRuntime0A30).ownerListHead;
              if (worldNode1 != (WorldRuntimeNode *)0x0) {
                do {
                  if ((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
                     (modelRuntime = *(int **)((int)worldNode1->runtimePayload + 8),
                     uVar7 == modelRuntime[3])) {
                    ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive(contextArg,modelRuntime);
                    uVar7 = extraout_EDX;
                  }
                  worldNode1 = (worldNode1->common).nextNode;
                } while (worldNode1 != (WorldRuntimeNode *)0x0);
                g_GameFactionRuntimeImage.records[uVar7].secondaryArmyAssetCount = 0;
                g_GameFactionRuntimeImage.records[uVar7].primaryArmyAssetCount = 0;
              }
              gameRuntime2 = g_InGameRuntimeRoot;
              if (uVar7 == (gameRuntime1->worldRuntime0A30).activeFactionRuntimeIndex) {
                g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x108;
              }
              pFVar11 = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
              uVar7 = 1;
              bVar5 = 4;
              do {
                pFVar11 = pFVar11 + 1;
                if (*pFVar11 == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
                  uVar4 = uVar7 + 1;
                  pFVar10 = pFVar11;
                  do {
                    pFVar10 = pFVar10 + 1;
                    if ((*pFVar10 == FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
                       ((g_GameFactionRuntimeImage.records[uVar4].packedRelationStates >>
                         (bVar5 & 0x1f) & 0xf) < 8)) {
                      if ((uint)pbVar12[4] ==
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
                    uVar4 = uVar4 + 1;
                  } while (uVar4 < 8);
                }
                uVar7 = uVar7 + 1;
                bVar5 = bVar5 + 4;
              } while (uVar7 < 7);
              contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
              uVar7 = (uint)pbVar12[4];
            }
            uVar4 = contextArg->activeFactionRuntimeIndex;
            g_EndMovieVariantIndex = (uint)pbVar12[1];
            if (((uVar4 != uVar7) &&
                (g_EndMovieVariantIndex = 0,
                g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar4] <
                FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED)) &&
               (g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar4] != 0)) {
              g_EndMovieVariantIndex = pbVar12[1] ^ 1;
              if (3 < (g_GameFactionRuntimeImage.records[uVar4].packedRelationStates >>
                       ((char)uVar7 * '\x04' & 0x1fU) & 0xf)) {
                g_EndMovieVariantIndex = (uint)pbVar12[1];
              }
            }
            g_EndMovieSelectionIndex = (uint)pbVar12[5];
            g_EndMoviePath = (word *)u_flm_ende0000_flm_0050df06;
            if (g_EndMovieVariantIndex == 0) {
              g_EndMoviePath = (word *)u_flm_ende0001_flm_0050df28;
            }
            g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x800;
            return;
          }
        }
        pbVar12 = pbVar12 + 8;
        iVar6 = iVar6 + -1;
        if (iVar6 == 0) {
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
  FactionAnchorCooldownTicks *pFVar1;
  FactionRelationTick *pFVar2;
  byte *pbVar3;
  FieldGridDimension FVar4;
  int *piVar5;
  InGameConditionRuntime *pIVar6;
  InGameRuntimeRootImageC3E4 *pIVar7;
  uint uVar8;
  int iVar9;
  uint extraout_EAX;
  int iVar10;
  int iVar11;
  int extraout_ECX;
  int iVar12;
  int incomingEcxValue;
  uint uVar13;
  uint uVar14;
  int extraout_ECX_00;
  FieldCellPackedFlagsAndMaterial requiredOccupancyMask;
  uint *puVar15;
  uint uVar16;
  uint uVar17;
  TerrainRegionCollectionCount TVar18;
  WorldRuntimeContext *worldRuntime;
  GameFactionRuntimeImage *pGVar19;
  int iVar20;
  int *piVar21;
  GameDataAuxState *pGVar22;
  FieldGridCell *gridCell1;
  FieldGridCell *gridCell2;
  FieldGridCell *cell;
  uint uVar23;
  uint *puVar24;
  undefined8 uVar25;
  InGameNotificationMovieId notificationMovieId;
  FieldGridAsset *fieldGrid1;
  WorldRuntimeNode *worldNode1;
  
  pIVar7 = g_InGameRuntimeRoot;
  pGVar19 = &g_GameFactionRuntimeImage;
  pGVar22 = &g_GameDataAuxState;
  iVar10 = 8;
  do {
    pGVar19->records[0].suppliedEnergyDemandQ4 = 0;
    pGVar19->records[0].unpoweredEnergyDemandQ4 = 0;
    pGVar19->records[0].xeniteExtractionRateQ4PerTick = 0;
    pGVar19->records[0].tritiumExtractionRateQ4PerTick = 0;
    pGVar22->pairPressureMatrix8x8[0] = pGVar22->pairPressureMatrix8x8[0] * 7 >> 3;
    *(dword *)((int)pGVar22 + 4) = *(dword *)((int)pGVar22 + 4) * 7 >> 3;
    *(dword *)((int)pGVar22 + 8) = *(dword *)((int)pGVar22 + 8) * 7 >> 3;
    *(dword *)((int)pGVar22 + 0xc) = *(dword *)((int)pGVar22 + 0xc) * 7 >> 3;
    *(dword *)((int)pGVar22 + 0x10) = *(dword *)((int)pGVar22 + 0x10) * 7 >> 3;
    *(dword *)((int)pGVar22 + 0x14) = *(dword *)((int)pGVar22 + 0x14) * 7 >> 3;
    *(dword *)((int)pGVar22 + 0x18) = *(dword *)((int)pGVar22 + 0x18) * 7 >> 3;
    *(dword *)((int)pGVar22 + 0x1c) = *(dword *)((int)pGVar22 + 0x1c) * 7 >> 3;
    if (pGVar19->records[0].anchorCooldown1 != 0) {
      pFVar1 = &pGVar19->records[0].anchorCooldown1;
      *pFVar1 = *pFVar1 - 1;
    }
    if (pGVar19->records[0].anchorCooldown2 != 0) {
      pFVar1 = &pGVar19->records[0].anchorCooldown2;
      *pFVar1 = *pFVar1 - 1;
    }
    if (pGVar19->records[0].primaryAnchorCooldown != 0) {
      pFVar1 = &pGVar19->records[0].primaryAnchorCooldown;
      *pFVar1 = *pFVar1 - 1;
    }
    if (pGVar19->records[0].anchorCooldown0 != 0) {
      pFVar1 = &pGVar19->records[0].anchorCooldown0;
      *pFVar1 = *pFVar1 - 1;
    }
    pFVar2 = &pGVar19->records[0].relationTransitionTick;
    *pFVar2 = *pFVar2 + 1;
    pGVar22 = (GameDataAuxState *)((int)pGVar22 + 0x20);
    pGVar19 = (GameFactionRuntimeImage *)(pGVar19->records + 1);
    iVar10 = iVar10 + -1;
  } while (iVar10 != 0);
  fieldGrid1 = (pIVar7->worldRuntime0A30).fieldGrid;
  FVar4 = fieldGrid1->gridWidth;
  iVar11 = FVar4 * fieldGrid1->gridHeight;
  gridCell1 = fieldGrid1->cells;
  requiredOccupancyMask = FIELD_CELL_XENITE_SUPPORT;
  iVar20 = 0;
  iVar10 = iVar11;
  gridCell2 = gridCell1;
  do {
    do {
      gridCell2->flagsAndMaterial =
           gridCell2->flagsAndMaterial & ~FIELD_CELL_CONNECTED_REGION_VISITED;
      iVar10 = iVar10 + -1;
      incomingEcxValue = iVar11;
      cell = gridCell1;
      gridCell2 = gridCell2 + 1;
    } while (iVar10 != 0);
    do {
      if (((cell->flagsAndMaterial & 0x88016000) == 0) &&
         ((cell->flagsAndMaterial & requiredOccupancyMask) != 0)) {
        g_TerrainRegionCollectionStoredCount = 0;
        g_TerrainRegionCollectionVisitedCount = 0;
        uVar25 = TerrainRegionCollection_CollectConnectedCellsRecursive
                           (incomingEcxValue,requiredOccupancyMask,requiredOccupancyMask,FVar4 << 7,
                            cell);
        requiredOccupancyMask = (FieldCellPackedFlagsAndMaterial)((ulonglong)uVar25 >> 0x20);
        incomingEcxValue = extraout_ECX;
        if (g_TerrainRegionCollectionStoredCount != 0) {
          iVar10 = (int)g_TerrainRegionCollectionVisitedCount /
                   (int)g_TerrainRegionCollectionStoredCount;
          puVar15 = g_TerrainRegionCollectionEntries;
          TVar18 = g_TerrainRegionCollectionStoredCount;
          do {
            uVar23 = *puVar15 >> 0xd & 0x7ff;
            uVar8 = iVar10 * 2 * (*puVar15 >> 0x18) *
                    g_GameFactionRuntimeImage.records[uVar23].terrainContributionScaleQ8 >> 0xf;
            uVar13 = puVar15[1];
            piVar21 = (int *)(iVar20 + 0x50f348 + uVar23 * 0x740);
            *piVar21 = *piVar21 + uVar8;
            iVar9 = uVar8 * g_InGameSimulationStepTicks;
            pbVar3 = g_GameFactionRuntimeImage.records[uVar23].reserved78_87 + iVar20 + -0x78;
            *(int *)pbVar3 = *(int *)pbVar3 + iVar9;
            piVar21 = (int *)(iVar20 + 0x50f34c + uVar23 * 0x740);
            *piVar21 = *piVar21 + iVar9;
            if ((uVar13 != 0) &&
               (iVar12 = uVar13 + g_ModelRuntimeRebaseDelta, *(int *)(iVar12 + 4) != 0)) {
              *(int *)(iVar12 + 0x60) = iVar9;
            }
            puVar15 = puVar15 + 2;
            TVar18 = TVar18 - 1;
          } while (TVar18 != 0);
        }
      }
      incomingEcxValue = incomingEcxValue + -1;
      cell = cell + 1;
    } while (incomingEcxValue != 0);
    requiredOccupancyMask = requiredOccupancyMask * 2;
    iVar20 = iVar20 + 0x10;
    iVar10 = iVar11;
    gridCell2 = gridCell1;
  } while (requiredOccupancyMask == FIELD_CELL_TRITIUM_SUPPORT);
  pGVar19 = &g_GameFactionRuntimeImage;
  iVar10 = 8;
  do {
    uVar13 = pGVar19->records[0].xeniteStorageLimitQ4;
    uVar8 = pGVar19->records[0].tritiumStorageLimitQ4;
    if (uVar13 < pGVar19->records[0].xeniteCurrentQ4) {
      pGVar19->records[0].xeniteCurrentQ4 = uVar13;
    }
    if (uVar8 < pGVar19->records[0].tritiumCurrentQ4) {
      pGVar19->records[0].tritiumCurrentQ4 = uVar8;
    }
    pGVar19 = (GameFactionRuntimeImage *)(pGVar19->records + 1);
    iVar10 = iVar10 + -1;
  } while (iVar10 != 0);
  uVar13 = 0;
  puVar15 = g_TerrainRegionCollectionEntries;
  for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      worldNode1 != (WorldRuntimeNode *)0x0; worldNode1 = (worldNode1->common).nextNode) {
    if ((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
       (piVar21 = worldNode1->runtimePayload, (piVar21[0x3b] & 0x10U) == 0)) {
      if (piVar21[0x3d] != 0) {
        if (0xff < uVar13)
        goto InGameRuntime_UpdateFactionTerrainAndCapacityState_AdvanceTerrainRegionCollectionScan;
        uVar8 = *(uint *)(piVar21[2] + 0xc);
        iVar10 = *(int *)(*piVar21 + 0x4c);
        *puVar15 = (uint)piVar21;
        puVar15[1] = uVar8;
        uVar8 = piVar21[0x3d];
        puVar15[3] = *(uint *)(&g_FactionEnergyAllocationPriorityByModelClass + iVar10 * 4);
        puVar15[2] = uVar8;
        uVar13 = uVar13 + 1;
        puVar15 = puVar15 + 4;
      }
      iVar10 = piVar21[3];
      if ((uVar13 < 0x100) && ((*(uint *)(*piVar21 + 0x68) & 0x80) != 0)) {
        for (; iVar10 != 0; iVar10 = iVar10 + -1) {
          piVar5 = (int *)piVar21[0x50];
          if (((piVar5 != (int *)0x0) && (piVar5[0x3d] != 0)) && (uVar13 < 0x100)) {
            uVar8 = *(uint *)(piVar5[2] + 0xc);
            iVar11 = *(int *)(*piVar5 + 0x4c);
            *puVar15 = (uint)piVar5;
            puVar15[1] = uVar8;
            uVar8 = piVar5[0x3d];
            puVar15[3] = *(uint *)(&g_FactionEnergyAllocationPriorityByModelClass + iVar11 * 4);
            puVar15[2] = uVar8;
            uVar13 = uVar13 + 1;
            puVar15 = puVar15 + 4;
          }
          piVar21 = piVar21 + 8;
        }
      }
    }
InGameRuntime_UpdateFactionTerrainAndCapacityState_AdvanceTerrainRegionCollectionScan:
  }
  if (uVar13 != 0) {
    if (1 < uVar13) {
      uVar8 = g_TerrainRegionCollectionEntries[3];
      puVar15 = g_TerrainRegionCollectionEntries + 4;
      iVar10 = uVar13 - 1;
      uVar23 = uVar13;
      puVar24 = g_TerrainRegionCollectionEntries;
      while( true ) {
        do {
          if (uVar8 < puVar15[3]) {
            LOCK();
            uVar16 = puVar15[3];
            puVar15[3] = uVar8;
            UNLOCK();
            puVar24[3] = uVar16;
            LOCK();
            uVar8 = *puVar15;
            *puVar15 = *puVar24;
            UNLOCK();
            *puVar24 = uVar8;
            LOCK();
            uVar8 = puVar15[2];
            puVar15[2] = puVar24[2];
            UNLOCK();
            puVar24[2] = uVar8;
            LOCK();
            uVar8 = puVar15[1];
            puVar15[1] = puVar24[1];
            UNLOCK();
            puVar24[1] = uVar8;
            uVar8 = uVar16;
          }
          puVar15 = puVar15 + 4;
          iVar10 = iVar10 + -1;
        } while (iVar10 != 0);
        if (uVar23 - 1 < 2) break;
        uVar8 = puVar24[7];
        puVar15 = puVar24 + 8;
        iVar10 = uVar23 - 2;
        uVar23 = uVar23 - 1;
        puVar24 = puVar24 + 4;
      }
    }
    uVar8 = 7;
    iVar10 = 0x512600;
    do {
      puVar15 = g_TerrainRegionCollectionEntries;
      iVar20 = 0;
      uVar23 = 0;
      for (iVar11 = *(int *)(iVar10 + 0x30); iVar11 != 0; iVar11 = iVar11 + -1) {
        if (*(int *)(*(int *)(iVar10 + 0x1e0 + iVar20 * 4) + 0x74) == 0) {
          uVar23 = uVar23 + 0x10;
        }
        else {
          uVar23 = uVar23 + 0x50;
        }
        iVar20 = iVar20 + 1;
      }
      uVar16 = *(int *)(iVar10 + 0x10) * 0x10 + *(int *)(iVar10 + 0x20);
      if (*(int *)(iVar10 + 0x24) < (int)uVar16) {
        uVar16 = *(uint *)(iVar10 + 0x24);
      }
      *(int *)(iVar10 + 0x28) = *(int *)(iVar10 + 0x28) + uVar23;
      uVar17 = uVar16 - uVar23;
      uVar14 = uVar13;
      if (uVar16 < uVar23) {
        uVar17 = 0;
      }
      do {
        uVar23 = puVar15[2];
        if (uVar8 == puVar15[1]) {
          uVar16 = *puVar15;
          if (uVar17 < uVar23) {
            puVar24 = (uint *)(uVar16 + 0xec);
            *puVar24 = *puVar24 | 1;
            *(int *)(iVar10 + 0x2c) = *(int *)(iVar10 + 0x2c) + uVar23;
          }
          else {
            uVar17 = uVar17 - uVar23;
            *(int *)(iVar10 + 0x28) = *(int *)(iVar10 + 0x28) + uVar23;
            puVar24 = (uint *)(uVar16 + 0xec);
            *puVar24 = *puVar24 & 0xfffffffe;
          }
        }
        pIVar6 = g_InGameConditionRuntime;
        puVar15 = puVar15 + 4;
        uVar14 = uVar14 - 1;
      } while (uVar14 != 0);
      uVar23 = *(uint *)(iVar10 + 0x28);
      uVar16 = uVar23 - *(uint *)(iVar10 + 0x20);
      if (uVar23 < *(uint *)(iVar10 + 0x20)) {
        uVar16 = 0;
      }
      if (*(int *)(iVar10 + 0x2c) == 0) {
        *(undefined4 *)(iVar10 + 0x68) = 0;
      }
      else if ((g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex == uVar8) {
        if (*(uint *)(iVar10 + 0x24) < uVar23 + *(int *)(iVar10 + 0x2c)) {
          if (*(int *)(iVar10 + 0x68) == 0) {
            notificationMovieId = 400;
            *(undefined4 *)(iVar10 + 0x68) = 0x96;
InGameRuntime_UpdateFactionTerrainAndCapacityState_QueueLocalCapacityStatusNotification:
            if (((*(int *)(pIVar6[1].reserved00_4F + 0x24) == 0x300074) &&
                (*(int *)(pIVar6[1].reserved00_4F + 0x28) == 0x5f0030)) &&
               (*(int *)(pIVar6[1].reserved00_4F + 0x2c) == 0x750074)) {
              notificationMovieId = 0x192;
              *(undefined4 *)(iVar10 + 0x68) = 0x7fffffff;
              *(undefined4 *)(iVar10 + 0x6c) = 0x7fffffff;
            }
            InGameNotificationQueue_InsertPriorityRecord(NONE,0,0,0,0,0,3,notificationMovieId);
            uVar16 = extraout_EAX;
          }
        }
        else if (*(int *)(iVar10 + 0x6c) == 0) {
          notificationMovieId = 0x191;
          *(undefined4 *)(iVar10 + 0x6c) = 0x96;
          goto 
          InGameRuntime_UpdateFactionTerrainAndCapacityState_QueueLocalCapacityStatusNotification;
        }
      }
      *(int *)(iVar10 + 0x10) =
           *(int *)(iVar10 + 0x10) - (uVar16 >> 4) * g_InGameSimulationStepTicks;
      iVar10 = iVar10 + -0x740;
      uVar8 = uVar8 - 1;
    } while (uVar8 != 0);
  }
  if ((g_GameFactionRuntimeImage.tail.simulationTick & 0x78) == 0) {
    iVar10 = 0x50fa80;
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
    if (g_GameFactionRuntimeImage.tail.simulationTick >> 7 < 0x1000) {
      uVar13 = 1;
      piVar21 = (int *)((g_GameFactionRuntimeImage.tail.simulationTick >> 7) * 0x38 +
                       (int)g_GameStatTableImage);
      do {
        GameFactionRuntime_RecomputeProgressAndScoreMetrics(uVar13,worldRuntime);
        iVar11 = *(int *)(iVar10 + 0x88);
        iVar20 = *(int *)(iVar10 + 0x8c);
        if (iVar11 < 0) {
          iVar11 = 0;
        }
        if (iVar20 < 0) {
          iVar20 = 0;
        }
        *piVar21 = iVar11;
        piVar21[1] = iVar20;
        uVar13 = extraout_ECX_00 + 1;
        iVar10 = iVar10 + 0x740;
        piVar21 = piVar21 + 2;
      } while (uVar13 < 8);
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
void InGameRuntime_UpdateCursorGridAndViewScaleCache(void)

{
  UQ12 UVar1;
  dword dVar2;
  qword qVar3;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  
  gameRuntime1 = g_InGameRuntimeRoot;
  qVar3 = FieldGrid_WorldToGridQ12
                    ((g_InGameRuntimeRoot->worldRuntime0A30).motion.targetPositionYQ12,
                     (g_InGameRuntimeRoot->worldRuntime0A30).motion.targetPositionXQ12);
  gameRuntime1->fieldGridPosition9A6C = qVar3;
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
void InGameRuntime_PublishRootWorldStatePointer(UiRootNode *inGameRoot)

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
void __cdecl InGameRuntime_UpdateSimulationAndNetworkTick(void)

{
  void *pvVar1;
  uint uVar2;
  uint extraout_ECX;
  uint extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_EDX;
  WorldRuntimeContext *worldRuntime;
  WorldRuntimeNode *worldNode1;
  ModelRuntimeNode *modelNode1;
  undefined1 in_CF;
  undefined1 uVar3;
  bool bVar4;
  undefined8 uVar5;
  InGameRuntimeRootImageC3E4 *gameRuntime1;
  
  (*g_SpinLockTryAcquire)(&g_InGameStateTickSpinLock);
  gameRuntime1 = g_InGameRuntimeRoot;
  if ((bool)in_CF) {
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
      uVar3 = 0;
      if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
        while (bVar4 = false, g_HostCommandBatchSyncSentThisInterval == 0) {
          uVar5 = UiRuntimeRecordRing_DiscardOldestCf();
          if (bVar4) {
            FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(1);
            if (bVar4) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
            break;
          }
          FrontendTransfer_HandleSyncRequest10021AndReply10023
                    ((NetworkSessionContext *)((ulonglong)uVar5 >> 0x20),
                     (FrontendTransferPacketUnion *)uVar5);
        }
        FrontendTransfer_DispatchStagedCommandRecords();
        g_HostCommandBatchSyncSentThisInterval = 0;
      }
      else {
        while( true ) {
          uVar5 = UiRuntimeRecordRing_DiscardOldestCf();
          if ((bool)uVar3) break;
          FrontendTransfer_HandleSyncRequest10021AndReply10023
                    ((NetworkSessionContext *)((ulonglong)uVar5 >> 0x20),
                     (FrontendTransferPacketUnion *)uVar5);
        }
        bVar4 = false;
        if ((g_HostCommandBatchSyncSentThisInterval == 0) &&
           (FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(0), !bVar4)) {
          g_HostCommandBatchSyncSentThisInterval = 1;
        }
      }
    }
  }
  else {
    bVar4 = false;
    if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
      UiRuntimeRecordRing_ContainsIdCf(g_FrontendSessionToken);
      uVar3 = 1;
      if (!bVar4) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
      do {
        uVar5 = UiRuntimeRecordRing_DiscardOldestCf();
        if ((bool)uVar3) break;
        FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf
                  ((NetworkSessionContext *)((ulonglong)uVar5 >> 0x20),
                   (FrontendTransferPacketUnion *)uVar5);
      } while (!(bool)uVar3);
      FrontendTransfer_ConsumeProcessedFlagCf();
      if ((bool)uVar3) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
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
          worldNode1 != (WorldRuntimeNode *)0x0; worldNode1 = (worldNode1->common).nextNode) {
        (*(&g_RuntimeMaintenanceCallbackPhases.primaryUpdate.army)
          [(int)worldNode1[2].common.nextNode])(worldRuntime,worldNode1);
        uVar2 = extraout_ECX;
      }
      if (g_GameFactionRuntimeImage.tail.simulationTick % 0x14 == 0) {
        InGameConditionRuntime_UpdateScheduledRecords();
        uVar2 = extraout_ECX_00;
      }
                    
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
        if (worldNode1 != (WorldRuntimeNode *)0x0) {
          do {
            (*(&g_RuntimeMaintenanceCallbackPhases.occupancyRebuild.army)
              [(int)worldNode1[2].common.nextNode])(worldRuntime,worldNode1);
            worldNode1 = (worldNode1->common).nextNode;
          } while (worldNode1 != (WorldRuntimeNode *)0x0);
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
          pvVar1 = ((modelNode1->runtimePayload).armyRuntime)->definitionOrAsset;
          (*g_ArmyPlacementContactKindDispatchTable.callbacks[*(int *)((int)pvVar1 + 0x278)])
                    (*(Q12 *)((int)pvVar1 + 0x54),(modelNode1->worldTransform).translation.y,
                     (modelNode1->worldTransform).translation.x,modelNode1,
                     &gameRuntime1->worldRuntime0A30);
          ModelNodeRuntime_RebuildTransformsFromRoot(extraout_ECX_01,extraout_EDX,modelNode1);
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
void InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf(void)

{
  return;
}
