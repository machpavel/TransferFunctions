#ifndef FILTERS_ITK_GAUSSIAN_FILTER_H_
#define FILTERS_ITK_GAUSSIAN_FILTER_H_

#include "../ItkImageFilter.h"
#include "../Constants.h"

#include <itkSmoothingRecursiveGaussianImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkGaussianFilter : public ItkImageFilter<PixelType, Dimension>
{
public:
  typedef itk::SmoothingRecursiveGaussianImageFilter<ImageType, ImageType> GaussianFilterType;

  ItkGaussianFilter(typename ImagePointer image) : ItkImageFilter(image)
  {
  }

  virtual ~ItkGaussianFilter()
  {
  }

  virtual typename ImagePointer GetFilterImage() override
  {
    GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
    gaussianFilter->SetInput(this->image.GetPointer());

    double sigma = 0;
    std::cout << "Enter gaussian sigma: ";
    std::cin >> sigma;

    gaussianFilter->SetSigma(sigma);
    gaussianFilter->Update();

    return gaussianFilter->GetOutput();
  }

  virtual std::string GetFilterName() override
  {
    return "Gaussian";
  }
};

#endif // FILTERS_ITK_GAUSSIAN_FILTER_H_