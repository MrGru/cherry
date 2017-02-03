mkdir -p build
cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug
make -C build
