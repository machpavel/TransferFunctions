#ifndef FILTERS_ITK_CANNY_EDGE_FILTER_H_
#define FILTERS_ITK_CANNY_EDGE_FILTER_H_

#include "../ItkImageFilter.h"
#include "../Constants.h"

#include <itkCannyEdgeDetectionImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkCannyEdgeFilter : public ItkImageFilter<PixelType, Dimension>
{
public:
  typedef itk::Image<float, Dimension> RealImageType;
  typedef itk::CannyEdgeDetectionImageFilter<RealImageType, RealImageType> CannyFilter;
  typedef itk::CastImageFilter<ImageType, RealImageType> CastToRealFilterType;
  typedef itk::CastImageFilter<RealImageType, ImageType> CastToShortFilterType;
  typedef itk::RescaleIntensityImageFilter<ImageType, ImageType> RescaleFilterType;

  ItkCannyEdgeFilter(typename ImagePointer image) : ItkImageFilter(image)
  {
  }

  virtual ~ItkCannyEdgeFilter()
  {
  }

  virtual typename ImagePointer GetFilterImage() override
  {
    CastToRealFilterType::Pointer toReal = CastToRealFilterType::New();
    toReal->SetInput(this->image);

    CannyFilter::Pointer cannyFilter = CannyFilter::New();

    float variance = 0;
    float upperThreshold = 0;
    float lowerThreshold = 0;

    std::cout << "Variance: ";
    std::cin >> variance;

    std::cout << "Upper threshold: ";
    std::cin >> upperThreshold;

    std::cout << "Lower threshold: ";
    std::cin >> lowerThreshold;

    cannyFilter->SetInput(toReal->GetOutput());
    cannyFilter->SetVariance(variance);
    cannyFilter->SetUpperThreshold(upperThreshold);
    cannyFilter->SetLowerThreshold(lowerThreshold);

    CastToShortFilterType::Pointer toShort = CastToShortFilterType::New();
    toShort->SetInput(cannyFilter->GetOutput());


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
    return "Canny edge detection";
  }
};

#endif // FILTERS_ITK_CANNY_EDGE_FILTER_H_