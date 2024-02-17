#!/usr/bin/env bash

# build.sh <action> ...
# ci_action:         dependencies|generate|build|install|test
# ci_source_dir:     source code directory
# ci_build_dir:      cmake cache directory
# ci_native_sdk_dir: native SDK (linux) installation directory
# ci_target_sdk_dir: target SDK (web) installation directory
# ci_build_mode:     sdk|subdirectory

ci_action=$1; shift;
ci_source_dir=${ci_source_dir%/};   # remove trailing slash if any

echo "ci_action=$ci_action"
echo "ci_source_dir=$ci_source_dir"
echo "ci_build_dir=$ci_build_dir"
echo "ci_native_sdk_dir=$ci_native_sdk_dir"
echo "ci_target_sdk_dir=$ci_target_sdk_dir"
echo "ci_build_mode=$ci_build_mode"

function action-dependencies() {
    sudo apt-get install -y --no-install-recommends uuid-dev ninja-build
}

function action-generate() {
    local extra_params=""
    if [[ $ci_build_mode == "sdk" ]]; then extra_params="-DREBELFORK_SDK=$ci_target_sdk_dir"; fi

    cmake \
        -G Ninja                                                                                    \
        -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake   \
        -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON                                                         \
        -DEMSCRIPTEN_ROOT_PATH=$EMSDK/upstream/emscripten/                                          \
        -DCMAKE_BUILD_TYPE=Release                                                                  \
        -DURHO3D_SDK=$ci_native_sdk_dir                                                             \
        -DCI_WEB_BUILD=ON                                                                           \
        -DBUILD_SHARED_LIBS=OFF                                                                     \
        -DURHO3D_PROFILING=OFF                                                                      \
        $extra_params                                                                               \
        -B $ci_build_dir -S "$ci_source_dir"
}

function action-build() {
    cmake --build $ci_build_dir --parallel $(nproc) --config "Release"
}

function action-prepare() {
    cd $ci_build_dir/bin
    mkdir -p project
    cp ./SampleProject.js ./SampleProject.wasm ./Resources.js ./Resources.js.data ./project
    cp ./SampleProject.html ./project/index.html
}

action-$ci_action
