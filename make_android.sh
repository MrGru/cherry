mkdir -p build_android

mkdir -p build_android/armv7
cmake -Bbuild_android/armv7 -H. -DCMAKE_BUILD_TYPE=Release -DOPENGL=true -DAPP_DEBUG=true  -DUSER_BUILD_ANDROID=true -DANDROID_NATIVE_API_LEVEL=23 -DCMAKE_TOOLCHAIN_FILE=android_toolchain/android.toolchain.cmake -DANDROID_ABI="armeabi-v7a" -DANDROID_NDK=/home/manh/Android/Sdk/ndk-bundle
make -j8 -C build_android/armv7

mkdir -p build_android/arm
cmake -Bbuild_android/arm -H. -DCMAKE_BUILD_TYPE=Release -DOPENGL=true -DAPP_DEBUG=true  -DUSER_BUILD_ANDROID=true -DANDROID_NATIVE_API_LEVEL=23 -DCMAKE_TOOLCHAIN_FILE=android_toolchain/android.toolchain.cmake -DANDROID_ABI="armeabi" -DANDROID_NDK=/home/manh/Android/Sdk/ndk-bundle
make -j8 -C build_android/arm

mkdir -p build_android/x86
cmake -Bbuild_android/x86 -H. -DCMAKE_BUILD_TYPE=Release -DOPENGL=true -DAPP_DEBUG=true  -DUSER_BUILD_ANDROID=true -DANDROID_NATIVE_API_LEVEL=23 -DCMAKE_TOOLCHAIN_FILE=android_toolchain/android.toolchain.cmake -DANDROID_ABI="x86" -DANDROID_NDK=/home/manh/Android/Sdk/ndk-bundle
make -j8 -C build_android/x86

mkdir -p build_android/x86_64
cmake -Bbuild_android/x86_64 -H. -DCMAKE_BUILD_TYPE=Release -DOPENGL=true -DAPP_DEBUG=true  -DUSER_BUILD_ANDROID=true -DANDROID_NATIVE_API_LEVEL=23 -DCMAKE_TOOLCHAIN_FILE=android_toolchain/android.toolchain.cmake -DANDROID_ABI="x86_64" -DANDROID_NDK=/home/manh/Android/Sdk/ndk-bundle
make -j8 -C build_android/x86_64