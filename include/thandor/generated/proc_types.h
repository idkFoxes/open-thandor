/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/generated/proc_types.h
 */

/* Function pointer types of the recovered data and callbacks (once generated from the Ghidra export together
   with the address macros of the original image; edited by hand since step 4c). */

#ifndef THANDOR_GENERATED_PROC_TYPES_H
#define THANDOR_GENERATED_PROC_TYPES_H

#include <thandor/generated/types.h>

/* Function-signature types Ghidra does not include in its C export, taken from
 * ghidra/export/function_definitions.jsonl (tools/ghidra/ExportBuildData.java). */
typedef AiTechnologyCandidateScore AiTechnologyCandidateScoreCallback(FactionRuntimeIndex factionIndex, PckTechnologyIdCatalog technologyId, WorldRuntimeContext * worldRuntime); /* Ghidra FunctionDefinition /Thandor/AI/Callbacks */
typedef uint32_t ArenaFreeProc(void * memory); /* Ghidra FunctionDefinition /Thandor/ABI */
typedef uint32_t ArenaShrinkProc(uint32_t newSize, void * memory); /* Ghidra FunctionDefinition /Thandor/ABI */
typedef uint8_t * CommandLineFindOptionProc(uint32_t length, char * option); /* Ghidra FunctionDefinition /Thandor/CommandLine/Methods */
typedef uint32_t __cdecl CpuDetectFeaturesProc(void); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef uint32_t FatalErrorPassThroughProc(uint32_t valueOrError, bool failed); /* Ghidra FunctionDefinition /Thandor/ABI */
typedef void FileSystemCloseProc(void * handle); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t FileSystemCopyProc(uint16_t * destinationPath, uint16_t * sourcePath); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef uint32_t FileSystemCreateDirectoryRecursiveProc(FileSystemCreateDirectoryFlags flags, uint16_t * path); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef uint32_t FileSystemDeleteProc(uint32_t unusedFlags, uint16_t * path); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef bool FileSystemDriveReadyProc(uint32_t driveLetter); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef uint32_t FileSystemEnumerateDirectoryOrVolumeEntriesProc(FileSystemEnumerationMode mode, uint32_t reserved, FileSystemOutputCapacityBytes outputCapacityBytes, uint8_t * outputRecords, uint8_t * pathOrVolumeText); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t FileSystemEnumerateDriveLettersProc(uint8_t * lettersOut); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef bool FileSystemGetCurrentDirectoryProc(uint16_t * destination); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef EngineDriveTypeCode FileSystemGetDriveTypeCodeProc(DosDriveLetterCode32 driveLetter); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef Win32DriveCapacity FileSystemGetFreeAndTotalBytesRegsProc(DosDriveLetterCode32 driveLetter); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef uint32_t FileSystemGetLastWriteDosDateProc(uint16_t * path, uint32_t * outDosDateTime); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef uint32_t FileSystemGetLastWriteTimeHighProc(uint16_t * path, uint32_t * outLastWriteTimeHigh); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef bool FileSystemGetPositionProc(void * handle, uint32_t * outPosition); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef bool FileSystemGetSizeProc(void * handle, uint32_t * outSize); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t FileSystemGetVolumeSerialNumberProc(uint8_t * outputLabel, char * path); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef uint32_t FileSystemMoveProc(uint16_t * destinationPath, uint16_t * sourcePath); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef uint32_t FileSystemOpenProc(FileSystemOpenFlags openFlags, uint16_t * path, void * * outHandle); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t FileSystemReadExactProc(FileIoByteCount byteCount, void * destination, void * handle); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t FileSystemRemoveDirectoryProc(uint16_t * path); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef uint32_t FileSystemSeekProc(FileSystemSeekOrigin moveMethod, FileSystemFilePosition distance, void * handle); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t FileSystemSetCurrentDirectoryProc(uint16_t * path); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef bool FileSystemValidateDos83Proc(FileSystemDos83ValidationFlags flags, uint8_t * pathAnsi); /* Ghidra FunctionDefinition /Thandor/System/FileSystem */
typedef uint32_t FileSystemWriteExactOrFlushProc(FileIoByteCount byteCount, void * source, void * handle); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t FrontendModelPointerResolvedActionCallbackProc(uint32_t surfaceHitDepth, uint32_t surfaceHitWorldY, uint32_t surfaceHitWorldX, int selectedHitMetric, ModelRuntimeNode * selectedModelNode, FrontendModelPointerHitContext * context); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef void GlideTextureUploadProc(GraphicsTextureResource * texture); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef void __stdcall GrAlphaBlendFunctionImportProc(uint32_t rgbSourceFactor, uint32_t rgbDestinationFactor, uint32_t alphaSourceFactor, uint32_t alphaDestinationFactor); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrAlphaCombineImportProc(uint32_t function, uint32_t factor, uint32_t local, uint32_t other, uint32_t invert); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrBufferClearImportProc(uint32_t color, uint32_t alpha, uint32_t depth); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrBufferSwapImportProc(uint32_t swapInterval); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrClipWindowImportProc(uint32_t minX, uint32_t minY, uint32_t maxX, uint32_t maxY); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrColorCombineImportProc(uint32_t function, uint32_t factor, uint32_t local, uint32_t other, uint32_t invert); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrCoordinateSpaceImportProc(uint32_t mode); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrCullModeImportProc(uint32_t mode); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrDepthBufferFunctionImportProc(uint32_t function); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrDepthBufferModeImportProc(uint32_t mode); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrDepthMaskImportProc(uint32_t enabled); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrDrawTriangleImportProc(uint32_t * vertexA, uint32_t * vertexB, uint32_t * vertexC); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrFinishImportProc(void); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef uint32_t __stdcall GrGetImportProc(uint32_t selector, uint32_t sizeBytes, void * output); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef char * __stdcall GrGetStringImportProc(uint32_t selector); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrGlideInitImportProc(void); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrGlideShutdownImportProc(void); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef uint32_t __stdcall GrLfbLockImportProc(uint32_t lockType, uint32_t buffer, uint32_t writeMode, uint32_t origin, uint32_t pixelPipeline, void * lfbInfo); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef uint32_t __stdcall GrLfbReadRegionImportProc(uint32_t buffer, GraphicsScreenCoordinate sourceX, GraphicsScreenCoordinate sourceY, GraphicsPixelDimension width, GraphicsPixelDimension height, uint32_t destinationStrideBytes, uint16_t * destinationPixels); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrLfbUnlockImportProc(uint32_t lockType, uint32_t buffer); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef uint32_t __stdcall GrQueryResolutionsImportProc(void * query, void * output); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrSstSelectImportProc(uint32_t boardIndex); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrSstWinCloseImportProc(uint32_t context); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef uint32_t __stdcall GrSstWinOpenImportProc(uint32_t windowHandle, uint32_t screenResolution, uint32_t refreshRate, uint32_t colorFormat, uint32_t origin, uint32_t colorBufferCount, uint32_t auxiliaryBufferCount); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrTexClampModeImportProc(uint32_t tmuIndex, uint32_t sClampMode, uint32_t tClampMode); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrTexCombineImportProc(uint32_t tmuIndex, uint32_t rgbFunction, uint32_t rgbFactor, uint32_t alphaFunction, uint32_t alphaFactor, uint32_t rgbInvert, uint32_t alphaInvert); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrTexDownloadMipMapImportProc(GraphicsTextureResidentTmuIndex tmuIndex, GraphicsTextureMemoryAddress startAddress, uint32_t evenOddMask, GrTexInfo * textureInfo); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrTexFilterModeImportProc(uint32_t tmuIndex, uint32_t minifyFilter, uint32_t magnifyFilter); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef GraphicsTextureMemoryAddress __stdcall GrTexMaxAddressImportProc(GraphicsTextureResidentTmuIndex tmuIndex); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef GraphicsTextureMemoryAddress __stdcall GrTexMinAddressImportProc(GraphicsTextureResidentTmuIndex tmuIndex); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrTexMipMapModeImportProc(uint32_t tmuIndex, uint32_t mode, uint32_t lodBlend); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrTexSourceImportProc(GraphicsTextureResidentTmuIndex tmuIndex, GraphicsTextureMemoryAddress residentAddress, uint32_t mode, GrTexInfo * textureInfo); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrVertexLayoutImportProc(uint32_t parameter, uint32_t byteOffset, uint32_t mode); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void __stdcall GrViewportImportProc(uint32_t x, uint32_t y, uint32_t width, uint32_t height); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void GraphicsBackendRefreshActiveAdapterProc(void); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef void GraphicsBeginSceneProc(void); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef bool GraphicsCursorConsumeEventProc(CursorPointerEvent *outEvent); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef void GraphicsDrawPrimitiveQueueProc(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, GraphicsPrimitiveQueue * queue); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef void GraphicsEndSceneProc(void); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef GraphicsTextureSourceAsset * GraphicsOffscreenRenderModelListToTextureSourceProc(GraphicsOffscreenSceneExtents * sceneExtents, AngleTurn32 * auxiliaryOrientationAngles, GraphicsOffscreenViewParameters * viewParameters, GraphicsPixelDimension outputHeight, GraphicsPixelDimension outputWidth, ModelRuntimeCount modelCount, ModelRuntimeNode * * modelNodes); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef GraphicsPaletteAsset * GraphicsPaletteAssetLoadPackageProc(uint16_t * pathUtf16, uint32_t * outErrorCode); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef GraphicsPaletteAsset * GraphicsPaletteAssetValidateProc(GraphicsPaletteAsset * paletteAsset, uint32_t * outErrorCode); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef void GraphicsPrimitiveQueueRadixSortProc(GraphicsBooleanState halveVertexRgb, GraphicsPrimitiveQueue * queue); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef void GraphicsSetViewportProc(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef void __cdecl GraphicsTextureRebuildAllProc(void); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef GraphicsTextureSet * GraphicsTextureSetCreateProc(GraphicsTextureSourceAsset * sourceAsset, uint32_t * outErrorCode); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef GraphicsTextureSourceAsset * GraphicsTextureSetDestroyProc(GraphicsTextureSet * set); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef GraphicsTextureSet * GraphicsTextureSetLoadPackageProc(uint16_t * pathUtf16, uint32_t * outErrorCode); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef void GraphicsTextureSetRefreshProc(uint32_t subresourceIndex, GraphicsTextureSet * set); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef void GraphicsTextureSetReleasePackageProc(GraphicsTextureSet * set); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef uint32_t GraphicsTextureSourceConvertPaletteEntriesProc(GraphicsPaletteTextureSourceAsset * sourceAsset); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef void __stdcall GuGammaCorrectionRGBImportProc(uint32_t redGamma, uint32_t greenGamma, uint32_t blueGamma); /* Ghidra FunctionDefinition /Thandor/Graphics/Glide/Imports */
typedef void InGameWorldOverlayPhaseCallbackProc(GraphicsBooleanState releaseMode, WorldRuntimeContext * worldRuntime); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef void InGameWorldOverlayRebuildCallbackProc(uint32_t arg0, WorldRuntimeContext * arg1); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef void InGameWorldTransientStateClearCallbackProc(WorldRuntimeContext * arg0); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef void KeyboardFlushEventsProc(void); /* Ghidra FunctionDefinition /Thandor/Input/Methods */
typedef bool KeyboardReadEventProc(uint32_t *outKeyCode, uint32_t *outStateMask); /* Ghidra FunctionDefinition /Thandor/Input/Methods */
typedef void LocaleCopyDefaultComputerLabelUtf16Proc(uint16_t * destination); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef uint32_t LocaleFormatCurrentDateUtf16Proc(uint16_t * destination); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef uint32_t LocaleFormatCurrentTimeUtf16Proc(uint16_t * destination); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef uint32_t LocaleFormatDateFieldsUtf16Proc(uint32_t year, uint32_t month, uint32_t day, uint16_t * destination); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef uint32_t LocaleFormatTimeFieldsUtf16Proc(uint32_t hour, uint32_t minute, uint16_t * destination); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef uint32_t LocaleGetPackedCurrentDateProc(void); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef uint32_t LocaleGetPackedCurrentTimeProc(void); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef uint32_t LocaleGetTelephoneCountryCodeProc(void); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef FrameProviderResult MovieFrameProviderProc(void * frameToReleaseOrNull); /* Ghidra FunctionDefinition /Thandor/Assets/Movie/Callbacks */
typedef void NetworkBackendCleanupCallback(void); /* Ghidra FunctionDefinition /Thandor/Network/Backend */
typedef void NetworkBackendCloseCallback(void); /* Ghidra FunctionDefinition /Thandor/Network/Backend */
typedef void NetworkBackendFormatAddressCallback(char * outputText, WinSockAddress * socketAddress); /* Ghidra FunctionDefinition /Thandor/Network/Backend */
typedef uint32_t NetworkBackendOpenBindCallback(uint32_t localPort); /* 0 or a FATAL_ERROR_NETWORK_* code; Ghidra FunctionDefinition /Thandor/Network/Backend */
typedef bool NetworkBackendParseEndpointCallback(UiTransferEndpointDescriptor * endpoint, char * endpointText); /* Ghidra FunctionDefinition /Thandor/Network/Backend */
typedef bool NetworkBackendReceiveCallback(WinSockAddress * sourceAddress, uint32_t byteCount, uint8_t * buffer); /* true when a datagram was received; Ghidra FunctionDefinition /Thandor/Network/Backend */
typedef bool NetworkBackendSendCallback(WinSockAddress * destinationAddress, uint32_t byteCount, uint8_t * buffer); /* true on success; Ghidra FunctionDefinition /Thandor/Network/Backend */
typedef uint32_t NetworkBackendSetSessionCallback(uint32_t backendIndex); /* 0 or a FATAL_ERROR_NETWORK_* code; Ghidra FunctionDefinition /Thandor/Network/Backend */
typedef bool PckCodecProc(uint32_t destinationCapacityOrOutputSize, uint8_t * destination, uint32_t sourceSize, uint8_t * source, uint32_t * outByteCount, uint32_t * outErrorCode); /* Ghidra FunctionDefinition /Thandor/Package/Methods */
typedef PcxDecodeResult PcxDecodeProc(FncModuleHeader * module, uint32_t sourceByteCount, void * sourceBytes); /* Ghidra FunctionDefinition /Thandor/UI/Pcx */
typedef PcxEncodeResult PcxEncodeProc(FncModuleHeader * module, void * framebufferCapture); /* Ghidra FunctionDefinition /Thandor/UI/Pcx */
typedef void PointerFlushEventsProc(void); /* Ghidra FunctionDefinition /Thandor/Input */
typedef void PointerSetPositionProc(int32_t positionY, int32_t positionX); /* Ghidra FunctionDefinition /Thandor/Input */
typedef void ScenarioCatalogRefreshSelectedRecordCallback(uint32_t arg0, uint32_t arg1, uint32_t arg2, UiListRowIndex selectionIndex); /* Ghidra FunctionDefinition /Thandor/UI/ActionHandlers/Callbacks */
typedef void SoftwareBuildPixelPackTablesProc(int32_t colorScaleQ16, int32_t colorBiasQ16); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef void SoftwareDrawQueueProc(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, GraphicsPrimitiveQueue * queue); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef SoftwareFramebufferAccess * SoftwareFramebufferCreateProc(uint32_t bytesPerPixel, uint32_t height, uint32_t width, uint32_t * outError); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef void SoftwareRasterHandler(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, GraphicsPrimitivePacket * packet); /* Ghidra FunctionDefinition /Thandor/Graphics/Methods */
typedef uint32_t SoundQueryVoiceRegsProc(IDirectSoundBuffer * voice); /* Ghidra FunctionDefinition /Thandor/Sound/Methods */
typedef void SpinLockAcquireProc(RuntimeSpinLockValue * lockValue); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef void SpinLockReleaseCallbackProc(void); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef void SpinLockReleaseAndInvokeProc(SpinLockReleaseCallbackProc * callback, RuntimeSpinLockValue * lockValue); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef void SpinLockReleaseProc(RuntimeSpinLockValue * lockValue); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef bool SpinLockTryAcquireFlagsProc(RuntimeSpinLockValue * lockValue); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef bool TerrainClassOverlayCallback(uint32_t cellFlagMask, int cellValue, uint32_t radiusWorldUnits, Q12 worldXQ12, Q12 worldYQ12, FieldGridAsset * fieldGrid); /* Ghidra FunctionDefinition /Thandor/Assets/FieldGrid/Callbacks */
typedef void __cdecl TimerCallbackProc(void); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef void TimerRegisterPeriodicProc(uint32_t frequencyHz, TimerCallbackProc * callback); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef void TimerUnregisterPeriodicProc(TimerCallbackProc * callback); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef bool UiRootCloseCallback(UiRootNode * arg0); /* Ghidra FunctionDefinition /Thandor/UI/Callbacks */
typedef void UiRootFrameCallback(UiRootNode * arg0); /* Ghidra FunctionDefinition /Thandor/UI/Callbacks */
typedef bool UiRootKeyboardFallback(UiKeyboardStateMask modifierFlags, UiActionId commandCode, UiRootNode * root); /* Ghidra FunctionDefinition /Thandor/UI/Callbacks */
typedef bool UiRootMethod08Callback(UiRootNode * root); /* Ghidra FunctionDefinition /Thandor/UI/Callbacks */
typedef int UiRootPointerMissPolicyCallback(UiRootNode * root); /* Ghidra FunctionDefinition /Thandor/UI/Callbacks */
typedef void UiRuntimePostUnlockCallbackProc(void); /* Ghidra FunctionDefinition /Thandor/UI/Runtime */
typedef int __stdcall WSAIoctl_Proc(uint32_t socket, uint32_t ioControlCode, void * inBuffer, uint32_t inBufferLength, void * outBuffer, uint32_t outBufferLength, uint32_t * bytesReturned, void * overlapped, void * completionRoutine); /* Ghidra FunctionDefinition /Thandor/Recovered/Network */
typedef int __stdcall WSAStringToAddressA_Proc(char * addressString, int addressFamily, void * protocolInfo, NetworkBackendSocketAddress16 * address, int * addressLength); /* Ghidra FunctionDefinition /Thandor/Recovered/Network */
typedef void __cdecl Win32PumpMessagesProc(void); /* Ghidra FunctionDefinition /Thandor/System/Methods */
typedef uint32_t __stdcall WinSock_WSAAsyncGetHostByAddrProc(uint32_t window, uint32_t message, uint8_t * address, int addressLength, int addressType, uint8_t * outputBuffer, int outputCapacity); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t __stdcall WinSock_WSAAsyncGetHostByNameProc(uint32_t window, uint32_t message, uint8_t * hostName, uint8_t * outputBuffer, int outputCapacity); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t __stdcall WinSock_WSAAsyncGetProtoByNameProc(uint32_t window, uint32_t message, uint8_t * protocolName, uint8_t * outputBuffer, int outputCapacity); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t __stdcall WinSock_WSAAsyncGetProtoByNumberProc(uint32_t window, uint32_t message, int protocolNumber, uint8_t * outputBuffer, int outputCapacity); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t __stdcall WinSock_WSAAsyncGetServByNameProc(uint32_t window, uint32_t message, uint8_t * serviceName, uint8_t * protocolName, uint8_t * outputBuffer, int outputCapacity); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t __stdcall WinSock_WSAAsyncGetServByPortProc(uint32_t window, uint32_t message, int portNetworkOrder, uint8_t * protocolName, uint8_t * outputBuffer, int outputCapacity); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_WSAAsyncSelectProc(uint32_t socket, uint32_t window, uint32_t message, int eventMask); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_WSACancelAsyncRequestProc(uint32_t asyncTaskHandle); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_WSACancelBlockingCallProc(void); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_WSACleanupProc(void); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_WSAGetLastErrorProc(void); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_WSAIsBlockingProc(void); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef pointer __stdcall WinSock_WSASetBlockingHookProc(pointer hookProcedure); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_WSAStartupProc(uint16_t requestedVersion, WinSockData11 * startupData); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_WSAUnhookBlockingHookProc(void); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t __stdcall WinSock_acceptProc(uint32_t socket, WinSockAddress * address, int * addressLength); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_bindProc(uint32_t socket, WinSockAddress * address, int addressLength); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_closesocketProc(uint32_t socket); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_connectProc(uint32_t socket, WinSockAddress * address, int addressLength); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef WinSockHostEnt32 * __stdcall WinSock_gethostbyaddrProc(uint8_t * address, int addressLength, int addressType); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef WinSockHostEnt32 * __stdcall WinSock_gethostbynameProc(uint8_t * hostName); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_gethostnameProc(uint8_t * outputName, int outputCapacity); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_getpeernameProc(uint32_t socket, WinSockAddress * address, int * addressLength); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef WinSockProtoEnt32 * __stdcall WinSock_getprotobynameProc(uint8_t * protocolName); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef WinSockProtoEnt32 * __stdcall WinSock_getprotobynumberProc(int protocolNumber); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef WinSockServEnt32 * __stdcall WinSock_getservbynameProc(uint8_t * serviceName, uint8_t * protocolName); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef WinSockServEnt32 * __stdcall WinSock_getservbyportProc(int portNetworkOrder, uint8_t * protocolName); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_getsocknameProc(uint32_t socket, WinSockAddress * address, int * addressLength); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_getsockoptProc(uint32_t socket, int level, int optionName, uint8_t * optionValue, int * optionLength); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t __stdcall WinSock_htonlProc(uint32_t hostLong); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint16_t __stdcall WinSock_htonsProc(uint16_t hostShort); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t __stdcall WinSock_inet_addrProc(uint8_t * addressText); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint8_t * __stdcall WinSock_inet_ntoaProc(uint32_t ipv4AddressNetworkOrder); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_ioctlsocketProc(uint32_t socket, uint32_t command, uint32_t * argument); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_listenProc(uint32_t socket, int backlog); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t __stdcall WinSock_ntohlProc(uint32_t networkLong); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint16_t __stdcall WinSock_ntohsProc(uint16_t networkShort); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_recvProc(uint32_t socket, uint8_t * buffer, int length, int flags); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_recvfromProc(uint32_t socket, uint8_t * buffer, int length, int flags, WinSockAddress * sourceAddress, int * sourceAddressLength); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_selectProc(int ignoredNfds, WinSockFdSet64 * readSet, WinSockFdSet64 * writeSet, WinSockFdSet64 * exceptSet, WinSockTimeVal32 * timeout); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_sendProc(uint32_t socket, uint8_t * buffer, int length, int flags); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_sendtoProc(uint32_t socket, uint8_t * buffer, int length, int flags, WinSockAddress * destinationAddress, int destinationAddressLength); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_setsockoptProc(uint32_t socket, int level, int optionName, uint8_t * optionValue, int optionLength); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef int __stdcall WinSock_shutdownProc(uint32_t socket, int how); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef uint32_t __stdcall WinSock_socketProc(int addressFamily, int socketType, int protocol); /* Ghidra FunctionDefinition /Thandor/Canonical/FunctionDefinitions */
typedef void WorldRuntimeNodeTraversalCallback(void * callbackContext, WorldOwnerListNode * node); /* Ghidra FunctionDefinition /Thandor/World/Callbacks */


#endif /* THANDOR_GENERATED_PROC_TYPES_H */
