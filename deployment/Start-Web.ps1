param(
    [ValidateSet('start', 'stop', 'status')][string]$Action = 'start',
    [switch]$External,
    [string]$InstallDir = 'runtime/server',
    [string]$BuildDir = 'outputs/web',
    [ValidateRange(0, 65535)][int]$Port = 18084,
    [string]$PublicOrigin = '',
    [string]$BindAddress = '0.0.0.0',
    [ValidateSet('auto', 'http2', 'quic')][string]$TunnelProtocol = 'http2',
    [switch]$SkipBuild,
    [switch]$Offline
)
$ErrorActionPreference = 'Stop'
$webStartupMutex = $null
$webStartupLock = $false
try {
    . (Join-Path $PSScriptRoot 'Project-Paths.ps1')
    [Console]::OutputEncoding = New-Object Text.UTF8Encoding($false)
    $env:PYTHONIOENCODING = 'utf-8'
    $resolvedInstall = Resolve-NrbProjectPath $InstallDir
    $resolvedBuild = Resolve-NrbProjectPath $BuildDir
    if ($External -and -not $PSBoundParameters.ContainsKey('Port')) { $Port = 0 }
    if (-not $External -and $Port -eq 0) { $Port = 18084 }
    if ($Port -ne 0 -and $Port -lt 1024) { throw 'Port must be 0 (automatic) or 1024..65535.' }
    if ($External -and ($PublicOrigin -or $PSBoundParameters.ContainsKey('BindAddress'))) {
        throw 'External mode assigns its HTTPS URL automatically and binds only to loopback. Omit PublicOrigin and BindAddress.'
    }
    $python = Join-Path $script:NrbProjectRoot 'deployment/.bootstrap/python/python.exe'
    if ($Action -eq 'start') {
        # Two simultaneous clicks must not compile into the same output tree.
        $identityBytes = [Text.Encoding]::UTF8.GetBytes($resolvedBuild.ToUpperInvariant())
        $identityHasher = [Security.Cryptography.SHA256]::Create()
        try { $identityHash = [BitConverter]::ToString($identityHasher.ComputeHash($identityBytes)).Replace('-', '') }
        finally { $identityHasher.Dispose() }
        $webStartupMutex = New-Object Threading.Mutex($false, ('Local\NRB-Web-' + $identityHash.Substring(0, 24)))
        try { $webStartupLock = $webStartupMutex.WaitOne(0) }
        catch [Threading.AbandonedMutexException] { $webStartupLock = $true }
        if (-not $webStartupLock) { throw 'Another Web startup is already preparing this client. Wait for that window to finish.' }
        $serverArgs = @{ Action = 'start'; InstallDir = $resolvedInstall }
        if ($Offline) { $serverArgs.Offline = $true }
        & (Join-Path $PSScriptRoot 'Deploy-Server.ps1') @serverArgs
        if ($LASTEXITCODE -ne 0) { throw 'Server startup failed; browser gateway was not started.' }
        $complete = $false
        $manifestPath = Join-Path $resolvedBuild 'web-client.json'
        if (Test-Path -LiteralPath $manifestPath) {
            try {
                $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
                $complete = $manifest.platform -eq 'web' -and $manifest.buildComplete -eq $true -and $manifest.accountAuth -eq 1
                foreach ($name in @('index.html', 'web.js', 'accounts.js', 'style.css', 'game.js', 'game.wasm', 'game.data')) {
                    $path = Join-Path $resolvedBuild $name
                    if (-not (Test-Path -LiteralPath $path -PathType Leaf) -or (Get-Item -LiteralPath $path).Length -eq 0) { $complete = $false }
                }
            } catch { $complete = $false }
        }
        if (-not $complete) {
            if ($SkipBuild -or $Offline) { throw 'Web build is missing/incomplete. Run deployment/build-web.cmd or extract the Web Actions artifact first.' }
            Write-Host 'Preparing the Web client; missing toolchain components will be downloaded on first use.'
            & (Join-Path $PSScriptRoot 'Build-Web.ps1') -OutputDir $resolvedBuild
            if ($LASTEXITCODE -ne 0) { throw 'Web build failed; external access was not started.' }
        }
    } elseif (-not (Test-Path -LiteralPath $python)) {
        throw 'No local deployment runtime. Run the Web start shortcut first.'
    }
    if ($External) {
        $externalArgs = @((Join-Path $PSScriptRoot 'web_external.py'), $Action,
            '--install-dir', $resolvedInstall, '--build-dir', $resolvedBuild,
            '--port', [string]$Port, '--protocol', $TunnelProtocol)
        if ($Offline) { $externalArgs += '--offline' }
        & $python @externalArgs
        if ($LASTEXITCODE -ne 0) { throw 'External Web command failed. Read the error above and logs/web_tunnel.stderr.log.' }
        return
    }
    if ($Action -ne 'start') {
        & $python (Join-Path $PSScriptRoot 'web_client.py') ('--managed-' + $Action) '--install-dir' $resolvedInstall
        if ($LASTEXITCODE -ne 0) { throw 'Web management command failed.' }
        return
    }
    $arguments = @((Join-Path $PSScriptRoot 'web_client.py'), '--managed-start', '--install-dir',
        $resolvedInstall, '--build-dir', $resolvedBuild, '--port', $Port, '--bind', $BindAddress)
    if ($PublicOrigin) { $arguments += @('--public-origin', $PublicOrigin) }
    & $python @arguments
    if ($LASTEXITCODE -ne 0) { throw 'Web gateway startup failed. Check runtime/server/logs/browser.stderr.log.' }
} catch {
    Write-Host ('ERROR: ' + $_.Exception.Message) -ForegroundColor Red
    exit 1
} finally {
    if ($webStartupLock) { $webStartupMutex.ReleaseMutex() }
    if ($null -ne $webStartupMutex) { $webStartupMutex.Dispose() }
}
