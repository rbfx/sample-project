pushd ..\..\sample-project-web
del Source\*.pak
cmake --build . --config Debug -j12
popd
