if not exist %~dp0\..\cmake-build-web\* mkdir %~dp0\..\cmake-build-web
pushd %~dp0\..\cmake-build-web
rem EMSCRIPTEN_ROOT_PATH=path/to/emsdk/upstream/emscripten
cmake -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=3rdParty/rbfx/cmake/Toolchains/Emscripten.cmake -DCMAKE_INSTALL_PREFIX=SDK -DCMAKE_BUILD_TYPE=RelWithDebInfo -DURHO3D_PACKAGING=ON -DBUILD_SHARED_LIBS=OFF -DEMSCRIPTEN_ROOT_PATH=%EMSCRIPTEN_ROOT_PATH% -S ../ -B .
popd
