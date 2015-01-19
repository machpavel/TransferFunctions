#ifndef FILTERS_FILTER_DESICION_H_
#define FILTERS_FILTER_DESICION_H_

#include "../ItkImageFilter.h"
#include "ItkGradientFilter.h"
#include "ItkHessianToVesselnessFilter.h"
#include "ItkEigenValuesFilter.h"
#include "ItkEigenValuesRaw.h"
#include "ItkCroppingFilter.h"
#include "../ImageDumpSerializer.h"

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class FilterDecision
{
public:

  typedef ItkImageFilter<PixelType, Dimension> ItkImageFilterType;

  static ItkImageFilterType* GetFilter(const std::string& filterName, typename ItkImageFilter<PixelType, Dimension>::ImagePointer image, ImageDumpSerializer<>* serializer)
  {
    if (filterName == "gradient")
    {
      return new ItkGradientFilter<PixelType, Dimension>(image);
    }
    else if (filterName == "hessian to vesselness")
    {
      return new ItkHessianToVesselnessFilter<PixelType, Dimension>(image);
    }
    else if (filterName == "hessian to eigenvalues")
    {
      return new ItkEigenValuesFilter<PixelType, Dimension>(image, serializer);
    }
    else if (filterName == "hessian to raw eigenvalues")
    {
      return new ItkEigenValuesRaw<PixelType, Dimension>(image, serializer);
    }
    else if (filterName == "cropping filter")
    {
      return new ItkCroppingFilter<PixelType, Dimension>(image);
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
  }
};

#endif // FILTERS_FILTER_DECISION_H_