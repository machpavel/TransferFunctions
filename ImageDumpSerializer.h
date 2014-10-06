#ifndef IMAGE_DUMP_SERIALIZER_H_
#define IMAGE_DUMP_SERIALIZER_H_

#include "itkImportImageFilter.h"

#include "Constants.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "ImageDumpBase.h"


template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ImageDumpSerializer : public ImageDumpBase
{
public:
  typedef itk::Image<PixelType, Dimension> ImageType;
  typedef typename ImageType::ConstPointer ImageConstPointer;
  typedef typename ImageType::Pointer ImagePointer;
  typedef itk::ImportImageFilter<PixelType, Dimension> ImageFilterType;

  ImageDumpSerializer(const std::string & filename) : writer(FileWriter(filename))
  {
  }

  void SerializeImage(ImageConstPointer exporter)
  {
    this->SerializeHeader();


    // TODO: Rewrite as a single write operation

    size_t width = this->maximums[0] - this->minimums[0];
    size_t height = this->maximums[1] - this->minimums[1];
    size_t depth = this->maximums[2] - this->minimums[2];

    for (size_t z = 0; z < depth; ++z)
    {
      for (size_t y = 0; y < height; ++y)
      {
        for (size_t x = 0; x < width; ++x)
        {
          itk::Image<PixelType, Dimension>::IndexType index;
          index[0] = x;
          index[1] = y; 
          index[2] = z;

          this->writer.Write<PixelType>(exporter->GetPixel(index));
        }
      }
    }

    this->writer.Write<unsigned int>(Constants::DUMP_END_MAGIC_NUMBER);
  }

  void SerializeImage(ImagePointer exporter)
  {
    this->SerializeHeader();


    // TODO: Rewrite as a single write operation

    size_t width = this->maximums[0] - this->minimums[0];
    size_t height = this->maximums[1] - this->minimums[1];
    size_t depth = this->maximums[2] - this->minimums[2];

    for (size_t z = 0; z < depth; ++z)
    {
      for (size_t y = 0; y < height; ++y)
      {
        for (size_t x = 0; x < width; ++x)
        {
          itk::Image<PixelType, Dimension>::IndexType index;
          index[0] = x;
          index[1] = y; 
          index[2] = z;

          this->writer.Write<PixelType>(exporter->GetPixel(index));
        }
      }
    }

    this->writer.Write<unsigned int>(Constants::DUMP_END_MAGIC_NUMBER);
  }

  void CloseFile()
  {
    this->writer.CloseFile();
  }

  void ReopenFile()
  {
    this->writer.ReopenFile();
  }

private:
  void SerializeHeader()
  {
    this->writer.Write<unsigned int>(Constants::DUMP_START_MAGIC_NUMBER);

    this->writer.Write<unsigned int>(Constants::ACTUAL_FORMAT_VERSION);

    this->writer.Write<unsigned int>(this->datasetType);

    this->writer.Write<unsigned int>(Dimension);

    this->writer.Write<unsigned int>(elementTypeID);

    for (size_t i = 0; i < Dimension; ++i )
    {
      this->writer.Write<int>(this->minimums[i]);
      this->writer.Write<int>(this->maximums[i]);
      this->writer.Write<float>(this->elementExtents[i]);
    }

    this->writer.Write<unsigned int>(Constants::DUMP_HEADER_END_MAGIC_NUMBER);
  }

  //CustomItkImage<PixelType, Dimension> image;
  FileWriter writer;
};

#endif // IMAGE_DUMP_SERIALIZER_H_