param(
    [Parameter(Mandatory = $true)]
    [string]$SourcePath
)

$ErrorActionPreference = 'Stop'

$workspaceRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$source = [System.IO.Path]::GetFullPath($SourcePath)

if ([System.IO.Path]::GetExtension($source) -ne '.cpp') {
    throw "The active file must be a .cpp file: $source"
}
if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
    throw "The active C++ file does not exist: $source"
}

$buildDirectory = Join-Path $workspaceRoot 'build'
& cmake -S $workspaceRoot -B $buildDirectory
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

$targetMappingsPath = Join-Path $buildDirectory 'cpp_targets.txt'
$matchingTargets = @()
foreach ($mapping in Get-Content -LiteralPath $targetMappingsPath) {
    $separator = $mapping.LastIndexOf("`t")
    if ($separator -lt 0) {
        continue
    }

    $mappedSource = [System.IO.Path]::GetFullPath($mapping.Substring(0, $separator))
    if ([System.String]::Equals(
            $mappedSource,
            $source,
            [System.StringComparison]::OrdinalIgnoreCase
        )) {
        $matchingTargets += $mapping.Substring($separator + 1)
    }
}

if ($matchingTargets.Count -eq 0) {
    throw "The active file is not part of a CMake target: $source"
}
if ($matchingTargets.Count -gt 1) {
    throw "The active file belongs to multiple CMake targets; cannot choose one automatically: $source"
}

$target = $matchingTargets[0]
& cmake --build $buildDirectory --target $target
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

$executable = Join-Path $buildDirectory "$target.exe"
if (-not (Test-Path -LiteralPath $executable -PathType Leaf)) {
    throw "The built target did not produce the expected executable: $executable"
}

& $executable
exit $LASTEXITCODE
