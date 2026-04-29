# RocoDex Qt

RocoDex Qt 是 RocoDex 的独立 Qt 6 版本，目标是保留离线图鉴的核心体验，并用 Qt Quick/QML 重做高质量桌面与移动端界面。

本项目明确不包含：

- 蛋组、繁育、蛋尺寸、闪孵路线等 rocomegg 补充功能。
- RocoFight、PVP、战斗模拟、MaskPPO、训练桥接等战斗相关模块。

本项目保留并重做：

- 精灵基础资料、属性、种族值、进化链、特性、任务、分布和技能展示。
- 名称、编号、拼音首字母、英文名、分布、技能、属性关键词搜索。
- 属性、阶段、分布状态筛选。
- 编号、名称、种族值总和、速度、精力排序。
- 收藏、最近查看、本地离线图片、远端 JSON 数据更新。
- 单只精灵的属性进攻覆盖、防守弱点、防守抵抗、综合克制矩阵计算。

## Recommended Stack

```text
Qt 6.7+ / 6.8+
C++20
QML / Qt Quick Controls
CMake
Qt Test
```

## Directory Layout

```text
src/
  main.cpp
  models/                 Plain data types and value objects
  app/                    Repository, runtime, list model, update service
qml/
  Main.qml
  pages/                  Page-level views
  components/             Data manager, filters, list rows, matchup, skill cards
resources/
  data/                   Generated Qt data bundle, not committed by default
  offline/pets/           Offline pet images, not committed by default
scripts/
  export_qt_data.mjs      Strip unsupported fields from current RocoDex bundle
docs/
  PRODUCT_SCOPE.md
  ARCHITECTURE.md
  UI_UX_SPEC.md
  DATA_CONTRACT.md
  ATTRIBUTE_MATCHUP.md
  WORKFLOW.md
  IMPLEMENTATION_PLAN.md
tests/
  Unit tests for runtime/filter/matchup logic
```

## First Setup

From the current React RocoDex repository:

```powershell
cd G:\RocoDex_qt
node .\scripts\export_qt_data.mjs --source G:\rock_world --copy-images
```

Then configure and run with Qt Creator, or from command line.

Verified on this machine with MSYS2 UCRT64 Qt 6.10.1:

```powershell
$env:PATH='C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64
cmake --build build --config Debug
.\build\RocoDexQt.exe
```

If using the official Qt installer instead of MSYS2, adjust `CMAKE_PREFIX_PATH` to the local Qt installation. Keep the compiler ABI consistent with the Qt kit.

## Quality Gates

```powershell
$env:PATH='C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe --build build --config Debug
C:\msys64\ucrt64\bin\ctest.exe --test-dir build --output-on-failure
powershell -ExecutionPolicy Bypass -File .\scripts\deploy_windows.ps1
```

The minimum acceptance bar for each milestone is documented in [docs/WORKFLOW.md](docs/WORKFLOW.md).

## Windows Release

The packaged app is a portable folder, not a single-file executable:

```text
G:\RocoDex_qt\release\RocoDexQt\RocoDexQt.exe
```

Keep the whole `release\RocoDexQt` directory together. The folder contains Qt DLLs, MSYS2 runtime DLLs, QML modules, JSON data, and offline pet images.
