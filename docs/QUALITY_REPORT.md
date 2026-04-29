# Quality Report

## Current Completion

The Qt edition now covers the Windows desktop product path end to end:

- Offline dex browsing from `resources/data/dex-bundle.json`.
- 465 pet images loaded from `resources/offline/pets`.
- Search, attribute filters, stage filter, distribution-only toggle, and sorting.
- Favorite and recent pet persistence through `QSettings`.
- Detail page hero, stats, evolution, trait, body, distribution, intro, skills, source link, and matchup panel.
- Attribute offensive coverage and defensive weak/resistant/neutral calculations.
- GitHub raw JSON update with version/time/size comparison.
- Reset to embedded data.
- Windows deployment folder with Qt/QML/MSYS2 runtime dependencies.

## Explicitly Removed

The Qt edition intentionally has no RocoFight, PVP, battle simulator, MaskPPO, egg-group, breeding, egg-size, or rocomegg learner UI. The exporter removes unsupported fields, and the runtime parser ignores old extra fields.

## Validation Commands

```powershell
$env:PATH='C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe --build build --config Debug
C:\msys64\ucrt64\bin\ctest.exe --test-dir build --output-on-failure
powershell -ExecutionPolicy Bypass -File .\scripts\deploy_windows.ps1
```

Release smoke test:

```powershell
$deploy='G:\RocoDex_qt\release\RocoDexQt'
$env:PATH='C:\Windows\System32;C:\Windows'
Start-Process "$deploy\RocoDexQt.exe" -WorkingDirectory $deploy
```

## Remaining Product Risks

- The remote JSON update can refresh structured data, but newly added offline image files still require a rebuilt release folder unless a future image update package is added.
- Android layout and back behavior remain a later milestone.
- Public or commercial redistribution still needs a final review of upstream data, image, and wiki licensing terms.
