param(
    [string]$HostAddress = '192.168.30.101',
    [int]$Port = 18081
)
$ErrorActionPreference = 'Stop'
$webRoot = Join-Path $PSScriptRoot 'generated/web'
if (-not (Test-Path -LiteralPath (Join-Path $webRoot '.onepiece-deployment.json'))) {
    throw 'Run prepare.py --host PHONE_REACHABLE_HOST first.'
}
if (Get-NetTCPConnection -State Listen -LocalPort $Port -ErrorAction SilentlyContinue) {
    throw "Port $Port is occupied; inspect the listener before starting another service."
}
$python = (Get-Command python.exe -ErrorAction Stop).Source
# Serve only the generated public assets, never the repository or database directories.
$process = Start-Process -FilePath $python -ArgumentList @('-m', 'http.server', [string]$Port, '--bind', $HostAddress) -WorkingDirectory $webRoot -WindowStyle Hidden -RedirectStandardOutput (Join-Path $PSScriptRoot 'generated/resource-server.stdout.log') -RedirectStandardError (Join-Path $PSScriptRoot 'generated/resource-server.stderr.log') -PassThru
$process.Id | Set-Content -LiteralPath (Join-Path $PSScriptRoot 'generated/resource-server.pid')
Write-Host "iOS resource service started: PID $($process.Id), http://${HostAddress}:$Port"
Write-Host 'The game service must also be running on port 25524; keep the phone on a reachable network.'
