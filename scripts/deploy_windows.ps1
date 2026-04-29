param(
    [string]$QtRoot = "C:\msys64\ucrt64",
    [string]$BuildDir = "G:\RocoDex_qt\build",
    [string]$DeployDir = "G:\RocoDex_qt\release\RocoDexQt"
)

$ErrorActionPreference = "Stop"

$qtBin = Join-Path $QtRoot "bin"
$qtQml = Join-Path $QtRoot "share\qt6\qml"
$exe = Join-Path $BuildDir "RocoDexQt.exe"

if (!(Test-Path $exe)) {
    throw "Build output not found: $exe"
}

if (Test-Path $DeployDir) {
    Remove-Item -LiteralPath $DeployDir -Recurse -Force
}

New-Item -ItemType Directory -Path $DeployDir -Force | Out-Null
Copy-Item -LiteralPath $exe -Destination $DeployDir
Copy-Item -LiteralPath "G:\RocoDex_qt\resources" -Destination $DeployDir -Recurse

$env:PATH = "$qtBin;C:\msys64\usr\bin;$env:PATH"
& (Join-Path $qtBin "windeployqt6.exe") --qmldir "G:\RocoDex_qt\qml" (Join-Path $DeployDir "RocoDexQt.exe")

# windeployqt from MSYS2 can miss MinGW/UCRT transitive runtime libraries.
$ldd = "C:\msys64\usr\bin\ldd.exe"
for ($pass = 0; $pass -lt 8; $pass++) {
    $copied = 0
    $files = Get-ChildItem $DeployDir -Recurse -Include *.exe,*.dll -File
    foreach ($file in $files) {
        $deps = & $ldd $file.FullName 2>$null
        foreach ($line in $deps) {
            if ($line -match "=> /ucrt64/bin/([^ ]+)") {
                $name = $matches[1]
                $src = Join-Path $qtBin $name
                $dst = Join-Path $DeployDir $name
                if ((Test-Path $src) -and !(Test-Path $dst)) {
                    Copy-Item -LiteralPath $src -Destination $dst
                    $copied++
                }
            }
        }
    }

    if ($copied -eq 0) {
        break
    }
}

# Qt Quick on D3D can require these at runtime.
$dxCompiler = "C:\Program Files (x86)\Windows Kits\10\Redist\D3D\x64\dxcompiler.dll"
$dxil = "C:\Program Files (x86)\Windows Kits\10\Redist\D3D\x64\dxil.dll"
if (Test-Path $dxCompiler) {
    Copy-Item -LiteralPath $dxCompiler -Destination $DeployDir -Force
}
if (Test-Path $dxil) {
    Copy-Item -LiteralPath $dxil -Destination $DeployDir -Force
}

# Ensure Qt Quick Controls impl modules are present. Some MSYS2 windeployqt
# builds create the directories but do not copy all files.
$srcControls = Join-Path $qtQml "QtQuick\Controls"
$dstControls = Join-Path $DeployDir "qml\QtQuick\Controls"
Get-ChildItem $srcControls -Directory -Recurse |
    Where-Object { $_.Name -eq "impl" } |
    ForEach-Object {
        $relative = $_.FullName.Substring($srcControls.Length).TrimStart("\")
        $target = Join-Path $dstControls $relative
        New-Item -ItemType Directory -Path $target -Force | Out-Null
        Copy-Item -Path (Join-Path $_.FullName "*") -Destination $target -Recurse -Force
    }

Get-ChildItem $qtBin -Filter "Qt6QuickControls2*StyleImpl.dll" |
    ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $DeployDir $_.Name) -Force
    }

Write-Host "Deployed to $DeployDir"
