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

add_executable(otbQgisDescriptor otbQgisDescriptor.cxx)
target_link_libraries(otbQgisDescriptor ${OTBQGIS_LIBRARIES})
otb_module_target(otbQgisDescriptor COMPONENT_Core)

add_custom_command(TARGET otbQgisDescriptor PRE_BUILD
  COMMAND "${CMAKE_COMMAND}" "-E" "echo" "Cleaning ${OTB_BINARY_DIR}/${OTB_INSTALL_DATA_DIR}/description/"
  COMMAND "${CMAKE_COMMAND}" "-E" "remove_directory" "${OTB_BINARY_DIR}/${OTB_INSTALL_DATA_DIR}/description/"
  COMMAND "${CMAKE_COMMAND}" "-E" "make_directory"   "${OTB_BINARY_DIR}/${OTB_INSTALL_DATA_DIR}/description/"
  )

file(RELATIVE_PATH app_location ${OTB_BINARY_DIR} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/otb/applications)

set(app_names ${OTB_APPLICATIONS_NAME_LIST})
list(REMOVE_ITEM app_names "TestApplication")
list(REMOVE_ITEM app_names "ApplicationExample")
list(REMOVE_DUPLICATES app_names)
set(dfiles)
set(generate_descriptor_cmd "$<TARGET_FILE:otbQgisDescriptor>")
#debian does not use RPATH and build is failing
#see gitlab #1593
#if( "${CMAKE_SYSTEM_NAME}"  MATCHES "Linux")
if(CMAKE_SKIP_RPATH AND "${CMAKE_SYSTEM_NAME}"  MATCHES "Linux")
  set(generate_descriptor_cmd "env;LD_LIBRARY_PATH=${OTB_BINARY_DIR}/lib;$<TARGET_FILE:otbQgisDescriptor>")
endif()
foreach(app_name ${app_names})
  add_dependencies(otbQgisDescriptor otbapp_${app_name})
  set(dfile "${OTB_BINARY_DIR}/${OTB_INSTALL_DATA_DIR}/description/${app_name}.txt")
  add_custom_command(OUTPUT "${dfile}"
    COMMAND ${generate_descriptor_cmd}
    "${app_name}" "${app_location}" "./${OTB_INSTALL_DATA_DIR}/description/"
    WORKING_DIRECTORY ${OTB_BINARY_DIR}
    COMMENT "./bin/otbQgisDescriptor ${app_name} ${app_location} ./${OTB_INSTALL_DATA_DIR}/description/"
    VERBATIM)
  list(APPEND dfiles "${dfile}")
endforeach()

add_custom_target(generate_descriptors DEPENDS ${dfiles} )
add_dependencies(${otb-module}-all generate_descriptors)
