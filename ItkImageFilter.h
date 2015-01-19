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
  typedef typename ImageType::Pointer ImagePointer;

  ItkImageFilter(ImagePointer image) : image(image)
  {
  }

  virtual ImagePointer GetFilterImage() = 0;

  virtual std::string GetFilterName() = 0;

protected:
  ImagePointer image;
};


#endif // ITK_IMAGE_FILTERS_H_