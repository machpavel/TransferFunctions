#ifndef FILTERS_ITK_HESSIAN_TO_VESSELNESS_FILTER_H_
#define FILTERS_ITK_HESSIAN_TO_VESSELNESS_FILTER_H_

#include "../ItkImageFilter.h"
#include "../Constants.h"

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkHessianToVesselnessFilter : public ItkImageFilter<Constants::GlobalPixelType, Dimension>
{
public:
  typedef itk::Image<itk::SymmetricSecondRankTensor< double, 3 >, 3> HessianOutputType;
  typedef itk::HessianRecursiveGaussianImageFilter<typename ItkImageFilter::ImageType, HessianOutputType> HessianFilterType;
  typedef itk::Hessian3DToVesselnessMeasureImageFilter<PixelType> VesselnessMeasureFilterType;


  ItkHessianToVesselnessFilter(typename ItkImageFilter::ImageConstPointer image) : ItkImageFilter(image)
  {
  }


  HessianOutputType::ConstPointer GetHessianRecursiveGaussianFilterImage(double sigma)
  {
    HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
    hessianFilter->SetInput(this->image.GetPointer());
    hessianFilter->SetSigma(sigma);
    hessianFilter->Update();

    return hessianFilter->GetOutput();
  }

  typename ItkImageFilter::ImageConstPointer GetHessianToVesselnessFilterImage(double hessianSigma, double alpha1, double alpha2)
  {
    VesselnessMeasureFilterType::Pointer vesselnessFilter = VesselnessMeasureFilterType::New();

    vesselnessFilter->SetInput(this->GetHessianRecursiveGaussianFilterImage(hessianSigma));
    vesselnessFilter->SetAlpha1(alpha1);
    vesselnessFilter->SetAlpha2(alpha2);
    vesselnessFilter->Update();

    return vesselnessFilter->GetOutput();
  }

  typename ItkImageFilter::ImageConstPointer GetHessianToVesselnessFilterImage()
  {
    double hessianSigma = 0;
    double alpha1 = 0;
    double alpha2 = 0;

    std::cout << "enter sigma value" << std::endl;
    std::cin >> hessianSigma;

    std::cout << "enter alpha1 value" << std::endl;
    std::cin >> alpha1;

    std::cout << "enter alpha2 value" << std::endl;
    std::cin >> alpha2;

    return this->GetHessianToVesselnessFilterImage(hessianSigma, alpha1, alpha2);
  }

  virtual ~ItkHessianToVesselnessFilter()
  {
  }

  virtual typename ImageConstPointer GetFilterImage() override
  {
    return this->GetHessianToVesselnessFilterImage();
  }

  virtual std::string GetFilterName() override
  {
    return "HessianToVesselness";
  }
};

#endif // FILTERS_ITK_HESSIAN_TO_VESSELNESS_FILTER_H_