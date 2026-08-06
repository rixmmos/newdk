# Dark Eden Launcher / Updater

This is the first Windows-friendly launcher pipeline. It uses a static
`manifest.json` plus file hashes so testers only download changed files.

## Build The Client

```powershell
& 'C:\Program Files\CMake\bin\cmake.exe' -S C:\newdk\dkrix -B C:\newdk\dkrix\build -G 'Visual Studio 17 2022' -A x64 -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
& 'C:\Program Files\CMake\bin\cmake.exe' --build C:\newdk\dkrix\build --config Debug -- /m:1
```

## Publish A Version

```powershell
powershell -ExecutionPolicy Bypass -File C:\newdk\dkrix\tools\launcher\package_client.ps1 -BuildBin C:\newdk\dkrix\build\bin\Debug -PublishDir C:\newdk\dkrix\publish\client -Version dev-001
```

Optional resource/data folders can be included with `-IncludePaths`:

```powershell
powershell -ExecutionPolicy Bypass -File C:\newdk\dkrix\tools\launcher\package_client.ps1 -BuildBin C:\newdk\dkrix\build\bin\Debug -PublishDir C:\newdk\dkrix\publish\client -Version dev-001 -IncludePaths C:\path\to\Data,C:\path\to\Sprite
```

## Test Locally

Serve the publish folder over HTTP:

```powershell
powershell -ExecutionPolicy Bypass -File C:\newdk\dkrix\tools\launcher\serve_publish.ps1 -PublishDir C:\newdk\dkrix\publish\client -Prefix http://127.0.0.1:8765/
```

In another PowerShell window, run the launcher:

```powershell
powershell -ExecutionPolicy Bypass -File C:\newdk\dkrix\tools\launcher\DarkEdenLauncher.ps1 -UpdateBaseUrl http://127.0.0.1:8765 -InstallDir C:\newdk\dkrix\test-install -NoLaunch
```

Remove `-NoLaunch` when the data/config files are ready and the game should
start automatically after updating.

## Friend/Test Machine

Give the tester `DarkEdenLauncher.ps1` and your update URL. They run:

```powershell
powershell -ExecutionPolicy Bypass -File .\DarkEdenLauncher.ps1 -UpdateBaseUrl http://YOUR-IP-OR-DOMAIN:8080 -InstallDir .\DarkEden
```

For real external testing, host `C:\newdk\dkrix\publish\client` on any static
HTTP server. The launcher does not require server-side code.

For quick LAN testing from this workstation, run PowerShell as Administrator
and serve on all interfaces:

```powershell
powershell -ExecutionPolicy Bypass -File C:\newdk\dkrix\tools\launcher\serve_publish.ps1 -PublishDir C:\newdk\dkrix\publish\client -Prefix http://+:8765/
```

The tester then uses your LAN IP:

```powershell
powershell -ExecutionPolicy Bypass -File .\DarkEdenLauncher.ps1 -UpdateBaseUrl http://YOUR-LAN-IP:8765 -InstallDir .\DarkEden
```
