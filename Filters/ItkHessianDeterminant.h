#ifndef FILTERS_ITK_HESSIAN_DETERMINANT_FILTER_H_
#define FILTERS_ITK_HESSIAN_DETERMINANT_FILTER_H_

#include "../ItkImageFilter.h"
#include "../Constants.h"
#include "../ImageDumpSerializer.h"

#include <vnl/algo/vnl_determinant.h>
#include <itkRescaleIntensityImageFilter.h>

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkHessianDeterminantFilter :
  public ItkImageFilter<PixelType, Dimension>
{
public:
  typedef itk::SymmetricSecondRankTensor<float, Dimension > TensorType;
  typedef itk::Image<TensorType, Dimension> HessianOutputType;
  typedef typename HessianOutputType::Pointer HessianOutputTypePointer;
  typedef itk::HessianRecursiveGaussianImageFilter<typename ItkImageFilter::ImageType, HessianOutputType> HessianFilterType;

  typedef itk::ImageRegionConstIterator<HessianOutputType> HessianIteratorType;
  typedef itk::Vector<float, Dimension> EigenValuesType;

  typedef itk::Image<float, Dimension> DeterminantsCollectionType;

  typedef itk::ImageRegionIterator<DeterminantsCollectionType> DeterminantsCollectionIteratorType;

  typedef itk::ImportImageFilter<float, Dimension> DeterminantsImportFilterType;

  typedef itk::RescaleIntensityImageFilter<DeterminantsCollectionType, ImageType> RescaleFilterType;


  ItkHessianDeterminantFilter(typename ItkImageFilter::ImagePointer image) : ItkImageFilter(image)
  {
  }

  virtual ~ItkHessianDeterminantFilter()
  {
  }

  virtual typename ImagePointer GetFilterImage() override
  {
    float hessianSigma = 0;
    std::cout << "Enter gaussian blur sigma: ";
    std::cin >> hessianSigma;

    HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
    hessianFilter->SetInput(this->image.GetPointer());
    hessianFilter->SetSigma(hessianSigma);
    hessianFilter->Update();

    HessianOutputType::Pointer hessians = hessianFilter->GetOutput();

    DeterminantsCollectionType::Pointer determinants = this->GetEmptyDeterminants();

    HessianIteratorType hessianIterator(hessians, hessians->GetRequestedRegion());
    DeterminantsCollectionIteratorType outputIterator(determinants, determinants->GetRequestedRegion());
    for (hessianIterator.GoToBegin(), outputIterator.GoToBegin(); !hessianIterator.IsAtEnd(); ++hessianIterator, ++outputIterator)
    {
      EigenValuesType eigenvalues;
      TensorType hessianMatrix = hessianIterator.Get();
      hessianMatrix.ComputeEigenValues(eigenvalues);

      float determinant = hessianMatrix.GetElement(0)*hessianMatrix.GetElement(1)*hessianMatrix.GetElement(2);
      outputIterator.Set(determinant);
    }


    RescaleFilterType::Pointer rescale = RescaleFilterType::New();
    rescale->SetInput(determinants);
    rescale->SetOutputMinimum(0);
    rescale->SetOutputMaximum(1000);
    rescale->Update();

    return rescale->GetOutput();
  }

  virtual std::string GetFilterName() override
  {
    return "Determinant of Hessian";
  }

private:
  typename DeterminantsCollectionType::Pointer GetEmptyDeterminants()
  {
    DeterminantsImportFilterType::Pointer importer = DeterminantsImportFilterType::New();

    ImageType::SizeType imageSize = this->image->GetLargestPossibleRegion().GetSize();
    DeterminantsImportFilterType::SizeType size;
    size[0] = imageSize[0];
    size[1] = imageSize[1];
    size[2] = imageSize[2];

    DeterminantsImportFilterType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;

    DeterminantsImportFilterType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);
    importer->SetRegion(region);

    double origin[Dimension];
    origin[0] = 0.0;    // X coordinate
    origin[1] = 0.0;    // Y coordinate
    origin[2] = 0.0;    // Z coordinate
    importer->SetOrigin(origin);

    double spacing[Dimension];
    spacing[0] = 1.0;    // along X direction
    spacing[1] = 1.0;    // along Y direction
    spacing[2] = 1.0;    // along Z direction
    importer->SetSpacing(spacing);


    /*const unsigned int numberOfPixels = size[0] * size[1] * size[2];
    EigenValuesType * pixelData = new EigenValuesType[numberOfPixels];

    const bool importFilterWillDeleteTheInputBuffer = false;
    importer->SetImportPointer(pixelData, numberOfPixels, importFilterWillDeleteTheInputBuffer);*/

    importer->Update();

    return importer->GetOutput();
  }
};

#endif // FILTERS_ITK_HESSIAN_DETERMINANT_FILTER_H_