param(
    # Resolve from this script's location (dkrix\tools\launcher\ -> dkrix root).
    [string]$PublishDir = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..\publish\client')),
    [string]$Prefix = "http://127.0.0.1:8765/"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $PublishDir -PathType Container)) {
    throw "Publish folder not found: $PublishDir"
}

$listener = [System.Net.HttpListener]::new()
$listener.Prefixes.Add($Prefix)
$listener.Start()

Write-Host "Serving Dark Eden client updates:"
Write-Host "  Folder: $PublishDir"
Write-Host "  URL:    $Prefix"
Write-Host "Press Ctrl+C to stop."

try {
    while ($listener.IsListening) {
        $context = $listener.GetContext()
        $relativePath = [Uri]::UnescapeDataString($context.Request.Url.AbsolutePath.TrimStart('/')).Replace('/', '\')
        if ([string]::IsNullOrWhiteSpace($relativePath)) {
            $relativePath = "manifest.json"
        }

        $fullPath = Join-Path $PublishDir $relativePath
        $resolvedRoot = (Resolve-Path -LiteralPath $PublishDir).Path
        $resolvedFull = $null
        if (Test-Path -LiteralPath $fullPath -PathType Leaf) {
            $resolvedFull = (Resolve-Path -LiteralPath $fullPath).Path
        }

        if ($null -eq $resolvedFull -or -not $resolvedFull.StartsWith($resolvedRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
            $context.Response.StatusCode = 404
            $context.Response.OutputStream.Close()
            continue
        }

        $bytes = [System.IO.File]::ReadAllBytes($resolvedFull)
        $context.Response.StatusCode = 200
        $context.Response.ContentLength64 = $bytes.Length
        $context.Response.OutputStream.Write($bytes, 0, $bytes.Length)
        $context.Response.OutputStream.Close()
    }
}
finally {
    $listener.Stop()
    $listener.Close()
}
