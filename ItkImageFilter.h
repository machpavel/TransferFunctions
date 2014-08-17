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

  ItkImageFilter(ImageConstPointer image) : image(image)
  {
  }

  virtual ImageConstPointer GetFilterImage() = 0;

  virtual std::string GetFilterName() = 0;

protected:
  ImageConstPointer image;
};


#endif // ITK_IMAGE_FILTERS_H_