$ErrorActionPreference = "Stop"

# ==========================================
# TOOLCHAIN CONFIGURATION
# ==========================================
$CC = "clang-cl.exe"
$LINKER = "lld-link"

# ==========================================
# DIRECTORIES
# ==========================================
$Dirs = @("build", "bin", "deps\donna")
foreach ($dir in $Dirs) {
    if (-not (Test-Path $dir)) { New-Item -ItemType Directory -Path $dir -Force | Out-Null }
}

# ==========================================
# 1. FETCH DONNA
# ==========================================
if (-not (Test-Path "deps\donna\curve25519_donna.c")) {
    Write-Host "Fetching curve25519-donna..." -ForegroundColor Cyan
    
    git clone --depth 1 https://github.com/agl/curve25519-donna.git temp_donna
    Copy-Item "temp_donna\curve25519-donna.c" -Destination "deps\donna\curve25519_donna.c"
    Remove-Item -Recurse -Force temp_donna
}
else {
    Write-Host "curve25519-donna already exists, skipping fetch." -ForegroundColor Yellow
}

# ==========================================
# 2. COMPILER & LINKER FLAGS
# ==========================================
$BaseFlags = @(
    "/nologo"
    "/clang:-fuse-ld=$LINKER"
)

# Test file needs to see the internal headers, the donna folder, and include the main lib
$CompileFlags = $BaseFlags + @(
    "/std:c17"
    "/W4"
    "/O2"
    "/clang:-O3"
    "-flto"
    "/arch:AVX2"                 # Switched from AVX512 to AVX2 for broader compatibility & better clock speeds
    "/wd4996"                    # Suppress deprecation warnings
    "/Iinclude"
    "/Isrc\internal"
    "/Ideps\donna"
    "/D_CRT_SECURE_NO_WARNINGS"
    "/DLOG_LEVEL=2"
    "/clang:-fomit-frame-pointer"
    "/clang:-fno-math-errno"
    "/clang:-mllvm"
    "/clang:-inline-threshold=1000"
)

$LinkFlags = @(
    "/nologo"
    "/LTCG"
    "/SUBSYSTEM:CONSOLE"
)

# ==========================================
# 3. COMPILE
# ==========================================
Write-Host "Compiling test_fe and donna with clang-cl..." -ForegroundColor Cyan

# Compile both source files to object files
& $CC @CompileFlags /c tests\test_fe.c /Fo:build\test_fe.obj
if ($LASTEXITCODE -ne 0) { throw "Compilation failed on test_fe.c" }

& $CC @CompileFlags /c deps\donna\curve25519_donna.c /Fo:build\curve25519_donna.obj
if ($LASTEXITCODE -ne 0) { throw "Compilation failed on curve25519_donna.c" }

# ==========================================
# 4. LINK
# ==========================================
Write-Host "Linking test executable..." -ForegroundColor Cyan

& $CC build\test_fe.obj build\curve25519_donna.obj /link @LinkFlags lib\cord.lib /OUT:bin\test_fe.exe
if ($LASTEXITCODE -ne 0) { throw "Link failed" }

Write-Host "Build Successful! Running tests..." -ForegroundColor Green
& .\bin\test_fe.exe