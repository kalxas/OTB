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

find_package ( OpenCV REQUIRED )
mark_as_advanced( OpenCV_DIR )

if("${OpenCV_VERSION}" VERSION_LESS "3.0.0")
  message(ERROR " The OpenCV version detected (${OpenCV_VERSION}) is older than the minimum supported version (3.0.0) ")
endif()

if(OpenCV_VERSION_MAJOR EQUAL 4)
set(OTB_OPENCV_4 1)
else()
set(OTB_OPENCV_4 0)
endif()
