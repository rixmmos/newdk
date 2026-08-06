param(
    # Resolve from this script's location (dkrix\tools\launcher\ -> dkrix root).
    [string]$BuildBin = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..\build\bin\Debug')),
    [string]$PublishDir = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..\publish\client')),
    [string]$Version = "",
    [string[]]$IncludePaths = @(),
    [string[]]$ExtraExecutables = @(),
    [switch]$IncludePdb
)

$ErrorActionPreference = "Stop"

function Get-RelativePath {
    param(
        [string]$BasePath,
        [string]$FullPath
    )

    $baseUri = [Uri]((Resolve-Path -LiteralPath $BasePath).Path.TrimEnd('\') + '\')
    $fullUri = [Uri](Resolve-Path -LiteralPath $FullPath).Path
    return [Uri]::UnescapeDataString($baseUri.MakeRelativeUri($fullUri).ToString()).Replace('/', '\')
}

function Copy-TreeContent {
    param(
        [string]$Source,
        [string]$Destination
    )

    if (-not (Test-Path -LiteralPath $Source)) {
        Write-Warning "Skipping missing include path: $Source"
        return
    }

    if (Test-Path -LiteralPath $Source -PathType Leaf) {
        New-Item -ItemType Directory -Force -Path $Destination | Out-Null
        Copy-Item -LiteralPath $Source -Destination (Join-Path $Destination (Split-Path -Leaf $Source)) -Force
        return
    }

    Get-ChildItem -LiteralPath $Source -Recurse -File | ForEach-Object {
        $relative = Get-RelativePath -BasePath $Source -FullPath $_.FullName
        $target = Join-Path $Destination $relative
        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $target) | Out-Null
        Copy-Item -LiteralPath $_.FullName -Destination $target -Force
    }
}

if (-not (Test-Path -LiteralPath $BuildBin)) {
    throw "Build output not found: $BuildBin"
}

if ([string]::IsNullOrWhiteSpace($Version)) {
    $Version = Get-Date -Format "yyyyMMdd-HHmmss"
}

$payloadDir = Join-Path $PublishDir "files"
if (Test-Path -LiteralPath $PublishDir) {
    Remove-Item -LiteralPath $PublishDir -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $payloadDir | Out-Null

$entrypoint = Join-Path $BuildBin "DarkEden.exe"
if (-not (Test-Path -LiteralPath $entrypoint -PathType Leaf)) {
    throw "DarkEden.exe not found in build output: $BuildBin"
}

Copy-Item -LiteralPath $entrypoint -Destination (Join-Path $payloadDir "DarkEden.exe") -Force

foreach ($exeName in $ExtraExecutables) {
    $exePath = Join-Path $BuildBin $exeName
    if (-not (Test-Path -LiteralPath $exePath -PathType Leaf)) {
        Write-Warning "Skipping missing executable: $exePath"
        continue
    }

    Copy-Item -LiteralPath $exePath -Destination (Join-Path $payloadDir (Split-Path -Leaf $exePath)) -Force
}

$extensions = @("*.dll", "*.json", "*.inf", "*.ini", "*.cfg", "*.conf")
foreach ($extension in $extensions) {
    Get-ChildItem -LiteralPath $BuildBin -Filter $extension -File | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $payloadDir $_.Name) -Force
    }
}

if ($IncludePdb) {
    Get-ChildItem -LiteralPath $BuildBin -Filter "*.pdb" -File | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $payloadDir $_.Name) -Force
    }
}

foreach ($includePath in $IncludePaths) {
    $resolved = Resolve-Path -LiteralPath $includePath -ErrorAction SilentlyContinue
    if ($null -eq $resolved) {
        Write-Warning "Skipping missing include path: $includePath"
        continue
    }

    $target = Join-Path $payloadDir (Split-Path -Leaf $resolved.Path)
    Copy-TreeContent -Source $resolved.Path -Destination $target
}

$files = Get-ChildItem -LiteralPath $payloadDir -Recurse -File | Sort-Object FullName | ForEach-Object {
    $relative = Get-RelativePath -BasePath $payloadDir -FullPath $_.FullName
    $hash = Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName
    [ordered]@{
        path = $relative.Replace('\', '/')
        size = $_.Length
        sha256 = $hash.Hash.ToLowerInvariant()
    }
}

$manifest = [ordered]@{
    version = $Version
    generatedAtUtc = (Get-Date).ToUniversalTime().ToString("o")
    entrypoint = "DarkEden.exe"
    filesBaseUrl = "files/"
    files = @($files)
}

$manifestPath = Join-Path $PublishDir "manifest.json"
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $manifestPath -Encoding UTF8

Write-Host "Published Dark Eden client package:"
Write-Host "  Version:  $Version"
Write-Host "  Files:    $($files.Count)"
Write-Host "  Folder:   $PublishDir"
Write-Host "  Manifest: $manifestPath"
