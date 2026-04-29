# Windows 单文件启动器

Qt 动态运行时无法真正合并成一个不落地任何依赖的纯单体程序，因此本项目采用更适合分发的单文件启动器方案：

- 用户只需要双击 `RocoDexQt.exe`
- 启动器内嵌完整的 Qt 发布包
- 首次启动会在启动器同目录创建 `RocoDexQt_runtime\app`
- 解包过程在后台静默完成，并由启动动画覆盖
- 后续启动会复用本地运行目录，版本不一致时自动重建

## 生成命令

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build_single_file_release.ps1
```

输出文件：

```text
G:\RocoDex_qt\release\RocoDexQt.exe
```

首次运行后会创建：

```text
G:\RocoDex_qt\release\RocoDexQt_runtime\app\RocoDexQt.exe
```

## 构建流程

1. 调用 `scripts\deploy_windows.ps1` 生成完整便携版目录。
2. 使用 7-Zip 将便携版目录打包成 `payload.zip`。
3. 使用 `windres` 把 `payload.zip` 编译成 Win32 资源。
4. 使用 MinGW `g++` 静态编译 `launcher\RocoDexSingleFileLauncher.cpp`。
5. 生成最终的 `release\RocoDexQt.exe`。

## 运行行为

启动器固定把运行文件放在 exe 同目录下的 `RocoDexQt_runtime` 文件夹，不会询问用户解压路径。解包、覆盖旧运行目录、启动主程序等操作都在后台完成；用户只会看到启动动画，然后进入 RocoDex Qt 主界面。

## 清理方式

如果需要恢复首次启动状态，删除下面目录即可：

```text
G:\RocoDex_qt\release\RocoDexQt_runtime
```
