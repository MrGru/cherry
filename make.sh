mkdir -p build
cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug -DOPENGL=true -DAPP_DEBUG=true
make -C build
