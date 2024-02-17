#!/usr/bin/env bash

# build.sh <action> ...
# ci_platform:       web|windows
# ci_action:         dependencies|generate|build|install|test
# ci_source_dir:     source code directory
# ci_build_dir:      cmake cache directory
# ci_native_sdk_dir: native SDK (linux) installation directory
# ci_target_sdk_dir: target SDK (web) installation directory
# ci_build_mode:     sdk|subdirectory

ci_action=$1; shift;
ci_source_dir=${ci_source_dir%/};   # remove trailing slash if any

echo "ci_platform=$ci_platform"
echo "ci_action=$ci_action"
echo "ci_source_dir=$ci_source_dir"
echo "ci_build_dir=$ci_build_dir"
echo "ci_native_sdk_dir=$ci_native_sdk_dir"
echo "ci_target_sdk_dir=$ci_target_sdk_dir"
echo "ci_build_mode=$ci_build_mode"

declare -A build_config=(
    [web]='Release'
    [windows]='RelWithDebInfo'
)

function action-dependencies() {
    if [[ "$ci_platform" == "web" ]]; then
        sudo apt-get install -y --no-install-recommends uuid-dev ninja-build
    elif [[ "$ci_platform" == "windows" ]]; then
        : # No dependencies
    fi
}

function action-generate() {
    local params=(
        "-B"
        "$ci_build_dir"
        "-S"
        "$ci_source_dir"
    )
    if [[ "$ci_platform" == "web" ]]; then
        params+=(
            "-G"
            "Ninja"
            "-DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
            "-DEMSCRIPTEN_ROOT_PATH=$EMSDK/upstream/emscripten/"
            "-DCMAKE_BUILD_WITH_INSTALL_RPATH=ON"
            "-DBUILD_SHARED_LIBS=OFF"
            "-DURHO3D_SDK=$ci_native_sdk_dir"
            "-DCI_WEB_BUILD=ON"
            "-DCMAKE_BUILD_TYPE=Release"
        )
    elif [[ "$ci_platform" == "windows" ]]; then
        params+=(
            "-G"
            "Visual Studio 17 2022"
            "-A"
            "x64"
            "-DCMAKE_BUILD_TYPE=RelWithDebInfo"
        )
    fi

    if [[ $ci_build_mode == "sdk" ]]; then
        params+=("-DREBELFORK_SDK=$ci_target_sdk_dir")
    fi

    cmake "${params[@]}"
}

function action-build() {
    cmake --build $ci_build_dir --parallel $(nproc) --config ${build_config[$ci_platform]}
}

function action-prepare() {
    cd $ci_build_dir/bin
    mkdir -p project
    if [[ "$ci_platform" == "web" ]]; then
        cp ./SampleProject.js ./SampleProject.wasm ./Resources.js ./Resources.js.data ./project
        cp ./SampleProject.html ./project/index.html
    elif [[ "$ci_platform" == "windows" ]]; then
        cp ./RelWithDebInfo/SampleProject.exe ./project
        cp ./RelWithDebInfo/*.dll ./project
        cp ./RelWithDebInfo/*.pdb ./project
        cp -r $ci_source_dir/Project/Data ./project
        cp -r $ci_target_sdk_dir/bin/CoreData ./project

        local dll_files=($(find "$ci_target_sdk_dir/bin/RelWithDebInfo" -type f -name "*.dll"))
        for dep in "${dll_files[@]}"; do
            local filename=$(basename "$dep")
            if [[ "$filename" =~ (.*CSharp.*)|(.*Managed.*)|(Urho3DNet.*)|(Sample\..*)|(.*Player.*)|(.*Editor.*) ]]; then
                continue
            fi
            cp "$dep" ./project
        done
    fi
}

action-$ci_action
