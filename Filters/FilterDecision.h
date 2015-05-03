#ifndef FILTERS_FILTER_DESICION_H_
#define FILTERS_FILTER_DESICION_H_

#include "../ItkImageFilter.h"
#include "ItkGradientFilter.h"
#include "ItkGaussianFilter.h"
#include "ItkHessianToVesselnessFilter.h"
#include "ItkEigenValuesFilter.h"
#include "ItkEigenValuesRaw.h"
#include "ItkCroppingFilter.h"
#include "ItkLoGFilter.h"
#include "ItkCannyFilter.h"
#include "ItkSobelFilter.h"
#include "CheckerFilter.h"
#include "ArtificialObjectsFilter.h"
#include "../ImageDumpSerializer.h"

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class FilterDecision
{
public:

  typedef ItkImageFilter<PixelType, Dimension> ItkImageFilterType;

  static ItkImageFilterType* GetFilter (
    const std::string& filterName,
    typename ItkImageFilter<PixelType, Dimension>::ImagePointer image,
    ImageDumpSerializer<PixelType, Dimension>* serializer,
    std::string secondaryFilename)
  {
    if (filterName == "gradient")
    {
      return new ItkGradientFilter<PixelType, Dimension>(image);
    }
    if (filterName == "gaussian")
    {
      return new ItkGaussianFilter<PixelType, Dimension>(image);
    }
    else if (filterName == "hessian to vesselness")
    {
      return new ItkHessianToVesselnessFilter<PixelType, Dimension>(image);
    }
    else if (filterName == "hessian to eigenvalues")
    {
      return new ItkEigenValuesFilter<PixelType, Dimension>(image, secondaryFilename, serializer);
    }
    else if (filterName == "hessian to raw eigenvalues")
    {
      return new ItkEigenValuesRaw<PixelType, Dimension>(image, secondaryFilename, serializer);
    }
    else if (filterName == "cropping")
    {
      return new ItkCroppingFilter<PixelType, Dimension>(image);
    }
    else if (filterName == "LoG")
    {
      return new ItkLoGFilter<PixelType, Dimension>(image);
    }
    else if (filterName == "canny")
    {
      return new ItkCannyEdgeFilter<PixelType, Dimension>(image);
    }
    else if (filterName == "sobel")
    {
      return new ItkSobelFilter<PixelType, Dimension>(image);
    }
    else if (filterName == "checker")
    {
      return new CheckerFilter<PixelType, Dimension>(image);
    }
    else if (filterName == "artificial")
    {
      return new ArtificialObjectsFilter<PixelType, Dimension>(image);
    }
    else
    {
      return nullptr;
    }
  }

  
  static void PrintFilterNames()
  {
    std::cout << "gradient" << std::endl;
    std::cout << "hessian to vesselness" << std::endl;
    std::cout << "hessian to eigenvalues" << std::endl;
    std::cout << "hessian to raw eigenvalues" << std::endl;
    std::cout << "cropping" << std::endl;
  }
};

#endif // FILTERS_FILTER_DECISION_H_