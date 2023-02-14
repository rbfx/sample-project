set cmakefolder = %~dp0\..\cmake-build-wasm32-emscripten
if not exist %cmakefolder%\* mkdir %cmakefolder%
pushd %cmakefolder%
del bin\*.pak
cmake --build . -j8
popd
