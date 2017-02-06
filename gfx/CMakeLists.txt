cmake_minimum_required(VERSION 2.8.9)
project (gfx C)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(MACOSX TRUE)
endif()

if(MACOSX)

        if(${USER_BUILD_MAC_OS} MATCHES “true”)
                link_directories(/usr/local/lib)
        endif()

        if(${USER_BUILD_IOS} MATCHES “true”)
                link_directories(/usr/local/lib)
        endif()

        if(METAL)
                find_library(METAL_LIBRARY Metal
                        PATH_SUFFIXES Frameworks)
                set(PROJECT_LINK_LIBS kernel m pthread SDL2 SDL2_image ${METAL_LIBRARY})
        else()
                find_package(OpenGL REQUIRED)
                find_package(GLUT REQUIRED)
                include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
                set(PROJECT_LINK_LIBS kernel m pthread SDL2 SDL2_image ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
        endif()
        
        include_directories(/usr/local/include)
        file(GLOB_RECURSE SOURCES "*.c" "*.m")

else() #linux

        set(PROJECT_LINK_LIBS kernel m pthread GL GLU glut SDL2 SDL2_image)

        file(GLOB_RECURSE SOURCES "*.c")

endif()


include_directories(../include)

add_library(gfx STATIC ${SOURCES})
target_link_libraries(gfx ${PROJECT_LINK_LIBS})
