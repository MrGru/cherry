cmake_minimum_required(VERSION 2.8.9)
project (cherry C)

if (${USER_BUILD_IOS} MATCHES "true")
        set(CMAKE_OSX_SYSROOT "iphoneos")
        set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator")
        set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)")
endif()


if(${OPENGL} MATCHES "true")
        add_definitions(-DGFX=OGL)
else(${METAL} MATCHES "true")
        add_definitions(-DGFX=MTL)
endif()

if(${APP_DEBUG} MATCHES "true")
        add_definitions(-DAPP_DEBUG=1)
else()
        add_definitions(-DAPP_DEBUG=0)
endif()

add_subdirectory(kernel)
add_subdirectory(gfx)

include_directories(include)

if(${USER_BUILD_IOS} MATCHES "true")

else()

        add_executable(cherry main.c)
        add_dependencies(cherry kernel gfx)
        target_link_libraries(cherry kernel gfx)

endif()
