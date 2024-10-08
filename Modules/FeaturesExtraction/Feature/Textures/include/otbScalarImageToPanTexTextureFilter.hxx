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

#ifndef otbScalarImageToPanTexTextureFilter_hxx
#define otbScalarImageToPanTexTextureFilter_hxx

#include "otbScalarImageToPanTexTextureFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"
#include "itkProgressReporter.h"
#include "itkNumericTraits.h"

namespace otb
{
template <class TInputImage, class TOutputImage>
ScalarImageToPanTexTextureFilter<TInputImage, TOutputImage>::ScalarImageToPanTexTextureFilter()
  : m_Radius(), m_NumberOfBinsPerAxis(8), m_InputImageMinimum(0), m_InputImageMaximum(255)
{
  // There are 1 output corresponding to the Pan Tex texture indice
  this->SetNumberOfRequiredOutputs(1);

  // Ten offsets are selected for contrast computation (2 pixels displacement grid, and given that the
  // co-occurance matrix is symmetric
  m_OffsetList = { {0, 1}, {0, 2}, {1, -2}, {1, -1}, {1, 0}, {1, 1}, {1, 2}, {2, -1}, {2, 0}, {2, 1} };
}

template <class TInputImage, class TOutputImage>
void ScalarImageToPanTexTextureFilter<TInputImage, TOutputImage>::GenerateInputRequestedRegion()
{
  // First, call superclass implementation
  Superclass::GenerateInputRequestedRegion();

  // Retrieve the input and output pointers
  InputImagePointerType  inputPtr  = const_cast<InputImageType*>(this->GetInput());
  OutputImagePointerType outputPtr = this->GetOutput();

  if (!inputPtr || !outputPtr)
  {
    return;
  }

  // Retrieve the output requested region
  // We use only the first output since requested regions for all outputs are enforced to be equal
  // by the default GenerateOutputRequestedRegiont() implementation
  OutputRegionType outputRequestedRegion = outputPtr->GetRequestedRegion();

  // Build the input requested region
  InputRegionType inputRequestedRegion = outputRequestedRegion;

  // Apply the radius
  SizeType maxOffsetSize = {2, 2};
  inputRequestedRegion.PadByRadius(m_Radius + maxOffsetSize);

  // Try to apply the requested region to the input image
  if (inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()))
  {
    inputPtr->SetRequestedRegion(inputRequestedRegion);
  }
  else
  {
    // Build an exception
    itk::InvalidRequestedRegionError e(__FILE__, __LINE__);
    e.SetLocation(ITK_LOCATION);
    e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
    e.SetDataObject(inputPtr);
    throw e;
  }
}

template <class TInputImage, class TOutputImage>
void ScalarImageToPanTexTextureFilter<TInputImage, TOutputImage>::ThreadedGenerateData(const OutputRegionType& outputRegionForThread,
                                                                                       itk::ThreadIdType threadId)
{
  // Retrieve the input and output pointers
  InputImagePointerType  inputPtr  = const_cast<InputImageType*>(this->GetInput());
  OutputImagePointerType outputPtr = this->GetOutput();

  itk::ImageRegionIteratorWithIndex<OutputImageType> outputIt(outputPtr, outputRegionForThread);
  outputIt.GoToBegin();

  // Set-up progress reporting
  itk::ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());

  // Iterate on outputs to compute textures
  while (!outputIt.IsAtEnd())
  {
    // Initialise output value
    double out = itk::NumericTraits<double>::max();

    // For each offset
    typename OffsetListType::const_iterator offIt;
    for (offIt = m_OffsetList.begin(); offIt != m_OffsetList.end(); ++offIt)
    {
      OffsetType currentOffset = *offIt;

      // Compute the region on which co-occurence will be estimated
      typename InputRegionType::IndexType inputIndex;
      typename InputRegionType::SizeType  inputSize;

      // First, create an window for neighborhood iterator based on m_Radius
      // For example, if xradius and yradius is 2. window size is 5x5 (2 *
      // radius + 1).
      for (unsigned int dim = 0; dim < InputImageType::ImageDimension; ++dim)
      {
        inputIndex[dim] = outputIt.GetIndex()[dim] - m_Radius[dim];
        inputSize[dim]  = 2 * m_Radius[dim] + 1;
      }
      
      // Build the input region
      InputRegionType inputRegion = {inputIndex, inputSize};
      inputRegion.Crop(inputPtr->GetRequestedRegion());

      SizeType neighborhoodRadius;
      /** calculate minimum offset and set it as neighborhood radius **/
      unsigned int minRadius = 0;
      for (unsigned int i = 0; i < currentOffset.GetOffsetDimension(); i++)
      {
        unsigned int distance = std::abs(currentOffset[i]);
        if (distance > minRadius)
        {
          minRadius = distance;
        }
      }
      neighborhoodRadius.Fill(minRadius);

      CooccurrenceIndexedListPointerType GLCIList = CooccurrenceIndexedListType::New();
      GLCIList->Initialize(m_NumberOfBinsPerAxis, m_InputImageMinimum, m_InputImageMaximum);

      typedef itk::ConstNeighborhoodIterator<InputImageType> NeighborhoodIteratorType;
      NeighborhoodIteratorType neighborIt = NeighborhoodIteratorType(neighborhoodRadius, inputPtr, inputRegion);

      for (neighborIt.GoToBegin(); !neighborIt.IsAtEnd(); ++neighborIt)
      {
        const InputPixelType centerPixelIntensity = neighborIt.GetCenterPixel();
        bool                 pixelInBounds;
        const InputPixelType pixelIntensity = neighborIt.GetPixel(currentOffset, pixelInBounds);
        if (!pixelInBounds)
        {
          continue; // don't put a pixel in the histogram if it's out-of-bounds.
        }
        GLCIList->AddPixelPair(centerPixelIntensity, pixelIntensity);
      }

      VectorConstIteratorType constVectorIt;
      VectorType              glcVector      = GLCIList->GetVector();
      double                  totalFrequency = static_cast<double>(GLCIList->GetTotalFrequency());

      // Compute inertia aka contrast
      double inertia = 0;
      constVectorIt  = glcVector.begin();
      while (constVectorIt != glcVector.end())
      {
        CooccurrenceIndexType index     = constVectorIt->first;
        RelativeFrequencyType frequency = constVectorIt->second / totalFrequency;
        inertia += (index[0] - index[1]) * (index[0] - index[1]) * frequency;
        ++constVectorIt;
      }

      if (inertia < out)
      {
        out = inertia;
      }
    }

    outputIt.Set(out);
    ++outputIt;
    progress.CompletedPixel();
  }
}

} // End namespace otb

#endif
