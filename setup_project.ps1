# File Management gRPC Project - Setup Script
# Run this script in PowerShell as Administrator (if installing vcpkg globally)

param(
    [switch]$SkipVcpkg,
    [switch]$BuildOnly,
    [string]$VcpkgPath = ""
)

$ErrorActionPreference = "Stop"
$ProjectRoot = $PSScriptRoot

Write-Host "============================================" -ForegroundColor Cyan
Write-Host "  gRPC File Management Project Setup" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

# Function to check if a command exists
function Test-Command {
    param($Command)
    try {
        Get-Command $Command -ErrorAction Stop
        return $true
    } catch {
        return $false
    }
}

# Function to find Visual Studio installation
function Find-VisualStudio {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vswhere) {
        $vsPath = & $vswhere -latest -property installationPath
        return $vsPath
    }
    return $null
}

# Step 1: Check Prerequisites
Write-Host "[Step 1/5] Checking prerequisites..." -ForegroundColor Yellow

# Check for CMake
if (Test-Command "cmake") {
    $cmakeVersion = cmake --version | Select-Object -First 1
    Write-Host "  ✓ CMake found: $cmakeVersion" -ForegroundColor Green
} else {
    Write-Host "  ✗ CMake not found!" -ForegroundColor Red
    Write-Host "    Install CMake from https://cmake.org/download/" -ForegroundColor Yellow
    Write-Host "    Or run: winget install Kitware.CMake" -ForegroundColor Yellow
    exit 1
}

# Check for Git
if (Test-Command "git") {
    Write-Host "  ✓ Git found" -ForegroundColor Green
} else {
    Write-Host "  ✗ Git not found!" -ForegroundColor Red
    Write-Host "    Install Git from https://git-scm.com/download/win" -ForegroundColor Yellow
    exit 1
}

# Check for Visual Studio
$vsPath = Find-VisualStudio
if ($vsPath) {
    Write-Host "  ✓ Visual Studio found: $vsPath" -ForegroundColor Green
} else {
    Write-Host "  ⚠ Visual Studio not found. Build Tools might work." -ForegroundColor Yellow
}

Write-Host ""

# Step 2: Setup vcpkg
Write-Host "[Step 2/5] Setting up vcpkg..." -ForegroundColor Yellow

# Determine vcpkg location
if ($VcpkgPath) {
    $vcpkgRoot = $VcpkgPath
} elseif ($env:VCPKG_ROOT) {
    $vcpkgRoot = $env:VCPKG_ROOT
} else {
    $vcpkgRoot = "C:\vcpkg"
}

if (-not $SkipVcpkg) {
    if (Test-Path "$vcpkgRoot\vcpkg.exe") {
        Write-Host "  ✓ vcpkg already installed at $vcpkgRoot" -ForegroundColor Green
    } else {
        Write-Host "  Installing vcpkg to $vcpkgRoot..." -ForegroundColor Cyan
        
        if (Test-Path $vcpkgRoot) {
            Remove-Item -Recurse -Force $vcpkgRoot
        }
        
        git clone https://github.com/Microsoft/vcpkg.git $vcpkgRoot
        Push-Location $vcpkgRoot
        .\bootstrap-vcpkg.bat
        Pop-Location
        
        Write-Host "  ✓ vcpkg installed" -ForegroundColor Green
    }
    
    # Set environment variable for current session
    $env:VCPKG_ROOT = $vcpkgRoot
    
    Write-Host "  VCPKG_ROOT = $vcpkgRoot" -ForegroundColor Gray
}

Write-Host ""

# Step 3: Install gRPC dependencies
Write-Host "[Step 3/5] Installing gRPC and Protobuf..." -ForegroundColor Yellow

if (-not $BuildOnly) {
    # Check if already installed
    $grpcInstalled = Test-Path "$vcpkgRoot\installed\x64-windows-static\lib\grpc.lib"
    
    if ($grpcInstalled) {
        Write-Host "  ✓ gRPC already installed (cached)" -ForegroundColor Green
    } else {
        Write-Host "  ⚠ This will take 30-60 minutes on first run..." -ForegroundColor Yellow
        Write-Host "  Installing grpc:x64-windows-static..." -ForegroundColor Cyan
        
        Push-Location $vcpkgRoot
        .\vcpkg install grpc:x64-windows-static protobuf:x64-windows-static
        Pop-Location
        
        Write-Host "  ✓ gRPC and Protobuf installed" -ForegroundColor Green
    }
}

Write-Host ""

# Step 4: Configure and Build
Write-Host "[Step 4/5] Configuring and building project..." -ForegroundColor Yellow

$buildDir = Join-Path $ProjectRoot "build"

# Clean build directory if exists
if (Test-Path $buildDir) {
    Write-Host "  Cleaning existing build directory..." -ForegroundColor Gray
    Remove-Item -Recurse -Force $buildDir
}

# Configure with CMake
Write-Host "  Running CMake configure..." -ForegroundColor Cyan
cmake -B $buildDir -S $ProjectRoot `
    "-DCMAKE_TOOLCHAIN_FILE=$vcpkgRoot/scripts/buildsystems/vcpkg.cmake" `
    "-DVCPKG_TARGET_TRIPLET=x64-windows-static" `
    "-DCMAKE_BUILD_TYPE=Release"

if ($LASTEXITCODE -ne 0) {
    Write-Host "  ✗ CMake configuration failed!" -ForegroundColor Red
    exit 1
}

Write-Host "  ✓ CMake configured" -ForegroundColor Green

# Build
Write-Host "  Building project..." -ForegroundColor Cyan
cmake --build $buildDir --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "  ✗ Build failed!" -ForegroundColor Red
    exit 1
}

Write-Host "  ✓ Build successful" -ForegroundColor Green

Write-Host ""

# Step 5: Verify
Write-Host "[Step 5/5] Verifying build..." -ForegroundColor Yellow

$serverExe = Join-Path $buildDir "Release\file_server.exe"
$clientExe = Join-Path $buildDir "Release\file_client.exe"

if ((Test-Path $serverExe) -and (Test-Path $clientExe)) {
    Write-Host "  ✓ file_server.exe found" -ForegroundColor Green
    Write-Host "  ✓ file_client.exe found" -ForegroundColor Green
} else {
    Write-Host "  ✗ Executables not found!" -ForegroundColor Red
    Write-Host "  Checking alternate locations..." -ForegroundColor Yellow
    
    Get-ChildItem -Path $buildDir -Recurse -Filter "*.exe" | ForEach-Object {
        Write-Host "    Found: $($_.FullName)" -ForegroundColor Gray
    }
    exit 1
}

Write-Host ""
Write-Host "============================================" -ForegroundColor Green
Write-Host "  Setup Complete!" -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Green
Write-Host ""
Write-Host "To run the project:" -ForegroundColor Cyan
Write-Host ""
Write-Host "  Terminal 1 (Server):" -ForegroundColor Yellow
Write-Host "    cd `"$ProjectRoot`"" -ForegroundColor Gray
Write-Host "    .\build\Release\file_server.exe" -ForegroundColor White
Write-Host ""
Write-Host "  Terminal 2 (Client):" -ForegroundColor Yellow
Write-Host "    cd `"$ProjectRoot`"" -ForegroundColor Gray
Write-Host "    .\build\Release\file_client.exe" -ForegroundColor White
Write-Host ""
Write-Host "Client commands: create, read, write, delete, list, mkdir, info, exit" -ForegroundColor Gray
Write-Host ""
