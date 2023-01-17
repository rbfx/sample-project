if not exist %~dp0\..\cmake-build-web\* mkdir %~dp0\..\cmake-build-web
pushd %~dp0\..\cmake-build-web
del bin\*.pak
cmake --build . -j8
popd
