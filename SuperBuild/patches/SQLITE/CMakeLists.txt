#
# Copyright (C) 2005-2024 Centre National d'Etudes Spatiales (CNES)
#
# This file is part of Orfeo Toolbox
#
#     https://www.orfeo-toolbox.org/
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.12)

project(sqlite)

option(BUILD_SHARED_LIBS "Building shared libs. Default is ON" ON)

include_directories("${CMAKE_SOURCE_DIR}")
add_compile_definitions(SQLITE_ENABLE_COLUMN_METADATA)
add_compile_definitions(SQLITE_ENABLE_RTREE)
add_compile_definitions(SQLITE_THREADSAFE=1)

if(WIN32)
  add_library(sqlite3 STATIC sqlite3.c)
else()
  add_library(sqlite3 sqlite3.c)
endif()

add_executable(sqlite3-bin shell.c sqlite3.c)
if(WIN32)
  target_link_libraries(sqlite3-bin sqlite3)
else()
  find_package( Threads )
  target_link_libraries(sqlite3-bin sqlite3 m ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
endif()

set_target_properties(sqlite3-bin PROPERTIES OUTPUT_NAME sqlite3)
set_target_properties(sqlite3-bin sqlite3
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS sqlite3 sqlite3-bin 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
  
file(GLOB SQLITE_HEADERS "${CMAKE_SOURCE_DIR}/sqlite*.h")
install(FILES ${SQLITE_HEADERS}
  DESTINATION include)

