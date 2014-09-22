#ifndef FILTERS_ITK_EIGEN_VALUES_FILTER_H_
#define FILTERS_ITK_EIGEN_VALUES_FILTER_H_

#include "../ItkImageFilter.h"
#include "../Constants.h"

#include <itkImageDuplicator.h>

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkEigenValuesFilter :
  public ItkImageFilter<Constants::GlobalPixelType, Dimension>
{
public:
  typedef itk::SymmetricSecondRankTensor< double, 3 > TensorType;
  typedef itk::Image<TensorType, 3> HessianOutputType;
  typedef itk::HessianRecursiveGaussianImageFilter<typename ItkImageFilter::ImageType, HessianOutputType> HessianFilterType;

  typedef itk::ImageRegionConstIterator<HessianOutputType>  HessianIteratorType;

  typedef itk::Vector<double, 3> EigenValuesType;
  typedef itk::Matrix<double, 3, 3> EigenVectorsType;
  typedef itk::Image<EigenValuesType> EigenValuesCollectionType;
  typedef itk::Image<EigenVectorsType> EigenVectorsCollectionType;

  typedef itk::ImageRegionConstIterator<EigenValuesCollectionType> EigenValuesCollectionIteratorType;
  typedef itk::ImageRegionConstIterator<EigenVectorsCollectionType> EigenVectorsCollectionIteratorType;

  typedef itk::SymmetricEigenAnalysis<HessianOutputType::PixelType, EigenValuesType, EigenVectorsType> EigenAnalysisType;


  ItkEigenValuesFilter(typename ItkImageFilter::ImageConstPointer image) : ItkImageFilter(image),
    lambda1(0), lambda2(0), lambda3(0)
  {
  }

  typename ItkImageFilter::ImageConstPointer GetEigenValuesFilterImage()
  {
    double hessianSigma = 0;

    std::cout << "enter sigma value for the gaussian filter to compute hessian matrices" << std::endl;
    std::cin >> hessianSigma;

    std::cout << "enter linear combination coeficients for the eigenvectors components" << std::endl;

    std::cout << "lambda1 coeficient: ";
    std::cin >> lambda1;
    
    std::cout << "lambda2 coeficient: ";
    std::cin >> lambda2;

    std::cout << "lambda3 coeficient: ";
    std::cin >> lambda3;
    std::cout << std::endl;

    typedef itk::ImageDuplicator<ImageType> DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage(this->image);
    duplicator->Update();
    ImageType::Pointer outputImage = duplicator->GetModifiableOutput();

    HessianOutputType::ConstPointer hessianOutput = this->GetHessianRecursiveGaussianFilterImage(hessianSigma);
    this->LoadEigenVectors(hessianOutput);
    
    HessianIteratorType imageIterator(hessianOutput, hessianOutput->GetRequestedRegion());
    itk::ImageRegionIterator<ImageType> outputImageIterator(outputImage, outputImage->GetRequestedRegion());
    EigenValuesCollectionIteratorType eigenValuesIterator(this->eigenValuesPerVoxel, this->eigenValuesPerVoxel->GetRequestedRegion());
    EigenVectorsCollectionIteratorType eigenVectorsIterator(this->eigenVectorsPerVoxel, this->eigenVectorsPerVoxel->GetRequestedRegion());    
    for (imageIterator = imageIterator.Begin(); !imageIterator.IsAtEnd(); imageIterator++, ++eigenValuesIterator, ++eigenVectorsIterator, ++outputImageIterator)
    {
      EigenValuesType eigenValues = eigenValuesIterator.Get();
      outputImageIterator.Set(this->lambda1 * eigenValues[0] + this->lambda2 * eigenValues[1] + this->lambda3 * eigenValues[2]);
    }

    return outputImage;
  }

  virtual ~ItkEigenValuesFilter()
  {
  }

  virtual typename ImageConstPointer GetFilterImage() override
  {
    return this->GetEigenValuesFilterImage();
  }

  virtual std::string GetFilterName() override
  {
    return "EigenValuesOfHessianMatrixFilter";
  }

private:
  HessianOutputType::ConstPointer GetHessianRecursiveGaussianFilterImage(double sigma)
  {
    HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
    hessianFilter->SetInput(this->image.GetPointer());
    hessianFilter->SetSigma(sigma);
    hessianFilter->Update();    

    return hessianFilter->GetOutput();
  }

  EigenAnalysisType LoadEigenVectors(HessianOutputType::ConstPointer hessianOutput)
  {
    EigenAnalysisType eigenAnalysis;

    eigenValuesPerVoxel = EigenValuesCollectionType::New();
    eigenVectorsPerVoxel = EigenVectorsCollectionType::New();

    HessianIteratorType it(hessianOutput, hessianOutput->GetRequestedRegion());
    EigenValuesCollectionIteratorType eigenValuesIterator(this->eigenValuesPerVoxel, this->eigenValuesPerVoxel->GetRequestedRegion());
    EigenVectorsCollectionIteratorType eigenVectorsIterator(this->eigenVectorsPerVoxel, this->eigenVectorsPerVoxel->GetRequestedRegion());
    for (it = it.Begin(); !it.IsAtEnd(); ++it, ++eigenValuesIterator, ++eigenVectorsIterator)
    {
      eigenAnalysis.ComputeEigenValuesAndVectors(it.Value(), eigenValuesIterator.Get(), eigenVectorsIterator.Get());
    }

    return eigenAnalysis;
  }

  EigenValuesCollectionType::Pointer eigenValuesPerVoxel;
  EigenVectorsCollectionType::Pointer eigenVectorsPerVoxel;

  double lambda1;
  double lambda2;
  double lambda3;
};

#endif // FILTERS_ITK_EIGEN_VALUES_FILTER_H_