#ifndef DICOM_IMPORT_H_
#define DICOM_IMPORT_H_


#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "Constants.h"
#include "ImageDumpSerializer.h"

template<typename PixelType = Constants::GlobalPixelType, size_t Dimension = 3>
class DicomImport
{
public:

  typedef itk::Image<PixelType, Dimension> ImageType;
  typedef typename ImageType::Pointer ImagePointerType;
  typedef itk::ImageSeriesReader<ImageType>ReaderType;
  typedef itk::GDCMImageIO ImageIOType;
  typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;


  DicomImport(const std::string& inputDirectory, const std::string& outputFilename)
    : inputDirectory(inputDirectory), outputFilename(outputFilename)
  {
  }

  ~DicomImport()
  {
  }



  void DicomImport::Convert()
  {
    ImageIOType::Pointer gdcmIO = ImageIOType::New();
    InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
    inputNames->SetInputDirectory(this->inputDirectory);

    const ReaderType::FileNamesContainer& filenames = inputNames->GetInputFileNames();

    ReaderType::Pointer reader = ReaderType::New();

    reader->SetImageIO(gdcmIO);
    reader->SetFileNames(filenames);

    try
    {
      reader->Update();
    }
    catch (itk::ExceptionObject &excp)
    {
      std::cerr << "Exception thrown while reading the series" << std::endl;
      std::cerr << excp << std::endl;
    }

    ImagePointerType image = reader->GetOutput();

    ImageDumpSerializer<PixelType, Dimension> serializer(this->outputFilename);
    serializer.SetDatasetType(Constants::DATASET_TYPE);
    serializer.SetElementTypeID(Constants::ELEMENT_ID_UNSIGNED_INT);
    serializer.SerializeImage(image);
  }

private:
  std::string inputDirectory;
  std::string outputFilename;
};

#endif // DICOM_IMPORT_H_