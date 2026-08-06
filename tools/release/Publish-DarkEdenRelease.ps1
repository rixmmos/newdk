param(
    [string]$RepoDir = "C:\newdk\dkrix",
    [string]$InstallDir = "C:\newdk\Darkeden",
    [string]$Version = (Get-Date -Format "yyyy.MM.dd.HHmm"),
    [string]$ReleaseNotes = "",
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"

function Resolve-RequiredPath {
    param([string]$Path)

    if (-not (Test-Path -LiteralPath $Path)) {
        throw "Required path does not exist: $Path"
    }

    return (Resolve-Path -LiteralPath $Path).Path
}

function Remove-IfInside {
    param(
        [string]$BaseDir,
        [string]$Path,
        [switch]$Recurse
    )

    $base = [IO.Path]::GetFullPath($BaseDir).TrimEnd('\') + '\'
    $full = [IO.Path]::GetFullPath($Path)

    if (-not $full.StartsWith($base, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to remove outside install dir: $full"
    }

    if (Test-Path -LiteralPath $full) {
        if ($Recurse) {
            Remove-Item -LiteralPath $full -Recurse -Force
        } else {
            Remove-Item -LiteralPath $full -Force
        }
    }
}

$repoPath = Resolve-RequiredPath $RepoDir
$installPath = Resolve-RequiredPath $InstallDir
$buildDir = Resolve-RequiredPath (Join-Path $repoPath "build")
$releaseBin = Join-Path $buildDir "bin\Release"
$publishLog = "C:\newdk\publish_release.log"

if (-not $SkipBuild) {
    Write-Host "Building Release..."
    & cmake --build $buildDir --config Release --target DarkEden -- /m:2 /v:minimal *> $publishLog
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Release build failed. See $publishLog"
        exit $LASTEXITCODE
    }
    Write-Host "Build OK. Log: $publishLog"
}

$releaseBinPath = Resolve-RequiredPath $releaseBin
$releaseExe = Resolve-RequiredPath (Join-Path $releaseBinPath "DarkEden.exe")

$artifactNames = @(
    'SDL2.dll',
    'SDL2_ttf.dll',
    'freetype.dll',
    'libpng16.dll',
    'zlib1.dll',
    'bz2.dll',
    'jpeg62.dll',
    'brotlicommon.dll',
    'brotlidec.dll'
)

Write-Host "Copying Release files..."
Copy-Item -LiteralPath $releaseExe -Destination (Join-Path $installPath "Darkeden.exe") -Force

foreach ($name in $artifactNames) {
    $source = Join-Path $releaseBinPath $name
    if (Test-Path -LiteralPath $source) {
        Copy-Item -LiteralPath $source -Destination (Join-Path $installPath $name) -Force
    }
}

$crtDir = Get-ChildItem -Path "C:\BuildTools\VC\Redist\MSVC" -Directory -ErrorAction SilentlyContinue |
    Sort-Object Name -Descending |
    ForEach-Object { Join-Path $_.FullName "x64\Microsoft.VC143.CRT" } |
    Where-Object { Test-Path -LiteralPath $_ } |
    Select-Object -First 1

if ($null -eq $crtDir) {
    throw "Could not find VC x64 runtime redist folder."
}

foreach ($name in @('vcruntime140.dll', 'vcruntime140_1.dll', 'msvcp140.dll')) {
    Copy-Item -LiteralPath (Join-Path $crtDir $name) -Destination (Join-Path $installPath $name) -Force
}

Write-Host "Cleaning debug leftovers and runtime logs..."
foreach ($name in @(
    'SDL2d.dll',
    'SDL2_ttfd.dll',
    'freetyped.dll',
    'libpng16d.dll',
    'zlibd1.dll',
    'bz2d.dll',
    'game_heartbeat.log',
    'login_flow_trace.log',
    'login_input_trace.log',
    'startup_trace.log',
    'combat_crash_marker.log',
    'interaction_trace.log',
    'zone_transition_trace.log',
    'pc_inventory_trace.log'
)) {
    Remove-IfInside -BaseDir $installPath -Path (Join-Path $installPath $name)
}

$crashDumpDir = Join-Path $installPath "CrashDumps"
if (Test-Path -LiteralPath $crashDumpDir) {
    Get-ChildItem -LiteralPath $crashDumpDir -File -ErrorAction SilentlyContinue | ForEach-Object {
        Remove-IfInside -BaseDir $installPath -Path $_.FullName
    }
}

Write-Host "Checking Release dependencies..."
$exeText = [Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes((Join-Path $installPath "Darkeden.exe")))
foreach ($debugNeedle in @('ucrtbased', 'VCRUNTIME140D', 'MSVCP140D', 'SDL2d.dll')) {
    if ($exeText.IndexOf($debugNeedle, [StringComparison]::OrdinalIgnoreCase) -ge 0) {
        throw "Release executable still references debug dependency: $debugNeedle"
    }
}

Write-Host "Generating launcher release manifest..."
$newRelease = Join-Path "C:\newdk\tools\release" "New-DarkEdenRelease.ps1"
& powershell -NoProfile -ExecutionPolicy Bypass -File $newRelease -SourceDir $installPath -Version $Version -ReleaseNotes $ReleaseNotes
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Host "Publish complete."
Write-Host "  Version: $Version"
Write-Host "  Install: $installPath"
Write-Host "  Release site: C:\newdk\release_site\darkeden"
