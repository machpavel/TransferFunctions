#ifndef FILTERS_ITK_EIGEN_VALUES_FILTER_H_
#define FILTERS_ITK_EIGEN_VALUES_FILTER_H_

#include "../ItkImageFilter.h"
#include "../Constants.h"
#include "../ImageDumpSerializer.h"

#include <itkImageDuplicator.h>

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkEigenValuesFilter :
  public ItkImageFilter<Constants::GlobalPixelType, Dimension>
{
public:
  typedef itk::SymmetricSecondRankTensor< double, Dimension > TensorType;
  typedef itk::Image<TensorType, Dimension> HessianOutputType;
  typedef typename HessianOutputType::ConstPointer HessianOutputTypeConstPointer;
  typedef itk::HessianRecursiveGaussianImageFilter<typename ItkImageFilter::ImageType, HessianOutputType> HessianFilterType;

  typedef itk::ImageRegionConstIterator<HessianOutputType>  HessianIteratorType;

  typedef itk::Vector<double, Dimension> EigenValuesType;
  typedef itk::Matrix<double, Dimension, Dimension> EigenVectorsType;
  typedef itk::Image<EigenValuesType, Dimension> EigenValuesCollectionType;
  //typedef itk::Image<EigenVectorsType, Dimension> EigenVectorsCollectionType;

  typedef itk::ImageRegionIterator<EigenValuesCollectionType> EigenValuesCollectionIteratorType;
  //typedef itk::ImageRegionIterator<EigenVectorsCollectionType> EigenVectorsCollectionIteratorType;

  typedef itk::ImportImageFilter<EigenValuesType, Dimension> EigenValuesImportFilterType;


  ItkEigenValuesFilter(typename ItkImageFilter::ImageConstPointer image, ImageDumpSerializer<>* serializer) : ItkImageFilter(image),
    lambda1(0), lambda2(0), lambda3(0), serializer(serializer)
  {
    this->InitializeEigenvalues();
  }

  typename ItkImageFilter::ImagePointer GetEigenValuesFilterImage()
  {
    double hessianSigma = 0;

    std::cout << "enter sigma value for the gaussian filter to compute hessian matrices" << std::endl;
    std::cin >> hessianSigma;
    std::cout << "computing hessian matrices" << std::endl;

    HessianOutputType::ConstPointer hessianOutput = this->GetHessianRecursiveGaussianFilterImage(hessianSigma);

    typedef itk::ImageDuplicator<ImageType> DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage(this->image);
    duplicator->Update();
    ImageType::Pointer outputImage = duplicator->GetModifiableOutput();

    std::cout << "computing eigenvalues at each point" << std::endl;
    this->LoadEigenVectors(hessianOutput);

    std::string read;

    do
    {
      std::cout << "enter linear combination coeficients for the eigenvectors components" << std::endl;

      std::cout << "lambda1 coeficient: ";
      std::cin >> lambda1;

      std::cout << "lambda2 coeficient: ";
      std::cin >> lambda2;

      std::cout << "lambda3 coeficient: ";
      std::cin >> lambda3;
      std::cout << std::endl;

      itk::ImageRegionIterator<ImageType> outputImageIterator(outputImage, outputImage->GetRequestedRegion());
      EigenValuesCollectionIteratorType eigenValuesIterator(this->eigenValuesPerVoxel, this->eigenValuesPerVoxel->GetRequestedRegion());
      eigenValuesIterator = eigenValuesIterator.Begin();
      for (outputImageIterator = outputImageIterator.Begin(); !outputImageIterator.IsAtEnd(); ++eigenValuesIterator, ++outputImageIterator)
      {
        EigenValuesType eigenValues = eigenValuesIterator.Get();
        outputImageIterator.Set(this->lambda1 * eigenValues[0] + this->lambda2 * eigenValues[1] + this->lambda3 * eigenValues[2]);
      }

      std::cout << "image computed, saving" << std::endl;

      this->serializer->SerializeImage(outputImage);
      this->serializer->CloseFile();

      std::cout << "continue with different coefficients? (yes/no)" << std:: endl;

      std::cin >> read;
    }
    while (read != "no");

    this->serializer->ReopenFile();

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
  HessianOutputTypeConstPointer GetHessianRecursiveGaussianFilterImage(double sigma)
  {
    HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
    hessianFilter->SetInput(this->image.GetPointer());
    hessianFilter->SetSigma(sigma);
    hessianFilter->Update();    

    HessianOutputType::ConstPointer output = hessianFilter->GetOutput();
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

    double origin[ Dimension ];
    origin[0] = 0.0;    // X coordinate
    origin[1] = 0.0;    // Y coordinate
    origin[2] = 0.0;    // Z coordinate
    importer->SetOrigin( origin );

    double spacing[ Dimension ];
    spacing[0] = 1.0;    // along X direction
    spacing[1] = 1.0;    // along Y direction
    spacing[2] = 1.0;    // along Z direction
    importer->SetSpacing( spacing );


    const unsigned int numberOfPixels =  size[0] * size[1] * size[2];
    EigenValuesType * pixelData = new EigenValuesType[numberOfPixels];

    const bool importFilterWillDeleteTheInputBuffer = false;
    importer->SetImportPointer(pixelData, numberOfPixels, importFilterWillDeleteTheInputBuffer);

    importer->Update();

    this->eigenValuesPerVoxel = importer->GetOutput();
  }

  void LoadEigenVectors(HessianOutputTypeConstPointer hessianOutput)
  {
    //eigenVectorsPerVoxel = EigenVectorsCollectionType::New();

    HessianIteratorType it(hessianOutput, hessianOutput->GetRequestedRegion());
    EigenValuesCollectionIteratorType eigenValuesIterator(this->eigenValuesPerVoxel, this->eigenValuesPerVoxel->GetRequestedRegion());
    //EigenVectorsCollectionIteratorType eigenVectorsIterator(this->eigenVectorsPerVoxel, this->eigenVectorsPerVoxel->GetRequestedRegion());
    //for (it = it.Begin(); !it.IsAtEnd(); ++it, ++eigenValuesIterator, ++eigenVectorsIterator)
    for (it = it.Begin(); !it.IsAtEnd(); ++it, ++eigenValuesIterator)
    {
      //eigenAnalysis.ComputeEigenValuesAndVectors(it.Value(), eigenValuesIterator.Get(), eigenVectorsIterator.Get());
      EigenValuesType eigenvalues;
      TensorType hessianMatrix = it.Get();
      hessianMatrix.ComputeEigenValues(eigenvalues);
      eigenValuesIterator.Set(eigenvalues);
    }
  }

  typename EigenValuesCollectionType::Pointer eigenValuesPerVoxel;
  //EigenVectorsCollectionType::Pointer eigenVectorsPerVoxel;

  double lambda1;
  double lambda2;
  double lambda3;

  ImageDumpSerializer<>* serializer;
};

#endif // FILTERS_ITK_EIGEN_VALUES_FILTER_H_