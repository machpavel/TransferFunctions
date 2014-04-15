#ifndef ITK_IMAGE_FILTERS_H_
#define ITK_IMAGE_FILTERS_H_

#include "itkImage.h"

#include <itkGradientMagnitudeImageFilter.h>
#include <itkHessianRecursiveGaussianImageFilter.h>

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkImageFilter
{
public:
  typedef itk::Image<PixelType, Dimension> ImageType;
  typedef typename ImageType::ConstPointer ImageConstPointer;
  typedef itk::GradientMagnitudeImageFilter<ImageType, ImageType> GradientFilterType;
  typedef itk::HessianRecursiveGaussianImageFilter<ImageType, ImageType> HessianFilterType;

  ItkImageFilter(ImageConstPointer image) : image(image)
  {
  }

  ImageConstPointer GetGradientFilterImage()
  {
    GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
    gradientFilter->SetInput(this->image.GetPointer());
    gradientFilter->Update();

    return gradientFilter->GetOutput();
  }

  ImageConstPointer GetHessianRecursiveGaussianFilterImage(double sigma)
  {
    HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
    hessianFilter->SetInput(this->image.GetPointer());
    hessianFilter->SetSigma(sigma);
    hessianFilter->Update();

    return hessianFilter->GetOutput();
  }

private:
  ImageConstPointer image;
};


#endif // ITK_IMAGE_FILTERS_H_