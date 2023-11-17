#!/usr/bin/env bash

# build.sh <action> ...
# ci_action:       dependencies|generate|build|install|test
# ci_source_dir:   source code directory
# ci_build_dir:    cmake cache directory
# ci_sdk_dir:      sdk installation directory

ci_action=$1; shift;
ci_source_dir=${ci_source_dir%/};   # remove trailing slash if any

echo "ci_action=$ci_action"
echo "ci_source_dir=$ci_source_dir"
echo "ci_build_dir=$ci_build_dir"
echo "ci_sdk_dir=$ci_sdk_dir"

function action-dependencies() {
    sudo apt-get install -y --no-install-recommends uuid-dev ninja-build
}

function action-generate() {
    cmake \
        -G Ninja                                                                                    \
        -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake   \
        -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON                                                         \
        -DEMSCRIPTEN_ROOT_PATH=$EMSDK/upstream/emscripten/                                          \
        -DCMAKE_BUILD_TYPE=Release                                                                  \
        -DURHO3D_SDK=$ci_sdk_dir                                                                    \
        -DCI_WEB_BUILD=ON                                                                           \
        -DBUILD_SHARED_LIBS=OFF                                                                     \
        -DURHO3D_PROFILING=OFF                                                                      \
        -B $ci_build_dir -S "$ci_source_dir"
}

function action-build() {
    cmake --build $ci_build_dir --parallel $(nproc) --config "Release"
}

function action-prepare() {
    cd $ci_build_dir/bin
    mkdir -p project
    cp ./Player.js ./Player.wasm ./Resources.js ./Resources.js.data ./project
    cp ./Player.html ./project/index.html
}

action-$ci_action
