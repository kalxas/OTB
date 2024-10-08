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

otb_module_test()

set(OTBDisparityMapTests
otbDisparityMapTestDriver.cxx
otbDisparityMapEstimationMethod.cxx
otbDisparityMapToDEMFilter.cxx
otbDisparityMapMedianFilter.cxx
otbDisparityTranslateFilter.cxx
otbSubPixelDisparityImageFilter.cxx
otbDisparityMapTo3DFilter.cxx
otbMultiDisparityMapTo3DFilter.cxx
otbFineRegistrationImageFilterTest.cxx
otbNCCRegistrationFilter.cxx
otbPixelWiseBlockMatchingImageFilter.cxx
)

add_executable(otbDisparityMapTestDriver ${OTBDisparityMapTests})
target_link_libraries(otbDisparityMapTestDriver ${OTBDisparityMap-Test_LIBRARIES})
otb_module_target_label(otbDisparityMapTestDriver)

# Tests Declaration

otb_add_test(NAME dmTvDisparityMapEstimationMethod COMMAND otbDisparityMapTestDriver
  --compare-ascii ${EPSILON_3}
  ${BASELINE_FILES}/dmDisparityMapEstimationOutput1.txt
  ${TEMP}/dmDisparityMapEstimationOutput1.txt
  otbDisparityMapEstimationMethod
  ${INPUTDATA}/fixed.png
  ${INPUTDATA}/moving.png
  ${INPUTDATA}/pointSet.png
  ${TEMP}/dmDisparityMapEstimationOutput1.txt
  10 10
  )

#otb_add_test(NAME dmTvDisparityMapToDEMFilter COMMAND otbDisparityMapTestDriver
  #--compare-image ${NOTOL}
  #${BASELINE}/../../Examples/DisparityMap/elevationOutput.tif
  #${TEMP}/dmTvDisparityMapToDEMFilterOutput.tif
  #otbDisparityMapToDEMFilter
  #${INPUTDATA}/sensor_stereo_blockmatching_output.tif
  #${INPUTDATA}/sensor_stereo_left.tif
  #${INPUTDATA}/sensor_stereo_right.tif
  #${INPUTDATA}/sensor_stereo_rectif_left.tif
  #${INPUTDATA}/sensor_stereo_rectif_right.tif
  #${TEMP}/dmTvDisparityMapToDEMFilterOutput.tif
  #130
  #220
  #140
  #2.5
  #${INPUTDATA}/sensor_stereo_dmap_mask.tif
  #)

otb_add_test(NAME dmTvDisparityMapMedianFilter COMMAND otbDisparityMapTestDriver
  --compare-image ${NOTOL}
  ${BASELINE}/dmTvDisparityMapMedianFilterOutput.tif
  ${TEMP}/dmTvDisparityMapMedianFilterOutput.tif
  otbDisparityMapMedianFilter
  ${INPUTDATA}/StereoHDisparity.tif
  ${TEMP}/dmTvDisparityMapMedianFilterOutput.tif
  2
  2.0
  )

otb_add_test(NAME dmTvDisparityTranslateFilter COMMAND otbDisparityMapTestDriver
  --compare-image ${EPSILON_6}
  ${BASELINE}/dmTvDisparityTranslateFilterOutput.tif
  ${TEMP}/dmTvDisparityTranslateFilterOutput.tif
  otbDisparityTranslateFilter
  ${INPUTDATA}/sensor_stereo_blockmatching_output.tif
  ${INPUTDATA}/sensor_stereo_rectif_inv_left.tif
  ${INPUTDATA}/sensor_stereo_rectif_right.tif
  ${INPUTDATA}/sensor_stereo_left.tif
  ${TEMP}/dmTvDisparityTranslateFilterOutput.tif
  )

otb_add_test(NAME dmTvSubPixelDisparityImageFilterNCC COMMAND otbDisparityMapTestDriver
  --compare-n-images ${NOTOL} 3
  ${BASELINE}/dmTvSubPixelWiseBlockMatchingImageFilterNCCOutputHDisparity.tif
  ${TEMP}/dmTvSubPixelWiseBlockMatchingImageFilterNCCOutputHDisparity.tif
  ${BASELINE}/dmTvSubPixelWiseBlockMatchingImageFilterNCCOutputVDisparity.tif
  ${TEMP}/dmTvSubPixelWiseBlockMatchingImageFilterNCCOutputVDisparity.tif
  ${BASELINE}/dmTvSubPixelWiseBlockMatchingImageFilterNCCOutputMetric.tif
  ${TEMP}/dmTvSubPixelWiseBlockMatchingImageFilterNCCOutputMetric.tif
  otbSubPixelDisparityImageFilter
  ${INPUTDATA}/StereoFixed.png
  ${INPUTDATA}/StereoMoving.png
  ${INPUTDATA}/StereoPixelWiseBlockMatchingDisparity.tif
  ${INPUTDATA}/StereoPixelWiseBlockMatchingMetric.tif
  ${TEMP}/dmTvSubPixelWiseBlockMatchingImageFilterNCCOutputHDisparity.tif
  ${TEMP}/dmTvSubPixelWiseBlockMatchingImageFilterNCCOutputVDisparity.tif
  ${TEMP}/dmTvSubPixelWiseBlockMatchingImageFilterNCCOutputMetric.tif
  1 # NCC
  1 # Parabolic
  2 # radius
  -3 +3 # hdisp threshold
  -2 2 # vdisp threshold
  )

#otb_add_test(NAME dmTvDisparityMapTo3DFilter COMMAND otbDisparityMapTestDriver
  #--compare-image ${EPSILON_6}
  #${BASELINE}/dmTvDisparityMapTo3DFilterOutput.tif
  #${TEMP}/dmTvDisparityMapTo3DFilterOutput.tif
  #otbDisparityMapTo3DFilter
  #${INPUTDATA}/sensor_stereo_blockmatching_output.tif
  #${INPUTDATA}/sensor_stereo_left.tif
  #${INPUTDATA}/sensor_stereo_right.tif
  #${INPUTDATA}/sensor_stereo_rectif_left.tif
  #${INPUTDATA}/sensor_stereo_rectif_right.tif
  #${TEMP}/dmTvDisparityMapTo3DFilterOutput.tif
  #${INPUTDATA}/sensor_stereo_dmap_mask.tif
  #)

otb_add_test(NAME dmTvMultiDisparityMapTo3DFilter COMMAND otbDisparityMapTestDriver
  --compare-n-images ${EPSILON_6} 2
  ${BASELINE}/dmTvMultiDisparityMapTo3DFilterOutput.tif
  ${TEMP}/dmTvMultiDisparityMapTo3DFilterOutput.tif
  ${BASELINE}/dmTvMultiDisparityMapTo3DFilterResidue.tif
  ${TEMP}/dmTvMultiDisparityMapTo3DFilterResidue.tif
  otbMultiDisparityMapTo3DFilter
  LARGEINPUT{PLEIADES/tristereo_sample/master_pan.tif}
  LARGEINPUT{PLEIADES/tristereo_sample/slave_pan_1.tif}
  LARGEINPUT{PLEIADES/tristereo_sample/slave_pan_2.tif}
  LARGEINPUT{PLEIADES/tristereo_sample/phys_disp_1.tif}
  LARGEINPUT{PLEIADES/tristereo_sample/phys_disp_2.tif}
  LARGEINPUT{PLEIADES/tristereo_sample/mask_1.tif}
  LARGEINPUT{PLEIADES/tristereo_sample/mask_2.tif}
  ${TEMP}/dmTvMultiDisparityMapTo3DFilterOutput.tif
  ${TEMP}/dmTvMultiDisparityMapTo3DFilterResidue.tif
  )

otb_add_test(NAME dmTvFineRegistrationImageFilterTestWithMeanSquare COMMAND otbDisparityMapTestDriver
  --compare-n-images ${EPSILON_10} 2
  ${BASELINE}/feTvFineRegistrationImageFilterTestWithMeanSquareMetric.tif
  ${TEMP}/feTvFineRegistrationImageFilterTestWithMeanSquareMetric.tif
  ${BASELINE}/feTvFineRegistrationImageFilterTestWithMeanSquareField.tif
  ${TEMP}/feTvFineRegistrationImageFilterTestWithMeanSquareField.tif
  otbFineRegistrationImageFilterTest
  ${INPUTDATA}/StereoFixed.png # fixedFileName
  ${INPUTDATA}/StereoMoving.png # movingFileName
  ${TEMP}/feTvFineRegistrationImageFilterTestWithMeanSquareMetric.tif # output correlFileName
  ${TEMP}/feTvFineRegistrationImageFilterTestWithMeanSquareField.tif  # output fieldFileName
  1 # radius
  1 # sradius
  0.01 # precision
  2 # Mean square
  1 # Grid step
  0 # Initial offset x
  0 # Initial offset y
  0 0 32 32 # region to proceed
  )
otb_add_test(NAME dmTvFineRegistrationImageFilterTestWithMeanReciprocalDifference COMMAND otbDisparityMapTestDriver
  --compare-n-images ${EPSILON_10} 2
  ${BASELINE}/feTvFineRegistrationImageFilterTestWithMeanReciprocalDifferenceMetric.tif
  ${TEMP}/feTvFineRegistrationImageFilterTestWithMeanReciprocalDifferenceMetric.tif
  ${BASELINE}/feTvFineRegistrationImageFilterTestWithMeanReciprocalDifferenceField.tif
  ${TEMP}/feTvFineRegistrationImageFilterTestWithMeanReciprocalDifferenceField.tif
  otbFineRegistrationImageFilterTest
  ${INPUTDATA}/StereoFixed.png # fixedFileName
  ${INPUTDATA}/StereoMoving.png # movingFileName
  ${TEMP}/feTvFineRegistrationImageFilterTestWithMeanReciprocalDifferenceMetric.tif # output correlFileName
  ${TEMP}/feTvFineRegistrationImageFilterTestWithMeanReciprocalDifferenceField.tif  # output fieldFileName
  3 # radius
  2 # sradius
  0.01 # precision
  3 # Mean reciprocal difference
  1 # Grid step
  0 # Initial offset x
  0 # Initial offset y
  0 0 80 130 # region to proceed
  )
otb_add_test(NAME dmTvFineRegistrationImageFilterTestWithNormalizedCorrelation COMMAND otbDisparityMapTestDriver
  --compare-n-images ${EPSILON_10} 2
  ${BASELINE}/feTvFineRegistrationImageFilterTestWithNormalizedCorrelationMetric.tif
  ${TEMP}/feTvFineRegistrationImageFilterTestWithNormalizedCorrelationMetric.tif
  ${BASELINE}/feTvFineRegistrationImageFilterTestWithNormalizedCorrelationField.tif
  ${TEMP}/feTvFineRegistrationImageFilterTestWithNormalizedCorrelationField.tif
  otbFineRegistrationImageFilterTest
  ${INPUTDATA}/StereoFixed.png # fixedFileName
  ${INPUTDATA}/StereoMoving.png # movingFileName
  ${TEMP}/feTvFineRegistrationImageFilterTestWithNormalizedCorrelationMetric.tif # output correlFileName
  ${TEMP}/feTvFineRegistrationImageFilterTestWithNormalizedCorrelationField.tif  # output fieldFileName
  3 # radius
  2 # sradius
  0.01 # precision
  1 # Normalized Correlation
  1 # Grid step
  0 # Initial offset x
  0 # Initial offset y
  0 0 80 130 # region to proceed
  )
otb_add_test(NAME dmTvFineRegistrationImageFilterTestWithCorrelation COMMAND otbDisparityMapTestDriver
  --compare-n-images ${EPSILON_10} 2
  ${BASELINE}/feTvFineRegistrationImageFilterTestWithCorrelationMetric.tif
  ${TEMP}/feTvFineRegistrationImageFilterTestWithCorrelationMetric.tif
  ${BASELINE}/feTvFineRegistrationImageFilterTestWithCorrelationField.tif
  ${TEMP}/feTvFineRegistrationImageFilterTestWithCorrelationField.tif
  otbFineRegistrationImageFilterTest
  ${INPUTDATA}/StereoFixed.png # fixedFileName
  ${INPUTDATA}/StereoMoving.png # movingFileName
  ${TEMP}/feTvFineRegistrationImageFilterTestWithCorrelationMetric.tif # output correlFileName
  ${TEMP}/feTvFineRegistrationImageFilterTestWithCorrelationField.tif  # output fieldFileName
  3 # radius
  2 # sradius
  0.01 # precision
  0 # Correlation
  1 # Grid step
  0 # Initial offset x
  0 # Initial offset y
  0 0 80 130 # region to proceed
  )
otb_add_test(NAME dmTvNCCRegistrationFilter COMMAND otbDisparityMapTestDriver
  --compare-image ${EPSILON_10}
  ${BASELINE}/dmNCCRegistrationFilterOutput.tif
  ${TEMP}/dmNCCRegistrationFilterOutput.tif
  otbNCCRegistrationFilter
  ${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
  ${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub_warped_sinus.tif
  ${TEMP}/dmNCCRegistrationFilterOutput.tif
  5 1.0 2)

otb_add_test(NAME dmTvPixelWiseBlockMatchingImageFilterNCC COMMAND otbDisparityMapTestDriver
  --compare-n-images ${NOTOL} 2
  ${BASELINE}/dmTvPixelWiseBlockMatchingImageFilterNCCOutputDisparity.tif
  ${TEMP}/dmTvPixelWiseBlockMatchingImageFilterNCCOutputDisparity.tif
  ${BASELINE}/dmTvPixelWiseBlockMatchingImageFilterNCCOutputMetric.tif
  ${TEMP}/dmTvPixelWiseBlockMatchingImageFilterNCCOutputMetric.tif
  otbPixelWiseBlockMatchingImageFilterNCC
  ${INPUTDATA}/StereoFixed.png
  ${INPUTDATA}/StereoMoving.png
  ${TEMP}/dmTvPixelWiseBlockMatchingImageFilterNCCOutputDisparity.tif
  ${TEMP}/dmTvPixelWiseBlockMatchingImageFilterNCCOutputMetric.tif
  2
  -10 +10
  )
otb_add_test(NAME dmTvPixelWiseBlockMatchingImageFilter COMMAND otbDisparityMapTestDriver
  --compare-n-images ${NOTOL} 2
  ${BASELINE}/dmTvPixelWiseBlockMatchingImageFilterOutputDisparity.tif
  ${TEMP}/dmTvPixelWiseBlockMatchingImageFilterOutputDisparity.tif
  ${BASELINE}/dmTvPixelWiseBlockMatchingImageFilterOutputMetric.tif
  ${TEMP}/dmTvPixelWiseBlockMatchingImageFilterOutputMetric.tif
  otbPixelWiseBlockMatchingImageFilter
  ${INPUTDATA}/StereoFixed.png
  ${INPUTDATA}/StereoMoving.png
  ${TEMP}/dmTvPixelWiseBlockMatchingImageFilterOutputDisparity.tif
  ${TEMP}/dmTvPixelWiseBlockMatchingImageFilterOutputMetric.tif
  2
  -10 +10
  )
