/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/generated/proc_types.h
 */

/* Function pointer types of the data tables and callbacks (once generated together with the address macros of
   the original image; edited by hand since step 4c). */

#ifndef THANDOR_GENERATED_PROC_TYPES_H
#define THANDOR_GENERATED_PROC_TYPES_H

#include <thandor/generated/types.h>

/* Function-signature types used for function pointers, callbacks and method tables. */
typedef AiTechnologyCandidateScore AiTechnologyCandidateScoreCallback(FactionRuntimeIndex factionIndex, PckTechnologyIdCatalog technologyId, WorldRuntimeContext * worldRuntime);
typedef uint8_t * CommandLineFindOptionProc(uint32_t length, char * option);
typedef uint32_t __cdecl CpuDetectFeaturesProc(void);
typedef uintptr_t FatalErrorPassThroughProc(uintptr_t valueOrError, Bool8 failed); /* value or pointer (5f) */
typedef void FileSystemCloseProc(void * handle);
typedef uint32_t FileSystemCopyProc(uint16_t * destinationPath, uint16_t * sourcePath);
typedef uint32_t FileSystemCreateDirectoryRecursiveProc(FileSystemCreateDirectoryFlags flags, uint16_t * path);
typedef uint32_t FileSystemDeleteProc(uint32_t unusedFlags, uint16_t * path);
typedef Bool8 FileSystemDriveReadyProc(uint32_t driveLetter);
typedef uint32_t FileSystemEnumerateDirectoryOrVolumeEntriesProc(FileSystemEnumerationMode mode, uint32_t reserved, FileSystemOutputCapacityBytes outputCapacityBytes, uint8_t * outputRecords, uint8_t * pathOrVolumeText);
typedef uint32_t FileSystemEnumerateDriveLettersProc(uint8_t * lettersOut);
typedef Bool8 FileSystemGetCurrentDirectoryProc(uint16_t * destination);
typedef EngineDriveTypeCode FileSystemGetDriveTypeCodeProc(DosDriveLetterCode32 driveLetter);
typedef Win32DriveCapacity FileSystemGetFreeAndTotalBytesRegsProc(DosDriveLetterCode32 driveLetter);
typedef uint32_t FileSystemGetLastWriteDosDateProc(uint16_t * path, uint32_t * outDosDateTime);
typedef uint32_t FileSystemGetLastWriteTimeHighProc(uint16_t * path, uint32_t * outLastWriteTimeHigh);
typedef Bool8 FileSystemGetPositionProc(void * handle, uint32_t * outPosition);
typedef Bool8 FileSystemGetSizeProc(void * handle, uint32_t * outSize);
typedef uint32_t FileSystemGetVolumeSerialNumberProc(uint8_t * outputLabel, char * path);
typedef uint32_t FileSystemMoveProc(uint16_t * destinationPath, uint16_t * sourcePath);
typedef uint32_t FileSystemOpenProc(FileSystemOpenFlags openFlags, uint16_t * path, void * * outHandle);
typedef uint32_t FileSystemReadExactProc(FileIoByteCount byteCount, void * destination, void * handle);
typedef uint32_t FileSystemRemoveDirectoryProc(uint16_t * path);
typedef uint32_t FileSystemSeekProc(FileSystemSeekOrigin moveMethod, FileSystemFilePosition distance, void * handle);
typedef uint32_t FileSystemSetCurrentDirectoryProc(uint16_t * path);
typedef Bool8 FileSystemValidateDos83Proc(FileSystemDos83ValidationFlags flags, uint8_t * pathAnsi);
typedef uint32_t FileSystemWriteExactOrFlushProc(FileIoByteCount byteCount, void * source, void * handle);
typedef void GraphicsBeginSceneProc(void);
typedef Bool8 GraphicsCursorConsumeEventProc(CursorPointerEvent *outEvent);
typedef void GraphicsDrawPrimitiveQueueProc(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, GraphicsPrimitiveQueue * queue);
typedef void GraphicsEndSceneProc(void);
typedef GraphicsTextureSourceAsset * GraphicsOffscreenRenderModelListToTextureSourceProc(GraphicsOffscreenSceneExtents * sceneExtents, AngleTurn32 * auxiliaryOrientationAngles, GraphicsOffscreenViewParameters * viewParameters, GraphicsPixelDimension outputHeight, GraphicsPixelDimension outputWidth, ModelRuntimeCount modelCount, ModelRuntimeNode * * modelNodes);
typedef GraphicsPaletteAsset * GraphicsPaletteAssetLoadPackageProc(uint16_t * pathUtf16, uint32_t * outErrorCode);
typedef GraphicsPaletteAsset * GraphicsPaletteAssetValidateProc(GraphicsPaletteAsset * paletteAsset, uint32_t * outErrorCode);
typedef void GraphicsPrimitiveQueueRadixSortProc(GraphicsBooleanState halveVertexRgb, GraphicsPrimitiveQueue * queue);
typedef void GraphicsSetViewportProc(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX);
typedef void __cdecl GraphicsTextureRebuildAllProc(void);
typedef GraphicsTextureSet * GraphicsTextureSetCreateProc(GraphicsTextureSourceAsset * sourceAsset, uint32_t * outErrorCode);
typedef GraphicsTextureSourceAsset * GraphicsTextureSetDestroyProc(GraphicsTextureSet * set);
typedef GraphicsTextureSet * GraphicsTextureSetLoadPackageProc(uint16_t * pathUtf16, uint32_t * outErrorCode);
typedef void GraphicsTextureSetRefreshProc(uint32_t subresourceIndex, GraphicsTextureSet * set);
typedef void GraphicsTextureSetReleasePackageProc(GraphicsTextureSet * set);
typedef uint32_t GraphicsTextureSourceConvertPaletteEntriesProc(GraphicsPaletteTextureSourceAsset * sourceAsset);
typedef void InGameWorldTransientStateClearCallbackProc(WorldRuntimeContext * arg0);
typedef void KeyboardFlushEventsProc(void);
typedef Bool8 KeyboardReadEventProc(uint32_t *outKeyCode, uint32_t *outStateMask);
typedef void LocaleCopyDefaultComputerLabelUtf16Proc(uint16_t * destination);
typedef uint32_t LocaleFormatCurrentDateUtf16Proc(uint16_t * destination);
typedef uint32_t LocaleFormatCurrentTimeUtf16Proc(uint16_t * destination);
typedef uint32_t LocaleFormatDateFieldsUtf16Proc(uint32_t year, uint32_t month, uint32_t day, uint16_t * destination);
typedef uint32_t LocaleFormatTimeFieldsUtf16Proc(uint32_t hour, uint32_t minute, uint16_t * destination);
typedef uint32_t LocaleGetPackedCurrentDateProc(void);
typedef uint32_t LocaleGetPackedCurrentTimeProc(void);
typedef uint32_t LocaleGetTelephoneCountryCodeProc(void);
typedef FrameProviderResult MovieFrameProviderProc(void * frameToReleaseOrNull);
typedef void NetworkBackendCleanupCallback(void);
typedef void NetworkBackendCloseCallback(void);
typedef void NetworkBackendFormatAddressCallback(char * outputText, WinSockAddress * socketAddress);
typedef uint32_t NetworkBackendOpenBindCallback(uint32_t localPort); /* 0 or a FATAL_ERROR_NETWORK_* code */
typedef Bool8 NetworkBackendParseEndpointCallback(UiTransferEndpointDescriptor * endpoint, char * endpointText);
typedef Bool8 NetworkBackendReceiveCallback(WinSockAddress * sourceAddress, uint32_t byteCount, uint8_t * buffer); /* true when a datagram was received */
typedef Bool8 NetworkBackendSendCallback(WinSockAddress * destinationAddress, uint32_t byteCount, uint8_t * buffer); /* true on success */
typedef uint32_t NetworkBackendSetSessionCallback(uint32_t backendIndex); /* 0 or a FATAL_ERROR_NETWORK_* code */
typedef Bool8 PckCodecProc(uint32_t destinationCapacityOrOutputSize, uint8_t * destination, uint32_t sourceSize, uint8_t * source, uint32_t * outByteCount, uint32_t * outErrorCode);
typedef void PointerFlushEventsProc(void);
typedef void PointerSetPositionProc(int32_t positionY, int32_t positionX);
typedef void ScenarioCatalogRefreshSelectedRecordCallback(uint32_t arg0, uint32_t arg1, uint32_t arg2, UiListRowIndex selectionIndex);
typedef void SoftwareBuildPixelPackTablesProc(int32_t colorScaleQ16, int32_t colorBiasQ16);
typedef void SoftwareDrawQueueProc(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, GraphicsPrimitiveQueue * queue);
typedef SoftwareFramebufferAccess * SoftwareFramebufferCreateProc(uint32_t bytesPerPixel, uint32_t height, uint32_t width, uint32_t * outError);
typedef void SoftwareRasterHandler(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, GraphicsPrimitivePacket * packet);
typedef void SpinLockAcquireProc(RuntimeSpinLockValue * lockValue);
typedef void SpinLockReleaseCallbackProc(void);
typedef void SpinLockReleaseAndInvokeProc(SpinLockReleaseCallbackProc * callback, RuntimeSpinLockValue * lockValue);
typedef void SpinLockReleaseProc(RuntimeSpinLockValue * lockValue);
typedef Bool8 SpinLockTryAcquireFlagsProc(RuntimeSpinLockValue * lockValue);
typedef Bool8 TerrainClassOverlayCallback(uint32_t cellFlagMask, int cellValue, uint32_t radiusWorldUnits, Q12 worldXQ12, Q12 worldYQ12, FieldGridAsset * fieldGrid);
typedef void __cdecl TimerCallbackProc(void);
typedef void TimerRegisterPeriodicProc(uint32_t frequencyHz, TimerCallbackProc * callback);
typedef void TimerUnregisterPeriodicProc(TimerCallbackProc * callback);
typedef int UiRootPointerMissPolicyCallback(UiRootNode * root);
typedef void UiRuntimePostUnlockCallbackProc(void);
typedef void __cdecl Win32PumpMessagesProc(void);
typedef int __stdcall WinSock_WSACleanupProc(void);
typedef int __stdcall WinSock_WSAGetLastErrorProc(void);
typedef int __stdcall WinSock_WSAStartupProc(uint16_t requestedVersion, WinSockData11 * startupData);
typedef int __stdcall WinSock_bindProc(uint32_t socket, WinSockAddress * address, int addressLength);
typedef int __stdcall WinSock_closesocketProc(uint32_t socket);
typedef WinSockHostEnt32 * __stdcall WinSock_gethostbynameProc(uint8_t * hostName);
typedef uint16_t __stdcall WinSock_htonsProc(uint16_t hostShort);
typedef uint32_t __stdcall WinSock_inet_addrProc(uint8_t * addressText);
typedef uint8_t * __stdcall WinSock_inet_ntoaProc(uint32_t ipv4AddressNetworkOrder);
typedef int __stdcall WinSock_ioctlsocketProc(uint32_t socket, uint32_t command, uint32_t * argument);
typedef int __stdcall WinSock_recvfromProc(uint32_t socket, uint8_t * buffer, int length, int flags, WinSockAddress * sourceAddress, int * sourceAddressLength);
typedef int __stdcall WinSock_sendtoProc(uint32_t socket, uint8_t * buffer, int length, int flags, WinSockAddress * destinationAddress, int destinationAddressLength);
typedef int __stdcall WinSock_setsockoptProc(uint32_t socket, int level, int optionName, uint8_t * optionValue, int optionLength);
typedef uint32_t __stdcall WinSock_socketProc(int addressFamily, int socketType, int protocol);
typedef void WorldRuntimeNodeTraversalCallback(void * callbackContext, WorldOwnerListNode * node);


#endif /* THANDOR_GENERATED_PROC_TYPES_H */
