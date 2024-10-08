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
    NAME           Quicklook
    SOURCES        otbQuicklook.cxx
    LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           MultiResolutionPyramid
  SOURCES        otbMultiResolutionPyramid.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           ColorMapping
  SOURCES        otbColorMapping.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           PixelValue
  SOURCES        otbPixelValue.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           TileFusion
  SOURCES        otbTileFusion.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

OTB_CREATE_APPLICATION(
  NAME           Synthetize
  SOURCES        otbSynthetize.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           SplitImage
  SOURCES        otbSplitImage.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           Smoothing
  SOURCES        otbSmoothing.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           ContrastEnhancement
  SOURCES        otbContrastEnhancement.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           FastNLMeans
  SOURCES        otbFastNLMeans.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           BandMath
  SOURCES        otbBandMath.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           BandMathX
  SOURCES        otbBandMathX.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           RadiometricIndices
  SOURCES        otbRadiometricIndices.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           LineSegmentDetection
  SOURCES        otbLineSegmentDetection.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           EdgeExtraction
  SOURCES        otbEdgeExtraction.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           BinaryMorphologicalOperation
  SOURCES        otbBinaryMorphologicalOperation.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           HaralickTextureExtraction
  SOURCES        otbHaralickTextureExtraction.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           SFSTextureExtraction
  SOURCES        otbSFSTextureExtraction.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           PantexTextureExtraction
  SOURCES        otbPantexTextureExtraction.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
NAME           MultivariateAlterationDetector
SOURCES        otbMultivariateAlterationDetector.cxx
LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           HomologousPointsExtraction
  SOURCES        otbHomologousPointsExtraction.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           DimensionalityReduction
  SOURCES        otbDimensionalityReduction.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})
    
otb_create_application(
  NAME           ConvertCartoToGeoPoint
  SOURCES        otbConvertCartoToGeoPoint.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           ObtainUTMZoneFromGeoPoint
  SOURCES        otbObtainUTMZoneFromGeoPoint.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})

otb_create_application(
  NAME           ConvertSensorToGeoPoint
  SOURCES        otbConvertSensorToGeoPoint.cxx
  LINK_LIBRARIES ${${otb-module}_LIBRARIES})