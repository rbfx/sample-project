#!/bin/bash

# Build script for Android project using RebelFork SDK and Docker

set -e  # Exit on any error

echo "=== RebelFork Android Build Script ==="

# Check if we're in the correct directory
if [ ! -f "CMakeLists.txt" ] || [ ! -d "android" ]; then
    echo "Error: This script must be run from the project root directory"
    echo "Please navigate to the root of your sample-project directory"
    exit 1
fi

# Clean up any previous SDK downloads
echo "Cleaning up previous SDK files..."
rm -rf rebelfork-sdk-android*

# Download RebelFork Android SDK
echo "Downloading RebelFork Android SDK..."
if command -v wget >/dev/null 2>&1; then
    wget https://github.com/rbfx/rbfx/releases/download/latest/rebelfork-sdk-android-clang-x64-dll-latest.7z -O rebelfork-sdk-android.7z
elif command -v curl >/dev/null 2>&1; then
    curl -L https://github.com/rbfx/rbfx/releases/download/latest/rebelfork-sdk-android-clang-x64-dll-latest.7z -o rebelfork-sdk-android.7z
else
    echo "Error: Neither wget nor curl is available. Please install one of them."
    exit 1
fi

# Extract SDK
echo "Extracting SDK..."
7z x rebelfork-sdk-android.7z

# Find the extracted SDK directory (name may vary)
SDK_DIR=$(find . -maxdepth 1 -type d -name "rebelfork-sdk-android*" | head -n 1)
if [ -z "$SDK_DIR" ]; then
    echo "Error: Could not find extracted SDK directory"
    exit 1
fi

echo "Found SDK directory: $SDK_DIR"

# Check if Docker is available
if ! command -v docker >/dev/null 2>&1; then
    echo "Error: Docker is not installed or not in PATH"
    echo "Please install Docker and make sure it's running"
    exit 1
fi

# Pull the Docker image
echo "Pulling Docker image..."
docker pull mobiledevops/android-sdk-image:36.1.0

# Build using Docker
echo "Building Android project with Docker..."
docker run --rm \
  -v "$PWD:/workspace" \
  -w /workspace \
  mobiledevops/android-sdk-image:36.1.0 \
  bash -c "
    set -e
    echo '=== Inside Docker container ==='
    echo 'Current directory:' \$PWD
    echo 'Listing files:'
    ls -la
    
    # Try to build using Gradle
    if [ -f 'android/gradlew' ]; then
      echo 'Using Gradle wrapper...'
      cd android && chmod +x gradlew && ./gradlew assembleDebug
    else
      echo 'Using system Gradle...'
      cd android && gradle assembleDebug
    fi
    
    echo 'Build completed!'
    echo 'APK files found:'
    find . -name '*.apk' 2>/dev/null || echo 'No APK files found'
  "

# Check for the output APK
echo "=== Build Summary ==="
if find android -name "*.apk" 2>/dev/null | grep -q apk; then
    echo "SUCCESS: APK files have been generated:"
    find android -name "*.apk" 2>/dev/null
else
    echo "WARNING: No APK files found. Check the build output above for errors."
fi

echo "Build script completed."