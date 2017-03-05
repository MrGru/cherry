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
mkdir -p build_emscripten
cmake -DCMAKE_TOOLCHAIN_FILE=/home/manh/Downloads/emsdk_portable/emscripten/master/cmake/Modules/Platform/Emscripten.cmake -Bbuild_emscripten -H. -DCMAKE_BUILD_TYPE=Release -DOPENGL=true -DAPP_DEBUG=true -DBUILD_WEB=true
make -C build_emscripten
cp -rf res build_emscripten
