#ifndef FILTERS_ITK_EIGEN_VALUES_RAW_H_
#define FILTERS_ITK_EIGEN_VALUES_RAW_H_

#include "../ItkImageFilter.h"
#include "../Constants.h"
#include "../ImageDumpSerializer.h"

template<typename PixelType = unsigned int, unsigned int Dimension = 3>
class ItkEigenValuesRaw :
  public ItkImageFilter<Constants::GlobalPixelType, Dimension>
{
public:
  typedef itk::SymmetricSecondRankTensor< float, Dimension > TensorType;
  typedef itk::Image<TensorType, Dimension> HessianOutputType;
  typedef typename HessianOutputType::Pointer HessianOutputTypePointer;
  typedef itk::HessianRecursiveGaussianImageFilter<typename ItkImageFilter::ImageType, HessianOutputType> HessianFilterType;

  typedef itk::ImageRegionConstIterator<HessianOutputType>  HessianIteratorType;

  typedef itk::Vector<float, Dimension> EigenValuesType;
  typedef itk::Image<EigenValuesType, Dimension> EigenValuesCollectionType;

  typedef itk::ImageRegionIterator<EigenValuesCollectionType> EigenValuesCollectionIteratorType;

  typedef itk::ImportImageFilter<EigenValuesType, Dimension> EigenValuesImportFilterType;


  ItkEigenValuesRaw(typename ItkImageFilter::ImagePointer image, std::string secondaryFilename, ImageDumpSerializer<>* serializer) : ItkImageFilter(image), serializer(secondaryFilename)
  {
    this->InitializeEigenvalues();

    this->serializer.SetMinimums(serializer->GetMinimums());
    this->serializer.SetMaximums(serializer->GetMaximums());
    this->serializer.SetElementExtents(serializer->GetElementExtents());

    // what is the dataset type?
    this->serializer.SetDatasetType(serializer->GetDatasetType());

    this->serializer.SetElementTypeID(Constants::ELEMENT_ID_VECTOR_3_FLOAT);
  }

  typename ItkEigenValuesRaw::ImagePointer GetEigenValuesFilterImage()
  {
    double hessianSigma = 0;

    std::cout << "enter sigma value for the gaussian filter to compute hessian matrices" << std::endl;
    std::cin >> hessianSigma;
    std::cout << "computing hessian matrices" << std::endl;

    HessianOutputType::Pointer hessianOutput = this->GetHessianRecursiveGaussianFilterImage(hessianSigma);

    std::cout << "computing eigenvalues at each point" << std::endl;
    this->LoadEigenVectors(hessianOutput);

    this->serializer.SerializeImage(this->eigenValuesPerVoxel);
    this->serializer.CloseFile();

    return this->image;
  }

  virtual ~ItkEigenValuesRaw()
  {
  }

  virtual typename ImagePointer GetFilterImage() override
  {
    return this->GetEigenValuesFilterImage();
  }

  virtual std::string GetFilterName() override
  {
    return "EigenValuesOfHessianMatrixRaw";
  }

private:
  HessianOutputTypePointer GetHessianRecursiveGaussianFilterImage(double sigma)
  {
    HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
    hessianFilter->SetInput(this->image.GetPointer());
    hessianFilter->SetSigma(sigma);
    hessianFilter->Update();

    HessianOutputType::Pointer output = hessianFilter->GetOutput();
    return output;
  }

  void InitializeEigenvalues()
  {
    EigenValuesImportFilterType::Pointer importer = EigenValuesImportFilterType::New();

    ImageType::SizeType imageSize = this->image->GetLargestPossibleRegion().GetSize();
    EigenValuesImportFilterType::SizeType size;
    size[0] = imageSize[0];
    size[1] = imageSize[1];
    size[2] = imageSize[2];

    EigenValuesCollectionType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;

    EigenValuesCollectionType::RegionType region;
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


    const unsigned int numberOfPixels = size[0] * size[1] * size[2];
    EigenValuesType * pixelData = new EigenValuesType[numberOfPixels];

    const bool importFilterWillDeleteTheInputBuffer = false;
    importer->SetImportPointer(pixelData, numberOfPixels, importFilterWillDeleteTheInputBuffer);

    importer->Update();

    this->eigenValuesPerVoxel = importer->GetOutput();
  }

  void LoadEigenVectors(HessianOutputTypePointer hessianOutput)
  {
    HessianIteratorType it(hessianOutput, hessianOutput->GetRequestedRegion());
    EigenValuesCollectionIteratorType eigenValuesIterator(this->eigenValuesPerVoxel, this->eigenValuesPerVoxel->GetRequestedRegion());
    for (it.GoToBegin(); !it.IsAtEnd(); ++it, ++eigenValuesIterator)
    {
      EigenValuesType eigenvalues;
      TensorType hessianMatrix = it.Get();
      hessianMatrix.ComputeEigenValues(eigenvalues);

      eigenValuesIterator.Set(eigenvalues);
    }
  }

  typename EigenValuesCollectionType::Pointer eigenValuesPerVoxel;

  ImageDumpSerializer<EigenValuesType, Dimension> serializer;
};

#endif // FILTERS_ITK_EIGEN_VALUES_RAW_H_