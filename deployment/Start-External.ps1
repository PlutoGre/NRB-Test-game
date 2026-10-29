param(
    [ValidateSet('start', 'stop', 'status', 'download', 'rotate-invite')]
    [string]$Action = 'start',
    [string]$InstallDir = 'runtime/server',
    [int]$GatewayPort = 18082,
    [ValidateSet('auto', 'http2', 'quic')]
    [string]$TunnelProtocol = 'auto',
    [switch]$Offline
)
$ErrorActionPreference = 'Stop'
try {
    . (Join-Path $PSScriptRoot 'Project-Paths.ps1')
    $InstallDir = Resolve-NrbProjectPath $InstallDir
    $python = Join-Path $PSScriptRoot '.bootstrap/python/python.exe'
    if (-not (Test-Path -LiteralPath $python)) {
        throw 'Run Deploy-Server.ps1 first to install the server and portable Python.'
    }
    $env:PYTHONIOENCODING = 'utf-8'
    $externalArgs = @((Join-Path $PSScriptRoot 'external.py'), $Action, '--install-dir', $InstallDir,
        '--port', [string]$GatewayPort, '--protocol', $TunnelProtocol)
    if ($Offline) { $externalArgs += '--offline' }
    & $python @externalArgs
    if ($LASTEXITCODE -ne 0) { throw "External access command failed (exit $LASTEXITCODE)." }
} catch {
    Write-Host $_.Exception.Message -ForegroundColor Red
    exit 1
}
