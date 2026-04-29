# Complete Workflow

## 1. Data Update In React Project

The existing React project remains the owner of upstream data collection.

```powershell
cd G:\rock_world
npm.cmd run data:import
npm.cmd run data:bundle
```

Do not run `data:supplement` for Qt-only needs unless the React app still needs it. The Qt exporter strips supplement data either way.

## 2. Export Qt Data

```powershell
cd G:\RocoDex_qt
node .\scripts\export_qt_data.mjs --source G:\rock_world --copy-images
```

Expected output:

```text
resources/data/dex-bundle.json
resources/offline/pets/*.png
```

The exported JSON has no `supplement` field.

## 3. Configure

```powershell
$env:PATH='C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64
```

Qt Creator can also open `CMakeLists.txt` directly.

## 4. Build

```powershell
C:\msys64\ucrt64\bin\cmake.exe --build build --config Debug
```

## 5. Test

```powershell
C:\msys64\ucrt64\bin\ctest.exe --test-dir build --output-on-failure
```

Minimum test coverage:

- Data bundle parser accepts current exported JSON.
- Search by id/name/initials.
- Attribute and stage filters.
- Sort by id/name/baseStats/speed/health.
- Evolution chain traversal.
- Attribute offensive coverage.
- Attribute defensive weak/resistant/neutral profile.
- Remote update comparison.

## 6. Run

```powershell
.\build\RocoDexQt.exe
```

## 7. Package Windows

```powershell
C:\msys64\ucrt64\bin\cmake.exe --build build --config Debug
powershell -ExecutionPolicy Bypass -File .\scripts\deploy_windows.ps1
```

The deployment script creates:

```text
G:\RocoDex_qt\release\RocoDexQt\RocoDexQt.exe
```

Use the whole `release\RocoDexQt` directory together. Do not copy the exe alone.

The deployment script copies Qt Quick modules, MSYS2/UCRT runtime DLLs, the app resources, and common D3D runtime files. A release validation should launch this exe with only the Windows system path available, so missing DLLs are caught before handoff.

## 8. Package Android

After desktop UI is stable:

```text
Qt Creator -> Android Kit -> Build APK
```

Android-specific requirements:

- Put JSON and images in deployable resources.
- Implement Android back behavior.
- Validate 360 px and 412 px portrait layouts.
- Check APK size after including pet images.

## Branching / Milestones

```text
milestone-0-docs-and-skeleton
milestone-1-desktop-mvp
milestone-2-full-dex-features
milestone-3-data-update
milestone-4-android
milestone-5-release-packaging
```

## Definition Of Done

For each feature:

- C++ logic has unit tests when it contains rules or transformations.
- QML has loading, empty, and error states.
- No RocoFight or egg-group references are introduced.
- Data exporter still produces supplement-free JSON.
- Desktop build runs.
- Release exe starts from the deployed folder without relying on the development PATH.
- The smallest supported mobile width has no overlap.
