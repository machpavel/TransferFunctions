#ifndef IMAGE_DUMP_DESERIALIZER_H_
#define IMAGE_DUMP_DESERIALIZER_H_

#include "itkImportImageFilter.h"

#include "Constants.h"
#include "FileReader.h"
#include "ImageDumpBase.h"


template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ImageDumpDeserializer : public ImageDumpBase
{
public:
  typedef itk::Image<PixelType, Dimension> ImageType;
  typedef itk::ImportImageFilter<PixelType, Dimension> ImageFilterType;

  ImageDumpDeserializer(const std::string & filename) : reader(FileReader(filename))
  {
  }

  typename ImageType::Pointer DeserializeImage()
  {
    this->DeserializeHeader();

    ImageFilterType::Pointer importer = ImageFilterType::New();

    ImageType::SizeType size;
    size[0] = this->maximums[0] - this->minimums[0];
    size[1] = this->maximums[1] - this->minimums[1];
    size[2] = this->maximums[2] - this->minimums[2];

    ImageType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;

    ImageType::RegionType region;
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
    PixelType * pixelData = new PixelType[numberOfPixels];
    this->reader.Read<PixelType>(pixelData, numberOfPixels);

    const bool importFilterWillDeleteTheInputBuffer = false;
    importer->SetImportPointer(pixelData, numberOfPixels, importFilterWillDeleteTheInputBuffer);

    importer->Update();
    return importer->GetOutput();
  }

private:
  void DeserializeHeader()
  {
    unsigned int startMagic = reader.Read<unsigned int>();
    assert (startMagic == Constants::DUMP_START_MAGIC_NUMBER);

    unsigned int formatVersion = reader.Read<unsigned int>();
    assert( formatVersion == Constants::ACTUAL_FORMAT_VERSION );

    this->datasetType = reader.Read<unsigned int>();

    unsigned int dimension = reader.Read<unsigned int>();
    assert (dimension == Dimension);

    this->elementTypeID = reader.Read<unsigned int>();

    for (size_t i = 0; i < dimension; ++i )
    {
      this->minimums.push_back(reader.Read<int>());
      this->maximums.push_back(reader.Read<int>());
      this->elementExtents.push_back(reader.Read<float>());
    }

    unsigned int headerEndMagic = reader.Read<unsigned int>();
    assert (headerEndMagic == Constants::DUMP_HEADER_END_MAGIC_NUMBER);
  }

  //CustomItkImage<PixelType, Dimension> image;
  FileReader reader;
};

#endif // IMAGE_DUMP_DESERIALIZER_H_