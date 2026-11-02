param(
    [string]$OutputDir = 'outputs/web',
    [string]$Emsdk = '',
    [ValidateRange(1, 16)][int]$Jobs = 2
)
$ErrorActionPreference = 'Stop'
try {
    . (Join-Path $PSScriptRoot 'Project-Paths.ps1')
    $python = Join-Path $script:NrbProjectRoot 'deployment/.bootstrap/python/python.exe'
    if (-not (Test-Path -LiteralPath $python)) {
        throw 'Run the root server start shortcut once to prepare portable Python, then run build-web.cmd again.'
    }
    $arguments = @((Join-Path $script:NrbProjectRoot 'client/web/build.py'), '--output',
        (Resolve-NrbProjectPath $OutputDir), '--jobs', $Jobs)
    if ($Emsdk) { $arguments += @('--emsdk', (Resolve-NrbProjectPath $Emsdk)) }
    & $python @arguments
    if ($LASTEXITCODE -ne 0) { throw 'Web compilation failed. Read the compiler log shown above.' }
    Write-Host 'Web client built. Run deployment/start-web.cmd to open the local URL.'
} catch {
    Write-Host ('ERROR: ' + $_.Exception.Message) -ForegroundColor Red
    exit 1
}
