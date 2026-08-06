param(
    [Parameter(Mandatory = $true)]
    [string]$UpdateBaseUrl,
    [string]$InstallDir = (Join-Path $PSScriptRoot "game"),
    [string]$ManifestName = "manifest.json",
    [switch]$NoLaunch
)

$ErrorActionPreference = "Stop"

function Join-Url {
    param(
        [string]$BaseUrl,
        [string]$RelativePath
    )

    return $BaseUrl.TrimEnd('/') + '/' + $RelativePath.TrimStart('/').Replace('\', '/')
}

function Resolve-ManifestTargetPath {
    param(
        [string]$InstallDir,
        [string]$ManifestPath
    )

    if ([string]::IsNullOrWhiteSpace($ManifestPath)) {
        throw "Manifest contains an empty file path."
    }

    $normalized = $ManifestPath.Replace('/', '\').Trim()
    if ([IO.Path]::IsPathRooted($normalized)) {
        throw "Manifest contains a rooted file path: $ManifestPath"
    }

    $parts = $normalized -split '[\\]+'
    if ($parts | Where-Object { $_ -eq "." -or $_ -eq ".." -or [string]::IsNullOrWhiteSpace($_) }) {
        throw "Manifest contains an unsafe file path: $ManifestPath"
    }

    $base = [IO.Path]::GetFullPath($InstallDir).TrimEnd('\') + '\'
    $target = [IO.Path]::GetFullPath((Join-Path $base $normalized))
    if (-not $target.StartsWith($base, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Manifest file path escapes install directory: $ManifestPath"
    }

    return $target
}

function Read-JsonFile {
    param([string]$Path)

    if (-not (Test-Path -LiteralPath $Path)) {
        return $null
    }

    return Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
}

function Test-FileMatchesManifest {
    param(
        [string]$Path,
        [string]$Sha256,
        [long]$Size
    )

    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        return $false
    }

    $item = Get-Item -LiteralPath $Path
    if ($item.Length -ne $Size) {
        return $false
    }

    $hash = Get-FileHash -Algorithm SHA256 -LiteralPath $Path
    return $hash.Hash.Equals($Sha256, [System.StringComparison]::OrdinalIgnoreCase)
}

New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null

$remoteManifestUrl = Join-Url -BaseUrl $UpdateBaseUrl -RelativePath $ManifestName
$localManifestPath = Join-Path $InstallDir $ManifestName
$downloadedManifestPath = Join-Path $env:TEMP ("darkeden-manifest-" + [Guid]::NewGuid().ToString("N") + ".json")

Write-Host "Checking for updates..."
Invoke-WebRequest -Uri $remoteManifestUrl -OutFile $downloadedManifestPath
$remoteManifest = Read-JsonFile -Path $downloadedManifestPath
$localManifest = Read-JsonFile -Path $localManifestPath

$remoteVersion = [string]$remoteManifest.version
$localVersion = if ($null -ne $localManifest) { [string]$localManifest.version } else { "<none>" }
Write-Host "  Local:  $localVersion"
Write-Host "  Remote: $remoteVersion"

$updated = 0
$filesBaseUrl = if ($remoteManifest.filesBaseUrl) { [string]$remoteManifest.filesBaseUrl } else { "files/" }

foreach ($file in $remoteManifest.files) {
    $relativePath = ([string]$file.path).Replace('/', '\')
    $targetPath = Resolve-ManifestTargetPath -InstallDir $InstallDir -ManifestPath ([string]$file.path)

    if (Test-FileMatchesManifest -Path $targetPath -Sha256 ([string]$file.sha256) -Size ([long]$file.size)) {
        continue
    }

    $fileUrl = Join-Url -BaseUrl $UpdateBaseUrl -RelativePath (Join-Url -BaseUrl $filesBaseUrl -RelativePath ([string]$file.path))
    $tempPath = $targetPath + ".download"

    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $targetPath) | Out-Null
    Write-Host "  Updating $relativePath"
    Invoke-WebRequest -Uri $fileUrl -OutFile $tempPath

    if (-not (Test-FileMatchesManifest -Path $tempPath -Sha256 ([string]$file.sha256) -Size ([long]$file.size))) {
        Remove-Item -LiteralPath $tempPath -Force -ErrorAction SilentlyContinue
        throw "Downloaded file failed hash check: $relativePath"
    }

    Move-Item -LiteralPath $tempPath -Destination $targetPath -Force
    $updated++
}

Copy-Item -LiteralPath $downloadedManifestPath -Destination $localManifestPath -Force
Remove-Item -LiteralPath $downloadedManifestPath -Force -ErrorAction SilentlyContinue

Write-Host "Update complete. Changed files: $updated"

if ($NoLaunch) {
    exit 0
}

$entrypoint = if ($remoteManifest.entrypoint) { [string]$remoteManifest.entrypoint } else { "DarkEden.exe" }
$exePath = Join-Path $InstallDir $entrypoint
if (-not (Test-Path -LiteralPath $exePath -PathType Leaf)) {
    throw "Entrypoint not found after update: $exePath"
}

Write-Host "Launching $entrypoint..."
Start-Process -FilePath $exePath -WorkingDirectory $InstallDir
