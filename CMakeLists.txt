cmake_minimum_required(VERSION 2.8.9)
project (cherry C)

if (${USER_BUILD_IOS} MATCHES "true")
set(CMAKE_OSX_SYSROOT "iphoneos")
set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator")
set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)")
endif()

add_subdirectory(kernel)

include_directories(include)

if(${USER_BUILD_IOS} MATCHES "true")

else()

add_executable(cherry main.c)
add_dependencies(cherry kernel)
target_link_libraries(cherry kernel)

endif()
