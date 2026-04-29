param(
    [string]$QtRoot = "C:\msys64\ucrt64",
    [string]$BuildDir = "G:\RocoDex_qt\build",
    [string]$DeployDir = "G:\RocoDex_qt\release\RocoDexQt",
    [string]$OutputExe = "G:\RocoDex_qt\release\RocoDexQt.exe"
)

$ErrorActionPreference = "Stop"

$sevenZip = "C:\Program Files\7-Zip\7z.exe"
$compiler = Join-Path $QtRoot "bin\g++.exe"
$windres = Join-Path $QtRoot "bin\windres.exe"
$workDir = Join-Path $env:TEMP "RocoDexQt_SingleFile"
$archive = Join-Path $workDir "payload.zip"
$resourceScript = Join-Path $workDir "payload.rc"
$resourceObject = Join-Path $workDir "payload_res.o"
$listFile = Join-Path $workDir "files.txt"
$launcherSource = Join-Path (Split-Path -Parent $PSScriptRoot) "launcher\RocoDexSingleFileLauncher.cpp"

if (!(Test-Path $sevenZip)) {
    throw "7-Zip not found: $sevenZip"
}

if (!(Test-Path $compiler)) {
    throw "g++ not found: $compiler"
}

if (!(Test-Path $windres)) {
    throw "windres not found: $windres"
}

if (!(Test-Path $launcherSource)) {
    throw "Launcher source not found: $launcherSource"
}

New-Item -ItemType Directory -Path $workDir -Force | Out-Null

$running = Get-Process RocoDexQt -ErrorAction SilentlyContinue
if ($running) {
    $running | Stop-Process -Force
}

& powershell -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot "deploy_windows.ps1") `
    -QtRoot $QtRoot `
    -BuildDir $BuildDir `
    -DeployDir $DeployDir

if ($LASTEXITCODE -ne 0) {
    throw "Portable release deployment failed"
}

if (Test-Path $OutputExe) {
    Remove-Item -LiteralPath $OutputExe -Force
}

if (Test-Path $archive) {
    Remove-Item -LiteralPath $archive -Force
}

foreach ($tempFile in @($resourceScript, $resourceObject, $listFile)) {
    if (Test-Path $tempFile) {
        Remove-Item -LiteralPath $tempFile -Force
    }
}

$cwd = Get-Location
try {
    Set-Location $DeployDir
    $fileList = Get-ChildItem -Recurse -File | ForEach-Object {
        $_.FullName.Substring($DeployDir.Length + 1)
    }
    if (@($fileList).Count -eq 0) {
        throw "No files found in deployed release folder: $DeployDir"
    }

    Set-Content -LiteralPath $listFile -Value ($fileList | Sort-Object) -Encoding UTF8
    & $sevenZip a -tzip -mx=9 -mmt=on -scsUTF-8 $archive "@$listFile"
    if ($LASTEXITCODE -ne 0) {
        throw "zip archive creation failed"
    }
}
finally {
    Set-Location $cwd
}

$archiveForRc = $archive.Replace('\', '\\')
$resourceText = @"
#define IDR_PAYLOAD 101
IDR_PAYLOAD RCDATA "$archiveForRc"
"@
Set-Content -LiteralPath $resourceScript -Value $resourceText -Encoding ASCII

& $windres -O coff -o $resourceObject $resourceScript
if ($LASTEXITCODE -ne 0) {
    throw "windres resource compilation failed"
}

$compileArgs = @(
    "-std=c++17",
    "-O2",
    "-finput-charset=UTF-8",
    "-municode",
    "-mwindows",
    "-static",
    "-static-libgcc",
    "-static-libstdc++",
    "-DUNICODE",
    "-D_UNICODE",
    $launcherSource,
    $resourceObject,
    "-o",
    $OutputExe,
    "-luser32",
    "-lgdi32"
)

& $compiler @compileArgs
if ($LASTEXITCODE -ne 0) {
    throw "launcher compilation failed"
}

Write-Host "Created single-file launcher: $OutputExe"
Write-Host "Runtime extraction directory: <exe directory>\RocoDexQt_runtime\app"
