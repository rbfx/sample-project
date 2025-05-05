set cmakefolder=%~dp0\..\cmake-build-wasm32-emscripten
if not exist "%cmakefolder%\*" mkdir "%cmakefolder%"
pushd "%cmakefolder%"
rem EMSCRIPTEN_ROOT_PATH=path/to/emsdk/upstream/emscripten
cmake -G "Ninja" -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON -DCMAKE_TOOLCHAIN_FILE=%EMSCRIPTEN_ROOT_PATH%/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_INSTALL_PREFIX=SDK -DCMAKE_BUILD_TYPE=RelWithDebInfo -DURHO3D_PACKAGING=ON -DBUILD_SHARED_LIBS=OFF -DEMSCRIPTEN_ROOT_PATH=%EMSCRIPTEN_ROOT_PATH% -S ../ -B .
popd
