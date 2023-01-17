if not exist %~dp0\..\cmake-build-win64\* mkdir %~dp0\..\cmake-build-win64
pushd %~dp0\..\cmake-build-win64
cmake -G "Visual Studio 17 2022" -DCMAKE_INSTALL_PREFIX=SDK -DCMAKE_BUILD_TYPE=RelWithDebInfo -DURHO3D_PACKAGING=ON -DBUILD_SHARED_LIBS=OFF -S ./.. -B .
popd
