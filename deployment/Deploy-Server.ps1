param(
    [ValidateSet('install', 'start', 'stop', 'status', 'backup', 'restore', 'download')]
    [string]$Action = 'install',
    [string]$InstallDir = 'runtime/server',
    [string]$PublicHost = '',
    [Nullable[int]]$WebPort = $null,
    [Nullable[int]]$GamePort = $null,
    [Nullable[int]]$DatabasePort = $null,
    [string]$ResourceVersion = '1.78.6',
    [ValidateSet('anzhi', 'all')]
    [string]$Channel = 'anzhi',
    [switch]$Offline,
    [string]$RestoreFile = '',
    [switch]$ConfirmRestore,
    [switch]$OpenFirewall
)
$ErrorActionPreference = 'Stop'
try {
    . (Join-Path $PSScriptRoot 'Project-Paths.ps1')
    $InstallDir = Resolve-NrbProjectPath $InstallDir
    if ($RestoreFile) { $RestoreFile = Resolve-NrbProjectPath $RestoreFile }
    if (-not [Environment]::Is64BitOperatingSystem) { throw '64-bit Windows is required.' }
    if ($OpenFirewall) {
        if ($Action -notin @('install', 'start')) { throw '-OpenFirewall is supported only with install or start.' }
        $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
        $principal = New-Object Security.Principal.WindowsPrincipal($identity)
        if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
            throw '-OpenFirewall requires an elevated PowerShell window; installation alone does not.'
        }
    }
    $manifest = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'runtime-manifest.json') -Raw | ConvertFrom-Json
    $cache = Join-Path $PSScriptRoot 'download-cache'
    $bootstrap = Join-Path $PSScriptRoot '.bootstrap/python'
    $python = Join-Path $bootstrap 'python.exe'
    if (-not (Test-Path -LiteralPath $python)) {
        New-Item -ItemType Directory -Path $cache -Force | Out-Null
        $archive = Join-Path $cache $manifest.python.file
        if (-not (Test-Path -LiteralPath $archive)) {
            if ($Offline) { throw "Offline package missing: $archive" }
            [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
            Write-Host 'Downloading portable Python from python.org...'
            Invoke-WebRequest -Uri $manifest.python.url -OutFile ($archive + '.partial') -UseBasicParsing
            $downloadHash = (Get-FileHash -LiteralPath ($archive + '.partial') -Algorithm SHA256).Hash
            if ($downloadHash -ne $manifest.python.sha256) { throw 'Python download checksum mismatch.' }
            Move-Item -LiteralPath ($archive + '.partial') -Destination $archive
        }
        if ((Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash -ne $manifest.python.sha256) {
            throw 'Cached Python checksum mismatch. Obtain the matching official archive.'
        }
        New-Item -ItemType Directory -Path $bootstrap -Force | Out-Null
        Expand-Archive -LiteralPath $archive -DestinationPath $bootstrap -Force
    }
    if (-not $PublicHost -and $Action -eq 'install') {
        $interfaces = @(Get-NetIPConfiguration | Where-Object { $_.IPv4DefaultGateway -and $_.IPv4Address })
        if ($interfaces.Count -eq 1) { $PublicHost = [string]$interfaces[0].IPv4Address[0].IPAddress }
        else { throw 'Cannot choose one LAN address. Re-run with -PublicHost YOUR_COMPUTER_LAN_IP.' }
    }
    $env:PYTHONIOENCODING = 'utf-8'
    $deployArgs = @((Join-Path $PSScriptRoot 'server.py'), $Action, '--install-dir', $InstallDir)
    if ($Action -eq 'install') {
        $deployArgs += @('--host', $PublicHost, '--resource-version', $ResourceVersion, '--channel', $Channel)
        # Omitted ports let Python reuse saved values or select free ports on first install.
        if ($null -ne $WebPort) { $deployArgs += @('--web-port', [string]$WebPort) }
        if ($null -ne $GamePort) { $deployArgs += @('--game-port', [string]$GamePort) }
        if ($null -ne $DatabasePort) { $deployArgs += @('--db-port', [string]$DatabasePort) }
    }
    if ($Offline) { $deployArgs += '--offline' }
    if ($RestoreFile) { $deployArgs += @('--restore-file', $RestoreFile) }
    if ($ConfirmRestore) { $deployArgs += '--confirm-restore' }
    & $python @deployArgs
    if ($LASTEXITCODE -ne 0) { throw "Deployment command failed (exit $LASTEXITCODE)." }
    if ($OpenFirewall) {
        $statePath = Join-Path $InstallDir '.onepiece-server.json'
        $state = Get-Content -LiteralPath $statePath -Raw -Encoding UTF8 | ConvertFrom-Json
        $sha = [Security.Cryptography.SHA256]::Create()
        try { $suffix = ([BitConverter]::ToString($sha.ComputeHash([Text.Encoding]::UTF8.GetBytes([IO.Path]::GetFullPath($InstallDir))))).Replace('-', '').Substring(0, 12) }
        finally { $sha.Dispose() }
        $ruleName = "OnePiece-LAN-$suffix"
        $ports = @([string]$state.webPort, [string]$state.gamePort)
        $rule = Get-NetFirewallRule -Name $ruleName -ErrorAction SilentlyContinue
        if ($rule) {
            Set-NetFirewallRule -Name $ruleName -Enabled True -Profile Private -Direction Inbound -Action Allow -RemoteAddress LocalSubnet | Out-Null
            $rule | Get-NetFirewallPortFilter | Set-NetFirewallPortFilter -Protocol TCP -LocalPort $ports | Out-Null
        } else {
            New-NetFirewallRule -Name $ruleName -DisplayName 'OnePiece local game and resources' -Direction Inbound -Action Allow -Profile Private -Protocol TCP -LocalPort $ports -RemoteAddress LocalSubnet | Out-Null
        }
        Write-Host 'Private-network firewall access configured for game/web only; database stays on loopback.'
    }
} catch {
    Write-Host $_.Exception.Message -ForegroundColor Red
    exit 1
}
