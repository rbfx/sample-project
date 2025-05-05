set cmakefolder=%~dp0\..\cmake-build-x64-windows
if not exist "%cmakefolder%\*" mkdir "%cmakefolder%"
pushd "%cmakefolder%"
cmake -G "Visual Studio 17 2022" -DCMAKE_INSTALL_PREFIX=SDK -DCMAKE_BUILD_TYPE=RelWithDebInfo -DURHO3D_PACKAGING=ON -DBUILD_SHARED_LIBS=OFF -S ./.. -B .
popd
