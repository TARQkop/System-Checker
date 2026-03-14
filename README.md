# System Checker

`System Checker` is a lightweight Windows console monitor written in C++ (OOP).
It shows live system status for CPU, RAM, Disk, and Network in a single terminal dashboard.

## Highlights

- Live CPU and memory usage with color-coded bars
- CPU details (logical cores, architecture)
- Disk monitoring with type, capacity, and usage
- Per-adapter network status and IP addresses
- Download/Upload rates calculated between refreshes
- Runtime control over refresh interval

## What It Shows

- CPU
- Logical processor count
- CPU architecture (`x64`, `x86`, `ARM64`, ...)
- CPU usage percentage with colored bar
- RAM
- Physical memory: total / used / free
- Commit memory: total / used / free
- Memory usage percentage with colored bar
- Disk
- All logical drives
- Drive type (`Fixed`, `Removable`, `Network`, `CD-ROM`, ...)
- Media estimation for fixed drives (`SSD`/`HDD` when supported)
- Capacity (total / used / free) + colored usage bar
- Network
- All adapters
- Adapter status (`Up`, `Down`, ...)
- IPv4 and IPv6 addresses
- Download/Upload rate per adapter (calculated between refreshes)

## Runtime Controls

- `r` refresh immediately
- `q` quit
- `+` increase refresh interval
- `-` decrease refresh interval

Default refresh interval is `60` seconds.  
Allowed range is `1` to `300` seconds.

## Color Rules

- `>= 80%` red
- `>= 75% and < 80%` yellow
- `< 75%` green

## Project Layout

```text
System_Checker.cpp              # main()
include/
  ConsoleRenderer.h
  CpuMonitor.h
  RamMonitor.h
  DiskMonitor.h
  NetworkMonitor.h
  SystemCheckerApp.h
  WsaSession.h
src/
  ConsoleRenderer.cpp
  CpuMonitor.cpp
  RamMonitor.cpp
  DiskMonitor.cpp
  NetworkMonitor.cpp
  SystemCheckerApp.cpp
  WsaSession.cpp
.vscode/tasks.json
```

## Requirements

- Windows
- MSYS2 UCRT64 (or compatible MinGW-w64 toolchain)
- `g++`
- Win32 libs:
  - `ws2_32`
  - `iphlpapi`

## Build (VS Code)

Use task:
- `C/C++: g++.exe build active file`

The task already compiles all `.cpp` files and outputs:
- `System_Checker.exe`

## Build (Terminal)

```powershell
C:\msys64\ucrt64\bin\g++.exe -fdiagnostics-color=always -g `
  -I "include" `
  "System_Checker.cpp" `
  "src\SystemCheckerApp.cpp" `
  "src\ConsoleRenderer.cpp" `
  "src\CpuMonitor.cpp" `
  "src\RamMonitor.cpp" `
  "src\DiskMonitor.cpp" `
  "src\NetworkMonitor.cpp" `
  "src\WsaSession.cpp" `
  -lws2_32 -liphlpapi `
  -o "System_Checker.exe"
```

## Run

```powershell
.\System_Checker.exe
```

## Important Notes

- The first frame may show `Calculating...` for CPU or network rate until at least two samples exist.
- Network speed is derived from adapter counters over elapsed time between samples.
- Media detection (`SSD`/`HDD`) depends on driver support; some drives return `Unknown`.
- Network throughput currently uses `GetIfEntry` counters (`dwInOctets`, `dwOutOctets`), which are 32-bit counters; on very high traffic links, counter wrap can reduce accuracy over longer intervals.

## Current Design

- OOP structure with separate monitor classes per subsystem.
- `WsaSession` is RAII-based for `WSAStartup` / `WSACleanup`.
- `ConsoleRenderer` owns console formatting concerns (sections, header, color, bars).

## Suggested Next Improvements

- Switch network throughput to `GetIfEntry2` (64-bit counters) when fully available in your build environment.
- Add optional logging mode (`.csv` / `.json`).
- Add sensor plugins (CPU/GPU/drive temperature).
- Add unit tests for pure utility logic (formatting, bar generation, thresholds).
