$ErrorActionPreference = "Stop"

Write-Host "=== RebelFork Android Build ==="

# Check project root directory
if (-not (Test-Path "CMakeLists.txt") -or -not (Test-Path "android")) {
    Write-Error "Error: run from project root"
    exit 1
}

# Download SDK only if not already extracted
$SdkDir = "rebelfork-sdk-android-clang-arm64-dll-latest"
if (-not (Test-Path $SdkDir)) {
    Write-Host "Downloading SDK..."
    if (Test-Path "rebelfork-sdk-android.7z") { Remove-Item "rebelfork-sdk-android.7z" -Force }
    
    $SdkUrl = "https://github.com/rbfx/rbfx/releases/download/latest/rebelfork-sdk-android-clang-arm64-dll-latest.7z"
    Invoke-WebRequest -Uri $SdkUrl -OutFile "rebelfork-sdk-android.7z"

    # Check for 7-Zip installation
    if (-not (Get-Command 7z -ErrorAction SilentlyContinue)) {
        Write-Error "Error: 7z (7-Zip) is required and was not found in PATH"
        exit 1
    }

    Write-Host "Extracting SDK..."
    7z x rebelfork-sdk-android.7z -o. -y | Out-Null
    Remove-Item "rebelfork-sdk-android.7z" -Force
} else {
    Write-Host "SDK already present, skipping download"
}

# Add coredata (should this be part of the SDK?)
Copy-Item -Path "..\rbfx\bin\CoreData" -Destination "Project\" -Recurse -Force

# Check Docker availability
if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
    Write-Error "Error: docker not found"
    exit 1
}

# Build Docker image
Write-Host "Building Docker image..."
docker build -t rbfx-android-builder-slim .

# Submodules
if (Test-Path ".gitmodules") {
    git submodule sync
    git submodule update --init --recursive
}

# Build
Write-Host "Building..."
$CurrentDir = Get-Location
docker run --rm `
  -v "${CurrentDir}:/workspace" `
  -w /workspace `
  rbfx-android-builder-slim `
  bash -c "set -e; cd android; gradle assembleDebug"

# Report
Write-Host "=== Result ==="
$Apks = Get-ChildItem -Path "android" -Filter "*.apk" -Recurse -ErrorAction SilentlyContinue

if ($Apks) {
    $Apks | ForEach-Object { Write-Host $_.FullName }
    Write-Host "Build succeeded"
} else {
    Write-Host "No APK found"
}
