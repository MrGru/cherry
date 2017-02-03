cmake_minimum_required(VERSION 2.8.9)
project (kernel C)

set(PROJECT_LINK_LIBS m)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

include_directories(../include)

if(MACOSX)

file(GLOB_RECURSE SOURCES "*.c" "*.m")

else()

file(GLOB_RECURSE SOURCES "*.c")

endif()

add_library(kernel STATIC ${SOURCES})
target_link_libraries(kernel ${PROJECT_LINK_LIBS})
