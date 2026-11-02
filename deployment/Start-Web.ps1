param(
    [string]$InstallDir = 'runtime/server',
    [string]$BuildDir = 'outputs/web',
    [ValidateRange(1024, 65535)][int]$Port = 18084,
    [string]$PublicOrigin = '',
    [string]$BindAddress = '0.0.0.0'
)
$ErrorActionPreference = 'Stop'
try {
    . (Join-Path $PSScriptRoot 'Project-Paths.ps1')
    $resolvedBuild = Resolve-NrbProjectPath $BuildDir
    if (-not (Test-Path -LiteralPath (Join-Path $resolvedBuild 'web-client.json'))) {
        throw 'Web build is missing. Run deployment/build-web.cmd first, or extract the Web Actions artifact into outputs/web.'
    }
    & (Join-Path $PSScriptRoot 'Deploy-Server.ps1') -Action start -InstallDir $InstallDir
    if ($LASTEXITCODE -ne 0) { throw 'Server startup failed; browser gateway was not started.' }
    $python = Join-Path $script:NrbProjectRoot 'deployment/.bootstrap/python/python.exe'
    $arguments = @((Join-Path $PSScriptRoot 'web_client.py'), '--managed-start', '--install-dir',
        (Resolve-NrbProjectPath $InstallDir), '--build-dir', $resolvedBuild, '--port', $Port, '--bind', $BindAddress)
    if ($PublicOrigin) { $arguments += @('--public-origin', $PublicOrigin) }
    & $python @arguments
    if ($LASTEXITCODE -ne 0) { throw 'Web gateway startup failed. Check runtime/server/logs/browser.stderr.log.' }
} catch {
    Write-Host ('ERROR: ' + $_.Exception.Message) -ForegroundColor Red
    exit 1
}
