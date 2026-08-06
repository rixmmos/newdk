# Darkeden Changelog

## 2026.04.30.1605 - 2026-04-30T13:05:55Z

### Notes
- Harden launcher update path validation and publish latest map metadata alignment.

### Files
- Added: 1
- Changed: 2
- Removed: 0

### Changed Files
- DarkEdenLauncher.ps1
- Data/Info/Zone.inf

### Added Files
- Data/Info/Zone.inf.backup_map_audit_20260430_132106


## 2026.04.30.0907 - 2026-04-30T06:07:46Z

### Notes
- Fix ingame chat Backspace handling for the Windows SDL client.

### Files
- Added: 0
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe


## 2026.04.29.2302 - 2026-04-29T20:02:26Z

### Notes
- Remove temporary client trace log writers after confirming FHD monster visibility fix.

### Files
- Added: 0
- Changed: 3
- Removed: 0

### Changed Files
- Darkeden.exe
- DarkEdenResolution.cfg
- Data/Info/Player.inf


## 2026.04.29.2212 - 2026-04-29T19:12:59Z

### Notes
- Fix FHD monster visibility by expanding client sight cleanup window.

### Files
- Added: 0
- Changed: 2
- Removed: 0

### Changed Files
- Darkeden.exe
- Data/Info/Player.inf


## 2026.04.29.1802 - 2026-04-29T15:02:58Z

### Notes
- Extend FHD monster visibility diagnostics with client zone add-result trace.

### Files
- Added: 0
- Changed: 2
- Removed: 0

### Changed Files
- Darkeden.exe
- Data/Info/Player.inf


## 2026.04.29.1646 - 2026-04-29T13:46:45Z

### Notes
- Add creature packet trace for FHD monster visibility investigation.

### Files
- Added: 0
- Changed: 2
- Removed: 0

### Changed Files
- Darkeden.exe
- Data/Info/Player.inf


## 2026.04.29.1310 - 2026-04-29T10:10:23Z

### Notes
- Add detailed FHD creature visibility diagnostics for map, monster draw, and offscreen position tracking.

### Files
- Added: 0
- Changed: 3
- Removed: 0

### Changed Files
- Darkeden.exe
- DarkEdenResolution.cfg
- Data/Info/Player.inf


## 2026.04.29.0911 - 2026-04-29T06:11:45Z

### Notes
- Add FHD creature visibility diagnostics to track draw, darkness skip, and offscreen counts.

### Files
- Added: 0
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe


## 2026.04.29.0123 - 2026-04-28T22:23:15Z

### Notes
- Cull offscreen static image objects during world rendering and keep DrawZone timing logs for QHD performance tuning.

### Files
- Added: 0
- Changed: 2
- Removed: 0

### Changed Files
- Darkeden.exe
- Data/Info/Player.inf


## 2026.04.29.0045 - 2026-04-28T21:45:34Z

### Notes
- Add detailed DrawZone timing to locate the QHD world-rendering bottleneck.

### Files
- Added: 0
- Changed: 2
- Removed: 0

### Changed Files
- Darkeden.exe
- Data/Info/Player.inf


## 2026.04.29.0009 - 2026-04-28T21:09:11Z

### Notes
- Add lightweight render phase timing to diagnose QHD frame-rate bottlenecks.

### Files
- Added: 0
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe


## 2026.04.28.2344 - 2026-04-28T20:44:45Z

### Notes
- Improve QHD world rendering by skipping daylight light-filter full-screen copies and restoring the normal tile cache margin.

### Files
- Added: 0
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe


## 2026.04.28.2321 - 2026-04-28T20:21:54Z

### Notes
- Smooth world scrolling by using real render-time interpolation and a wider tile-surface cache margin.

### Files
- Added: 0
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe


## 2026.04.28.2300 - 2026-04-28T20:01:03Z

### Notes
- Restore forward movement interpolation and force full game redraws every render frame so world scrolling can update at 60 FPS.

### Files
- Added: 0
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe


## 2026.04.28.2235 - 2026-04-28T19:35:16Z

### Notes
- Tune 60 FPS movement interpolation to blend from the previous tick into the current tick, reducing camera/world rubberbanding.

### Files
- Added: 0
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe


## 2026.04.28.2207 - 2026-04-28T19:07:13Z

### Notes
- Add visual movement interpolation between legacy gameplay ticks for smoother 60 FPS rendering.

### Files
- Added: 0
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe


## 2026.04.28.2135 - 2026-04-28T18:35:42Z

### Notes
- Cap rendering at 60 FPS while keeping the legacy gameplay update tick unchanged.

### Files
- Added: 0
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe


## 2026.04.28.2037 - 2026-04-28T17:38:07Z

### Notes
- Make client DPI-aware and auto-select FHD/QHD safely based on the player's desktop resolution.

### Files
- Added: 0
- Changed: 2
- Removed: 0

### Changed Files
- Darkeden.exe
- DarkEdenResolution.cfg


## 2026.04.28.1319 - 2026-04-28T10:20:00Z

### Notes
- Fix QHD creature and NPC visibility after player movement by matching the client object-retention radius to the expanded server viewport.

### Files
- Added: 0
- Changed: 3
- Removed: 0

### Changed Files
- Darkeden.exe
- DarkEdenResolution.cfg
- Data/Info/Player.inf


## 2026.04.28.0020 - 2026-04-27T21:20:54Z

### Notes
- Add config-based resolution selection for FHD and QHD. DarkEdenResolution.cfg now controls whether the client starts at 1920x1080 or 2560x1440, and server vision was expanded for QHD.

### Files
- Added: 1
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe

### Added Files
- DarkEdenResolution.cfg


## 2026.04.27.2205 - 2026-04-27T19:05:31Z

### Notes
- Show the startup splash for two seconds and fade smoothly into the login screen without an extra flip/flicker.

### Files
- Added: 0
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe


## 2026.04.27.2103 - 2026-04-27T18:03:54Z

### Notes
- Replace startup splash screen with the new DarkEden artwork and hold it for one second before the login screen appears.

### Files
- Added: 1
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe

### Added Files
- Data/Image/StartupSplash.bmp


## 2026.04.27.2023 - 2026-04-27T17:23:20Z

### Notes
- Set client to a 1920x1080 FHD baseline and expand server vision width so monsters, NPCs, corpses, and items populate the wider viewport.

### Files
- Added: 0
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe


## 2026.04.27.1648 - 2026-04-27T13:48:34Z

### Notes
- New character name field now blocks invalid characters while typing; only English letters can be entered, with backspace/control keys still working.

### Files
- Added: 0
- Changed: 1
- Removed: 0

### Changed Files
- Darkeden.exe


## 2026.04.27.1610 - 2026-04-27T13:10:47Z

### Notes
- Add automatic changelog generation for every release.

### Files
- Added: 0
- Changed: 0
- Removed: 0


























