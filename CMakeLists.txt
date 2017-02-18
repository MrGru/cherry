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
cmake_minimum_required(VERSION 2.8.9)
project (cherry C)

if (${USER_BUILD_IOS} MATCHES "true")
        set(CMAKE_OSX_SYSROOT "iphoneos")
        set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator")
        set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)")
endif()


if(${OPENGL} MATCHES "true")
        add_definitions(-DGFX=OGL -DIMAGE_TYPE=IMAGE_SDL2)
else(${METAL} MATCHES "true")
        add_definitions(-DGFX=MTL -DIMAGE_TYPE=IMAGE_OBJC)
endif()

if(${APP_DEBUG} MATCHES "true")
        add_definitions(-DAPP_DEBUG=1)
else()
        add_definitions(-DAPP_DEBUG=0)
endif()

add_subdirectory(kernel)
add_subdirectory(gfx)
add_subdirectory(game)
add_subdirectory(audio)
add_subdirectory(xml)

include_directories(include)

if(${USER_BUILD_IOS} MATCHES "true")

elseif(${USER_BUILD_ANDROID} MATCHES "true")

else()

        add_executable(cherry main.c)
        add_dependencies(cherry kernel gfx game audio xml)
        target_link_libraries(cherry kernel gfx game audio xml)

endif()
