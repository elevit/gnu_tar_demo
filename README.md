# Licensed GNU tar C++ Demo (Windows)

This from-scratch C++17 project launches a separately installed GNU tar process
to create, list, and extract a `.tar.gz` archive.

GNU tar is licensed under GPLv3-or-later. It does **not** require a paid license,
activation key, or license server. This project does not bundle or link GNU tar;
the user supplies the executable. See `THIRD_PARTY_NOTICES.md`.

## Requirements

- Windows 10 or 11
- Visual Studio 2022 with **Desktop development with C++** and CMake
- GNU tar, for example from MSYS2 or Git for Windows

## Build

```powershell
cmake -S . -B build
cmake --build build --config Release
```

## Run

Pass the exact GNU tar location to avoid accidentally selecting Windows BSD tar:

```powershell
.\build\Release\tar_demo.exe `
  --tar "C:\Program Files\Git\usr\bin\tar.exe" `
  --output ".\demo-output"
```

You can instead set `GNU_TAR` to the full executable path. At startup, confirm
the displayed version begins with `tar (GNU tar)`. The program then creates
sample files, builds `sample.tar.gz`, lists it, and extracts it.

Only extract trusted archives. Production code should validate archive members,
links, and output paths before extraction.

