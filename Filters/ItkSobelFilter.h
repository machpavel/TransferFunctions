#ifndef FILTERS_ITK_SOBEL_FILTER_H_
#define FILTERS_ITK_SOBEL_FILTER_H_

#include "../ItkImageFilter.h"
#include "../Constants.h"

#include <itkSobelEdgeDetectionImageFilter.h>

#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkSobelFilter : public ItkImageFilter<PixelType, Dimension>
{
public:
  typedef itk::Image<float, Dimension> RealImageType;
  typedef itk::SobelEdgeDetectionImageFilter<ImageType, RealImageType> SobelEdgeDetectionImageFilterType;
  typedef itk::CastImageFilter<ImageType, RealImageType> CastToRealFilterType;
  typedef itk::CastImageFilter<RealImageType, ImageType> CastToShortFilterType;
  typedef itk::RescaleIntensityImageFilter<ImageType, ImageType> RescaleFilterType;

  ItkSobelFilter(typename ImagePointer image) : ItkImageFilter(image)
  {
  }

  virtual ~ItkSobelFilter()
  {
  }

  virtual typename ImagePointer GetFilterImage() override
  {
    CastToRealFilterType::Pointer toReal = CastToRealFilterType::New();
    toReal->SetInput(this->image);

    SobelEdgeDetectionImageFilterType::Pointer sobelFilter = SobelEdgeDetectionImageFilterType::New();

    sobelFilter->SetInput(image);

    CastToShortFilterType::Pointer toShort = CastToShortFilterType::New();
    toShort->SetInput(sobelFilter->GetOutput());


    // remap values
    RescaleFilterType::Pointer rescale = RescaleFilterType::New();
    rescale->SetInput(toShort->GetOutput());
    rescale->SetOutputMinimum(0);
    rescale->SetOutputMaximum(2000);
    rescale->Update();

    return rescale->GetOutput();
  }

  virtual std::string GetFilterName() override
  {
    return "Sobel edge detection";
  }
};

#endif // FILTERS_ITK_SOBEL_FILTER_H_