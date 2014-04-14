#ifndef ITK_IMAGE_FILTERS_H_
#define ITK_IMAGE_FILTERS_H_

#include "itkImage.h"

#include "itkGradientMagnitudeImageFilter.h"

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkImageFilter
{
public:
  typedef itk::Image<PixelType, Dimension> ImageType;
  typedef typename ImageType::ConstPointer ImageConstPointer;
  typedef itk::GradientMagnitudeImageFilter<ImageType, ImageType> GradientFilterType;

  ItkImageFilter(ImageConstPointer image) : image(image)
  {
  }

  ImageConstPointer GetGradientFilterImage()
  {
    GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
    gradientFilter->SetInput( image.GetPointer() );
    gradientFilter->Update();

    return gradientFilter->GetOutput();
  }

private:
  ImageConstPointer image;
};


#endif // ITK_IMAGE_FILTERS_H_