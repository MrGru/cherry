cmake_minimum_required(VERSION 2.8.9)
project (audio C)

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
set(PROJECT_LINK_LIBS kernel)

else()

file(GLOB_RECURSE SOURCES "*.c")
set(PROJECT_LINK_LIBS kernel SDL2)

endif()

add_library(audio STATIC ${SOURCES})
target_link_libraries(audio ${PROJECT_LINK_LIBS})
