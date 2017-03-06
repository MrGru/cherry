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
project (application_server C)

if(${APP_DEBUG} MATCHES "true")
        add_definitions(-DAPP_DEBUG=1)
else()
        add_definitions(-DAPP_DEBUG=0)
endif()

add_subdirectory(../kernel "../${OUT}/kernel")
add_subdirectory(../xml ../${OUT}/xml)
add_subdirectory(../server ../${OUT}/server)

include_directories(../include)

add_executable(application_server main.c)
add_dependencies(application_server kernel xml server)
target_link_libraries(application_server kernel xml server)
