pushd ..\..\sample-project-web
del bin\*.pak
cmake --build . --config Debug -j12
popd
