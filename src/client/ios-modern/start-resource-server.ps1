param(
    [string]$HostAddress = '0.0.0.0',
    [int]$Port = 0,
    [string]$WebRoot = 'src/client/ios-modern/generated/web'
)
$ErrorActionPreference = 'Stop'
$projectRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../..'))
if ([IO.Path]::IsPathRooted($WebRoot) -and $WebRoot -notmatch '^[A-Za-z]:[\\/]|^\\\\') {
    throw 'Use a project-relative web root or a fully qualified absolute path.'
}
$webRoot = if ([IO.Path]::IsPathRooted($WebRoot)) { [IO.Path]::GetFullPath($WebRoot) } else { [IO.Path]::GetFullPath((Join-Path $projectRoot $WebRoot)) }
if (-not (Test-Path -LiteralPath (Join-Path $webRoot '.onepiece-deployment.json'))) {
    throw 'Run prepare.py --host PHONE_REACHABLE_HOST first.'
}
$manifest = Get-Content -LiteralPath (Join-Path $webRoot '.onepiece-deployment.json') -Raw -Encoding UTF8 | ConvertFrom-Json
if (-not $Port) { $Port = [int]$manifest.webPort }
if ($Port -lt 1 -or $Port -gt 65535) { throw 'Use a resource port in 1..65535.' }
if (Get-NetTCPConnection -State Listen -LocalPort $Port -ErrorAction SilentlyContinue) {
    throw "Port $Port is occupied; inspect the listener before starting another service."
}
$python = Join-Path $projectRoot 'deployment/.bootstrap/python/python.exe'
if (-not (Test-Path -LiteralPath $python)) { $python = (Get-Command python.exe -ErrorAction Stop).Source }
$logRoot = Join-Path $projectRoot 'runtime/ios-resource'
New-Item -ItemType Directory -Path $logRoot -Force | Out-Null
# Serve only the generated public assets, never the repository or database directories.
$process = Start-Process -FilePath $python -ArgumentList @('-m', 'http.server', [string]$Port, '--bind', $HostAddress) -WorkingDirectory $webRoot -WindowStyle Hidden -RedirectStandardOutput (Join-Path $logRoot 'stdout.log') -RedirectStandardError (Join-Path $logRoot 'stderr.log') -PassThru
$process.Id | Set-Content -LiteralPath (Join-Path $logRoot 'resource-server.pid')
Write-Host "iOS resource service started: PID $($process.Id), http://$($manifest.host):$Port"
Write-Host "The game service must also be running on port $($manifest.gamePort); keep the phone on a reachable network."
