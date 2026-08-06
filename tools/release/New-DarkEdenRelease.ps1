param(
    [string]$SourceDir = "C:\newdk\Darkeden",
    [string]$OutputDir = "C:\newdk\release_site\darkeden",
    [string]$Version = (Get-Date -Format "yyyy.MM.dd.HHmm"),
    [string]$FilesBaseUrl = "files/",
    [string]$Entrypoint = "Darkeden.exe",
    [string]$ChangelogPath = "C:\newdk\CHANGELOG.md",
    [string]$ReleaseNotes = ""
)

$ErrorActionPreference = "Stop"

function ConvertTo-RelativePath {
    param(
        [string]$BasePath,
        [string]$Path
    )

    $base = [IO.Path]::GetFullPath($BasePath).TrimEnd('\') + '\'
    $full = [IO.Path]::GetFullPath($Path)
    return $full.Substring($base.Length).Replace('\', '/')
}

function Test-ExcludedPath {
    param([string]$RelativePath)

    $normalized = $RelativePath.Replace('\', '/')
    $excludePrefixes = @(
        'CrashDumps/',
        'UserSet/',
        'Log/',
        'Temp/',
        'Update/'
    )
    $excludeNames = @(
        'manifest.json',
        'local_manifest.json',
        'RUN_LOCAL_CLIENT.cmd'
    )
    $excludeExtensions = @(
        '.log',
        '.dmp',
        '.tmp',
        '.download'
    )

    foreach ($prefix in $excludePrefixes) {
        if ($normalized.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase)) {
            return $true
        }
    }

    $name = [IO.Path]::GetFileName($normalized)
    foreach ($excludeName in $excludeNames) {
        if ($name.Equals($excludeName, [StringComparison]::OrdinalIgnoreCase)) {
            return $true
        }
    }

    $extension = [IO.Path]::GetExtension($normalized)
    foreach ($excludeExtension in $excludeExtensions) {
        if ($extension.Equals($excludeExtension, [StringComparison]::OrdinalIgnoreCase)) {
            return $true
        }
    }

    return $false
}

function ConvertTo-ManifestMap {
    param($Files)

    $map = @{}
    if ($null -eq $Files) {
        return $map
    }

    foreach ($file in $Files) {
        if ($null -ne $file.path) {
            $map[$file.path] = $file
        }
    }

    return $map
}

function New-ChangelogEntry {
    param(
        [string]$Version,
        [string]$GeneratedAt,
        [string]$ReleaseNotes,
        [object[]]$Added,
        [object[]]$Changed,
        [string[]]$Removed
    )

    $lines = New-Object System.Collections.Generic.List[string]
    $lines.Add("## $Version - $GeneratedAt")
    $lines.Add("")

    if (![string]::IsNullOrWhiteSpace($ReleaseNotes)) {
        $lines.Add("### Notes")
        foreach ($noteLine in ($ReleaseNotes -split "(`r`n|`n|`r)")) {
            if (![string]::IsNullOrWhiteSpace($noteLine)) {
                $lines.Add("- $noteLine")
            }
        }
        $lines.Add("")
    }

    $lines.Add("### Files")
    $lines.Add("- Added: $($Added.Count)")
    $lines.Add("- Changed: $($Changed.Count)")
    $lines.Add("- Removed: $($Removed.Count)")
    $lines.Add("")

    $maxListItems = 40
    if ($Changed.Count -gt 0) {
        $lines.Add("### Changed Files")
        foreach ($file in ($Changed | Select-Object -First $maxListItems)) {
            $lines.Add("- $($file.path)")
        }
        if ($Changed.Count -gt $maxListItems) {
            $lines.Add("- ...and $($Changed.Count - $maxListItems) more")
        }
        $lines.Add("")
    }

    if ($Added.Count -gt 0) {
        $lines.Add("### Added Files")
        foreach ($file in ($Added | Select-Object -First $maxListItems)) {
            $lines.Add("- $($file.path)")
        }
        if ($Added.Count -gt $maxListItems) {
            $lines.Add("- ...and $($Added.Count - $maxListItems) more")
        }
        $lines.Add("")
    }

    if ($Removed.Count -gt 0) {
        $lines.Add("### Removed Files")
        foreach ($path in ($Removed | Select-Object -First $maxListItems)) {
            $lines.Add("- $path")
        }
        if ($Removed.Count -gt $maxListItems) {
            $lines.Add("- ...and $($Removed.Count - $maxListItems) more")
        }
        $lines.Add("")
    }

    return ($lines -join [Environment]::NewLine)
}

$source = Resolve-Path -LiteralPath $SourceDir
$sourcePath = $source.Path
$outputPath = [IO.Path]::GetFullPath($OutputDir)
$filesPath = Join-Path $outputPath 'files'
$previousManifestPath = Join-Path $outputPath 'manifest.json'
$previousManifest = $null

if (Test-Path -LiteralPath $previousManifestPath) {
    $previousManifest = Get-Content -LiteralPath $previousManifestPath -Raw | ConvertFrom-Json
}

if ($outputPath.StartsWith($sourcePath, [StringComparison]::OrdinalIgnoreCase)) {
    throw "OutputDir must not be inside SourceDir."
}

if (Test-Path -LiteralPath $outputPath) {
    Remove-Item -LiteralPath $outputPath -Recurse -Force
}

New-Item -ItemType Directory -Force -Path $filesPath | Out-Null

$manifestFiles = New-Object System.Collections.Generic.List[object]
$sourceFiles = Get-ChildItem -LiteralPath $sourcePath -Recurse -File | Sort-Object FullName

foreach ($file in $sourceFiles) {
    $relative = ConvertTo-RelativePath -BasePath $sourcePath -Path $file.FullName
    if (Test-ExcludedPath -RelativePath $relative) {
        continue
    }

    $targetPath = Join-Path $filesPath ($relative.Replace('/', '\'))
    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $targetPath) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $targetPath -Force

    $hash = Get-FileHash -Algorithm SHA256 -LiteralPath $file.FullName
    $manifestFiles.Add([ordered]@{
        path = $relative
        size = [int64]$file.Length
        sha256 = $hash.Hash.ToLowerInvariant()
    })
}

$manifest = [ordered]@{
    version = $Version
    generatedAt = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ssZ")
    entrypoint = $Entrypoint
    filesBaseUrl = $FilesBaseUrl
    changelog = "CHANGELOG.md"
    files = $manifestFiles
}

$manifestPath = Join-Path $outputPath 'manifest.json'
$manifest | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $manifestPath -Encoding UTF8

$previousFiles = ConvertTo-ManifestMap -Files $previousManifest.files
$currentFiles = ConvertTo-ManifestMap -Files $manifestFiles

$addedFiles = @()
$changedFiles = @()
$removedFiles = @()

foreach ($file in $manifestFiles) {
    if (!$previousFiles.ContainsKey($file.path)) {
        $addedFiles += $file
        continue
    }

    $previousFile = $previousFiles[$file.path]
    if ($previousFile.sha256 -ne $file.sha256 -or [int64]$previousFile.size -ne [int64]$file.size) {
        $changedFiles += $file
    }
}

foreach ($path in $previousFiles.Keys) {
    if (!$currentFiles.ContainsKey($path)) {
        $removedFiles += $path
    }
}

$entry = New-ChangelogEntry `
    -Version $Version `
    -GeneratedAt $manifest.generatedAt `
    -ReleaseNotes $ReleaseNotes `
    -Added $addedFiles `
    -Changed $changedFiles `
    -Removed $removedFiles

$existingChangelog = ""
if (Test-Path -LiteralPath $ChangelogPath) {
    $existingChangelog = Get-Content -LiteralPath $ChangelogPath -Raw
}

if ([string]::IsNullOrWhiteSpace($existingChangelog)) {
    $newChangelog = "# Darkeden Changelog" + [Environment]::NewLine + [Environment]::NewLine + $entry + [Environment]::NewLine
}
else {
    $newChangelog = "# Darkeden Changelog" + [Environment]::NewLine + [Environment]::NewLine + $entry + [Environment]::NewLine + [Environment]::NewLine + ($existingChangelog -replace '^\s*# Darkeden Changelog\s*', '').TrimStart()
}

Set-Content -LiteralPath $ChangelogPath -Value $newChangelog -Encoding UTF8
Copy-Item -LiteralPath $ChangelogPath -Destination (Join-Path $outputPath 'CHANGELOG.md') -Force

Write-Host "Release prepared."
Write-Host "  Version: $Version"
Write-Host "  Files:   $($manifestFiles.Count)"
Write-Host "  Output:  $outputPath"
Write-Host "  Manifest: $manifestPath"
Write-Host "  Changelog: $ChangelogPath"
