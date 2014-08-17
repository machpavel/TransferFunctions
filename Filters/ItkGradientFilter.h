#ifndef FILTERS_ITK_GRADIENT_FILTER_H_
#define FILTERS_ITK_GRADIENT_FILTER_H_

#include "../ItkImageFilter.h"
#include "../Constants.h"

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkGradientFilter : public ItkImageFilter<Constants::GlobalPixelType, Dimension>
{
public:
  typedef itk::GradientMagnitudeImageFilter<ImageType, ImageType> GradientFilterType;

  ItkGradientFilter(typename ImageConstPointer image) : ItkImageFilter(image)
  {
  }

  typename ImageConstPointer GetGradientFilterImage()
  {
    GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
    gradientFilter->SetInput(this->image.GetPointer());
    gradientFilter->Update();

    return gradientFilter->GetOutput();
  }

  virtual ~ItkGradientFilter()
  {
  }

  virtual typename ImageConstPointer GetFilterImage() override
  {
    return this->GetGradientFilterImage();
  }

  virtual std::string GetFilterName() override
  {
    return "Gradient";
  }
};

#endif // FILTERS_ITK_GRADIENT_FILTER_H_