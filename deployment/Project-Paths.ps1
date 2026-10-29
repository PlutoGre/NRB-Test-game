# Shared by entry points so double-clicking and launching from another cwd agree.
$script:NrbProjectRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))

function Resolve-NrbProjectPath {
    param([Parameter(Mandatory = $true)][string]$Path)
    if ([IO.Path]::IsPathRooted($Path)) {
        if ($Path -notmatch '^[A-Za-z]:[\\/]|^\\\\') { throw 'Drive-relative paths are ambiguous. Use a full path or a project-relative path.' }
        return [IO.Path]::GetFullPath($Path)
    }
    return [IO.Path]::GetFullPath((Join-Path $script:NrbProjectRoot $Path))
}
