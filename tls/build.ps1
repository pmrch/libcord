<#
.SYNOPSIS
    Build script for libcord using the LLVM toolchain (clang-cl + lld).
.DESCRIPTION
    Compiles the library and tests using aggressive LLVM optimizations.
    Requires LLVM to be installed and in your PATH.
#>

param (
    [string]$TargetArch = "AVX512",
    [string]$LogLevel = "2",
    [switch]$RunTests
)

$ErrorActionPreference = "Stop"

# ==========================================
# TOOLCHAIN CONFIGURATION
# ==========================================
$CC = "clang-cl.exe"
$AR = "llvm-lib.exe"
$LINKER = "lld-link" # Passed via -fuse-ld

# ==========================================
# DIRECTORIES
# ==========================================
$Dirs = @("build", "lib", "bin", "asm")
foreach ($dir in $Dirs) {
    if (-not (Test-Path $dir)) { New-Item -ItemType Directory -Path $dir | Out-Null }
}

# ==========================================
# COMPILER & LINKER FLAGS
# ==========================================
# Base flags used for BOTH compiling and linking
$BaseFlags = @(
    "/nologo"
    "/clang:-fuse-ld=lld"  # Force LLVM linker
)

# Flags ONLY for the compiler
$CompileFlags = $BaseFlags + @(
    "/std:c17"
    "/W4"
    "/Iinclude"
    "/Isrc\internal"
    "/D_CRT_SECURE_NO_WARNINGS"
    "/D_POSIX_C_SOURCE=200809L"
    "/DLOG_LEVEL=$LogLevel"
    "/arch:$TargetArch"
    "/O2"
    "/clang:-O3"
    "-flto"
    "/clang:-fomit-frame-pointer"
    "/clang:-fno-math-errno"
    "/clang:-mllvm"
    "/clang:-inline-threshold=1000"
)

# Flags ONLY for the linker
$LinkFlags = @(
    "/nologo"
    "/LTCG"
    "/SUBSYSTEM:CONSOLE"
)

# ==========================================
# PHASE 1: BUILD THE LIBRARY
# ==========================================
Write-Host "Compiling library with $CC..." -ForegroundColor Cyan

$libSources = Get-ChildItem -Path src -Filter *.c -Recurse | Where-Object { 
    $_.FullName -notmatch "\\posix\\" 
}

$libObjs = @()
$compileCommands = @()

foreach ($file in $libSources) {
    $relPath = $file.FullName.Replace("$PWD\", "")
    $objName = "$($file.BaseName).obj"
    $objPath = "build\lib_$objName"
    $libObjs += $objPath

    # Recreate source directory structure in the asm folder
    $asmDir = "asm\" + $file.DirectoryName.Replace("$PWD\src", "").TrimStart('\')
    if (-not (Test-Path $asmDir)) { New-Item -ItemType Directory -Path $asmDir -Force | Out-Null }
    
    $asmPath = "asm\" + $relPath.Replace("src\", "").Replace(".c", ".asm")
    if (-not (Test-Path (Split-Path $asmPath -Parent))) { New-Item -ItemType Directory -Path (Split-Path $asmPath -Parent) -Force | Out-Null }

    # Build compile_commands.json entry
    $compileCommands += [PSCustomObject]@{
        directory = $PWD.Path.Replace("\", "/")
        command   = "$CC $($CompileFlags -join ' ') /c $($relPath.Replace('\', '/')) /Fo:$($objPath.Replace('\', '/'))"
        file      = $relPath.Replace('\', '/')
    }

    # Compile
    & $CC @CompileFlags /FAs /Fa$asmPath /c $relPath /Fo:$objPath
    if ($LASTEXITCODE -ne 0) { throw "Compilation failed on $relPath" }
}

$compileCommands | ConvertTo-Json -Depth 5 | Out-File -Encoding utf8 compile_commands.json

Write-Host "Archiving library with $AR..." -ForegroundColor Cyan
& $AR /nologo /OUT:lib\cord.lib @libObjs
if ($LASTEXITCODE -ne 0) { throw "$AR failed" }

# ==========================================
# PHASE 2: BUILD THE TEST EXECUTABLE
# ==========================================
Write-Host "Compiling and linking tests..." -ForegroundColor Cyan

# Step 1: Compile main.c (using CompileFlags)
& $CC @CompileFlags /c tests\main.c /Fo:build\test_main.obj
if ($LASTEXITCODE -ne 0) { throw "Compilation of main.c failed" }

# Step 2: Link (using LinkFlags, NO compile flags allowed here)
& $CC build\test_main.obj /link @LinkFlags lib\cord.lib /OUT:bin\cdiscord.exe
if ($LASTEXITCODE -ne 0) { throw "Test link failed" }

Write-Host "Build Successful! Binary is at bin\cdiscord.exe" -ForegroundColor Green

# ==========================================
# OPTIONAL: RUN TESTS
# ==========================================
if ($RunTests) {
    Write-Host "Running tests..." -ForegroundColor Yellow
    & ".\bin\cdiscord.exe"
    if ($LASTEXITCODE -ne 0) { throw "Tests failed" }
}