/*=========================================================================

  Program:   Monteverdi2
  Language:  C++


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See Copyright.txt for details.

  Monteverdi2 is distributed under the CeCILL licence version 2. See
  Licence_CeCILL_V2-en.txt or
  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt for more details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mvdVectorImageModel.h"


/*****************************************************************************/
/* INCLUDE SECTION                                                           */

//
// Qt includes (sorted by alphabetic order)
//// Must be included before system/custom includes.

//
// System includes (sorted by alphabetic order)

//
// ITK includes (sorted by alphabetic order)
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itksys/SystemTools.hxx"

//
// OTB includes (sorted by alphabetic order)
#include "otbConfigure.h"
#include "otbGDALDriverManagerWrapper.h"
#include "otbStandardOneLineFilterWatcher.h"

//
// Monteverdi includes (sorted by alphabetic order)
#include "mvdAlgorithm.h"
#include "mvdQuicklookModel.h"


namespace mvd
{
/*
  TRANSLATOR mvd::VectorImageModel

  Necessary for lupdate to be aware of C++ namespaces.

  Context comment for translator.
*/


/*****************************************************************************/
/* CLASS IMPLEMENTATION SECTION                                              */

/*******************************************************************************/
VectorImageModel
::VectorImageModel( QObject* parent ) :
  AbstractImageModel( parent ),
  m_Image(),
  m_ImageFileReader(),
  m_RasterizedBuffer( NULL ),
  m_ExtractFilter(),
  m_RenderingFilter(),
  m_Settings(),
  m_PreviousRegion(),
  m_AlreadyLoadedRegion(),
  m_Region(),
  m_RegionsToLoadVector(),
  m_Filename()
{
}

/*******************************************************************************/
VectorImageModel
::~VectorImageModel()
{
  this->ClearBuffer();
}

/*******************************************************************************/
void
VectorImageModel
::SetFilename( const QString& filename , int w, int h)
{
  // 1. store the input filename
  m_Filename = filename;

  // Get the largest possible region of the image
  m_ImageFileReader = DefaultImageFileReaderType::New();

  m_ImageFileReader->SetFileName( m_Filename.toAscii().constData() );
  m_ImageFileReader->UpdateOutputInformation();

  // Build overviews if necessary
  bool hasOverviewsSupport = m_ImageFileReader->HasOverviewsSupport();
  int nbOfAvailableOvw = m_ImageFileReader->GetNbOfAvailableOverviews();
  bool forceToCacheOvw = true;

  if (!hasOverviewsSupport)
    { // the current file don't have overviews available and the ImageIO don't support overviews
    throw std::runtime_error(tr( "The ImageIO use to read this file don't support Overviews !" ).toLatin1().constData());
    }
  else
    {
    std::cout << "The ImageIO use to read this file support Overviews !" << std::endl;

    // TODO MSD: how to manage case of JPEG2000 with no overviews ? : wait GDAL support OpenJPEG ...
    if (nbOfAvailableOvw == 0)
      { // The current file don't have overviews available
      std::cout << "The file don't have overviews !" << std::endl;
      if (forceToCacheOvw)
        { // the user want to cache the overviews
        std::cout << "Caching of overviews !" << std::endl;
        typedef otb::GDALOverviewsBuilder FilterType;
        FilterType::Pointer filter = FilterType::New();

        //m_ImageFileReader->GetAvailableResolutions(m_AvailableLod);

        std::string tempfilename(m_Filename.toAscii().constData());

        filter->SetInputFileName(tempfilename);
        filter->SetNbOfResolutions(m_ImageFileReader->GetAvailableResolutions().size());
          {
          otb::StandardOneLineFilterWatcher watcher(filter,  tr("Overviews creation: " ).toLatin1().constData());
          filter->Update();
          }

        }
      else
        { // the user don't want to cache the overviews, GDAL will virtually compute the ovw on demand
        std::cout << "Keep GDAL decimate the file on demand !" << std::endl;
        }
      }
    else
      {
      std::cout << "The file have already overviews !" << std::endl;
      }
    }


  // Retrieve the list of Lod from file
  m_AvailableLod = m_ImageFileReader->GetAvailableResolutions();

  // Remember native largest region.
  m_NativeLargestRegion =
      m_ImageFileReader->GetOutput()->GetLargestPossibleRegion();

  // Remember native spacing
  m_NativeSpacing = m_ImageFileReader->GetOutput()->GetSpacing();


  //
  // 2. Setup file-reader.
  SetupCurrentLodImage(w, h);
}

/*******************************************************************************/
void
VectorImageModel
::virtual_BuildModel( void* context )
{
  //
  // Step #1: Perform pre-process of AbstractModel::BuildModel()
  // pattern.

  // Store default display settings in the pre-process stage
  // i.e. before histogram is generated by the standard
  // AbstractImageModel::BuildModel().
  InitializeColorSetupSettings();

  //
  // Step #2: Perform standard AbstractModel::BuildModel()
  // pattern. Call parent virtual method.

  // The call to the parent BuildModel() method will, for example,
  // generate the histogram.
  AbstractImageModel::virtual_BuildModel( context );

  //
  // Step #3: Post-process of the BuildModel() pattern.

  // Store min/max pixel for color-dynamics once histogram has been
  // generated.
  InitializeColorDynamicsSettings();

  // Initialize RgbaImageModel.
  InitializeRgbaPipeline();
}

/*******************************************************************************/
void
VectorImageModel
::InitializeColorSetupSettings()
{
  // Remember meta-data interface.
  otb::ImageMetadataInterfaceBase::ConstPointer metaData(
    GetMetaDataInterface()
  );

  // Ensure default display returns valid band indices (see OTB bug).
  assert( metaData->GetDefaultDisplay().size()==3 );
#if 0
  assert( metaData->GetDefaultDisplay()[ 0 ]
	  < m_Image->GetNumberOfComponentsPerPixel() );
  assert( metaData->GetDefaultDisplay()[ 1 ]
	  < m_Image->GetNumberOfComponentsPerPixel() );
  assert( metaData->GetDefaultDisplay()[ 2 ]
	  < m_Image->GetNumberOfComponentsPerPixel() );
#endif

  // Patch invalid band indices of default-display (see OTB bug).
  Settings::ChannelVector rgb( metaData->GetDefaultDisplay() );

  if( rgb[ 0 ]>=m_Image->GetNumberOfComponentsPerPixel() )
    {
    rgb[ 0 ] = 0;
    }

  if( rgb[ 1 ]>=m_Image->GetNumberOfComponentsPerPixel() )
    {
    rgb[ 1 ] = 0;
    }

  if( rgb[ 2 ]>=m_Image->GetNumberOfComponentsPerPixel() )
    {
    rgb[ 2 ] = 0;
    }

  // Store default display settings.
  GetSettings().SetRgbChannels( rgb );
}

/*******************************************************************************/
void
VectorImageModel
::InitializeColorDynamicsSettings()
{
  // Get the histogram-model.
  HistogramModel* histogramModel = GetHistogramModel();
  assert( histogramModel!=NULL );

  // Remember min/max pixels.
  DefaultImageType::PixelType min( histogramModel->GetMinPixel() );
  DefaultImageType::PixelType max( histogramModel->GetMaxPixel() );

  // Store min/max intensities of default-display channels.
  for( CountType i=0; i<RGBA_CHANNEL_ALPHA; ++i )
    {
    Settings::ChannelVector::value_type band =
      GetSettings().RgbChannel( static_cast< RgbaChannel >( i ) );

    ParametersType::ValueType index = 2 * i;

    GetSettings().DynamicsParam( index ) = min[ band ];
    GetSettings().DynamicsParam( index + 1 ) = max[ band ];
    }
}

/*******************************************************************************/
void
VectorImageModel
::InitializeRgbaPipeline()
{
  m_ExtractFilter = ExtractFilterType::New();
  m_ExtractFilter->SetInput( m_Image );

  m_RenderingFilter = RenderingFilterType::New();
  m_RenderingFilter->SetInput( m_ExtractFilter->GetOutput() );
}

/*******************************************************************************/
void
VectorImageModel
::ClearBuffer()
{
  // Delete previous buffer if needed
  delete[] m_RasterizedBuffer;
  m_RasterizedBuffer = NULL;
}

/*******************************************************************************/
unsigned char *
VectorImageModel
::RasterizeRegion( const ImageRegionType& region, const double zoomFactor, bool refresh)
{
  m_Region = region;

  // Compute the best level of detail
  CountType bestLod = this->ComputeBestLevelOfDetail(zoomFactor);

  // Set the corresponding Level of Detail
  if( GetCurrentLod()!=bestLod )
    {
    this->SetCurrentLod(bestLod);
    }

  // Don't do anything if the region did not changed
  if ( m_PreviousRegion!=region ||
       GetSettings().IsDirty() ||
       refresh )
    {
    // check that the current and the previous region have some pixels in
    // common 
    ImageRegionType  tempPreviousRegionRegion = m_PreviousRegion;
    bool res =  tempPreviousRegionRegion.Crop(region);

    // if the first time or no pixels in common , reload all
    if ( res &&
	 m_PreviousRegion!=ImageRegionType() &&
	 !GetSettings().IsDirty() &&
         !refresh )
      {
      // Compute loaded region, and the four regions not loaded yet
      // within the new requested region
      this->ComputeRegionsToLoad(m_Region);

      // Copy the previous buffer into a temporary buf to access the
      // previously loaded data
      unsigned char * previousRasterizedBuffer =  
        new unsigned char[3 * m_PreviousRegion.GetNumberOfPixels()];

      std::memcpy(previousRasterizedBuffer, m_RasterizedBuffer, 3 * m_PreviousRegion.GetNumberOfPixels());

      // Clear the previous buffer 
      this->ClearBuffer();

      // Allocate new memory
      unsigned int nbPixels = 3 * region.GetNumberOfPixels();
      m_RasterizedBuffer = new unsigned char[nbPixels];
    
      // Copy the already loaded pixels into the m_RasterizedBuffer
      unsigned int previousNbPixels = m_PreviousRegion.GetNumberOfPixels();
      
      for (unsigned int idx = 0; idx < previousNbPixels; idx++)
        {
        // compose the image index from the buffer index
        IndexType imageIndex;
        imageIndex = ComputeImageIndexFromFlippedBuffer(idx, m_PreviousRegion);

        if (m_AlreadyLoadedRegion.IsInside(imageIndex))
          {
          // Get the buffer index relative to the imageIndex in the
          // new requested region 
          unsigned int newBufferindex = 0;
          newBufferindex = ComputeXAxisFlippedBufferIndex(imageIndex, m_Region);
          
          // Copy the already loaded values into the new buffer
          m_RasterizedBuffer[newBufferindex]     = previousRasterizedBuffer[3*idx];
          m_RasterizedBuffer[newBufferindex + 1] = previousRasterizedBuffer[3*idx + 1];
          m_RasterizedBuffer[newBufferindex + 2] = previousRasterizedBuffer[3*idx + 2];
          }
        }
      
      // Dump pixels not loaded in the m_RasterizedBuffer
      for (unsigned int idx = 0; idx < m_RegionsToLoadVector.size() ; idx++)
        {
        this->DumpImagePixelsWithinRegionIntoBuffer(m_RegionsToLoadVector[idx]);
        }

      // free the previous buffer memory (copied one)
      if (previousRasterizedBuffer != NULL)
        {
        delete[] previousRasterizedBuffer;
        previousRasterizedBuffer = NULL;
        }
      }
    else
      {
      // Delete previous buffer if needed
      this->ClearBuffer();
 
      // Allocate new memory
      m_RasterizedBuffer = new unsigned char[3 * region.GetNumberOfPixels()];

      // rasterize the region
      this->DumpImagePixelsWithinRegionIntoBuffer(region);
      }
    }

  // settings changes have been taken into account, clean the dirty flag
  GetSettings().ClearDirty();

  // Store the region
  m_PreviousRegion = region;
  
  // if ok return the  buffer
  return m_RasterizedBuffer;
}

/*******************************************************************************/
void
VectorImageModel
::DumpImagePixelsWithinRegionIntoBuffer(const ImageRegionType& region)
{
  // Before doing anything, check if region is inside the buffered
  // region of image

  // TODO : add some checking

  assert( !m_Image.IsNull() );

  // some checking
  if (!m_Image->GetBufferedRegion().IsInside(region))
    {
    //itkExceptionMacro(<< "Region to read is oustside of the buffered region.");
    }

  assert( !m_ExtractFilter.IsNull() );

  // Extract the region of interest in the m_Image
  m_ExtractFilter->SetInput( m_Image );
  m_ExtractFilter->SetExtractionRegion(region);

  assert( !m_RenderingFilter.IsNull() );

  // Use the rendering filter to get 
  m_RenderingFilter->GetRenderingFunction()->SetAutoMinMax(false);

// ----------------------------------
  // RenderingFilterType::RenderingFunctionType::ParametersType  paramsMinMax;
  // paramsMinMax.SetSize(6);
    
  // // Update the parameters
  // for (unsigned int i = 0; i < paramsMinMax.Size(); i = i + 2)
  //   {
  //   paramsMinMax.SetElement(i, 30);
  //   paramsMinMax.SetElement(i + 1, 2048/*256*/);
  //   }
  //m_RenderingFilter->GetRenderingFunction()->SetParameters(paramsMinMax);
// ---------------------------- 

  // TODO: Remove local variable.
  // Local variable because RenderingFunction::SetChannels() gets a
  // non-const std::vector< unsigned int >& as argument instead of a
  // const one.
  /*
    // Done in VectorImageModel::OnModelUpdated() slot.
  Settings::ChannelVector rgb( m_Settings.GetRgbChannels() );
  m_RenderingFilter->GetRenderingFunction()->SetChannelList(
    rgb
  );
  */
  m_RenderingFilter->GetOutput()->SetRequestedRegion(region);
  m_RenderingFilter->Update();

  // Declare the iterator
  itk::ImageRegionConstIteratorWithIndex< RenderingFilterType::OutputImageType >
    it(m_RenderingFilter->GetOutput(), region);

  // Go to begin
  it.GoToBegin();

  while (!it.IsAtEnd())
    {
    // Fill the buffer
    unsigned int index = 0;
    index = ComputeXAxisFlippedBufferIndex(it.GetIndex(), m_Region);

    // Fill the buffer
    m_RasterizedBuffer[index]  = it.Get()[0];
    m_RasterizedBuffer[index + 1] = it.Get()[1];
    m_RasterizedBuffer[index + 2] = it.Get()[2];
    ++it;
    }
}

/*******************************************************************************/
void
VectorImageModel
::ComputeRegionsToLoad(const ImageRegionType& region)
{
  // Initialize the region and clear vector
  m_AlreadyLoadedRegion = m_PreviousRegion;
  m_RegionsToLoadVector.clear();

  // 4 regions to compute
  ImageRegionType upperRegion;
  ImageRegionType lowerRegion;
  ImageRegionType leftRegion;
  ImageRegionType rightRegion;

  // local variables
  IndexType index;
  ImageRegionType::SizeType size;

  // Compute the already loaded region as a simple Crop
  m_AlreadyLoadedRegion.Crop(region);

  // ------ upper region
  upperRegion.SetIndex(region.GetIndex());
  size[0] = region.GetSize()[0];
  size[1] = vcl_abs( region.GetIndex()[1] - m_AlreadyLoadedRegion.GetIndex()[1]);
  upperRegion.SetSize(size);

  // ------ lower region
  index[0] = region.GetIndex()[0];
  index[1] = m_AlreadyLoadedRegion.GetSize()[1] + m_AlreadyLoadedRegion.GetIndex()[1];
  lowerRegion.SetIndex(index);

  size[0] = region.GetSize()[0];
  size[1] = vcl_abs( static_cast<int>(region.GetIndex()[1] + region.GetSize()[1] 
                     - m_AlreadyLoadedRegion.GetIndex()[1] - m_AlreadyLoadedRegion.GetSize()[1] ));
  lowerRegion.SetSize(size);

  // ------- left region
  index[0] = region.GetIndex()[0];
  index[1] = m_AlreadyLoadedRegion.GetIndex()[1];
  leftRegion.SetIndex(index);

  size[0] = vcl_abs(region.GetIndex(0) - m_AlreadyLoadedRegion.GetIndex()[0]);
  size[1] = m_AlreadyLoadedRegion.GetSize()[1];
  leftRegion.SetSize(size);

  // -------- right region
  index[0] = m_AlreadyLoadedRegion.GetIndex()[0] + m_AlreadyLoadedRegion.GetSize()[0];
  index[1] = m_AlreadyLoadedRegion.GetIndex()[1];
  rightRegion.SetIndex(index);

size[0] = vcl_abs(static_cast<int>(region.GetSize()[0] + region.GetIndex()[0] 
                                   - m_AlreadyLoadedRegion.GetIndex()[0] - m_AlreadyLoadedRegion.GetSize()[0]));
  size[1] = m_AlreadyLoadedRegion.GetSize()[1]; 
  rightRegion.SetSize(size);

  // add the upper region if any pixel
  if ( upperRegion .GetNumberOfPixels() > 0 )
    m_RegionsToLoadVector.push_back(upperRegion);

  // add the lower region if any pixel
  if ( lowerRegion.GetNumberOfPixels() > 0 )
    m_RegionsToLoadVector.push_back(lowerRegion);

  // add the left region if any pixel
  if ( leftRegion.GetNumberOfPixels() > 0 )
    m_RegionsToLoadVector.push_back(leftRegion);

  // add the right region if any pixel
  if ( rightRegion.GetNumberOfPixels() > 0 )
    m_RegionsToLoadVector.push_back(rightRegion);
}

/*******************************************************************************/
CountType
VectorImageModel::ComputeBestLevelOfDetail(const double zoomFactor)
{
  int inverseZoomFactor =  static_cast<int>((1/zoomFactor + 0.5));
  CountType bestLod = this->Closest(inverseZoomFactor, m_AvailableLod);
  return bestLod;
}

/*******************************************************************************/
unsigned int 
VectorImageModel::Closest(double invZoomfactor, const std::vector<unsigned int> & res) 
{
  double minDist       = 50000.;
  unsigned int closest = 0;

  // Compute the diff and keep the index that minimize the distance
  for (unsigned int idx = 0; idx < res.size(); idx++)
    {
    double diff = vcl_abs((double)(1<<idx) - invZoomfactor);

    if (diff < minDist)
      {
      minDist = diff;
      closest = idx;
      }
    }

  return closest;
}

/*******************************************************************************/
void
VectorImageModel
::SetupCurrentLodImage( int width, int height )
{
  CountType bestInitialLod = 0;
  // Compute the initial zoom factor and the best LOD.
  if( width>0 && height>0 )
    {
    ImageRegionType nativeLargestRegion( GetNativeLargestRegion() );

    double factorX =
      double( width ) / double( nativeLargestRegion.GetSize()[ 0 ] );

    double factorY =
      double( height ) / double( nativeLargestRegion.GetSize()[ 1 ] );

    double initialZoomFactor = std::min(factorX, factorY);

    // Compute the best lod from the initialZoomFactor
    bestInitialLod = ComputeBestLevelOfDetail(initialZoomFactor);
    }

    this->SetCurrentLod( bestInitialLod );
}

/*******************************************************************************/
CountType
VectorImageModel
::GetNbLod() const
{
  return m_AvailableLod.size();
}

/*******************************************************************************/
void
VectorImageModel
::virtual_SetCurrentLod( CountType lod )
{
  // new filename if lod is not 0
  QString lodFilename( m_Filename );

  // If model is a multi-resolution image.
  lodFilename.append( QString( "?&resol=%1" ).arg( lod ) );

  // Update m_ImageFileReader
  DefaultImageFileReaderType::Pointer fileReader(
    DefaultImageFileReaderType::New()
  );

  try
    {
    fileReader->SetFileName( static_cast<const char*>(lodFilename.toAscii()) );
    fileReader->UpdateOutputInformation();

    m_ImageFileReader = fileReader;
    }
  catch( std::exception& exc )
    {
    throw;
    }

  // (Always) Update m_Image reference.
  m_Image = m_ImageFileReader->GetOutput();
}

/*******************************************************************************/
ImageBaseType::ConstPointer
VectorImageModel
::ToImageBase() const
{
  return ImageBaseType::ConstPointer( m_Image );
}

/*******************************************************************************/
ImageBaseType::Pointer
VectorImageModel
::ToImageBase()
{
  return ImageBaseType::Pointer( m_Image );
}

/*******************************************************************************/
/* SLOTS                                                                       */
/*******************************************************************************/
void
VectorImageModel
::OnModelUpdated()
{
  qDebug() << "OnModelUpdated()";

  RenderingFilterType::RenderingFunctionType* renderingFunc =
    m_RenderingFilter->GetRenderingFunction();

  // TODO: Remove local variable.
  // Local variable because RenderingFunction::SetChannels() gets a
  // non-const std::vector< unsigned int >& as argument instead of a
  // const one.
  Settings::ChannelVector rgb( GetSettings().GetRgbChannels() );

  renderingFunc->SetChannelList( rgb );
  renderingFunc->SetParameters( GetSettings().GetDynamicsParams() );

  // TODO: Remove temporary hack (rendering settings).
  QuicklookModel* quicklookModel = GetQuicklookModel();
  if( quicklookModel!=NULL )
    {
    quicklookModel->SetSettings( GetSettings() );
    quicklookModel->OnModelUpdated();
    }

  emit SettingsUpdated();
}

} // end namespace 'mvd'
