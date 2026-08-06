param(
    [string]$RootDir = "C:\newdk\release_site",
    [int]$Port = 8080,
    [string[]]$AllowedIP = @("127.0.0.1", "::1")
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $RootDir -PathType Container)) {
    throw "Release site folder does not exist: $RootDir"
}

$python = (Get-Command python -ErrorAction Stop).Source
$serverScript = Join-Path $PSScriptRoot "serve_release_site.py"
if (-not (Test-Path -LiteralPath $serverScript -PathType Leaf)) {
    throw "Update server script does not exist: $serverScript"
}

Write-Host "Serving $RootDir"
Write-Host "URL: http://0.0.0.0:$Port/"
Write-Host "Public manifest: http://90.190.31.134:$Port/darkeden/manifest.json"
Write-Host "Allowed IPs: $($AllowedIP -join ', ')"
Write-Host "Press Ctrl+C to stop."

$args = @($serverScript, "--root", $RootDir, "--port", $Port, "--bind", "0.0.0.0")
foreach ($ip in $AllowedIP) {
    if (-not [string]::IsNullOrWhiteSpace($ip)) {
        $args += @("--allowed-ip", $ip)
    }
}

& $python @args
