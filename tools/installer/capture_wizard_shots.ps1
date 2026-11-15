<#
.SYNOPSIS
  README screenshots of the patch installer (docs/images/installer-welcome.png, -folder.png, -ready.png).

.DESCRIPTION
  Runs a TEST Setup (built with `build_installer.py BUILD_DIR -D TestLowPriv --out <scratch>`, no admin rights)
  against a throwaway folder that this script creates (<OutDir>\target\Thandor with dummy DATEN.PCK, thandor.exe
  and thandor.sym; thandor.sym marks the dummy as an Open Thandor build, so no "unknown thandor.exe" prompt), passed
  with /DIR (or as <Drive>:\Thandor through a temporary subst with -Drive, so the pictures show a short path
  instead of the temp path with the user name). It captures only the wizard window (PrintWindow on its handle; the wizard belongs to the Setup.tmp
  child of the started process), steps Welcome -> Folder -> Ready with the buttons and then CANCELS (and confirms
  the cancel). It never clicks "Installieren". On the Folder page it reads the path from the edit control and
  cancels at once when it is not the throwaway folder.

  Output: <OutDir>\raw_<page>.png (full size) and <OutDir>\installer-<page>.png scaled to Width x Height (the size of
  the committed README pictures, 420x323). Copy the scaled ones to docs/images by hand after looking at them.

.EXAMPLE
  python tools\installer\build_installer.py build-mingw-release -D TestLowPriv --out %TEMP%\otshots\setup
  powershell -ExecutionPolicy Bypass -File tools\installer\capture_wizard_shots.ps1 -Setup %TEMP%\otshots\setup\Thandor-Patch-8.exe -OutDir %TEMP%\otshots -Drive T
#>
param(
    [Parameter(Mandatory = $true)][string]$Setup,
    [Parameter(Mandatory = $true)][string]$OutDir,
    [int]$Width = 420,
    [int]$Height = 323,
    [string]$Drive = ''
)
$ErrorActionPreference = 'Stop'

$Setup = (Resolve-Path $Setup).Path
if ((Split-Path $Setup -Leaf) -match 'Patch-5') { throw 'refusing to run the original Patch 5 setup' }
New-Item -ItemType Directory -Force $OutDir | Out-Null
$OutDir = (Resolve-Path $OutDir).Path

# throwaway target folder with dummy files only
$target = Join-Path $OutDir 'target\Thandor'
if (Test-Path $target) { Remove-Item -Recurse -Force $target }
New-Item -ItemType Directory -Force $target | Out-Null
Set-Content -Path (Join-Path $target 'DATEN.PCK') -Value 'dummy' -Encoding ascii
Set-Content -Path (Join-Path $target 'thandor.exe') -Value 'dummy' -Encoding ascii
Set-Content -Path (Join-Path $target 'thandor.sym') -Value 'dummy' -Encoding ascii
$realTarget = $target

# optional: show the throwaway folder as <Drive>:\Thandor (a temporary subst of <OutDir>\target, removed at the
# end), so the pictures do not show the long temp path with the user name
$substed = $false
if ($Drive) {
    $Drive = $Drive.TrimEnd(':').ToUpper()
    if (Test-Path "${Drive}:\") { throw "drive ${Drive}: is in use, pick a free letter" }
    subst "${Drive}:" (Join-Path $OutDir 'target')
    if (-not (Test-Path "${Drive}:\Thandor\DATEN.PCK")) { throw "subst ${Drive}: failed" }
    $substed = $true
    $target = "${Drive}:\Thandor"
}

Add-Type -AssemblyName System.Drawing
Add-Type -ReferencedAssemblies System.Drawing @"
using System; using System.Text; using System.Collections.Generic; using System.Runtime.InteropServices;
using System.Drawing; using System.Drawing.Imaging;
public static class OtWin {
  public delegate bool EnumProc(IntPtr h, IntPtr l);
  [DllImport("user32.dll")] static extern bool EnumWindows(EnumProc f, IntPtr l);
  [DllImport("user32.dll")] static extern bool EnumChildWindows(IntPtr p, EnumProc f, IntPtr l);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetClassName(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern IntPtr SendMessage(IntPtr h, uint m, IntPtr w, StringBuilder l);
  [DllImport("user32.dll")] static extern IntPtr SendMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
  [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
  [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] static extern bool PrintWindow(IntPtr h, IntPtr dc, uint f);
  [DllImport("dwmapi.dll")] static extern int DwmGetWindowAttribute(IntPtr h, int a, out RECT r, int n);
  public struct RECT { public int L, T, R, B; }
  public static string Cls(IntPtr h) { var s = new StringBuilder(256); GetClassName(h, s, 256); return s.ToString(); }
  public static string Text(IntPtr h) {
    int n = (int)SendMessage(h, 0x000E, IntPtr.Zero, IntPtr.Zero); // WM_GETTEXTLENGTH
    var s = new StringBuilder(n + 2); SendMessage(h, 0x000D, (IntPtr)(n + 1), s); return s.ToString(); }
  public static List<IntPtr> TopWindows(uint pid) {
    var r = new List<IntPtr>();
    EnumWindows((h, l) => { uint p; GetWindowThreadProcessId(h, out p); if (p == pid && IsWindowVisible(h)) r.Add(h); return true; }, IntPtr.Zero);
    return r; }
  public static List<IntPtr> AllTopWindows() {
    var r = new List<IntPtr>();
    EnumWindows((h, l) => { if (IsWindowVisible(h)) r.Add(h); return true; }, IntPtr.Zero);
    return r; }
  public static List<IntPtr> Children(IntPtr p) {
    var r = new List<IntPtr>(); EnumChildWindows(p, (h, l) => { r.Add(h); return true; }, IntPtr.Zero); return r; }
  public static void Capture(IntPtr h, string full, string small, int w, int hgt) {
    RECT wr; GetWindowRect(h, out wr);
    RECT fr; if (DwmGetWindowAttribute(h, 9, out fr, Marshal.SizeOf(typeof(RECT))) != 0) fr = wr; // extended frame bounds
    using (var bmp = new Bitmap(wr.R - wr.L, wr.B - wr.T, PixelFormat.Format32bppArgb)) {
      using (var g = Graphics.FromImage(bmp)) { IntPtr dc = g.GetHdc(); PrintWindow(h, dc, 2); g.ReleaseHdc(dc); }
      var crop = new Rectangle(fr.L - wr.L, fr.T - wr.T, fr.R - fr.L, fr.B - fr.T);
      using (var vis = bmp.Clone(crop, PixelFormat.Format32bppArgb)) {
        vis.Save(full, ImageFormat.Png);
        using (var sm = new Bitmap(w, hgt, PixelFormat.Format32bppArgb))
        using (var g2 = Graphics.FromImage(sm)) {
          g2.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
          g2.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.HighQuality;
          g2.DrawImage(vis, new Rectangle(0, 0, w, hgt));
          sm.Save(small, ImageFormat.Png);
        }
      }
    }
  }
}
"@

$BM_CLICK = 0x00F5
function Norm([string]$s) { ($s -replace '&', '' -replace '[<>]', '').Trim() }
function Find-Button([IntPtr]$parent, [string]$label) {
    foreach ($c in [OtWin]::Children($parent)) {
        if ([OtWin]::Cls($c) -match 'Button' -and [OtWin]::IsWindowVisible($c) -and (Norm ([OtWin]::Text($c))) -eq $label) { return $c }
    }
    return [IntPtr]::Zero
}
function Click([IntPtr]$parent, [string]$label) {
    $b = Find-Button $parent $label
    if ($b -eq [IntPtr]::Zero) { throw "button '$label' not found" }
    [void][OtWin]::PostMessage($b, $BM_CLICK, [IntPtr]::Zero, [IntPtr]::Zero)
    Start-Sleep -Milliseconds 2500
}
function Page-Has([IntPtr]$wiz, [string]$label) { (Find-Button $wiz $label) -ne [IntPtr]::Zero }

$log = Join-Path $OutDir 'setup.log'
$proc = Start-Process -FilePath $Setup -ArgumentList @("/DIR=`"$target`"", "/LOG=`"$log`"") -PassThru
"started setup PID $($proc.Id)"
$wiz = [IntPtr]::Zero; $wizPid = 0
try {
    # the wizard window belongs to the Setup.tmp child of our process; look at visible windows (bounded)
    for ($i = 0; $i -lt 60 -and $wiz -eq [IntPtr]::Zero; $i++) {
        Start-Sleep -Milliseconds 500
        foreach ($h in [OtWin]::AllTopWindows()) {
            if ([OtWin]::Cls($h) -ne 'TWizardForm') { continue }
            $p = 0; [void][OtWin]::GetWindowThreadProcessId($h, [ref]$p)
            $parent = (Get-CimInstance Win32_Process -Filter "ProcessId=$p").ParentProcessId
            if ($p -eq $proc.Id -or $parent -eq $proc.Id) { $wiz = $h; $wizPid = $p }
        }
    }
    if ($wiz -eq [IntPtr]::Zero) { throw 'wizard window not found' }
    "wizard window in PID $wizPid"
    Start-Sleep -Milliseconds 1500

    if (-not (Page-Has $wiz 'Weiter') -or (Page-Has $wiz "Zur$([char]0xFC)ck")) { throw 'not on the welcome page' }
    [OtWin]::Capture($wiz, (Join-Path $OutDir 'raw_welcome.png'), (Join-Path $OutDir 'installer-welcome.png'), $Width, $Height)
    Click $wiz 'Weiter'

    # folder page: the path must be our throwaway folder
    $edits = [OtWin]::Children($wiz) | Where-Object { [OtWin]::Cls($_) -match 'Edit' -and [OtWin]::IsWindowVisible($_) }
    $dirs = @($edits | ForEach-Object { [OtWin]::Text($_) })
    "folder page path: $($dirs -join ' | ')"
    if ($dirs.Count -ne 1 -or $dirs[0].TrimEnd('\') -ne $target) { throw "folder page does not show the throwaway folder: $($dirs -join ' | ')" }
    [OtWin]::Capture($wiz, (Join-Path $OutDir 'raw_folder.png'), (Join-Path $OutDir 'installer-folder.png'), $Width, $Height)
    Click $wiz 'Weiter'

    if (-not (Page-Has $wiz 'Installieren')) { throw 'not on the ready page' }
    [OtWin]::Capture($wiz, (Join-Path $OutDir 'raw_ready.png'), (Join-Path $OutDir 'installer-ready.png'), $Width, $Height)
}
catch { "ERROR: $_" }
finally {
    # cancel (never install) and confirm the cancel prompt
    if ($wiz -ne [IntPtr]::Zero -and (Find-Button $wiz 'Abbrechen') -ne [IntPtr]::Zero) {
        Click $wiz 'Abbrechen'
        foreach ($h in [OtWin]::TopWindows([uint32]$wizPid)) {
            if ([OtWin]::Cls($h) -eq '#32770') { $ja = Find-Button $h 'Ja'; if ($ja -ne [IntPtr]::Zero) { [void][OtWin]::PostMessage($ja, $BM_CLICK, [IntPtr]::Zero, [IntPtr]::Zero) } }
        }
    }
    foreach ($id in @($wizPid, $proc.Id) | Where-Object { $_ -ne 0 }) {
        try { Wait-Process -Id $id -Timeout 20 -ErrorAction Stop } catch {}
        if (Get-Process -Id $id -ErrorAction SilentlyContinue) { "killing own PID $id"; Stop-Process -Id $id -Force }
        else { "PID $id exited" }
    }
    if ($substed) { subst "${Drive}:" /D; "removed subst ${Drive}:" }
}
