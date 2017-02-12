mkdir -p ios_build
cmake . -Bios_build -GXcode -DUSER_BUILD_IOS=true -DOPENGL=true  -DAPP_DEBUG=false
cd ios_build
xcodebuild -project cherry.xcodeproj -target ALL_BUILD -configuration Release OTHER_CFLAGS="-fembed-bitcode"
xcodebuild -project cherry.xcodeproj -target ALL_BUILD -configuration Debug
cd ..

# mkdir -p ios_build_metal
# cmake . -Bios_build_metal -GXcode -DUSER_BUILD_IOS=true -DMETAL=true  -DAPP_DEBUG=false
# cd ios_build_metal
# xcodebuild -project cherry.xcodeproj -target ALL_BUILD -configuration Release OTHER_CFLAGS="-fembed-bitcode"
# xcodebuild -project cherry.xcodeproj -target ALL_BUILD -configuration Debug
# cd ..
