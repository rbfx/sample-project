#!/bin/bash

set -e

echo "=== RebelFork Android Build ==="

if [ ! -f "CMakeLists.txt" ] || [ ! -d "android" ]; then
    echo "Error: run from project root"
    exit 1
fi

# Download SDK only if not already extracted
SDK_DIR="rebelfork-sdk-android-clang-x64-dll-latest"
if [ ! -d "$SDK_DIR" ]; then
    echo "Downloading SDK..."
    rm -f rebelfork-sdk-android-clang-x64-dll-latest.7z
    if command -v wget >/dev/null 2>&1; then
        wget -q https://github.com/rbfx/rbfx/releases/download/latest/rebelfork-sdk-android-clang-x64-dll-latest.7z -O rebelfork-sdk-android.7z
    elif command -v curl >/dev/null 2>&1; then
        curl -sL https://github.com/rbfx/rbfx/releases/download/latest/rebelfork-sdk-android-clang-x64-dll-latest.7z -o rebelfork-sdk-android.7z
    else
        echo "Error: wget or curl required"
        exit 1
    fi
    echo "Extracting SDK..."
    7z x rebelfork-sdk-android.7z -o. -y > /dev/null
    rm -f rebelfork-sdk-android.7z
else
    echo "SDK already present, skipping download"
fi

# Patch SDK with missing SDL Java sources (not packaged in released SDK)
SDL_SRC="../rbfx/Source/ThirdParty/SDL/android-project/app/src/main/java"
SDL_DST="$SDK_DIR/share/Urho3D/Android/java"
if [ -d "$SDL_SRC" ]; then
    for java_file in \
        org/libsdl/app/SDLActivity.java \
        org/libsdl/app/SDL.java \
        org/libsdl/app/SDLAudioManager.java \
        org/libsdl/app/SDLControllerManager.java \
        org/libsdl/app/HIDDevice.java \
        org/libsdl/app/HIDDeviceManager.java \
        org/libsdl/app/HIDDeviceBLESteamController.java \
        org/libsdl/app/HIDDeviceUSB.java; do
        if [ ! -f "$SDK_DIR/share/Urho3D/Android/java/$java_file" ]; then
            mkdir -p "$SDL_DST/$(dirname "$java_file")"
            cp "$SDL_SRC/$java_file" "$SDL_DST/$java_file"
            echo "Patched: $java_file"
        fi
    done
    echo "SDL Java sources patched into SDK"
else
    echo "Warning: $SDL_SRC not found, SDL Java patch skipped"
fi

# Docker
if ! command -v docker >/dev/null 2>&1; then
    echo "Error: docker not found"
    exit 1
fi

# Build Docker image
echo "Building Docker image..."
docker build -t rbfx-android-builder-slim .

# Submodules
if [ -f ".gitmodules" ]; then
    git submodule sync
    git submodule update --init --recursive
fi

# Build
echo "Building..."
docker run --rm \
  -v "$PWD:/workspace" \
  -w /workspace \
  rbfx-android-builder-slim \
  bash -c "
    set -e
    cd android
    gradle assembleDebug
  "

# Report
echo "=== Result ==="
find android -name "*.apk" 2>/dev/null && echo "Build succeeded" || echo "No APK found"
