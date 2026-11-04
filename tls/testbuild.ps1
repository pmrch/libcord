$ErrorActionPreference = "Stop"

# --- 1. Fetch Donna ---
if (-not (Test-Path "deps\donna\curve25519_donna.c")) {
    Write-Host "Fetching curve25519-donna..." -ForegroundColor Cyan
    if (-not (Test-Path "deps\donna")) { New-Item -ItemType Directory -Path "deps\donna" -Force | Out-Null }
    
    git clone --depth 1 https://github.com/agl/curve25519-donna.git temp_donna
    Copy-Item "temp_donna\curve25519-donna.c" -Destination "deps\donna\curve25519_donna.c"
    Remove-Item -Recurse -Force temp_donna
} else {
    Write-Host "curve25519-donna already exists, skipping fetch." -ForegroundColor Yellow
}

# --- 2. Compile Donna and Test ---
Write-Host "Compiling test_fe with donna..." -ForegroundColor Cyan

if (-not (Test-Path "build")) { New-Item -ItemType Directory -Path "build" | Out-Null }
if (-not (Test-Path "bin")) { New-Item -ItemType Directory -Path "bin" | Out-Null }

# Flags (added /DLOG_LEVEL=2 and /wd4996 to suppress warnings)
$clFlags = "/nologo /std:c17 /O2 /W4 /arch:AVX2 /wd4996 /Iinclude /Isrc\internal /Ideps\donna /D_CRT_SECURE_NO_WARNINGS /DLOG_LEVEL=2"

# Using direct invocation (&) to show linker errors
& cl.exe $clFlags.Split(' ') tests\test_fe.c deps\donna\curve25519_donna.c lib\cord.lib /Fe:bin\test_fe.exe /link /LTCG

if ($LASTEXITCODE -ne 0) { throw "Compilation failed!" }

Write-Host "Build Successful! Running tests..." -ForegroundColor Green
& .\bin\test_fe.exe