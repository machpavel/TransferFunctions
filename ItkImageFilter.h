#ifndef ITK_IMAGE_FILTERS_H_
#define ITK_IMAGE_FILTERS_H_

#include "itkImage.h"

#include <itkGradientMagnitudeImageFilter.h>
#include <itkHessianRecursiveGaussianImageFilter.h>
#include <itkHessian3DToVesselnessMeasureImageFilter.h>

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkImageFilter
{
public:
  typedef itk::Image<PixelType, Dimension> ImageType;
  typedef typename ImageType::ConstPointer ImageConstPointer;
  typedef itk::GradientMagnitudeImageFilter<ImageType, ImageType> GradientFilterType;
  typedef itk::HessianRecursiveGaussianImageFilter<ImageType, ImageType> HessianFilterType;
  typedef itk::Hessian3DToVesselnessMeasureImageFilter<PixelType> VesselnessMeasureFilterType;

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

  ImageConstPointer GetHessianRecursiveGaussianFilterImage(double sigma) const
  {
    HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
    hessianFilter->SetInput(this->image.GetPointer());
    hessianFilter->SetSigma(sigma);
    hessianFilter->Update();

    return hessianFilter->GetOutput();
  }

  ImageConstPointer GetHessianToVesselnessFilterImage(double hessianSigma, double alpha1, double alpha2)
  {
    VesselnessMeasureFilterType::Pointer vesselnessFilter = VesselnessMeasureFilterType::New();
    vesselnessFilter->SetInput(this->GetHessianRecursiveGaussianFilterImage(hessianSigma));
    vesselnessFilter->SetAlpha1(alpha1);
    vesselnessFilter->SetAlpha2(alpha2);
    vesselnessFilter->Update();

    return vesselnessFilter->GetOutput();
  }

private:
  ImageConstPointer image;
};


#endif // ITK_IMAGE_FILTERS_H_