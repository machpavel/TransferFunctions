#ifndef FILTERS_ITK_HESSIAN_TO_VESSELNESS_FILTER_H_
#define FILTERS_ITK_HESSIAN_TO_VESSELNESS_FILTER_H_

#include "../ItkImageFilter.h"
#include "../Constants.h"

#include <itkRescaleIntensityImageFilter.h>

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>

class ItkHessianToVesselnessFilter :
  public ItkImageFilter<PixelType, Dimension>
{
public:
  typedef itk::SymmetricSecondRankTensor< double, 3 > TensorType;
  typedef itk::RescaleIntensityImageFilter<ImageType, ImageType> RescaleFilterType;
  typedef itk::Image<TensorType, 3> HessianOutputType;
  typedef itk::HessianRecursiveGaussianImageFilter<typename ItkImageFilter::ImageType, HessianOutputType> HessianFilterType;
  typedef itk::Hessian3DToVesselnessMeasureImageFilter<PixelType> VesselnessMeasureFilterType;

  ItkHessianToVesselnessFilter(typename ItkImageFilter::ImagePointer image) : ItkImageFilter(image)
  {
  }
  
  typename ItkImageFilter::ImagePointer GetHessianToVesselnessFilterImage()
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

  virtual typename ImagePointer GetFilterImage() override
  {
    return this->GetHessianToVesselnessFilterImage();
  }

  virtual std::string GetFilterName() override
  {
    return "HessianToVesselness";
  }

private:

  HessianOutputType::Pointer GetHessianRecursiveGaussianFilterImage(double sigma)
  {
    HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
    hessianFilter->SetInput(this->image.GetPointer());
    hessianFilter->SetSigma(sigma);
    hessianFilter->Update();    

    return hessianFilter->GetOutput();
  }

  typename ItkImageFilter::ImagePointer GetHessianToVesselnessFilterImage(double hessianSigma, double alpha1, double alpha2)
  {
    VesselnessMeasureFilterType::Pointer vesselnessFilter = VesselnessMeasureFilterType::New();

    HessianOutputType::Pointer hessianOutput = this->GetHessianRecursiveGaussianFilterImage(hessianSigma);

    vesselnessFilter->SetInput(hessianOutput);
    vesselnessFilter->SetAlpha1(alpha1);
    vesselnessFilter->SetAlpha2(alpha2);
    vesselnessFilter->Update();


    // remap values
    RescaleFilterType::Pointer rescale = RescaleFilterType::New();
    rescale->SetInput(vesselnessFilter->GetOutput());
    rescale->SetOutputMinimum(0);
    rescale->SetOutputMaximum(1000);
    rescale->Update();

    return rescale->GetOutput();
  }
};

#endif // FILTERS_ITK_HESSIAN_TO_VESSELNESS_FILTER_H_