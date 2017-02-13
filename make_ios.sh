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
mkdir -p ios_build
cmake . -Bios_build -GXcode -DCMAKE_BUILD_TYPE=Release -DUSER_BUILD_IOS=true -DOPENGL=true  -DAPP_DEBUG=false
cd ios_build
xcodebuild -project cherry.xcodeproj -target ALL_BUILD -configuration Release OTHER_CFLAGS="-fembed-bitcode"
xcodebuild -project cherry.xcodeproj -target ALL_BUILD -configuration Debug
cd ..

mkdir -p ios_build_metal
cmake . -Bios_build_metal -GXcode -DCMAKE_BUILD_TYPE=Release -DUSER_BUILD_IOS=true -DMETAL=true  -DAPP_DEBUG=false
cd ios_build_metal
xcodebuild -project cherry.xcodeproj -target ALL_BUILD -configuration Release OTHER_CFLAGS="-fembed-bitcode"
xcodebuild -project cherry.xcodeproj -target ALL_BUILD -configuration Debug
cd ..
