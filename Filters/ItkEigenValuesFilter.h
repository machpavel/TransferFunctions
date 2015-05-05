#ifndef FILTERS_ITK_EIGEN_VALUES_FILTER_H_
#define FILTERS_ITK_EIGEN_VALUES_FILTER_H_

#include "../ItkImageFilter.h"
#include "../Constants.h"
#include "../ImageDumpSerializer.h"
#include "IEigenValuesFilterVisitor.h"

#include <itkImageDuplicator.h>

#include <boost/filesystem.hpp>
#include <itkRescaleIntensityImageFilter.h>

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkEigenValuesFilter :
  public ItkImageFilter<PixelType, Dimension>
{
public:
  typedef itk::SymmetricSecondRankTensor<float, Dimension > TensorType;
  typedef itk::Image<TensorType, Dimension> HessianOutputType;
  typedef typename HessianOutputType::Pointer HessianOutputTypePointer;
  typedef itk::HessianRecursiveGaussianImageFilter<typename ItkImageFilter::ImageType, HessianOutputType> HessianFilterType;

  typedef itk::ImageRegionConstIterator<HessianOutputType>  HessianIteratorType;

  typedef itk::Vector<float, Dimension> EigenValuesType;
  typedef itk::Image<EigenValuesType, Dimension> EigenValuesCollectionType;

  typedef itk::ImageRegionIterator<EigenValuesCollectionType> EigenValuesCollectionIteratorType;

  typedef itk::ImportImageFilter<EigenValuesType, Dimension> EigenValuesImportFilterType;

  typedef itk::RescaleIntensityImageFilter<ImageType, ImageType> RescaleFilterType;


  ItkEigenValuesFilter(typename ItkImageFilter::ImagePointer image, std::string secondaryFilename, ImageDumpSerializer<PixelType, Dimension>* serializer) : ItkImageFilter(image),
    secondaryFilename(secondaryFilename), serializerToCopy(serializer), serializeEigenvaluesAtTheEnd(false)
  {
    this->InitializeEigenvalues();
  }

  typename ItkImageFilter::ImagePointer GetEigenValuesFilterImage()
  {
    if (boost::filesystem::exists(boost::filesystem::path(this->secondaryFilename)))
    {
      std::cout << "found image containing eigenvalues" << std::endl;

      this->LoadEigenvaluesFromImage();
    }
    else
    {
      std::cout << "image containing eigenvalues not found, they will be computed" << std::endl;

      this->ComputeEigenvalues();

      this->serializeEigenvaluesAtTheEnd = true;
    }

    std::cout << "Performing computes on eigenvalues" << std::endl;

    this->visitor.Initialize();

    typedef itk::ImageDuplicator<ImageType> DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage(this->image);
    duplicator->Update();
    ImageType::Pointer outputImage = duplicator->GetModifiableOutput();

    itk::ImageRegionIterator<ImageType> outputImageIterator(outputImage, outputImage->GetRequestedRegion());
    EigenValuesCollectionIteratorType eigenValuesIterator(this->eigenValuesPerVoxel, this->eigenValuesPerVoxel->GetRequestedRegion());
    eigenValuesIterator.GoToBegin();
    for (outputImageIterator.GoToBegin(); !outputImageIterator.IsAtEnd(); ++eigenValuesIterator, ++outputImageIterator)
    {
      EigenValuesType eigenValues = eigenValuesIterator.Get();
      float computedValue = this->visitor.Visit(eigenValues[0], eigenValues[1], eigenValues[2]);
        outputImageIterator.Set(computedValue*1000);
    }

    if (this->serializeEigenvaluesAtTheEnd)
    {
      this->SerializeEigenvalues();
    }

    RescaleFilterType::Pointer rescale = RescaleFilterType::New();
    rescale->SetInput(outputImage);
    rescale->SetOutputMinimum(0);
    rescale->SetOutputMaximum(1000);
    rescale->Update();

    return rescale->GetOutput();
  }

  void SerializeEigenvalues()
  {
    EigenValuesCollectionIteratorType eigenValuesIterator(this->eigenValuesPerVoxel, this->eigenValuesPerVoxel->GetRequestedRegion());
    for (eigenValuesIterator.GoToBegin(); !eigenValuesIterator.IsAtEnd(); ++eigenValuesIterator)
    {
      EigenValuesType eigenValues = eigenValuesIterator.Get();
      //eigenValues[0] = this->EncodeEigenvalue(eigenValues[0]);
      //eigenValues[1] = this->EncodeEigenvalue(eigenValues[1]);
      //eigenValues[2] = this->EncodeEigenvalue(eigenValues[2]);
      eigenValuesIterator.Set(eigenValues);
    }

    ImageDumpSerializer<EigenValuesType, Dimension> serializer(this->secondaryFilename);

    serializer.SetMinimums(this->serializerToCopy->GetMinimums());
    serializer.SetMaximums(this->serializerToCopy->GetMaximums());
    serializer.SetElementExtents(this->serializerToCopy->GetElementExtents());

    // what is the dataset type?
    serializer.SetDatasetType(this->serializerToCopy->GetDatasetType());

    serializer.SetElementTypeID(Constants::ELEMENT_ID_VECTOR_3_FLOAT);

    std::cout << "saving computed eigenvalues" << std::endl;
    serializer.SerializeImage(this->eigenValuesPerVoxel);
    serializer.CloseFile();
  }

  virtual ~ItkEigenValuesFilter()
  {
  }

  void ComputeEigenvalues()
  {
    double hessianSigma = 0;

    std::cout << "enter sigma value for the gaussian filter to compute hessian matrices" << std::endl;
    std::cin >> hessianSigma;
    std::cout << "computing hessian matrices" << std::endl;

    HessianOutputType::Pointer hessianOutput = this->GetHessianRecursiveGaussianFilterImage(hessianSigma);

    std::cout << "computing eigenvalues at each point" << std::endl;
    this->ComputeEigenVectors(hessianOutput);

    std::cout << "eigenvalues computed, saving: " << this->secondaryFilename << std::endl;
  }

  void LoadEigenvaluesFromImage()
  {
    ImageDumpDeserializer<EigenValuesType, Dimension> deserializer(this->secondaryFilename);

    deserializer.DeserializeImage(this->eigenValuesPerVoxel);
  }

  virtual typename ImagePointer GetFilterImage() override
  {
    return this->GetEigenValuesFilterImage();
  }

  virtual std::string GetFilterName() override
  {
    return "EigenValuesOfHessianMatrixFilter";
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

  float EncodeEigenvalue(float eigenvalue)
  {
    return eigenvalue >= 0 ? (eigenvalue * Constants::RANGE_NORMALIZATION_CONSTANT) :
      (-eigenvalue * Constants::RANGE_NORMALIZATION_CONSTANT) + Constants::RANGE_NORMALIZATION_CONSTANT;
  }

  void ComputeEigenVectors(HessianOutputTypePointer hessianOutput)
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

  EigenValuesFrangiVesselnessVisitor<PixelType> visitor;

  ImageDumpSerializer<PixelType, Dimension>* serializerToCopy;

  std::string secondaryFilename;

  bool serializeEigenvaluesAtTheEnd;
};

#endif // FILTERS_ITK_EIGEN_VALUES_FILTER_H_