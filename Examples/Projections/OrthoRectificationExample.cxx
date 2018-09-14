/*
 * Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

// Software Guide : BeginLatex
//
// This example demonstrates the use of the
// \doxygen{otb}{OrthoRectificationFilter}. This filter is intended to
// orthorectify images which are in a distributor format with the
// appropriate meta-data describing the sensor model. In this example,
// we will choose to use an UTM projection for the output image.
//
// The first step toward the use of these filters is to include the
// proper header files: the one for the ortho-rectification filter and
// the one defining the different projections available in OTB.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
#include "otbOrthoRectificationFilter.h"
#include "otbSpatialReference.h"
// Software Guide : EndCodeSnippet

int main(int argc, char* argv[])
{

  if (argc != 11)
    {
    std::cout << argv[0] <<
    " <input_filename> <output_filename>  <utm zone> <hemisphere N/S> <x_ground_upper_left_corner> <y_ground_upper_left_corner> <x_Size> <y_Size> <x_groundSamplingDistance> <y_groundSamplingDistance> (should be negative since origin is upper left)>"
              << std::endl;

    return EXIT_FAILURE;
    }

// Software Guide : BeginLatex
//
// We will start by defining the types for the images, the image file
// reader and the image file writer. The writer will be a
// \doxygen{otb}{ImageFileWriter} which will allow us to set
// the number of stream divisions we want to apply when writing the
// output image, which can be very large.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  typedef otb::Image<int, 2>                             ImageType;
  typedef otb::VectorImage<int, 2>                       VectorImageType;
  typedef otb::ImageFileReader<VectorImageType>          ReaderType;
  typedef otb::ImageFileWriter<VectorImageType> WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName(argv[1]);
  writer->SetFileName(argv[2]);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// We can now proceed to declare the type for the ortho-rectification
// filter. The class \doxygen{otb}{OrthoRectificationFilter} is
// templated over the input and the output image types as well as over
// the cartographic projection. We define therefore the
// type of the projection we want, which is an UTM projection for this case.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  typedef otb::GenericMapProjection<otb::TransformDirection::FORWARD> MapProjectionType;
  typedef otb::OrthoRectificationFilter<VectorImageType, VectorImageType,
      MapProjectionType>
  OrthoRectifFilterType;

  OrthoRectifFilterType::Pointer orthoRectifFilter =
    OrthoRectifFilterType::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Now we need to
// instantiate the map projection, set the {\em zone} and {\em hemisphere}
// parameters and pass this projection to the orthorectification filter.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  
  MapProjectionType::Pointer utmMapProjection =
    MapProjectionType::New();
  utmMapProjection->SetWkt(otb::SpatialReference::FromUTM(atoi(argv[3]),atoi(argv[4])).ToWkt());
  orthoRectifFilter->SetMapProjection(utmMapProjection);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// We then wire the input image to the orthorectification filter.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  orthoRectifFilter->SetInput(reader->GetOutput());
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Using the user-provided information, we define the output region
// for the image generated by the orthorectification filter.
// We also define the spacing of the deformation grid where actual
// deformation values are estimated. Choosing a bigger deformation field
// spacing will speed up computation.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  ImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;
  orthoRectifFilter->SetOutputStartIndex(start);

  ImageType::SizeType size;
  size[0] = atoi(argv[7]);
  size[1] = atoi(argv[8]);
  orthoRectifFilter->SetOutputSize(size);

  ImageType::SpacingType spacing;
  spacing[0] = atof(argv[9]);
  spacing[1] = atof(argv[10]);
  orthoRectifFilter->SetOutputSpacing(spacing);

  ImageType::SpacingType gridSpacing;
  gridSpacing[0] = 2.*atof(argv[9]);
  gridSpacing[1] = 2.*atof(argv[10]);
  orthoRectifFilter->SetDisplacementFieldSpacing(gridSpacing);

  ImageType::PointType origin;
  origin[0] = strtod(argv[5], nullptr);
  origin[1] = strtod(argv[6], nullptr);
  orthoRectifFilter->SetOutputOrigin(origin);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// We can now set plug the ortho-rectification filter to the writer
// and set the number of tiles we want to split the output image in
// for the writing step.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  writer->SetInput(orthoRectifFilter->GetOutput());

  writer->SetAutomaticTiledStreaming();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Finally, we trigger the pipeline execution by calling the
// \code{Update()} method on the writer. Please note that the
// ortho-rectification filter is derived from the
// \doxygen{otb}{StreamingResampleImageFilter} in order to be able to
// compute the input image regions which are needed to build the
// output image. Since the resampler applies a geometric
// transformation (scale, rotation, etc.), this region computation is
// not trivial.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  writer->Update();
// Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;

}
