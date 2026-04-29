param(
    [string]$QtRoot = "C:\msys64\ucrt64",
    [string]$BuildDir = "G:\RocoDex_qt\build",
    [string]$DeployDir = "G:\RocoDex_qt\release\RocoDexQt",
    [string]$OutputExe = "G:\RocoDex_qt\release\RocoDexQt_SFX.exe"
)

$ErrorActionPreference = "Stop"

$sevenZip = "C:\Program Files\7-Zip\7z.exe"
$sfxStub = "C:\Program Files\7-Zip\7z.sfx"
$workDir = Join-Path $env:TEMP "RocoDexQt_SFX"
$archive = Join-Path $workDir "payload.7z"
$config = Join-Path $workDir "config.txt"
$listFile = Join-Path $workDir "files.txt"

if (!(Test-Path $sevenZip)) {
    throw "7-Zip not found: $sevenZip"
}

if (!(Test-Path $sfxStub)) {
    throw "7z SFX stub not found: $sfxStub"
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

if (Test-Path $config) {
    Remove-Item -LiteralPath $config -Force
}
if (Test-Path $listFile) {
    Remove-Item -LiteralPath $listFile -Force
}

$configText = @"
;!@Install@!UTF-8!
InstallPath="%TEMP%\RocoDexQt"
GUIMode="2"
OverwriteMode="2"
Delete="1"
RunProgram="RocoDexQt.exe"
;!@InstallEnd@!
"@

Set-Content -LiteralPath $config -Value $configText -Encoding UTF8

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
    & $sevenZip a -t7z -mx=9 -mmt=on -scsUTF-8 $archive "@$listFile"
    if ($LASTEXITCODE -ne 0) {
        throw "7z archive creation failed"
    }
}
finally {
    Set-Location $cwd
}

$outStream = [System.IO.File]::Create($OutputExe)
try {
    foreach ($part in @($sfxStub, $config, $archive)) {
        $inStream = [System.IO.File]::OpenRead($part)
        try {
            $inStream.CopyTo($outStream)
        }
        finally {
            $inStream.Dispose()
        }
    }
}
finally {
    $outStream.Dispose()
}

Write-Host "Created single-file package: $OutputExe"
