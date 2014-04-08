#include "itkImage.h"
#include "itkImportImageFilter.h"

#include "itkImageFileWriter.h"

#include "ImageDumpDeserializer.h"
#include "ImageDumpSerializer.h"

int main(int argc, char * argv[])
{
  typedef Constants::GlobalPixelType PixelType;
  const unsigned int Dimension = 3;
  typedef itk::ImportImageFilter< PixelType, Dimension >   ImportFilterType;


  ImageDumpDeserializer<> *deserializer = new ImageDumpDeserializer<>(std::string(argv[1]));
  CustomItkImage<>::Pointer image = deserializer->DeserializeImage();

  
  ImageDumpSerializer<> *serializer = new ImageDumpSerializer<>(std::string(argv[2]));
  serializer->SetMinimums(deserializer->GetMinimums());
  serializer->SetMaximums(deserializer->GetMaximums());
  serializer->SetElementExtents(deserializer->GetElementExtents());
  serializer->SetDatasetType(deserializer->GetDatasetType());
  serializer->SetElementTypeID(deserializer->GetElementTypeID());

  serializer->SerializeImage(image.GetPointer());

  delete deserializer;
  delete serializer;

  return EXIT_SUCCESS;
}
