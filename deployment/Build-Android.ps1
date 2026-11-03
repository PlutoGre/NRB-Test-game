param(
    [string]$InstallDir = 'runtime/server',
    [string]$AndroidSdk = 'tools/android-sdk',
    [string]$AndroidNdk = 'tools/android-ndk-r8e',
    [string]$AntDir = 'tools/apache-ant',
    [string]$JavaHome = '',
    [string]$OutputDir = 'outputs/android',
    [ValidateRange(1, 16)][int]$Jobs = 2,
    [switch]$SceneInspection,
    [switch]$Offline,
    [switch]$PrepareToolsOnly
)
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'Project-Paths.ps1')
$originalLocation = Get-Location
$createdMappings = New-Object 'System.Collections.Generic.List[object]'
$savedEnvironment = @{}
$inspectionManifest = ''
$inspectionOriginal = $null
foreach ($name in @('PATH', 'JAVA_HOME', 'ANDROID_HOME', 'ANDROID_SDK_ROOT', 'ANT_HOME',
                    'NDK_ROOT', 'NDK_MODULE_PATH', 'NRB_PROJECT_ROOT', 'PYTHONIOENCODING', 'TEMP', 'TMP')) {
    $savedEnvironment[$name] = [Environment]::GetEnvironmentVariable($name, 'Process')
}

# r8e's makefiles cannot handle spaces or Unicode in their working paths. A
# temporary DOS device alias keeps all source/build data in the checkout itself.
function New-NrbBuildDrive {
    param([string]$Directory)
    $target = Resolve-NrbProjectPath $Directory
    if (-not (Test-Path -LiteralPath $target -PathType Container)) { throw "Directory missing: $target" }
    $mutex = New-Object Threading.Mutex($false, 'Local\NRB-BuildDriveAllocation')
    try {
        try { [void]$mutex.WaitOne() } catch [Threading.AbandonedMutexException] { }
        try {
            for ($number = 25; $number -ge 3; $number--) {
                if ([Nrb.BuildPaths]::GetLogicalDrives() -band ([uint32]1 -shl $number)) { continue }
                $device = ([char](65 + $number)).ToString() + ':'
                if (Get-PSDrive -Name $device.Substring(0, 1) -ErrorAction SilentlyContinue) { continue }
                $existing = New-Object Text.StringBuilder 32768
                if ([Nrb.BuildPaths]::QueryDosDevice($device, $existing, $existing.Capacity)) { continue }
                if (-not [Nrb.BuildPaths]::DefineDosDevice(0, $device, $target)) {
                    throw ('Cannot create build drive: ' + [Runtime.InteropServices.Marshal]::GetLastWin32Error())
                }
                $actual = New-Object Text.StringBuilder 32768
                $expected = if ($target.StartsWith('\\')) { '\??\UNC\' + $target.Substring(2) } else { '\??\' + $target }
                if (-not [Nrb.BuildPaths]::QueryDosDevice($device, $actual, $actual.Capacity) `
                        -or -not $actual.ToString().Equals($expected, [StringComparison]::OrdinalIgnoreCase)) {
                    # Exact-target removal never removes a different user's drive mapping.
                    [void][Nrb.BuildPaths]::DefineDosDevice(6, $device, $target)
                    throw 'Cannot record the build drive identity.'
                }
                $createdMappings.Add(@{ Device = $device; Target = $actual.ToString() })
                return $device + '\'
            }
            throw 'No free drive letter is available for legacy Android tools.'
        } finally { $mutex.ReleaseMutex() }
    } finally { $mutex.Dispose() }
}

function ConvertTo-NrbBuildDirectory {
    param([string]$Directory)
    $path = Resolve-NrbProjectPath $Directory
    if ($path -eq $script:NrbProjectRoot) { return $script:NrbBuildRoot }
    if ($path.StartsWith($script:NrbProjectRoot + '\', [StringComparison]::OrdinalIgnoreCase)) {
        $candidate = $script:NrbBuildRoot + $path.Substring($script:NrbProjectRoot.Length + 1)
        if ($candidate -match '^[A-Za-z0-9_:\\/.\-]+$') { return $candidate }
    }
    return New-NrbBuildDrive $path
}

try {
    if (-not [Environment]::Is64BitOperatingSystem) { throw '64-bit Windows is required.' }
    $runtime = Resolve-NrbProjectPath $InstallDir
    $settingsFile = Join-Path $runtime 'client-config/deployment.json'
    if (-not (Test-Path -LiteralPath $settingsFile)) { throw 'Deploy the server first, or provide its -InstallDir.' }
    $settings = Get-Content -LiteralPath $settingsFile -Raw -Encoding UTF8 | ConvertFrom-Json
    $portablePython = Join-Path $script:NrbProjectRoot 'deployment/.bootstrap/python/python.exe'
    if (-not (Test-Path -LiteralPath $portablePython)) { throw 'Run deployment/server.cmd first to prepare Python.' }
    if (-not $JavaHome) { $JavaHome = Join-Path $runtime 'runtimes/java' }
    if (-not (Test-Path -LiteralPath (Join-Path (Resolve-NrbProjectPath $JavaHome) 'bin/javac.exe'))) {
        throw 'JDK 8 is missing. Finish server deployment or provide -JavaHome.'
    }
    $env:PYTHONIOENCODING = 'utf-8'
    $setupArgs = @((Join-Path $PSScriptRoot 'setup_android.py'), '--sdk', $AndroidSdk,
        '--ndk', $AndroidNdk, '--ant', $AntDir)
    if ($Offline) { $setupArgs += '--offline' }
    Write-Host 'Preparing the local Android toolchain (downloads missing components on first use)...'
    & $portablePython @setupArgs
    if ($LASTEXITCODE -ne 0) { throw 'Android tool setup failed. Read the download/directory error above.' }
    $toolchain = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'android-toolchain.json') -Raw -Encoding UTF8 | ConvertFrom-Json
    foreach ($tool in @((Join-Path (Resolve-NrbProjectPath $AndroidSdk) 'tools/ant/build.xml'),
                        (Join-Path (Resolve-NrbProjectPath $AndroidSdk) 'platforms/android-21/android.jar'),
                        (Join-Path (Resolve-NrbProjectPath $AndroidNdk) 'ndk-build.cmd'),
                        (Join-Path (Resolve-NrbProjectPath $AntDir) 'bin/ant.bat'),
                        (Join-Path (Resolve-NrbProjectPath $JavaHome) 'bin/javac.exe'))) {
        if (-not (Test-Path -LiteralPath $tool)) { throw "Required tool missing: $tool. See docs/客户端打包流程.md." }
    }
    if ($PrepareToolsOnly) {
        Write-Host 'Android tools are ready. No compilation or APK packaging was started.'
        return
    }
    if (-not ('Nrb.BuildPaths' -as [type])) {
        Add-Type -TypeDefinition @'
using System.Runtime.InteropServices;
using System.Text;
namespace Nrb {
    public static class BuildPaths {
        [DllImport("kernel32.dll")] public static extern uint GetLogicalDrives();
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        public static extern uint QueryDosDevice(string name, StringBuilder target, int length);
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool DefineDosDevice(uint flags, string name, string target);
    }
}
'@
    }
    $script:NrbBuildRoot = New-NrbBuildDrive $script:NrbProjectRoot
    $sdk = ConvertTo-NrbBuildDirectory $AndroidSdk
    $ndk = ConvertTo-NrbBuildDirectory $AndroidNdk
    $ant = ConvertTo-NrbBuildDirectory $AntDir
    $java = ConvertTo-NrbBuildDirectory $JavaHome
    $python = Join-Path $script:NrbBuildRoot 'deployment/.bootstrap/python/python.exe'
    $project = Join-Path $script:NrbBuildRoot 'client/source/Game/proj.android'
    $channel = Join-Path $project 'antbuild/anzhi'
    $prepare = Join-Path $script:NrbBuildRoot 'deployment/prepare_android.py'
    $tempDir = Join-Path $script:NrbBuildRoot 'runtime/android/tmp'
    New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
    $env:NRB_PROJECT_ROOT = $script:NrbBuildRoot
    $env:PYTHONIOENCODING = 'utf-8'
    $env:JAVA_HOME = $java
    $env:ANDROID_HOME = $sdk
    $env:ANDROID_SDK_ROOT = $sdk
    $env:ANT_HOME = $ant
    $env:NDK_ROOT = $ndk
    $client = (Join-Path $script:NrbBuildRoot 'client/source').Replace('\', '/')
    $env:NDK_MODULE_PATH = "$client;$client/cocos2dx/platform/third_party/android/prebuilt"
    $env:PATH = "$java\bin;$ant\bin;$sdk\tools;" + $savedEnvironment['PATH']
    $env:TEMP = $tempDir; $env:TMP = $tempDir
    Set-Location $script:NrbBuildRoot
    # Regenerate overlays in this checkout, including when the runtime was reused.
    & $python (Join-Path $script:NrbBuildRoot 'scripts/configure_deployment.py') `
        --host $settings.host --web-port $settings.webPort --game-port $settings.gamePort `
        --resource-version $settings.resourceVersion --channel anzhi --local-login --web-root (Join-Path $runtime 'web')
    if ($LASTEXITCODE -ne 0) { throw 'Deployment overlay preparation failed.' }
    & $python $prepare --sdk $sdk --skip-native-sync
    if ($LASTEXITCODE -ne 0) { throw 'Android source preparation failed.' }
    # r8e dependency files contain absolute paths. Keep incomplete work reusable,
    # but never load caches from a different checkout/alias or the legacy layout.
    $contextPath = Join-Path $script:NrbProjectRoot 'runtime/android/native-build-context.json'
    $context = [ordered]@{ project = $script:NrbProjectRoot; buildRoot = $script:NrbBuildRoot; ndk = $ndk; sdk = $sdk }
    $contextText = $context | ConvertTo-Json -Compress
    $previousContext = if (Test-Path -LiteralPath $contextPath) { [IO.File]::ReadAllText($contextPath).Trim() } else { '' }
    $nativeObjects = [IO.Path]::GetFullPath((Join-Path $project 'obj'))
    if ((Test-Path -LiteralPath $nativeObjects) -and $previousContext -ne $contextText) {
        $physicalObjects = [IO.Path]::GetFullPath((Join-Path $script:NrbProjectRoot 'client/source/Game/proj.android/obj'))
        $backupRoot = [IO.Path]::GetFullPath((Join-Path $script:NrbProjectRoot 'runtime/android/native-cache-backups'))
        $backupObjects = [IO.Path]::GetFullPath((Join-Path $backupRoot ([Guid]::NewGuid().ToString('N'))))
        if (-not $physicalObjects.StartsWith($script:NrbProjectRoot + '\', [StringComparison]::OrdinalIgnoreCase) `
            -or -not $backupObjects.StartsWith($backupRoot + '\', [StringComparison]::OrdinalIgnoreCase)) {
            throw 'Native cache backup must remain inside this project.'
        }
        New-Item -ItemType Directory -Path $backupRoot -Force | Out-Null
        Move-Item -LiteralPath $physicalObjects -Destination $backupObjects
        Write-Host "Backed up native cache from an older build location: $backupObjects"
    }
    New-Item -ItemType Directory -Path (Split-Path -Parent $contextPath) -Force | Out-Null
    [IO.File]::WriteAllText($contextPath, $contextText, (New-Object Text.UTF8Encoding($false)))
    Write-Host 'Building native game libraries...'
    & (Join-Path $ndk 'ndk-build.cmd') '-C' $project "-j$Jobs"
    if ($LASTEXITCODE -ne 0) { throw 'NDK compilation failed; APK packaging was not started.' }
    & $python $prepare --sdk $sdk
    if ($LASTEXITCODE -ne 0) { throw 'Native library synchronization failed.' }
    Set-Location $channel
    if ($SceneInspection) {
        $inspectionManifest = Join-Path $channel 'AndroidManifest.xml'
        $inspectionOriginal = [IO.File]::ReadAllBytes($inspectionManifest)
        $manifestText = [IO.File]::ReadAllText($inspectionManifest)
        if (-not $manifestText.Contains('android:debuggable="false"')) { throw 'Unexpected debug flag in Android manifest.' }
        [IO.File]::WriteAllText($inspectionManifest, $manifestText.Replace('android:debuggable="false"', 'android:debuggable="true"'), (New-Object Text.UTF8Encoding($false)))
        Write-Host 'Scene inspection build: local file opt-in is required; no game actions are exposed.'
    }
    # Avoid selecting a newer installed Build Tools version that no longer has dx.
    $buildTools = Join-Path $sdk ($toolchain.'build-tools'.destination.Substring(4))
    # Command-line properties keep the same absolute SDK path in every Ant subproject.
    & (Join-Path $ant 'bin/ant.bat') ("-Ddeployment.python=" + $python.Replace('\', '/')) `
        ("-Dsdk.dir=" + $sdk.Replace('\', '/')) `
        ("-Dandroid.build.tools.dir=" + $buildTools.Replace('\', '/')) debug
    if ($LASTEXITCODE -ne 0) { throw 'Ant packaging failed.' }
    $apk = Join-Path $channel 'bin/DreamOnePiece-debug.apk'
    if (-not (Test-Path -LiteralPath $apk)) { throw 'Ant completed without the expected APK.' }
    $destination = Resolve-NrbProjectPath $OutputDir
    New-Item -ItemType Directory -Path $destination -Force | Out-Null
    Copy-Item -LiteralPath $apk -Destination (Join-Path $destination 'DreamOnePiece-debug.apk') -Force
    Write-Host ('APK: ' + (Join-Path $destination 'DreamOnePiece-debug.apk'))

    # Some emulators prefer ARM even when an APK contains x86. A separate package
    # prevents installation from selecting the legacy ARM translation path.
    $emulatorLibraries = Join-Path $script:NrbBuildRoot 'runtime/android/x86-apk-libs'
    New-Item -ItemType Directory -Path (Join-Path $emulatorLibraries 'x86') -Force | Out-Null
    Copy-Item -LiteralPath (Join-Path $project 'libs/x86/libDreamOnePiece.so') `
        -Destination (Join-Path $emulatorLibraries 'x86/libDreamOnePiece.so') -Force
    $emulatorApk = Join-Path $channel 'bin/DreamOnePiece-emulator-x86-debug.apk'
    Write-Host 'Packaging the native x86 emulator APK...'
    & (Join-Path $ant 'bin/ant.bat') ("-Ddeployment.python=" + $python.Replace('\', '/')) `
        ("-Dsdk.dir=" + $sdk.Replace('\', '/')) `
        ("-Dandroid.build.tools.dir=" + $buildTools.Replace('\', '/')) `
        '-Dnrb.emulator.x86=true' `
        ("-Dnative.libs.absolute.dir=" + $emulatorLibraries.Replace('\', '/')) `
        ("-Dout.packaged.file=" + (Join-Path $channel 'bin/DreamOnePiece-emulator-x86-debug-unaligned.apk').Replace('\', '/')) `
        ("-Dout.final.file=" + $emulatorApk.Replace('\', '/')) debug
    if ($LASTEXITCODE -ne 0 -or -not (Test-Path -LiteralPath $emulatorApk)) { throw 'x86 emulator APK packaging failed.' }
    & $python -c "import sys,zipfile; z=zipfile.ZipFile(sys.argv[1]); libs={n for n in z.namelist() if n.startswith('lib/') and n.endswith('.so')}; assert libs == {'lib/x86/libDreamOnePiece.so'}, 'Unexpected emulator native libraries: ' + repr(libs)" $emulatorApk
    if ($LASTEXITCODE -ne 0) { throw 'Emulator APK must contain only the x86 game library.' }
    Copy-Item -LiteralPath $emulatorApk -Destination (Join-Path $destination 'DreamOnePiece-emulator-x86-debug.apk') -Force
    Write-Host ('Emulator APK: ' + (Join-Path $destination 'DreamOnePiece-emulator-x86-debug.apk'))
} catch {
    Write-Host $_.Exception.Message -ForegroundColor Red
    exit 1
} finally {
    if ($inspectionOriginal -ne $null -and $inspectionManifest) {
        [IO.File]::WriteAllBytes($inspectionManifest, $inspectionOriginal)
    }
    Set-Location $originalLocation
    foreach ($name in $savedEnvironment.Keys) {
        [Environment]::SetEnvironmentVariable($name, $savedEnvironment[$name], 'Process')
    }
    for ($i = $createdMappings.Count - 1; $i -ge 0; $i--) {
        $mapping = $createdMappings[$i]
        $current = New-Object Text.StringBuilder 32768
        if ([Nrb.BuildPaths]::QueryDosDevice($mapping.Device, $current, $current.Capacity) `
                -and $current.ToString() -eq $mapping.Target) {
            if (-not [Nrb.BuildPaths]::DefineDosDevice(7, $mapping.Device, $mapping.Target)) {
                Write-Warning ('Cannot release temporary build drive ' + $mapping.Device)
            }
        } else { Write-Warning ('Build drive identity changed; retained ' + $mapping.Device) }
    }
}
