#ifndef FILTERS_ITK_LOG_FILTER_H_
#define FILTERS_ITK_LOG_FILTER_H_

#include "../ItkImageFilter.h"
#include "../Constants.h"

#include <itkLaplacianRecursiveGaussianImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkAbsImageFilter.h>

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkLoGFilter : public ItkImageFilter<PixelType, Dimension>
{
public:
  typedef itk::Image<double, Dimension> FilteredImageType;
  typedef itk::AbsImageFilter <FilteredImageType, FilteredImageType> AbsImageFilterType;
  typedef itk::RescaleIntensityImageFilter<FilteredImageType, ImageType> RescaleFilterType;
  typedef itk::LaplacianRecursiveGaussianImageFilter<ImageType, FilteredImageType>  FilterType;

  ItkLoGFilter(typename ImagePointer image) : ItkImageFilter(image)
  {
  }

  typename ImagePointer GetLoGFilterImage()
  {
    FilterType::Pointer laplacian = FilterType::New();
    laplacian->SetNormalizeAcrossScale(false);
    laplacian->SetInput(this->image);

    std::cout << "enter sigma value of the gaussian: ";
    double sigma = 0;
    std::cin >> sigma;

    laplacian->SetSigma(sigma);

    AbsImageFilterType::Pointer absFilter = AbsImageFilterType::New();
    absFilter->SetInput(laplacian->GetOutput());

    // remap values
    RescaleFilterType::Pointer rescale = RescaleFilterType::New();
    rescale->SetInput(absFilter->GetOutput());
    rescale->SetOutputMinimum(0);
    rescale->SetOutputMaximum(1000);
    rescale->Update();

    return rescale->GetOutput();
  }

  virtual ~ItkLoGFilter()
  {
  }

  virtual typename ImagePointer GetFilterImage() override
  {
    return this->GetLoGFilterImage();
  }

  virtual std::string GetFilterName() override
  {
    return "Laplacian of Gaussian";
  }
};

#endif // FILTERS_ITK_LOG_FILTER_H_