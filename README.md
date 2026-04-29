# RocoDex Qt

RocoDex Qt 是 RocoDex 的独立 Qt 6 桌面版本，目标是把离线图鉴做成一个完整、可直接运行、可直接打包分发的原生 Qt 工程。

这个仓库里已经包含了可用的本地资源包，不是只有源码壳子：

- `resources/data/dex-bundle.json`
- `resources/offline/pets/`

## 项目范围

保留并重做：

- 精灵基础资料、属性、种族值、进化链、特性、任务、分布和技能展示
- 名称、编号、拼音首字母、英文名、分布、技能、属性关键词搜索
- 属性、阶段、分布状态筛选
- 编号、名称、种族值总和、速度、精力排序
- 收藏、最近查看、本地离线图片、远端 JSON 数据更新
- 单只精灵的属性进攻覆盖、防守弱点、防守抵抗、综合克制矩阵计算

明确不包含：

- 蛋组、繁育、蛋尺寸、闪孵路线等 rocomegg 补充功能
- RocoFight、PVP、战斗模拟、MaskPPO、训练桥接等战斗相关模块

## 本地资源包

仓库中已经提交了 Qt 版离线资源：

- 数据包：`resources/data/dex-bundle.json`
- 离线图片：`resources/offline/pets/*.png`

这意味着你从 GitHub 拉下仓库后，可以直接拿到图鉴数据和图片资源，不需要先去别的项目再导出一遍。

## 构建环境

推荐环境：

- Qt 6.7+
- C++20
- QML / Qt Quick Controls
- CMake
- Qt Test

本机验证使用的是 MSYS2 UCRT64 Qt 6.10.1。

## 快速开始

### 1. 导出或刷新本地数据

如果你需要从上游项目重新导出 Qt 数据，可以运行：

```powershell
cd G:\RocoDex_qt
node .\scripts\export_qt_data.mjs --source G:\rock_world --copy-images
```

### 2. 配置与编译

```powershell
$env:PATH='C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64
C:\msys64\ucrt64\bin\cmake.exe --build build --config Debug
```

### 3. 运行

```powershell
.\build\RocoDexQt.exe
```

## Windows 发布

如果要生成可分发的 Windows 版本：

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\deploy_windows.ps1
```

发布目录会生成在：

```text
G:\RocoDex_qt\release\RocoDexQt\
```

这是一个便携目录，不是单文件 exe。`RocoDexQt.exe` 需要和同目录下的 Qt DLL、QML 模块、资源文件一起使用。

## 功能概览

- 左侧筛选栏：搜索、属性、阶段、分布状态、排序
- 中间列表：精灵卡片、收藏标识、快速结果数
- 右侧详情：头像、属性标签、种族值、进化链、特性、体型、分布、图鉴介绍、技能、数据来源
- 在线更新：从 GitHub raw 拉取 `dex-bundle.json`
- 本地恢复：随时切回内置数据

## 目录结构

```text
src/           C++ 后端、仓库、运行时、列表模型
qml/           Qt Quick 页面和组件
resources/     本地数据包与离线图片
scripts/       导出与发布脚本
tests/         单元测试
docs/          设计、工作流、数据契约与质量说明
```

## 验证项

- `cmake --build build --config Debug`
- `ctest --test-dir build --output-on-failure`
- 发布目录 exe 可脱离开发 PATH 启动
- 资源包已包含在仓库内

## 备注

当前 Qt 版是独立工程，和原 React 项目分开维护。Qt 版只保留图鉴与数据更新能力，不再包含战斗和蛋组相关模块。
