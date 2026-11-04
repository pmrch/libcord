$ErrorActionPreference = "Stop"

if (-not (Test-Path build)) { New-Item -ItemType Directory -Path build | Out-Null }
if (-not (Test-Path lib)) { New-Item -ItemType Directory -Path lib | Out-Null }
if (-not (Test-Path bin)) { New-Item -ItemType Directory -Path bin | Out-Null }
if (-not (Test-Path asm)) { New-Item -ItemType Directory -Path asm | Out-Null }

$clFlags = "/nologo /std:c17 /O2 /GL /arch:AVX2 /W4 /Iinclude /Isrc\internal /D_CRT_SECURE_NO_WARNINGS /D_POSIX_C_SOURCE=200809L /DLOG_LEVEL=2"

# --- PHASE 1: Build the library ---
$libSources = Get-ChildItem -Path src -Filter *.c -Recurse | Where-Object { 
    $_.FullName -notmatch "\\posix\\" 
}

$libObjs = @()
$compileCommands = @()

Write-Host "Compiling library..." -ForegroundColor Cyan
foreach ($file in $libSources) {
    $relPath = $file.FullName.Replace("$PWD\", "")
    $objName = "$($file.BaseName).obj"
    $objPath = "build\lib_$objName"
    $libObjs += $objPath

    # We must create the asm folder structure to match the src structure if it's nested
    $asmDir = "asm\" + $file.DirectoryName.Replace("$PWD\src", "").TrimStart('\')
    if (-not (Test-Path $asmDir)) { New-Item -ItemType Directory -Path $asmDir -Force | Out-Null }
    
    # Construct the exact path for the .asm file
    $asmPath = "asm\" + $relPath.Replace("src\", "").Replace(".c", ".asm")
    if (-not (Test-Path (Split-Path $asmPath -Parent))) { New-Item -ItemType Directory -Path (Split-Path $asmPath -Parent) -Force | Out-Null }

    $compileCommands += [PSCustomObject]@{
        directory = $PWD.Path.Replace("\", "/")
        command   = "cl.exe $clFlags /c $($relPath.Replace('\', '/')) /Fo:build/lib_$objName"
        file      = $relPath.Replace("\", "/")
    }

    # /FAs enables listing, /Fa sets the explicit output path
    & cl.exe $clFlags.Split(' ') /FAs /Fa$asmPath /c $relPath /Fo:$objPath
    if ($LASTEXITCODE -ne 0) { throw "Compilation failed on $relPath" }
}

$compileCommands | ConvertTo-Json -Depth 5 | Out-File -Encoding utf8 compile_commands.json

Write-Host "Archiving library..." -ForegroundColor Cyan
& lib.exe /nologo /OUT:lib\cord.lib $libObjs
if ($LASTEXITCODE -ne 0) { throw "lib.exe failed" }

# --- PHASE 2: Build the test executable ---
Write-Host "Compiling and linking tests..." -ForegroundColor Cyan
& cl.exe $clFlags.Split(' ') tests\main.c /Fo:build\test_main.obj /link /LTCG /SUBSYSTEM:CONSOLE lib\cord.lib /OUT:bin\cdiscord.exe
if ($LASTEXITCODE -ne 0) { throw "Test link failed" }

Write-Host "Build Successful! Binary is at bin\cdiscord.exe" -ForegroundColor Green