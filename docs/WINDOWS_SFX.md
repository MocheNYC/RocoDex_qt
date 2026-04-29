# Windows 单文件发布

当前 Qt 版不是静态链接 Qt，因此“真正纯单 exe、零外部 DLL”的方式不现实。这里提供的是更实用的单文件发布包方案：

- 把完整 `release\RocoDexQt\` 目录打进一个自解压 exe
- 启动时先解压到 `%TEMP%\RocoDexQt`
- 然后自动运行 `RocoDexQt.exe`

## 生成命令

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build_single_file_release.ps1
```

输出文件：

```text
G:\RocoDex_qt\release\RocoDexQt_SFX.exe
```

## 适用范围

- 适合直接发给别人双击运行
- 适合 Windows 单文件分发
- 适合现有动态 Qt 构建，不需要切换静态 Qt 工具链

## 说明

这个包里仍然包含 Qt 运行时、QML 模块、资源和图片，只是把它们封装进一个 exe 里。它和“静态链接的真单文件 exe”不是同一个东西，但对分发和使用已经足够接近。
