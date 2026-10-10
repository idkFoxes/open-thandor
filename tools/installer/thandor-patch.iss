; Open Thandor patch installer ("Thandor Patch 6"): puts the Open Thandor build onto an existing Thandor
; installation, like the original Thandor-Patch-5.exe did (classic Inno Setup wizard, German only).
; Plan: docs/plans/step10_installer.md. Compiled with Inno Setup 7 (ISCC), normally through
; tools/installer/build_installer.py, which stages the files and passes the parameters below.
;
; Parameters (ISCC /D<name>=<value>), all optional:
;   AppVersion        numeric version, default 1.0.7 (VersionInfoVersion gets ".0" appended)
;   PatchName         AppName and wizard name, default "Thandor Patch 6"
;   Stage             folder with thandor.exe, SDL3.dll and optionally thandor.sym and LICENSE-SDL3.txt,
;                     default <repo>\build-mingw-release\installer\stage
;   OutDir            output folder (ISCC /O wins), default <repo>\build-mingw-release\installer
;   WizardImage       large wizard image(s), e.g. "a-164.bmp,a-328.bmp"; default: every
;                     tools\installer\images\wizard-image-*.bmp, else Inno's own placeholder
;   WizardSmallImage  small wizard image(s); default: every tools\installer\images\wizard-small-image-*.bmp,
;                     else Inno's own placeholder
;   SetupIcon         .ico of Setup and the uninstaller; default src\platform\bootstrap\thandor.ico if present
;   TestLowPriv       (defined = on) TEST ONLY: install without admin rights (PrivilegesRequired=lowest), so the
;                     silent install/uninstall tests can run on a copy of the game folder without UAC
;
; What it does (plan section 3):
;   - installs thandor.exe, SDL3.dll, thandor.sym and OpenThandor-liesmich.txt into the game folder;
;   - before that, copies the game's own thandor.exe once to {app}\thandor-1.05.exe (never
;     overwritten by a reinstall); the uninstaller puts it back (read-only attribute kept), so the folder is on
;     1.05 again;
;   - grants the Users group modify rights on the game folder and save\ (a 64-bit process gets no UAC file
;     virtualization; the game writes thandor.ini, logs and saves next to itself); uninstall leaves the ACL;
;   - refuses folders without DATEN.PCK and thandor.exe, and a running Thandor;
;   - never touches *.PCK, thandor.dat, thandor.ini, save\, flm\, the logs or the registry keys of the game.

#ifndef AppVersion
  #define AppVersion "1.0.7"
#endif
#ifndef PatchName
  #define PatchName "Thandor Patch 6"
#endif
#define RepoDir AddBackslash(SourcePath) + "..\.."
#ifndef Stage
  #define Stage RepoDir + "\build-mingw-release\installer\stage"
#endif
#ifndef OutDir
  #define OutDir RepoDir + "\build-mingw-release\installer"
#endif
#ifndef SetupIcon
  #if FileExists(RepoDir + "\src\platform\bootstrap\thandor.ico")
    #define SetupIcon RepoDir + "\src\platform\bootstrap\thandor.ico"
  #endif
#endif
; Wizard images: without WizardImage / WizardSmallImage the finished BMPs in tools\installer\images are used
; (wizard-image-<W>x<H>.bmp, wizard-small-image-<N>.bmp, made by make_wizard_images.py, see README.md); when that
; folder holds none, Inno's own placeholders stay.
#define ImageDir RepoDir + "\tools\installer\images"
#define FindHandle
#define FindResult
#define FoundImages ""
#sub AddFoundImage
  #define public FoundImages FoundImages + (FoundImages == "" ? "" : ",") + ImageDir + "\" + FindGetFileName(FindHandle)
#endsub
#ifndef WizardImage
  #define FoundImages ""
  #for {FindHandle = FindResult = FindFirst(ImageDir + "\wizard-image-*.bmp", 0); FindResult; FindResult = FindNext(FindHandle)} AddFoundImage
  #if FindHandle
    #expr FindClose(FindHandle)
  #endif
  #if FoundImages != ""
    #define WizardImage FoundImages
  #endif
#endif
#ifndef WizardSmallImage
  #define FoundImages ""
  #for {FindHandle = FindResult = FindFirst(ImageDir + "\wizard-small-image-*.bmp", 0); FindResult; FindResult = FindNext(FindHandle)} AddFoundImage
  #if FindHandle
    #expr FindClose(FindHandle)
  #endif
  #if FoundImages != ""
    #define WizardSmallImage FoundImages
  #endif
#endif
#if !FileExists(Stage + "\thandor.exe") || !FileExists(Stage + "\SDL3.dll")
  #error Stage folder without thandor.exe and SDL3.dll (run tools/installer/build_installer.py or pass /DStage=...)
#endif

[Setup]
; a new id (not the "Thandor Patch 5" of the original): uninstall key {EAF5A447-...}_is1
AppId={{EAF5A447-044F-40C7-A60B-D9247E851438}
AppName={#PatchName}
AppVersion={#AppVersion}
AppVerName=Version {#AppVersion}
AppPublisher=Open Thandor
AppPublisherURL=https://github.com/idkFoxes/open-thandor
AppSupportURL=https://github.com/idkFoxes/open-thandor
AppUpdatesURL=https://github.com/idkFoxes/open-thandor
VersionInfoVersion={#AppVersion}.0
VersionInfoProductName=Thandor
VersionInfoProductVersion={#AppVersion}.0
VersionInfoDescription={#PatchName} Setup
VersionInfoCompany=Open Thandor
; the folder is the existing game folder: found by FindThandorDir, confirmed on the directory page
DefaultDirName={code:FindThandorDir}
AppendDefaultDirName=no
DirExistsWarning=no
UsePreviousAppDir=yes
DisableWelcomePage=no
DisableDirPage=no
DisableProgramGroupPage=yes
DisableReadyPage=no
DisableFinishedPage=no
; the uninstaller lives in {app}\OpenThandor (Patch 5 used {app}\unins000.exe); the backup of the original exe is
; {app}\thandor-1.05.exe, outside that folder, so that Inno creates OpenThandor itself and removes it on uninstall
UninstallFilesDir={app}\OpenThandor
UninstallDisplayName={#PatchName} (Version {#AppVersion})
UninstallDisplayIcon={app}\thandor.exe
; 64-bit Setup for the 64-bit game, Windows 10 or newer
SetupArchitecture=x64
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
#ifdef TestLowPriv
PrivilegesRequired=lowest
#else
PrivilegesRequired=admin
#endif
; the running-game check is ours (PrepareToInstall); no Restart Manager prompts
CloseApplications=no
RestartApplications=no
WizardStyle=classic
#ifdef WizardImage
WizardImageFile={#WizardImage}
#endif
#ifdef WizardSmallImage
WizardSmallImageFile={#WizardSmallImage}
#endif
#ifdef SetupIcon
SetupIconFile={#SetupIcon}
#endif
ShowLanguageDialog=no
SetupLogging=yes
OutputDir={#OutDir}
OutputBaseFilename=Thandor-Patch-6
Compression=lzma2/max
SolidCompression=yes

[Languages]
Name: "german"; MessagesFile: "compiler:Languages\German.isl"

[Messages]
german.SelectDirLabel3=Wählen Sie den Ordner, in dem Thandor installiert ist (er enthält DATEN.PCK und thandor.exe). Das Setup wird [name] in diesen Ordner installieren.
german.FinishedLabelNoIcons=Das Setup hat die Installation von [name] auf Ihrem Computer abgeschlossen. Thandor wird wie bisher über thandor.exe im Spielordner gestartet.

[CustomMessages]
german.NoThandorDir=Im Ordner%n%n%1%n%nwurde keine Thandor-Installation gefunden (DATEN.PCK oder thandor.exe fehlt).%n%nBitte wählen Sie den Ordner, in dem Thandor installiert ist.
german.UnknownExe=Im Ordner%n%n%1%n%nwurde nicht Version 1.05 (Patch 5) von thandor.exe gefunden.%n%nDie vorhandene thandor.exe wird gesichert und bei der Deinstallation wiederhergestellt. Trotzdem fortfahren?
german.GameRunning=Thandor läuft noch (%1).%n%nBitte beenden Sie das Spiel und versuchen Sie es erneut.
german.BackupFailed=Die vorhandene thandor.exe konnte nicht nach%n%n%1%n%ngesichert werden. Die Installation wird abgebrochen.
german.RestoreFailed=Die gesicherte thandor.exe konnte nicht wiederhergestellt werden. Sie liegt noch unter%n%n%1

[Dirs]
; users-modify: a 64-bit process gets no UAC file virtualization, and the game writes thandor.ini, its logs and
; save\*.sve next to itself; uninstall leaves the folders and their ACL
Name: "{app}"; Permissions: users-modify; Flags: uninsneveruninstall
Name: "{app}\save"; Permissions: users-modify; Flags: uninsneveruninstall

[Files]
; thandor.exe: removed by our uninstall code, which puts the backed-up original back in its place
Source: "{#Stage}\thandor.exe"; DestDir: "{app}"; Flags: ignoreversion overwritereadonly uninsneveruninstall
Source: "{#Stage}\SDL3.dll"; DestDir: "{app}"; Flags: ignoreversion overwritereadonly
Source: "{#Stage}\thandor.sym"; DestDir: "{app}"; Flags: ignoreversion overwritereadonly skipifsourcedoesntexist
Source: "{#Stage}\LICENSE-SDL3.txt"; DestDir: "{app}\OpenThandor"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#SourcePath}\OpenThandor-liesmich.txt"; DestDir: "{app}"; Flags: ignoreversion overwritereadonly isreadme

[Code]
const
  { SHA-256 of the thandor.exe of Patch 5 / version 1.05 (MD5 bd565d4c8ac0207f187a553900bf8873, 1,642,496 bytes) }
  PATCH5_EXE_SHA256 = '53d097963e0b99c92b3b635255373d83f8b2ae5f65451ec76a11ff41588bd969';

function SetFileAttributes(lpFileName: string; dwFileAttributes: Cardinal): BOOL;
  external 'SetFileAttributesW@kernel32.dll stdcall';

var
  BackupMadeNow: Boolean;
  InstallSucceeded: Boolean;

function BackupFileOf(const Dir: string): string;
begin
  Result := AddBackslash(Dir) + 'thandor-1.05.exe';
end;

function IsThandorDir(const Dir: string): Boolean;
begin
  { thandor.exe may be missing only when our backup is there (an earlier uninstall stopped half-way) }
  Result := FileExists(AddBackslash(Dir) + 'DATEN.PCK') and
            (FileExists(AddBackslash(Dir) + 'thandor.exe') or FileExists(BackupFileOf(Dir)));
end;

{ Default folder: the original setup's Planet4\Pfad (32-bit view), then Software\Thandor\GamePath (the key Patch 5
  read), then Thandor in the 32-bit Program Files; each only when it holds DATEN.PCK. }
function FindThandorDir(Param: string): string;
var
  Dir: string;
begin
  if RegQueryStringValue(HKLM32, 'Software\Planet4\Thandor', 'Pfad', Dir) and IsThandorDir(Dir) then begin
    Result := RemoveBackslashUnlessRoot(Dir);
    exit;
  end;
  if RegQueryStringValue(HKLM32, 'Software\Thandor', 'GamePath', Dir) and IsThandorDir(Dir) then begin
    Result := RemoveBackslashUnlessRoot(Dir);
    exit;
  end;
  if RegQueryStringValue(HKLM64, 'Software\Thandor', 'GamePath', Dir) and IsThandorDir(Dir) then begin
    Result := RemoveBackslashUnlessRoot(Dir);
    exit;
  end;
  Result := ExpandConstant('{commonpf32}\Thandor');
end;

{ Ours: an Open Thandor build has a version resource 1.0.6 <= version < 2 (the original has none), or is an older
  Open Thandor build copied by hand (thandor.sym next to it). }
function IsOpenThandorExe(const Exe: string): Boolean;
var
  VersionMS, VersionLS: Cardinal;
begin
  Result := False;
  if GetVersionNumbers(Exe, VersionMS, VersionLS) then
    Result := ((VersionMS = $00010000) and (VersionLS >= $00060000)) or
              ((VersionMS > $00010000) and (VersionMS < $00020000));
  if not Result then
    Result := FileExists(AddBackslash(ExtractFileDir(Exe)) + 'thandor.sym');
end;

function IsPatch5Exe(const Exe: string): Boolean;
begin
  Result := False;
  try
    Result := CompareText(GetSHA256OfFile(Exe), PATCH5_EXE_SHA256) = 0;
  except
    Log('cannot hash ' + Exe + ': ' + GetExceptionMessage);
  end;
end;

{ A thandor.exe started from Dir (WMI; processes whose path is hidden from us do not count). Returns its path. }
function RunningThandor(const Dir: string): string;
var
  Locator, Service, Processes, Process, PathValue: Variant;
  I: Integer;
  Wanted, Path: string;
begin
  Result := '';
  Wanted := AddBackslash(Dir) + 'thandor.exe';
  try
    Locator := CreateOleObject('WbemScripting.SWbemLocator');
    Service := Locator.ConnectServer('.', 'root\CIMV2');
    Processes := Service.ExecQuery('SELECT ProcessId, ExecutablePath FROM Win32_Process WHERE Name = ''thandor.exe''');
    for I := 0 to Processes.Count - 1 do begin
      Process := Processes.ItemIndex(I);
      PathValue := Process.ExecutablePath;
      if VarIsNull(PathValue) or VarIsEmpty(PathValue) then
        continue;
      Path := PathValue;
      if CompareText(Path, Wanted) = 0 then begin
        Result := Path;
        exit;
      end;
    end;
  except
    Log('running-game check failed: ' + GetExceptionMessage);
  end;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
var
  Dir, Exe: string;
begin
  Result := True;
  if CurPageID <> wpSelectDir then
    exit;
  Dir := RemoveBackslashUnlessRoot(WizardDirValue);
  if not IsThandorDir(Dir) then begin
    Log('refused folder without DATEN.PCK and thandor.exe: ' + Dir);
    SuppressibleMsgBox(FmtMessage(CustomMessage('NoThandorDir'), [Dir]), mbError, MB_OK, IDOK);
    Result := False;
    exit;
  end;
  Exe := AddBackslash(Dir) + 'thandor.exe';
  if FileExists(Exe) and not FileExists(BackupFileOf(Dir)) and not IsPatch5Exe(Exe) and
     not IsOpenThandorExe(Exe) then begin
    Log('thandor.exe is not the one of version 1.05: ' + Exe);
    if SuppressibleMsgBox(FmtMessage(CustomMessage('UnknownExe'), [Dir]), mbConfirmation, MB_YESNO,
                          IDYES) <> IDYES then
      Result := False;
  end;
end;

{ After the Ready page, before any file is copied: refuse a running game, then back up the game's own exe once. }
function PrepareToInstall(var NeedsRestart: Boolean): String;
var
  Dir, Exe, Backup, Running: string;
begin
  Result := '';
  Dir := ExpandConstant('{app}');
  if not IsThandorDir(Dir) then begin
    Result := FmtMessage(CustomMessage('NoThandorDir'), [Dir]);
    exit;
  end;
  Running := RunningThandor(Dir);
  if Running <> '' then begin
    Result := FmtMessage(CustomMessage('GameRunning'), [Running]);
    exit;
  end;
  Exe := AddBackslash(Dir) + 'thandor.exe';
  Backup := BackupFileOf(Dir);
  if FileExists(Backup) then begin
    Log('backup kept (made by an earlier install): ' + Backup);
    exit;
  end;
  if not FileExists(Exe) then
    exit;
  if IsOpenThandorExe(Exe) then begin
    Log('thandor.exe is already an Open Thandor build, no backup: ' + Exe);
    exit;
  end;
  { CopyFile keeps the attributes, so the backup stays read-only like the original }
  if not CopyFile(Exe, Backup, True) then begin
    Result := FmtMessage(CustomMessage('BackupFailed'), [Backup]);
    exit;
  end;
  BackupMadeNow := True;
  Log('backed up ' + Exe + ' to ' + Backup);
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
    InstallSucceeded := True;
end;

{ An install that did not finish: drop a backup made by this run, unless thandor.exe was replaced already
  (Inno does not restore overwritten files on rollback, then the backup is the only original). }
procedure DeinitializeSetup();
var
  Dir, Exe, Backup: string;
begin
  if not BackupMadeNow or InstallSucceeded then
    exit;
  Dir := ExpandConstant('{app}');
  Exe := AddBackslash(Dir) + 'thandor.exe';
  Backup := BackupFileOf(Dir);
  if FileExists(Exe) and (GetSHA256OfFile(Exe) = GetSHA256OfFile(Backup)) then begin
    SetFileAttributes(Backup, FILE_ATTRIBUTE_NORMAL);
    DeleteFile(Backup);
  end;
end;

function InitializeUninstall(): Boolean;
var
  Running: string;
begin
  Result := True;
  Running := RunningThandor(ExpandConstant('{app}'));
  if Running <> '' then begin
    SuppressibleMsgBox(FmtMessage(CustomMessage('GameRunning'), [Running]), mbError, MB_OK, IDOK);
    Result := False;
  end;
end;

{ Uninstall: our thandor.exe out, the backed-up original back in (with its read-only attribute), before Inno
  removes the other files; without a backup our exe stays, so the folder keeps a game exe. }
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  Dir, Exe, Backup: string;
begin
  if CurUninstallStep <> usUninstall then
    exit;
  Dir := ExpandConstant('{app}');
  Exe := AddBackslash(Dir) + 'thandor.exe';
  Backup := BackupFileOf(Dir);
  if not FileExists(Backup) then begin
    Log('no backup of the original thandor.exe, Open Thandor''s stays: ' + Exe);
    exit;
  end;
  if FileExists(Exe) then begin
    SetFileAttributes(Exe, FILE_ATTRIBUTE_NORMAL);
    if not DeleteFile(Exe) then begin
      Log('cannot delete ' + Exe);
      SuppressibleMsgBox(FmtMessage(CustomMessage('RestoreFailed'), [Backup]), mbError, MB_OK, IDOK);
      exit;
    end;
  end;
  if RenameFile(Backup, Exe) then begin
    Log('restored ' + Exe + ' from ' + Backup);
  end else begin
    Log('cannot move ' + Backup + ' to ' + Exe);
    SuppressibleMsgBox(FmtMessage(CustomMessage('RestoreFailed'), [Backup]), mbError, MB_OK, IDOK);
  end;
end;
