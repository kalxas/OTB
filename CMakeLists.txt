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

cmake_minimum_required(VERSION 3.13.0)

foreach(p
    CMP0072 # CMake 3.11
    CMP0078 # CMake 3.13
    CMP0086 # CMake 3.14
    )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()

project(OTB)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Check compiler versions
set(OTB_MIN_GNU_VER 5)
set(OTB_MIN_MSVC_VER 19)
set(OTB_MIN_Clang_VER 3.4)
if(DEFINED OTB_MIN_${CMAKE_CXX_COMPILER_ID}_VER)
  if(CMAKE_CXX_COMPILER_VERSION AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${OTB_MIN_${CMAKE_CXX_COMPILER_ID}_VER})
    message(FATAL_ERROR "The version of your ${CMAKE_CXX_COMPILER_ID} "
      "compiler is not supported by Orfeo ToolBox. Please consider updating your compiler to version "
      "${OTB_MIN_${CMAKE_CXX_COMPILER_ID}_VER} or later.")
  endif()
endif()

include(CMakeDependentOption)
#
# use ExternalProject
include(ExternalProject)

if( CMAKE_HOST_WIN32 )

  string( LENGTH "${CMAKE_CURRENT_SOURCE_DIR}" n )
  if( n GREATER 50 )
    message(
      FATAL_ERROR
      "OTB source code directory path length is too long (${n} > 50)."
      "Please move the OTB source code directory to a directory with a shorter path."
      )
  endif()

  string( LENGTH "${CMAKE_CURRENT_BINARY_DIR}" n )
  if( n GREATER 50 )
    message(
      FATAL_ERROR
      "OTB build directory path length is too long (${n} > 50)."
      "Please set the OTB build directory to a directory with a shorter path."
      )
  endif()

endif()
set(OTB_APPLICATIONS_NAME_LIST "" CACHE STRING "List of all applications" FORCE)
mark_as_advanced(OTB_APPLICATIONS_NAME_LIST)

set(OTB_CMAKE_DIR ${OTB_SOURCE_DIR}/CMake)
set(CMAKE_MODULE_PATH ${OTB_CMAKE_DIR} ${CMAKE_MODULE_PATH})
include(SourceStatus)
include(GetVersionFromGitTag)
include(PreventInSourceBuilds)
include(PreventInBuildInstalls)
include(OTBModuleMacros)
include(OTBModuleRemote)
include(OTBModuleTest)

repository_status(${PROJECT_SOURCE_DIR} OTB_GIT_STATUS_MESSAGE)

if(OTB_WRAP_PYTHON)
  find_package(Python REQUIRED COMPONENTS Interpreter Development)
  if(Python_FOUND)
    message(STATUS "Python Library directory : ${Python_LIBRARY_DIRS}")
    message(STATUS "Python Headers directory : ${Python_INCLUDE_DIRS}")
  endif()
endif()

#-----------------------------------------------------------------------------
# Configure the default OTB_DATA_ROOT for the location of OTB Data.
find_path(OTB_DATA_ROOT
  NAMES README-OTB-Data
  HINTS ${OTB_SOURCE_DIR}/Data
  )
#mark_as_advanced(OTB_DATA_ROOT)

if(OTB_DATA_ROOT)
  repository_status(${OTB_DATA_ROOT} OTB_DATA_GIT_STATUS_MESSAGE)
endif()

set(main_project_name ${_OTBModuleMacros_DEFAULT_LABEL})

#-----------------------------------------------------------------------------
# OTB version number.
set(OTB_VERSION_MAJOR "9")
set(OTB_VERSION_MINOR "1")
set(OTB_VERSION_PATCH "0")
set(OTB_VERSION_STRING "${OTB_VERSION_MAJOR}.${OTB_VERSION_MINOR}.${OTB_VERSION_PATCH}")

get_package_name(${OTB_SOURCE_DIR} ${PROJECT_NAME} OTB_VERSION_STRING2)

#-----------------------------------------------------------------------------
string(TIMESTAMP OTB_BUILD_TIMESTAMP)

if(NOT OTB_INSTALL_RUNTIME_DIR)
  set(OTB_INSTALL_RUNTIME_DIR bin)
endif()
if(NOT OTB_INSTALL_LIBRARY_DIR)
  set(OTB_INSTALL_LIBRARY_DIR lib)
endif()
if(NOT OTB_INSTALL_ARCHIVE_DIR)
  set(OTB_INSTALL_ARCHIVE_DIR lib)
endif()
if(NOT OTB_INSTALL_INCLUDE_DIR)
  set(OTB_INSTALL_INCLUDE_DIR include/OTB-${OTB_VERSION_MAJOR}.${OTB_VERSION_MINOR})
endif()
if(NOT OTB_INSTALL_APP_DIR)
  set(OTB_INSTALL_APP_DIR "${OTB_INSTALL_LIBRARY_DIR}/otb/applications")
endif()
if(NOT OTB_INSTALL_PYTHON_DIR)
  set(OTB_INSTALL_PYTHON_DIR "${OTB_INSTALL_LIBRARY_DIR}/otb/python")
endif()
if(NOT OTB_INSTALL_DATA_DIR)
  set(OTB_INSTALL_DATA_DIR "share/otb")
endif()

if(NOT OTB_INSTALL_DOC_DIR)
  set(OTB_INSTALL_DOC_DIR "share/doc/otb")
endif()
if(NOT OTB_INSTALL_PACKAGE_DIR)
  set(OTB_INSTALL_PACKAGE_DIR "${OTB_INSTALL_LIBRARY_DIR}/cmake/OTB-${OTB_VERSION_MAJOR}.${OTB_VERSION_MINOR}")
endif()

#convert path to native for reconfiguring otbcli.bat.in
file(TO_NATIVE_PATH "${OTB_INSTALL_APP_DIR}" OTB_INSTALL_APP_DIR_NATIVE)

#-----------------------------------------------------------------------------
# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


#-----------------------------------------------------------------------------
# Enable running cppcheck for each module on its source and test directories.
option(OTB_CPPCHECK_TEST "Run cppcheck for static code analysis" OFF)
mark_as_advanced(OTB_CPPCHECK_TEST)

#-----------------------------------------------------------------------------
# Forbid downloading resources from the network during a build. This helps
# when building on systems without network connectivity to determine which
# resources much be obtained manually and made available to the build.
option(OTB_FORBID_DOWNLOADS "Do not download source code or data from the network" OFF)
mark_as_advanced(OTB_FORBID_DOWNLOADS)
macro(otb_download_attempt_check _name)
  if(OTB_FORBID_DOWNLOADS)
    message(SEND_ERROR "Attempted to download ${_name} when OTB_FORBID_DOWNLOADS is ON")
  endif()
endmacro()

# TODO : handle shared libs on windows
option(BUILD_SHARED_LIBS "Build OTB with shared libraries." ON)
mark_as_advanced(BUILD_SHARED_LIBS)

if(MSVC)
  option(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS "Build OTB with shared libraries." ON)
  mark_as_advanced(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS)
  set(OTB_CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ${CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS})
else()
  set(OTB_CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
endif()

# Although the OpenMP option looks like a 3rd party module, it is NOT. This option
# is handled with other compilation flags in file OTBSetStandardCompilerFlags.cmake
# before any module is parsed or enabled (see OTBModuleEnablement.cmake later on
# during configure). Trying to modify this option from any module is
# useless. Any 3rd party module dragging OpenMP should append
# the corresponding flags to CMAKE_CXX_FLAGS, CMAKE_EXE_LINKER_FLAGS, and so on.
option(OTB_USE_OPENMP "Add openmp compiler and linker flags" OFF)

option(OTB_USE_SSE_FLAGS "Enable SIMD optimizations (hardware dependent)." ON)
mark_as_advanced(OTB_USE_SSE_FLAGS)

include(OTBSetStandardCompilerFlags)
#---------------------------------------------------------------
# run try compiles and tests for OTB

# TODO check if we need this
#include(otbTestFriendTemplatedFunction)

set(CMAKE_C_FLAGS "${OTB_REQUIRED_C_FLAGS} ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${OTB_REQUIRED_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${OTB_REQUIRED_LINK_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${OTB_REQUIRED_LINK_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${OTB_REQUIRED_LINK_FLAGS} ${CMAKE_MODULE_LINKER_FLAGS}")
# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")

# TODO BUILD_TESTING is created by default by CTest.cmake, and is ON by default.
option(BUILD_TESTING "Build the testing tree." OFF)

include(CTest)

include( CppcheckTargets )

# Setup build locations.
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OTB_BINARY_DIR}/bin)
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OTB_BINARY_DIR}/lib)
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OTB_BINARY_DIR}/lib)
endif()
set(OTB_MODULES_DIR "${OTB_BINARY_DIR}/${OTB_INSTALL_PACKAGE_DIR}/Modules")

#-----------------------------------------------------------------------------
# By default, OTB does not build the Examples that are illustrated in the Software Guide
option(BUILD_EXAMPLES "Build the Examples directory." OFF)

#----------------------------------------------------------------------------
set(OTB_TEST_OUTPUT_DIR "${OTB_BINARY_DIR}/Testing/Temporary")

option(OTB_DATA_USE_LARGEINPUT "Use Large inputs images test." OFF)
mark_as_advanced(OTB_DATA_USE_LARGEINPUT)
if(OTB_DATA_USE_LARGEINPUT)
  find_path(OTB_DATA_LARGEINPUT_ROOT
    NAMES OTBData.readme
    HINTS $ENV{OTB_DATA_LARGEINPUT_ROOT}
  )
  mark_as_advanced(OTB_DATA_LARGEINPUT_ROOT)
endif()

# Small macro to copy a file and rename it
macro(otb_copy_rename _input _output)
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_input}" "${_output}"
    RESULT_VARIABLE _err_code
    OUTPUT_QUIET
    ERROR_QUIET
    )
  if(_err_code)
    message(FATAL_ERROR "Failed to copy file ${_input} to ${_output}")
  endif()
endmacro()

#Generate cmake variables with a set of largeinput data used in tests
include(OTBManageLargeInputPaths)

set(BASELINE       ${OTB_DATA_ROOT}/Baseline/OTB/Images)
set(BASELINE_FILES ${OTB_DATA_ROOT}/Baseline/OTB/Files)
set(INPUTDATA      ${OTB_DATA_ROOT}/Input)
set(TEMP           ${OTB_BINARY_DIR}/Testing/Temporary)
set(OTBAPP_BASELINE       ${OTB_DATA_ROOT}/Baseline/OTB-Applications/Images)
set(OTBAPP_BASELINE_FILES ${OTB_DATA_ROOT}/Baseline/OTB-Applications/Files)

set(NOTOL      0.0)
set(EPSILON_1  0.1)
set(EPSILON_2  0.01)
set(EPSILON_3  0.001)
set(EPSILON_4  0.0001)
set(EPSILON_5  0.00001)
set(EPSILON_6  0.000001)
set(EPSILON_7  0.0000001)
set(EPSILON_8  0.00000001)
set(EPSILON_9  0.000000001)
set(EPSILON_10 0.0000000001)
set(EPSILON_11 0.00000000001)
set(EPSILON_12 0.000000000001)
set(EPSILON_13 0.0000000000001)
set(EPSILON_14 0.00000000000001)
set(EPSILON_15 0.000000000000001)

# Location of OTB Example Data.
set(OTB_EXAMPLE_DATA_ROOT "${OTB_SOURCE_DIR}/Examples/Data")

#----------------------------------------------------------------------
# Make sure remote modules are downloaded before sorting out the module
# dependencies.
add_subdirectory(Modules/Remote)

# Enable modules according to user inputs and the module dependency DAG.
include(OTBModuleEnablement)

#----------------------------------------------------------------------
# Generate OTBConfig.cmake for the build tree.
set(OTB_CONFIG_CODE "
set(OTB_MODULES_DIR \"${OTB_MODULES_DIR}\")")
set(OTB_CONFIG_CMAKE_DIR "${OTB_SOURCE_DIR}/CMake")
set(OTB_USE_FILE "${OTB_CONFIG_CMAKE_DIR}/UseOTB.cmake")

if(OTB_WRAP_PYTHON)
set(OTB_CONFIG_CODE "${OTB_CONFIG_CODE}
set(OTB_PYTHONPATH \"${OTB_BINARY_DIR}/${OTB_INSTALL_PYTHON_DIR}\")")
endif()

set(OTB_CONFIG_APPLICATION_PATH "${OTB_BINARY_DIR}/${OTB_INSTALL_APP_DIR}")
set(OTB_CONFIG_TARGETS_CONDITION " AND NOT OTB_BINARY_DIR")
set(OTB_CONFIG_TARGETS_FILE "${OTB_BINARY_DIR}/OTBTargets.cmake")
set(OTB_CONFIG_MODULE_API_FILE "${OTB_SOURCE_DIR}/CMake/OTBModuleAPI.cmake")
set(OTB_CONFIG_APPLI_MACROS_FILE "${OTB_SOURCE_DIR}/CMake/OTBApplicationMacros.cmake")
configure_file(CMake/OTBConfig.cmake.in OTBConfig.cmake @ONLY)

# Generate OTBConfig.cmake for the install tree.
set(OTB_CONFIG_CODE "
# Compute the installation prefix from this OTBConfig.cmake file location.
get_filename_component(OTB_INSTALL_PREFIX \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)")
# Construct the proper number of get_filename_component(... PATH)
# calls to compute the installation prefix.
string(REGEX REPLACE "/" ";" _count "${OTB_INSTALL_PACKAGE_DIR}")
foreach(p ${_count})
  set(OTB_CONFIG_CODE "${OTB_CONFIG_CODE}
get_filename_component(OTB_INSTALL_PREFIX \"\${OTB_INSTALL_PREFIX}\" PATH)")
endforeach()
set(OTB_CONFIG_CODE "${OTB_CONFIG_CODE}
set(OTB_MODULES_DIR \"\${OTB_INSTALL_PREFIX}/${OTB_INSTALL_PACKAGE_DIR}/Modules\")")
set(OTB_USE_FILE "\${OTB_INSTALL_PREFIX}/${OTB_INSTALL_PACKAGE_DIR}/UseOTB.cmake")
set(OTB_CONFIG_CMAKE_DIR "\${OTB_INSTALL_PREFIX}/${OTB_INSTALL_PACKAGE_DIR}")

if(OTB_WRAP_PYTHON)
set(OTB_CONFIG_CODE "${OTB_CONFIG_CODE}
set(OTB_PYTHONPATH \"\${OTB_INSTALL_PREFIX}/${OTB_INSTALL_PYTHON_DIR}\")")
endif()

set(OTB_CONFIG_APPLICATION_PATH "\${OTB_INSTALL_PREFIX}/${OTB_INSTALL_APP_DIR}")
set(OTB_CONFIG_TARGETS_CONDITION "")
set(OTB_CONFIG_TARGETS_FILE "\${OTB_INSTALL_PREFIX}/${OTB_INSTALL_PACKAGE_DIR}/OTBTargets.cmake")
set(OTB_CONFIG_MODULE_API_FILE "\${OTB_INSTALL_PREFIX}/${OTB_INSTALL_PACKAGE_DIR}/OTBModuleAPI.cmake")
set(OTB_CONFIG_APPLI_MACROS_FILE "\${OTB_INSTALL_PREFIX}/${OTB_INSTALL_PACKAGE_DIR}/OTBApplicationMacros.cmake")

configure_file(CMake/OTBConfig.cmake.in CMakeFiles/OTBConfig.cmake @ONLY)

# TODO : port unscrustify scripts from ITK to OTB ?
# #----------------------------------------------------------------------------
# # Configure maintenance scripts
# configure_file(Utilities/Maintenance/doSingleKWStyleUncrustifyFix.sh.in
#   Utilities/Maintenance/doSingleKWStyleUncrustifyFix.sh @ONLY)

#-----------------------------------------------------------------------------
configure_file(CMake/OTBConfigVersion.cmake.in OTBConfigVersion.cmake @ONLY)

install(FILES ${OTB_BINARY_DIR}/CMakeFiles/OTBConfig.cmake
              ${OTB_BINARY_DIR}/OTBConfigVersion.cmake
              CMake/OTBApplicationMacros.cmake
              CMake/OTBModuleAPI.cmake
              CMake/OTBStandaloneModuleMacros.cmake
              CMake/OTBModuleExternal.cmake
              CMake/UseOTB.cmake
              CMake/GenerateExportHeaderCustom.cmake
              CMake/exportheader.cmake.in
              CMake/NamespaceHandler.cmake
  DESTINATION ${OTB_INSTALL_PACKAGE_DIR}
  COMPONENT Core)
get_property(OTBTargets_MODULES GLOBAL PROPERTY OTBTargets_MODULES)
if(OTBTargets_MODULES)
  install(EXPORT OTBTargets DESTINATION ${OTB_INSTALL_PACKAGE_DIR}
          COMPONENT Core)
else()
  set(CMAKE_CONFIGURABLE_FILE_CONTENT "# No targets!")
  configure_file(${CMAKE_ROOT}/Modules/CMakeConfigurableFile.in
                 ${OTB_BINARY_DIR}/CMakeFiles/OTBTargets.cmake @ONLY)
  install(FILES ${OTB_BINARY_DIR}/CMakeFiles/OTBTargets.cmake
          DESTINATION ${OTB_INSTALL_PACKAGE_DIR} COMPONENT Core)
endif()

if(XDK_INSTALL_PATH)
    # In order to package the external libraries built by the SuperBuild, we have to "unpack" this xdk in the current build directory and tell
    # Cmake it is here
    message(STATUS "XDK FOUND, preparing it for packaging")
    install(DIRECTORY ${XDK_INSTALL_PATH}/lib
            DESTINATION .
            USE_SOURCE_PERMISSIONS
            COMPONENT Dependencies)
    # Redhat8 case : the bindings are installed by default in lib64/python3.8
    if( EXISTS ${XDK_INSTALL_PATH}/lib64)
      install(DIRECTORY ${XDK_INSTALL_PATH}/lib64/python3.8
              DESTINATION ./lib
              USE_SOURCE_PERMISSIONS
              COMPONENT Dependencies)
    endif()
    install(DIRECTORY ${XDK_INSTALL_PATH}/share
            DESTINATION .
            USE_SOURCE_PERMISSIONS
            COMPONENT Core)
    install(DIRECTORY ${XDK_INSTALL_PATH}/include
            DESTINATION .
            USE_SOURCE_PERMISSIONS
            COMPONENT Core)
    install(DIRECTORY ${XDK_INSTALL_PATH}/bin
            DESTINATION .
            USE_SOURCE_PERMISSIONS
            COMPONENT Core)
    install(DIRECTORY ${XDK_INSTALL_PATH}/cmake
            DESTINATION ./lib
            USE_SOURCE_PERMISSIONS
            COMPONENT Core)
else()
    message(WARNING "XDK NOT FOUND, it will not be packaged")
endif()

# install FindXXX.cmake
file(GLOB OTB_FIND_PACKAGE_SCRIPTS CMake/Find*.cmake)
install(FILES ${OTB_FIND_PACKAGE_SCRIPTS} DESTINATION ${OTB_INSTALL_PACKAGE_DIR} COMPONENT Core)

# TODO : install release notes, copyright notice, ...
install(FILES "LICENSE" DESTINATION . COMPONENT Core)

# Configure and install VERSION file
configure_file(VERSION VERSION @ONLY)
install(FILES ${OTB_BINARY_DIR}/VERSION DESTINATION ${OTB_INSTALL_DOC_DIR})

if(EXISTS ${OTB_BINARY_DIR}/${OTB_INSTALL_DATA_DIR})
  message("Directory share/otb already exists")
else()
  file(MAKE_DIRECTORY ${OTB_BINARY_DIR}/${OTB_INSTALL_DATA_DIR})
endif()
install(DIRECTORY ${OTB_BINARY_DIR}/${OTB_INSTALL_DATA_DIR} DESTINATION share COMPONENT Core)

install(FILES Packaging/Files/build_wrapping.cmake
DESTINATION ./share/otb/swig
COMPONENT Core)
install(FILES Packaging/Files/build_wrapping.md
DESTINATION ./share/otb/swig
RENAME README.md
COMPONENT Core)

#start installing script and other stuff related to package
if(UNIX)
  install( PROGRAMS
    ${OTB_SOURCE_DIR}/Packaging/Files/recompile_bindings.sh
    ${OTB_SOURCE_DIR}/Packaging/Files/otbenv.profile
    DESTINATION .
    COMPONENT Core
    )
  configure_file(${OTB_SOURCE_DIR}/Packaging/Files/post_install.sh.in
                 ${CMAKE_BINARY_DIR}/post_install.sh @ONLY)
  install( PROGRAMS
    ${OTB_SOURCE_DIR}/Packaging/Files/check_python_env.sh
    ${CMAKE_BINARY_DIR}/post_install.sh
    ${OTB_SOURCE_DIR}/Packaging/Files/sanitize_rpath.sh
    DESTINATION ./tools
    COMPONENT Core
    )
endif()

#my_file_ext is required for otbcli files which has
# no extension on linux. (hence we cannot use SCRIPT_EXT

#For windows otbenv.bash and otbenv.bat is required.
# Former is for windows shell users and latter is for other
# linux emulator such as bash for windows or mobxterm
set(my_file_ext "")
if(WIN32)
  set(my_file_ext ".bat")
  install( PROGRAMS
    ${OTB_SOURCE_DIR}/Packaging/Files/otbenv.bat
    ${OTB_SOURCE_DIR}/Packaging/Files/otbenv.ps1
    ${OTB_SOURCE_DIR}/Packaging/Files/otbenv.bash
    DESTINATION .
    COMPONENT Core
    )
endif()

# #configure README from rst docs
set(RSTDOC_DIR "${OTB_SOURCE_DIR}/Documentation/Cookbook/rst")
if(WIN32) #windows
  set(README_FILE ${RSTDOC_DIR}/Installation_Windows.rst)
else()
  set(README_FILE ${RSTDOC_DIR}/Installation_Linux.rst)
endif()
configure_file("${README_FILE}" ${CMAKE_BINARY_DIR}/README )
install(FILES ${CMAKE_BINARY_DIR}/README DESTINATION . COMPONENT Core)

if(BUILD_TESTING)
  add_subdirectory(Utilities/InstallTest)
endif()
#-----------------------------------------------------------------------------
# The subdirectories added below this line should use only the public
# interface with find_package(ITK). Set ITK_DIR to use this ITK build.
set(OTB_DIR "${OTB_BINARY_DIR}")

if(BUILD_EXAMPLES)
  add_subdirectory(Examples)
endif()

#----------------------------------------------------------------------
# Provide an option for generating documentation.
add_subdirectory(Utilities/Doxygen)

add_subdirectory(Utilities/Completion)

option(BUILD_COOKBOOK "Build the CookBook documentation." OFF)

if (BUILD_COOKBOOK)
    add_subdirectory(Documentation/Cookbook)
endif()

#----------------------------------------------------------------------------
# Provide a target to generate the SuperBuild archive (only for Unix)
if(UNIX)
  add_custom_target(SuperBuild-archive
    COMMAND ./SuperbuildDownloadList.sh
      ${OTB_BINARY_DIR}/Download
      ${OTB_BINARY_DIR}
    WORKING_DIRECTORY ${OTB_SOURCE_DIR}/CI
    )
endif()

# TODO cleanup
# Create target to download data from the OTBData group. This must come after
# all tests have been added that reference the group, so we put it last.

#-----------------------------------------------------------------------------
# uninstall target
configure_file(
    "${CMAKE_SOURCE_DIR}/CMake/cmake_uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake)

#-----------------------------------------------------------------------------
#macro to put a fixed space between key, value  in summary
macro(get_white_spaces var res)
  string(LENGTH "${var}" len)
  math(EXPR required_spaces "23 - ${len}")
  set(${res})
  foreach(loop_var RANGE ${required_spaces})
    set(${res} "${${res}} ")
  endforeach()
endmacro()

message("\n======================= Begin of OTB cmake summary =======================\n")

if(OTB_GIT_STATUS_MESSAGE)
  message( ${OTB_GIT_STATUS_MESSAGE})
endif()
if(OTB_DATA_GIT_STATUS_MESSAGE)
  message( ${OTB_DATA_GIT_STATUS_MESSAGE})
endif()

set(option_list ${OTB_MODULE_ACTIVATION_OPTION_LIST})
list(SORT option_list)

foreach(otb_option
    CMAKE_SOURCE_DIR
    CMAKE_BINARY_DIR
    CMAKE_INSTALL_PREFIX
    CMAKE_COMMAND
    CMAKE_C_COMPILER
    CMAKE_CXX_COMPILER
    CMAKE_MAKE_PROGRAM
    CMAKE_GENERATOR
    CMAKE_BUILD_TYPE
    CMAKE_C_FLAGS
    CMAKE_CXX_FLAGS
    ${option_list}
    OTB_WRAP_PYTHON
    OTB_WRAP_QGIS
    OTB_DATA_ROOT
    OTB_USE_OPENMP
    OTB_USE_SSE_FLAGS
    )
    get_white_spaces("${otb_option}" spaces)
  message("${otb_option} ${spaces} = ${${otb_option}}")
endforeach()

# Cleaning option_list as we are using a new one now
unset(option_list)

#To report non-optional dependencies inside summary
list(APPEND option_list Boost)
list(APPEND option_list GDAL)
list(APPEND option_list GEOTIFF)
list(APPEND option_list ITK)
list(APPEND option_list TINYXML)

#sort again!
list(SORT option_list)

foreach(dep ${option_list})

  string(REGEX REPLACE "OTB_USE_" "" dep ${dep})

  unset(var1_key)
  unset(var2_key)
  unset(var3_key)

  unset(var1_val)
  unset(var2_val)
  unset(var3_val)

  set(var1_key "${dep}")
  if(${dep}_VERSION_STRING)
    set(var1_val ${${dep}_VERSION_STRING})
  endif()
  if(NOT var1_val)
    if(${dep}_VERSION)
      set(var1_val ${${dep}_VERSION})
    endif()
  endif()

  if(DEFINED ${dep}_DIR)
    set(var2_key  ${dep}_DIR)
    set(var2_val ${${dep}_DIR})
  else()
    if(${dep}_INCLUDE_DIR)
      set(var2_key  ${dep}_INCLUDE_DIR)
      set(var2_val ${${dep}_INCLUDE_DIR})
    elseif(${dep}_INCLUDE_DIRS)
      set(var2_key  ${dep}_INCLUDE_DIRS)
      set(var2_val ${${dep}_INCLUDE_DIRS})
    endif()
    if(${dep}_LIBRARY)
      set(var3_key  ${dep}_LIBRARY)
      set(var3_val ${${dep}_LIBRARY})
    elseif(${dep}_LIBRARIES)
      set(var3_key  ${dep}_LIBRARIES)
      set(var3_val ${${dep}_LIBRARIES})
    endif()
  endif()

  get_white_spaces("${var1_key}" insert_spaces1)
  get_white_spaces("${var2_key}" insert_spaces2)
  get_white_spaces("${var3_key}" insert_spaces3)

  message("${var1_key} ${insert_spaces1} = ${var1_val}")
  if(var2_val)
    message("${var2_key} ${insert_spaces2} = ${var2_val}")
  endif()
  if(var3_val)
    message("${var3_key} ${insert_spaces3} = ${var3_val}")
  endif()

endforeach()

message("\nCMake ${CMAKE_VERSION} successfully configured ${PROJECT_NAME} using ${CMAKE_GENERATOR} generator\n")

message("======================= End of OTB cmake summary =======================")

#don't need this one anymore. so unset it.
unset(option_list)

#----------------------------------------------------------------------------
include(Packaging_OTB)
