/*
 * Copyright (C) 2005-2024 Centre National d'Etudes Spatiales (CNES)
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

#include "itkMacro.h"

#include "otbNRIBandImagesToOneNComplexBandsImage.h"

#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbNRIBandImagesToOneNComplexBandsImage(int itkNotUsed(argc), char* argv[])
{

  typedef double PixelType;
  typedef otb::VectorImage<PixelType, 2> InputImageType;

  typedef std::complex<double> OutputPixelType;
  typedef otb::VectorImage<OutputPixelType, 2> OutputImageType;


  typedef otb::NRIBandImagesToOneNComplexBandsImage<InputImageType, OutputImageType> FilterType;
  typedef otb::ImageFileReader<InputImageType>  ReaderType;
  typedef otb::ImageFileWriter<OutputImageType> WriterType;

  ReaderType::Pointer readerA = ReaderType::New();
  ReaderType::Pointer readerB = ReaderType::New();
  ReaderType::Pointer readerC = ReaderType::New();
  FilterType::Pointer filter  = FilterType::New();
  WriterType::Pointer writer  = WriterType::New();


  readerA->SetFileName(argv[1]);
  readerB->SetFileName(argv[2]);
  readerC->SetFileName(argv[3]);
  writer->SetFileName(argv[4]);

  filter->SetInput(0, readerA->GetOutput());
  filter->SetInput(1, readerB->GetOutput());
  filter->SetInput(2, readerC->GetOutput());
  writer->SetInput(filter->GetOutput());
  writer->Update();


  return EXIT_SUCCESS;
}
