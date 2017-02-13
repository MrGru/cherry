#
# Copyright (C) 2017 Manh Tran
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
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
