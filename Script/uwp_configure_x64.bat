if not exist %~dp0\..\cmake-build-uwp-x64\* mkdir %~dp0\..\cmake-build-uwp-x64
pushd %~dp0\..\cmake-build-uwp-x64
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION='10.0' -DCMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION='10.0.17763.0' -DCMAKE_INSTALL_PREFIX=SDK -DCMAKE_BUILD_TYPE=RelWithDebInfo -DURHO3D_PACKAGING=ON -DBUILD_SHARED_LIBS=OFF -S ./.. -B .
popd
