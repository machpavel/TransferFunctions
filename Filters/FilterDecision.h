#ifndef FILTERS_FILTER_DESICION_H_
#define FILTERS_FILTER_DESICION_H_

#include "../ItkImageFilter.h"
#include "ItkGradientFilter.h"
#include "ItkHessianToVesselnessFilter.h"

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class FilterDecision
{
public:

  typedef ItkImageFilter<PixelType, Dimension> ItkImageFilterType;

  static ItkImageFilterType* GetFilter(const std::string& filterName, typename ItkImageFilter<PixelType, Dimension>::ImageConstPointer image)
  {
    if (filterName == "gradient")
    {
      return new ItkGradientFilter<PixelType, Dimension>(image);
    }
    else if (filterName == "hessian to vesselness")
    {
      return new ItkHessianToVesselnessFilter<PixelType, Dimension>(image);
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
  }
};

#endif // FILTERS_FILTER_DECISION_H_