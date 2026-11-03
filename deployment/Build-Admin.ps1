param([string]$JavaHome = '', [switch]$ServerOnly, [string]$OutputDir = '')
$ErrorActionPreference = 'Stop'
$repo = Split-Path -Parent $PSScriptRoot
$admin = Join-Path $PSScriptRoot 'admin'
$artifactDir = $admin
if ($OutputDir) {
    if (-not [IO.Path]::IsPathRooted($OutputDir)) { $OutputDir = Join-Path $repo $OutputDir }
    $artifactDir = [IO.Path]::GetFullPath($OutputDir)
    New-Item -ItemType Directory -Path $artifactDir -Force | Out-Null
}
$buildDir = Join-Path $artifactDir '.build'
$classes = Join-Path $buildDir ('classes-' + [Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $classes -Force | Out-Null
if (-not $JavaHome) {
    $candidates = @((Join-Path $repo 'runtime/server/runtimes/java'), $env:JAVA_HOME)
    foreach ($candidate in $candidates) {
        if ($candidate -and (Test-Path -LiteralPath (Join-Path $candidate 'bin/javac.exe'))) { $JavaHome = $candidate; break }
    }
}
if (-not $JavaHome -or -not (Test-Path -LiteralPath (Join-Path $JavaHome 'bin/javac.exe'))) {
    throw 'JDK 8 is required to generate the server companion package. Deploy the server first, or pass -JavaHome.'
}
$python = Join-Path $PSScriptRoot '.bootstrap/python/python.exe'
if (-not (Test-Path -LiteralPath $python)) { throw 'Run the server deployment once to prepare portable Python.' }
& $python (Join-Path $admin 'export_names.py') --output $classes
if ($LASTEXITCODE -ne 0) { throw 'Administration resource generation failed.' }
$originalJar = Join-Path $repo 'server/distribution/DZM.jar'
$inputTool = Join-Path $admin 'source_inputs.py'
$sources = @(& $python $inputTool --sources)
if ($LASTEXITCODE -ne 0) { throw 'Server overlay source enumeration failed.' }
# Build only the explicitly listed overlay classes against the original self-contained JAR.
$emptySources = Join-Path $buildDir 'empty-sourcepath'
New-Item -ItemType Directory -Path $emptySources -Force | Out-Null
& (Join-Path $JavaHome 'bin/javac.exe') '-J-Duser.language=en' '-J-Duser.country=US' -encoding UTF-8 -source 8 -target 8 -cp $originalJar -sourcepath $emptySources -d $classes @sources
if ($LASTEXITCODE -ne 0) { throw 'Administration server compilation failed.' }
$jarFile = Join-Path $artifactDir 'NRBAdminServer.jar'
& (Join-Path $JavaHome 'bin/jar.exe') cf ($jarFile + '.writing') -C $classes .
if ($LASTEXITCODE -ne 0) { throw 'Administration server packaging failed.' }
Move-Item -LiteralPath ($jarFile + '.writing') -Destination $jarFile -Force
$exe = Join-Path $repo 'NRB游戏管理.exe'
$exeWriting = Join-Path $buildDir 'NRBAdmin.exe'
if (-not $ServerOnly) {
    $compiler = Join-Path $env:WINDIR 'Microsoft.NET/Framework/v4.0.30319/csc.exe'
    if (-not (Test-Path -LiteralPath $compiler)) { throw 'Windows .NET Framework C# compiler is unavailable.' }
    & $compiler /nologo /utf8output /target:winexe /platform:anycpu /optimize+ /codepage:65001 /langversion:5 `
        "/out:$exeWriting" "/win32manifest:$(Join-Path $admin 'app.manifest')" `
        /reference:System.dll /reference:System.Core.dll /reference:System.Drawing.dll `
        /reference:System.Windows.Forms.dll /reference:System.Web.Extensions.dll (Join-Path $admin 'AdminTool.cs')
    if ($LASTEXITCODE -ne 0) { throw 'Windows administration EXE compilation failed.' }
    Copy-Item -LiteralPath $exeWriting -Destination $exe -Force
}
$sourceFingerprint = (& $python $inputTool).Trim()
if ($LASTEXITCODE -ne 0) { throw 'Server overlay fingerprint generation failed.' }
$metadata = [ordered]@{
    protocol = 1
    baseJarSha256 = (Get-FileHash -LiteralPath $originalJar -Algorithm SHA256).Hash.ToLowerInvariant()
    serverJarSha256 = (Get-FileHash -LiteralPath $jarFile -Algorithm SHA256).Hash.ToLowerInvariant()
    exeSha256 = (Get-FileHash -LiteralPath $exe -Algorithm SHA256).Hash.ToLowerInvariant()
    sourceSha256 = $sourceFingerprint
}
$json = ($metadata | ConvertTo-Json) + "`n"
[IO.File]::WriteAllText((Join-Path $artifactDir 'artifacts.json'), $json, (New-Object Text.UTF8Encoding($false)))
if (-not $ServerOnly) { Write-Host "Generated EXE: $exe" }
Write-Host "Generated server companion: $jarFile"
Write-Host 'Artifact generation only. No server startup, login, grant or gameplay test was executed.'
