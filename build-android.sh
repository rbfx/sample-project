#!/bin/bash

# Build script for Android project using RebelFork SDK and custom Docker image

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

# Pass SDK path to gradle
export SDK_PATH="${PWD}/${SDK_DIR}"

# Create Gradle wrapper with the correct version
echo "Setting up Gradle wrapper..."
mkdir -p android/gradle/wrapper
cat > android/gradle/wrapper/gradle-wrapper.properties << 'EOF'
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-7.3.3-bin.zip
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
EOF

# Initialize git submodules if they exist
if [ -f ".gitmodules" ]; then
    echo "Initializing git submodules..."
    git submodule sync
    git submodule update --init --recursive
fi

# Create 3rdParty/rbfx directory (needs rbfx source for SDL Java files)
echo "Setting up rbfx source directory..."
mkdir -p 3rdParty
if [ ! -d "3rdParty/rbfx" ]; then
    echo "Cloning rbfx source repository..."
    git clone --depth 1 https://github.com/rbfx/rbfx.git 3rdParty/rbfx
fi

# Build using our custom Docker image with Gradle wrapper
echo "Building Android project with custom Docker image..."
docker run --rm \
  -v "$PWD:/workspace" \
  -w /workspace \
  rbfx-android-builder \
  bash -c "
    set -e
    echo '=== Inside Docker container ==='
    echo 'Current directory:' \$PWD
    echo 'Listing files top-level:'
    ls -la
    
    echo 'Checking 3rdParty/rbfx:'
    ls 3rdParty/rbfx/Source/ThirdParty/SDL/android-project/app/src/main/java/ 2>/dev/null || echo 'SDL Java dir missing'
    
    cd android
    
    # Make gradlew executable
    chmod +x gradlew
    
    # Build using Gradle wrapper (already configured with Gradle 7.3.3)
    echo 'Building with Gradle wrapper (Gradle 7.3.3)...'
    ./gradlew assembleDebug
    
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
