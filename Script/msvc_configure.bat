if not exist %~dp0\..\..\sample-project-win\* mkdir %~dp0\..\..\sample-project-win
pushd %~dp0\..\..\sample-project-win
cmake -G "Visual Studio 17 2022" -DCMAKE_INSTALL_PREFIX=SDK -DCMAKE_BUILD_TYPE=RelWithDebInfo -DURHO3D_PACKAGING=ON -DBUILD_SHARED_LIBS=OFF -S ../sample-project -B .
popd
