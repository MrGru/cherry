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
project (gfx C)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(MACOSX TRUE)
endif()

if(${BUILD_WEB} MATCHES "true")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s LEGACY_GL_EMULATION=1")
        set(PROJECT_LINK_LIBS kernel xml m)

        file(GLOB_RECURSE SOURCES "*.c")
elseif(ANDROID)
        include_directories(../android_include)

        if(${ANDROID_ABI} MATCHES "armeabi-v7a")
                link_directories(../android_lib/armv7)
        elseif(${ANDROID_ABI} MATCHES "armeabi")
                link_directories(../android_lib/arm)
        elseif(${ANDROID_ABI} MATCHES "x86")
                link_directories(../android_lib/x86)
        elseif(${ANDROID_ABI} MATCHES "x86_64")
                link_directories(../android_lib/x86_64)
        endif()

        set(PROJECT_LINK_LIBS kernel xml m pthread SDL2 SDL2_image)

        file(GLOB_RECURSE SOURCES "*.c")

elseif(MACOSX)

        if(${USER_BUILD_MAC_OS} MATCHES “true”)
                link_directories(/usr/local/lib)
        endif()

        if(${USER_BUILD_IOS} MATCHES “true”)
                link_directories(/usr/local/lib)
        endif()

        if(METAL)
                find_library(METAL_LIBRARY Metal
                        PATH_SUFFIXES Frameworks)
                set(PROJECT_LINK_LIBS kernel xml m pthread SDL2 SDL2_image ${METAL_LIBRARY})
        else()
                find_package(OpenGL REQUIRED)
                find_package(GLUT REQUIRED)
                include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
                set(PROJECT_LINK_LIBS kernel xml m pthread SDL2 SDL2_image ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
        endif()

        include_directories(/usr/local/include)
        file(GLOB_RECURSE SOURCES "*.c" "*.m")

else() #linux
        set(PROJECT_LINK_LIBS kernel xml m pthread GL GLU glut SDL2 SDL2_image)

        file(GLOB_RECURSE SOURCES "*.c")
endif()


include_directories(../include)
include_directories(../spin_include)

add_library(gfx STATIC ${SOURCES})
target_link_libraries(gfx ${PROJECT_LINK_LIBS})
