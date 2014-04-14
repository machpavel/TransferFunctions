#include <iostream>

#include "itkImage.h"
#include "itkImportImageFilter.h"

#include "itkImageFileWriter.h"

#include "ImageDumpDeserializer.h"
#include "ImageDumpSerializer.h"
#include "ItkImageFilter.h"

void PrintUsage()
{
  std::cout << "..:: Usage ::.." << std::endl;
  std::cout << "First parameter: input filename" << std::endl;
  std::cout << "Second parameter: output filename" << std::endl;
}

int main(int argc, char * argv[])
{
  if (argc != 2)
  {
    PrintUsage();
    return EXIT_SUCCESS;
  }

  typedef Constants::GlobalPixelType PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image<PixelType, Dimension> ImageType;
  typedef itk::ImportImageFilter< PixelType, Dimension >   ImportFilterType;


  ImageDumpDeserializer<> *deserializer = new ImageDumpDeserializer<>(std::string(argv[1]));
  ImageType::ConstPointer image = deserializer->DeserializeImage();

  ItkImageFilter<PixelType, Dimension> filter(image);

  ImageDumpSerializer<> *serializer = new ImageDumpSerializer<>(std::string(argv[2]));
  serializer->SetMinimums(deserializer->GetMinimums());
  serializer->SetMaximums(deserializer->GetMaximums());
  serializer->SetElementExtents(deserializer->GetElementExtents());
  serializer->SetDatasetType(deserializer->GetDatasetType());
  serializer->SetElementTypeID(deserializer->GetElementTypeID());

  serializer->SerializeImage(filter.GetGradientFilterImage());

  delete deserializer;
  delete serializer;

  return EXIT_SUCCESS;
}
