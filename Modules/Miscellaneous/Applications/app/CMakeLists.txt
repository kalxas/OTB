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

otb_create_application(
  NAME           LocalStatisticExtraction
  SOURCES        otbLocalStatisticExtraction.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
NAME           DomainTransform
SOURCES        otbDomainTransform.cxx
LINK_LIBRARIES ${${otb-module}_LIBRARIES})

OTB_CREATE_APPLICATION(
NAME TrainDimensionalityReduction
SOURCES otbTrainDimensionalityReduction.cxx
LINK_LIBRARIES ${${otb-module}_LIBRARIES} ${OTBCommon_LIBRARIES} ${OTBITK_LIBRARIES} ${OTBBoost_LIBRARIES} ${OTBShark_LIBRARIES} 
)

OTB_CREATE_APPLICATION(
  NAME ImageDimensionalityReduction
  SOURCES otbImageDimensionalityReduction.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES} ${OTBCommon_LIBRARIES} ${OTBITK_LIBRARIES} ${OTBBoost_LIBRARIES} ${OTBShark_LIBRARIES})

OTB_CREATE_APPLICATION(
  NAME VectorDimensionalityReduction
  SOURCES otbVectorDimensionalityReduction.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES} ${OTBCommon_LIBRARIES} ${OTBITK_LIBRARIES} ${OTBBoost_LIBRARIES} ${OTBShark_LIBRARIES})


otb_create_application(
  NAME           GrayScaleMorphologicalOperation
  SOURCES        otbGrayScaleMorphologicalOperation.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})


otb_create_application(
  NAME           MorphologicalClassification
  SOURCES otbMorphologicalClassification.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           MorphologicalProfilesAnalysis
  SOURCES        otbMorphologicalProfilesAnalysis.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           MorphologicalMultiScaleDecomposition
  SOURCES        otbMorphologicalMultiScaleDecomposition.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})