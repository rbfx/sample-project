pushd ..\..\sample-project-web
del bin\*.pak
cmake --build . -j8
popd
